// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Test/Test>
#include <Mib/Time/TimeMeasure>

namespace NMib::NTest
{
	class CTestPerformanceMeasure
	{
		NTime::TCCyclesTimeMeasure<false> m_Cycles;
		NTime::CTimeMeasure m_Timer;
		uint64 m_nIterations;

		NContainer::TCVector<fp64> m_CyclesHistory;
		fp64 m_CyclesSum;
		fp64 m_MinCycles;
		fp64 m_MaxCycles;

		NContainer::TCVector<fp64> m_TimerHistory;
		fp64 m_TimerSum;
		fp64 m_MinTimer;
		fp64 m_MaxTimer;

		uint64 m_nIterationsSum;
		uint64 m_nRepetitions;
		NStr::CStr m_Name;

		uint32 m_nContributingThreads;

		static NTime::TCCyclesTimeMeasure<false> ms_CyclesCorrection;
		static NTime::CTimeMeasure ms_TimerCorrection;
		static bool ms_CalculateCorrection;

		void fp_CalcRepetition();

	public:
		CTestPerformanceMeasure(NStr::CStr const &_Name);

		void f_AddThreadResult(CTestPerformanceMeasure const &_Right);
		inline_never void f_Start();
		inline_never void f_Stop(uint64 _nIterations, uint32 _nContributingThreads = 1);
		inline_never void f_NewRepetition();
		NStr::CStr const &f_Name();
		uint64 f_Iterations();
		uint64 f_Repetitions();
		uint32 f_ContributingThreads();
		fp64 f_CyclesMin();
		fp64 f_CyclesMax();
		fp64 f_CyclesAverage();
		fp64 f_CyclesMedian();
		fp64 f_CyclesStdDev();
		fp64 f_TimeMin();
		fp64 f_TimeMax();
		fp64 f_TimeAverage();
		fp64 f_TimeMedian();
		fp64 f_TimeStdDev();

	};

	template <typename t_CTimer, typename t_CIterations>
	class TCTestMeasureScope
	{
	public:
		t_CTimer &m_Timer;
		t_CIterations const &m_Iterations;

		TCTestMeasureScope(t_CTimer &_Timer, t_CIterations const &_Iterations)
			: m_Iterations(_Iterations)
			, m_Timer(_Timer)
		{
			_Timer.f_Start();
		}

		~TCTestMeasureScope()
		{
			m_Timer.f_Stop(m_Iterations);
		}
	};

	template <typename t_CTimer, typename t_CIterations>
	class TCTestMeasureScopeWithThread
	{
	public:
		t_CTimer &m_Timer;
		t_CIterations const &m_Iterations;
		umint m_nThreads;

		TCTestMeasureScopeWithThread(t_CTimer &_Timer, t_CIterations const &_Iterations, umint _nThreads)
			: m_Iterations(_Iterations)
			, m_Timer(_Timer)
			, m_nThreads(_nThreads)
		{
			_Timer.f_Start();
		}

		~TCTestMeasureScopeWithThread()
		{
			m_Timer.f_Stop(m_Iterations, m_nThreads);
		}
	};

#	define DMibTestScopeMeasure(_Measure, _nIterations) NMib::NTest::TCTestMeasureScope<decltype(_Measure), decltype(_nIterations)> TimerScope(_Measure, _nIterations)
#	define DMibTestScopeMeasureThreads(_Measure, _nIterations, _nThreads) NMib::NTest::TCTestMeasureScopeWithThread<decltype(_Measure), decltype(_nIterations)> TimerScope(_Measure, _nIterations, _nThreads)

#	ifndef DMibPNoShortCuts
#		define DTestScopeMeasure DMibTestScopeMeasure
#		define DTestScopeMeasureThreads DMibTestScopeMeasureThreads
#	endif

	class CTestPerformance
	{
		CTestPerformanceResults m_Results;
		NContainer::TCMap<fp64, umint> m_References;
		bool m_bModifyDescription;
		CTestPerformanceResult &fp_Add(CTestPerformanceMeasure &_Timer);
		NStr::CStr fp_GetOutput() const;
	public:
#if !defined(DConfig_Optimized) && !defined(DConfig_Release) && !defined(DConfig_Profile)
		bool f_IsIgnored() const;
#endif
		CTestPerformance(fp64 const &_Tolerance, bool _bModifyDescription = true);
		void f_AddBaseline(CTestPerformanceMeasure &_Timer);
		void f_AddReference(CTestPerformanceMeasure &_Timer);
		void f_AddDebug(CTestPerformanceMeasure &_Timer);
		void f_Add(CTestPerformanceMeasure &_Timer);

		void f_Add(CTestPerformanceResult const &_Result);

		void f_TestReport(CTestResults &_Results) const;
		operator bool () const;
		NStr::CStr f_ModifyDescription(NStr::CStr const &_Description);
		template <typename t_CFormatter>
		int f_GetStringFormatType(t_CFormatter &_Formatter);
		template <typename t_CFormatter>
		// This crashes GCC
		// auto f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, ""))
		NStr::CStrFormatTypeClassifier_String f_CreateStringFormatter(t_CFormatter &_Formatter) const
		{
			return NStr::fg_CreateStringFormatter(_Formatter, fg_ByValue(fp_GetOutput()));
		}
	};
}
