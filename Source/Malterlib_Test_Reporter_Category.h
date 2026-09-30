// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Test.h"

namespace NMib::NTest::NPrivate
{
	struct CCategoryLister : CTestResults
	{
		void f_ReportHeader(ETestReportFlag _ReportFlags) override;
		void f_ReportFooter(umint _nTestsTotal, umint _nSuccess, umint _nSuccessUnexpected, umint _nFailed, umint _nExpectFailed, umint _nWarnings, umint _nIgnored) override;
		void f_ReportResult
			(
				ETestResult _Result
				, NStr::CStr const &_TestPath
				, NStr::CStr const &_Message
				, NStr::CStr const &_Values
				, CTestLocation const &_Location
				, ETest _FailureAction
				, ECheckType _CheckType
				, ETestFlag _Flags
				, NStr::CStr const &_ExtraMultiLineReportData
			)
			override
		;
		void f_ReportSuite(NStr::CStr const &_TestPath, NContainer::TCSet<NStr::CStr> const &_TestGroups, CTestLocation const &_Location) override;
		ETestNeedReportFlag f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags) override;
		void f_PerformanceResults(CTestPerformanceResults const &_Results) override;
		void f_MemoryResults(CTestMemoryResults const &_Results) override;
	};
}
