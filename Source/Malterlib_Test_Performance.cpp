// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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


	NTime::TCCycles<false> CTestPerformanceMeasure::ms_CyclesCorrection;
	NTime::CTimer CTestPerformanceMeasure::ms_TimerCorrection;
	bool CTestPerformanceMeasure::ms_CalculateCorrection = true;

	CTestPerformanceMeasure::CTestPerformanceMeasure(NStr::CStr const &_Name)
		: m_MinCycles(fp64::fs_Inf())
		, m_MinTimer(fp64::fs_Inf())
		, m_MaxCycles(0.0)
		, m_MaxTimer(0.0)
		, m_TimerSum(0.0)
		, m_CyclesSum(0.0)
		, m_nRepetitions(0)
		, m_nIterationsSum(0)
		, m_Name(_Name)
		, m_nIterations(0)
		, m_nContributingThreads(0)
	{
		if (ms_CalculateCorrection)
		{
			ms_CalculateCorrection = false;
			CTestPerformanceMeasure Timer("");
			uint64 nIter = 1;
			for (mint i = 0; i < 100; ++i)
				DMibTestScopeMeasure(Timer, nIter);
			ms_CyclesCorrection = Timer.m_MinCycles.f_ToInt();
			ms_TimerCorrection = Timer.m_MinTimer.f_ToInt();
		}
	}


	void CTestPerformanceMeasure::fp_CalcRepetition()
	{
		uint64 nIterations = m_nIterations;
		m_nIterationsSum += nIterations;
		++m_nRepetitions;

		fp64 CyclesPerIteration = fp64(m_Cycles.f_GetCycles()) / fp64(nIterations);
		fp64 TimePerIteration = fp64(m_Timer.f_GetTime()) / fp64(nIterations);
		m_CyclesSum += CyclesPerIteration;
		m_TimerSum += TimePerIteration;
		m_CyclesHistory.f_Insert(CyclesPerIteration);
		m_TimerHistory.f_Insert(TimePerIteration);

		if (CyclesPerIteration < m_MinCycles)
			m_MinCycles = CyclesPerIteration;
		if (TimePerIteration < m_MinTimer)
			m_MinTimer = TimePerIteration;
		if (CyclesPerIteration > m_MaxCycles)
			m_MaxCycles = CyclesPerIteration;
		if (TimePerIteration > m_MaxTimer)
			m_MaxTimer = TimePerIteration;
	}

	inline_never void CTestPerformanceMeasure::f_NewRepetition()
	{
		fp_CalcRepetition();
		m_nIterations = 0;
		m_Cycles.f_Reset();
		m_Timer = NTime::CTimer();
	}


	void CTestPerformanceMeasure::f_AddThreadResult(CTestPerformanceMeasure const &_Right)
	{
		DMibRequire(_Right.m_nRepetitions == 1);
		m_Timer += _Right.m_Timer;
		m_Cycles += _Right.m_Cycles;
		m_nIterations += _Right.m_nIterations;
		++m_nContributingThreads;
	}

	inline_never void CTestPerformanceMeasure::f_Start()
	{
		NMib::NAtomic::fg_MemoryFence();
		m_Timer.f_Start();
		m_Cycles.f_Start();
		NMib::NAtomic::fg_MemoryFence();
	}

	inline_never void CTestPerformanceMeasure::f_Stop(uint64 _nIterations, uint32 _nContributingThreads)
	{
		NMib::NAtomic::fg_MemoryFence();
		m_Cycles.f_Stop();
		m_Timer.f_Stop();
		NMib::NAtomic::fg_MemoryFence();

		m_Cycles -= ms_CyclesCorrection;
		m_Timer -= ms_TimerCorrection;

		m_Cycles *= _nContributingThreads;
		m_Timer *= _nContributingThreads;

		DMibRequire(_nIterations != 0);
		m_nIterations = _nIterations;
		m_nContributingThreads += _nContributingThreads;
		fp_CalcRepetition();
	}

	uint32 CTestPerformanceMeasure::f_ContributingThreads()
	{
		return m_nContributingThreads / m_nRepetitions;
	}

	uint64 CTestPerformanceMeasure::f_Repetitions()
	{
		return m_nRepetitions;
	}

	uint64 CTestPerformanceMeasure::f_Iterations()
	{
		return m_nIterationsSum / m_nRepetitions;
	}

	NStr::CStr const &CTestPerformanceMeasure::f_Name()
	{
		return m_Name;
	}

	fp64 CTestPerformanceMeasure::f_CyclesMin()
	{
		return m_MinCycles;
	}

	fp64 CTestPerformanceMeasure::f_CyclesMax()
	{
		return m_MaxCycles;
	}

	fp64 CTestPerformanceMeasure::f_CyclesAverage()
	{
		return m_CyclesSum / fp64(m_nRepetitions);
	}

	fp64 CTestPerformanceMeasure::f_CyclesStdDev()
	{
		fp64 Average = m_CyclesSum / fp64(m_nRepetitions);
		mint nHistory = m_CyclesHistory.f_GetLen();
		if (nHistory < 2)
			return 0.0;
		fp64 SqrSum = 0.0;
		for (mint i = 0; i < nHistory; ++i)
		{
			fp64 Delta = m_CyclesHistory[i] - Average;
			SqrSum += Delta*Delta;
		}

		return (SqrSum / fp64(nHistory -1)).f_Sqrt();
	}

	fp64 CTestPerformanceMeasure::f_CyclesMedian()
	{
		mint nHistory = m_CyclesHistory.f_GetLen();
		if (nHistory < 1)
			return 0.0;
		NContainer::TCVector<fp64> MedianList = m_CyclesHistory;
		MedianList.f_Sort();
		return MedianList[nHistory / 2];
	}

	fp64 CTestPerformanceMeasure::f_TimeMin()
	{
		return m_MinTimer;
	}

	fp64 CTestPerformanceMeasure::f_TimeMax()
	{
		return m_MaxTimer;
	}

	fp64 CTestPerformanceMeasure::f_TimeAverage()
	{
		return m_TimerSum / fp64(m_nRepetitions);
	}

	fp64 CTestPerformanceMeasure::f_TimeStdDev()
	{
		fp64 Average = m_TimerSum / fp64(m_nRepetitions);
		mint nHistory = m_TimerHistory.f_GetLen();
		if (nHistory < 2)
			return 0.0;
		fp64 SqrSum = 0.0;
		for (mint i = 0; i < nHistory; ++i)
		{
			fp64 Delta = m_TimerHistory[i] - Average;
			SqrSum += Delta*Delta;
		}

		return (SqrSum / fp64(nHistory -1)).f_Sqrt();
	}

	fp64 CTestPerformanceMeasure::f_TimeMedian()
	{
		mint nHistory = m_TimerHistory.f_GetLen();
		if (nHistory < 1)
			return 0.0;
		NContainer::TCVector<fp64> MedianList = m_TimerHistory;

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
		CTestPerformanceResult &Result = m_Results.m_Results.f_Insert();
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
		m_bModifyDescription = _bModifyDescription;
		m_Results.m_Tolerance = _Tolerance;
	}
	void CTestPerformance::f_AddReference(CTestPerformanceMeasure &_Timer)
	{
		CTestPerformanceResult &Result = fp_Add(_Timer);
		Result.m_MeasureType = ETestMeasureType_Reference;
		m_References[Result.m_Cycles.m_Average] = &Result - m_Results.m_Results.f_GetArray();
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
		CTestPerformanceResult &Result = m_Results.m_Results.f_Insert(_Result);
		if (Result.m_MeasureType == ETestMeasureType_Reference)
			m_References[Result.m_Cycles.m_Average] = &Result - m_Results.m_Results.f_GetArray();
	}

	void CTestPerformance::f_TestReport(CTestResults &_Results) const
	{
		_Results.f_PerformanceResults(m_Results);
	}
	CTestPerformance::operator bool () const
	{
#if !defined(DConfig_Optimized) && !defined(DConfig_Release) && !defined(DConfig_Profile)
		return true;
#else
		if (m_References.f_IsEmpty())
			return true;
		fp64 Reference = m_References.fs_GetKey(m_References.f_FindSmallest());
		mint nResults = m_Results.m_Results.f_GetLen();
		fp64 BestValue = 0;
		for (mint i = 0; i < nResults; ++i)
		{
			CTestPerformanceResult const &Result = m_Results.m_Results[i];
			if (Result.m_MeasureType == ETestMeasureType_Normal)
			{
				fp64 ThisValue = Reference / Result.m_Cycles.m_Average;
				if (ThisValue > BestValue)
				{
					BestValue = ThisValue;
				}
			}
		}
		return BestValue > m_Results.m_Tolerance;
#endif
	}

	NStr::CStr CTestPerformance::f_ModifyDescription(NStr::CStr const &_Description)
	{
		if (m_bModifyDescription)
		{
			mint nResults = m_Results.m_Results.f_GetLen();
			if (nResults > 0)
				return NStr::CStr::CFormat("{}({})") << _Description << m_Results.m_Results[0].m_nIterations;
		}
		return _Description;
	}

	NStr::CStr CTestPerformance::fp_GetOutput() const
	{
		NStr::CStr Output;
		if (m_References.f_IsEmpty())
		{
			if (m_Results.m_Results.f_IsEmpty())
				Output = "No results";
			else
			{
				fp64 Throughput;
				mint nResults = m_Results.m_Results.f_GetLen();
				bool bFound = false;
				for (mint i = 0; i < nResults; ++i)
				{
					CTestPerformanceResult const &Result = m_Results.m_Results[i];
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
			mint const *pBestRefIndex = m_References.f_FindSmallest();
			CTestPerformanceResult const *pBestRef = &m_Results.m_Results[*pBestRefIndex];
			CTestPerformanceResult const *pBest = nullptr;
			fp64 Reference = pBestRef->m_Cycles.m_Average;
			mint nResults = m_Results.m_Results.f_GetLen();
			for (mint i = 0; i < nResults; ++i)
			{
				CTestPerformanceResult const &Result = m_Results.m_Results[i];
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
				Output = NStr::CStr::CFormat("{fn3,fe3}   {} {sz9,fn1,fe1}   {} {sz9,fn1,fe1}") << BestValue << pBestRef->m_Name << pBestRef->m_Cycles.m_Min << pBest->m_Name << pBest->m_Cycles.m_Min;
			else
				Output = "No value to compare reference to";
		}

		return Output;
	}
}
