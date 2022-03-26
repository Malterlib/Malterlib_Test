// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Test_Expression.h"
#include <Mib/Core/RuntimeType>
#include <Mib/Container/Registry>

namespace NMib::NConcurrency
{
	struct CAllowUnsafeThis;
}

namespace NMib::NTest
{
	enum ETest
	{
		ETest_None,
		ETest_Warn,
		ETest_Fail,
		ETest_FailAndStop,
		ETest_ExpectFail,
		ETest_ExpectFailAndStop,
	};

	enum ETestFlag
	{
		ETestFlag_None = 0,
		ETestFlag_NoValues = DMibBit(0),
		ETestFlag_NoExceptionFilter = DMibBit(1),
		ETestFlag_Aggregated = DMibBit(2),
		ETestFlag_NoValuesOnSuccess = DMibBit(3),
	};

	enum ETestCategoryFlag
	{
		ETestCategoryFlag_None = 0,
		ETestCategoryFlag_DisableValues = DMibBit(0),
		ETestCategoryFlag_DisableExceptionFilter = DMibBit(1),
		ETestCategoryFlag_EnableValues = DMibBit(2),
		ETestCategoryFlag_EnableExceptionFilter = DMibBit(3),
		ETestCategoryFlag_Tests = DMibBit(4),
	};

	enum ECheckType
	{
		ECheckType_Predicate,
		ECheckType_Message,
	};

	enum ETestReportFlag
	{
		ETestReportFlag_None,
		ETestReportFlag_Success = DMibBit(0),
		ETestReportFlag_Warning = DMibBit(1),
		ETestReportFlag_Fail = DMibBit(2),
		ETestReportFlag_FailAndStop = DMibBit(3),
		ETestReportFlag_ExpectFail = DMibBit(4),
		ETestReportFlag_ExpectFailAndStop = DMibBit(5),
		ETestReportFlag_ReportCategories = DMibBit(6),
		ETestReportFlag_DetailedPerformance = DMibBit(7),
		ETestReportFlag_Ignored = DMibBit(8),
		ETestReportFlag_DetailedMemory = DMibBit(9),
		ETestReportFlag_BreakOnFail = DMibBit(10),
		ETestReportFlag_ProcessRecursive = DMibBit(11),
		ETestReportFlag_CompareToBaseline = DMibBit(12),
		ETestReportFlag_CrashOnException = DMibBit(13),
		ETestReportFlag_ReportValues = DMibBit(14),

		ETestReportFlag_Default = ETestReportFlag_Warning | ETestReportFlag_Fail | ETestReportFlag_FailAndStop,
		ETestReportFlag_All = ETestReportFlag_Default | ETestReportFlag_Success | ETestReportFlag_ExpectFail | ETestReportFlag_ExpectFailAndStop | ETestReportFlag_Ignored
	};

	enum ETestResult
	{
		ETestResult_Fail = 0
		, ETestResult_Success = 1
		, ETestResult_Ignored = 2
	};

	enum ETestMeasureType
	{
		ETestMeasureType_Normal = 0		// Used for the application you are trying to measure
		, ETestMeasureType_Reference	// Used as a reference implementation to compare against
		, ETestMeasureType_Baseline		// Should be used as a baseline for measuring the overhead of testing code
		, ETestMeasureType_Debug		// Used for debug, just to compare results with for example less features enabled
	};

	class CTest : public NConcurrency::CAllowUnsafeThis
	{
	public:
		virtual ~CTest()
		{
		}
		virtual void f_DoTests() = 0;
	};

#	if DMibConfig_Tests_Enable
#		define	DMibTestRegister(d_ClassName, d_TestSuitePath) DMibRuntimeClassNamedCasted(NMib::NTest::CTest, d_ClassName, NTest::d_TestSuitePath::d_ClassName, ::NMib::NTest::CTest)
#	else
#		define	DMibTestRegister(d_ClassName, d_TestSuitePath) uint8 g_RuntimeClassNamed_##d_ClassName
#	endif
	struct CTestStats
	{
		CTestStats()
			: m_Min(0.0)
			, m_Max(0.0)
			, m_Average(0.0)
			, m_Median(0.0)
			, m_StdDev(0.0)
		{
		}

		CTestStats(fp64 _Stat)
			: m_Min(_Stat)
			, m_Max(_Stat)
			, m_Average(_Stat)
			, m_Median(_Stat)
			, m_StdDev(0.0)
		{
		}

		fp64 f_StdDevFraction() const
		{
			if (m_Average != 0.0)
				return m_StdDev / m_Average;
			else
				return 0.0;
		}

		template <typename tf_CStream>
		void f_Stream(tf_CStream &_Stream)
		{
			_Stream % m_Min;
			_Stream % m_Max;
			_Stream % m_Average;
			_Stream % m_Median;
			_Stream % m_StdDev;
		}

		fp64 m_Min;
		fp64 m_Max;
		fp64 m_Average;
		fp64 m_Median;
		fp64 m_StdDev;
	};

	struct CTestResult
	{
		CTestResult()
			: m_ThreadID(0)
			, m_ResultID(0)
			, m_Result(ETestResult_Fail)
			, m_Line(0)
			, m_FailureAction(ETest_None)
			, m_CheckType(ECheckType_Predicate)
			, m_Flags(ETestFlag_None)
		{
		}

		// ThreadID and ResultID is unique together
		uint64 m_ThreadID;
		uint32 m_ResultID;

		ETestResult m_Result;
		NStr::CStr m_TestPath;
		NStr::CStr m_Message;
		NStr::CStr m_Values;
		NStr::CStr m_File;
		int32 m_Line;
		ETest m_FailureAction;
		ECheckType m_CheckType;
		ETestFlag m_Flags;
		NStr::CStr m_ExtraMultiLineReportData;
	};

	struct CTestPerformanceResult
	{
		CTestPerformanceResult()
			: m_nIterations(0)
			, m_nRepetitions(0)
			, m_MeasureType(ETestMeasureType_Normal)
			, m_nContributingThreads(0)
		{
		}
		NStr::CStr m_Name;
		uint64 m_nIterations;
		uint64 m_nRepetitions;
		// All the following values per iteration
		CTestStats m_Cycles;
		CTestStats m_Time;
		NContainer::TCMap<NStr::CStr, CTestStats> m_PerformanceCounters; // Used to report performance counters. For example L2misses etc.
		ETestMeasureType m_MeasureType;
		uint32 m_nContributingThreads;
	};

	struct CTestMemoryStats
	{
		CTestStats m_nAllocations;
		CTestStats m_nFree;
		CTestStats m_nResize;
		CTestStats m_nRealloc;
		CTestStats m_nGetSize;
		CTestStats m_nProtect;
		CTestStats m_nCommit;
		CTestStats m_nDecommit;

		CTestStats m_BytesAlloc; // Includes resize and realloc activity
		CTestStats m_BytesFree; // Includes resize and realloc activity
		CTestStats m_BytesResize;
		CTestStats m_BytesRealloc;
		CTestStats m_BytesProtect;
		CTestStats m_BytesCommit;
		CTestStats m_BytesDecommit;

		CTestStats m_BytesOverhead;
		CTestStats m_BytesPotentialWaste;

		CTestStats m_BytesMaxAlloc;
		struct CAtMaxAlloc
		{
			CTestStats m_BytesOverhead;
			CTestStats m_BytesPotentialWaste;
			CTestStats m_nAllocations;
		};
		CAtMaxAlloc m_AtBytesMaxAlloc;
	};

	struct CTestMemoryResult
	{
		CTestMemoryResult()
			: m_nIterations(0)
			, m_nRepetitions(0)
			, m_MeasureType(ETestMeasureType_Normal)
		{
		}
		NStr::CStr m_Name;
		uint64 m_nIterations;
		uint64 m_nRepetitions;
		// All the following values per iteration
		CTestMemoryStats m_AllAllocations;
		NContainer::TCMap<NStr::CStr, CTestMemoryStats> m_PerAllocationType;
		ETestMeasureType m_MeasureType;
	};

	struct CTestPerformanceResults
	{
		CTestPerformanceResults()
			: m_Tolerance(0.0)
		{
		}
		fp64 m_Tolerance;
		NContainer::TCVector<CTestPerformanceResult> m_Results;
	};

	struct CTestMemoryResults
	{
		CTestMemoryResults()
			: m_Tolerance(0.0)
		{
		}
		fp64 m_Tolerance;
		NContainer::TCVector<CTestMemoryResult> m_Results;
	};

	class CTestGroup
	{
		NStr::CStr mp_Group;
	public:
		CTestGroup(NStr::CStr const &_Group)
			: mp_Group(_Group)
		{
			DMibRequire
				(
				 	_Group == "Performance"
				 	|| _Group == "Torture"
				 	|| _Group == "Memory"
				 	|| _Group == "Unfinished"
				 	|| _Group == "Expensive"
				 	|| _Group == "Manual"
				 	|| _Group == "SuperUser"
				)
			;
		}
		NStr::CStr const &f_GetGroup() const
		{
			return mp_Group;
		}
	};

	class CTestCategory
	{
		NStr::CStr m_Category;
		NContainer::TCMap<NStr::CStr> m_Groups;
	public:
		CTestCategory(ch8 const *_pCategory)
			: m_Category(_pCategory)
		{
		}
		CTestCategory(NStr::CStr const &_Category)
			: m_Category(_Category)
		{
		}
		CTestCategory(NStr::CStr::CFormat const &_Category)
			: m_Category(_Category)
		{
		}
		CTestCategory &operator << (CTestGroup const &_Group)
		{
			m_Groups[_Group.f_GetGroup()];
			return *this;
		}
		NStr::CStr const &f_GetCategory() const
		{
			return m_Category;
		}
		NContainer::TCMap<NStr::CStr> const &f_GetGroups() const
		{
			return m_Groups;
		}
	};

	enum ETestNeedReportFlag
	{
		ETestNeedReportFlag_None = 0
		, ETestNeedReportFlag_Report = DMibBit(0)
		, ETestNeedReportFlag_Count = DMibBit(1)
		, ETestNeedReportFlag_AllowDuplicates = DMibBit(2)
		, ETestNeedReportFlag_Abort = DMibBit(3)
	};

	struct CTestLocation
	{
		CTestLocation()
		{
		}

		CTestLocation(NStr::CStr  const &_File, int _Line)
			: m_File(_File)
			, m_Line(_Line)
		{
		}

		NStr::CStr m_File;
		int32 m_Line = -1;
	};

	class CTestResults
	{
	public:
		virtual ETestReportFlag f_GetReportFlags()
		{
			return ETestReportFlag_None;
		}
		virtual void f_ReportHeader(ETestReportFlag _ReportFlags) = 0;
		virtual void f_ReportFooter(mint _nTestsTotal, mint _nSuccess, mint _nSuccessUnexpected, mint _nFailed, mint _nExpectFailed, mint _nWarnings, mint _nIgnored) = 0;
		virtual void f_ReportResult
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
			) = 0
		;
		virtual void f_ReportSuite(const NStr::CStr &_TestPath, const NContainer::TCMap<NStr::CStr> &_TestGroups, CTestLocation const &_Location) = 0;
		virtual ETestNeedReportFlag f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags) = 0;
		virtual void f_PerformanceResults(CTestPerformanceResults const &_Results) = 0;
		virtual void f_MemoryResults(CTestMemoryResults const &_Results) = 0;
	};

	struct CRunTestOptions
	{
		NContainer::TCVector<NStr::CStr> m_Paths;
		NContainer::TCVector<NStr::CStr> m_ExcludeGroups;
		NContainer::TCVector<NStr::CStr> m_IncludeGroups;
		ETestReportFlag m_ReportFlags = ETestReportFlag_Default | ETestReportFlag_ReportValues;
		NMib::NStr::CStr m_ExtraData;
	};

	uint32 fg_RunTests(CRunTestOptions const &_Options);
	uint32 fg_RunTests(CTestResults *_pResults, CRunTestOptions const &_Options);
	uint32 fg_RunTests(); // Parses command line
	ETestReportFlag fg_TestReportFlags();
	NStr::CStr fg_TestGetCurrentPath();
	NStr::CStr fg_TestGetExtraData();
	NContainer::TCMap<NStr::CStr> fg_TestGetCurrentGroups();

	bool fg_GroupActive(NStr::CStr const &_Group);

	void fg_TestSetReturnValue(uint32 _RetValue); // Set the return value for the application

#if defined(DMibSanitizerEnabled)
	constexpr pfp64 gc_TimeoutMultiplier = 10.0;
#elif defined(DMibDebug)
	constexpr pfp64 gc_TimeoutMultiplier = 2.0;
#else
	constexpr pfp64 gc_TimeoutMultiplier = 1.0;
#endif
}

#include "Malterlib_Test_Private.h"

namespace NMib::NTest
{
#	if DMibConfig_Tests_Enable

		typedef NPrivate::CTestCategoryScope CTestCategoryScope;
		typedef NPrivate::CTestPathScope CTestPathScope;
		typedef NPrivate::TCTestFunctionHelper<false> CTestFunctionHelper;
		typedef NPrivate::TCTestFunctionHelper<true> CTestFunctionHelperOnlyFailure;
#	else
		class CTestCategoryScope
		{
		public:
			CTestCategoryScope(const CTestCategory &_Category, const ch8 *_pFile, int32 _Line, ETestCategoryFlag _Flags = ETestCategoryFlag_None)
			{
			}

			~CTestCategoryScope()
			{
			}

			template <typename t_CFunctionType>
			void operator % (const t_CFunctionType &_Function)
			{
			}
		};

		class CTestPathScope
		{
		public:
			CTestPathScope(NStr::CStr const &_Path, ch8 const *_pFile, uint32 _Line)
			{
			}
			~CTestPathScope()
			{
			}
		};

#	endif


#	define DMibTestCategory(d_TestCategory) ::NMib::NTest::CTestCategoryScope(d_TestCategory, DMibPFile, DMibPLine) % [&] ()
#	define DMibTestSuite(d_TestCategory) ::NMib::NTest::CTestCategoryScope(d_TestCategory, DMibPFile, DMibPLine, ::NMib::NTest::ETestCategoryFlag_Tests) % [&] ()
#	define DMibTestCategoryFlags(d_TestCategory, d_Flags) ::NMib::NTest::CTestCategoryScope(d_TestCategory, DMibPFile, DMibPLine, d_Flags) % [&] ()
#	define DMibTestPath(d_Path) ::NMib::NTest::CTestPathScope MalterlibTestPathScope(d_Path, DMibPFile, DMibPLine)

#	if DMibConfig_Tests_Enable
#		define DMibTest false ? false : ::NMib::NTest::CTestFunctionHelper(DMibPFile, DMibPLine, ::NMib::NTest::NPrivate::CDummyExpression())
#		define DMibTestExpr(d_Expression) false ? false : ::NMib::NTest::CTestFunctionHelper(DMibPFile, DMibPLine, ::NMib::NTest::NPrivate::CDummyExpression())(DMibExpr(d_Expression))
#		define DMibTestOnlyFail false ? false : ::NMib::NTest::CTestFunctionHelperOnlyFailure(DMibPFile, DMibPLine, ::NMib::NTest::NPrivate::CDummyExpression())
#		define DMibTestMark ::NMib::NTest::NPrivate::fg_SetTestLastLocation(DMibPFile, DMibPLine)
#	else
		enum {DMibTestHelperExpression_A, DMibTestHelperExpression_B };
		#define DMibTestHelperExpression_B(_Expression) DMibTestHelperExpression_OP(_Expression, A)
		#define DMibTestHelperExpression_A(_Expression) DMibTestHelperExpression_OP(_Expression, B)
		#define DMibTestHelperExpression_OP(_Expression, _Next) DMibTestHelperExpression_ ## _Next

		#define DMibTest false ? false : DMibTestHelperExpression_A
		#define DMibTestExpr(d_Expression) false ? false : DMibTestHelperExpression_A
		#define DMibTestOnlyFail false ? false : DMibTestHelperExpression_A
		#define DMibTestMark
#	endif

#	define DMibAssert(d_Left, d_Operator, d_Right) DMibTest(DMibExpr(d_Left) d_Operator DMibExpr(d_Right))(::NMib::NTest::ETest_FailAndStop)
#	define DMibAssertException(d_Expression, d_Exception) DMibTest(DMibExpr(NMib::NTest::fg_ThrowsException(d_Exception)) == DMibLExpr(d_Expression))(::NMib::NTest::ETest_FailAndStop)
#	define DMibAssertExceptionType(d_Expression, d_ExceptionType) DMibTest(DMibExpr(NMib::NTest::TCThrowsException<d_ExceptionType>()) == DMibLExpr(d_Expression))(::NMib::NTest::ETest_FailAndStop)
#	define DMibAssertViolatesRequire(d_Expression, ...) DMibTest(DMibExpr(NMib::NTest::fg_ViolatesRequire(__VA_ARGS__)) == DMibLExpr(d_Expression))(::NMib::NTest::ETest_FailAndStop)
#	define DMibAssertViolatesSafeCheck(d_Expression, ...) DMibTest(DMibExpr(NMib::NTest::fg_ViolatesSafeCheck(__VA_ARGS__)) == DMibLExpr(d_Expression))(::NMib::NTest::ETest_FailAndStop)
#	define DMibAssertTrue(d_Right) DMibTest(DMibExpr(d_Right))(::NMib::NTest::ETest_FailAndStop)
#	define DMibAssertFalse(d_Right) DMibTest(!DMibExpr(d_Right))(::NMib::NTest::ETest_FailAndStop)

#	define DMibExpect(d_Left, d_Operator, d_Right) DMibTest(DMibExpr(d_Left) d_Operator DMibExpr(d_Right))
#	define DMibExpectException(d_Expression, d_Exception) DMibTest(DMibExpr(::NMib::NTest::fg_ThrowsException(d_Exception)) == DMibLExpr(d_Expression))
#	define DMibExpectExceptionType(d_Expression, d_ExceptionType) DMibTest(DMibExpr(::NMib::NTest::TCThrowsException<d_ExceptionType>()) == DMibLExpr(d_Expression))
#	define DMibExpectViolatesRequire(d_Expression, ...) DMibTest(DMibExpr(::NMib::NTest::fg_ViolatesRequire(__VA_ARGS__)) == DMibLExpr(d_Expression))
#	define DMibExpectViolatesSafeCheck(d_Expression, ...) DMibTest(DMibExpr(::NMib::NTest::fg_ViolatesSafeCheck(__VA_ARGS__)) == DMibLExpr(d_Expression))
#	define DMibExpectTrue(d_Right) DMibTest(DMibExpr(d_Right))
#	define DMibExpectFalse(d_Right) DMibTest(!DMibExpr(d_Right))

#	ifndef DMibPNoShortCuts
#		define DTest DMibTest
#		define DTestMark DMibTestMark
#		define DAssert DMibAssert
#		define DAssertException DMibAssertException
#		define DAssertViolatesRequire DMibAssertViolatesRequire
#		define DAssertViolatesSafeCheck DMibAssertViolatesSafeCheck
#		define DAssertExceptionType DMibAssertExceptionType
#		define DAssertTrue DMibAssertTrue
#		define DAssertFalse DMibAssertFalse
#		define DExpect DMibExpect
#		define DExpectException DMibExpectException
#		define DExpectViolatesRequire DMibExpectViolatesRequire
#		define DExpectViolatesSafeCheck DMibExpectViolatesSafeCheck
#		define DExpectExceptionType DMibExpectExceptionType
#		define DExpectTrue DMibExpectTrue
#		define DExpectFalse DMibExpectFalse
#		define DTestExpr DMibTestExpr
#		define DTestOnlyFail DMibTestOnlyFail
#		define DTestPath DMibTestPath
#		define DTestRegister DMibTestRegister
#		define DTestCategory DMibTestCategory
#		define DTestCategoryFlags DMibTestCategoryFlags
#		define DTestSuite DMibTestSuite
#	endif
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NTest;
#endif

