// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Test.h"

namespace NMib::NTest
{
	class CRegistryTestResults : public CTestResults
	{
		ETestReportFlag mp_ReportFlags;

		NAtomic::TCAtomic<int32> m_NextID;
		NThread::TCThreadLocal<uint32> m_CurrentID;

		static NContainer::CRegistry *fsp_AddStatistics(NContainer::CRegistry *_pParentReg, NStr::CStr const &_Name, CTestStats const &_Stats);
		static NContainer::CRegistry *fsp_AddMemoryStatistics(NContainer::CRegistry *_pParentReg, NStr::CStr const &_Name, CTestMemoryStats const &_Stats);

		static void fsp_DecodeStatistics(NContainer::CRegistry const &_ParentReg, CTestStats &_Stats);
		static void fsp_DecodeMemoryStatistics(NContainer::CRegistry const &_ParentReg, CTestMemoryStats &_Stats);

	public:

		static NStr::CStr fs_MeasureTypeToStr(ETestMeasureType _MeasureType);
		static ETestMeasureType fs_MeasureTypeFromStr(NStr::CStr const &_MeasureType);
		static NStr::CStr fs_ReportFlagsToStr(ETestReportFlag _Flags);
		static ETestReportFlag fs_ReportFlagsFromStr(NStr::CStr const &_Flags);
		static NStr::CStr fs_TestFlagsToStr(ETestFlag _Flags);
		static ETestFlag fs_TestFlagsFromStr(NStr::CStr const &_Flags);
		static NStr::CStr fs_FailureActionToStr(ETest _Action);
		static ETest fs_FailureActionFromStr(NStr::CStr const &_Action);
		static NStr::CStr fs_TestResultToStr(ETestResult _TestResult);
		static ETestResult fs_TestResultFromStr(NStr::CStr const &_TestResult);
		static NStr::CStr fs_CheckTypeToStr(ECheckType _CheckType);
		static ECheckType fs_CheckTypeFromStr(NStr::CStr const &_CheckType);
		static void fs_DecodeMemoryResults(NContainer::CRegistry const &_Registry, CTestMemoryResults &_Results);
		static void fs_DecodePerformanceResults(NContainer::CRegistry const &_Registry, CTestPerformanceResults &_Results);
		static void fs_DecodeResult(NContainer::CRegistry const &_Registry, CTestResult &_Results);

		void f_ReportHeader(ETestReportFlag _ReportFlags) override;
		void f_ReportFooter(mint _nTestsTotal, mint _nSuccessful, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored) override;
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
	};
}
