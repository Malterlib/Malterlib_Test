// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Test>

#include "Malterlib_Test_Reporter_Category.h"

namespace NMib::NTest::NPrivate
{
	void CCategoryLister::f_ReportHeader(ETestReportFlag _ReportFlags)
	{
	}

	void CCategoryLister::f_ReportFooter(umint _nTestsTotal, umint _nSuccess, umint _nSuccessUnexpected, umint _nFailed, umint _nExpectFailed, umint _nWarnings, umint _nIgnored)
	{
	}

	void CCategoryLister::f_ReportResult
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

	void CCategoryLister::f_ReportSuite(NStr::CStr const &_TestPath, NContainer::TCSet<NStr::CStr> const &_TestGroups, CTestLocation const &_Location)
	{
		NStr::CStr Groups;
		NMisc::fg_ForEach
			(
				_TestGroups
				, [&](NStr::CStr const &_Group)
				{
					NStr::fg_AddStrSep(Groups, _Group, ',');
				}
			)
		;
		if (Groups.f_IsEmpty())
			DMibConOut("{}" DMibNewLine, _TestPath);
		else
			DMibConOut("{} : {}" DMibNewLine, _TestPath, Groups);
	}

	ETestNeedReportFlag CCategoryLister::f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags)
	{
		return ETestNeedReportFlag_Count | ETestNeedReportFlag_Report;
	}

	void CCategoryLister::f_PerformanceResults(CTestPerformanceResults const &_Results)
	{
	}

	void CCategoryLister::f_MemoryResults(CTestMemoryResults const &_Results)
	{
	}
}
