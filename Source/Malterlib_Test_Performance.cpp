// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Test>
#include <Mib/Test/Recursive>
#include <Mib/Test/Performance>

namespace NMib::NTest
{
	/***************************************************************************************************\
	|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
	| Timer																								|
	|___________________________________________________________________________________________________|
	\***************************************************************************************************/

	NTime::TCCyclesTimeMeasure<false> CTestPerformanceMeasure::msp_CyclesCorrection(0);
	NTime::CTimeMeasure CTestPerformanceMeasure::msp_TimerCorrection(0);
	bool CTestPerformanceMeasure::msp_CalculateCorrection = true;

	CTestPerformanceMeasure::CTestPerformanceMeasure(NStr::CStr const &_Name)
		: mp_MinCycles(fp64::fs_Inf())
		, mp_MinTimer(fp64::fs_Inf())
		, mp_MaxCycles(0.0)
		, mp_MaxTimer(0.0)
		, mp_TimerSum(0.0)
		, mp_CyclesSum(0.0)
		, mp_nRepetitions(0)
		, mp_nIterationsSum(0)
		, mp_Name(_Name)
		, mp_Cycles(0)
		, mp_Timer(0)
		, mp_nIterations(0)
		, mp_nContributingThreads(0)
	{
		if (msp_CalculateCorrection)
		{
			msp_CalculateCorrection = false;
			CTestPerformanceMeasure Timer("");
			uint64 nIter = 1;
			for (umint iSample = 0; iSample < 100; ++iSample)
				DMibTestScopeMeasure(Timer, nIter);
			msp_CyclesCorrection = Timer.mp_MinCycles.f_ToInt();
			msp_TimerCorrection = Timer.mp_MinTimer.f_ToInt();
			DMibFastCheck(msp_CyclesCorrection.f_GetCycles() >= 0);
			DMibFastCheck(msp_TimerCorrection.f_GetTime() >= 0.0);
		}
	}

	void CTestPerformanceMeasure::fp_CalcRepetition()
	{
		uint64 nIterations = mp_nIterations;
		mp_nIterationsSum += nIterations;
		++mp_nRepetitions;

		fp64 CyclesPerIteration = fp64(mp_Cycles.f_GetCycles()) / fp64(nIterations);
		fp64 TimePerIteration = fp64(mp_Timer.f_GetTime()) / fp64(nIterations);
		mp_CyclesSum += CyclesPerIteration;
		mp_TimerSum += TimePerIteration;
		mp_CyclesHistory.f_Insert(CyclesPerIteration);
		mp_TimerHistory.f_Insert(TimePerIteration);

		if (CyclesPerIteration < mp_MinCycles)
			mp_MinCycles = CyclesPerIteration;
		if (TimePerIteration < mp_MinTimer)
			mp_MinTimer = TimePerIteration;
		if (CyclesPerIteration > mp_MaxCycles)
			mp_MaxCycles = CyclesPerIteration;
		if (TimePerIteration > mp_MaxTimer)
			mp_MaxTimer = TimePerIteration;
	}

	inline_never void CTestPerformanceMeasure::f_NewRepetition()
	{
		fp_CalcRepetition();
		mp_nIterations = 0;
		mp_Cycles.f_Reset(0);
		mp_Timer.f_Reset(0);
	}

	void CTestPerformanceMeasure::f_AddThreadResult(CTestPerformanceMeasure const &_Right)
	{
		DMibRequire(_Right.mp_nRepetitions == 1);
		mp_Timer += _Right.mp_Timer;
		mp_Cycles += _Right.mp_Cycles;
		mp_nIterations += _Right.mp_nIterations;
		++mp_nContributingThreads;
	}

	inline_never void CTestPerformanceMeasure::f_Start()
	{
		NMib::NAtomic::fg_MemoryFence();
		mp_Timer.f_Start();
		mp_Cycles.f_Start();
		NMib::NAtomic::fg_MemoryFence();
	}

	inline_never void CTestPerformanceMeasure::f_Stop(uint64 _nIterations, uint32 _nContributingThreads)
	{
		NMib::NAtomic::fg_MemoryFence();
		mp_Cycles.f_Stop();
		mp_Timer.f_Stop();
		NMib::NAtomic::fg_MemoryFence();

		mp_Cycles -= msp_CyclesCorrection;
		mp_Timer -= msp_TimerCorrection;

		if (mp_Cycles.f_GetCycles() < 0)
			mp_Cycles.f_Reset(0);

		if (mp_Timer.f_GetTicks() < 0)
			mp_Timer.f_Reset(0);

		mp_Cycles *= _nContributingThreads;
		mp_Timer *= _nContributingThreads;

		DMibRequire(_nIterations != 0);
		mp_nIterations = _nIterations;
		mp_nContributingThreads += _nContributingThreads;
		fp_CalcRepetition();
	}

	uint32 CTestPerformanceMeasure::f_ContributingThreads()
	{
		return mp_nContributingThreads / mp_nRepetitions;
	}

	uint64 CTestPerformanceMeasure::f_Repetitions()
	{
		return mp_nRepetitions;
	}

	uint64 CTestPerformanceMeasure::f_Iterations()
	{
		return mp_nIterationsSum / mp_nRepetitions;
	}

	NStr::CStr const &CTestPerformanceMeasure::f_Name()
	{
		return mp_Name;
	}

	fp64 CTestPerformanceMeasure::f_CyclesMin()
	{
		return mp_MinCycles;
	}

	fp64 CTestPerformanceMeasure::f_CyclesMax()
	{
		return mp_MaxCycles;
	}

	fp64 CTestPerformanceMeasure::f_CyclesAverage()
	{
		return mp_CyclesSum / fp64(mp_nRepetitions);
	}

	fp64 CTestPerformanceMeasure::f_CyclesStdDev()
	{
		fp64 Average = mp_CyclesSum / fp64(mp_nRepetitions);
		umint nHistory = mp_CyclesHistory.f_GetLen();
		if (nHistory < 2)
			return 0.0;
		fp64 SqrSum = 0.0;
		for (umint iHistory = 0; iHistory < nHistory; ++iHistory)
		{
			fp64 Delta = mp_CyclesHistory[iHistory] - Average;
			SqrSum += Delta * Delta;
		}

		return (SqrSum / fp64(nHistory - 1)).f_Sqrt();
	}

	fp64 CTestPerformanceMeasure::f_CyclesMedian()
	{
		umint nHistory = mp_CyclesHistory.f_GetLen();
		if (nHistory < 1)
			return 0.0;
		NContainer::TCVector<fp64> MedianList = mp_CyclesHistory;
		MedianList.f_Sort();
		return MedianList[nHistory / 2];
	}

	fp64 CTestPerformanceMeasure::f_TimeMin()
	{
		return mp_MinTimer;
	}

	fp64 CTestPerformanceMeasure::f_TimeMax()
	{
		return mp_MaxTimer;
	}

	fp64 CTestPerformanceMeasure::f_TimeAverage()
	{
		return mp_TimerSum / fp64(mp_nRepetitions);
	}

	fp64 CTestPerformanceMeasure::f_TimeStdDev()
	{
		fp64 Average = mp_TimerSum / fp64(mp_nRepetitions);
		umint nHistory = mp_TimerHistory.f_GetLen();
		if (nHistory < 2)
			return 0.0;
		fp64 SqrSum = 0.0;
		for (umint iHistory = 0; iHistory < nHistory; ++iHistory)
		{
			fp64 Delta = mp_TimerHistory[iHistory] - Average;
			SqrSum += Delta * Delta;
		}

		return (SqrSum / fp64(nHistory - 1)).f_Sqrt();
	}

	fp64 CTestPerformanceMeasure::f_TimeMedian()
	{
		umint nHistory = mp_TimerHistory.f_GetLen();
		if (nHistory < 1)
			return 0.0;
		NContainer::TCVector<fp64> MedianList = mp_TimerHistory;

		MedianList.f_Sort();

		return MedianList[nHistory / 2];
	}

	/***************************************************************************************************\
	|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
	| Results																							|
	|___________________________________________________________________________________________________|
	\***************************************************************************************************/

	CTestPerformanceResult &CTestPerformance::fp_Add(CTestPerformanceMeasure &_Timer)
	{
		DMibRequire(_Timer.f_Repetitions() > 0);
		CTestPerformanceResult &Result = mp_Results.m_Results.f_Insert();
		Result.m_Name = _Timer.f_Name();
		Result.m_MeasureType = ETestMeasureType_Normal;
		Result.m_nIterations = _Timer.f_Iterations();
		Result.m_nContributingThreads = _Timer.f_ContributingThreads();
		Result.m_nRepetitions = _Timer.f_Repetitions();
		Result.m_Cycles.m_Min = _Timer.f_CyclesMin();
		Result.m_Cycles.m_Max = _Timer.f_CyclesMax();
		Result.m_Cycles.m_Average = _Timer.f_CyclesAverage();
		Result.m_Cycles.m_Median = _Timer.f_CyclesMedian();
		Result.m_Cycles.m_StdDev = _Timer.f_CyclesStdDev();
		Result.m_Time.m_Min = _Timer.f_TimeMin();
		Result.m_Time.m_Max = _Timer.f_TimeMax();
		Result.m_Time.m_Average = _Timer.f_TimeAverage();
		Result.m_Time.m_Median = _Timer.f_TimeMedian();
		Result.m_Time.m_StdDev = _Timer.f_TimeStdDev();
		return Result;
	}

#if !defined(DConfig_Optimized) && !defined(DConfig_Release) && !defined(DConfig_Profile)
	bool CTestPerformance::f_IsIgnored() const
	{
		return true;
	}
#endif
	CTestPerformance::CTestPerformance(fp64 const &_Tolerance, bool _bModifyDescription)
	{
		mp_bModifyDescription = _bModifyDescription;
		mp_Results.m_Tolerance = _Tolerance;
	}

	void CTestPerformance::f_AddReference(CTestPerformanceMeasure &_Timer)
	{
		CTestPerformanceResult &Result = fp_Add(_Timer);
		Result.m_MeasureType = ETestMeasureType_Reference;
		mp_References[Result.m_Cycles.m_Average] = &Result - mp_Results.m_Results.f_GetArray();
	}

	void CTestPerformance::f_AddBaseline(CTestPerformanceMeasure &_Timer)
	{
		CTestPerformanceResult &Result = fp_Add(_Timer);
		Result.m_MeasureType = ETestMeasureType_Baseline;
	}

	void CTestPerformance::f_AddDebug(CTestPerformanceMeasure &_Timer)
	{
		CTestPerformanceResult &Result = fp_Add(_Timer);
		Result.m_MeasureType = ETestMeasureType_Debug;
	}

	void CTestPerformance::f_Add(CTestPerformanceMeasure &_Timer)
	{
		fp_Add(_Timer);
	}

	void CTestPerformance::f_Add(CTestPerformanceResult const &_Result)
	{
		CTestPerformanceResult &Result = mp_Results.m_Results.f_Insert(_Result);
		if (Result.m_MeasureType == ETestMeasureType_Reference)
			mp_References[Result.m_Cycles.m_Average] = &Result - mp_Results.m_Results.f_GetArray();
	}

	void CTestPerformance::f_TestReport(CTestResults &_Results) const
	{
		_Results.f_PerformanceResults(mp_Results);
	}

	CTestPerformance::operator bool () const
	{
#if !defined(DConfig_Optimized) && !defined(DConfig_Release) && !defined(DConfig_Profile)
		return true;
#else
		if (mp_References.f_IsEmpty())
			return true;
		fp64 Reference = mp_References.fs_GetKey(mp_References.f_FindSmallest());
		umint nResults = mp_Results.m_Results.f_GetLen();
		fp64 BestValue = 0;
		for (umint i = 0; i < nResults; ++i)
		{
			CTestPerformanceResult const &Result = mp_Results.m_Results[i];
			if (Result.m_MeasureType == ETestMeasureType_Normal)
			{
				fp64 ThisValue = Reference / Result.m_Cycles.m_Average;
				if (ThisValue > BestValue)
					BestValue = ThisValue;
			}
		}
		return BestValue > mp_Results.m_Tolerance;
#endif
	}

	NStr::CStr CTestPerformance::f_ModifyDescription(NStr::CStr const &_Description)
	{
		if (mp_bModifyDescription)
		{
			umint nResults = mp_Results.m_Results.f_GetLen();
			if (nResults > 0)
				return NStr::CStr::CFormat("{}({})") << _Description << mp_Results.m_Results[0].m_nIterations;
		}
		return _Description;
	}

	NStr::CStr CTestPerformance::fp_GetOutput() const
	{
		NStr::CStr Output;
		if (mp_References.f_IsEmpty())
		{
			if (mp_Results.m_Results.f_IsEmpty())
				Output = "No results";
			else
			{
				fp64 Throughput;
				umint nResults = mp_Results.m_Results.f_GetLen();
				bool bFound = false;
				for (umint iResult = 0; iResult < nResults; ++iResult)
				{
					CTestPerformanceResult const &Result = mp_Results.m_Results[iResult];
					if (Result.m_MeasureType == ETestMeasureType_Normal)
					{
						Throughput = fp64(Result.m_nContributingThreads) / Result.m_Time.m_Min;
						bFound = true;
						break;
					}
				}

				if (bFound)
					Output = NStr::fg_Format("{fe1} it/s", Throughput);
				else
					Output = "No value to compare reference to";
			}
		}
		else
		{
			fp64 BestValue = 0;
			umint const *pBestRefIndex = mp_References.f_FindSmallest();
			CTestPerformanceResult const *pBestRef = &mp_Results.m_Results[*pBestRefIndex];
			CTestPerformanceResult const *pBest = nullptr;
			fp64 Reference = pBestRef->m_Cycles.m_Average;
			umint nResults = mp_Results.m_Results.f_GetLen();
			for (umint iResult = 0; iResult < nResults; ++iResult)
			{
				CTestPerformanceResult const &Result = mp_Results.m_Results[iResult];
				if (Result.m_MeasureType == ETestMeasureType_Normal)
				{
					fp64 ThisValue = Reference / Result.m_Cycles.m_Average;
					if (ThisValue > BestValue)
					{
						BestValue = ThisValue;
						pBest = &Result;
					}
				}
			}

			if (pBest)
			{
				Output
					= NStr::CStr::CFormat("{fn3,fe3}   {} {sz9,fn1,fe1}   {} {sz9,fn1,fe1}")
					<< BestValue
					<< pBestRef->m_Name
					<< pBestRef->m_Cycles.m_Min
					<< pBest->m_Name
					<< pBest->m_Cycles.m_Min
				;
			}
			else
				Output = "No value to compare reference to";
		}

		return Output;
	}
}
