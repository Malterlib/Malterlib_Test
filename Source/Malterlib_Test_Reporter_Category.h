// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Test.h"

namespace NMib::NTest::NPrivate
{
	struct CCategoryLister : CTestResults
	{
		void f_ReportHeader(ETestReportFlag _ReportFlags) override;
		void f_ReportFooter(mint _nTestsTotal, mint _nSuccess, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored) override;
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
