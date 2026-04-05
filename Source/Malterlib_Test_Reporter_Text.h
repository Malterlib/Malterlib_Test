// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Test.h"

#include <Mib/CommandLine/AnsiEncoding>

namespace NMib::NTest
{
	enum ETestSeverity
	{
		ETestSeverity_None
		, ETestSeverity_Success
		, ETestSeverity_Warning
		, ETestSeverity_Error
	};

	class CTextTestResults : public CTestResults
	{
	public:
		CTextTestResults(NCommandLine::EAnsiEncodingFlag _AnsiEncodingFlags);

		ETestReportFlag f_GetReportFlags() override
		{
			return mp_ReportFlags;
		}
		void f_ReportHeader(ETestReportFlag _ReportFlags) override;
		void f_ReportFooter(umint _nTestsTotal, umint _nSuccessful, umint _nSuccessUnexpected, umint _nFailed, umint _nExpectFailed, umint _nWarnings, umint _nIgnored) override;
		ETestNeedReportFlag f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags) override;
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
		void f_PerformanceResults(CTestPerformanceResults const &_Results) override;
		void f_MemoryResults(CTestMemoryResults const &_Results) override;

	protected:
		virtual void fp_AddReport
			(
				const NStr::CStr &_FileLocation
				, const NStr::CStr &_MessageStr
				, const NStr::CStr &_TestPath
				, const NStr::CStr &_Message
				, const NStr::CStr &_Values
				, ETestSeverity _Severity
				, bool _bHeader
			)
		;
		virtual void fp_ReportText(const NStr::CStr &_Text, ETestSeverity _Severity) = 0;
		NStr::CStr fp_ColorSeverity(NStr::CStr const &_ToColor, ETestSeverity _Severity) const;
		NStr::CStr fp_ColorPath(NStr::CStr const &_String) const;
		NStr::CStr fp_ColorExpression(NStr::CStr const &_String) const;
		NStr::CStr fp_ColorValues(NStr::CStr const &_String) const;
		NStr::CStr fp_ColorFileLocation(NStr::CStr const &_String) const;
		NStr::CStr fp_ColorHeading(NStr::CStr const &_String) const;

		NCommandLine::EAnsiEncodingFlag mp_AnsiEncodingFlags;
		ETestReportFlag mp_ReportFlags;
	};

	class CTextTestResultsBrief : public CTextTestResults
	{
	public:
		CTextTestResultsBrief(NCommandLine::EAnsiEncodingFlag _AnsiEncodingFlags);

		void fp_AddReport
			(
				const NStr::CStr &_FileLocation
				, const NStr::CStr &_MessageStr
				, const NStr::CStr &_TestPath
				, const NStr::CStr &_Message
				, const NStr::CStr &_Values
				, ETestSeverity _Severity
				, bool _bHeader
			)
			override
		;
	};
}
