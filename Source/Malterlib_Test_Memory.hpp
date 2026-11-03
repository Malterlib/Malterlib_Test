// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTest
{
	template <CStatAffectFlagUnderlying tf_Flags>
	void CTestMemoryMeasure::CAllocatorStats::f_TransferToStats(CTestMemoryStats &_Stats) const
	{
		fsp_TransferStat<tf_Flags>(_Stats.m_nAllocations, m_nAllocations);
		fsp_TransferStat<tf_Flags>(_Stats.m_nFree, m_nFree);
		fsp_TransferStat<tf_Flags>(_Stats.m_nResize, m_nResize);
		fsp_TransferStat<tf_Flags>(_Stats.m_nRealloc, m_nRealloc);
		fsp_TransferStat<tf_Flags>(_Stats.m_nGetSize, m_nGetSize);
		fsp_TransferStat<tf_Flags>(_Stats.m_nProtect, m_nProtect);
		fsp_TransferStat<tf_Flags>(_Stats.m_nCommit, m_nCommit);
		fsp_TransferStat<tf_Flags>(_Stats.m_nDecommit, m_nDecommit);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesAlloc, m_BytesAlloc);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesFree, m_BytesFree);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesResize, m_BytesResize);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesRealloc, m_BytesRealloc);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesProtect, m_BytesProtect);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesCommit, m_BytesCommit);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesDecommit, m_BytesDecommit);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesOverhead, fp64(m_BytesOverhead) / fp64(1000.0));
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesPotentialWaste, m_BytesPotentialWaste);
		fsp_TransferStat<tf_Flags>(_Stats.m_BytesMaxAlloc, m_BytesMaxAlloc);
		fsp_TransferStat<tf_Flags>(_Stats.m_AtBytesMaxAlloc.m_BytesOverhead, fp64(m_BytesOverheadAtMaxAlloc) / fp64(1000.0));
		fsp_TransferStat<tf_Flags>(_Stats.m_AtBytesMaxAlloc.m_BytesPotentialWaste, m_BytesPotentialWasteAtMaxAlloc);
		fsp_TransferStat<tf_Flags>(_Stats.m_AtBytesMaxAlloc.m_nAllocations, m_nAllocationsAtMaxAlloc);
	}

	template <CStatAffectFlagUnderlying tf_Flags>
	void CTestMemoryMeasure::CAllocatorStats::fsp_TransferStat(CTestStats &_Stat, fp64 _Value)
	{
		if constexpr (tf_Flags & EStatAffectFlag_Average)
			_Stat.m_Average = _Value;
		if constexpr ((tf_Flags & EStatAffectFlag_Min) != 0)
			_Stat.m_Min = _Value;
		if constexpr ((tf_Flags & EStatAffectFlag_Max) != 0)
			_Stat.m_Max = _Value;
		if constexpr ((tf_Flags & EStatAffectFlag_StdDev) != 0)
			_Stat.m_StdDev = _Value;
		if constexpr ((tf_Flags & EStatAffectFlag_Median) != 0)
			_Stat.m_Median = _Value;
	}

	template <typename tf_CFormatter>
	NStr::CStrFormatTypeClassifier_String CTestMemory::f_CreateStringFormatter(tf_CFormatter &_Formatter) const
	{
		return NStr::fg_CreateStringFormatter(_Formatter, fg_ByValue(fp_GetOutput()));
	}
}
