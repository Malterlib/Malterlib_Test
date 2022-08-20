// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>

#include "Malterlib_Test_Reporter_Text.h"

namespace NMib::NTest
{
	CTextTestResultsBrief::CTextTestResultsBrief(NCommandLine::EAnsiEncodingFlag _AnsiEncodingFlags)
		: CTextTestResults(_AnsiEncodingFlags)
	{
	}

	void CTextTestResultsBrief::fp_AddReport
		(
			NStr::CStr const &_FileLocation
			, NStr::CStr const &_Result
			, NStr::CStr const &_TestPath
			, NStr::CStr const &_Expression
			, NStr::CStr const &_Values
			, ETestSeverity _Severity
			, bool _bHeader
		)
	{
		using namespace NStr;
		NStr::CStr Result = _Result;
		if (Result == "FAILED (Expected) and ABORTED")
			Result = "FAILED (Exp) and AB";

		if (_bHeader)
		{
			fp_ReportText
				(
					"{} {} {}  [{}]{\n}"_f
					<< fp_ColorHeading("{sj100,a-}"_f << _TestPath)
					<< fp_ColorHeading("{sj20,a-}"_f << Result)
					<< fp_ColorHeading(_Expression)
					<< fp_ColorHeading(_Values)
					, _Severity
				)
			;
		}
		else
		{
			fp_ReportText
				(
					"{} {} {}  [{}]{\n}"_f
					<< fp_ColorPath("{sj100,a-}"_f << _TestPath)
					<< fp_ColorSeverity("{sj20,a-}"_f << Result, _Severity)
					<< fp_ColorExpression(_Expression)
					<< fp_ColorValues(_Values)
					, _Severity
				)
			;
		}
	}

	CTextTestResults::CTextTestResults(NCommandLine::EAnsiEncodingFlag _AnsiEncodingFlags)
		: mp_AnsiEncodingFlags(_AnsiEncodingFlags)
	{
	}

	NStr::CStr CTextTestResults::fp_ColorPath(NStr::CStr const &_String) const
	{
		using namespace NStr;
		NCommandLine::CAnsiEncoding AnsiEncoding(mp_AnsiEncodingFlags);
		return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGBFormat(220, 220, 220) << _String << AnsiEncoding.f_Default();
	}

	static NStr::CStr fg_FormatOperators(NCommandLine::CAnsiEncoding const &_AnsiEncoding, NStr::CStr const &_String, NStr::CStr const &_DefaultFormat)
	{
		using namespace NStr;
		auto fColorExpr = [&](CStr const &_String)
			{
				return CStr("{}{}{}"_f << _AnsiEncoding.f_ForegroundRGBFormat(255, 255, 255) << _String << _DefaultFormat);
			}
		;

		CStr String = _String;
		String = String.f_Replace("[", fColorExpr("["));
		String = String.f_Replace("=", fColorExpr("="));
		String = String.f_Replace("]", fColorExpr("]"));
		String = String.f_Replace("!", fColorExpr("!"));
		String = String.f_Replace("<", fColorExpr("<"));
		String = String.f_Replace(">", fColorExpr(">"));
		String = String.f_Replace("{", fColorExpr("{"));
		String = String.f_Replace("}", fColorExpr("}"));
		String = String.f_Replace("^", fColorExpr("^"));
		String = String.f_Replace("~", fColorExpr("~"));
		String = String.f_Replace("&", fColorExpr("&"));
		String = String.f_Replace("|", fColorExpr("|"));
		String = String.f_Replace("(", fColorExpr("("));
		String = String.f_Replace(")", fColorExpr(")"));
		String = String.f_Replace("+", fColorExpr("+"));
		String = String.f_Replace("-", fColorExpr("-"));
		String = String.f_Replace("/", fColorExpr("/"));
		String = String.f_Replace("*", fColorExpr("*"));
		String = String.f_Replace(",", fColorExpr(","));

		return String;
	}

	NStr::CStr CTextTestResults::fp_ColorExpression(NStr::CStr const &_String) const
	{
		using namespace NStr;
		NCommandLine::CAnsiEncoding AnsiEncoding(mp_AnsiEncodingFlags);
		CStr Color = AnsiEncoding.f_ForegroundRGBFormat(0xFF, 0xD7, 0x00);
		CStr String = fg_FormatOperators(AnsiEncoding, _String, Color);
		return "{}{}{}"_f << Color << String << AnsiEncoding.f_Default();
	}

	NStr::CStr CTextTestResults::fp_ColorValues(NStr::CStr const &_String) const
	{
		using namespace NStr;
		NCommandLine::CAnsiEncoding AnsiEncoding(mp_AnsiEncodingFlags);
		CStr Color = AnsiEncoding.f_ForegroundRGBFormat(0xFF, 0x8A, 0xC5);
		CStr String = fg_FormatOperators(AnsiEncoding, _String, Color);
		return "{}{}{}"_f << Color << String << AnsiEncoding.f_Default();
	}

	NStr::CStr CTextTestResults::fp_ColorFileLocation(NStr::CStr const &_String) const
	{
		using namespace NStr;
		NCommandLine::CAnsiEncoding AnsiEncoding(mp_AnsiEncodingFlags);
		return "{}{}{}"_f << AnsiEncoding.f_ForegroundRGBFormat(160, 160, 160) << _String << AnsiEncoding.f_Default();
	}

	NStr::CStr CTextTestResults::fp_ColorHeading(NStr::CStr const &_String) const
	{
		using namespace NStr;
		NCommandLine::CAnsiEncoding AnsiEncoding(mp_AnsiEncodingFlags);
		return "{}{}{}"_f << AnsiEncoding.f_Bold() << _String << AnsiEncoding.f_Default();
	}

	NStr::CStr CTextTestResults::fp_ColorSeverity(NStr::CStr const &_ToColor, ETestSeverity _Severity) const
	{
		NCommandLine::CAnsiEncoding AnsiEncoding(mp_AnsiEncodingFlags);
		switch (_Severity)
		{
		case ETestSeverity_Error: return AnsiEncoding.f_StatusError(_ToColor);
		case ETestSeverity_Warning: return AnsiEncoding.f_StatusWarning(_ToColor);
		case ETestSeverity_Success: return AnsiEncoding.f_StatusNormal(_ToColor);
		default: return _ToColor;
		}
	}

	void CTextTestResults::fp_AddReport
		(
			NStr::CStr const &_FileLocation
			, NStr::CStr const &_Result
			, NStr::CStr const &_TestPath
			, NStr::CStr const &_Expression
			, NStr::CStr const &_Values
			, ETestSeverity _Severity
			, bool _bHeader
		)
	{
		using namespace NStr;
		if (_bHeader)
		{
			fp_ReportText
				(
					"{} {} {}{\n}"_f
					<< fp_ColorHeading("{sl100,a-}"_f << _FileLocation)
					<< fp_ColorHeading("{sl24,a-}"_f << _Result)
					<< fp_ColorHeading(_TestPath)
					, _Severity
				)
			;
		}
		else
		{
			CStr Values;
			if (_Values)
				Values = "{\n}{}{\n}{\n}"_f << fp_ColorValues("{}"_f << _Values.f_Indent("    ", true));

			fp_ReportText
				(
					"{} {} {}{\n}{\n}{}{\n}{}"_f
					<< fp_ColorFileLocation("{sl100,a-}"_f << _FileLocation)
					<< fp_ColorSeverity("{sl24,a-}"_f << _Result, _Severity)
					<< fp_ColorPath(_TestPath)
					<< fp_ColorExpression("{}"_f << _Expression.f_Indent("    ", true))
					<< Values
					, _Severity
				)
			;
		}
	}

	void CTextTestResults::f_ReportHeader(ETestReportFlag _ReportFlags)
	{
		mp_ReportFlags = _ReportFlags;
		fp_ReportText("------ Tests started ------" DMibNewLine, ETestSeverity_None);
		fp_AddReport("", "Result", "Test Path", "Expression", "Values", ETestSeverity_None, true);
	}

	void CTextTestResults::f_ReportFooter(mint _nTestsTotal, mint _nSuccessful, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored)
	{
		using namespace NStr;
		fp_ReportText
			(
				NStr::CStr::CFormat(DMibNewLine "========== Test: {} ({}) succeeded (unexpected), {} ({}) failed (expected), {} warnings, {} ignored ==========" DMibNewLine DMibNewLine)
				<< fp_ColorSeverity("{}"_f << _nSuccessful, _nSuccessful > 0 ? ETestSeverity_Success : ETestSeverity_None)
				<< fp_ColorSeverity("{}"_f << _nSuccessUnexpected, _nSuccessUnexpected > 0 ? ETestSeverity_Warning : ETestSeverity_None)
				<< fp_ColorSeverity("{}"_f << _nFailed, _nFailed > 0 ? ETestSeverity_Error : ETestSeverity_None)
				<< fp_ColorSeverity("{}"_f << _nExpectFailed, _nExpectFailed > 0 ? ETestSeverity_Warning : ETestSeverity_None)
				<< fp_ColorSeverity("{}"_f << _nWarnings, _nWarnings > 0 ? ETestSeverity_Warning : ETestSeverity_None)
				<< _nIgnored
				, ETestSeverity_None
			)
		;
	}

	ETestNeedReportFlag CTextTestResults::f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags)
	{
		bool bReport = false;
		if (_Result == ETestResult_Fail)
		{
			switch (_FailureAction)
			{
			case ETest_Warn:
				if (mp_ReportFlags & ETestReportFlag_Warning)
					bReport = true;
				break;
			case ETest_Fail:
				if (mp_ReportFlags & ETestReportFlag_Fail)
					bReport = true;
				if (mp_ReportFlags & ETestReportFlag_BreakOnFail)
					DMibPDebugBreak;
				break;
			case ETest_ExpectFail:
				if (mp_ReportFlags & ETestReportFlag_ExpectFail)
					bReport = true;
				break;
			case ETest_FailAndStop:
				if (mp_ReportFlags & ETestReportFlag_FailAndStop)
					bReport = true;
				if (mp_ReportFlags & ETestReportFlag_BreakOnFail)
					DMibPDebugBreak;
				break;
			case ETest_ExpectFailAndStop:
				if (mp_ReportFlags & ETestReportFlag_ExpectFailAndStop)
					bReport = true;
				break;
			case ETest_None:
				break;
			}
		}
		else if (_Result == ETestResult_Success)
		{
			if (_FailureAction == ETest_ExpectFail || _FailureAction == ETest_ExpectFailAndStop)
				bReport = true;
			else if (mp_ReportFlags & ETestReportFlag_Success)
				bReport = true;
		}
		else if (_Result == ETestResult_Ignored)
		{
			if (mp_ReportFlags & ETestReportFlag_Ignored)
				bReport = true;
		}
		if (bReport)
			return ETestNeedReportFlag_Count | ETestNeedReportFlag_Report;
		else
			return ETestNeedReportFlag_Count;
	}

	void CTextTestResults::f_ReportResult
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
		auto fReport = [&,this](NStr::CStr const &_MessageStr, ETestSeverity _Severity)
		{
			NStr::CStr FileLocation = NStr::CStr::CFormat(DMibPFileLineFormat "") << _Location.m_File << _Location.m_Line;
			fp_AddReport(FileLocation, _MessageStr, _TestPath, _Message, _Values, _Severity, false);
			if (!_ExtraMultiLineReportData.f_IsEmpty())
			{
				fp_ReportText("	" DMibNewLine, ETestSeverity_None);

				bool bLastEmpty = false;
				for (auto &Line : _ExtraMultiLineReportData.f_SplitLine())
				{
					bLastEmpty = Line.f_IsEmpty();
					fp_ReportText(NStr::CStr::CFormat("	{}" DMibNewLine) << Line, ETestSeverity_None);
				}

				if (!bLastEmpty)
					fp_ReportText("	" DMibNewLine, ETestSeverity_None);
			}
		};
		if (_Result == ETestResult_Fail)
		{
			switch (_FailureAction)
			{
			case ETest_Warn:
				if (mp_ReportFlags & ETestReportFlag_Warning)
					fReport("Warning", ETestSeverity_Warning);
				break;
			case ETest_Fail:
				if (mp_ReportFlags & ETestReportFlag_Fail)
					fReport("FAILED", ETestSeverity_Error);
				break;
			case ETest_ExpectFail:
				if (mp_ReportFlags & ETestReportFlag_ExpectFail)
					fReport("FAILED (Expected)", ETestSeverity_Warning);
				break;
			case ETest_FailAndStop:
				if (mp_ReportFlags & ETestReportFlag_FailAndStop)
					fReport("FAILED and ABORTED", ETestSeverity_Error);
				break;
			case ETest_ExpectFailAndStop:
				if (mp_ReportFlags & ETestReportFlag_ExpectFailAndStop)
					fReport("FAILED (Expected) and ABORTED", ETestSeverity_Warning);
				break;
			case ETest_None:
				break;
			}
		}
		else if (_Result == ETestResult_Success)
		{
			switch (_FailureAction)
			{
			case ETest_ExpectFail:
			case ETest_ExpectFailAndStop:
				if (mp_ReportFlags & ETestReportFlag_Fail)
					fReport("Success (UNEXPECTED)", ETestSeverity_Warning);
				break;
			default:
				if (mp_ReportFlags & ETestReportFlag_Success)
					fReport("Success", ETestSeverity_Success);
				break;
			}
		}
		else if (_Result == ETestResult_Ignored)
		{
			if (mp_ReportFlags & ETestReportFlag_Ignored)
				fReport("Ignored", ETestSeverity_None);
		}
	}
	void CTextTestResults::f_ReportSuite(const NStr::CStr &_TestPath, const NContainer::TCMap<NStr::CStr> &_TestGroups, CTestLocation const &_Location)
	{
	}
	void CTextTestResults::f_PerformanceResults(CTestPerformanceResults const &_Results)
	{
		if (mp_ReportFlags & ETestReportFlag_DetailedPerformance)
		{
			CTestPerformanceResults Results = _Results;
			auto fl_ReportRow0 = [&]
			(
				NStr::CStr const &_Name
				, NStr::CStr const &_Iterations
				, NStr::CStr const &_IterationsPerSec
				, NStr::CStr const &_ThroughputPerSec
				, NStr::CStr const &_BaseLine
				, NStr::CStr const &_Repetitions
				, NStr::CStr const &_CyclesMedian
				, NStr::CStr const &_CyclesAverage
				, NStr::CStr const &_CyclesMin
				, NStr::CStr const &_CyclesMax
				, NStr::CStr const &_CyclesStdDev
			)
			{
				fp_ReportText
					(
						NStr::CStr::CFormat("{sz20,a-}{sz17}{sz17}{sz17}{sz12}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}" DMibNewLine)
						<< _Name
						<< _Iterations
						<< _IterationsPerSec
						<< _ThroughputPerSec
						<< _BaseLine
						<< _Repetitions
						<< _CyclesMin
						<< _CyclesMedian
						<< _CyclesAverage
						<< _CyclesMax
						<< _CyclesStdDev
						, ETestSeverity_None
					)
				;
			};

			auto fl_ReportRow1 = [&]
			(
				NStr::CStr const &_Name
				, NStr::CStr const &_TimeMedian
				, NStr::CStr const &_TimeAverage
				, NStr::CStr const &_TimeMin
				, NStr::CStr const &_TimeMax
				, NStr::CStr const &_TimeStdDev
			)
			{
				fp_ReportText
					(
						NStr::CStr::CFormat("{sz20,a-}{sz16}{sz16}{sz16}{sz16}{sz16}" DMibNewLine)
						<< _Name
						<< _TimeMin
						<< _TimeMedian
						<< _TimeAverage
						<< _TimeMax
						<< _TimeStdDev
						, ETestSeverity_None
					)
				;
			};

			auto fl_ReportCounterRow = [&]
			(
				NStr::CStr const &_Counter
				, NStr::CStr const &_Name
				, NStr::CStr const &_CyclesMedian
				, NStr::CStr const &_CyclesAverage
				, NStr::CStr const &_CyclesMin
				, NStr::CStr const &_CyclesMax
				, NStr::CStr const &_CyclesStdDev
			)
			{
				fp_ReportText
					(
						NStr::CStr::CFormat("{sz20,a-}{sz20,a-}{sz16}{sz16}{sz16}{sz16}{sz16}" DMibNewLine)
						<< _Counter
						<< _Name
						<< _CyclesMedian
						<< _CyclesAverage
						<< _CyclesMin
						<< _CyclesMax
						<< _CyclesStdDev
						, ETestSeverity_None
					)
				;
			};

			fl_ReportRow0
			(
				"Name(*=ref)"
				, "Iterations"
				, "Iteratians/s"
				, "Throughput/s"
				, "Baseline(%)"
				, "Repetitions"
				, "Median(cycles)"
				, "Average(cycles)"
				, "Min(cycles)"
				, "Max(cycles)"
				, "StdDev(%)"
			);
			CTestPerformanceResult const *pBaseLine = nullptr;
			mint nResults = Results.m_Results.f_GetLen();
			NContainer::TCSet<NStr::CStr> Counters;

			Results.m_Results.f_Sort
				(
					[](CTestPerformanceResult const& _Left, CTestPerformanceResult const& _Right) -> COrdering_Partial
					{
						if (auto Result = (_Left.m_MeasureType == ETestMeasureType_Baseline) <=> (_Right.m_MeasureType == ETestMeasureType_Baseline); Result != 0)
							return Result;

						if (auto Result = (_Left.m_MeasureType == ETestMeasureType_Debug) <=> (_Right.m_MeasureType == ETestMeasureType_Debug); Result != 0)
							return Result;

						return _Left.m_Time.m_Min <=> _Right.m_Time.m_Min;
					}
				)
			;

			for (mint i = 0 ; i < nResults; ++i)
			{
				CTestPerformanceResult const &Result = Results.m_Results[i];
				if (Result.m_MeasureType == ETestMeasureType_Baseline)
				{
					pBaseLine = &Result;
					break;
				}
			}
			for (mint i = 0 ; i < nResults; ++i)
			{
				CTestPerformanceResult const &Result = Results.m_Results[i];
				NStr::CStr Name = Result.m_Name;
				if (Result.m_MeasureType == ETestMeasureType_Reference)
					Name = "*" + Name;
				else if (Result.m_MeasureType == ETestMeasureType_Baseline)
					Name = "+" + Name;
				else if (Result.m_MeasureType == ETestMeasureType_Debug)
					Name = "-" + Name;
				else
					Name = " " + Name;

				for (auto Iter = Result.m_PerformanceCounters.f_GetIterator(); Iter; ++Iter)
					Counters[Iter.f_GetKey()];

				NStr::CStr BaseLine;
				auto IterationsPerSec = (fp64(1.0)/Result.m_Time.m_Min);
				auto Throughput = (fp64(Result.m_nContributingThreads)/Result.m_Time.m_Min);
				if (pBaseLine && (&Result != pBaseLine))
					BaseLine = NStr::CStr::CFormat("{fe2,fn2}") << (pBaseLine->m_Cycles.m_Min / Result.m_Cycles.m_Min) * 100.0;
				if (pBaseLine && (mp_ReportFlags & ETestReportFlag_CompareToBaseline) && pBaseLine != &Result)
				{
					fl_ReportRow0
					(
						Name
						, NStr::CStr::CFormat("{}") << Result.m_nIterations
						, NStr::CStr::CFormat("{}.{fr1,fn1}") << IterationsPerSec.f_ToInt() << IterationsPerSec.f_Fraction()
						, NStr::CStr::CFormat("{}.{fr1,fn1}") << Throughput.f_ToInt() << Throughput.f_Fraction()
						, BaseLine
						, NStr::CStr::CFormat("{}") << Result.m_nRepetitions
						, NStr::CStr::CFormat("+ {fe2,fn2}") << (Result.m_Cycles.m_Median - pBaseLine->m_Cycles.m_Median)
						, NStr::CStr::CFormat("+ {fe2,fn2}") << (Result.m_Cycles.m_Average - pBaseLine->m_Cycles.m_Average)
						, NStr::CStr::CFormat("+ {fe2,fn2}") << (Result.m_Cycles.m_Min - pBaseLine->m_Cycles.m_Min)
						, NStr::CStr::CFormat("+ {fe2,fn2}") << (Result.m_Cycles.m_Max - pBaseLine->m_Cycles.m_Max)
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Cycles.f_StdDevFraction() * 100.0
					);
				}
				else
				{
					fl_ReportRow0
					(
						Name
						, NStr::CStr::CFormat("{}") << Result.m_nIterations
						, NStr::CStr::CFormat("{}.{fr1,fn1}") << IterationsPerSec.f_ToInt() << IterationsPerSec.f_Fraction()
						, NStr::CStr::CFormat("{}.{fr1,fn1}") << Throughput.f_ToInt() << Throughput.f_Fraction()
						, BaseLine
						, NStr::CStr::CFormat("{}") << Result.m_nRepetitions
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Cycles.m_Median
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Cycles.m_Average
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Cycles.m_Min
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Cycles.m_Max
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Cycles.f_StdDevFraction() * 100.0
					);
				}
			}

			fp64 SmallestAverage = fp64::fs_Inf();
			for (mint i = 0 ; i < nResults; ++i)
			{
				CTestPerformanceResult const &Result = Results.m_Results[i];
				if (Result.m_MeasureType == ETestMeasureType_Baseline && nResults != 1)
					continue; // Baseline is normally much smaller
				if (Result.m_Time.m_Min < SmallestAverage)
					SmallestAverage = Result.m_Time.m_Min;
			}

			NStr::CStr TimeUnit;
			fp64 TimeScale;


			if (SmallestAverage < 0.000000000001)
			{
				TimeUnit = "fs";
				TimeScale = 1000000000000000.0;
			}
			else if (SmallestAverage < 0.000000001)
			{
				TimeUnit = "ps";
				TimeScale = 1000000000000.0;
			}
			else if (SmallestAverage < 0.000001)
			{
				TimeUnit = "ns";
				TimeScale = 1000000000.0;
			}
			else if (SmallestAverage < 0.001)
			{
				TimeUnit = NStr::CStr(str_utf8("µs"));
				TimeScale = 1000000.0;
			}
			else if (SmallestAverage < 1.0)
			{
				TimeUnit = "ms";
				TimeScale = 1000.0;
			}
			else
			{
				TimeUnit = "s";
				TimeScale = 1.0;
			}

			fl_ReportRow1
			(
				"Name(*=ref)"
				, NStr::CStr::CFormat("Median({})") << TimeUnit
				, NStr::CStr::CFormat("Average({})") << TimeUnit
				, NStr::CStr::CFormat("Min({})") << TimeUnit
				, NStr::CStr::CFormat("Max({})") << TimeUnit
				, "StdDev(%)"
			);
			for (mint i = 0 ; i < nResults; ++i)
			{
				CTestPerformanceResult const &Result = Results.m_Results[i];
				NStr::CStr Name = Result.m_Name;
				if (Result.m_MeasureType == ETestMeasureType_Reference)
					Name = "*" + Name;
				else if (Result.m_MeasureType == ETestMeasureType_Baseline)
					Name = "+" + Name;
				else if (Result.m_MeasureType == ETestMeasureType_Debug)
					Name = "-" + Name;
				else
					Name = " " + Name;

				if ((pBaseLine && pBaseLine != &Result && (mp_ReportFlags & ETestReportFlag_CompareToBaseline)))
				{
					fl_ReportRow1
					(
						Name
						, NStr::CStr::CFormat("+ {fd3,fm3}") << (Result.m_Time.m_Median - pBaseLine->m_Time.m_Median)*TimeScale
						, NStr::CStr::CFormat("+ {fd3,fm3}") << (Result.m_Time.m_Average - pBaseLine->m_Time.m_Average)*TimeScale
						, NStr::CStr::CFormat("+ {fd3,fm3}") << (Result.m_Time.m_Min - pBaseLine->m_Time.m_Min)*TimeScale
						, NStr::CStr::CFormat("+ {fd3,fm3}") << (Result.m_Time.m_Max - pBaseLine->m_Time.m_Max)*TimeScale
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Time.f_StdDevFraction() * 100.0
					);
				}
				else
				{
					fl_ReportRow1
					(
						Name
						, NStr::CStr::CFormat("{fd3,fm3}") << Result.m_Time.m_Median*TimeScale
						, NStr::CStr::CFormat("{fd3,fm3}") << Result.m_Time.m_Average*TimeScale
						, NStr::CStr::CFormat("{fd3,fm3}") << Result.m_Time.m_Min*TimeScale
						, NStr::CStr::CFormat("{fd3,fm3}") << Result.m_Time.m_Max*TimeScale
						, NStr::CStr::CFormat("{fe2,fn2}") << Result.m_Time.f_StdDevFraction() * 100.0
					);
				}
			}

			for (auto Iter = Counters.f_GetIterator(); Iter; ++Iter)
			{
				fl_ReportCounterRow
					(
						"Counter"
						, "Name"
						, "Median"
						, "Average"
						, "Min"
						, "Max"
						, "StdDev(%)"
					)
				;
				fp_ReportText(NStr::CStr::CFormat("{}" DMibNewLine) << *Iter, ETestSeverity_None);
				for (auto ResultIter = Results.m_Results.f_GetIterator(); ResultIter; ++ResultIter)
				{
					CTestPerformanceResult const &Result = *ResultIter;

					CTestStats const *pStat = Result.m_PerformanceCounters.f_FindEqual(*Iter);

					CTestStats const *pBaselineStat
						= (pBaseLine && (mp_ReportFlags & ETestReportFlag_CompareToBaseline) && pBaseLine != &Result)
						? pBaseLine->m_PerformanceCounters.f_FindEqual(*Iter)
						: nullptr
					;

					if (pStat)
					{
						NStr::CStr Name = Result.m_Name;
						if (Result.m_MeasureType == ETestMeasureType_Reference)
							Name = "*" + Name;
						else if (Result.m_MeasureType == ETestMeasureType_Baseline)
							Name = "+" + Name;
						else if (Result.m_MeasureType == ETestMeasureType_Debug)
							Name = "-" + Name;
						else
							Name = " " + Name;

						if (pBaselineStat)
						{
							fl_ReportCounterRow
								(
									""
									, Name
									, NStr::CStr::CFormat("+ {fe2,fn2}") << (pStat->m_Median - pBaselineStat->m_Median)
									, NStr::CStr::CFormat("+ {fe2,fn2}") << (pStat->m_Average - pBaselineStat->m_Average)
									, NStr::CStr::CFormat("+ {fe2,fn2}") << (pStat->m_Min - pBaselineStat->m_Min)
									, NStr::CStr::CFormat("+ {fe2,fn2}") << (pStat->m_Max - pBaselineStat->m_Max)
									, NStr::CStr::CFormat("{fe2,fn2}") << pStat->f_StdDevFraction() * 100.0
								)
							;
						}
						else
						{
							fl_ReportCounterRow
								(
									""
									, Name
									, NStr::CStr::CFormat("{fe2,fn2}") << pStat->m_Median
									, NStr::CStr::CFormat("{fe2,fn2}") << pStat->m_Average
									, NStr::CStr::CFormat("{fe2,fn2}") << pStat->m_Min
									, NStr::CStr::CFormat("{fe2,fn2}") << pStat->m_Max
									, NStr::CStr::CFormat("{fe2,fn2}") << pStat->f_StdDevFraction() * 100.0
								)
							;
						}
					}
				}
			}
		}
	}

	void CTextTestResults::f_MemoryResults(CTestMemoryResults const &_Results)
	{
		if (mp_ReportFlags & ETestReportFlag_DetailedMemory)
		{
			auto fl_ReportRow = [&]
			(
				NStr::CStr const &_Allocator
				, NStr::CStr const &_Name
				, NStr::CStr const &_0
				, NStr::CStr const &_1
				, NStr::CStr const &_2
				, NStr::CStr const &_3
				, NStr::CStr const &_4
				, NStr::CStr const &_5
				, NStr::CStr const &_6
				, NStr::CStr const &_7
				, NStr::CStr const &_8
				, NStr::CStr const &_9
				, NStr::CStr const &_10
				, NStr::CStr const &_11
				, NStr::CStr const &_12
				, NStr::CStr const &_13
				, NStr::CStr const &_14
				, NStr::CStr const &_15
				, NStr::CStr const &_16
				, NStr::CStr const &_17
				, NStr::CStr const &_18
				, NStr::CStr const &_19
				, NStr::CStr const &_20
				, NStr::CStr const &_21
				, NStr::CStr const &_22
			)
			{
				fp_ReportText
					(
						NStr::CStr::CFormat
						(
							"{sz20,a-}{sz20,a-}{sz12}{sz12}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}{sz16}" DMibNewLine
						)
						<< _Allocator
						<< _Name
						<< _0
						<< _1
						<< _2
						<< _3
						<< _4
						<< _5
						<< _6
						<< _7
						<< _8
						<< _9
						<< _10
						<< _11
						<< _12
						<< _13
						<< _14
						<< _15
						<< _16
						<< _17
						<< _18
						<< _19
						<< _20
						<< _21
						<< _22
						, ETestSeverity_None
					)
				;
			};

			fl_ReportRow
			(
				"Allocator"
				, "Name(*=ref)"
				, "Iterations"
				, "Repetitions"
				, "BytesMaxAlloc"
				, "nAllocations"
				, "nFree"
				, "nResize"
				, "nRealloc"
				, "nGetSize"
				, "nProtect"
				, "nCommit"
				, "nDecommit"
				, "BytesAlloc"
				, "BytesFree"
				, "BytesResize"
				, "BytesRealloc"
				, "BytesProtect"
				, "BytesCommit"
				, "BytesDecommit"
				, "BytesOverhead"
				, "BytesWaste"
				, "OverheadAtMax"
				, "WasteAtMax"
				, "nAllocAtMax"
			);

			NContainer::TCMap<NStr::CStr> Allocators;
			Allocators["All"];
			mint nResults = _Results.m_Results.f_GetLen();
			for (mint i = 0 ; i < nResults; ++i)
			{
				CTestMemoryResult const &Result = _Results.m_Results[i];

				NMisc::fg_ForEach
					(
						Result.m_PerAllocationType
						, [&](CTestMemoryStats const &_Stats)
						{
							Allocators[Result.m_PerAllocationType.fs_GetKey(_Stats)];
						}
					)
				;

			}

			CTestMemoryStats EmptyStats;

			NMisc::fg_ForEach
				(
					Allocators
					, [&](NContainer::CMapNoData const &_Allocator)
					{
						NStr::CStr Allocator = Allocators.fs_GetKey(_Allocator);
						fp_ReportText(NStr::CStr::CFormat("{}" DMibNewLine) << Allocator, ETestSeverity_None);
						mint nResults = _Results.m_Results.f_GetLen();
						for (mint i = 0 ; i < nResults; ++i)
						{
							CTestMemoryResult const &Result = _Results.m_Results[i];
							NStr::CStr Name = Result.m_Name;
							if (Result.m_MeasureType == ETestMeasureType_Reference)
								Name = "*" + Name;
							else if (Result.m_MeasureType == ETestMeasureType_Baseline)
								Name = "+" + Name;
							else if (Result.m_MeasureType == ETestMeasureType_Debug)
								Name = "-" + Name;
							else
								Name = " " + Name;

							CTestMemoryStats const *pStats = nullptr;
							if (Allocator == "All")
								pStats = &Result.m_AllAllocations;
							else
							{
								pStats = Result.m_PerAllocationType.f_FindEqual(Allocator);
								if (!pStats)
									pStats = &EmptyStats;
							}

							fl_ReportRow
							(
								""
								, Name
								, NStr::CStr::CFormat("{}") << Result.m_nIterations
								, NStr::CStr::CFormat("{}") << Result.m_nRepetitions
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesMaxAlloc.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nAllocations.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nFree.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nResize.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nRealloc.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nGetSize.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nProtect.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nCommit.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_nDecommit.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesAlloc.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesFree.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesResize.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesRealloc.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesProtect.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesCommit.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesDecommit.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesOverhead.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_BytesPotentialWaste.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_AtBytesMaxAlloc.m_BytesOverhead.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
								, NStr::CStr::CFormat("{fe2,fn2}") << pStats->m_AtBytesMaxAlloc.m_nAllocations.m_Average
							);
						}
					}
				)
			;
		}
	}
}
