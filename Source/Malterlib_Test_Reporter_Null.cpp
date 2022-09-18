// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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

	void CNullTestResults::f_ReportFooter(mint _nTestsTotal, mint _nSuccessful, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored)
	{
	}

	ETestNeedReportFlag CNullTestResults::f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags)
	{
		return ETestNeedReportFlag_None;
	}

	void CNullTestResults::f_ReportResult
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
		)
	{
	}

	void CNullTestResults::f_ReportSuite(const NStr::CStr &_TestPath, const NContainer::TCSet<NStr::CStr> &_TestGroups, CTestLocation const &_Location)
	{
	}

	void CNullTestResults::f_PerformanceResults(CTestPerformanceResults const &_Results)
	{
	}

	void CNullTestResults::f_MemoryResults(CTestMemoryResults const &_Results)
	{
	}
}
