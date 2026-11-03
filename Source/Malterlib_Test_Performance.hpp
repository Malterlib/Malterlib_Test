// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTest
{
	template <typename t_CTimer, typename t_CIterations>
	TCTestMeasureScope<t_CTimer, t_CIterations>::TCTestMeasureScope(t_CTimer &_Timer, t_CIterations const &_Iterations)
		: m_Iterations(_Iterations)
		, m_Timer(_Timer)
	{
		_Timer.f_Start();
	}

	template <typename t_CTimer, typename t_CIterations>
	TCTestMeasureScope<t_CTimer, t_CIterations>::~TCTestMeasureScope()
	{
		m_Timer.f_Stop(m_Iterations);
	}

	template <typename t_CTimer, typename t_CIterations>
	TCTestMeasureScopeWithThread<t_CTimer, t_CIterations>::TCTestMeasureScopeWithThread(t_CTimer &_Timer, t_CIterations const &_Iterations, umint _nThreads)
		: m_Iterations(_Iterations)
		, m_Timer(_Timer)
		, m_nThreads(_nThreads)
	{
		_Timer.f_Start();
	}

	template <typename t_CTimer, typename t_CIterations>
	TCTestMeasureScopeWithThread<t_CTimer, t_CIterations>::~TCTestMeasureScopeWithThread()
	{
		m_Timer.f_Stop(m_Iterations, m_nThreads);
	}

	template <typename tf_CFormatter>
	NStr::CStrFormatTypeClassifier_String CTestPerformance::f_CreateStringFormatter(tf_CFormatter &_Formatter) const
	{
		return NStr::fg_CreateStringFormatter(_Formatter, fg_ByValue(fp_GetOutput()));
	}
}
