// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Test/Test>

namespace NMib::NTest
{
	using CStatAffectFlagUnderlaying = int32;
	enum EStatAffectFlag : int32
	{
		EStatAffectFlag_Average = DMibBit(0)
		, EStatAffectFlag_Min = DMibBit(1)
		, EStatAffectFlag_Max = DMibBit(2)
		, EStatAffectFlag_StdDev = DMibBit(3)
		, EStatAffectFlag_Median = DMibBit(4)
	};

	class CTestMemoryMeasure : NMemory::CReportMemory
	{
		uint64 mp_nIterationsSum;
		uint32 mp_nRepetitions;
		NStr::CStr mp_Name;
		CReportMemory *mp_pOldReporter;

		NThread::CMutual mp_Lock;

		struct CAllocatorStats
		{
			uint64 m_nAllocations;
			uint64 m_nFree;
			uint64 m_nResize;
			uint64 m_nRealloc;
			uint64 m_nGetSize;
			uint64 m_nProtect;
			uint64 m_nCommit;
			uint64 m_nDecommit;

			uint64 m_BytesAlloc;
			uint64 m_BytesFree;
			uint64 m_BytesResize;
			uint64 m_BytesRealloc;
			uint64 m_BytesProtect;
			uint64 m_BytesCommit;
			uint64 m_BytesDecommit;
			uint64 m_BytesOverhead;
			uint64 m_BytesPotentialWaste;

			uint64 m_CurrentBytesAlloc;
			uint64 m_CurrentBytesOverhead;
			uint64 m_CurrentBytesPotentialWaste;
			uint64 m_nCurrentAllocations;

			uint64 m_BytesMaxAlloc;
			uint64 m_BytesOverheadAtMaxAlloc;
			uint64 m_BytesPotentialWasteAtMaxAlloc;
			uint64 m_nAllocationsAtMaxAlloc;

			CAllocatorStats &operator += (CAllocatorStats const &_Right)
			{
				m_nAllocations += _Right.m_nAllocations;
				m_nFree += _Right.m_nFree;
				m_nResize += _Right.m_nResize;
				m_nRealloc += _Right.m_nRealloc;
				m_nGetSize += _Right.m_nGetSize;
				m_nProtect += _Right.m_nProtect;
				m_nCommit += _Right.m_nCommit;
				m_nDecommit += _Right.m_nDecommit;
				m_BytesAlloc += _Right.m_BytesAlloc;
				m_BytesFree += _Right.m_BytesFree;
				m_BytesResize += _Right.m_BytesResize;
				m_BytesRealloc += _Right.m_BytesRealloc;
				m_BytesProtect += _Right.m_BytesProtect;
				m_BytesCommit += _Right.m_BytesCommit;
				m_BytesDecommit += _Right.m_BytesDecommit;
				m_BytesOverhead += _Right.m_BytesOverhead;
				m_BytesPotentialWaste += _Right.m_BytesPotentialWaste;
				m_CurrentBytesAlloc += _Right.m_CurrentBytesAlloc;
				m_CurrentBytesOverhead += _Right.m_CurrentBytesOverhead;
				m_CurrentBytesPotentialWaste += _Right.m_CurrentBytesPotentialWaste;
				m_nCurrentAllocations += _Right.m_nCurrentAllocations;
				m_BytesMaxAlloc += _Right.m_BytesMaxAlloc;
				m_BytesOverheadAtMaxAlloc += _Right.m_BytesOverheadAtMaxAlloc;
				m_BytesPotentialWasteAtMaxAlloc += _Right.m_BytesPotentialWasteAtMaxAlloc;
				m_nAllocationsAtMaxAlloc += _Right.m_nAllocationsAtMaxAlloc;

				return *this;
			}

			template <CStatAffectFlagUnderlaying tf_Flags>
			void f_TransferToStats(CTestMemoryStats &_Stats) const
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

			void f_Clear();

			void f_Alloc(uint64 _Size, uint64 _Overhead, uint64 _Waste);
			void f_Resize(uint64 _Size, uint64 _Overhead, uint64 _Waste);
			void f_Realloc(uint64 _Size, uint64 _Overhead, uint64 _Waste);
			void f_Free(uint64 _Size, uint64 _Overhead, uint64 _Waste);
			void f_ResizeReallocFree(uint64 _Size, uint64 _Overhead, uint64 _Waste);
			void f_GetSize();
			void f_Protect(uint64 _Size);
			void f_Commit(uint64 _Size);
			void f_Decommit(uint64 _Size);

			CAllocatorStats();

		private:
			template <CStatAffectFlagUnderlaying tf_Flags>
			static void fsp_TransferStat(CTestStats &_Stat, fp64 _Value)
			{
				if (tf_Flags & EStatAffectFlag_Average)
					_Stat.m_Average = _Value;
				if (tf_Flags & EStatAffectFlag_Min)
					_Stat.m_Min = _Value;
				if (tf_Flags & EStatAffectFlag_Max)
					_Stat.m_Max = _Value;
				if (tf_Flags & EStatAffectFlag_StdDev)
					_Stat.m_StdDev = _Value;
				if (tf_Flags & EStatAffectFlag_Median)
					_Stat.m_Median = _Value;
			}

			void fp_UpdateAtMaxAlloc();
		};

		struct CAllocationKey
		{
			mint m_MemoryAllocator;
			mint m_Address;
			bint operator < (CAllocationKey const &_Right) const
			{
				if (m_MemoryAllocator < _Right.m_MemoryAllocator)
					return true;
				else if (m_MemoryAllocator > _Right.m_MemoryAllocator)
					return false;
				return m_Address < _Right.m_Address;
			}
		};
		struct CAllocation
		{
			mint m_Size;
			mint m_Overhead;
			mint m_PotentialWaste;
		};

		NContainer::TCMap<mint, CAllocatorStats *, NMib::CSort_Default, NMemory::CAllocator_NonTrackedHeap> mp_ActiveAllocators;
		NContainer::TCMap<CAllocationKey, CAllocation, CSort_Default, NMemory::CAllocator_NonTrackedHeap> mp_Allocations;
		NContainer::TCMap<NStr::CStrNonTracked, CAllocatorStats, CSort_Default, NMemory::CAllocator_NonTrackedHeap> mp_Allocators;
		CAllocatorStats mp_AllAllocations;

		NContainer::TCMap
			<
				NStr::CStrNonTracked
				, NContainer::TCVector<CAllocatorStats, NMemory::CAllocator_NonTrackedHeap>
				, NMib::CSort_Default
				, NMemory::CAllocator_NonTrackedHeap
			> mp_AllocatorHistory
		;

		CAllocatorStats *fp_GetActiveStats(mint _Heap);
		CAllocatorStats &fp_GetActiveStats(mint _Heap, const ch8 *_pHeapName);

		void f_Alloc
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
			override
		;
		void f_Resize
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
			override
		;
		void f_Realloc
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
			override
		;
		void f_Free(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size, void const *_pAllocationInfo) override;
		void f_GetSize(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size, void const *_pAllocationInfo) override;
		void f_Protect(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size, uaint _Protect) override;
		void f_Commit(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size) override;
		void f_Decommit(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth, mint _Address, mint _Size) override;
		void f_AllocatorDelete(mint _MemoryAllocator, ch8 const *_pAllocatorName, mint _AllocatorDepth) override;

	public:
		CTestMemoryMeasure(NStr::CStr const &_Name);
		inline_never void f_Start();
		inline_never void f_Stop(uint32 _nIterations);
		NStr::CStr const &f_Name();
		uint32 f_Iterations();
		uint32 f_Repetitions();
		void f_GetResults(CTestMemoryResult &_Result);
	};

	// Todo: allow to specify a flag field for what stats should be better in the test
	class CTestMemory
	{
		CTestMemoryResults mp_Results;
		bint mp_bCompareNumAllocations;

		CTestMemoryResult &fp_Add(CTestMemoryMeasure &_Measure);
		NStr::CStr fp_GetOutput() const;
		void fp_GetBest(CTestMemoryResult const *&_pBestRef, CTestMemoryResult const *&_pBest) const;
	public:
#if !defined(DConfig_Optimized) && !defined(DConfig_Release) && !defined(DConfig_Profile)
		bint f_IsIgnored() const;
#endif
		CTestMemory(fp64 const &_Tolerance, bint _bCompareNumAllocations);
		void f_AddBaseline(CTestMemoryMeasure &_Measure);
		void f_AddReference(CTestMemoryMeasure &_Measure);
		void f_AddDebug(CTestMemoryMeasure &_Measure);
		void f_Add(CTestMemoryMeasure &_Measure);
		void f_Add(CTestMemoryResult const &_Measure);

		bint f_IsEmpty();

		void f_TestReport(CTestResults &_Results) const;

		operator bool () const;

		NStr::CStr f_ModifyDescription(NStr::CStr const &_Description);

		template <typename tf_CFormatter>
		int f_GetStringFormatType(tf_CFormatter &_Formatter);

		template <typename tf_CFormatter>
		NStr::CStrFormatTypeClassifier_String f_CreateStringFormatter(tf_CFormatter &_Formatter) const
		{
			return NStr::fg_CreateStringFormatter(_Formatter, fg_ByValue(fp_GetOutput()));
		}
	};

	class CTestMemoryMaxAllocatedBytes : public CTestMemory
	{
	public:
		CTestMemoryMaxAllocatedBytes(fp64 const &_Tolerance)
			: CTestMemory(_Tolerance, false)
		{
		}
	};

	class CTestMemoryNumAllocations : public CTestMemory
	{
	public:
		CTestMemoryNumAllocations(fp64 const &_Tolerance)
			: CTestMemory(_Tolerance, true)
		{
		}
	};
}
