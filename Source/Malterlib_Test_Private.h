// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Container/Registry>

namespace NMib::NTest::NPrivate
{
#if DMibConfig_Tests_Enable
	class CReportTestAbortException
	{
		const static uint32 mcp_Magic = 0xF538CB10;
	public:
		uint32 m_Magic;
		CReportTestAbortException()
			: m_Magic(mcp_Magic)
		{
		}
		bool f_IsValid() const
		{
			return m_Magic == mcp_Magic;
		}

	};

	class CTestExceptionFilter : public NException::CExceptionFilter
	{
	public:
		CTestExceptionFilter()
			: m_pStackTraceInfo(nullptr)
		{
		}
		~CTestExceptionFilter()
		{
			if (m_pStackTraceInfo)
				NSys::fg_Debug_ReleaseStackTraceInfo(m_pStackTraceInfo);
			f_SetDumpFiles(fg_Default());
		}
		NContainer::TCVector<NStr::CStr> m_DumpFiles;
		CStackTraceInfo *m_pStackTraceInfo;
		void f_Exception(void *_pExceptionData);
		void f_SetDumpFiles(const NContainer::TCVector<NStr::CStr> &_Files);
		NContainer::TCVector<NStr::CStr> f_DetachDumpFiles();
		const ch8 *f_GetFile();
		int32 f_GetLine();
	};


	class CTestExceptionNoFilter : public NException::CExceptionFilter
	{
	public:
		~CTestExceptionNoFilter()
		{
		}
		void f_Exception(void *_pExceptionData)
		{
		}
	};

	enum ETestResultReportFlag
	{
		ETestResultReportFlag_None = 0
		, ETestResultReportFlag_Report = DMibBit(0)
		, ETestResultReportFlag_AskReport = DMibBit(1)
		, ETestResultReportFlag_Abort = DMibBit(2)
	};

	void fg_SetTestLastLocation(const ch8 *_pFile, int32 _Line);
	void fg_GetTestLastLocation(const ch8 *&o_pFile, int32 &o_Line);
	uint32 fg_RunTests(const NStr::CStr &_Path, CTestResults *_pResults);
	NStr::CStr fg_PushCategory(const NStr::CStr &_Category);
	void fg_InsideTestSuite(bool _bInsideTestSuite);
	bool fg_InsideTestSuite();
	bool fg_SetEnableValues(bool _bEnableValues);
	bool fg_GetEnableValues();
	NStr::CStr const &fg_GetDynamicValue();
	void fg_SetDynamicValue(NStr::CStr &&_Value);
	bool fg_SetEnableExceptionFilter(bool _bEnableExceptionFilter);
	bool fg_GetEnableExceptionFilter();
	void fg_PopCategory(NStr::CStr const &PreviousPath);
	NContainer::TCMap<NStr::CStr> fg_SetGroups(const NContainer::TCMap<NStr::CStr> &_Groups);
	NContainer::TCMap<NStr::CStr> fg_GetGroups();
	NContainer::TCVector<NStr::CStr> fg_DumpTestException();
	void fg_ReportTestException(const NContainer::TCVector<NStr::CStr> &_DumpFiles, const ch8 *_pFile, int32 _Line);
	CTestResults &fg_GetResultReporter();
	CTestResults *fg_SetResultReporter(CTestResults *_pNewReporter);
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
			, ETestResultReportFlag _ReportOptions
		)
	;
	ETestResultReportFlag fg_NeedReport
		(
			NStr::CStr const &_Description
			, ETestResult _Result
			, ETest _FailureAction
			, ECheckType _CheckType
			, const ch8 *_pFile
			, int32 _Line
			, ETestFlag _Flags
		)
	;
	bool fg_ShouldRunSubTest(bool _bLeaf);
	void fg_StepIntoSuites(bool _Step);

	class CTestReporterScope
	{
	public:
		CTestReporterScope(CTestResults &_NewResults)
		{
			m_pOldResults = fg_SetResultReporter(&_NewResults);
		}
		~CTestReporterScope()
		{
			fg_SetResultReporter(m_pOldResults);
		}

	private:
		DMibThreadLocalScopeDebugMember;
		CTestResults *m_pOldResults;
	};

	class CTest;

	DMibTypeTraitsImplement_MemberTraits(f_TestReport);
	DMibTypeTraitsImplement_MemberTraits(f_IsIgnored);
	DMibTypeTraitsImplement_MemberTraits(f_GetVariable);
	DMibTypeTraitsImplement_MemberTraits(f_ModifyDescription);
	DMibTypeTraitsImplement_MemberTraits(f_GetExtraData);

	struct CDummyExpression
	{
	};

	template <bool t_bFailureOnly, typename t_CExpression = CDummyExpression>
	class TCTestFunctionHelper
	{
	public:
		template <typename tf_CExpression>
		static auto fsp_DoReportReport(tf_CExpression const &_Expression)
		-> typename TCEnableIf
		<
			TCHasMember_f_TestReport<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, void
		>::CType
		{
			_Expression.f_GetVariable().f_TestReport(fg_GetResultReporter());
		}
		template <typename tf_CExpression>
		static auto fsp_DoReportReport(tf_CExpression const &_Expression)
		-> typename TCEnableIf
		<
			!TCHasMember_f_TestReport<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, void
		>::CType
		{

		}

		template <typename tf_CExpression>
		static auto fsp_IsIgnored(tf_CExpression const &_Expression)
		-> typename TCEnableIf
		<
			TCHasMember_f_IsIgnored<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, bool
		>::CType
		{
			return _Expression.f_GetVariable().f_IsIgnored();
		}
		template <typename tf_CExpression>
		static auto fsp_IsIgnored(tf_CExpression const &_Expression)
		-> typename TCEnableIf
		<
			!TCHasMember_f_IsIgnored<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, bool
		>::CType
		{
			return false;
		}

		template <typename tf_CExpression>
		static auto fsp_ModifyDescription(tf_CExpression const &_Expression, NStr::CStr const &_Description)
		-> typename TCEnableIf
		<
			TCHasMember_f_ModifyDescription<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, NStr::CStr
		>::CType
		{
			return _Expression.f_GetVariable().f_ModifyDescription(_Description);
		}
		template <typename tf_CExpression>
		static auto fsp_ModifyDescription(tf_CExpression const &_Expression, NStr::CStr const &_Description)
		-> typename TCEnableIf
		<
			!TCHasMember_f_ModifyDescription<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, NStr::CStr const &
		>::CType
		{
			return _Description;
		}

		template <typename tf_CExpression>
		static auto fsp_GetExtraData(tf_CExpression const &_Expression)
		-> typename TCEnableIf
		<
			TCHasMember_f_GetExtraData<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, NStr::CStr
		>::CType
		{
			return _Expression.f_GetVariable().f_GetExtraData();
		}
		template <typename tf_CExpression>
		static auto fsp_GetExtraData(tf_CExpression const &_Expression)
		-> typename TCEnableIf
		<
			!TCHasMember_f_GetExtraData<typename TCIsMemberCallableWith_f_GetVariable<tf_CExpression, void ()>::CReturnType>::mc_Value
			, NStr::CStr
		>::CType
		{
			return NStr::CStr();
		}


		t_CExpression m_Expression;
		ETest m_FailureAction;
		ETestFlag m_Flags;
		const ch8 *m_pFile;
		int32 m_Line;
		NStr::CStr m_CustomMessage;
		bool m_bValid;
		bool m_bOnlyCustom;

		template <typename tf_CExpression>
		typename TCEnableIf<NTraits::TCIsSame<tf_CExpression, bool>::mc_Value, bool>::CType fp_Evaluate() const
		{
			if (t_bFailureOnly && m_Expression)
				return m_Expression;
			ETestResultReportFlag ReportFlags = fg_NeedReport(m_CustomMessage, (ETestResult)m_Expression, m_FailureAction, ECheckType_Message, m_pFile, m_Line, m_Flags);
			if (ReportFlags)
			{
				fg_ReportTestResult
					(
						(ETestResult)m_Expression
						, m_CustomMessage
						, ""
						, m_FailureAction
						, ECheckType_Message
						, m_pFile
						, m_Line
						, NStr::CStr()
						, m_Flags
						, ReportFlags
					)
				;
			}
			return m_Expression;
		}

		template <typename tf_CExpression>
		typename TCDisableIf<NTraits::TCIsSame<tf_CExpression, bool>::mc_Value, bool>::CType fp_Evaluate() const
		{
			ETestResult Result;
			Result = (ETestResult)m_Expression.f_Eval();
			if (fsp_IsIgnored(m_Expression))
				Result = ETestResult_Ignored;
			if (t_bFailureOnly && Result != ETestResult_Fail)
				return Result != ETestResult_Fail;
			NStr::CStr Desc = fsp_ModifyDescription(m_Expression, m_Expression.f_GetDesc());
			if (!m_CustomMessage.f_IsEmpty())
			{
				if (m_bOnlyCustom)
					Desc = m_CustomMessage;
				else
				{
					Desc += " ";
					Desc += m_CustomMessage;
				}
			}
			ETestResultReportFlag ReportFlags = fg_NeedReport(Desc, Result, m_FailureAction, ECheckType_Predicate, m_pFile, m_Line, m_Flags);
			if (ReportFlags)
			{
				bool bIsSuccess =
					Result == ETestResult_Success
					|| ((Result == ETestResult_Fail) && (m_FailureAction == ETest_ExpectFail || m_FailureAction == ETest_ExpectFailAndStop))
				;
				bool bNeedValues =
					fg_GetEnableValues()
					&& !(m_Flags & ETestFlag_NoValues)
					&& !m_Expression.f_GetDisableValues()
					&&
					(
						!(m_Flags & ETestFlag_NoValuesOnSuccess)
						|| !bIsSuccess
					)
					&& (fg_TestReportFlags() & ETestReportFlag_ReportValues)
				;

				fg_ReportTestResult
					(
						Result
						, Desc
						, bNeedValues ? m_Expression.f_GetValueDesc() : NStr::CStr()
						, m_FailureAction
						, ECheckType_Predicate
						, m_pFile
						, m_Line
						, fsp_GetExtraData(m_Expression)
						, m_Flags
						, ReportFlags
					)
				;
				fsp_DoReportReport(m_Expression);
			}

			return Result != ETestResult_Fail;
		}

	public:

		TCTestFunctionHelper(const ch8 *_pFile, int32 _Line, t_CExpression &&_Expression)
			: m_pFile(_pFile)
			, m_Line(_Line)
			, m_Expression(fg_Move(_Expression))
			, m_FailureAction(ETest_Fail)
			, m_Flags(ETestFlag_None)
			, m_bValid(false)
			, m_bOnlyCustom(false)
		{
			fg_SetTestLastLocation(_pFile, _Line);
		}

		TCTestFunctionHelper(TCTestFunctionHelper &&_Right)
			: m_Expression(fg_Move(_Right.m_Expression))
			, m_FailureAction(_Right.m_FailureAction)
			, m_Flags(_Right.m_Flags)
			, m_pFile(_Right.m_pFile)
			, m_Line(_Right.m_Line)
			, m_CustomMessage(fg_Move(_Right.m_CustomMessage))
			, m_bValid(_Right.m_bValid)
			, m_bOnlyCustom(_Right.m_bOnlyCustom)
		{
		}

		template <bool tf_bFailureOnly, typename tf_CExpression>
		TCTestFunctionHelper(TCTestFunctionHelper<tf_bFailureOnly, tf_CExpression> &&_Right, t_CExpression const &_NewExpression)
			: m_Expression(_NewExpression)
			, m_FailureAction(_Right.m_FailureAction)
			, m_Flags(_Right.m_Flags)
			, m_pFile(_Right.m_pFile)
			, m_Line(_Right.m_Line)
			, m_CustomMessage(fg_Move(_Right.m_CustomMessage))
			, m_bValid(true)
			, m_bOnlyCustom(_Right.m_bOnlyCustom)
		{
		}

		~TCTestFunctionHelper()
		{
		}

		operator bool () const
		{
			DMibFastCheck(m_bValid);
			DMibFastCheck(NMib::NTest::NPrivate::fg_InsideTestSuite());
			if (m_Flags & ETestFlag_NoExceptionFilter)
			{
				NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
				DMibExceptionFilter(TestExceptionFilter);
				return fp_Evaluate<t_CExpression>();
			}
			else
				return fp_Evaluate<t_CExpression>();
		}

		inline_always TCTestFunctionHelper &operator () (ETest _FailureAction)
		{
			m_FailureAction = _FailureAction;
			return *this;
		}

		inline_always TCTestFunctionHelper &operator () (ETestFlag _Flags)
		{
			m_Flags |= _Flags;
			return *this;
		}

		inline_always TCTestFunctionHelper &operator () (const ch8 *_pMessage)
		{
			DMibFastCheck(m_CustomMessage.f_IsEmpty()); // Custom message already specified
			m_CustomMessage = _pMessage;
			if constexpr (NTraits::TCIsSame<t_CExpression, CDummyExpression>::mc_Value)
				m_bOnlyCustom = true;
			return *this;
		}

		inline_always TCTestFunctionHelper &operator () (NStr::CStr const &_Message)
		{
			DMibFastCheck(m_CustomMessage.f_IsEmpty()); // Custom message already specified
			m_CustomMessage = _Message;
			if constexpr (NTraits::TCIsSame<t_CExpression, CDummyExpression>::mc_Value)
				m_bOnlyCustom = true;
			return *this;
		}

		inline_always TCTestFunctionHelper &operator () (NStr::CStr &&_Message)
		{
			DMibFastCheck(m_CustomMessage.f_IsEmpty()); // Custom message already specified
			m_CustomMessage = fg_Move(_Message);
			if constexpr (NTraits::TCIsSame<t_CExpression, CDummyExpression>::mc_Value)
				m_bOnlyCustom = true;
			return *this;
		}

		template <typename tf_CExpression>
		inline TCTestFunctionHelper<t_bFailureOnly, tf_CExpression> operator () (const tf_CExpression &_Expression)
		{
			static_assert(NTraits::TCIsSame<t_CExpression, CDummyExpression>::mc_Value, "You can only specify expressions once");
			return TCTestFunctionHelper<t_bFailureOnly, tf_CExpression>(fg_Move(*this), _Expression);
		}

		inline TCTestFunctionHelper<t_bFailureOnly, bool> operator () (bool _bExpression)
		{
			static_assert(NTraits::TCIsSame<t_CExpression, CDummyExpression>::mc_Value, "You can only specify expressions once");
			return TCTestFunctionHelper<t_bFailureOnly, bool>(fg_Move(*this), _bExpression);
		}
	};

	class CTestPathScope
	{
	public:
		CTestPathScope(NStr::CStr const &_Path, ch8 const *_pFile, uint32 _Line)
		{
			fg_SetTestLastLocation(_pFile, _Line);
			m_PreviousPath = NMib::NTest::NPrivate::fg_PushCategory(_Path);
		}
		~CTestPathScope()
		{
			NMib::NTest::NPrivate::fg_PopCategory(m_PreviousPath);
		}

	private:
		DMibThreadLocalScopeDebugMember;
		NStr::CStr m_PreviousPath;
	};

	class CTestCategoryScope
	{
		bool f_ContinueEnumerating() const;
		void f_ReportLeafCategory();
	public:
		CTestCategoryScope(const CTestCategory &_Category, const ch8 *_pFile, int32 _Line, ETestCategoryFlag _Flags = ETestCategoryFlag_None)
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

		~CTestCategoryScope()
		{
			if (mp_Flags & (ETestCategoryFlag_DisableValues | ETestCategoryFlag_EnableValues))
				NMib::NTest::NPrivate::fg_SetEnableValues(mp_bOldEnableValues);
			if (mp_Flags & (ETestCategoryFlag_DisableExceptionFilter | ETestCategoryFlag_EnableExceptionFilter))
				NMib::NTest::NPrivate::fg_SetEnableExceptionFilter(mp_bOldEnableExceptionFilter);
			NMib::NTest::NPrivate::fg_InsideTestSuite(false);
		}

		void operator % (NFunction::TCFunction<void ()> const &_Function)
		{
			bool bLeaf = (mp_Flags & ETestCategoryFlag_Tests) != 0;
			NContainer::TCMap<NStr::CStr> const &Groups = mp_Category.f_GetGroups();
			NContainer::TCMap<NStr::CStr> OldGroups;

			NStr::CStr PreviousPath = NMib::NTest::NPrivate::fg_PushCategory(mp_Category.f_GetCategory());
			if (!Groups.f_IsEmpty())
				OldGroups = NMib::NTest::NPrivate::fg_SetGroups(mp_Category.f_GetGroups());

			{
				if (NMib::NTest::NPrivate::fg_GetEnableExceptionFilter())
				{
					NMib::NTest::NPrivate::CTestExceptionFilter TestExceptionFilter;
					DMibExceptionFilter(TestExceptionFilter);
					if (fg_TestReportFlags() & ETestReportFlag_CrashOnException)
					{
						try
						{
							if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
							{
								if (f_ContinueEnumerating())
									_Function();
								if (bLeaf)
									f_ReportLeafCategory();
							}
						}
						catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
						{
						}
					}
					else
					{
						try
						{
							if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
							{
								if (f_ContinueEnumerating())
									_Function();
								if (bLeaf)
									f_ReportLeafCategory();
							}
						}
						catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
						{
						}
						catch (...)
						{
							NMib::NTest::NPrivate::fg_ReportTestException
								(
									TestExceptionFilter.f_DetachDumpFiles()
									, TestExceptionFilter.f_GetFile() ? TestExceptionFilter.f_GetFile()
									: mp_pFile, TestExceptionFilter.f_GetFile() ? TestExceptionFilter.f_GetLine()
									: mp_Line
								)
							;
						}
					}
				}
				else
				{
					NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
					DMibExceptionFilter(TestExceptionFilter);
					if (fg_TestReportFlags() & ETestReportFlag_CrashOnException)
					{
						try
						{
							if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
							{
								if (f_ContinueEnumerating())
									_Function();
								if (bLeaf)
									f_ReportLeafCategory();
							}
						}
						catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
						{
						}
					}
					else
					{
						try
						{
							if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
							{
								if (f_ContinueEnumerating())
									_Function();
								if (bLeaf)
									f_ReportLeafCategory();
							}
						}
						catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
						{
						}
						catch (...)
						{
							NMib::NTest::NPrivate::fg_ReportTestException(NContainer::TCVector<NStr::CStr>(), mp_pFile, mp_Line);
						}
					}
				}
			}

			if (!Groups.f_IsEmpty())
				NMib::NTest::NPrivate::fg_SetGroups(OldGroups);
			NMib::NTest::NPrivate::fg_PopCategory(PreviousPath);
		}

	private:
		DMibThreadLocalScopeDebugMember;
		CTestCategory mp_Category;
		bool mp_bOldEnableValues;
		bool mp_bOldEnableExceptionFilter;
		const ch8 *mp_pFile;
		int32 mp_Line;
		ETestCategoryFlag mp_Flags;
	};
#endif
}
