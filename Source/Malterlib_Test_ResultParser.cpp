// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Test>
#include <Mib/Test/ReporterRegistry>

#include "Malterlib_Test_ResultParser.h"

namespace NMib::NTest
{
	void CTestResultParser::fp_HandleRecord(NStr::CStr const &_Text)
	{
		NContainer::CRegistry Registry;
		Registry.f_ParseStr(_Text);
		NContainer::CRegistry *pChild = nullptr;
		if ((pChild = Registry.f_GetChild("Header")))
		{
			f_HandleHeader(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("Footer")))
		{
			f_HandleFooter(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("Category")))
		{
			f_HandleCategory(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("Result")))
		{
			f_HandleResult(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("ResultsPerformance")))
		{
			f_HandlePerformanceResult(*pChild);
		}
		else if ((pChild = Registry.f_GetChild("ResultsMemory")))
		{
			f_HandleMemoryResult(*pChild);
		}
		else
			DMibFastCheck(0);
	}

	void CTestResultParser::f_FeedText(NStr::CStr const &_Text)
	{
		m_TextBuffer += _Text;
		while (1)
		{
			ch8 const *pParse = m_TextBuffer;
			ch8 const *pParseStart = pParse;
			bool bFound = false;
			while (*pParse)
			{
				if (*pParse == '}')
				{
					++pParse;
					NStr::CStr FoundText = m_TextBuffer.f_Left(pParse - pParseStart);
					NStr::fg_ParseToEndOfLine(pParse);
					NStr::fg_ParseEndOfLine(pParse);
					m_TextBuffer = m_TextBuffer.f_Extract(pParse - pParseStart);
					bFound = true;
					fp_HandleRecord(FoundText);
					break;
				}
				NStr::fg_ParseToEndOfLine(pParse);
				NStr::fg_ParseEndOfLine(pParse);
			}
			if (!bFound)
				break;
		}
	}

	NStr::CStr CTestResultParser::fs_MeasureTypeToStr(ETestMeasureType _MeasureType)
	{
		return CRegistryTestResults::fs_MeasureTypeToStr(_MeasureType);
	}

	ETestMeasureType CTestResultParser::fs_MeasureTypeFromStr(NStr::CStr const &_MeasureType)
	{
		return CRegistryTestResults::fs_MeasureTypeFromStr(_MeasureType);
	}

	NStr::CStr CTestResultParser::fs_ReportFlagsToStr(ETestReportFlag _Flags)
	{
		return CRegistryTestResults::fs_ReportFlagsToStr(_Flags);
	}

	ETestReportFlag CTestResultParser::fs_ReportFlagsFromStr(NStr::CStr const &_Flags)
	{
		return CRegistryTestResults::fs_ReportFlagsFromStr(_Flags);
	}

	NStr::CStr CTestResultParser::fs_TestFlagsToStr(ETestFlag _Flags)
	{
		return CRegistryTestResults::fs_TestFlagsToStr(_Flags);
	}

	ETestFlag CTestResultParser::fs_TestFlagsFromStr(NStr::CStr const &_Flags)
	{
		return CRegistryTestResults::fs_TestFlagsFromStr(_Flags);
	}

	NStr::CStr CTestResultParser::fs_FailureActionToStr(ETest _Action)
	{
		return CRegistryTestResults::fs_FailureActionToStr(_Action);
	}

	ETest CTestResultParser::fs_FailureActionFromStr(NStr::CStr const &_Action)
	{
		return CRegistryTestResults::fs_FailureActionFromStr(_Action);
	}

	NStr::CStr CTestResultParser::fs_TestResultToStr(ETestResult _TestResult)
	{
		return CRegistryTestResults::fs_TestResultToStr(_TestResult);
	}

	ETestResult CTestResultParser::fs_TestResultFromStr(NStr::CStr const &_TestResult)
	{
		return CRegistryTestResults::fs_TestResultFromStr(_TestResult);
	}

	NStr::CStr CTestResultParser::fs_CheckTypeToStr(ECheckType _CheckType)
	{
		return CRegistryTestResults::fs_CheckTypeToStr(_CheckType);
	}

	ECheckType CTestResultParser::fs_CheckTypeFromStr(NStr::CStr const &_CheckType)
	{
		return CRegistryTestResults::fs_CheckTypeFromStr(_CheckType);
	}

	void CTestResultParser::fs_DecodeMemoryResults(NContainer::CRegistry const &_Registry, CTestMemoryResults &_Results)
	{
		return CRegistryTestResults::fs_DecodeMemoryResults(_Registry, _Results);
	}

	void CTestResultParser::fs_DecodePerformanceResults(NContainer::CRegistry const &_Registry, CTestPerformanceResults &_Results)
	{
		return CRegistryTestResults::fs_DecodePerformanceResults(_Registry, _Results);
	}

	void CTestResultParser::fs_DecodeResult(NContainer::CRegistry const &_Registry, CTestResult &_Results)
	{
		return CRegistryTestResults::fs_DecodeResult(_Registry, _Results);
	}
}
