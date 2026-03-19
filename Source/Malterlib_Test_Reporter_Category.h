// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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
				, const NStr::CStr &_TestPath
				, const NStr::CStr &_Message
				, const NStr::CStr &_Values
				, CTestLocation const &_Location
				, ETest _FailureAction
				, ECheckType _CheckType
				, ETestFlag _Flags
				, const NStr::CStr &_ExtraMultiLineReportData
			) override
		;
		void f_ReportSuite(const NStr::CStr &_TestPath, const NContainer::TCSet<NStr::CStr> &_TestGroups, CTestLocation const &_Location) override;
		ETestNeedReportFlag f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags) override;
		void f_PerformanceResults(CTestPerformanceResults const &_Results) override;
		void f_MemoryResults(CTestMemoryResults const &_Results) override;
	};
}
