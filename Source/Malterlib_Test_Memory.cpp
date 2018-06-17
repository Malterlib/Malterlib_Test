// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>
#include <Mib/Test/Memory>

namespace NMib
{
	namespace NTest
	{
		/***************************************************************************************************\
		|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
		| Stats manipulation																				|
		|___________________________________________________________________________________________________|
		\***************************************************************************************************/

		namespace 
		{
			template <CStatAffectFlagUnderlaying tf_Flags, typename tf_CFunctor>
			static void fg_OperateOnStatUnary(CTestStats &_Stat, tf_CFunctor &&_Functor)
			{
				if (tf_Flags & EStatAffectFlag_Average)
					_Functor(_Stat.m_Average);
				if (tf_Flags & EStatAffectFlag_Min)
					_Functor(_Stat.m_Min);
				if (tf_Flags & EStatAffectFlag_Max)
					_Functor(_Stat.m_Max);
				if (tf_Flags & EStatAffectFlag_StdDev)
					_Functor(_Stat.m_StdDev);
				if (tf_Flags & EStatAffectFlag_Median)
					_Functor(_Stat.m_Median);
			}

			template <CStatAffectFlagUnderlaying tf_Flags, typename tf_CFunctor>
			void fg_OperateOnStatsUnary(CTestMemoryStats &_Stats, tf_CFunctor &&_Functor)
			{
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nAllocations, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nFree, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nResize, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nRealloc, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nGetSize, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nProtect, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nCommit, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_nDecommit, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesAlloc, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesFree, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesResize, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesRealloc, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesProtect, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesCommit, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesDecommit, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesOverhead, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesPotentialWaste, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_BytesMaxAlloc, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_AtBytesMaxAlloc.m_BytesOverhead, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_AtBytesMaxAlloc.m_BytesPotentialWaste, _Functor);
				fg_OperateOnStatUnary<tf_Flags>(_Stats.m_AtBytesMaxAlloc.m_nAllocations, _Functor);
			}
			template <CStatAffectFlagUnderlaying tf_LeftFlags, CStatAffectFlagUnderlaying tf_RightFlags, typename tf_CFunctor>
			static void fg_OperateOnStatBinary(CTestStats &_StatLeft, CTestStats &_StatRight, tf_CFunctor &&_Functor)
			{
				if (tf_LeftFlags == tf_RightFlags)
				{
					if (tf_LeftFlags & EStatAffectFlag_Average)
						_Functor(_StatLeft.m_Average, _StatRight.m_Average);
					if (tf_LeftFlags & EStatAffectFlag_Min)
						_Functor(_StatLeft.m_Min, _StatRight.m_Min);
					if (tf_LeftFlags & EStatAffectFlag_Max)
						_Functor(_StatLeft.m_Max, _StatRight.m_Max);
					if (tf_LeftFlags & EStatAffectFlag_StdDev)
						_Functor(_StatLeft.m_StdDev, _StatRight.m_StdDev);
					if (tf_LeftFlags & EStatAffectFlag_Median)
						_Functor(_StatLeft.m_Median, _StatRight.m_Median);
				}
				else
				{
					fp64 *pLeft = nullptr;
					fp64 *pRight = nullptr;
					if (tf_LeftFlags == EStatAffectFlag_Average)
						pLeft = &_StatLeft.m_Average;
					if (tf_LeftFlags == EStatAffectFlag_Min)
						pLeft = &_StatLeft.m_Min;
					if (tf_LeftFlags == EStatAffectFlag_Max)
						pLeft = &_StatLeft.m_Max;
					if (tf_LeftFlags == EStatAffectFlag_StdDev)
						pLeft = &_StatLeft.m_StdDev;
					if (tf_LeftFlags == EStatAffectFlag_Median)
						pLeft = &_StatLeft.m_Median;
					if (tf_RightFlags == EStatAffectFlag_Average)
						pRight = &_StatRight.m_Average;
					if (tf_RightFlags == EStatAffectFlag_Min)
						pRight = &_StatRight.m_Min;
					if (tf_RightFlags == EStatAffectFlag_Max)
						pRight = &_StatRight.m_Max;
					if (tf_RightFlags == EStatAffectFlag_StdDev)
						pRight = &_StatRight.m_StdDev;
					if (tf_RightFlags == EStatAffectFlag_Median)
						pRight = &_StatRight.m_Median;
					_Functor(*pLeft, *pRight);
				}
			}

			template <CStatAffectFlagUnderlaying tf_Flags, typename tf_CFunctor>
			void fg_OperateOnStatsBinary(CTestMemoryStats &_StatsLeft, CTestMemoryStats &_StatsRight, tf_CFunctor &&_Functor)
			{
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nAllocations, _StatsRight.m_nAllocations, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nFree, _StatsRight.m_nFree, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nResize, _StatsRight.m_nResize, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nRealloc, _StatsRight.m_nRealloc, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nGetSize, _StatsRight.m_nGetSize, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nProtect, _StatsRight.m_nProtect, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nCommit, _StatsRight.m_nCommit, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_nDecommit, _StatsRight.m_nDecommit, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesAlloc, _StatsRight.m_BytesAlloc, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesFree, _StatsRight.m_BytesFree, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesResize, _StatsRight.m_BytesResize, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesRealloc, _StatsRight.m_BytesRealloc, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesProtect, _StatsRight.m_BytesProtect, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesCommit, _StatsRight.m_BytesCommit, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesDecommit, _StatsRight.m_BytesDecommit, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesOverhead, _StatsRight.m_BytesOverhead, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesPotentialWaste, _StatsRight.m_BytesPotentialWaste, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_BytesMaxAlloc, _StatsRight.m_BytesMaxAlloc, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_AtBytesMaxAlloc.m_BytesOverhead, _StatsRight.m_AtBytesMaxAlloc.m_BytesOverhead, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_AtBytesMaxAlloc.m_BytesPotentialWaste, _StatsRight.m_AtBytesMaxAlloc.m_BytesPotentialWaste, _Functor);
				fg_OperateOnStatBinary<tf_Flags, tf_Flags>(_StatsLeft.m_AtBytesMaxAlloc.m_nAllocations, _StatsRight.m_AtBytesMaxAlloc.m_nAllocations, _Functor);
			}

			template <CStatAffectFlagUnderlaying tf_LeftFlags, CStatAffectFlagUnderlaying tf_RightFlags, typename tf_CFunctor>
			void fg_OperateOnStatsBinary(CTestMemoryStats &_StatsLeft, CTestMemoryStats &_StatsRight, tf_CFunctor &&_Functor)
			{
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nAllocations, _StatsRight.m_nAllocations, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nFree, _StatsRight.m_nFree, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nResize, _StatsRight.m_nResize, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nRealloc, _StatsRight.m_nRealloc, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nGetSize, _StatsRight.m_nGetSize, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nProtect, _StatsRight.m_nProtect, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nCommit, _StatsRight.m_nCommit, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_nDecommit, _StatsRight.m_nDecommit, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesAlloc, _StatsRight.m_BytesAlloc, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesFree, _StatsRight.m_BytesFree, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesResize, _StatsRight.m_BytesResize, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesRealloc, _StatsRight.m_BytesRealloc, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesProtect, _StatsRight.m_BytesProtect, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesCommit, _StatsRight.m_BytesCommit, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesDecommit, _StatsRight.m_BytesDecommit, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesOverhead, _StatsRight.m_BytesOverhead, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesPotentialWaste, _StatsRight.m_BytesPotentialWaste, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_BytesMaxAlloc, _StatsRight.m_BytesMaxAlloc, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_AtBytesMaxAlloc.m_BytesOverhead, _StatsRight.m_AtBytesMaxAlloc.m_BytesOverhead, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_AtBytesMaxAlloc.m_BytesPotentialWaste, _StatsRight.m_AtBytesMaxAlloc.m_BytesPotentialWaste, _Functor);
				fg_OperateOnStatBinary<tf_LeftFlags, tf_RightFlags>(_StatsLeft.m_AtBytesMaxAlloc.m_nAllocations, _StatsRight.m_AtBytesMaxAlloc.m_nAllocations, _Functor);
			}
		}


		/***************************************************************************************************\
		|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
		| Measure																							|
		|___________________________________________________________________________________________________|
		\***************************************************************************************************/


		CTestMemoryMeasure::CTestMemoryMeasure(NStr::CStr const &_Name)
			: mp_nIterationsSum(0)
			, mp_nRepetitions(0)
			, mp_Name(_Name)
			, mp_pOldReporter(nullptr)
		{
		}

		inline_never void CTestMemoryMeasure::f_Start()
		{
#if DMibConfig_Memory_Shims_Enable
			mp_pOldReporter = NMem::fg_ReportMemoryTo(this);
#endif
		}

		inline_never void CTestMemoryMeasure::f_Stop(uint32 _nIterations)
		{
#if DMibConfig_Memory_Shims_Enable
			NMem::fg_ReportMemoryTo(mp_pOldReporter);
#endif
			DMibRequire(_nIterations != 0);
			mp_nIterationsSum += _nIterations;
			++mp_nRepetitions;

			mp_AllocatorHistory["All"].f_Insert(mp_AllAllocations);

			for (auto &Stats : mp_Allocators)
				mp_AllocatorHistory[mp_Allocators.fs_GetKey(Stats)].f_Insert(Stats);

			mp_AllAllocations.f_Clear();
			mp_Allocators.f_Clear();
			mp_Allocations.f_Clear();
			mp_ActiveAllocators.f_Clear();
		}

		uint32 CTestMemoryMeasure::f_Repetitions()
		{
			return mp_nRepetitions;
		}

		uint32 CTestMemoryMeasure::f_Iterations()
		{
			return mp_nIterationsSum / mp_nRepetitions;
		}

		NStr::CStr const &CTestMemoryMeasure::f_Name()
		{
			return mp_Name;
		}

		CTestMemoryMeasure::CAllocatorStats::CAllocatorStats()
		{
			f_Clear();
		}

		void CTestMemoryMeasure::CAllocatorStats::f_Clear()
		{
			m_nAllocations = 0;
			m_nFree = 0;
			m_nResize = 0;
			m_nRealloc = 0;
			m_nGetSize = 0;
			m_nProtect = 0;
			m_nCommit = 0;
			m_nDecommit = 0;
			m_BytesAlloc = 0;
			m_BytesFree = 0;
			m_BytesResize = 0;
			m_BytesRealloc = 0;
			m_BytesProtect = 0;
			m_BytesCommit = 0;
			m_BytesDecommit = 0;
			m_BytesOverhead = 0;
			m_BytesPotentialWaste = 0;
			m_CurrentBytesAlloc = 0;
			m_CurrentBytesOverhead = 0;
			m_CurrentBytesPotentialWaste = 0;
			m_nCurrentAllocations = 0;
			m_BytesMaxAlloc = 0;
			m_BytesOverheadAtMaxAlloc = 0;
			m_BytesPotentialWasteAtMaxAlloc = 0;
			m_nAllocationsAtMaxAlloc = 0;
		}


		void CTestMemoryMeasure::CAllocatorStats::fp_UpdateAtMaxAlloc()
		{
			if (m_CurrentBytesAlloc > m_BytesMaxAlloc)
			{
				m_BytesMaxAlloc = m_CurrentBytesAlloc;
				m_BytesOverheadAtMaxAlloc = m_CurrentBytesOverhead;
				m_BytesPotentialWasteAtMaxAlloc = m_CurrentBytesPotentialWaste;
				m_nAllocationsAtMaxAlloc = m_nCurrentAllocations;
			}
		}

		void CTestMemoryMeasure::CAllocatorStats::f_Alloc(uint64 _Size, uint64 _Overhead, uint64 _Waste)
		{
			++m_nAllocations;
			m_BytesAlloc += _Size;
			m_BytesOverhead += _Overhead;
			m_BytesPotentialWaste += _Waste;

			++m_nCurrentAllocations;
			m_CurrentBytesAlloc += _Size;
			m_CurrentBytesOverhead += _Overhead;
			m_CurrentBytesPotentialWaste += _Waste;
			fp_UpdateAtMaxAlloc();
		}
		void CTestMemoryMeasure::CAllocatorStats::f_ResizeReallocFree(uint64 _Size, uint64 _Overhead, uint64 _Waste)
		{
			m_BytesFree += _Size;
			--m_nCurrentAllocations;
			m_CurrentBytesAlloc -= _Size;
			m_CurrentBytesOverhead -= _Overhead;
			m_CurrentBytesPotentialWaste -= _Waste;
		}
		void CTestMemoryMeasure::CAllocatorStats::f_Resize(uint64 _Size, uint64 _Overhead, uint64 _Waste)
		{
			++m_nResize;
			m_BytesResize += _Size;
			m_BytesAlloc += _Size;
			m_BytesOverhead += _Overhead;
			m_BytesPotentialWaste += _Waste;

			++m_nCurrentAllocations;
			m_CurrentBytesAlloc += _Size;
			m_CurrentBytesOverhead += _Overhead;
			m_CurrentBytesPotentialWaste += _Waste;
			fp_UpdateAtMaxAlloc();
		}
		void CTestMemoryMeasure::CAllocatorStats::f_Realloc(uint64 _Size, uint64 _Overhead, uint64 _Waste)
		{
			++m_nRealloc;
			m_BytesResize += _Size;
			m_BytesAlloc += _Size;
			m_BytesOverhead += _Overhead;
			m_BytesPotentialWaste += _Waste;

			++m_nCurrentAllocations;
			m_CurrentBytesAlloc += _Size;
			m_CurrentBytesOverhead += _Overhead;
			m_CurrentBytesPotentialWaste += _Waste;
			fp_UpdateAtMaxAlloc();
		}
		void CTestMemoryMeasure::CAllocatorStats::f_Free(uint64 _Size, uint64 _Overhead, uint64 _Waste)
		{
			++m_nFree;
			m_BytesFree += _Size;
			--m_nCurrentAllocations;
			m_CurrentBytesAlloc -= _Size;
			m_CurrentBytesOverhead -= _Overhead;
			m_CurrentBytesPotentialWaste -= _Waste;
		}
		void CTestMemoryMeasure::CAllocatorStats::f_GetSize()
		{
			++m_nGetSize;
		}
		void CTestMemoryMeasure::CAllocatorStats::f_Protect(uint64 _Size)
		{
			++m_nProtect;
			m_BytesProtect += _Size;
		}
		void CTestMemoryMeasure::CAllocatorStats::f_Commit(uint64 _Size)
		{
			++m_nCommit;
			m_BytesCommit += _Size;
		}
		void CTestMemoryMeasure::CAllocatorStats::f_Decommit(uint64 _Size)
		{
			++m_nDecommit;
			m_BytesDecommit += _Size;
		}

		CTestMemoryMeasure::CAllocatorStats *CTestMemoryMeasure::fp_GetActiveStats(mint _Heap)
		{
			CAllocatorStats **pStats = mp_ActiveAllocators.f_FindEqual(_Heap);
			if (pStats)
				return *pStats;
			return nullptr;
		}

		CTestMemoryMeasure::CAllocatorStats &CTestMemoryMeasure::fp_GetActiveStats(mint _Heap, const ch8 *_pHeapName)
		{
			CAllocatorStats **pStats = mp_ActiveAllocators.f_FindEqual(_Heap);
			if (pStats)
				return **pStats;

			CAllocatorStats &Stats = mp_Allocators[_pHeapName];
			mp_ActiveAllocators[_Heap] = &Stats;
			return Stats;
		}

		void CTestMemoryMeasure::f_Alloc
			(
				mint _MemoryAllocator
				, mint _AllocatorDepth
				, ch8 const *_pAllocatorName
				, mint _Address
				, mint _RequestedAlignment
				, mint _RequestedSize
				, mint _ReturnedSize
				, fp32 _nBytesOverhead
				, void *_pAllocationInfo
			)
		{
			if (_AllocatorDepth != 1)
				return;
			DMibLock(mp_Lock);
			bint bCreated = false;
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);

			CAllocationKey Key;
			Key.m_MemoryAllocator = _MemoryAllocator;
			Key.m_Address = _Address;

			bCreated = false;
			CAllocation &Alloc = mp_Allocations.f_Map(Key, bCreated);
			DMibFastCheck(bCreated);
			DMibFastCheck(_ReturnedSize >= _RequestedSize);

			Alloc.m_Size = _RequestedSize;
			Alloc.m_Overhead = (_nBytesOverhead * 1000.0f).f_ToIntRound();
			Alloc.m_PotentialWaste = _ReturnedSize - _RequestedSize;

			AllocatorStats.f_Alloc(_RequestedSize, Alloc.m_Overhead, Alloc.m_PotentialWaste);
			mp_AllAllocations.f_Alloc(_RequestedSize, Alloc.m_Overhead, Alloc.m_PotentialWaste);
		}

		void CTestMemoryMeasure::f_Resize
			(
				mint _MemoryAllocator
				, mint _AllocatorDepth
				, ch8 const *_pAllocatorName
				, mint _OldAddress
				, mint _OldSize
				, void const *_pOldAllocationInfo
				, mint _Address
				, mint _RequestedAlignment
				, mint _RequestedSize
				, mint _ReturnedSize
				, fp32 _nBytesOverhead
				, void *_pAllocationInfo
			)
		{
			DMibLock(mp_Lock);
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);

			CAllocationKey OldKey;
			OldKey.m_MemoryAllocator = _MemoryAllocator;
			OldKey.m_Address = _OldAddress;

			CAllocation *pOldAlloc = mp_Allocations.f_FindEqual(OldKey);

			if (pOldAlloc)
			{
				AllocatorStats.f_ResizeReallocFree(pOldAlloc->m_Size, pOldAlloc->m_Overhead, pOldAlloc->m_PotentialWaste);
				mp_AllAllocations.f_ResizeReallocFree(pOldAlloc->m_Size, pOldAlloc->m_Overhead, pOldAlloc->m_PotentialWaste);
				mp_Allocations.f_Remove(pOldAlloc);
			}
			else if (_AllocatorDepth != 1)
				return;

			CAllocationKey Key;
			Key.m_MemoryAllocator = _MemoryAllocator;
			Key.m_Address = _Address;
			bint bCreated = false;
			CAllocation &Alloc = mp_Allocations.f_Map(Key, bCreated);
			DMibFastCheck(bCreated);
			DMibFastCheck(_ReturnedSize >= _RequestedSize);

			Alloc.m_Size = _RequestedSize;
			Alloc.m_Overhead = (_nBytesOverhead * 1000.0f).f_ToIntRound();
			Alloc.m_PotentialWaste = _ReturnedSize - _RequestedSize;

			AllocatorStats.f_Resize(_RequestedSize, Alloc.m_Overhead, Alloc.m_PotentialWaste);
			mp_AllAllocations.f_Resize(_RequestedSize, Alloc.m_Overhead, Alloc.m_PotentialWaste);
		}

		void CTestMemoryMeasure::f_Realloc
			(
				mint _MemoryAllocator
				, mint _AllocatorDepth
				, ch8 const *_pAllocatorName
				, mint _OldAddress
				, mint _OldSize
				, void const *_pOldAllocationInfo
				, mint _Address
				, mint _RequestedAlignment
				, mint _RequestedSize
				, mint _ReturnedSize
				, fp32 _nBytesOverhead
				, void *_pAllocationInfo
			)
		{
			DMibLock(mp_Lock);
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);

			CAllocationKey OldKey;
			OldKey.m_MemoryAllocator = _MemoryAllocator;
			OldKey.m_Address = _OldAddress;

			CAllocation *pOldAlloc = mp_Allocations.f_FindEqual(OldKey);

			if (pOldAlloc)
			{
				AllocatorStats.f_ResizeReallocFree(pOldAlloc->m_Size, pOldAlloc->m_Overhead, pOldAlloc->m_PotentialWaste);
				mp_AllAllocations.f_ResizeReallocFree(pOldAlloc->m_Size, pOldAlloc->m_Overhead, pOldAlloc->m_PotentialWaste);
				mp_Allocations.f_Remove(pOldAlloc);
			}
			else if (_AllocatorDepth != 1)
				return;

			CAllocationKey Key;
			Key.m_MemoryAllocator = _MemoryAllocator;
			Key.m_Address = _Address;
			bint bCreated = false;
			CAllocation &Alloc = mp_Allocations.f_Map(Key, bCreated);
			DMibFastCheck(bCreated);
			DMibFastCheck(_ReturnedSize >= _RequestedSize);

			Alloc.m_Size = _RequestedSize;
			Alloc.m_Overhead = (_nBytesOverhead * 1000.0f).f_ToIntRound();
			Alloc.m_PotentialWaste = _ReturnedSize - _RequestedSize;

			AllocatorStats.f_Realloc(_RequestedSize, Alloc.m_Overhead, Alloc.m_PotentialWaste);
			mp_AllAllocations.f_Realloc(_RequestedSize, Alloc.m_Overhead, Alloc.m_PotentialWaste);
		}

		void CTestMemoryMeasure::f_Free(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size, void const *_pAllocationInfo)
		{
			DMibLock(mp_Lock);
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);
			CAllocationKey OldKey;
			OldKey.m_MemoryAllocator = _MemoryAllocator;
			OldKey.m_Address = _Address;
			CAllocation *pOldAlloc = mp_Allocations.f_FindEqual(OldKey);
			if (pOldAlloc)
			{
				AllocatorStats.f_Free(pOldAlloc->m_Size, pOldAlloc->m_Overhead, pOldAlloc->m_PotentialWaste);
				mp_AllAllocations.f_Free(pOldAlloc->m_Size, pOldAlloc->m_Overhead, pOldAlloc->m_PotentialWaste);
				mp_Allocations.f_Remove(pOldAlloc);
			}
			
		}

		void CTestMemoryMeasure::f_GetSize(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size, void const *_pAllocationInfo)
		{
			if (_AllocatorDepth != 1)
				return;
			DMibLock(mp_Lock);
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);
			AllocatorStats.f_GetSize();
			mp_AllAllocations.f_GetSize();
		}

		void CTestMemoryMeasure::f_Protect(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size, uaint _Protect)
		{
			if (_AllocatorDepth != 1)
				return;
			DMibLock(mp_Lock);
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);
			AllocatorStats.f_Protect(_Size);
			mp_AllAllocations.f_Protect(_Size);
		}

		void CTestMemoryMeasure::f_Commit(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size)
		{
			if (_AllocatorDepth != 1)
				return;
			DMibLock(mp_Lock);
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);
			AllocatorStats.f_Commit(_Size);
			mp_AllAllocations.f_Commit(_Size);
		}

		void CTestMemoryMeasure::f_Decommit(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size)
		{
			if (_AllocatorDepth != 1)
				return;
			DMibLock(mp_Lock);
			CAllocatorStats &AllocatorStats = fp_GetActiveStats(_MemoryAllocator, _pAllocatorName);
			AllocatorStats.f_Decommit(_Size);
			mp_AllAllocations.f_Decommit(_Size);
		}

		void CTestMemoryMeasure::f_AllocatorDelete(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth)
		{
			DMibLock(mp_Lock);
			mp_ActiveAllocators.f_Remove(_MemoryAllocator);
		}

		void CTestMemoryMeasure::f_GetResults(CTestMemoryResult &_Result)
		{
			_Result.m_Name = mp_Name;
			_Result.m_MeasureType = ETestMeasureType_Normal;
			_Result.m_nIterations = f_Iterations();
			_Result.m_nRepetitions = mp_nRepetitions;

			for (auto &History : mp_AllocatorHistory)
			{
				NStr::CStrNonTracked const &Name = mp_AllocatorHistory.fs_GetKey(History);

				CTestMemoryStats *pStats;
				if (Name == "All")
					pStats = &_Result.m_AllAllocations;
				else
					pStats = &_Result.m_PerAllocationType[Name];

				for (auto &SourceStats : History)
				{
					CTestMemoryStats Stats;
					SourceStats.f_TransferToStats<EStatAffectFlag(int(EStatAffectFlag_Min) | int(EStatAffectFlag_Max) | int(EStatAffectFlag_Average))>(Stats);

					fg_OperateOnStatsBinary<EStatAffectFlag_Min>
						(
							*pStats
							, Stats
							, [] (fp64 &_Left, fp64 &_Right)
							{
								_Left = fg_Min(_Left, _Right);
							}
						)
					;
					fg_OperateOnStatsBinary<EStatAffectFlag_Max>
						(
							*pStats
							, Stats
							, [] (fp64 &_Left, fp64 &_Right)
							{
								_Left = fg_Max(_Left, _Right);
							}
						)
					;
					fg_OperateOnStatsBinary<EStatAffectFlag_Average>
						(
							*pStats
							, Stats
							, [] (fp64 &_Left, fp64 &_Right)
							{
								_Left += _Right;
							}
						)
					;
				}

				fp64 Repetitions = mp_nRepetitions;
				fg_OperateOnStatsUnary<EStatAffectFlag_Average>
					(
						*pStats
						, [&] (fp64 &_Left)
						{
							_Left /= Repetitions;
						}
					)
				;

				if (mp_nRepetitions >= 2)
				{
					// Calculate StdDev
					CTestMemoryStats SqrSum;

					for (auto & SourceStats : History)
					{
						CTestMemoryStats Stats;
						SourceStats.f_TransferToStats<EStatAffectFlag(EStatAffectFlag_StdDev)>(Stats);

						fg_OperateOnStatsBinary<EStatAffectFlag_StdDev, EStatAffectFlag_Average>
							(
								Stats
								, *pStats
								, [] (fp64 &_Left, fp64 &_Right)
								{
									fp64 Delta = (_Left - _Right);
									_Left = Delta * Delta;
								}
							)
						;
						fg_OperateOnStatsBinary<EStatAffectFlag_StdDev>
							(
								SqrSum
								, Stats
								, [] (fp64 &_Left, fp64 &_Right)
								{
									_Left += _Right;
								}
							)
						;
					}

					fg_OperateOnStatsBinary<EStatAffectFlag_StdDev>
						(
							*pStats
							, SqrSum
							, [&] (fp64 &_Left, fp64 &_Right)
							{
								_Left = (_Right / fp64(mp_nRepetitions - 1)).f_Sqrt();
							}
						)
					;
				}

				if (History.f_GetLen() >= 1)
				{
					
					NContainer::TCVector<fp64> nAllocations;
					NContainer::TCVector<fp64> nFree;
					NContainer::TCVector<fp64> nResize;
					NContainer::TCVector<fp64> nRealloc;
					NContainer::TCVector<fp64> nGetSize;
					NContainer::TCVector<fp64> nProtect;
					NContainer::TCVector<fp64> nCommit;
					NContainer::TCVector<fp64> nDecommit;
					NContainer::TCVector<fp64> BytesAlloc;
					NContainer::TCVector<fp64> BytesFree;
					NContainer::TCVector<fp64> BytesResize; 
					NContainer::TCVector<fp64> BytesRealloc;
					NContainer::TCVector<fp64> BytesProtect;
					NContainer::TCVector<fp64> BytesCommit;
					NContainer::TCVector<fp64> BytesDecommit;
					NContainer::TCVector<fp64> BytesOverhead;
					NContainer::TCVector<fp64> BytesPotentialWaste;
					NContainer::TCVector<fp64> BytesMaxAlloc;
					NContainer::TCVector<fp64> AtBytesMaxAllocBytesOverhead;
					NContainer::TCVector<fp64> AtBytesMaxAllocBytesPotentialWaste;
					NContainer::TCVector<fp64> AtBytesMaxAllocnAllocations;

					for (auto & SourceStats : History)
					{
						CTestMemoryStats Stats;
						SourceStats.f_TransferToStats<EStatAffectFlag(EStatAffectFlag_Median)>(Stats);

						nAllocations.f_Insert(Stats.m_nAllocations.m_Median);
						nFree.f_Insert(Stats.m_nFree.m_Median);
						nResize.f_Insert(Stats.m_nResize.m_Median);
						nRealloc.f_Insert(Stats.m_nRealloc.m_Median);
						nGetSize.f_Insert(Stats.m_nGetSize.m_Median);
						nProtect.f_Insert(Stats.m_nProtect.m_Median);
						nCommit.f_Insert(Stats.m_nCommit.m_Median);
						nDecommit.f_Insert(Stats.m_nDecommit.m_Median);
						BytesAlloc.f_Insert(Stats.m_BytesAlloc.m_Median);
						BytesFree.f_Insert(Stats.m_BytesFree.m_Median);
						BytesResize.f_Insert(Stats.m_BytesResize.m_Median);
						BytesRealloc.f_Insert(Stats.m_BytesRealloc.m_Median);
						BytesProtect.f_Insert(Stats.m_BytesProtect.m_Median);
						BytesCommit.f_Insert(Stats.m_BytesCommit.m_Median);
						BytesDecommit.f_Insert(Stats.m_BytesDecommit.m_Median);
						BytesOverhead.f_Insert(Stats.m_BytesOverhead.m_Median);
						BytesPotentialWaste.f_Insert(Stats.m_BytesPotentialWaste.m_Median);
						BytesMaxAlloc.f_Insert(Stats.m_BytesMaxAlloc.m_Median);
						AtBytesMaxAllocBytesOverhead.f_Insert(Stats.m_AtBytesMaxAlloc.m_BytesOverhead.m_Median);
						AtBytesMaxAllocBytesPotentialWaste.f_Insert(Stats.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Median);
						AtBytesMaxAllocnAllocations.f_Insert(Stats.m_AtBytesMaxAlloc.m_nAllocations.m_Median);
					}

					nAllocations.f_Sort();
					nFree.f_Sort();
					nResize.f_Sort();
					nRealloc.f_Sort();
					nGetSize.f_Sort();
					nProtect.f_Sort();
					nCommit.f_Sort();
					nDecommit.f_Sort();
					BytesAlloc.f_Sort();
					BytesFree.f_Sort();
					BytesResize.f_Sort();
					BytesRealloc.f_Sort();
					BytesProtect.f_Sort();
					BytesCommit.f_Sort();
					BytesDecommit.f_Sort();
					BytesOverhead.f_Sort();
					BytesPotentialWaste.f_Sort();
					BytesMaxAlloc.f_Sort();
					AtBytesMaxAllocBytesOverhead.f_Sort();
					AtBytesMaxAllocBytesPotentialWaste.f_Sort();
					AtBytesMaxAllocnAllocations.f_Sort();
					mint iIndex = History.f_GetLen() / 2;

					pStats->m_nAllocations.m_Median = nAllocations[iIndex];
					pStats->m_nFree.m_Median = nFree[iIndex];
					pStats->m_nResize.m_Median = nResize[iIndex];
					pStats->m_nRealloc.m_Median = nRealloc[iIndex];
					pStats->m_nGetSize.m_Median = nGetSize[iIndex];
					pStats->m_nProtect.m_Median = nProtect[iIndex];
					pStats->m_nCommit.m_Median = nCommit[iIndex];
					pStats->m_nDecommit.m_Median = nDecommit[iIndex];
					pStats->m_BytesAlloc.m_Median = BytesAlloc[iIndex];
					pStats->m_BytesFree.m_Median = BytesFree[iIndex];
					pStats->m_BytesResize.m_Median = BytesResize[iIndex];
					pStats->m_BytesRealloc.m_Median = BytesRealloc[iIndex];
					pStats->m_BytesProtect.m_Median = BytesProtect[iIndex];
					pStats->m_BytesCommit.m_Median = BytesCommit[iIndex];
					pStats->m_BytesDecommit.m_Median = BytesDecommit[iIndex];
					pStats->m_BytesOverhead.m_Median = BytesOverhead[iIndex];
					pStats->m_BytesPotentialWaste.m_Median = BytesPotentialWaste[iIndex];
					pStats->m_BytesMaxAlloc.m_Median = BytesMaxAlloc[iIndex];
					pStats->m_AtBytesMaxAlloc.m_BytesOverhead.m_Median = AtBytesMaxAllocBytesOverhead[iIndex];
					pStats->m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Median = AtBytesMaxAllocBytesPotentialWaste[iIndex];
					pStats->m_AtBytesMaxAlloc.m_nAllocations.m_Median = AtBytesMaxAllocnAllocations[iIndex];

				}

			}
		}


		/***************************************************************************************************\
		|¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯|
		| Results																							|
		|___________________________________________________________________________________________________|
		\***************************************************************************************************/

		CTestMemoryResult &CTestMemory::fp_Add(CTestMemoryMeasure &_Measure)
		{
			DMibRequire(_Measure.f_Repetitions() > 0);
			CTestMemoryResult &Result = mp_Results.m_Results.f_Insert();
			_Measure.f_GetResults(Result);
			return Result;
		}

#if !defined(DConfig_Optimized) && !defined(DConfig_Release) && !defined(DConfig_Profile)
		bint CTestMemory::f_IsIgnored() const
		{
			return true;
		}
#endif
		CTestMemory::CTestMemory(fp64 const &_Tolerance, bint _bCompareNumAllocations)
		{
			mp_Results.m_Tolerance = _Tolerance;
			mp_bCompareNumAllocations = _bCompareNumAllocations;
		}
		void CTestMemory::f_AddReference(CTestMemoryMeasure &_Measure)
		{
			CTestMemoryResult &Result = fp_Add(_Measure);
			Result.m_MeasureType = ETestMeasureType_Reference;
		}
		void CTestMemory::f_AddBaseline(CTestMemoryMeasure &_Measure)
		{
			CTestMemoryResult &Result = fp_Add(_Measure);
			Result.m_MeasureType = ETestMeasureType_Baseline;
		}
		void CTestMemory::f_AddDebug(CTestMemoryMeasure &_Measure)
		{
			CTestMemoryResult &Result = fp_Add(_Measure);
			Result.m_MeasureType = ETestMeasureType_Debug;
		}
		void CTestMemory::f_Add(CTestMemoryMeasure &_Measure)
		{
			fp_Add(_Measure);
		}

		void CTestMemory::f_Add(CTestMemoryResult const &_Measure)
		{
			/*CTestMemoryResult &Result =*/ mp_Results.m_Results.f_Insert(_Measure);
		}

		bint CTestMemory::f_IsEmpty()
		{
			return mp_Results.m_Results.f_IsEmpty();
		}

		void CTestMemory::f_TestReport(CTestResults &_Results) const
		{
			_Results.f_MemoryResults(mp_Results);
		}
		CTestMemory::operator bool () const
		{
#if defined(DConfig_Optimized) || defined(DConfig_Release) || defined(DConfig_Profile)
			CTestMemoryResult const *pBestRef = nullptr;
			CTestMemoryResult const *pBest = nullptr;
			fp_GetBest(pBestRef, pBest);
			if (pBestRef && pBest)
			{
				if (mp_bCompareNumAllocations)
				{
					fp64 RefValue = pBestRef->m_AllAllocations.m_nAllocations.m_Average;
					fp64 Value = pBest->m_AllAllocations.m_nAllocations.m_Average; 
					return (Value * mp_Results.m_Tolerance) <= RefValue;
				}
				else
				{
					fp64 RefValue
						= pBestRef->m_AllAllocations.m_BytesMaxAlloc.m_Average 
						+ pBestRef->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesOverhead.m_Average 
						+ pBestRef->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
					;
					fp64 Value 
						= pBest->m_AllAllocations.m_BytesMaxAlloc.m_Average 
						+ pBest->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesOverhead.m_Average 
						+ pBest->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
					;
					return (Value * mp_Results.m_Tolerance) <= RefValue;
				}
			}
#endif
			return true;
		}

		NStr::CStr CTestMemory::f_ModifyDescription(NStr::CStr const &_Description)
		{
			mint nResults = mp_Results.m_Results.f_GetLen();
			if (nResults > 0)
			{
				return NStr::CStr::CFormat("{}({})") << _Description << mp_Results.m_Results[0].m_nIterations;
			}
			return _Description;
		}

		void CTestMemory::fp_GetBest(CTestMemoryResult const *&_pBestRef, CTestMemoryResult const *&_pBest) const
		{
			CTestMemoryResult const *pBestRef = nullptr;
			CTestMemoryResult const *pBest = nullptr;
			fp64 BestRef = fp64::fs_LimitMax();
			fp64 Best = fp64::fs_LimitMax();
			mint nResults = mp_Results.m_Results.f_GetLen();
			for (mint i = 0; i < nResults; ++i)
			{
				CTestMemoryResult const &Result = mp_Results.m_Results[i];
				fp64 ThisValue;
				if (mp_bCompareNumAllocations)
				{
					ThisValue
						= Result.m_AllAllocations.m_nAllocations.m_Average
						+ Result.m_AllAllocations.m_nResize.m_Average
						+ Result.m_AllAllocations.m_nRealloc.m_Average
					;
				}
				else
				{
					ThisValue
						= Result.m_AllAllocations.m_BytesMaxAlloc.m_Average
						+ Result.m_AllAllocations.m_AtBytesMaxAlloc.m_BytesOverhead.m_Average
						+ Result.m_AllAllocations.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
					;
				}
				
				if (Result.m_MeasureType == ETestMeasureType_Normal)
				{
					if (ThisValue < Best)
					{
						Best = ThisValue;
						pBest = &Result;
					}
				}
				else if (Result.m_MeasureType == ETestMeasureType_Reference)
				{
					if (ThisValue < BestRef)
					{
						BestRef = ThisValue;
						pBestRef = &Result;
					}
				}
			}
			_pBestRef = pBestRef;
			_pBest = pBest;
		}

		NStr::CStr CTestMemory::fp_GetOutput() const
		{
			NStr::CStr Output;
			CTestMemoryResult const *pBestRef = nullptr;
			CTestMemoryResult const *pBest = nullptr;
			fp_GetBest(pBestRef, pBest);
			if (mp_bCompareNumAllocations)
			{
				if (pBestRef && pBest)
				{
					fp64 RefValue
						= pBestRef->m_AllAllocations.m_nAllocations.m_Average
						+ pBestRef->m_AllAllocations.m_nResize.m_Average
						+ pBestRef->m_AllAllocations.m_nRealloc.m_Average
					;
					fp64 Value 
						= pBest->m_AllAllocations.m_nAllocations.m_Average
						+ pBest->m_AllAllocations.m_nResize.m_Average
						+ pBest->m_AllAllocations.m_nRealloc.m_Average
					;
					fp64 Diff = 1.0;
					if (Value != 0.0)
						Diff = RefValue / Value;
					Output = NStr::CStr::CFormat("{fn3,fe3}   {} {sz9,fn1,fe1}   {} {sz9,fn1,fe1}") << Diff << pBestRef->m_Name << RefValue << pBest->m_Name << Value;
				}
				else if (pBest)
				{
					fp64 Value 
						= pBest->m_AllAllocations.m_nAllocations.m_Average
						+ pBest->m_AllAllocations.m_nResize.m_Average
						+ pBest->m_AllAllocations.m_nRealloc.m_Average
					;
					Output = NStr::CStr::CFormat("{} {sz9,fn1,fe1}") << pBest->m_Name << Value;
				}
				else if (pBestRef)
				{
					fp64 RefValue
						= pBestRef->m_AllAllocations.m_nAllocations.m_Average
						+ pBestRef->m_AllAllocations.m_nResize.m_Average
						+ pBestRef->m_AllAllocations.m_nRealloc.m_Average
					;
					Output = NStr::CStr::CFormat("{} {sz9,fn1,fe1}") << pBestRef->m_Name << RefValue;
				}
				else
					Output = "No measures added";
			}
			else
			{
				if (pBestRef && pBest)
				{
				
					fp64 RefValue
						= pBestRef->m_AllAllocations.m_BytesMaxAlloc.m_Average 
						+ pBestRef->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesOverhead.m_Average 
						+ pBestRef->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
					;
					fp64 Value 
						= pBest->m_AllAllocations.m_BytesMaxAlloc.m_Average 
						+ pBest->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesOverhead.m_Average 
						+ pBest->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
					;
					fp64 Diff = 1.0;
					if (Value != 0.0)
						Diff = RefValue / Value;
					else
						Diff = 1.0;
					Output = NStr::CStr::CFormat("{fn3,fe3}   {} {sz9,fn1,fe1}   {} {sz9,fn1,fe1}") << Diff << pBestRef->m_Name << RefValue << pBest->m_Name << Value;
				}
				else if (pBest)
				{
					fp64 Value 
						= pBest->m_AllAllocations.m_BytesMaxAlloc.m_Average 
						+ pBest->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesOverhead.m_Average 
						+ pBest->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
					;
					Output = NStr::CStr::CFormat("{} {sz9,fn1,fe1}") << pBest->m_Name << Value;
				}
				else if (pBestRef)
				{
					fp64 RefValue
						= pBestRef->m_AllAllocations.m_BytesMaxAlloc.m_Average 
						+ pBestRef->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesOverhead.m_Average 
						+ pBestRef->m_AllAllocations.m_AtBytesMaxAlloc.m_BytesPotentialWaste.m_Average
					;
					Output = NStr::CStr::CFormat("{} {sz9,fn1,fe1}") << pBestRef->m_Name << RefValue;
				}
				else
					Output = "No measures added";
			}

			return Output;
		}
	}
}

