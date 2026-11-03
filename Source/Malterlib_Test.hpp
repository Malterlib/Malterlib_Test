// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTest
{
	template <typename tf_CStream>
	void CTestStats::f_Stream(tf_CStream &_Stream)
	{
		_Stream % m_Min;
		_Stream % m_Max;
		_Stream % m_Average;
		_Stream % m_Median;
		_Stream % m_StdDev;
	}

#if !DMibConfig_Tests_Enable
	template <typename t_CFunctionType>
	void CTestCategoryScope::operator % (t_CFunctionType const &_Function)
	{
	}
#endif
}
