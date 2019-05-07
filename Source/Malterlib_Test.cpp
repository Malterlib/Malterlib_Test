// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>
#include <Mib/Test/Exception>
#include <Mib/CommandLine/CommandLine>
#include <Mib/Core/RuntimeType>

namespace NMib::NTest
{
#if defined DMibContractConfigure_RequireEnabled
	TCThrowsException<NContract::CContractException_Require> fg_ViolatesRequire()
	{
		return TCThrowsException<NContract::CContractException_Require>();
	}
	TCThrowsExceptionExact<NContract::CContractException_Require> fg_ViolatesRequire(const ch8 *_pError)
	{
		return NContract::CContractException_Require("CContractException_Require", DMibPFile, DMibPLine, DMibPFunction, _pError, true);
	}
#else
	TCThrowsException<> fg_ViolatesRequire()
	{
		return TCThrowsException<>();
	}
	TCThrowsException<> fg_ViolatesRequire(const ch8 *_pError)
	{
		return TCThrowsException<>();
	}
#endif

#if DMibEnableSafeCheck > 0
	TCThrowsException<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck()
	{
		return TCThrowsException<NException::CExceptionSafeCheck>();
	}

	TCThrowsExceptionExact<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck(const ch8 *_pError)
	{
		return NException::CExceptionSafeCheck("CExceptionSafeCheck", DMibPFile, DMibPLine, DMibPFunction, _pError, true);
	}
#else
	TCThrowsException<> fg_ViolatesSafeCheck()
	{
		return TCThrowsException<>();
	}
	TCThrowsException<> fg_ViolatesSafeCheck(const ch8 *_pError)
	{
		return TCThrowsException<>();
	}
#endif


	namespace NPrivate
	{
#if DMibConfig_Tests_Enable

		class CTestManager
		{
		public:

			NThread::CMutual m_Lock;

			CTestResults *m_pResults;

			ETestReportFlag m_ReportFlags;

			NMib::NStr::CStr m_ExtraData;

			NContainer::TCVector<NStr::CStr> m_PathPatterns;
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
				bint operator < (CUniqueTest const &_Other) const
				{
					auto Ret = NStr::fg_StrCmp(m_pFile, _Other.m_pFile);
					if (Ret < 0)
						return true;
					else if (Ret > 0)
						return false;
					return m_Line < _Other.m_Line;
				}
			};
			class CThreadLocal
			{
			public:
				CThreadLocal() = default;
				CThreadLocal(CThreadLocal const &_Inherit)
					: m_bEnableValues{_Inherit.m_bEnableValues}
					, m_bEnableExceptionFilter{_Inherit.m_bEnableExceptionFilter}
					, m_bInsideTestSuite{_Inherit.m_bInsideTestSuite}
					, m_pLastTestFile{_Inherit.m_pLastTestFile}
					, m_LastTestLine{_Inherit.m_LastTestLine}
				{
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
					NContainer::TCMap<NStr::CStr, NContainer::TCMap<CUniqueTest, TCAutoClearInt<bint>>> UniqueTests;
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
									NContainer::TCMap<CUniqueTest, TCAutoClearInt<bint>> const &Aggregated = *Iter;
									bint bCreated = false;
									NContainer::TCMap<CUniqueTest, TCAutoClearInt<bint>> &AggregatedNew = m_pInherit->m_UniqueTests.f_Map(Iter.f_GetKey(), bCreated);
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
				NStr::CStr m_TestPath;
				mutable NThread::CMutual m_TestPathLock;
				NContainer::TCMap<NStr::CStr> m_TestGroups;
				CTestResults *m_pResults = nullptr;
				const ch8 *m_pLastTestFile = nullptr;
				int32 m_LastTestLine = 0;

				bint m_bEnableValues = true;
				bint m_bEnableExceptionFilter = true;
				bint m_bEnumerating = false;
				bint m_bInsideTestSuite = false;
				mutable NThread::CMutual m_UniqueTestsLock;
				mutable NContainer::TCMap<NStr::CStr, NContainer::TCMap<CUniqueTest, TCAutoClearInt<bint>>> m_UniqueTests;
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

		static NStorage::TCAggregate<CTestManager, 256> g_Tests = {DAggregateInit};

		CTestManager &CTestManager::fs_GetManager()
		{
			return *g_Tests;
		}

		bint fg_SetEnableValues(bint _bEnableValues)
		{
			CTestManager *pTestManager = g_Tests;
			bint bReturn = pTestManager->m_ThreadLocal->m_bEnableValues;
			pTestManager->m_ThreadLocal->m_bEnableValues = _bEnableValues;
			return bReturn;
		}

		bint fg_GetEnableValues()
		{
			CTestManager *pTestManager = g_Tests;
			return pTestManager->m_ThreadLocal->m_bEnableValues;
		}

		bint fg_SetEnableExceptionFilter(bint _bEnableExceptionFilter)
		{
			CTestManager *pTestManager = g_Tests;
			bint bReturn = pTestManager->m_ThreadLocal->m_bEnableExceptionFilter;
			pTestManager->m_ThreadLocal->m_bEnableExceptionFilter = _bEnableExceptionFilter;
			return bReturn;
		}

		bint fg_GetEnableExceptionFilter()
		{
			CTestManager *pTestManager = g_Tests;
			return pTestManager->m_ThreadLocal->m_bEnableExceptionFilter;
		}

		bint fg_InsideTestSuite()
		{
			CTestManager *pTestManager = g_Tests;
			return pTestManager->m_ThreadLocal->m_bInsideTestSuite;
		}
		void fg_InsideTestSuite(bint _bInsideTestSuite)
		{
			CTestManager *pTestManager = g_Tests;
			pTestManager->m_ThreadLocal->m_bInsideTestSuite = _bInsideTestSuite;
		}
		NStr::CStr fg_PushCategory(const NStr::CStr &_Category)
		{
			CTestManager *pTestManager = g_Tests;
			auto &ThreadLocal = *pTestManager->m_ThreadLocal;
			DMibLock(ThreadLocal.m_TestPathLock);
			NStr::CStr PreviousPath = ThreadLocal.m_TestPath;
			NStr::fg_StrAddWithSeparator(ThreadLocal.m_TestPath, _Category ,"/");
			return PreviousPath;
		}

		void fg_SetTestLastLocation(const ch8 *_pFile, int32 _Line)
		{
			CTestManager *pTestManager = g_Tests;
			auto &ThreadLocal = *pTestManager->m_ThreadLocal;
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
				}
			}
			if (!(NeedReportFlags & ETestNeedReportFlag_AllowDuplicates))
			{
				NStr::CStr FullPath = ThreadLocal.m_TestPath + "/" +_Description;
				{
					DMibLock(ThreadLocal.m_UniqueTestsLock);
					bint bCreated = false;
					NContainer::TCMap<CTestManager::CUniqueTest, TCAutoClearInt<bint>> &AggregatedTests = ThreadLocal.m_UniqueTests.f_Map(FullPath, bCreated);
					bint bAllowNonUnique = false;
					if (_Flags & ETestFlag_Aggregated)
					{
						bint bCreated = false;
						CTestManager::CUniqueTest Key;
						Key.m_pFile = _pFile;
						Key.m_Line = _Line;

						TCAutoClearInt<bint> &bValue = AggregatedTests.f_Map(Key, bCreated);
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

		void CTestExceptionFilter::f_SetDumpFiles(const NContainer::TCVector<NStr::CStr> &_Files)
		{
			mint nDumpFiles = m_DumpFiles.f_GetLen();
			for (mint i = 0; i < nDumpFiles; ++i)
			{
				try
				{
					NFile::CFile::fs_DeleteFile(m_DumpFiles[i]);
				}
				catch (NException::CException)
				{
				}
			}
			m_DumpFiles = _Files;
		}

		NContainer::TCVector<NStr::CStr> CTestExceptionFilter::f_DetachDumpFiles()
		{
			return fg_Move(m_DumpFiles);
		}
		const ch8 *CTestExceptionFilter::f_GetFile()
		{
			if (!m_pStackTraceInfo || !(*m_pStackTraceInfo->m_pSourceFileName))
				return nullptr;
			return m_pStackTraceInfo->m_pSourceFileName;
		}

		int32 CTestExceptionFilter::f_GetLine()
		{
			if (!m_pStackTraceInfo)
				return 0;
			return m_pStackTraceInfo->m_SourceLine;
		}


		void CTestExceptionFilter::f_Exception(void *_pExceptionData)
		{
			if (m_pStackTraceInfo)
				NSys::fg_Debug_ReleaseStackTraceInfo(m_pStackTraceInfo);
			m_pStackTraceInfo = nullptr;

			CReportTestAbortException *pException = (CReportTestAbortException *)_pExceptionData;
			if (!pException || !pException->f_IsValid())
			{
				NContainer::TCVector<NStr::CStr> DumpFiles = fg_DumpTestException();
				f_SetDumpFiles(DumpFiles);
			}
			CMibCodeAddress Stack[64];
			mint nStack = NSys::fg_System_GetStackTrace(Stack, 64);
			for (mint i = 0; i < nStack; ++i)
			{
				CStackTraceInfo *pInfo = NSys::fg_Debug_AquireStackTraceInfo(Stack[i]);
				if (pInfo)
				{
					if (*pInfo->m_pSourceFileName && NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "core\\source\\platform") < 0
						&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "core/source/platform") < 0
						&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "throw.cpp") < 0
						&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_test.cpp") < 0
						&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_contract.cpp") < 0
						&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_contract.h") < 0
						&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_debug.cpp") < 0
						&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "include\\xx") < 0
						)
					{
						//DMibTrace(DMibPFileLineFormat " {}" DMibNewLine, pInfo->m_pSourceFileName << pInfo->m_SourceLine << pInfo->m_pFunctionName);
						m_pStackTraceInfo = pInfo;
						break;
					}
					NSys::fg_Debug_ReleaseStackTraceInfo(pInfo);
				}
			}
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

		NContainer::TCVector<NStr::CStr> fg_DumpTestException()
		{
			NContainer::TCVector<NStr::CStr> GeneratedLogs;
			NSys::fg_Debug_GenerateCrashDump("", "", GeneratedLogs, false);
			return GeneratedLogs;
		}

		void fg_ReportTestException(const NContainer::TCVector<NStr::CStr> &_DumpFiles, const ch8 *_pFile, int32 _Line)
		{
			using namespace NStr;

			const ch8 *pFile = nullptr;
			int32 Line = 0;
			fg_GetTestLastLocation(pFile, Line);
			NStr::CStr ExtraReportData;

			if (pFile)
				ExtraReportData += DMibPFileLineFormat " Last know test location{\n}"_f << pFile << Line;

			mint nLogs = _DumpFiles.f_GetLen();
			if (nLogs != 0)
			{
				ExtraReportData += "The following crash dump files were generated:" DMibNewLine;
				for (mint i = 0; i < nLogs; ++i)
				{
					ExtraReportData += "{}{\n}"_f << _DumpFiles[i];
				}
			}

			try
			{
				try
				{
					throw;
				}
				catch (NContract::CContractException_Require const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " {}{\n}{\n}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, "Require contract violation"
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (NContract::CContractException_Check const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " {}{\n}{\n}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, "Check contract violation"
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (NContract::CContractException_Ensure const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " {}{\n}{\n}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, "Ensure contract violation"
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (NContract::CContractException_Invariant const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " {}{\n}{\n}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, "Invariant contract violation"
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (NContract::CContractException_NeverGetHere const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " {}{\n}{\n}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, "Never get here contract violation"
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (NContract::CContractException const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " {}{\n}{\n}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, NStr::CStr::CFormat("Contract violation: {}") << _Exception.f_GetClass()
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (NException::CExceptionSafeCheck const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " {}{\n}{\n}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, NStr::CStr::CFormat("Assert violation")
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (NException::CException const &_Exception)
				{
					CStr ReportData = DMibPFileLineFormat " Uncaught {} exception: {}{\n}{\n}{}"_f
						<< _Exception.f_GetFile()
						<< _Exception.f_GetLine()
						<< _Exception.f_GetClass()
						<< _Exception.f_GetErrorStr()
						<< ExtraReportData
					;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, NStr::CStr::CFormat("Uncaught exception")
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (std::exception const& _Exception)
				{
					CStr ReportData = " Uncaught {} exception{\n}{\n}{}"_f
						<< NStr::CStr(_Exception.what())
						<< ExtraReportData
					;
					fg_ReportTestResult
						(
							ETestResult_Fail
							, NStr::CStr("Uncaught exception")
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
				catch (...)
				{
					fg_ReportTestResult
						(
							ETestResult_Fail
							, "Uncaught exception"
							, ""
							, ETest_FailAndStop
							, ECheckType_Message
							, _pFile
							, _Line
							, ExtraReportData
							, ETestFlag_None
							, ETestResultReportFlag_AskReport
						)
					;
				}
			}
			catch (CReportTestAbortException)
			{
			}
		}


		NStr::EMatchWildcardResult fg_MatchPattern(const NStr::CStr &_String, NContainer::TCVector<NStr::CStr> const &_PathPatterns)
		{
			NStr::EMatchWildcardResult Ret = NStr::EMatchWildcardResult_NotMatched;

			if (_PathPatterns.f_IsEmpty())
				return NStr::EMatchWildcardResult_WholeStringMatchedAndPatternExhausted;

			for (auto &Pattern : _PathPatterns)
				Ret = fg_Max(Ret, NStr::fg_StrMatchWildcard(_String.f_GetStr(), Pattern.f_GetStr()));

			return Ret;
		}

		bint fg_GroupActive(NStr::CStr const &_Group)
		{
			CTestManager *pManager = g_Tests;

			return pManager->m_IncludeGroups.f_FindEqual(_Group) && !pManager->m_ExcludeGroups.f_FindEqual(_Group);
		}

		bint fg_ShouldRunSubTest(bint _bLeaf)
		{
			CTestManager *pManager = g_Tests;

			CTestManager::CThreadLocal &ThreadLocal = *pManager->m_ThreadLocal;

			NStr::EMatchWildcardResult Result = fg_MatchPattern(ThreadLocal.m_TestPath, pManager->m_PathPatterns);
			if (!(Result & NStr::EMatchWildcardResult_WholeStringMatched))
				return false;

			if (_bLeaf)
			{
				if (Result != NStr::EMatchWildcardResult_WholeStringMatchedAndPatternExhausted)
					return false;
			}

			bint bIncluded = pManager->m_IncludeGroups.f_IsEmpty() && ThreadLocal.m_TestGroups.f_IsEmpty();
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
		void fg_StepIntoSuites(bint _Step)
		{
			auto& Manager = CTestManager::fs_GetManager();
			Manager.m_ThreadLocal->m_bEnumerating = _Step;
		}

		uint32 fg_RunTests(CTestResults *_pResults, CRunTestOptions const &_Options)
		{
			NFunction::TCFunction<void(NContract::CContractViolation const&)> OldLogger = NContract::fg_SetContractLoggerThrowExceptions();

			if (_Options.m_ReportFlags & ETestReportFlag_ReportCategories)
				fg_StepIntoSuites(true);
			bint bOldEnableExceptionTrace = NException::fg_SetEnableExceptionTrace(false);
#			if DMibEnableSafeCheck > 0
				bint bOldAssethThrow = NContract::fg_MibSafeCheckSetThrowsException(true);
#			endif

			CTestManager *pManager = g_Tests;

			pManager->m_ReportFlags = _Options.m_ReportFlags;
			pManager->m_ExtraData = _Options.m_ExtraData;

			_pResults->f_ReportHeader(_Options.m_ReportFlags);

			// We can only run one test at a time, this is because a test might create threads by itself
			DMibLock(pManager->m_Lock);

			pManager->m_pResults = _pResults;
			pManager->f_ClearStatistics();
			{
				mint nPaths = _Options.m_Paths.f_GetLen();
				pManager->m_PathPatterns.f_SetLen(nPaths);
				for (mint i = 0; i < nPaths; ++i)
				{
					pManager->m_PathPatterns[i] = _Options.m_Paths[i];
				}
			}
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
					NStr::CStr FullName = pIter->f_GetNamespaceName().f_Replace("::", "/");
					NStr::CStr Name = pIter->f_GetName();
					aint iFindTests = Name.f_FindReverse("_Tests");
					if (iFindTests >= 0)
					{
						Name = Name.f_Left(iFindTests);
						if (Name[0] == 'C')
							Name = Name.f_Extract(1);
					}

					if (FullName.f_IsEmpty())
						FullName = Name;
					else
						FullName += "/" + Name;

					NStr::CStr ToFind = "NTest/";
					aint iFindTest = FullName.f_Find(ToFind);
					if (iFindTest >= 0)
					{
						FullName = FullName.f_Extract(ToFind.f_GetLen());
						bint bFound = fg_MatchPattern(FullName, pManager->m_PathPatterns) & NStr::EMatchWildcardResult_WholeStringMatched;

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
			void fp_ReportText(const NStr::CStr &_Text, NSys::EColor _Color) override
			{
				NMib::NCommandLine::fg_MalterlibConOut(_Color, _Text);
			}
		};


		class CDefaultTestResultsBrief : public CTextTestResultsBrief
		{
			void fp_ReportText(const NStr::CStr &_Text, NSys::EColor _Color) override
			{
				NMib::NCommandLine::fg_MalterlibConOut(_Color, _Text);
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
		NPrivate::CDefaultTestResults Results;
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
		NContainer::TCVector<NStr::CStr> CommandLineArgs;
		NSys::fg_Process_GetCommandLineArgs(CommandLineArgs);
		NContainer::TCVector<NStr::CStr> Argv = CommandLineArgs;

		NPrivate::CRegistryTestResults RegistryResults;
		NPrivate::CDefaultTestResults DefaultResults;
		NPrivate::CDefaultTestResultsBrief BriefResults;
		NPrivate::CCategoryLister CategoryResults;
		NPrivate::CNullTestResults NullResults;

		CTestResults *pResults = &DefaultResults;

		CRunTestOptions RunOptions;

		NMib::NCommandLine::CParser Parser;
		NCommandLine::CCommandArguments Options;
		if
		(
			![&]() -> bint
			{
				using namespace NMib::NCommandLine;

				COption Tests("Tests", "t", "run tests contained in this binary");
				Tests.f_Add(CValue("Path"));
				Parser.f_Add(Tests);

				COption TestsList("TestsList", "l", "list test categories contained in this binary");
				TestsList.f_Add(CValue("Path"));
				Parser.f_Add(TestsList);

				COption TestData("TestData", "d", "supply extra general data to tests");
				TestData.f_Add(CValue("Data"));
				Parser.f_Add(TestData);

				COption TestResults("TestResults", "r", "filter test results");
				{
					CValue::CValidValues Valid;
					Valid.f_Insert("All");
					Valid.f_Insert("Default");
					Valid.f_Insert("Success");
					Valid.f_Insert("Ignored");
					Valid.f_Insert("Warning");
					Valid.f_Insert("Fail");
					Valid.f_Insert("FailAndStop");
					Valid.f_Insert("ExpectFail");
					Valid.f_Insert("ExpectFailAndStop");
					Valid.f_Insert("UseColour");
					Valid.f_Insert("DetailedPerformance");
					Valid.f_Insert("DetailedMemory");
					Valid.f_Insert("BreakOnFail");
					Valid.f_Insert("ProcessRecursive");
					Valid.f_Insert("CompareToBaseline");
					Valid.f_Insert("CrashOnException");
					TestResults.f_AddList(CValue("Display",Valid));
				}
				Parser.f_Add(TestResults);

				COption TestLogger("TestLogger", "L", "specify the logger to use");
				{
					CValue::CValidValues Valid;
					Valid.f_Insert("Default");
					Valid.f_Insert("Brief");
					Valid.f_Insert("Registry");
					Valid.f_Insert("Null");
					TestLogger.f_AddList(CValue("Logger", Valid));
				}
				Parser.f_Add(TestLogger);

				auto fGroups = []() -> CValue::CValidValues
					{
						return {"Default", "Performance", "Torture", "Memory", "Unfinished", "Expensive", "Manual", "SuperUser", ""};
					}
				;

				COption TestGroups("TestGroups", "G", "specify the groups to include in test");
				TestGroups.f_AddList(CValue("Groups", fGroups()));
				Parser.f_Add(TestGroups);

				COption TestExcludeGroups("TestExcludeGroups", "E", "specify the groups to exclude from test");
				TestExcludeGroups.f_AddList(CValue("Groups", fGroups()));
				Parser.f_Add(TestExcludeGroups);

				try
				{
					Options = fg_ParseCommandLine(Parser,Argv);
					return true;
				}
				catch(CInvalidArguments const &_Ex)
				{
					DMibConOut("Incorrect argument: {}" DMibNewLine, _Ex.f_Arg() );
					auto Msg = Parser.f_HelpMessage();
					DMibConOut("{}", Msg);
				}
				return false;
			}()
		)
			return 1;

		if (Options.f_IsSet("help"))
		{
			auto Msg = Parser.f_HelpMessage();
			DMibConOut("{}", Msg);
			return 0;
		}

		RunOptions.m_ReportFlags = ETestReportFlag_None;
		if (Options.f_IsSet("TestResults"))
		{
			if (Options.f_HasList("TestResults", "Display"))
			{
				if (Options.f_HasListItem("TestResults", "Display", "All"))
					RunOptions.m_ReportFlags |= ETestReportFlag_All;
				if (Options.f_HasListItem("TestResults", "Display", "Default"))
					RunOptions.m_ReportFlags |= ETestReportFlag_Default;
				if (Options.f_HasListItem("TestResults", "Display", "Success"))
					RunOptions.m_ReportFlags |= ETestReportFlag_Success;
				if (Options.f_HasListItem("TestResults", "Display", "Warning"))
					RunOptions.m_ReportFlags |= ETestReportFlag_Warning;
				if (Options.f_HasListItem("TestResults", "Display", "Fail"))
					RunOptions.m_ReportFlags |= ETestReportFlag_Fail;
				if (Options.f_HasListItem("TestResults", "Display", "FailAndStop"))
					RunOptions.m_ReportFlags |= ETestReportFlag_FailAndStop;
				if (Options.f_HasListItem("TestResults", "Display", "ExpectFail"))
					RunOptions.m_ReportFlags |= ETestReportFlag_ExpectFail;
				if (Options.f_HasListItem("TestResults", "Display", "ExectFailAndStop"))
					RunOptions.m_ReportFlags |= ETestReportFlag_ExpectFailAndStop;
				if (Options.f_HasListItem("TestResults", "Display", "UseColor"))
					RunOptions.m_ReportFlags |= ETestReportFlag_UseColor;
				if (Options.f_HasListItem("TestResults", "Display", "DetailedPerformance"))
					RunOptions.m_ReportFlags |= ETestReportFlag_DetailedPerformance;
				if (Options.f_HasListItem("TestResults", "Display", "Ignored"))
					RunOptions.m_ReportFlags |= ETestReportFlag_Ignored;
				if (Options.f_HasListItem("TestResults", "Display", "DetailedMemory"))
					RunOptions.m_ReportFlags |= ETestReportFlag_DetailedMemory;
				if (Options.f_HasListItem("TestResults", "Display", "BreakOnFail"))
					RunOptions.m_ReportFlags |= ETestReportFlag_BreakOnFail;
				if (Options.f_HasListItem("TestResults", "Display", "ProcessRecursive"))
					RunOptions.m_ReportFlags |= ETestReportFlag_ProcessRecursive;
				if (Options.f_HasListItem("TestResults", "Display", "CompareToBaseline"))
					RunOptions.m_ReportFlags |= ETestReportFlag_CompareToBaseline;
				if (Options.f_HasListItem("TestResults", "Display", "CrashOnException"))
					RunOptions.m_ReportFlags |= ETestReportFlag_CrashOnException;


			}
		}
		if (RunOptions.m_ReportFlags == ETestReportFlag_None)
			RunOptions.m_ReportFlags = ETestReportFlag_Default;
		if (Options.f_IsSet("TestGroups"))
		{
			if (Options.f_HasList("TestGroups", "Groups"))
			{
				if (Options.f_HasListItem("TestGroups", "Groups", "Default"))
					RunOptions.m_IncludeGroups.f_Insert("");
				if (Options.f_HasListItem("TestGroups", "Groups", "Performance"))
					RunOptions.m_IncludeGroups.f_Insert("Performance");
				if (Options.f_HasListItem("TestGroups", "Groups", "Torture"))
					RunOptions.m_IncludeGroups.f_Insert("Torture");
				if (Options.f_HasListItem("TestGroups", "Groups", "Memory"))
					RunOptions.m_IncludeGroups.f_Insert("Memory");
				if (Options.f_HasListItem("TestGroups", "Groups", "Unfinished"))
					RunOptions.m_IncludeGroups.f_Insert("Unfinished");
				if (Options.f_HasListItem("TestGroups", "Groups", "Expensive"))
					RunOptions.m_IncludeGroups.f_Insert("Expensive");
				if (Options.f_HasListItem("TestGroups", "Groups", "Manual"))
					RunOptions.m_IncludeGroups.f_Insert("Manual");
				if (Options.f_HasListItem("TestGroups", "Groups", "SuperUser"))
					RunOptions.m_IncludeGroups.f_Insert("SuperUser");
				if (Options.f_HasListItem("TestGroups", "Groups", ""))
					RunOptions.m_IncludeGroups.f_Insert("");
			}
		}
		if (Options.f_IsSet("TestExcludeGroups"))
		{
			if (Options.f_HasList("TestExcludeGroups", "Groups"))
			{
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "Default"))
					RunOptions.m_ExcludeGroups.f_Insert("");
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "Performance"))
					RunOptions.m_ExcludeGroups.f_Insert("Performance");
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "Torture"))
					RunOptions.m_ExcludeGroups.f_Insert("Torture");
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "Memory"))
					RunOptions.m_ExcludeGroups.f_Insert("Memory");
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "Unfinished"))
					RunOptions.m_ExcludeGroups.f_Insert("Unfinished");
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "Expensive"))
					RunOptions.m_ExcludeGroups.f_Insert("Expensive");
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "Manual"))
					RunOptions.m_ExcludeGroups.f_Insert("Manual");
				if (Options.f_HasListItem("TestExcludeGroups", "Groups", "SuperUser"))
					RunOptions.m_ExcludeGroups.f_Insert("SuperUser");
				if (Options.f_HasListItem("TestGroups", "Groups", ""))
					RunOptions.m_IncludeGroups.f_Insert("");
			}
		}

		if (Options.f_IsSet("TestsList"))
		{
			if (Options.f_HasValue("TestsList", "Path"))
				RunOptions.m_Paths.f_Insert(fg_StrSplit(Options.f_GetValue("TestsList", "Path"), ";"));
			RunOptions.m_ReportFlags |= ETestReportFlag_ReportCategories;
			pResults = &CategoryResults;
		}
		else if (Options.f_IsSet("Tests"))
		{
			NContainer::TCVector<NStr::CStr> Paths;
			if (Options.f_HasValue("Tests", "Path"))
				RunOptions.m_Paths.f_Insert(fg_StrSplit(Options.f_GetValue("Tests", "Path"), ";"));
			//auto Path = Options.f_HasValue("TestsList", "Path") ? Options.f_GetValue("TestsList", "Path") : "";
		}

		if (Options.f_IsSet("TestData"))
		{
			if (Options.f_HasValue("TestData", "Data") )
				RunOptions.m_ExtraData = Options.f_GetValue("TestData", "Data");
		}
		if (Options.f_IsSet("TestLogger"))
		{
			if (Options.f_HasList("TestLogger", "Logger"))
			{
				if (Options.f_HasListItem("TestLogger", "Logger", "Default"))
					pResults = &DefaultResults;
				else if (Options.f_HasListItem("TestLogger", "Logger", "Brief"))
					pResults = &BriefResults;
				else if (Options.f_HasListItem("TestLogger", "Logger", "Registry"))
					pResults = &RegistryResults;
				else if (Options.f_HasListItem("TestLogger", "Logger", "Null"))
					pResults = &NullResults;
			}
		}
		return fg_RunTests(pResults, RunOptions);
	}

	void CTestResultParser::fp_HandleRecord(NStr::CStr const &_Text)
	{
		NContainer::CRegistry_CStr Registry;
		Registry.f_ParseStr(_Text);
		NContainer::CRegistry_CStr *pChild = nullptr;
		if ((pChild = Registry.f_GetChild("Header")))
		{
			f_HandleHeader(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("Footer")))
		{
			f_HandleFooter(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("Category")))
		{
			f_HandleCategory(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("Result")))
		{
			f_HandleResult(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("ResultsPerformance")))
		{
			f_HandlePerformanceResult(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("ResultsMemory")))
		{
			f_HandleMemoryResult(*pChild);
		}
		else
			DMibFastCheck(0);
	}

	void CTestResultParser::f_FeedText(NStr::CStr const &_Text)
	{
		m_TextBuffer += _Text;
		while (1)
		{
			ch8 const *pParse = m_TextBuffer;
			ch8 const *pParseStart = pParse;
			bint bFound = false;
			while (*pParse)
			{
				if (*pParse == '}')
				{
					++pParse;
					NStr::CStr FoundText = m_TextBuffer.f_Left(pParse - pParseStart);
					NStr::fg_ParseToEndOfLine(pParse);
					NStr::fg_ParseEndOfLine(pParse);
					m_TextBuffer = m_TextBuffer.f_Extract(pParse - pParseStart);
					bFound = true;
					fp_HandleRecord(FoundText);
					break;
				}
				NStr::fg_ParseToEndOfLine(pParse);
				NStr::fg_ParseEndOfLine(pParse);
			}
			if (!bFound)
				break;
		}
	}

	NStr::CStr CTestResultParser::fs_MeasureTypeToStr(ETestMeasureType _MeasureType)
	{
		return NPrivate::CRegistryTestResults::fs_MeasureTypeToStr(_MeasureType);
	}
	ETestMeasureType CTestResultParser::fs_MeasureTypeFromStr(NStr::CStr const &_MeasureType)
	{
		return NPrivate::CRegistryTestResults::fs_MeasureTypeFromStr(_MeasureType);
	}

	NStr::CStr CTestResultParser::fs_ReportFlagsToStr(ETestReportFlag _Flags)
	{
		return NPrivate::CRegistryTestResults::fs_ReportFlagsToStr(_Flags);
	}

	ETestReportFlag CTestResultParser::fs_ReportFlagsFromStr(NStr::CStr const &_Flags)
	{
		return NPrivate::CRegistryTestResults::fs_ReportFlagsFromStr(_Flags);
	}

	NStr::CStr CTestResultParser::fs_TestFlagsToStr(ETestFlag _Flags)
	{
		return NPrivate::CRegistryTestResults::fs_TestFlagsToStr(_Flags);
	}

	ETestFlag CTestResultParser::fs_TestFlagsFromStr(NStr::CStr const &_Flags)
	{
		return NPrivate::CRegistryTestResults::fs_TestFlagsFromStr(_Flags);
	}

	NStr::CStr CTestResultParser::fs_FailureActionToStr(ETest _Action)
	{
		return NPrivate::CRegistryTestResults::fs_FailureActionToStr(_Action);
	}

	ETest CTestResultParser::fs_FailureActionFromStr(NStr::CStr const &_Action)
	{
		return NPrivate::CRegistryTestResults::fs_FailureActionFromStr(_Action);
	}

	NStr::CStr CTestResultParser::fs_TestResultToStr(ETestResult _TestResult)
	{
		return NPrivate::CRegistryTestResults::fs_TestResultToStr(_TestResult);
	}

	ETestResult CTestResultParser::fs_TestResultFromStr(NStr::CStr const &_TestResult)
	{
		return NPrivate::CRegistryTestResults::fs_TestResultFromStr(_TestResult);
	}

	NStr::CStr CTestResultParser::fs_CheckTypeToStr(ECheckType _CheckType)
	{
		return NPrivate::CRegistryTestResults::fs_CheckTypeToStr(_CheckType);
	}

	ECheckType CTestResultParser::fs_CheckTypeFromStr(NStr::CStr const &_CheckType)
	{
		return NPrivate::CRegistryTestResults::fs_CheckTypeFromStr(_CheckType);
	}

	void CTestResultParser::fs_DecodeMemoryResults(NContainer::CRegistry_CStr const &_Registry, CTestMemoryResults &_Results)
	{
		return NPrivate::CRegistryTestResults::fs_DecodeMemoryResults(_Registry, _Results);
	}
	void CTestResultParser::fs_DecodePerformanceResults(NContainer::CRegistry_CStr const &_Registry, CTestPerformanceResults &_Results)
	{
		return NPrivate::CRegistryTestResults::fs_DecodePerformanceResults(_Registry, _Results);
	}
	void CTestResultParser::fs_DecodeResult(NContainer::CRegistry_CStr const &_Registry, CTestResult &_Results)
	{
		return NPrivate::CRegistryTestResults::fs_DecodeResult(_Registry, _Results);
	}

	bint fg_GroupActive(NStr::CStr const &_Group)
	{
#if DMibConfig_Tests_Enable
		return NPrivate::fg_GroupActive(_Group);
#else
		return false;
#endif
	}
}
