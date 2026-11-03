// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Container/Registry>

namespace NMib::NTest::NPrivate
{
#if DMibConfig_Tests_Enable
	struct CReportTestAbortException
	{
		uint32 m_Magic = mcp_Magic;
		CReportTestAbortException();

		bool f_IsValid() const;

	private:
		static constexpr uint32 mcp_Magic = 0xF538CB10;
	};

	struct CTestExceptionFilter : public NException::CExceptionFilter
	{
		CTestExceptionFilter();
		~CTestExceptionFilter();

		NContainer::TCVector<NStr::CStr> m_DumpFiles;
		CStackTraceInfo *m_pStackTraceInfo;

		void f_Exception(void *_pExceptionData);
		void f_SetDumpFiles(NContainer::TCVector<NStr::CStr> const &_Files);
		NContainer::TCVector<NStr::CStr> f_DetachDumpFiles();
		ch8 const *f_GetFile();
		int32 f_GetLine();
	};

	struct CTestExceptionNoFilter : public NException::CExceptionFilter
	{
		~CTestExceptionNoFilter();

		void f_Exception(void *_pExceptionData);
	};

	enum ETestResultReportFlag
	{
		ETestResultReportFlag_None            = 0
		, ETestResultReportFlag_Report        = DMibBit(0)
		, ETestResultReportFlag_AskReport     = DMibBit(1)
		, ETestResultReportFlag_Abort         = DMibBit(2)
		, ETestResultReportFlag_FromException = DMibBit(3)
	};

	void fg_SetTestLastLocation(ch8 const *_pFile, int32 _Line);
	void fg_GetTestLastLocation(ch8 const * &o_pFile, int32 &o_Line);
	uint32 fg_RunTests(NStr::CStr const &_Path, CTestResults *_pResults);
	NStr::CStr fg_PushCategory(NStr::CStr const &_Category);
	void fg_InsideTestSuite(bool _bInsideTestSuite);
	bool fg_InsideTestSuite();
	bool fg_SetEnableValues(bool _bEnableValues);
	bool fg_GetEnableValues();
	void fg_RegisterDynamicValue(NStr::CStr *_pValue);
	void fg_SetDynamicValue(NStr::CStr &&_Value);
	bool fg_SetEnableExceptionFilter(bool _bEnableExceptionFilter);
	bool fg_GetEnableExceptionFilter();
	void fg_PopCategory(NStr::CStr const &_PreviousPath);
	NStr::CStr fg_GetExceptionCategoryPath();
	NContainer::TCSet<NStr::CStr> fg_SetGroups(NContainer::TCSet<NStr::CStr> const &_Groups);
	NContainer::TCSet<NStr::CStr> fg_GetGroups();
	NContainer::TCVector<NStr::CStr> fg_DumpTestException();
	void fg_ReportTestException(NContainer::TCVector<NStr::CStr> const &_DumpFiles, ch8 const *_pFile, int32 _Line);
	CTestResults &fg_GetResultReporter();
	CTestResults *fg_SetResultReporter(CTestResults *_pNewReporter);
	void fg_ReportTestResult
		(
			ETestResult _Result
			, NStr::CStr const &_Description
			, NStr::CStr const &_Values
			, ETest _FailureAction
			, ECheckType _CheckType
			, NStr::CStr const &_File
			, int32 _Line
			, NStr::CStr const &_ExtraMultiLineReportData
			, ETestFlag _Flags
			, ETestResultReportFlag _ReportOptions
		)
	;
	ETestResultReportFlag fg_NeedReport(NStr::CStr const &_Description, ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ch8 const *_pFile, int32 _Line, ETestFlag _Flags);
	bool fg_ShouldRunSubTest(bool _bLeaf);
	void fg_StepIntoSuites(bool _bStep);

	struct CTestReporterScope
	{
		CTestReporterScope(CTestResults &_NewResults);
		~CTestReporterScope();

	private:
		DMibThreadLocalScopeDebugMember;
		CTestResults *mp_pOldResults;
	};

	struct CTest;

	struct CDummyExpression
	{
	};

	template <bool t_bFailureOnly, typename t_CExpression = CDummyExpression>
	struct TCTestFunctionHelper
	{
		t_CExpression m_Expression;

	private:
		bool fp_Evaluate() const
#	ifndef DCompiler_MSVC_Workaround
			requires NTraits::cIsSame<t_CExpression, bool> || requires ()
			{
				this->m_Expression.f_Eval(nullptr);
			}
#	endif
		;

	public:
		TCTestFunctionHelper(ch8 const *_pFile, int32 _Line, t_CExpression &&_Expression);
		TCTestFunctionHelper(TCTestFunctionHelper &&_Right);
		template <bool tf_bFailureOnly, typename tf_CExpression>
		TCTestFunctionHelper(TCTestFunctionHelper<tf_bFailureOnly, tf_CExpression> &&_Right, t_CExpression const &_NewExpression);
		~TCTestFunctionHelper();

		operator bool () const
#	ifndef DCompiler_MSVC_Workaround
			requires requires ()
			{
				fp_Evaluate();
			}
#	endif
		;

		TCTestFunctionHelper &operator () (ETest _FailureAction);
		TCTestFunctionHelper &operator () (ETestFlag _Flags);
		TCTestFunctionHelper &operator () (ch8 const *_pMessage);
		TCTestFunctionHelper &operator () (NStr::CStr const &_Message);
		TCTestFunctionHelper &operator () (NStr::CStr &&_Message);

		template <typename tf_CExpression>
		TCTestFunctionHelper<t_bFailureOnly, tf_CExpression> operator () (tf_CExpression const &_Expression);
		TCTestFunctionHelper<t_bFailureOnly, bool> operator () (bool _bExpression);

		ETest m_FailureAction;
		ETestFlag m_Flags;
		ch8 const *m_pFile;
		int32 m_Line;
		NStr::CStr m_CustomMessage;
		bool m_bValid;
		bool m_bOnlyCustom;
	};

	struct CTestPathScope : public CCoroutineThreadLocalHandler
	{
		CTestPathScope(NStr::CStr const &_Path, ch8 const *_pFile, uint32 _Line);
		~CTestPathScope();

		void f_Suspend() noexcept override;
		void f_ResumeNoExcept() noexcept override;

	private:
		NStr::CStr mp_PreviousPath;
		NStr::CStr mp_Path;
		ch8 const *mp_pFile;
		uint32 mp_Line;
	};

	NStr::CStr fg_GetSuiteLockPath(NStr::CStr const &_Suite);

	struct CTestCategoryScope
	{
		CTestCategoryScope(CTestCategory const &_Category, ch8 const *_pFile, int32 _Line, ETestCategoryFlag _Flags = ETestCategoryFlag_None);
		~CTestCategoryScope();

		template <typename tf_FFunction>
		void operator % (tf_FFunction &&_fCategory);

		void f_ProcessAsyncCategory(NFunction::TCFunctionMovable<NConcurrency::TCFuture<void> ()> &&_fFunction);
		void f_ProcessCategory(NFunction::TCFunctionMovable<void ()> &&_fFunction);

	private:
		bool fp_ContinueEnumerating() const;
		void fp_ReportLeafCategory();
		void fp_LockSuite();

		DMibThreadLocalScopeDebugMember;
		CTestCategory mp_Category;
		bool mp_bOldEnableValues;
		bool mp_bOldEnableExceptionFilter;
		ch8 const *mp_pFile;
		int32 mp_Line;
		ETestCategoryFlag mp_Flags;
	};
#endif
}

#include "Malterlib_Test_Private.hpp"
