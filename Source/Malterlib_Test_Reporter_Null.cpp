// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Test>

#include "Malterlib_Test_Reporter_Null.h"

namespace NMib::NTest::NPrivate
{
	ETestReportFlag CNullTestResults::f_GetReportFlags()
	{
		return mp_ReportFlags;
	}

	void CNullTestResults::f_ReportHeader(ETestReportFlag _ReportFlags)
	{
		mp_ReportFlags = _ReportFlags;
	}

	void CNullTestResults::f_ReportFooter(umint _nTestsTotal, umint _nSuccessful, umint _nSuccessUnexpected, umint _nFailed, umint _nExpectFailed, umint _nWarnings, umint _nIgnored)
	{
	}

	ETestNeedReportFlag CNullTestResults::f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags)
	{
		return ETestNeedReportFlag_None;
	}

	void CNullTestResults::f_ReportResult
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
	{
	}

	void CNullTestResults::f_ReportSuite(NStr::CStr const &_TestPath, NContainer::TCSet<NStr::CStr> const &_TestGroups, CTestLocation const &_Location)
	{
	}

	void CNullTestResults::f_PerformanceResults(CTestPerformanceResults const &_Results)
	{
	}

	void CNullTestResults::f_MemoryResults(CTestMemoryResults const &_Results)
	{
	}
}
