// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>
#include <Mib/CommandLine/CommandLine>
#include <Mib/Process/Platform>

namespace NMib
{
	namespace NTest
	{

		void CTextTestResultsBrief::fp_AddReport
			(
				const NStr::CStr &_FileLocation
				, const NStr::CStr &_Result
				, const NStr::CStr &_TestPath
				, const NStr::CStr &_Expression
				, const NStr::CStr &_Values
				, NSys::EColor _Color
			)
		{
			NStr::CStr Result = _Result;
			if (Result == "FAILED (Expected) and ABORTED")
				Result = "FAILED (Exp) and AB";
				
			fp_ReportText(NStr::CStr::CFormat("{sj100,a-} {sj20,a-} {}  [{}]" DMibNewLine) << _TestPath << Result << _Expression << _Values, _Color);
		}
		void CTextTestResultsBrief::fp_AddReport(const NStr::CStr &_FileLocation, const NStr::CStr &_Result, const NStr::CStr &_TestPath, const NStr::CStr &_Expression, const NStr::CStr &_Values)
		{
			NStr::CStr Result = _Result;
			if (Result == "FAILED (Expected) and ABORTED")
				Result = "FAILED (Exp) and AB";
				
			fp_ReportText(NStr::CStr::CFormat("{sj100,a-} {sj20,a-} {}  [{}]" DMibNewLine) << _TestPath << Result << _Expression << _Values);
		}
		
		void CTextTestResults::fp_AddReport
			(
				const NStr::CStr &_FileLocation
				, const NStr::CStr &_Result
				, const NStr::CStr &_TestPath
				, const NStr::CStr &_Expression
				, const NStr::CStr &_Values
				, NSys::EColor _Color
			)
		{
			fp_ReportText(NStr::CStr::CFormat("{sl100,a-} {sl24,a-} {sl60,a-} {sl60,a-} {}" DMibNewLine) << _FileLocation << _Result << _Expression << _Values << _TestPath, _Color);
		}
		void CTextTestResults::fp_AddReport(const NStr::CStr &_FileLocation, const NStr::CStr &_Result, const NStr::CStr &_TestPath, const NStr::CStr &_Expression, const NStr::CStr &_Values)
		{
			fp_ReportText(NStr::CStr::CFormat("{sl100,a-} {sl24,a-} {sl60,a-} {sl60,a-} {}" DMibNewLine) << _FileLocation << _Result << _Expression << _Values << _TestPath);
		}
		void CTextTestResults::f_ReportHeader(ETestReportFlag _ReportFlags)
		{
			mp_ReportFlags = _ReportFlags;
			fp_ReportText("------ Tests started ------" DMibNewLine);
			fp_AddReport("", "Result", "Test Path", "Expression", "Values");
		}

		void CTextTestResults::f_ReportFooter(mint _nTestsTotal, mint _nSuccessful, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored)
		{
			fp_ReportText
			(
				NStr::CStr::CFormat(DMibNewLine "========== Test: {} ({}) succeeded (unexpected), {} ({}) failed (expected), {} warnings, {} ignored ==========" DMibNewLine DMibNewLine)
				<< _nSuccessful 
				<< _nSuccessUnexpected
				<< _nFailed 
				<< _nExpectFailed 
				<< _nWarnings 
				<< _nIgnored
			);
		}

		ETestNeedReportFlag CTextTestResults::f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags)
		{
			bint bReport = false;
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
			auto f_Report = [&,this](NStr::CStr const &_MessageStr,NSys::EColor _Color)
			{
				NStr::CStr FileLocation = NStr::CStr::CFormat(DMibPFileLineFormat "") << _Location.m_File << _Location.m_Line;
				if (mp_ReportFlags & ETestReportFlag_UseColor)
					fp_AddReport(FileLocation, _MessageStr, _TestPath, _Message, _Values, _Color);
				else
					fp_AddReport(FileLocation, _MessageStr, _TestPath, _Message, _Values);
				if (!_ExtraMultiLineReportData.f_IsEmpty())
				{
					fp_ReportText("	" DMibNewLine);
					NStr::CStr Temp = _ExtraMultiLineReportData;
					while (!Temp.f_IsEmpty())
					{
						NStr::CStr Line = NStr::fg_GetStrLineSep(Temp);
						fp_ReportText(NStr::CStr::CFormat("	{}" DMibNewLine) << Line);
					}
					fp_ReportText("	" DMibNewLine);
				}
			};
			if (_Result == ETestResult_Fail)
			{
				switch (_FailureAction)
				{
				case ETest_Warn:
					if (mp_ReportFlags & ETestReportFlag_Warning)
						f_Report("Warning", NSys::EColor_Yellow);
					break;
				case ETest_Fail:
					if (mp_ReportFlags & ETestReportFlag_Fail)
						f_Report("FAILED", NSys::EColor_Red);
					break;
				case ETest_ExpectFail:
					if (mp_ReportFlags & ETestReportFlag_ExpectFail)
						f_Report("FAILED (Expected)", NSys::EColor_Yellow);
					break;
				case ETest_FailAndStop:
					if (mp_ReportFlags & ETestReportFlag_FailAndStop)
						f_Report("FAILED and ABORTED", NSys::EColor_Red);
					break;
				case ETest_ExpectFailAndStop:
					if (mp_ReportFlags & ETestReportFlag_ExpectFailAndStop)
						f_Report("FAILED (Expected) and ABORTED", NSys::EColor_Yellow);
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
						f_Report("Success (UNEXPECTED)", NSys::EColor_Yellow);
					break;
				default:
					if (mp_ReportFlags & ETestReportFlag_Success)
						f_Report("Success", NSys::EColor_Green);
					break;
				}
			}
			else if (_Result == ETestResult_Ignored)
			{ 
				if (mp_ReportFlags & ETestReportFlag_Ignored)
					f_Report("Ignored", NSys::EColor_Default);
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
						<< _CyclesMedian
						<< _CyclesAverage
						<< _CyclesMin
						<< _CyclesMax 
						<< _CyclesStdDev
					);
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
						<< _TimeMedian
						<< _TimeAverage
						<< _TimeMin
						<< _TimeMax 
						<< _TimeStdDev
					);
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
					);
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
						[](CTestPerformanceResult const& _Left, CTestPerformanceResult const& _Right) -> bool
						{
							if (_Left.m_MeasureType == ETestMeasureType_Baseline && _Right.m_MeasureType != ETestMeasureType_Baseline)
								return true;
							if (_Left.m_MeasureType != ETestMeasureType_Baseline && _Right.m_MeasureType == ETestMeasureType_Baseline)
								return false;

							if (_Left.m_MeasureType == ETestMeasureType_Debug && _Right.m_MeasureType != ETestMeasureType_Debug)
								return false;
							if (_Left.m_MeasureType != ETestMeasureType_Debug && _Right.m_MeasureType == ETestMeasureType_Debug)
								return true;

							return _Left.m_Time.m_Median < _Right.m_Time.m_Median;
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
					auto IterationsPerSec = (fp64(1.0)/Result.m_Time.m_Median);
					auto Throughput = (fp64(Result.m_nContributingThreads)/Result.m_Time.m_Median);
					if (pBaseLine && (&Result != pBaseLine))
						BaseLine = NStr::CStr::CFormat("{fe2,fn2}") << (pBaseLine->m_Cycles.m_Median / Result.m_Cycles.m_Median) * 100.0;
					if (pBaseLine && (mp_ReportFlags & ETestReportFlag_CompareToBaseline) && pBaseLine != &Result)
					{
						fl_ReportRow0
						(
							Name
							, NStr::CStr::CFormat("{ns }") << Result.m_nIterations
							, NStr::CStr::CFormat("{ns }.{fr1,fn1}") << IterationsPerSec.f_ToInt() << IterationsPerSec.f_Fraction()
							, NStr::CStr::CFormat("{ns }.{fr1,fn1}") << Throughput.f_ToInt() << Throughput.f_Fraction()
							, BaseLine
							, NStr::CStr::CFormat("{ns }") << Result.m_nRepetitions
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
							, NStr::CStr::CFormat("{ns }") << Result.m_nIterations
							, NStr::CStr::CFormat("{ns }.{fr1,fn1}") << IterationsPerSec.f_ToInt() << IterationsPerSec.f_Fraction()
							, NStr::CStr::CFormat("{ns }.{fr1,fn1}") << Throughput.f_ToInt() << Throughput.f_Fraction()
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
					if (Result.m_Time.m_Median < SmallestAverage)
						SmallestAverage = Result.m_Time.m_Median;
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
					fp_ReportText(NStr::CStr::CFormat("{}" DMibNewLine) << *Iter);
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
					);
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
							fp_ReportText(NStr::CStr::CFormat("{}" DMibNewLine) << Allocator);
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

		namespace NPrivate
		{

			void CRegistryTestResults::fsp_DecodeStatistics(NRegistry::CRegistry_CStr const &_ParentReg, CTestStats &_Stats)
			{
				_Stats.m_Min = _ParentReg.f_GetValueNoPath("Min").f_ToFloat(_Stats.m_Min);
				_Stats.m_Max = _ParentReg.f_GetValueNoPath("Max").f_ToFloat(_Stats.m_Max);
				_Stats.m_Average = _ParentReg.f_GetValueNoPath("Average").f_ToFloat(_Stats.m_Average);
				_Stats.m_Median = _ParentReg.f_GetValueNoPath("Median").f_ToFloat(_Stats.m_Median);
				_Stats.m_StdDev = _ParentReg.f_GetValueNoPath("StdDev").f_ToFloat(_Stats.m_StdDev);
			}

			NRegistry::CRegistry_CStr *CRegistryTestResults::fsp_AddStatistics(NRegistry::CRegistry_CStr *_pParentReg, NStr::CStr const &_Name, CTestStats const &_Stats)
			{
				NRegistry::CRegistry_CStr *pStat = _pParentReg->f_CreateChild("Stat", true);
				pStat->f_SetThisValue(_Name);

				pStat->f_SetValue("Min", NStr::CStr::fs_ToStr(_Stats.m_Min));
				pStat->f_SetValue("Max", NStr::CStr::fs_ToStr(_Stats.m_Max));
				pStat->f_SetValue("Average", NStr::CStr::fs_ToStr(_Stats.m_Average));
				pStat->f_SetValue("Median", NStr::CStr::fs_ToStr(_Stats.m_Median));
				pStat->f_SetValue("StdDev", NStr::CStr::fs_ToStr(_Stats.m_StdDev));
				
				return pStat;
			}

			void CRegistryTestResults::fsp_DecodeMemoryStatistics(NRegistry::CRegistry_CStr const &_ParentReg, CTestMemoryStats &_Stats)
			{
				for (auto Iter = _ParentReg.f_GetChildIterator("Stat"); Iter && Iter->f_GetName() == "Stat"; ++Iter)
				{
					if (Iter->f_GetThisValue() == "Num allocations")
						fsp_DecodeStatistics(*Iter, _Stats.m_nAllocations);
					else if (Iter->f_GetThisValue() == "Num free")
						fsp_DecodeStatistics(*Iter, _Stats.m_nFree);
					else if (Iter->f_GetThisValue() == "Num resize")
						fsp_DecodeStatistics(*Iter, _Stats.m_nResize);
					else if (Iter->f_GetThisValue() == "Num realloc")
						fsp_DecodeStatistics(*Iter, _Stats.m_nRealloc);
					else if (Iter->f_GetThisValue() == "Num get size")
						fsp_DecodeStatistics(*Iter, _Stats.m_nGetSize);
					else if (Iter->f_GetThisValue() == "Num protect")
						fsp_DecodeStatistics(*Iter, _Stats.m_nProtect);
					else if (Iter->f_GetThisValue() == "Num commit")
						fsp_DecodeStatistics(*Iter, _Stats.m_nCommit);
					else if (Iter->f_GetThisValue() == "Num decommit")
						fsp_DecodeStatistics(*Iter, _Stats.m_nDecommit);
					else if (Iter->f_GetThisValue() == "Bytes allocated")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesAlloc);
					else if (Iter->f_GetThisValue() == "Bytes freed")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesFree);
					else if (Iter->f_GetThisValue() == "Bytes resized")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesResize);
					else if (Iter->f_GetThisValue() == "Bytes rellocated")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesRealloc);
					else if (Iter->f_GetThisValue() == "Bytes protected")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesProtect);
					else if (Iter->f_GetThisValue() == "Bytes commited")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesCommit);
					else if (Iter->f_GetThisValue() == "Bytes decommited")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesDecommit);
					else if (Iter->f_GetThisValue() == "Bytes overhead")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesOverhead);
					else if (Iter->f_GetThisValue() == "Bytes potential waste")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesOverhead);
					else if (Iter->f_GetThisValue() == "Bytes allocated max")
						fsp_DecodeStatistics(*Iter, _Stats.m_BytesMaxAlloc);
					else if (Iter->f_GetThisValue() == "Bytes overhead at max allocated")
						fsp_DecodeStatistics(*Iter, _Stats.m_AtBytesMaxAlloc.m_BytesOverhead);
					else if (Iter->f_GetThisValue() == "Bytes potential waste at max allocated")
						fsp_DecodeStatistics(*Iter, _Stats.m_AtBytesMaxAlloc.m_BytesOverhead);
					else if (Iter->f_GetThisValue() == "Num allocations at max allocated")
						fsp_DecodeStatistics(*Iter, _Stats.m_AtBytesMaxAlloc.m_nAllocations);
					else
						DMibNeverGetHere(Iter->f_GetThisValue());
				}
			}

			NRegistry::CRegistry_CStr *CRegistryTestResults::fsp_AddMemoryStatistics(NRegistry::CRegistry_CStr *_pParentReg, NStr::CStr const &_Name, CTestMemoryStats const &_Stats)
			{
				NRegistry::CRegistry_CStr *pAllocationType = _pParentReg->f_CreateChild("AllocationType", true);
				pAllocationType->f_SetThisValue(_Name);

				fsp_AddStatistics(pAllocationType, "Num allocations", _Stats.m_nAllocations);
				fsp_AddStatistics(pAllocationType, "Num free", _Stats.m_nFree);
				fsp_AddStatistics(pAllocationType, "Num resize", _Stats.m_nResize);
				fsp_AddStatistics(pAllocationType, "Num realloc", _Stats.m_nRealloc);
				fsp_AddStatistics(pAllocationType, "Num get size", _Stats.m_nGetSize);
				fsp_AddStatistics(pAllocationType, "Num protect", _Stats.m_nProtect);
				fsp_AddStatistics(pAllocationType, "Num commit", _Stats.m_nCommit);
				fsp_AddStatistics(pAllocationType, "Num decommit", _Stats.m_nDecommit);
				fsp_AddStatistics(pAllocationType, "Bytes allocated", _Stats.m_BytesAlloc);
				fsp_AddStatistics(pAllocationType, "Bytes freed", _Stats.m_BytesFree);
				fsp_AddStatistics(pAllocationType, "Bytes resized", _Stats.m_BytesResize);
				fsp_AddStatistics(pAllocationType, "Bytes rellocated", _Stats.m_BytesRealloc);
				fsp_AddStatistics(pAllocationType, "Bytes protected", _Stats.m_BytesProtect);
				fsp_AddStatistics(pAllocationType, "Bytes commited", _Stats.m_BytesCommit);
				fsp_AddStatistics(pAllocationType, "Bytes decommited", _Stats.m_BytesDecommit);
				fsp_AddStatistics(pAllocationType, "Bytes overhead", _Stats.m_BytesOverhead);
				fsp_AddStatistics(pAllocationType, "Bytes potential waste", _Stats.m_BytesOverhead);
				fsp_AddStatistics(pAllocationType, "Bytes allocated max", _Stats.m_BytesMaxAlloc);
				fsp_AddStatistics(pAllocationType, "Bytes overhead at max allocated", _Stats.m_AtBytesMaxAlloc.m_BytesOverhead);
				fsp_AddStatistics(pAllocationType, "Bytes potential waste at max allocated", _Stats.m_AtBytesMaxAlloc.m_BytesOverhead);
				fsp_AddStatistics(pAllocationType, "Num allocations at max allocated", _Stats.m_AtBytesMaxAlloc.m_nAllocations);

				return pAllocationType;
			}

			NStr::CStr CRegistryTestResults::fs_ReportFlagsToStr(ETestReportFlag _Flags)
			{
				NStr::CStr Ret;
				if (_Flags & ETestReportFlag_Success)
					fg_AddStrSep(Ret, "Success", ',');
				if (_Flags & ETestReportFlag_Warning)
					fg_AddStrSep(Ret, "Warning", ',');
				if (_Flags & ETestReportFlag_Fail)
					fg_AddStrSep(Ret, "Fail", ',');
				if (_Flags & ETestReportFlag_FailAndStop)
					fg_AddStrSep(Ret, "FailAndStop", ',');
				if (_Flags & ETestReportFlag_ExpectFail)
					fg_AddStrSep(Ret, "ExpectFail", ',');
				if (_Flags & ETestReportFlag_ExpectFailAndStop)
					fg_AddStrSep(Ret, "ExpectFailAndStop", ',');
				if (_Flags & ETestReportFlag_UseColor)
					fg_AddStrSep(Ret, "UseColor", ',');
				if (_Flags & ETestReportFlag_ReportCategories)
					fg_AddStrSep(Ret, "ReportCategories", ',');
				if (_Flags & ETestReportFlag_DetailedPerformance)
					fg_AddStrSep(Ret, "DetailedPerformance", ',');
				if (_Flags & ETestReportFlag_Ignored)
					fg_AddStrSep(Ret, "Ignored", ',');
				if (_Flags & ETestReportFlag_DetailedMemory)
					fg_AddStrSep(Ret, "DetailedMemory", ',');
				if (_Flags & ETestReportFlag_BreakOnFail)
					fg_AddStrSep(Ret, "BreakOnFail", ',');
				if (_Flags & ETestReportFlag_CompareToBaseline)
					fg_AddStrSep(Ret, "CompareToBaseline", ',');
				if (_Flags & ETestReportFlag_CrashOnException)
					fg_AddStrSep(Ret, "CrashOnException", ',');
					
				return Ret;
			}


			ETestReportFlag CRegistryTestResults::fs_ReportFlagsFromStr(NStr::CStr const &_Flags)
			{
				NStr::CStr Flags = _Flags;
				ETestReportFlag Ret = ETestReportFlag_None;
				while (!Flags.f_IsEmpty())
				{
					NStr::CStr Flag = fg_GetStrSep(Flags, ",");
					if (Flag == "Success")
						Ret |= ETestReportFlag_Success;
					else if (Flag == "Warning")
						Ret |= ETestReportFlag_Warning;
					else if (Flag == "Fail")
						Ret |= ETestReportFlag_Fail;
					else if (Flag == "FailAndStop")
						Ret |= ETestReportFlag_FailAndStop;
					else if (Flag == "ExpectFail")
						Ret |= ETestReportFlag_ExpectFail;
					else if (Flag == "ExpectFailAndStop")
						Ret |= ETestReportFlag_ExpectFailAndStop;
					else if (Flag == "UseColor")
						Ret |= ETestReportFlag_UseColor;
					else if (Flag == "ReportCategories")
						Ret |= ETestReportFlag_ReportCategories;
					else if (Flag == "DetailedPerformance")
						Ret |= ETestReportFlag_DetailedPerformance;
					else if (Flag == "CompareToBaseline")
						Ret |= ETestReportFlag_CompareToBaseline;
					else if (Flag == "CrashOnException")
						Ret |= ETestReportFlag_CrashOnException;
					else if (Flag == "Ignored")
						Ret |= ETestReportFlag_Ignored;
					else if (Flag == "DetailedMemory")
						Ret |= ETestReportFlag_DetailedMemory;
					else if (Flag == "BreakOnFail")
						Ret |= ETestReportFlag_BreakOnFail;
				}
				return Ret;
			}

			NStr::CStr CRegistryTestResults::fs_TestFlagsToStr(ETestFlag _Flags)
			{
				NStr::CStr Ret;
				if (_Flags & ETestFlag_NoValues)
					fg_AddStrSep(Ret, "NoValues", ',');
				if (_Flags & ETestFlag_NoExceptionFilter)
					fg_AddStrSep(Ret, "NoExceptionFilter", ',');
				if (_Flags & ETestFlag_Aggregated)
					fg_AddStrSep(Ret, "Aggregate", ',');
				return Ret;
			}


			ETestFlag CRegistryTestResults::fs_TestFlagsFromStr(NStr::CStr const &_Flags)
			{
				NStr::CStr Flags = _Flags;
				ETestFlag Ret = ETestFlag_None;
				while (!Flags.f_IsEmpty())
				{
					NStr::CStr Flag = fg_GetStrSep(Flags, ",");
					if (Flag == "NoValues")
						Ret |= ETestFlag_NoValues;
					else if (Flag == "NoExceptionFilter")
						Ret |= ETestFlag_NoExceptionFilter;
					else if (Flag == "Aggregate")
						Ret |= ETestFlag_Aggregated;
				}
				return Ret;
			}

			NStr::CStr CRegistryTestResults::fs_MeasureTypeToStr(ETestMeasureType _MeasureType)
			{
				switch (_MeasureType)
				{
				case ETestMeasureType_Baseline: return "Baseline";
				case ETestMeasureType_Normal: return "Normal";
				case ETestMeasureType_Reference: return "Reference";
				case ETestMeasureType_Debug: return "Debug";
				default: DMibNeverGetHere(_MeasureType);
				}
				return "";
			}

			ETestMeasureType CRegistryTestResults::fs_MeasureTypeFromStr(NStr::CStr const &_MeasureType)
			{
				if (_MeasureType == "Baseline")
					return ETestMeasureType_Baseline;
				else if (_MeasureType == "Normal")
					return ETestMeasureType_Normal;
				else if (_MeasureType == "Reference")
					return ETestMeasureType_Reference;
				else if (_MeasureType == "Debug")
					return ETestMeasureType_Debug;
				else
					DMibNeverGetHere(_MeasureType);
				return ETestMeasureType_Normal;
			}

			NStr::CStr CRegistryTestResults::fs_FailureActionToStr(ETest _Action)
			{
				switch (_Action)
				{
				case ETest_Warn: return "Wait";
				case ETest_Fail: return "Fail";
				case ETest_FailAndStop: return "FailAndStop";
				case ETest_ExpectFail: return "ExpectFail";
				case ETest_ExpectFailAndStop: return "ExpectFailAndStop";
				default: DMibNeverGetHere(_Action);
				}
				return "";
			}

			ETest CRegistryTestResults::fs_FailureActionFromStr(NStr::CStr const &_Action)
			{
				if (_Action == "Wait")
					return ETest_Warn;
				else if (_Action == "Fail")
					return ETest_Fail;
				else if (_Action == "FailAndStop")
					return ETest_FailAndStop;
				else if (_Action == "ExpectFail")
					return ETest_ExpectFail;
				else if (_Action == "ExpectFailAndStop")
					return ETest_ExpectFailAndStop;
				else
					DMibNeverGetHere(_Action);
				return ETest_Warn;
			}

			NStr::CStr CRegistryTestResults::fs_TestResultToStr(ETestResult _TestResult)
			{
				switch (_TestResult)
				{
				case ETestResult_Fail: return "Fail";
				case ETestResult_Success: return "Success";
				case ETestResult_Ignored: return "Ignored";
				default: DMibNeverGetHere(_TestResult);
				}
				return "";
			}

			ETestResult CRegistryTestResults::fs_TestResultFromStr(NStr::CStr const &_TestResult)
			{
				if (_TestResult == "Fail")
					return ETestResult_Fail;
				else if (_TestResult == "Success")
					return ETestResult_Success;
				else if (_TestResult == "Ignored")
					return ETestResult_Ignored;
				else
					DMibNeverGetHere(_TestResult);
				DMibFastCheck(0);
				return ETestResult_Fail;
			}

			NStr::CStr CRegistryTestResults::fs_CheckTypeToStr(ECheckType _CheckType)
			{
				switch (_CheckType)
				{
				case ECheckType_Predicate: return "Predicate";
				case ECheckType_Message: return "Message";
				default: DMibNeverGetHere(_CheckType);
				}
				return "";
			}

			ECheckType CRegistryTestResults::fs_CheckTypeFromStr(NStr::CStr const &_CheckType)
			{
				if (_CheckType == "Predicate")
					return ECheckType_Predicate;
				else if (_CheckType == "Message")
					return ECheckType_Message;
				else
					DMibNeverGetHere(_CheckType);
				return ECheckType_Predicate;
			}


			void CRegistryTestResults::f_ReportHeader(ETestReportFlag _ReportFlags)
			{
				mp_ReportFlags = _ReportFlags;
				NRegistry::CRegistry_CStr Registry;
				NRegistry::CRegistry_CStr *pReg = Registry.f_CreateChild("Header");
				pReg->f_SetValue("Executable", NFile::CFile::fs_GetProgramPath());
				pReg->f_SetValue("Command Line", NSys::fg_CommandLineParameters());
				pReg->f_SetValue("Domain", NProcess::NPlatform::fg_Process_GetComputerDomain());
				pReg->f_SetValue("Computer", NProcess::NPlatform::fg_Process_GetComputerName());
				pReg->f_SetValue("ReportFlags", fs_ReportFlagsToStr(_ReportFlags));
				DMibConOut("{}", Registry.f_GenerateStr());
			}

			void CRegistryTestResults::f_ReportFooter(mint _nTestsTotal, mint _nSuccessful, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored)
			{
				NRegistry::CRegistry_CStr Registry;
				NRegistry::CRegistry_CStr *pReg = Registry.f_CreateChild("Footer");
				pReg->f_SetValue("TestsTotal", NStr::CStr::fs_ToStr(_nTestsTotal));
				pReg->f_SetValue("TestsSuccessful", NStr::CStr::fs_ToStr(_nSuccessful));
				pReg->f_SetValue("TestsSuccessUnexpected", NStr::CStr::fs_ToStr(_nSuccessUnexpected));
				pReg->f_SetValue("TestsFailed", NStr::CStr::fs_ToStr(_nFailed));
				pReg->f_SetValue("TestsExpectFailed", NStr::CStr::fs_ToStr(_nExpectFailed));
				pReg->f_SetValue("TestsWarnings", NStr::CStr::fs_ToStr(_nWarnings));
				pReg->f_SetValue("TestsIgnored", NStr::CStr::fs_ToStr(_nIgnored));
				DMibConOut("{}", Registry.f_GenerateStr());
			}

			ETestNeedReportFlag CRegistryTestResults::f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags)
			{
				bint bReport = false;
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

			void CRegistryTestResults::fs_DecodeResult(NRegistry::CRegistry_CStr const &_Registry, CTestResult &_Results)
			{
				_Results.m_ResultID = _Registry.f_GetThisValue().f_ToInt(_Results.m_ResultID);
				_Results.m_ThreadID = _Registry.f_GetValueNoPath("Thread").f_ToInt(_Results.m_ThreadID);

				_Results.m_TestPath = _Registry.f_GetValueNoPath("Path");
				_Results.m_Message = _Registry.f_GetValueNoPath("Message");
				_Results.m_Values = _Registry.f_GetValueNoPath("Values");
				_Results.m_File = _Registry.f_GetValueNoPath("File");
				_Results.m_Line = _Registry.f_GetValueNoPath("Line").f_ToInt(_Results.m_Line);
				_Results.m_FailureAction = fs_FailureActionFromStr(_Registry.f_GetValueNoPath("FailureAction"));
				_Results.m_Result = fs_TestResultFromStr(_Registry.f_GetValueNoPath("TestResult"));
				_Results.m_CheckType = fs_CheckTypeFromStr(_Registry.f_GetValueNoPath("CheckType"));
				_Results.m_Flags = fs_TestFlagsFromStr(_Registry.f_GetValueNoPath("TestFlags"));
				_Results.m_ExtraMultiLineReportData = _Registry.f_GetValueNoPath("ExtraData");
			}

			void CRegistryTestResults::f_ReportResult
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
				uint32 &CurrentID = *m_CurrentID;
				CurrentID = m_NextID.f_FetchAdd(1);
				NRegistry::CRegistry_CStr Registry;
				NRegistry::CRegistry_CStr *pReg = Registry.f_CreateChild("Result");
				pReg->f_SetThisValue(NStr::CStr::fs_ToStr(CurrentID));
				pReg->f_SetValue("Thread", NStr::CStr::fs_ToStr(NSys::fg_Thread_GetCurrentUID()));
				pReg->f_SetValue("Path", _TestPath);
				pReg->f_SetValue("Message", _Message);
				pReg->f_SetValue("Values", _Values);
				pReg->f_SetValue("File", _Location.m_File);
				pReg->f_SetValue("Line", NStr::CStr::fs_ToStr(_Location.m_Line));
				pReg->f_SetValue("FailureAction", fs_FailureActionToStr(_FailureAction));
				pReg->f_SetValue("TestResult", fs_TestResultToStr(_Result));
				pReg->f_SetValue("CheckType", fs_CheckTypeToStr(_CheckType));
				pReg->f_SetValue("TestFlags", fs_TestFlagsToStr(_Flags));
				pReg->f_SetValue("ExtraData", _ExtraMultiLineReportData);
				DMibConOut("{}", Registry.f_GenerateStr());
			}
			void CRegistryTestResults::f_ReportSuite(const NStr::CStr &_TestPath, const NContainer::TCMap<NStr::CStr> &_TestGroups, CTestLocation const &_Location)
			{
				NRegistry::CRegistry_CStr Registry;
				NRegistry::CRegistry_CStr *pReg = Registry.f_CreateChild("Category");
				pReg->f_SetValue("Path", _TestPath);
				pReg->f_SetValue("File", _Location.m_File);
				pReg->f_SetValue("Line", NStr::CStr::fs_ToStr(_Location.m_Line));
				pReg->f_SetValue("Thread", NStr::CStr::fs_ToStr(NSys::fg_Thread_GetCurrentUID()));
				NRegistry::CRegistry_CStr *pGroups = pReg->f_CreateChild("Groups", true);
				NMisc::fg_ForEach
					(
						_TestGroups
						, [&](NContainer::CMapNoData const &_Group)
						{
							NRegistry::CRegistry_CStr *pGroup = pGroups->f_CreateChild("Group", true);
							pGroup->f_SetThisValue(NContainer::TCMap<NStr::CStr>::fs_GetKey(_Group));
						}
					)
				;
				DMibConOut("{}", Registry.f_GenerateStr());
			}

			void CRegistryTestResults::fs_DecodePerformanceResults(NRegistry::CRegistry_CStr const &_Registry, CTestPerformanceResults &_Results)
			{
				_Results.m_Tolerance = _Registry.f_GetValueNoPath("Tolerance").f_ToFloat(fp64(1.0));

				for (auto Iter = _Registry.f_GetChildIterator("Result"); Iter && Iter->f_GetName() == "Result"; ++Iter)
				{
					auto &Result = _Results.m_Results.f_Insert();
					Result.m_Name = Iter->f_GetThisValue();
					Result.m_MeasureType = fs_MeasureTypeFromStr(Iter->f_GetValueNoPath("MeasureType"));
					Result.m_nIterations = Iter->f_GetValueNoPath("NumIterations").f_ToInt(Result.m_nIterations);
					Result.m_nRepetitions = Iter->f_GetValueNoPath("NumRepetitions").f_ToInt(Result.m_nRepetitions);
					Result.m_nContributingThreads = Iter->f_GetValueNoPath("NumContributingThreads").f_ToInt(Result.m_nContributingThreads);

					for (auto StatIter = Iter->f_GetChildNoPath("PerformanceStats")->f_GetChildIterator("Stat"); StatIter && StatIter->f_GetName() == "Stat"; ++StatIter)
					{
						if (StatIter->f_GetThisValue() == "Time")
							fsp_DecodeStatistics(*StatIter, Result.m_Time);
						else if (StatIter->f_GetThisValue() == "Cycles")
							fsp_DecodeStatistics(*StatIter, Result.m_Cycles);
						else
							fsp_DecodeStatistics(*StatIter, Result.m_PerformanceCounters[StatIter->f_GetThisValue()]);
					}

				}
			}

			void CRegistryTestResults::f_PerformanceResults(CTestPerformanceResults const &_Results)
			{
				uint32 const &CurrentID = *m_CurrentID;

				NRegistry::CRegistry_CStr Registry;
				NRegistry::CRegistry_CStr *pReg = Registry.f_CreateChild("ResultsPerformance");
				pReg->f_SetThisValue(NStr::CStr::fs_ToStr(CurrentID));
				pReg->f_SetValue("Tolerance", NStr::CStr::fs_ToStr(_Results.m_Tolerance));

				mint nResults = _Results.m_Results.f_GetLen();
				for (mint i = 0; i < nResults; ++i)
				{
					CTestPerformanceResult const &Result = _Results.m_Results[i];
					NRegistry::CRegistry_CStr *pResultReg = pReg->f_CreateChild("Result", true);
					pResultReg->f_SetThisValue(Result.m_Name);
					pResultReg->f_SetValue("MeasureType", fs_MeasureTypeToStr(Result.m_MeasureType));
					pResultReg->f_SetValue("NumIterations", NStr::CStr::fs_ToStr(Result.m_nIterations));
					pResultReg->f_SetValue("NumRepetitions", NStr::CStr::fs_ToStr(Result.m_nRepetitions));
					pResultReg->f_SetValue("NumContributingThreads", NStr::CStr::fs_ToStr(Result.m_nContributingThreads));
					
					NRegistry::CRegistry_CStr *pPerfReg = pResultReg->f_CreateChild("PerformanceStats", true);
					fsp_AddStatistics(pPerfReg, "Time", Result.m_Time);
					fsp_AddStatistics(pPerfReg, "Cycles", Result.m_Cycles);

					for (auto Iter = Result.m_PerformanceCounters.f_GetIterator(); Iter; ++Iter)
					{
						fsp_AddStatistics(pPerfReg, Iter.f_GetKey(), *Iter);
					}
				}
					
				DMibConOut("{}", Registry.f_GenerateStr());
			}

			void CRegistryTestResults::fs_DecodeMemoryResults(NRegistry::CRegistry_CStr const &_Registry, CTestMemoryResults &_Results)
			{
				_Results.m_Tolerance = _Registry.f_GetValueNoPath("Tolerance").f_ToFloat(fp64(1.0));

				for (auto Iter = _Registry.f_GetChildIterator("Result"); Iter && Iter->f_GetName() == "Result"; ++Iter)
				{
					auto &Result = _Results.m_Results.f_Insert();
					Result.m_Name = Iter->f_GetThisValue();
					Result.m_MeasureType = fs_MeasureTypeFromStr(Iter->f_GetValueNoPath("MeasureType"));
					Result.m_nIterations = Iter->f_GetValueNoPath("NumIterations").f_ToInt(Result.m_nIterations);
					Result.m_nRepetitions = Iter->f_GetValueNoPath("NumRepetitions").f_ToInt(Result.m_nRepetitions);

					for (auto StatIter = Iter->f_GetChildNoPath("MemoryStats")->f_GetChildIterator("AllocationType"); StatIter && StatIter->f_GetName() == "AllocationType"; ++StatIter)
					{
						if (StatIter->f_GetThisValue() == "All")
							fsp_DecodeMemoryStatistics(*StatIter, Result.m_AllAllocations);
						else
							fsp_DecodeMemoryStatistics(*StatIter, Result.m_PerAllocationType[StatIter->f_GetThisValue()]);
					}

				}
			}

			void CRegistryTestResults::f_MemoryResults(CTestMemoryResults const &_Results)
			{
				uint32 const &CurrentID = *m_CurrentID;

				NRegistry::CRegistry_CStr Registry;
				NRegistry::CRegistry_CStr *pReg = Registry.f_CreateChild("ResultsMemory");
				pReg->f_SetThisValue(NStr::CStr::fs_ToStr(CurrentID));
				pReg->f_SetValue("Tolerance", NStr::CStr::fs_ToStr(_Results.m_Tolerance));

				mint nResults = _Results.m_Results.f_GetLen();
				for (mint i = 0; i < nResults; ++i)
				{
					CTestMemoryResult const &Result = _Results.m_Results[i];
					NRegistry::CRegistry_CStr *pResultReg = pReg->f_CreateChild("Result", true);
					pResultReg->f_SetThisValue(Result.m_Name);
					pResultReg->f_SetValue("MeasureType", fs_MeasureTypeToStr(Result.m_MeasureType));
					pResultReg->f_SetValue("NumIterations", NStr::CStr::fs_ToStr(Result.m_nIterations));
					pResultReg->f_SetValue("NumRepetitions", NStr::CStr::fs_ToStr(Result.m_nRepetitions));
					NRegistry::CRegistry_CStr *pPerfReg = pResultReg->f_CreateChild("MemoryStats", true);
					fsp_AddMemoryStatistics(pPerfReg, "All", Result.m_AllAllocations);

					for (auto Iter = Result.m_PerAllocationType.f_GetIterator(); Iter; ++Iter)
					{
						fsp_AddMemoryStatistics(pPerfReg, Iter.f_GetKey(), *Iter);
					}
				}
					
				DMibConOut("{}", Registry.f_GenerateStr());
			}

			/***************************************************************************************************\
			|ÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃ|
			| CCategoryLister																					|
			|___________________________________________________________________________________________________|
			\***************************************************************************************************/

			void CCategoryLister::f_ReportHeader(ETestReportFlag _ReportFlags)
			{
			}
			void CCategoryLister::f_ReportFooter(mint _nTestsTotal, mint _nSuccess, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored)
			{
			}
			void CCategoryLister::f_ReportResult
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
			void CCategoryLister::f_ReportSuite(const NStr::CStr &_TestPath, const NContainer::TCMap<NStr::CStr> &_TestGroups, CTestLocation const &_Location)
			{
				NStr::CStr Groups;
				NMisc::fg_ForEach
					(
						_TestGroups
						, [&](NContainer::CMapNoData const &_Group)
						{
							NStr::fg_AddStrSep(Groups, NContainer::TCMap<NStr::CStr>::fs_GetKey(_Group), ',');
						}
					)
				;
				if (Groups.f_IsEmpty())
					DMibConOut("{}" DMibNewLine, _TestPath);
				else
					DMibConOut("{} : {}" DMibNewLine, _TestPath << Groups);
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

	}
}

