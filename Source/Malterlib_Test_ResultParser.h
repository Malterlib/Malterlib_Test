// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Test.h"

namespace NMib::NTest
{
	class CTestResultParser
	{
		NStr::CStr m_TextBuffer;
		void fp_HandleRecord(NStr::CStr const &_Text);
	public:
		virtual void f_HandleHeader(NContainer::CRegistry const &_Reg) = 0;
		virtual void f_HandleFooter(NContainer::CRegistry const &_Reg) = 0;
		virtual void f_HandleCategory(NContainer::CRegistry const &_Reg) = 0;
		virtual void f_HandleResult(NContainer::CRegistry const &_Reg) = 0;
		virtual void f_HandlePerformanceResult(NContainer::CRegistry const &_Reg) = 0;
		virtual void f_HandleMemoryResult(NContainer::CRegistry const &_Reg) = 0;
		void f_FeedText(NStr::CStr const &_Text);

		static NStr::CStr fs_MeasureTypeToStr(ETestMeasureType _MeasureType);
		static ETestMeasureType fs_MeasureTypeFromStr(NStr::CStr const &_MeasureType);
		static NStr::CStr fs_ReportFlagsToStr(ETestReportFlag _Flags);
		static ETestReportFlag fs_ReportFlagsFromStr(NStr::CStr const &_Flags);
		static NStr::CStr fs_FailureActionToStr(ETest _Action);
		static ETest fs_FailureActionFromStr(NStr::CStr const &_Action);
		static NStr::CStr fs_TestResultToStr(ETestResult _TestResult);
		static ETestResult fs_TestResultFromStr(NStr::CStr const &_TestResult);
		static NStr::CStr fs_CheckTypeToStr(ECheckType _CheckType);
		static ECheckType fs_CheckTypeFromStr(NStr::CStr const &_CheckType);
		static NStr::CStr fs_TestFlagsToStr(ETestFlag _Flags);
		static ETestFlag fs_TestFlagsFromStr(NStr::CStr const &_Flags);

		static void fs_DecodeMemoryResults(NContainer::CRegistry const &_Registry, CTestMemoryResults &_Results);
		static void fs_DecodePerformanceResults(NContainer::CRegistry const &_Registry, CTestPerformanceResults &_Results);
		static void fs_DecodeResult(NContainer::CRegistry const &_Registry, CTestResult &_Results);
	};
}
