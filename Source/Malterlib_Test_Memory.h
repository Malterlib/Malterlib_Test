// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Test/Test>

namespace NMib::NTest
{
	using CStatAffectFlagUnderlying = int32;
	enum EStatAffectFlag : int32
	{
		EStatAffectFlag_Average  = DMibBit(0)
		, EStatAffectFlag_Min    = DMibBit(1)
		, EStatAffectFlag_Max    = DMibBit(2)
		, EStatAffectFlag_StdDev = DMibBit(3)
		, EStatAffectFlag_Median = DMibBit(4)
	};

	struct CTestMemoryMeasure : private NMemory::CReportMemory
	{
		CTestMemoryMeasure(NStr::CStr const &_Name);
		inline_never void f_Start();
		inline_never void f_Stop(uint32 _nIterations);
		NStr::CStr const &f_Name();
		uint32 f_Iterations();
		uint32 f_Repetitions();
		void f_GetResults(CTestMemoryResult &_Result);

	private:
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

			CAllocatorStats &operator += (CAllocatorStats const &_Right);

			template <CStatAffectFlagUnderlying tf_Flags>
			void f_TransferToStats(CTestMemoryStats &_Stats) const;

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
			template <CStatAffectFlagUnderlying tf_Flags>
			static void fsp_TransferStat(CTestStats &_Stat, fp64 _Value);

			void fp_UpdateAtMaxAlloc();
		};

		struct CAllocationKey
		{
			umint m_MemoryAllocator;
			umint m_Address;
			auto operator <=> (CAllocationKey const &_Right) const noexcept = default;
		};

		struct CAllocation
		{
			umint m_Size;
			umint m_Overhead;
			umint m_PotentialWaste;
		};

		NContainer::TCMap<umint, CAllocatorStats *, NMib::CSort_Default, NMemory::CAllocator_NonTrackedHeap> mp_ActiveAllocators;
		NContainer::TCMap<CAllocationKey, CAllocation, CSort_Default, NMemory::CAllocator_NonTrackedHeap> mp_Allocations;
		NContainer::TCMap<NStr::CStrNonTracked, CAllocatorStats, CSort_Default, NMemory::CAllocator_NonTrackedHeap> mp_Allocators;
		CAllocatorStats mp_AllAllocations;

		NContainer::TCMap<NStr::CStrNonTracked, NContainer::TCVector<CAllocatorStats, NMemory::CAllocator_NonTrackedHeap>, NMib::CSort_Default, NMemory::CAllocator_NonTrackedHeap>
			mp_AllocatorHistory
		;

		CAllocatorStats *fp_GetActiveStats(umint _Heap);
		CAllocatorStats &fp_GetActiveStats(umint _Heap, ch8 const *_pHeapName);

		void f_Alloc
			(
				umint _MemoryAllocator
				, umint _AllocatorDepth
				, ch8 const *_pAllocatorName
				, umint _Address
				, umint _RequestedAlignment
				, umint _RequestedSize
				, umint _ReturnedSize
				, fp32 _nBytesOverhead
				, void *_pAllocationInfo
			)
			override
		;
		void f_Resize
			(
				umint _MemoryAllocator
				, umint _AllocatorDepth
				, ch8 const *_pAllocatorName
				, umint _OldAddress
				, umint _OldSize
				, void const *_pOldAllocationInfo
				, umint _Address
				, umint _RequestedAlignment
				, umint _RequestedSize
				, umint _ReturnedSize
				, fp32 _nBytesOverhead
				, void *_pAllocationInfo
			)
			override
		;
		void f_Realloc
			(
				umint _MemoryAllocator
				, umint _AllocatorDepth
				, ch8 const *_pAllocatorName
				, umint _OldAddress
				, umint _OldSize
				, void const *_pOldAllocationInfo
				, umint _Address
				, umint _RequestedAlignment
				, umint _RequestedSize
				, umint _ReturnedSize
				, fp32 _nBytesOverhead
				, void *_pAllocationInfo
			)
			override
		;
		void f_Free(umint _MemoryAllocator, ch8 const *_pAllocatorName, umint _AllocatorDepth, umint _Address, umint _Size, void const *_pAllocationInfo) override;
		void f_GetSize(umint _MemoryAllocator, ch8 const *_pAllocatorName, umint _AllocatorDepth, umint _Address, umint _Size, void const *_pAllocationInfo) override;
		void f_Protect(umint _MemoryAllocator, ch8 const *_pAllocatorName, umint _AllocatorDepth, umint _Address, umint _Size, uaint _Protect) override;
		void f_Commit(umint _MemoryAllocator, ch8 const *_pAllocatorName, umint _AllocatorDepth, umint _Address, umint _Size) override;
		void f_Decommit(umint _MemoryAllocator, ch8 const *_pAllocatorName, umint _AllocatorDepth, umint _Address, umint _Size) override;
		void f_AllocatorDelete(umint _MemoryAllocator, ch8 const *_pAllocatorName, umint _AllocatorDepth) override;
	};

	// Todo: allow to specify a flag field for what stats should be better in the test
	struct CTestMemory
	{
#if !defined(DConfig_Optimized) && !defined(DConfig_Release) && !defined(DConfig_Profile)
		bool f_IsIgnored() const;
#endif
		CTestMemory(fp64 const &_Tolerance, bool _bCompareNumAllocations);
		void f_AddBaseline(CTestMemoryMeasure &_Measure);
		void f_AddReference(CTestMemoryMeasure &_Measure);
		void f_AddDebug(CTestMemoryMeasure &_Measure);
		void f_Add(CTestMemoryMeasure &_Measure);
		void f_Add(CTestMemoryResult const &_Measure);

		bool f_IsEmpty();

		void f_TestReport(CTestResults &_Results) const;

		operator bool () const;

		NStr::CStr f_ModifyDescription(NStr::CStr const &_Description);

		template <typename tf_CFormatter>
		int f_GetStringFormatType(tf_CFormatter &_Formatter);
		template <typename tf_CFormatter>
		NStr::CStrFormatTypeClassifier_String f_CreateStringFormatter(tf_CFormatter &_Formatter) const;

	private:
		CTestMemoryResults mp_Results;
		bool mp_bCompareNumAllocations;

		CTestMemoryResult &fp_Add(CTestMemoryMeasure &_Measure);
		NStr::CStr fp_GetOutput() const;
		void fp_GetBest(CTestMemoryResult const * &_pBestRef, CTestMemoryResult const * &_pBest) const;
	};

	struct CTestMemoryMaxAllocatedBytes : public CTestMemory
	{
		CTestMemoryMaxAllocatedBytes(fp64 const &_Tolerance);
	};

	struct CTestMemoryNumAllocations : public CTestMemory
	{
		CTestMemoryNumAllocations(fp64 const &_Tolerance);
	};
}

#include "Malterlib_Test_Memory.hpp"
