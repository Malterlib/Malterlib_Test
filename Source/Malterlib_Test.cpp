// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>
#include <Mib/Test/Exception>
#include <Mib/CommandLine/CommandLine>
#include <Mib/CommandLine/CommandLineClient>
#include <Mib/Core/RuntimeType>
#include <Mib/Encoding/JSONShortcuts>

#include "Malterlib_Test_Reporter_Null.h"
#include "Malterlib_Test_Reporter_Text.h"
#include "Malterlib_Test_Reporter_Registry.h"
#include "Malterlib_Test_Reporter_Category.h"

namespace NMib::NTest
{
	namespace NPrivate
	{
#if DMibConfig_Tests_Enable
		struct CTestPathRestoringScope;

		class CTestManager
		{
		public:

			NThread::CMutual m_Lock;

			CTestResults *m_pResults;

			ETestReportFlag m_ReportFlags;

			NMib::NStr::CStr m_ExtraData;

			NContainer::TCVector<NStr::CStr> m_IncludePatterns;
			NContainer::TCVector<NStr::CStr> m_ExcludePatterns;
			NContainer::TCSet<NStr::CStr> m_IncludeGroups;
			NContainer::TCSet<NStr::CStr> m_ExcludeGroups;
			NAtomic::TCAtomic<smint> m_nTotalTests;
			NAtomic::TCAtomic<smint> m_nWarnings;
			NAtomic::TCAtomic<smint> m_nFailed;
			NAtomic::TCAtomic<smint> m_nSuccess;
			NAtomic::TCAtomic<smint> m_nSuccessUnexpected;
			NAtomic::TCAtomic<smint> m_nExpectedFailed;
			NAtomic::TCAtomic<smint> m_nIgnored;

			NAtomic::TCAtomic<uint32> m_ReturnValue;

			NThread::CMutual m_ThreadLocalLock;

			static CTestManager &fs_GetManager();

			class CUniqueTest
			{
			public:
				const ch8 *m_pFile;
				int32 m_Line;
				COrdering_Weak operator <=> (CUniqueTest const &_Other) const
				{
					if (auto Ret = NStr::fg_StrCmp(m_pFile, _Other.m_pFile); Ret != 0)
						return Ret <=> 0;

					return m_Line <=> _Other.m_Line;
				}
			};

			struct CCoroutineState
			{
				NStr::CStr m_Path;
				CTestPathRestoringScope *m_pCurrentScope = nullptr;
			};

			class CThreadLocal
			{
			public:
				CThreadLocal() = default;
				CThreadLocal(CThreadLocal const &_Inherit)
				{
					{
						DMibLock(_Inherit.m_PropertiesLock);
						m_bEnableValues = _Inherit.m_bEnableValues;
						m_bEnableExceptionFilter = _Inherit.m_bEnableExceptionFilter;
						m_bInsideTestSuite = _Inherit.m_bInsideTestSuite;
						m_pLastTestFile = _Inherit.m_pLastTestFile;
						m_LastTestLine = _Inherit.m_LastTestLine;
					}
					{
						DMibLock(_Inherit.m_TestPathLock);
						m_TestPath = _Inherit.m_TestPath;
					}

					{
						DMibLock(fs_GetManager().m_ThreadLocalLock);
						m_pInherit = &_Inherit;
					}
				}

				~CThreadLocal()
				{
					NContainer::TCMap<NStr::CStr, NContainer::TCMap<CUniqueTest, TCAutoClearInt<bool>>> UniqueTests;
					{
						DMibLock(m_UniqueTestsLock);
						UniqueTests = m_UniqueTests;
					}
					DMibLock(fs_GetManager().m_ThreadLocalLock);
					{
						if (m_pInherit)
						{
							{
								DMibLock(m_pInherit->m_UniqueTestsLock);
								for (auto Iter = UniqueTests.f_GetIterator(); Iter; ++Iter)
								{
									NContainer::TCMap<CUniqueTest, TCAutoClearInt<bool>> const &Aggregated = *Iter;
									bool bCreated = false;
									NContainer::TCMap<CUniqueTest, TCAutoClearInt<bool>> &AggregatedNew = m_pInherit->m_UniqueTests.f_Map(Iter.f_GetKey(), bCreated);
									AggregatedNew += Aggregated;
									if (!bCreated && Aggregated.f_IsEmpty())
									{
										DMibTrace("Duplicate Test Path: {}" DMibNewLine, Iter.f_GetKey());
										DMibPDebugBreak; // The same test run twice is not allowed for reporting reasons
									}
								}
							}
						}
						m_AutoClearPtrMember.f_Clear();
					}
				}

				NStorage::TCAutoClearPtr<CThreadLocal const> m_pInherit;
				mutable NThread::CMutual m_TestPathLock;
				NStr::CStr m_TestPath;
				mutable NThread::CMutual m_PropertiesLock;
				NContainer::TCMap<NStr::CStr> m_TestGroups;
				CTestResults *m_pResults = nullptr;
				const ch8 *m_pLastTestFile = nullptr;
				int32 m_LastTestLine = 0;

				NStr::CStr m_DynamicValue;

				CCoroutineState m_CoroutineState;

				bool m_bEnableValues = true;
				bool m_bEnableExceptionFilter = true;
				bool m_bEnumerating = false;
				bool m_bInsideTestSuite = false;
				mutable NThread::CMutual m_UniqueTestsLock;
				mutable NContainer::TCMap<NStr::CStr, NContainer::TCMap<CUniqueTest, TCAutoClearInt<bool>>> m_UniqueTests;

				DMibAutoClearPtrDeclare;
			};

			NThread::TCThreadLocal<CThreadLocal, NMemory::CAllocator_Heap, NThread::EThreadLocalFlag_Inherit> m_ThreadLocal;

			CTestResults *f_GetResults(CThreadLocal const &_ThreadLocal) const
			{
				if (_ThreadLocal.m_pResults)
					return _ThreadLocal.m_pResults;
				return m_pResults;
			}

			void f_ClearStatistics()
			{
				m_nTotalTests.f_Exchange(0);
				m_nWarnings.f_Exchange(0);
				m_nFailed.f_Exchange(0);
				m_nSuccess.f_Exchange(0);
				m_nSuccessUnexpected.f_Exchange(0);
				m_nExpectedFailed.f_Exchange(0);
				m_nIgnored.f_Exchange(0);
			}

		};

		constinit static NStorage::TCAggregate<CTestManager, 256> g_Tests = {DAggregateInit};

		CTestManager &CTestManager::fs_GetManager()
		{
			return *g_Tests;
		}

		struct CTestPathRestoringScope final : public CCrossActorCallStateScope
		{
			CTestPathRestoringScope()
				: CCrossActorCallStateScope(false)
			{
				auto &ThreadLocal = *g_Tests->m_ThreadLocal;
				auto &CoroutineState = ThreadLocal.m_CoroutineState;
				m_pOldScope = CoroutineState.m_pCurrentScope;
				CoroutineState.m_pCurrentScope = this;
			}

			~CTestPathRestoringScope()
			{
				auto &ThreadLocal = *g_Tests->m_ThreadLocal;
				auto &CoroutineState = ThreadLocal.m_CoroutineState;
				CoroutineState.m_pCurrentScope = m_pOldScope;
			}

			void f_InitialSuspend() override
			{
				auto &ThreadLocal = *g_Tests->m_ThreadLocal;
				auto &CoroutineState = ThreadLocal.m_CoroutineState;
				if (CoroutineState.m_pCurrentScope != this)
					return;

				DMibLock(ThreadLocal.m_TestPathLock);
				CoroutineState.m_Path = ThreadLocal.m_TestPath;
			}

			NFunction::TCFunctionMovable<void (bool _bException)> f_StoreState(bool _bFromSuspend) override
			{
				auto &ThreadLocal = *g_Tests->m_ThreadLocal;
				auto &CoroutineState = ThreadLocal.m_CoroutineState;
				if (CoroutineState.m_pCurrentScope != this)
					return {};

				DMibLock(ThreadLocal.m_TestPathLock);
				const ch8 *pFile = ThreadLocal.m_pLastTestFile;
				int32 Line = ThreadLocal.m_LastTestLine;
				auto Path = ThreadLocal.m_TestPath;

				if (_bFromSuspend)
				{
					DMibLock(ThreadLocal.m_TestPathLock);
					ThreadLocal.m_TestPath = CoroutineState.m_Path;
				}

				return [Path, pFile, Line, pForwardingScope = NStorage::TCSharedPointer<CTestPathRestoringScope>{}](bool _bException) mutable
					{
						auto &ThreadLocal = *g_Tests->m_ThreadLocal;
						auto &CoroutineState = ThreadLocal.m_CoroutineState;

						if (!_bException)
						{
							CoroutineState.m_Path = Path;
							{
								DMibLock(ThreadLocal.m_TestPathLock);
								ThreadLocal.m_TestPath = Path;
							}
							{
								DMibLock(ThreadLocal.m_PropertiesLock);
								ThreadLocal.m_pLastTestFile = pFile;
								ThreadLocal.m_LastTestLine = Line;
							}
						}

						if (!CoroutineState.m_pCurrentScope)
							pForwardingScope = fg_Construct();
					}
				;
			}

			CTestPathRestoringScope *m_pOldScope = nullptr;
		};

		CTestCategoryScope::CTestCategoryScope(const CTestCategory &_Category, const ch8 *_pFile, int32 _Line, ETestCategoryFlag _Flags)
			: mp_Category(_Category)
			, mp_pFile(_pFile)
			, mp_Line(_Line)
			, mp_Flags(_Flags)
		{
			fg_SetTestLastLocation(_pFile, _Line);

			DMibFastCheck(_Category.f_GetCategory().f_FindChar('/') < 0 && _Category.f_GetCategory().f_FindChar('\\') < 0);
			DMibFastCheck(!NMib::NTest::NPrivate::fg_InsideTestSuite());
			NMib::NTest::NPrivate::fg_InsideTestSuite(mp_Flags & ETestCategoryFlag_Tests);
			if (_Flags & ETestCategoryFlag_DisableValues)
				mp_bOldEnableValues = NMib::NTest::NPrivate::fg_SetEnableValues(false);
			else if (_Flags & ETestCategoryFlag_EnableValues)
				mp_bOldEnableValues = NMib::NTest::NPrivate::fg_SetEnableValues(true);

			if (_Flags & ETestCategoryFlag_DisableExceptionFilter)
				mp_bOldEnableExceptionFilter = NMib::NTest::NPrivate::fg_SetEnableExceptionFilter(false);
			else if (_Flags & ETestCategoryFlag_EnableExceptionFilter)
				mp_bOldEnableExceptionFilter = NMib::NTest::NPrivate::fg_SetEnableExceptionFilter(true);

		}

		CTestCategoryScope::~CTestCategoryScope()
		{
			if (mp_Flags & (ETestCategoryFlag_DisableValues | ETestCategoryFlag_EnableValues))
				NMib::NTest::NPrivate::fg_SetEnableValues(mp_bOldEnableValues);
			if (mp_Flags & (ETestCategoryFlag_DisableExceptionFilter | ETestCategoryFlag_EnableExceptionFilter))
				NMib::NTest::NPrivate::fg_SetEnableExceptionFilter(mp_bOldEnableExceptionFilter);
			NMib::NTest::NPrivate::fg_InsideTestSuite(false);
		}

		void CTestCategoryScope::f_ProcessAsyncCategory(NFunction::TCFunctionMovable<NConcurrency::TCFuture<void> ()> &&_Function)
		{
			f_ProcessCategory
				(
					[&]
					{
						NStorage::TCSharedPointer<NConcurrency::CDefaultRunLoop> pRunLoop = fg_Construct();
						auto CleanupRunLoop = g_OnScopeExit > [&]
							{
								while (pRunLoop->f_RefCountGet() > 0)
									pRunLoop->f_WaitOnceTimeout(0.1);
							}
						;

						NConcurrency::TCActor<NConcurrency::CDispatchingActor> HelperActor(fg_Construct(), pRunLoop->f_Dispatcher());
						auto CleanupHelperActor = g_OnScopeExit > [&]
							{
								HelperActor->f_BlockDestroy(pRunLoop->f_ActorDestroyLoop());
							}
						;

						CTestPathRestoringScope RestorePathScope;

						auto &ThreadLocal = **g_SystemThreadLocal;
						auto OldFlags = ThreadLocal.m_ExtraCoroutineFlags;
						ThreadLocal.m_ExtraCoroutineFlags |= NConcurrency::ECoroutineFlag_CaptureExceptions;

						auto CleanupFlags = g_OnScopeExit > [&]
							{
								ThreadLocal.m_ExtraCoroutineFlags = OldFlags;
							}
						;

						(NConcurrency::g_Dispatch(HelperActor) / fg_Move(_Function)).f_CallSync(pRunLoop);
					}
				)
			;
		}

		NStr::CStr const &fg_GetDynamicValue()
		{
			CTestManager *pTestManager = g_Tests;
			return pTestManager->m_ThreadLocal->m_DynamicValue;
		}

		void fg_SetDynamicValue(NStr::CStr &&_Value)
		{
			CTestManager *pTestManager = g_Tests;
			pTestManager->m_ThreadLocal->m_DynamicValue = fg_Move(_Value);
		}

		bool fg_SetEnableValues(bool _bEnableValues)
		{
			CTestManager *pTestManager = g_Tests;
			DMibLock(pTestManager->m_ThreadLocal->m_PropertiesLock);
			bool bReturn = pTestManager->m_ThreadLocal->m_bEnableValues;
			pTestManager->m_ThreadLocal->m_bEnableValues = _bEnableValues;
			return bReturn;
		}

		bool fg_GetEnableValues()
		{
			CTestManager *pTestManager = g_Tests;
			return pTestManager->m_ThreadLocal->m_bEnableValues;
		}

		bool fg_SetEnableExceptionFilter(bool _bEnableExceptionFilter)
		{
			CTestManager *pTestManager = g_Tests;
			DMibLock(pTestManager->m_ThreadLocal->m_PropertiesLock);
			bool bReturn = pTestManager->m_ThreadLocal->m_bEnableExceptionFilter;
			pTestManager->m_ThreadLocal->m_bEnableExceptionFilter = _bEnableExceptionFilter;
			return bReturn;
		}

		bool fg_GetEnableExceptionFilter()
		{
			CTestManager *pTestManager = g_Tests;
			return pTestManager->m_ThreadLocal->m_bEnableExceptionFilter;
		}

		bool fg_InsideTestSuite()
		{
			CTestManager *pTestManager = g_Tests;
			return pTestManager->m_ThreadLocal->m_bInsideTestSuite;
		}
		void fg_InsideTestSuite(bool _bInsideTestSuite)
		{
			CTestManager *pTestManager = g_Tests;
			DMibLock(pTestManager->m_ThreadLocal->m_PropertiesLock);
			pTestManager->m_ThreadLocal->m_bInsideTestSuite = _bInsideTestSuite;
		}
		NStr::CStr fg_PushCategory(const NStr::CStr &_Category)
		{
			CTestManager *pTestManager = g_Tests;
			auto &ThreadLocal = *pTestManager->m_ThreadLocal;
			DMibLock(ThreadLocal.m_TestPathLock);
			NStr::CStr PreviousPath = ThreadLocal.m_TestPath;
			NStr::fg_StrAddWithSeparator(ThreadLocal.m_TestPath, _Category, "/");
			return PreviousPath;
		}

		void fg_SetTestLastLocation(const ch8 *_pFile, int32 _Line)
		{
			CTestManager *pTestManager = g_Tests;
			auto &ThreadLocal = *pTestManager->m_ThreadLocal;
			DMibLock(pTestManager->m_ThreadLocal->m_PropertiesLock);
			ThreadLocal.m_pLastTestFile = _pFile;
			ThreadLocal.m_LastTestLine = _Line;
		}

		void fg_GetTestLastLocation(const ch8 *&o_pFile, int32 &o_Line)
		{
			CTestManager *pTestManager = g_Tests;
			auto &ThreadLocal = *pTestManager->m_ThreadLocal;
			o_pFile = ThreadLocal.m_pLastTestFile;
			o_Line = ThreadLocal.m_LastTestLine;
		}

		void fg_PopCategory(NStr::CStr const &PreviousPath)
		{
			CTestManager *pTestManager = g_Tests;
			CTestManager::CThreadLocal &ThreadLocal = *pTestManager->m_ThreadLocal;
			DMibLock(ThreadLocal.m_TestPathLock);
			ThreadLocal.m_TestPath = PreviousPath;
		}

		NContainer::TCMap<NStr::CStr> fg_SetGroups(const NContainer::TCMap<NStr::CStr> &_Groups)
		{
			CTestManager *pTestManager = g_Tests;
			NContainer::TCMap<NStr::CStr> Ret = fg_Move(pTestManager->m_ThreadLocal->m_TestGroups);
			pTestManager->m_ThreadLocal->m_TestGroups = _Groups;

			return Ret;
		}

		NContainer::TCMap<NStr::CStr> fg_GetGroups()
		{
			CTestManager *pTestManager = g_Tests;
			NContainer::TCMap<NStr::CStr> Ret = pTestManager->m_ThreadLocal->m_TestGroups;

			return Ret;
		}


		inline_small void fg_NeedReportImpl
			(
				CTestManager *_pTestManager
				, ETestResult _Result
				, ETest _FailureAction
				, ECheckType _CheckType
				, const ch8 *_pFile
				, int32 _Line
				, ETestFlag _Flags
			)
		{
			_pTestManager->m_nTotalTests.f_FetchAdd(1);
			if (_Result == ETestResult_Fail)
			{
				switch (_FailureAction)
				{
				case ETest_Warn:
					_pTestManager->m_nWarnings.f_FetchAdd(1);
					break;
				case ETest_Fail:
					_pTestManager->m_nFailed.f_FetchAdd(1);
					break;
				case ETest_FailAndStop:
					_pTestManager->m_nFailed.f_FetchAdd(1);
					break;
				case ETest_ExpectFail:
					_pTestManager->m_nExpectedFailed.f_FetchAdd(1);
					break;
				case ETest_ExpectFailAndStop:
					_pTestManager->m_nExpectedFailed.f_FetchAdd(1);
					break;
				case ETest_None:
					break;
				}
			}
			else if (_Result == ETestResult_Success)
			{
				if (_FailureAction == ETest_ExpectFail || _FailureAction == ETest_ExpectFailAndStop)
					_pTestManager->m_nSuccessUnexpected.f_FetchAdd(1);
				else
					_pTestManager->m_nSuccess.f_FetchAdd(1);
			}
			else
				_pTestManager->m_nIgnored.f_FetchAdd(1);
		}

		ETestResultReportFlag fg_NeedReport(NStr::CStr const &_Description, ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, const ch8 *_pFile, int32 _Line, ETestFlag _Flags)
		{
			CTestManager *pTestManager = g_Tests;
			auto &ThreadLocal = *pTestManager->m_ThreadLocal;

			CTestResults *pResults = pTestManager->f_GetResults(ThreadLocal);

			ETestNeedReportFlag NeedReportFlags = pResults->f_NeedReport(_Result, _FailureAction, _CheckType, _Flags);
			ETestResultReportFlag Ret = ETestResultReportFlag_None;
			if (NeedReportFlags & ETestNeedReportFlag_Abort)
				Ret |= ETestResultReportFlag_Abort;

			if (_Result == ETestResult_Fail)
			{
				switch (_FailureAction)
				{
				case ETest_FailAndStop:
				case ETest_ExpectFailAndStop:
					Ret |= ETestResultReportFlag_Abort;
					break;
				case ETest_None:
				case ETest_Warn:
				case ETest_Fail:
				case ETest_ExpectFail:
					break;
				}
			}
			if (!(NeedReportFlags & ETestNeedReportFlag_AllowDuplicates))
			{
				NStr::CStr FullPath = ThreadLocal.m_TestPath + "/" +_Description;
				{
					DMibLock(ThreadLocal.m_UniqueTestsLock);
					bool bCreated = false;
					NContainer::TCMap<CTestManager::CUniqueTest, TCAutoClearInt<bool>> &AggregatedTests = ThreadLocal.m_UniqueTests.f_Map(FullPath, bCreated);
					bool bAllowNonUnique = false;
					if (_Flags & ETestFlag_Aggregated)
					{
						bool bCreated = false;
						CTestManager::CUniqueTest Key;
						Key.m_pFile = _pFile;
						Key.m_Line = _Line;

						TCAutoClearInt<bool> &bValue = AggregatedTests.f_Map(Key, bCreated);
						if (bCreated)
						{
							bValue = _Result != ETestResult_Fail;
						}
						else
						{
							if (_Result == ETestResult_Fail && bValue)
							{
								bAllowNonUnique = true;
								bValue = false;
							}
							else
								return Ret;
						}
					}
					if (!bCreated && !bAllowNonUnique)
					{
						DMibTrace(DMibPFileLineFormat " Duplicate Test Path: {}" DMibNewLine, _pFile << _Line << FullPath);
						DMibPDebugBreak; // The same test run twice is not allowed for reporting reasons
					}
				}
			}
			if (NeedReportFlags & ETestNeedReportFlag_Count)
				fg_NeedReportImpl(pTestManager, _Result, _FailureAction, _CheckType, _pFile, _Line, _Flags);

			if (NeedReportFlags & ETestNeedReportFlag_Report)
				Ret |= ETestResultReportFlag_Report;

			return Ret;
		}

		CTestResults *fg_SetResultReporter(CTestResults *_pNewReporter)
		{
			CTestManager *pTestManager = g_Tests;
			CTestManager::CThreadLocal &ThreadLocal = *pTestManager->m_ThreadLocal;
			CTestResults *pOld = ThreadLocal.m_pResults;
			ThreadLocal.m_pResults = _pNewReporter;
			return pOld;
		}

		CTestResults &fg_GetResultReporter()
		{
			CTestManager *pTestManager = g_Tests;
			return *pTestManager->f_GetResults(*pTestManager->m_ThreadLocal);
		}

		void fg_ReportTestResult
			(
				ETestResult _Result
				, const NStr::CStr &_Description
				, const NStr::CStr &_Values
				, ETest _FailureAction
				, ECheckType _CheckType
				, const NStr::CStr &_File
				, int32 _Line
				, const NStr::CStr &_ExtraMultiLineReportData
				, ETestFlag _Flags
				, ETestResultReportFlag _AlwaysReport
			)
		{
			if
				(
					_AlwaysReport & ETestResultReportFlag_Report
					||
					(
						_AlwaysReport & ETestResultReportFlag_AskReport
						&& ((_AlwaysReport = fg_NeedReport(_Description, _Result, _FailureAction, _CheckType, _File, _Line, _Flags)) & ETestResultReportFlag_Report) != 0
					)
				)
			{
				CTestManager *pTestManager = g_Tests;
				auto &ThreadLocal = *pTestManager->m_ThreadLocal;

				pTestManager->f_GetResults(ThreadLocal)->f_ReportResult
					(
						_Result
						, ThreadLocal.m_TestPath
						, _Description
						, _Values
						, CTestLocation(_File, _Line)
						, _FailureAction
						, _CheckType
						, _Flags
						, _ExtraMultiLineReportData
					)
				;
			}
			if (_AlwaysReport & ETestResultReportFlag_Abort)
				throw CReportTestAbortException();
		}

		bool CTestCategoryScope::f_ContinueEnumerating() const
		{
			auto& Manager = CTestManager::fs_GetManager();
			return !Manager.m_ThreadLocal->m_bEnumerating
				|| !(mp_Flags & ETestCategoryFlag_Tests);
		}
	
		void CTestCategoryScope::f_ReportLeafCategory()
		{
			auto& Manager = CTestManager::fs_GetManager();
			Manager.f_GetResults(*Manager.m_ThreadLocal)->f_ReportSuite(Manager.m_ThreadLocal->m_TestPath, Manager.m_ThreadLocal->m_TestGroups, CTestLocation(mp_pFile, mp_Line));
		}

		NStr::EMatchWildcardResult fg_MatchPattern(const NStr::CStr &_String, NContainer::TCVector<NStr::CStr> const &_PathPatterns, NStr::EMatchWildcardResult _Default)
		{
			NStr::EMatchWildcardResult Ret = NStr::EMatchWildcardResult_NotMatched;

			if (_PathPatterns.f_IsEmpty())
				return _Default;

			for (auto &Pattern : _PathPatterns)
				Ret = fg_Max(Ret, NStr::fg_StrMatchWildcard(_String.f_GetStr(), Pattern.f_GetStr()));

			return Ret;
		}

		bool fg_GroupActive(NStr::CStr const &_Group)
		{
			CTestManager *pManager = g_Tests;

			return pManager->m_IncludeGroups.f_FindEqual(_Group) && !pManager->m_ExcludeGroups.f_FindEqual(_Group);
		}

		bool fg_ShouldRunSubTest(bool _bLeaf)
		{
			CTestManager *pManager = g_Tests;

			CTestManager::CThreadLocal &ThreadLocal = *pManager->m_ThreadLocal;

			NStr::EMatchWildcardResult IncludeResult = fg_MatchPattern(ThreadLocal.m_TestPath, pManager->m_IncludePatterns, NStr::EMatchWildcardResult_WholeStringMatchedAndPatternExhausted);
			if (!(IncludeResult & NStr::EMatchWildcardResult_WholeStringMatched))
				return false;

			if (_bLeaf)
			{
				if
					(
						IncludeResult != NStr::EMatchWildcardResult_WholeStringMatchedAndPatternExhausted
						|| fg_MatchPattern(ThreadLocal.m_TestPath, pManager->m_ExcludePatterns, NStr::EMatchWildcardResult_NotMatched)
						== NStr::EMatchWildcardResult_WholeStringMatchedAndPatternExhausted
					)
				{
					return false;
				}
			}

			bool bIncluded = pManager->m_IncludeGroups.f_IsEmpty() && ThreadLocal.m_TestGroups.f_IsEmpty();
			if (!_bLeaf && ThreadLocal.m_TestGroups.f_IsEmpty())
				bIncluded = true;

			{
				auto Iter = pManager->m_IncludeGroups.f_GetIterator();
				while (Iter)
				{
					if (Iter->f_IsEmpty())
					{
						if (ThreadLocal.m_TestGroups.f_IsEmpty())
						{
							bIncluded = true;
						}
					}
					else if (ThreadLocal.m_TestGroups.f_FindEqual(*Iter))
						bIncluded = true;

					++Iter;
				}
			}
			{
				auto Iter = pManager->m_ExcludeGroups.f_GetIterator();
				while (Iter)
				{
					if (Iter->f_IsEmpty())
					{
						if (ThreadLocal.m_TestGroups.f_IsEmpty())
						{
							bIncluded = false;
						}
					}
					else if (ThreadLocal.m_TestGroups.f_FindEqual(*Iter))
						bIncluded = false;
					++Iter;
				}
			}

			return bIncluded;
		}
		void fg_StepIntoSuites(bool _Step)
		{
			auto& Manager = CTestManager::fs_GetManager();
			Manager.m_ThreadLocal->m_bEnumerating = _Step;
		}

		uint32 fg_RunTests(CTestResults *_pResults, CRunTestOptions const &_Options)
		{
			NFunction::TCFunction<void(NContract::CContractViolation const&)> OldLogger = NContract::fg_SetContractLoggerThrowExceptions();

			if (_Options.m_ReportFlags & ETestReportFlag_ReportCategories)
				fg_StepIntoSuites(true);
			bool bOldEnableExceptionTrace = NException::fg_SetEnableExceptionTrace(false);
#			if DMibEnableSafeCheck > 0
				bool bOldAssethThrow = NContract::fg_MibSafeCheckSetThrowsException(true);
#			endif

			CTestManager *pManager = g_Tests;

			pManager->m_ReportFlags = _Options.m_ReportFlags;
			pManager->m_ExtraData = _Options.m_ExtraData;

			_pResults->f_ReportHeader(_Options.m_ReportFlags);

			// We can only run one test at a time, this is because a test might create threads by itself
			DMibLock(pManager->m_Lock);

			pManager->m_pResults = _pResults;
			pManager->f_ClearStatistics();
			pManager->m_IncludePatterns = _Options.m_IncludePatterns;
			pManager->m_ExcludePatterns = _Options.m_ExcludePatterns;
			{
				mint nGroups = _Options.m_IncludeGroups.f_GetLen();
				for (mint i = 0; i < nGroups; ++i)
				{
					NStr::CStr Group = _Options.m_IncludeGroups[i];
					pManager->m_IncludeGroups[Group];
				}
			}
			{
				mint nGroups = _Options.m_ExcludeGroups.f_GetLen();
				for (mint i = 0; i < nGroups; ++i)
				{
					NStr::CStr Group = _Options.m_ExcludeGroups[i];
					pManager->m_ExcludeGroups[Group];
				}
			}

			NMib::CRunTimeObjectInfo * const pTests = fg_GetRuntimeTypeInfo("NMib::NTest::CTest");

			if (pTests)
			{
				auto Iter = pTests->m_Children.f_GetIter();

				while (Iter)
				{
					NMib::CRunTimeObjectInfo *pIter = Iter;

					NStr::CStr Name = pIter->f_GetName();
					if (Name.f_EndsWith("_Tests"))
					{
						Name = Name.f_RemoveSuffix("_Tests");

						if (Name.f_StartsWith("C"))
							Name = Name.f_RemovePrefix("C");
					}

					Name = Name.f_ReplaceChar('_', '/');

					NStr::CStr FullName = pIter->f_GetNamespaceName().f_Replace("::", "/") / Name;

					NStr::CStr ToFind = "NTest/";
					aint iFindTest = FullName.f_Find(ToFind);
					if (iFindTest >= 0)
					{
						FullName = FullName.f_Extract(ToFind.f_GetLen());
						bool bFound = fg_MatchPattern(FullName, pManager->m_IncludePatterns, NStr::EMatchWildcardResult_WholeStringMatchedAndPatternExhausted)
							& NStr::EMatchWildcardResult_WholeStringMatched
						;

						if (bFound)
						{
							NMib::NTest::CTest *pObject = (NMib::NTest::CTest *)pIter->f_CreateObject();
							if (pObject)
							{
								CTestExceptionFilter ExceptionFilter;
								{
									DMibExceptionFilter(ExceptionFilter);
									{
										DMibLock(pManager->m_ThreadLocal->m_TestPathLock);
										pManager->m_ThreadLocal->m_TestPath = FullName;
									}

									if (pManager->m_ReportFlags & ETestReportFlag_CrashOnException)
									{
										try
										{
											pObject->f_DoTests();
										}
										catch (CReportTestAbortException)
										{
										}
									}
									else
									{
										try
										{
											pObject->f_DoTests();
										}
										catch (CReportTestAbortException)
										{
										}
										catch (...)
										{
											fg_ReportTestException
											(
												ExceptionFilter.f_DetachDumpFiles()
												, ExceptionFilter.f_GetFile() ? ExceptionFilter.f_GetFile() : DMibPFile
												, ExceptionFilter.f_GetFile() ? ExceptionFilter.f_GetLine() : DMibPLine
											);
										}
									}

									delete pObject;
								}

							}
						}
					}
					++Iter;
				}
			}

			_pResults->f_ReportFooter
			(
				pManager->m_nTotalTests.f_Load()
				, pManager->m_nSuccess.f_Load()
				, pManager->m_nSuccessUnexpected.f_Load()
				, pManager->m_nFailed.f_Load()
				, pManager->m_nExpectedFailed.f_Load()
				, pManager->m_nWarnings.f_Load()
				, pManager->m_nIgnored.f_Load()
			);

#			if DMibEnableSafeCheck > 0
				NContract::fg_MibSafeCheckSetThrowsException(bOldAssethThrow);
#			endif
			NContract::fg_SetContractLogger(OldLogger);
			NException::fg_SetEnableExceptionTrace(bOldEnableExceptionTrace);
			if (pManager->m_nFailed.f_Load() != 0 || pManager->m_nSuccessUnexpected.f_Load() != 0)
				return 1;
			return pManager->m_ReturnValue;
		}
#endif // DMibConfig_Tests_Enable

		class CDefaultTestResults : public CTextTestResults
		{
		public:
			CDefaultTestResults(NCommandLine::EAnsiEncodingFlag _AnsiEncodingFlags)
				: CTextTestResults(_AnsiEncodingFlags)
			{
			}

			void fp_ReportText(NStr::CStr const &_Text, ETestSeverity _Severity) override
			{
				NMib::NCommandLine::fg_MalterlibConOut(_Text);
			}
		};

		class CDefaultTestResultsBrief : public CTextTestResultsBrief
		{
		public:
			CDefaultTestResultsBrief(NCommandLine::EAnsiEncodingFlag _AnsiEncodingFlags)
				: CTextTestResultsBrief(_AnsiEncodingFlags)
			{
			}

			void fp_ReportText(const NStr::CStr &_Text, ETestSeverity _Severity) override
			{
				NMib::NCommandLine::fg_MalterlibConOut(_Text);
			}
		};
	}

	void fg_TestSetReturnValue(uint32 _RetValue)
	{
#		if DMibConfig_Tests_Enable
			NPrivate::CTestManager *pManager = NPrivate::g_Tests;
			pManager->m_ReturnValue = _RetValue;
#		endif
	}

	uint32 fg_RunTests(CTestResults *_pResults, CRunTestOptions const &_Options)
	{
#		if DMibConfig_Tests_Enable
			return NPrivate::fg_RunTests(_pResults, _Options);
#		else
			_pResults->f_ReportResult(ETestResult_Fail, "Root", "Tests disabled", "", CTestLocation(DMibPFile, DMibPLine), ETest_FailAndStop, ECheckType_Message, ETestFlag_None, "");
			return 1;
#		endif
	}

	uint32 fg_RunTests(CRunTestOptions const &_Options)
	{
		NPrivate::CDefaultTestResults Results(NCommandLine::EAnsiEncodingFlag_None);

#		if DMibConfig_Tests_Enable
			return NPrivate::fg_RunTests(&Results, _Options);
#		else
			Results.f_ReportResult(ETestResult_Fail, "Root", "Tests disabled", "", CTestLocation(DMibPFile, DMibPLine), ETest_FailAndStop, ECheckType_Message, ETestFlag_None, "");
			return 1;
#		endif
	}

	NStr::CStr fg_TestGetExtraData()
	{
#		if DMibConfig_Tests_Enable
			NPrivate::CTestManager *pTestManager = NPrivate::g_Tests;
			return pTestManager->m_ExtraData;
#		else
			return NStr::CStr();
#		endif
	}

	NStr::CStr fg_TestGetCurrentPath()
	{
#		if DMibConfig_Tests_Enable
			NPrivate::CTestManager *pTestManager = NPrivate::g_Tests;
			NPrivate::CTestManager::CThreadLocal &ThreadLocal = *pTestManager->m_ThreadLocal;
			return ThreadLocal.m_TestPath;
#		else
			return NStr::CStr();
#		endif
	}

	NContainer::TCMap<NStr::CStr> fg_TestGetCurrentGroups()
	{
#		if DMibConfig_Tests_Enable
			return NPrivate::fg_GetGroups();
#		else
			return NContainer::TCMap<NStr::CStr>();
#		endif
	}

	ETestReportFlag fg_TestReportFlags()
	{
#		if DMibConfig_Tests_Enable
			NPrivate::CTestManager *pTestManager = NPrivate::g_Tests;
			return pTestManager->m_ReportFlags;
#		else
			return ETestReportFlag_None;
#		endif
	}

	NContainer::TCVector<NStr::CStr> fg_StrSplit(NStr::CStr const& _String, ch8 const* _pSplit)
	{
		NContainer::TCVector<NStr::CStr> Ret;

		NStr::CStr String = _String;
		while (!String.f_IsEmpty())
			Ret.f_Insert(NStr::fg_GetStrSep(String, _pSplit));

		return Ret;
	}

	uint32 fg_RunTests()
	{
		NStorage::TCSharedPointer<NMib::NCommandLine::CCommandLineSpecification> pCommandLineSpec = fg_Construct();
		pCommandLineSpec->f_AddHelpCommand();
		pCommandLineSpec->f_AddTerminalOptions();

		auto Section = pCommandLineSpec->f_AddSection("Test", "Commands for running tests");

		auto fRunTests = [](NEncoding::CEJSON const &_Parameters, CRunTestOptions const &_RunOptions, NCommandLine::EAnsiEncodingFlag _AnsiEncodingFlags) -> uint32
			{
				CRegistryTestResults RegistryResults;
				NPrivate::CDefaultTestResults DefaultResults(_AnsiEncodingFlags);
				NPrivate::CDefaultTestResultsBrief BriefResults(_AnsiEncodingFlags);
				NPrivate::CCategoryLister CategoryResults;
				NPrivate::CNullTestResults NullResults;

				auto RunOptions = _RunOptions;
				CTestResults *pResults = &DefaultResults;
				if (RunOptions.m_ReportFlags & ETestReportFlag_ReportCategories)
					pResults = &CategoryResults;

				if (auto pValue = _Parameters.f_GetMember("FilterResults"))
				{
					for (auto &Filter : pValue->f_Array())
					{
						auto &FilterName = Filter.f_String();
						if (FilterName == "All")
							RunOptions.m_ReportFlags |= ETestReportFlag_All;
						else if (FilterName == "Default")
							RunOptions.m_ReportFlags |= ETestReportFlag_Default;
						else if (FilterName == "Success")
							RunOptions.m_ReportFlags |= ETestReportFlag_Success;
						else if (FilterName == "Ignore")
							RunOptions.m_ReportFlags |= ETestReportFlag_Ignored;
						else if (FilterName == "Warning")
							RunOptions.m_ReportFlags |= ETestReportFlag_Warning;
						else if (FilterName == "Fail")
							RunOptions.m_ReportFlags |= ETestReportFlag_Fail;
						else if (FilterName == "FailAndStop")
							RunOptions.m_ReportFlags |= ETestReportFlag_FailAndStop;
						else if (FilterName == "ExpectFail")
							RunOptions.m_ReportFlags |= ETestReportFlag_ExpectFail;
						else if (FilterName == "ExectFailAndStop")
							RunOptions.m_ReportFlags |= ETestReportFlag_ExpectFailAndStop;
					}
				}

				if (auto pValue = _Parameters.f_GetMember("ReportValues"); pValue && pValue->f_Boolean())
					RunOptions.m_ReportFlags |= ETestReportFlag_ReportValues;

				if (auto pValue = _Parameters.f_GetMember("DetailedPerformance"); pValue && pValue->f_Boolean())
					RunOptions.m_ReportFlags |= ETestReportFlag_DetailedPerformance;

				if (auto pValue = _Parameters.f_GetMember("DetailedMemory"); pValue && pValue->f_Boolean())
					RunOptions.m_ReportFlags |= ETestReportFlag_DetailedMemory;

				if (auto pValue = _Parameters.f_GetMember("BreakOnFail"); pValue && pValue->f_Boolean())
					RunOptions.m_ReportFlags |= ETestReportFlag_BreakOnFail;

				if (auto pValue = _Parameters.f_GetMember("ProcessRecursive"); pValue && pValue->f_Boolean())
					RunOptions.m_ReportFlags |= ETestReportFlag_ProcessRecursive;

				if (auto pValue = _Parameters.f_GetMember("CompareToBaseline"); pValue && pValue->f_Boolean())
					RunOptions.m_ReportFlags |= ETestReportFlag_CompareToBaseline;

				if (auto pValue = _Parameters.f_GetMember("CrashOnException"); pValue && pValue->f_Boolean())
					RunOptions.m_ReportFlags |= ETestReportFlag_CrashOnException;

				auto fGetGroups = [&](NEncoding::CEJSON const &_Groups)
					{
						NContainer::TCVector<NStr::CStr> OutGroups;
						for (auto &Group : _Groups.f_Array())
						{
							if (Group.f_String() == "Default")
								OutGroups.f_Insert("");
							else
								OutGroups.f_Insert(Group.f_String());
						}

						return OutGroups;
					}
				;

				if (auto pValue = _Parameters.f_GetMember("Groups"))
					RunOptions.m_IncludeGroups = fGetGroups(*pValue);

				if (auto pValue = _Parameters.f_GetMember("ExcludeGroups"))
					RunOptions.m_ExcludeGroups = fGetGroups(*pValue);

				if (auto pValue = _Parameters.f_GetMember("ExtraData"))
					RunOptions.m_ExtraData = pValue->f_String();

				if (auto pValue = _Parameters.f_GetMember("Logger"))
				{
					auto &LoggerName = pValue->f_String();
					if (LoggerName == "Default")
						;
					else if (LoggerName == "Brief")
						pResults = &BriefResults;
					else if (LoggerName == "Registry")
						pResults = &RegistryResults;
					else if (LoggerName == "Null")
						pResults = &NullResults;
					else
						DMibNeverGetHere;
				}

				if (auto pValue = _Parameters.f_GetMember("Paths"))
				{
					for (auto &Path : pValue->f_Array())
						RunOptions.m_IncludePatterns.f_Insert(Path.f_String());
				}

				if (auto pValue = _Parameters.f_GetMember("ExcludePaths"))
				{
					for (auto &Path : pValue->f_Array())
						RunOptions.m_ExcludePatterns.f_Insert(Path.f_String());
				}

				return fg_RunTests(pResults, RunOptions);
			}
		;

		auto Parameter_Paths = "Paths...?"_=
			{
				"Type"_= {""}
				, "Default"_= _[_]
				, "Description"_= "Specify the test paths to run tests for. Can be wildcards."
			}
		;
		auto Option_ExcludePaths = "ExcludePaths?"_=
			{
				"Names"_= {"--exclude-paths"}
				, "Type"_= {""}
				, "Default"_= _[_]
				, "Description"_= "Specify the test paths to exclude. Can be wildcards."
			}
		;
		auto Option_ExtraData = "ExtraData?"_=
			{
				"Names"_= {"--extra-data", "-d"}
				, "Default"_= ""
				, "Description"_= "Supply extra general data to tests. Accessible with fg_TestGetExtraData from tests."
			}
		;
		auto Option_FilterResults = "FilterResults?"_=
			{
				"Names"_= {"--filter-results", "-r"}
				, "Default"_= {"Default"}
				, "Type"_= {NCommandLine::COneOf{"All", "Default", "Success", "Ignore", "Warning", "Fail", "FailAndStop", "ExpectFail", "ExpectFailAndStop"}}
				, "Description"_= "Filter test results.\n"
				"@Indent=23\r"
				"   All:                Report all test results.\r"
				"   Default:            Report default test results. This includes Warning, Fail, FailAndStop\r"
				"   Success:            Report successful tests.\r"
				"   Ignore:             Report ignored tests.\r"
				"   Warning:            Report tests with warnings.\r"
				"   Fail:               Report failed tests.\r"
				"   FailAndStop:        Report fail and stop tests.\r"
				"   ExpectFail:         Report expected fail tests.\r"
				"   ExpectFailAndStop:  Report expected faild and stop tests.\r"
				"\r"
			}
		;
		auto GroupsList = NCommandLine::COneOf{"Default", "Performance", "Torture", "Memory", "Unfinished", "Expensive", "Manual", "SuperUser"};
		auto Option_Groups = "Groups?"_=
			{
				"Names"_= {"--groups", "-g"}
				, "Default"_= {"Default"}
				, "Type"_= {GroupsList}
				, "Description"_= "Specify the groups to include in test.\n"
				"@Indent=17\r"
				"   Default:      Run tests without a group specified.\r"
				"   Performance:  Run tests with Performance group specified.\r"
				"   Torture:      Run tests with Torgutre group specified.\r"
				"   Memory:       Run tests with Performance group specified.\r"
				"   Unfinished:   Run tests with Unfinished group specified.\r"
				"   Expensive:    Run tests with Expensive group specified.\r"
				"   Manual:       Run tests with Manual group specified.\r"
				"   SuperUser:    Run tests with SuperUser group specified.\r"
				"\r"
			}
		;
		auto Option_ExcludeGroups = "ExcludeGroups?"_=
			{
				"Names"_= {"--exclude-groups", "-e"}
				, "Default"_= _[_]
				, "Type"_= {GroupsList}
				, "Description"_= "Specify the groups to include in test.\n"
				"@Indent=17\r"
				"   Default:      Don't run tests without a group specified.\r"
				"   Performance:  Don't run tests with Performance group specified.\r"
				"   Torture:      Don't run tests with Torgutre group specified.\r"
				"   Memory:       Don't run tests with Performance group specified.\r"
				"   Unfinished:   Don't run tests with Unfinished group specified.\r"
				"   Expensive:    Don't run tests with Expensive group specified.\r"
				"   Manual:       Don't run tests with Manual group specified.\r"
				"   SuperUser:    Don't run tests with SuperUser group specified.\r"
				"\r"
			}
		;
		auto Option_Logger = "Logger?"_=
			{
				"Names"_= {"--logger"}
				, "Default"_= "Default"
				, "Type"_= NCommandLine::COneOf{"Default", "Brief", "Registry", "Null"}
				, "Description"_= "Test logger.\n"
				"@Indent=14\r"
				"   Default:   Report test in human readable format.\r"
				"   Brief:     Report test in human readable brief format.\r"
				"   Registry:  Report test in registry format.\r"
				"   Null:      Don't report test results.\r"
				"\r"
			}
		;
		auto Option_ReportValues = "ReportValues?"_=
			{
				"Names"_= {"--values"}
				, "Default"_= true
				, "Description"_= "Include values in results.\n"
			}
		;
		auto Option_DetailedPerformance = "DetailedPerformance?"_=
			{
				"Names"_= {"--detailed-performance", "-p"}
				, "Default"_= false
				, "Description"_= "Display detailed performance results.\n"
			}
		;
		auto Option_DetailedMemory = "DetailedMemory?"_=
			{
				"Names"_= {"--detailed-memory", "-m"}
				, "Default"_= false
				, "Description"_= "Display detailed memory results.\n"
			}
		;
		auto Option_BreakOnFail = "BreakOnFail?"_=
			{
				"Names"_= {"--break-on-fail", "-b"}
				, "Default"_= false
				, "Description"_= "Break into debugger on failure.\n"
			}
		;
		auto Option_ProcessRecursive = "ProcessRecursive?"_=
			{
				"Names"_= {"--process-recursive"}
				, "Default"_= false
				, "Description"_= "Break into debugger on failure.\n"
			}
		;
		auto Option_CompareToBaseline = "CompareToBaseline?"_=
			{
				"Names"_= {"--compare-to-baseline", "-c"}
				, "Default"_= false
				, "Description"_= "Compare performance results to baseline.\n"
			}
		;
		auto Option_CrashOnException = "CrashOnException?"_=
			{
				"Names"_= {"--fault-on-exception", "-f"}
				, "Default"_= false
				, "Description"_= "Crash instead of capturing exceptions.\n"
			}
		;

		auto TestCommand = Section.f_RegisterDirectCommand
			(
				{
					"Names"_= {"--test", "-t"}
					, "GreedyDefaultCommandParameters"_= true
					, "Description"_= "Run tests contained in this binary.\n"
					, "Options"_=
					{
						Option_ExcludePaths
						, Option_FilterResults
						, Option_Groups
						, Option_ExcludeGroups
						, Option_Logger
						, Option_ReportValues
						, Option_DetailedPerformance
						, Option_DetailedMemory
						, Option_BreakOnFail
						, Option_ProcessRecursive
						, Option_CompareToBaseline
						, Option_CrashOnException
						, Option_ExtraData
					}
					, "Parameters"_=
					{
						Parameter_Paths
					}
				}
				, [fRunTests](NEncoding::CEJSON const &_Parameters, NCommandLine::CCommandLineClient &_CommandLineClient)
				{
					CRunTestOptions RunOptions;
					RunOptions.m_ReportFlags = ETestReportFlag_None;
					return fRunTests(_Parameters, RunOptions, _CommandLineClient.f_AnsiEncodingFlags());
				}
			)
		;

		Section.f_RegisterDirectCommand
			(
				{
					"Names"_= {"--test-list", "-l"}
					, "Description"_= "List test suites contained in this binary.\n"
					, "Options"_=
					{
						Option_ExcludePaths
						, Option_Groups
						, Option_ExcludeGroups
						, Option_Logger
						, Option_ExtraData
					}
					, "Parameters"_=
					{
						Parameter_Paths
					}
				}
				, [fRunTests](NEncoding::CEJSON const &_Parameters, NCommandLine::CCommandLineClient &_CommandLineClient)
				{
					CRunTestOptions RunOptions;
					RunOptions.m_ReportFlags = ETestReportFlag_ReportCategories;
					return fRunTests(_Parameters, RunOptions, _CommandLineClient.f_AnsiEncodingFlags());
				}
			)
		;

		pCommandLineSpec->f_SetDefaultCommand(TestCommand);

		NCommandLine::CCommandLineClient Client(pCommandLineSpec);

		try
		{
			return Client.f_RunCommandLine();
		}
		catch (NException::CException const &_Exception)
		{
			DMibConErrOut("{}\n", _Exception);
			return 1;
		}
	}

	bool fg_GroupActive(NStr::CStr const &_Group)
	{
#if DMibConfig_Tests_Enable
		return NPrivate::fg_GroupActive(_Group);
#else
		return false;
#endif
	}
}
