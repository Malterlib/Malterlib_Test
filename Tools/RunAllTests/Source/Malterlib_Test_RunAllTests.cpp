// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/Application>
#include <Mib/Process/ProcessLaunch>
#include <Mib/Process/VirtualProcessLaunch>
#include <Mib/Concurrency/ThreadSafeQueue>
#include <Mib/Cryptography/UUID>
#include <Mib/CommandLine/CommandLineClient>
#include <Mib/Encoding/JsonShortcuts>

using namespace NMib;
using namespace NMib::NProcess;
using namespace NMib::NFunction;
using namespace NMib::NStr;

extern ch8 const *g_AllTests[];
extern mint g_nAllTests;

#if DMalterlibCodeCoverage
	extern ch8 const *g_AllCoverageBinaries[];
	extern mint g_nAllCoverageBinaries;

	CUniversallyUniqueIdentifier g_UUIDNamespace{"6B2A543F-CB3F-4B6D-A617-7D286BBC931E", EUniversallyUniqueIdentifierFormat_Bare};
#endif

struct CRunAllTestsApplication : public NMib::CApplication
{
	constexpr static uint64 mc_DefaultMemoryPerTestBase = (3 * 1024) / 2;

#if defined(DMibSanitizerEnabled_UndefinedBehavior)
	constexpr static uint64 mc_DefaultMemoryPerTest = (mc_DefaultMemoryPerTestBase * 3) / 2;
#elif defined(DMibSanitizerEnabled_Address)
	constexpr static uint64 mc_DefaultMemoryPerTest = mc_DefaultMemoryPerTestBase * 8;
#elif defined(DMibSanitizerEnabled_Thread)
	constexpr static uint64 mc_DefaultMemoryPerTest = mc_DefaultMemoryPerTestBase * 3;
#elif defined(DMibDebug)
	constexpr static uint64 mc_DefaultMemoryPerTest = (mc_DefaultMemoryPerTestBase * 5) / 2;
#else
	constexpr static uint64 mc_DefaultMemoryPerTest = mc_DefaultMemoryPerTestBase;
#endif

	CRunAllTestsApplication()
	{
	}

	aint f_Main()
	{
		NStorage::TCSharedPointer<NMib::NCommandLine::CCommandLineSpecification> pCommandLineSpec = fg_Construct();
		pCommandLineSpec->f_AddHelpCommand();
		pCommandLineSpec->f_AddTerminalOptions();

		auto GroupsList = NCommandLine::COneOf{"Default", "Performance", "Torture", "Memory", "Unfinished", "Expensive", "Manual", "SuperUser"};

		auto Option_SuiteOrder = "SuiteOrder?"_o=
			{
				"Names"_o= _o["--suite-order"]
				, "Type"_o= COneOf{"natural", "slow_first", "fast_first", "random"}
				, "Default"_o= "slow_first"
				, "Description"_o= "The order to schedule the test suites in.\n"
				"If no previous run time statistics exists, slow_first and fast_first suite order will behave the same as natural order."
			}
		;
		auto Option_Groups = "Groups?"_o=
			{
				"Names"_o= _o["--groups", "-g"]
				, "Default"_o= _o["Default"]
				, "Type"_o= _o[GroupsList]
				, "Description"_o= "Specify the groups to include in test.\n"
				"@Indent=17\r"
				"   Default:      Run tests without a group specified.\r"
				"   Performance:  Run tests with Performance group specified.\r"
				"   Torture:      Run tests with Torture group specified.\r"
				"   Memory:       Run tests with Performance group specified.\r"
				"   Unfinished:   Run tests with Unfinished group specified.\r"
				"   Expensive:    Run tests with Expensive group specified.\r"
				"   Manual:       Run tests with Manual group specified.\r"
				"   SuperUser:    Run tests with SuperUser group specified.\r"
				"\r"
			}
		;
		auto Option_Paths = "Paths?"_o=
			{
				"Names"_o= _o["--paths"]
				, "Default"_o= _o[]
				, "Type"_o= _o[""]
				, "Description"_o= "Specify the paths to include in test.\n"
			}
		;
		auto Option_FlakyErrors = "FlakyErrors?"_o=
			{
				"Names"_o= _o["--flaky-errors"]
				, "Type"_o= _o[""]
				, "Default"_o= fg_GetSys()->f_GetEnvironmentVariable("MalterlibFlakyErrors", "").f_Split<true>(";")
				, "Description"_o= "String to look for in test output to determine if the test was flaky. These tests will be rerun up to 10 times to check for success.\n"
				"Will only be respected when --launch-per-suite is true.\n"
			}
		;

		auto Section = pCommandLineSpec->f_AddSection("Test", "Run tests.");
		Section.f_RegisterDirectCommand
			(
				{
					"Names"_o= _o["--list-all-tests"]
					, "Description"_o= "List all test suites in all test binaries.\n"
					, "Options"_o=
					{
						Option_Groups
						, Option_Paths
						, Option_SuiteOrder
						, Option_FlakyErrors
					}
				}
				, [this](NEncoding::CEJsonSorted const &_Parameters, NCommandLine::CCommandLineClient &_CommandLineClient)
				{
					return fp_RunTests(_Parameters, _CommandLineClient.f_AnsiEncoding(), true);
				}
			)
		;

		auto RunAllTestsCommand = Section.f_RegisterDirectCommand
			(
				{
					"Names"_o= _o["--run-all-tests"]
					, "Description"_o= "Run all test suites in all test binaries.\n"
					, "Options"_o=
					{
						"Parallel?"_o=
						{
							"Names"_o= _o["--parallel", "-p"]
							, "Default"_o= true
							, "Description"_o= "Run tests in parallel utilizing all cores.\n"
						}
						, "Quiet?"_o=
						{
							"Names"_o= _o["--quiet"]
							, "Default"_o= true
							, "Description"_o= "Don't output test results unless a failure occurs.\n"
						}
						, "QuietStats?"_o=
						{
							"Names"_o= _o["--quiet-stats"]
							, "Default"_o= true
							, "Description"_o= "Don't output memory and concurrency statistics.\n"
						}
						, "ReportSuccess?"_o=
						{
							"Names"_o= _o["--report-success"]
							, "Default"_o= true
							, "Description"_o= "Report a success message when all tests pass.\n"
						}
						, "Loop?"_o=
						{
							"Names"_o= _o["--loop"]
							, "Default"_o= false
							, "Description"_o= "Loop tests until aborted.\n"
						}
						, "MemoryPerTest?"_o=
						{
							"Names"_o= _o["--memory-per-test"]
							, "Default"_o= mc_DefaultMemoryPerTest
							, "Description"_o= "The amount of memory needed per test. Concurrency will be limited by amount of available memory.\n"
						}
						, "Timeout?"_o=
						{
							"Names"_o= _o["--timeout"]
							, "Default"_o= fp64::fs_Inf()
							, "Description"_o= "Stop tests after this timeout.\n"
						}
						, "LoopIterations?"_o=
						{
							"Names"_o= _o["--iterations", "-i"]
							, "Default"_o= 0
							, "Description"_o= "Abort loop after iterations.\n"
						}
						, "LoopAbortOnFailure?"_o=
						{
							"Names"_o= _o["--abort-on-failure", "-a"]
							, "Default"_o= false
							, "Description"_o= "Abort loop after first failure.\n"
						}
						, "LaunchPerSuite?"_o=
						{
							"Names"_o= _o["--launch-per-suite"]
							, "Default"_o= true
							, "Description"_o= "Launch the executable per suite.\n"
						}
						, "FlakySuites?"_o=
						{
							"Names"_o= _o["--flaky-suites"]
							, "Type"_o= _o[""]
							, "Default"_o= fg_GetSys()->f_GetEnvironmentVariable("MalterlibFlakySuites", "").f_Split<true>(";")
							, "Description"_o= "Wildcard for test paths that are expected flaky. These tests will be rerun up to 10 times to check for success.\n"
							"Will only be respected when --launch-per-suite is true.\n"
						}
						, Option_FlakyErrors
						, Option_Groups
						, Option_Paths
						, Option_SuiteOrder
#if DMalterlibCodeCoverage
						, "Coverage?"_o=
						{
							"Names"_o= _o["--coverage"]
							, "Default"_o= true
							, "Description"_o= "Record code coverage and report results.\n"
						}
						, "CoverageOnly?"_o=
						{
							"Names"_o= _o["--coverage-only"]
							, "Default"_o= false
							, "Description"_o= "Only display coverage results from previous run, don't run tests.\n"
						}
						, "CoverageSources?"_o=
						{
							"Names"_o= _o["--coverage-sources"]
							, "Default"_o= _o[]
							, "Type"_o= _o[""]
							, "Description"_o= "Only display coverage for these source files. Can use wildcarcds.\n"
						}
						, "CoverageExecutable?"_o=
						{
							"Names"_o= _o["--coverage-executable"]
							, "Default"_o= ""
							, "Description"_o= "Specify the executable used to display code coverage results.\n"
						}
#endif
					}
					, "Parameters"_o=
					{
						"TestParams...?"_o=
						{
							"Type"_o= _o[""]
							, "Default"_o= _o[]
							, "Description"_o= "The parameters to forward to the individual tests."
						}
					}
					, "ErrorOnCommandAsParameter"_o= false
					, "ErrorOnOptionAsParameter"_o= false
					, "GreedyDefaultCommandParameters"_o= true
				}
				, [this](NEncoding::CEJsonSorted const &_Parameters, NCommandLine::CCommandLineClient &_CommandLineClient)
				{
					return fp_RunTests(_Parameters, _CommandLineClient.f_AnsiEncoding(), false);
				}
			)
		;

		pCommandLineSpec->f_SetDefaultCommand(RunAllTestsCommand);

		NCommandLine::CCommandLineClient Client(pCommandLineSpec);

		try
		{
			return Client.f_RunCommandLine();
		}
		catch (NException::CException const &_Exception)
		{
			DMibConErrOut("{}\n", _Exception);
			return 1;
		}
	}

private:
	struct CSettings
	{
		template <typename tf_CType>
		static tf_CType fs_GetSetting(NEncoding::CEJsonSorted const &_Parameters, CStr const &_Name)
		{
			auto *pValue = _Parameters.f_GetMember(_Name);
			if (!pValue)
				return {};

			if constexpr (cIsSame<tf_CType, TCVector<CStr>>)
				return pValue->f_StringArray();
			else if constexpr (cIsSame<tf_CType, CStr>)
				return pValue->f_String();
			else if constexpr (cIsSame<tf_CType, bool>)
				return pValue->f_Boolean();
			else if constexpr (cIsSame<tf_CType, fp64>)
				return pValue->f_Float();
			else if constexpr (cIsSame<tf_CType, int64>)
				return pValue->f_Integer();
			else
				static_assert(cIsSame<tf_CType, void>, "Unsupported type");

			return {};
		}

		CSettings(NEncoding::CEJsonSorted const &_Parameters, bool _bList)
			: m_TestParams(fs_GetSetting<TCVector<CStr>>(_Parameters, "TestParams"))
			, m_TestGroups(fs_GetSetting<TCVector<CStr>>(_Parameters, "Groups"))
			, m_TestPaths(fs_GetSetting<TCVector<CStr>>(_Parameters, "Paths"))
			, m_bParallel(fs_GetSetting<bool>(_Parameters, "Parallel"))
			, m_bLoopTests(fs_GetSetting<bool>(_Parameters, "Loop"))
			, m_bQuiet(fs_GetSetting<bool>(_Parameters, "Quiet"))
			, m_bQuietStats(fs_GetSetting<bool>(_Parameters, "QuietStats"))
			, m_bReportSuccess(fs_GetSetting<bool>(_Parameters, "ReportSuccess"))
			, m_bLaunchPerSuite(_bList || fs_GetSetting<bool>(_Parameters, "LaunchPerSuite"))
			, m_bAbortOnFailure(fs_GetSetting<bool>(_Parameters, "LoopAbortOnFailure"))
			, m_nLoops(fs_GetSetting<int64>(_Parameters, "LoopIterations"))
			, m_Timeout(fs_GetSetting<fp64>(_Parameters, "Timeout"))
			, m_MemoryPerTest(fs_GetSetting<int64>(_Parameters, "MemoryPerTest"))
			, m_FlakySuites(TCSet<CStr>::fs_FromContainer(fs_GetSetting<TCVector<CStr>>(_Parameters, "FlakySuites")))
			, m_FlakyErrors(TCSet<CStr>::fs_FromContainer(fs_GetSetting<TCVector<CStr>>(_Parameters, "FlakyErrors")))
#if DMalterlibCodeCoverage
			, m_bCoverage(fs_GetSetting<bool>(_Parameters, "Coverage"))
			, m_bCoverageOnly(fs_GetSetting<bool>(_Parameters, "CoverageOnly"))
			, m_CoverageExecutable(fs_GetSetting<CStr>(_Parameters, "CoverageExecutable"))
			, m_CoverageSources(fs_GetSetting<TCVector<CStr>>(_Parameters, "CoverageSources"))
#endif
			, m_SuiteOrder(fs_GetSetting<CStr>(_Parameters, "SuiteOrder"))
			, m_bList(_bList)
		{
			if (!m_bLoopTests)
				m_nLoops = 1;
		}

		TCVector<CStr> m_TestParams;
		TCVector<CStr> m_TestGroups;
		TCVector<CStr> m_TestPaths;

		TCSet<CStr> m_FlakySuites;
		TCSet<CStr> m_FlakyErrors;

		CStr m_SuiteOrder;

		int64 m_nLoops = 0;
		fp64 m_Timeout = fp64::fs_Inf();
		uint64 m_MemoryPerTest = 0;
#if DMalterlibCodeCoverage
		CStr m_CoverageExecutable;
		TCVector<CStr> m_CoverageSources;
		CStr m_CoverageDirectory = NFile::CFile::fs_AppendPath(NFile::CFile::fs_GetProgramDirectory(), "Coverage");
		bool m_bCoverage = false;
		bool m_bCoverageOnly = false;
#endif
		bool m_bParallel = true;
		bool m_bLoopTests = false;
		bool m_bQuiet = true;
		bool m_bQuietStats = false;
		bool m_bReportSuccess = true;
		bool m_bLaunchPerSuite = false;
		bool m_bAbortOnFailure = false;
		bool m_bList = false;
	};

#if DMalterlibCodeCoverage
	void fp_DisplayCoverage(CSettings const &_Settings)
	{
		CStr ClangPath = DMalterlibClangPath;
		CDisableExceptionTraceScope DisableExceptionTrace;
		try
		{
			{
				TCVector<CStr> Params =
					{
						"merge"
						, "-sparse"
						, "-o"
						, NFile::CFile::fs_GetProgramDirectory() / "Coverage/Tests.profdata"
					}
				;

				for (auto &CoverageFile : CFile::fs_FindFiles(fg_Format("{}/*.profraw", _Settings.m_CoverageDirectory), EFileAttrib_File, false))
				{
					{
						CFile File;
						File.f_Open(CoverageFile, EFileOpen_Read);
						if (File.f_GetLength() == 0)
							continue;
					}
					Params.f_Insert(CoverageFile);
				}

				CProcessLaunch::fs_LaunchTool(ClangPath / "llvm-profdata", Params);
			}

			{
				TCVector<CStr> Params =
					{
						"report"
						, fg_Format("-instr-profile={}", NFile::CFile::fs_GetProgramDirectory() / "Coverage/Tests.profdata")
					}
				;

				if (!_Settings.m_CoverageExecutable.f_IsEmpty())
					Params.f_Insert(_Settings.m_CoverageExecutable);
				else
				{
					for (mint i = 0; i < g_nAllCoverageBinaries; ++i)
					{
						CStr Binary = NFile::CFile::fs_GetProgramDirectory() / g_AllCoverageBinaries[i];

						if (Binary.f_EndsWith(".app"))
						{
							auto Files = NFile::CFile::fs_FindFiles(Binary / "Contents/MacOS/*");
							for (auto &File : Files)
							{
								if (!File.f_EndsWith(".dylib"))
								{
									Binary = File;
									break;
								}
							}
						}

						if (i > 0)
							Params.f_Insert("-object");
						Params.f_Insert(Binary);
					}
				}

				CStr Report = CProcessLaunch::fs_LaunchTool(ClangPath / "llvm-cov", Params);

				if (!_Settings.m_CoverageSources.f_IsEmpty())
				{
					uint64 TotalRegions = 0;
					uint64 TotalMissedRegions = 0;
					mint RegionsLocation = 0;
					mint MissedRegionsLocation = 0;
					mint PercentageLocation = 0;
					while (!Report.f_IsEmpty())
					{
						CStr Line = fg_GetStrLineSep(Report);
						if (Line.f_StartsWith("---"))
						{
							DConOut("{}{\n}", Line);
							continue;
						}
						CStr File;
						uint64 Regions = 0;
						uint64 MissedRegions = 0;

						ch8 const *pParse = Line;
						do
						{
							auto iSpaces = fg_StrFind(pParse, "  ");
							if (iSpaces < 0)
								break;
							File = CStr(pParse, iSpaces);

							pParse += iSpaces;
							while (*pParse == ' ')
								++pParse;
							Regions = fg_StrToIntParse(pParse, TCLimitsInt<uint64>::mc_Max);

							if (Regions == TCLimitsInt<uint64>::mc_Max)
								continue;

							RegionsLocation = pParse - Line.f_GetStr();

							while (*pParse == ' ')
								++pParse;

							MissedRegions = fg_StrToIntParse(pParse, TCLimitsInt<uint64>::mc_Max);

							if (MissedRegions == TCLimitsInt<uint64>::mc_Max)
								continue;

							MissedRegionsLocation = pParse - Line.f_GetStr();

							while (*pParse == ' ')
								++pParse;
							while (*pParse && *pParse != ' ')
								++pParse;
							PercentageLocation = pParse - Line.f_GetStr();
						}
						while (false)
							;

						if (File.f_IsEmpty())
							continue;

						if (File == "Filename")
						{
							DConOut("{}{\n}", Line);
							continue;
						}

						if (File == "TOTAL")
							continue;

						for (auto &Source : _Settings.m_CoverageSources)
						{
							if (fg_StrMatchWildcard(Line.f_GetStr(), Source.f_GetStr()) == EMatchWildcardResult_WholeStringMatchedAndPatternExhausted)
							{
								DConOut("{}{\n}", Line);
								TotalRegions += Regions;
								TotalMissedRegions += MissedRegions;
								//DConOut("{} {} {} {\n}", Total, Uncovered, Line);
								break;
							}
						}
					}

					DConOut
						(
							"TOTAL{a+,sj*}{a+,sj*}{fe2,sj*}%{\n}"
							, TotalRegions, RegionsLocation - 5
							, TotalMissedRegions, MissedRegionsLocation - RegionsLocation
							, (fp64(TotalRegions - TotalMissedRegions) / fp64(TotalRegions)) * 100.0, (PercentageLocation - MissedRegionsLocation) - 1
						)
					;

					//(PercentageLocation - MissedRegionsLocation)
//						DConOut("Params: {vs}{\n}", Params);
//						Report = CProcessLaunch::fs_LaunchTool(CFile::fs_AppendPath(ClangPath, "llvm-cov"), Params);
				}
				else
				{
					DConOut("{}{\n}", Report);
				}
			}
		}
		catch (NException::CException const &_Exception)
		{
			DMibError("Code coverage analysis failed: {}"_f << _Exception);
		}
	}
#endif

	struct CTestSuite
	{
		auto operator <=> (CTestSuite const &) const = default;

		template <typename tf_CStr>
		void f_Format(tf_CStr &o_Str) const
		{
			o_Str += typename tf_CStr::CFormat("{}: {}") << m_Executable << m_Suite;
		}

		CStr m_Executable;
		CStr m_Suite;
	};

	uint32 fp_ExecuteTests(CSettings const &_Settings, CAnsiEncoding const &_AnsiEncoding)
	{
#if DMalterlibCodeCoverage

		auto fModifyEnvironment = [&](auto &_Params, auto &_Test)
			{
				if (!_Settings.m_bCoverage)
					return;

				_Params.m_bMergeEnvironment = true;
				CStr CoverageFile = fg_Format
					(
						"{}-{}-%p-%9m.profraw"
						, CFile::fs_GetFileNoExt(_Test)
						, fg_GetHashedUuidString(_Test, g_UUIDNamespace, EUniversallyUniqueIdentifierFormat_AlphaNum)
					)
				;
				//_Params.m_Environment["MalterlibProtectedEnvironment"] = "LLVM_PROFILE_FILE";
				_Params.m_Environment["LLVM_PROFILE_FILE"] = _Settings.m_CoverageDirectory / CoverageFile;
			}
		;
#else
		auto fModifyEnvironment = [](auto &_Params, auto &_Test)
			{
			}
		;
#endif

		bool bRunningCI = fg_GetSys()->f_GetEnvironmentVariable("RunningCI", "") == "true";
		constexpr static mint c_MaxFlakyTries = 10;

		struct CFlakyState
		{
			mint m_nTries = 1;
			NProcess::CProcessLaunchParams m_Params;
		};

		struct CState
		{
			void f_Clear()
			{
				m_nFailed = 0;
				m_nTotalLaunches = 0;
				m_FlakyStateStates.f_Clear();
				m_nRunning = 0;
				m_nDone = 0;
				m_NotLaunched.f_Clear();
			}

			CSettings const m_Settings;
			CAnsiEncoding const m_AnsiEncoding;
			TCMap<mint, CFlakyState> m_FlakyStateStates;
			TCMap<CTestSuite, fp64> m_RunTimes;
			TCLinkedList<CProcessLaunchParams> m_NotLaunched;
			TCFunction<void ()> m_fAddLaunches;
			mint m_nRunning = 0;
			mint m_nDone = 0;
			mint m_nFailed = 0;
			mint m_nTotalLaunches = 0;
			uint32 m_CombinedExitCode = 0;
		};

		TCSharedPointer<CState> pState = fg_Construct(CState{.m_Settings = _Settings, .m_AnsiEncoding = _AnsiEncoding});

		auto CleanupState = g_OnScopeExit / [&]
			{
				pState->f_Clear();
				pState->m_fAddLaunches.f_Clear();
			}
		;

		bool bCancelled = false;
		bool bSignalled = false;
		bool bShouldOutput = false;
		CStopwatch SignalStopwatch;
		CStopwatch TimeoutStopwatch;
		fp64 LastSignal = 0.0;
		SignalStopwatch.f_Start();
		TimeoutStopwatch.f_Start();
		NThread::CEventAutoReset DispatchEvent;

		auto Cleaunup = NProcess::NPlatform::fg_Process_WaitForTermination
			(
				[&]
				{
					bSignalled = true;
					DispatchEvent.f_Signal();
				}
			)
		;

		int64 nLoops = 0;
		mint nThreads = NSys::fg_Thread_GetVirtualCores();

		CStr ProgramDirectory = NFile::CFile::fs_GetProgramDirectory();

		CHash_SHA256 RuntimesHash;

		auto fAddStringHash = [&](CStr const &_String)
			{
				RuntimesHash.f_AddData(_String.f_GetStr(), _String.f_GetLen());
			}
		;

		fAddStringHash(ProgramDirectory);
		fAddStringHash(CStr::fs_ToStr(_Settings.m_TestGroups));
		fAddStringHash(DMibStringize(DConfig));
		fAddStringHash(DMibStringize(DArchitecture));
#ifdef DMibSanitizerEnabled_UndefinedBehavior
		fAddStringHash("DMibSanitizerEnabled_UndefinedBehavior");
#endif
#ifdef DMibSanitizerEnabled_Address
		fAddStringHash("DMibSanitizerEnabled_Address");
#endif
#ifdef DMibSanitizerEnabled_Thread
		fAddStringHash("DMibSanitizerEnabled_Thread");
#endif

		CStr RuntimesPath = CFile::fs_GetUserHomeDirectory() / (".Malterlib/TestRuntimes/{}/TestRuntimes.json"_f << RuntimesHash.f_GetDigest().f_GetString().f_Left(8));

		TCVector<CProcessStatistics> MemoryStats;
		TCVector<CTestSuite> TestSuites;

		struct CSortedTestSuite
		{
			CTestSuite const *m_pTestSuite = nullptr;
			fp64 m_PreviousRunTime = 0.0;
			mint m_Index = 0;
		};

		TCVector<CSortedTestSuite> SortedTestSuites;

		TCMap<CTestSuite, fp64> PreviousRunTimes;

		if (CFile::fs_FileExists(RuntimesPath))
		{
			CEJsonSorted OutputJson = CEJsonSorted::fs_FromString(CFile::fs_ReadStringFromFile(RuntimesPath, true), RuntimesPath);
			for (auto &Suites : fg_Const(OutputJson).f_Object())
			{
				auto Executable = Suites.f_Name();

				for (auto &RunTimes : Suites.f_Value().f_Object())
					PreviousRunTimes[CTestSuite{.m_Executable = Executable, .m_Suite = RunTimes.f_Name()}] = RunTimes.f_Value().f_Float();
			}
		}
		else if (_Settings.m_SuiteOrder == "fast_first" || _Settings.m_SuiteOrder == "slow_first")
			DMibConOut("Warning: No previous runtimes exists for suites{\n}");

		while (!bCancelled && (!_Settings.m_nLoops || (nLoops < _Settings.m_nLoops)) && (!_Settings.m_bAbortOnFailure || !pState->m_nFailed))
		{
			++nLoops;
			MemoryStats.f_Clear();
			CProcessLaunchHandler LaunchHandler;

			TCSharedPointer<bool> pExited = fg_Construct(false);
			auto CleanupExited = g_OnScopeExit / [&]
				{
					*pExited = true;
				}
			;

			TCThreadSafeQueue<TCFunction<void ()>> ToDispatch;
			auto Cleanup = g_OnScopeExitShared / [&]
				{
					while (auto Entry = ToDispatch.f_Pop())
						(*Entry)();
				}
			;

			mint iNextFlakyID = 0;

			pState->f_Clear();

			mint nMaxRunning = 1;
			if (_Settings.m_bParallel)
				nMaxRunning = fg_Clamp(NProcess::NPlatform::fg_Process_GetPhysicalMemory() / (_Settings.m_MemoryPerTest * 1024 * 1024), 1, nThreads);

			if (!_Settings.m_bList && !_Settings.m_bQuietStats && nLoops == 1)
			{
				DMibConOut("Concurrency         {sj8,ns,}{\n}", nMaxRunning);
				DMibConOut("Memory per test     {sj8,ns,} MiB{\n}", _Settings.m_MemoryPerTest);
			}

			{
				auto fAddLaunches = [&]
					{
						if (bCancelled || pState->m_NotLaunched.f_IsEmpty() || pState->m_nRunning >= nMaxRunning)
							return false;

						if (_Settings.m_bAbortOnFailure && pState->m_nFailed)
						{
							pState->m_NotLaunched.f_Clear();
							return false;
						}

						auto Params = pState->m_NotLaunched.f_Pop();

						++pState->m_nRunning;
						LaunchHandler.f_AddLaunch
							(
								Params
								, false
								, [&](CProcessLaunchParams const &_Params, EProcessLaunchCloseFlag _DestructFlags) -> TCUniquePointer<CVirtualProcessLaunch>
								{
									return fg_Construct<CVirtualProcessLaunch_Default>(_Params, _DestructFlags);
								}
							)
						;
						return true;
					}
				;
				pState->m_fAddLaunches = fAddLaunches;
				auto Cleanup = g_OnScopeExit / [&]
					{
						pState->m_fAddLaunches.f_Clear();
					}
				;
				TCVector<TCFunction<void (CStr const &_Description, bool _bForceOutput)>> OutputDeferredOutput;
				mint MaxTestLen = 0;
				for (mint i = 0; i < g_nAllTests; ++i)
				{
					CStr Test = g_AllTests[i];
					MaxTestLen = fg_Max(MaxTestLen, (mint)Test.f_GetLen());
				}

				TestSuites.f_Clear();

				if (_Settings.m_bLaunchPerSuite)
				{
					CProcessLaunchHandler LaunchHandler;

					struct CExecutableResult
					{
						CProcessLaunchParams m_LaunchParams;
						CStr m_Output;
						CStr m_OutputWithErrors;
						uint32 m_ExitResult = 255;
					};

					struct CTestState
					{
						NThread::CMutual m_Lock;

						TCMap<CStr, CExecutableResult> m_Executables;
					};

					TCSharedPointer<CTestState> pTestState = fg_Construct();

					auto Cleanup = g_OnScopeExit / [&]
						{
							pTestState->m_Executables.f_Clear();
						}
					;

					for (mint i = 0; i < g_nAllTests; ++i)
					{
						CStr ExecutableName = g_AllTests[i];

						LaunchHandler.f_BlockOnExit(0.0, nMaxRunning - 1);

						TCVector<CStr> ExecutableParams = {"-l", "-g", CStr::fs_Join(_Settings.m_TestGroups, ",")};
						ExecutableParams.f_Insert(_Settings.m_TestPaths);

						CExecutableResult *pExecutable;
						{
							DMibLock(pTestState->m_Lock);
							pExecutable = &(pTestState->m_Executables[ExecutableName]);
						}

						pExecutable->m_LaunchParams = NProcess::CProcessLaunchParams::fs_LaunchExecutable
							(
								ProgramDirectory / ExecutableName
								, ExecutableParams
								, ProgramDirectory
								, [pTestState, ExecutableName](CProcessLaunchStateChangeVariant const &_StateChange, fp64 _TimeSinceLaunch)
								{
									if (_StateChange.f_GetTypeID() == EProcessLaunchState_Exited)
									{
										DMibLock(pTestState->m_Lock);

										auto &Executable = pTestState->m_Executables[ExecutableName];
										Executable.m_ExitResult = _StateChange.f_Get<EProcessLaunchState_Exited>();
									}
									else if (_StateChange.f_GetTypeID() == EProcessLaunchState_LaunchFailed)
									{
										DMibLock(pTestState->m_Lock);

										auto &Executable = pTestState->m_Executables[ExecutableName];
										Executable.m_ExitResult = 255;
										Executable.m_OutputWithErrors += "Failed to launch: {}"_f << _StateChange.f_Get<EProcessLaunchState_LaunchFailed>();
									}
								}
							)
						;

						pExecutable->m_LaunchParams.m_fOnOutput = [pTestState, ExecutableName](EProcessLaunchOutputType _OutputType, CStr const &_Output)
							{
								DMibLock(pTestState->m_Lock);
								auto &Executable = pTestState->m_Executables[ExecutableName];
								if (_OutputType == EProcessLaunchOutputType_StdOut)
									Executable.m_Output += _Output;

								Executable.m_OutputWithErrors += _Output;
							}
						;

						LaunchHandler.f_AddLaunch
							(
								pExecutable->m_LaunchParams
								, false
								, [&](CProcessLaunchParams const &_Params, EProcessLaunchCloseFlag _DestructFlags) -> TCUniquePointer<CVirtualProcessLaunch>
								{
									return fg_Construct<CVirtualProcessLaunch_Default>(_Params, _DestructFlags);
								}
							)
						;
					}

					LaunchHandler.f_BlockOnExit();

					bool bFailed = false;
					for (auto &Result : pTestState->m_Executables)
					{
						auto &ExecutableName = pTestState->m_Executables.fs_GetKey(Result);
						bool bFailedThisTest = false;
						for (mint i = 0; i < c_MaxFlakyTries; ++i)
						{
							if (Result.m_ExitResult == 0)
							{
								bFailedThisTest = false;
								break;
							}

							bool bFlaky = false;
							for (auto &ErrorString : pState->m_Settings.m_FlakyErrors)
							{
								if (Result.m_OutputWithErrors.f_Find(ErrorString) >= 0)
								{
									bFlaky = true;
									break;
								}
							}

							bFailedThisTest = true;

							if (!bFlaky)
								break;

							DMibConErrOut
								(
									" {sz*,a-}  Test suite list is flaky, rescheduling {}/{}{\n}"
									, ExecutableName
									, MaxTestLen
									, i
									, c_MaxFlakyTries
								)
							;

							Result.m_Output.f_Clear();
							Result.m_OutputWithErrors.f_Clear();
							Result.m_ExitResult = 255;
							{
								NMib::NProcess::CProcessLaunch ProcessLaunch(Result.m_LaunchParams, NMib::NProcess::EProcessLaunchCloseFlag_BlockOnExit);
							}
						}

						if (bFailedThisTest)
						{
							bFailed = true;
							DMibConErrOut
								(
									" {sz*,a-}  Failed to enumerate tests ({}, 0x{nfh,sj8,sf0}):{\n}{}{\n}"
									, ExecutableName
									, MaxTestLen
									, Result.m_ExitResult
									, Result.m_ExitResult
									, Result.m_OutputWithErrors.f_Trim()
								)
							;
						}

						for (auto &Suite : Result.m_Output.f_SplitLine<true>())
							TestSuites.f_Insert({ExecutableName, Suite.f_Split(" : ")[0]});
					}

					if (bFailed)
						return 1;
				}
				else
				{
					for (mint i = 0; i < g_nAllTests; ++i)
						TestSuites.f_Insert({g_AllTests[i], ""});
				}

				if (!_Settings.m_bList && !_Settings.m_bQuietStats && nLoops == 1)
					DMibConOut("Test suite launches {sj8,ns,}{\n}", TestSuites.f_GetLen());

				{
					mint iSuite = 0;
					for (auto &Suite : TestSuites)
					{
						auto pPrevious = PreviousRunTimes.f_FindEqual(Suite);
						SortedTestSuites.f_Insert(CSortedTestSuite{.m_pTestSuite = &Suite, .m_PreviousRunTime = pPrevious ? *pPrevious : 0.0, .m_Index = iSuite++});
					}
				}

				if (_Settings.m_SuiteOrder == "fast_first" || _Settings.m_SuiteOrder == "slow_first")
				{
					SortedTestSuites.f_Sort
						(
							[&](CSortedTestSuite const &_Left, CSortedTestSuite const &_Right) -> COrdering_Partial
							{
								if (auto Compare = _Left.m_PreviousRunTime <=> _Right.m_PreviousRunTime; Compare != 0)
									return Compare;

								return _Left.m_Index <=> _Right.m_Index;
							}
						)
					;

					if (_Settings.m_SuiteOrder == "slow_first")
						SortedTestSuites = SortedTestSuites.f_Reverse();
				}
				else if (_Settings.m_SuiteOrder == "random")
				{
					for (auto &Suite : SortedTestSuites)
						Suite.m_Index = fg_GetRandomUnsigned();

					SortedTestSuites.f_Sort
						(
							[&](CSortedTestSuite const &_Left, CSortedTestSuite const &_Right) -> COrdering_Partial
							{
								if (auto Compare = _Left.m_Index <=> _Right.m_Index; Compare != 0)
									return Compare;

								return &_Left <=> &_Right;
							}
						)
					;
				}

				if (_Settings.m_bList)
				{
					for (auto &SortedSuite : SortedTestSuites)
					{
						auto &Suite = *SortedSuite.m_pTestSuite;
						CStr Executable = Suite.m_Executable;
						DMibConOut("{sz*,a-} {}{\n}", Executable, MaxTestLen, Suite.m_Suite);
					}
					return 0;
				}

				for (auto &SortedSuite : SortedTestSuites)
				{
					auto &Suite = *SortedSuite.m_pTestSuite;
					CStr Executable = Suite.m_Executable;

					NStorage::TCSharedPointer<NTime::CStopwatch> pStopwatch = fg_Construct();
					CStr LaunchPath = NFile::CFile::fs_GetProgramDirectory() / Executable;

					TCSharedPointer<CStr> pOutput = fg_Construct();

					auto fOutputThisTest = [pOutput, MaxTestLen, _Settings, Executable, Suite](CStr const &_Description, bool _bForceOutput)
						{
							if (pOutput->f_IsEmpty() && !_bForceOutput)
								return;

							if (_Settings.m_bLaunchPerSuite)
								DMibConOut(" {sz*,a-}  {} ({}){\n}", Executable, MaxTestLen, _Description, Suite.m_Suite);
							else
								DMibConOut(" {sz*,a-}  {}{\n}", Executable, MaxTestLen, _Description);

							for (auto &Line : pOutput->f_Trim().f_SplitLine<>())
								DMibConOut(" {sz*,a-}  {}\n", "", MaxTestLen, Line);
							pOutput->f_Clear();
						}
					;

					auto TestParams = _Settings.m_TestParams;

					if (_Settings.m_bLaunchPerSuite)
						TestParams.f_Insert(Suite.m_Suite);

					TestParams.f_Insert("-g");
					TestParams.f_Insert(CStr::fs_Join(_Settings.m_TestGroups, ","));
					if (!_Settings.m_bLaunchPerSuite)
						TestParams.f_Insert(_Settings.m_TestPaths);

					auto iFlakyID = iNextFlakyID++;
					auto &FlakyState = pState->m_FlakyStateStates[iFlakyID];

					auto Params = NProcess::CProcessLaunchParams::fs_LaunchExecutable
						(
							LaunchPath
							, TestParams
							, CFile::fs_GetPath(LaunchPath)
							, [pState, iFlakyID, pExited, Executable, pStopwatch, pOutput, fOutputThisTest, Suite, MaxTestLen]
							(CProcessLaunchStateChangeVariant const &_StateChange, fp64 _TimeSinceLaunch) mutable
							{
								if (*pExited)
									return;

								if (_StateChange.f_GetTypeID() == EProcessLaunchState_Launched)
								{
									pStopwatch->f_Start();
									if (!pState->m_Settings.m_bQuiet)
									{
										if (pState->m_Settings.m_bLaunchPerSuite)
											DMibConOut(" {sz*,a-}  Launched ({}){\n}", Executable, MaxTestLen, Suite.m_Suite);
										else
											DMibConOut(" {sz*,a-}  Launched{\n}", Executable, MaxTestLen);
									}
								}
								else if (_StateChange.f_GetTypeID() == EProcessLaunchState_Exited)
								{
									--pState->m_nRunning;

									auto RunTime = pStopwatch->f_GetTime();

									pState->m_RunTimes[Suite] = RunTime;

									auto ExitCode = _StateChange.f_Get<EProcessLaunchState_Exited>();
									auto fHasFlakyError = [&]() -> bool
										{
											for (auto &ErrorString : pState->m_Settings.m_FlakyErrors)
											{
												if (pOutput->f_Find(ErrorString) >= 0)
													return true;
											}

											return false;
										}
									;

									bool bHasFlakyError = false;

									if (ExitCode != 0)
									{
										CStr Color = pState->m_AnsiEncoding.f_StatusError();
										CStr Default = pState->m_AnsiEncoding.f_Default();
										bHasFlakyError = fHasFlakyError();
										fOutputThisTest("{}Exited uncleanly with{} {} (0x{nfh,sj8,sf0})"_f << Color << Default << ExitCode << ExitCode, true);
									}
									else if (!pState->m_Settings.m_bQuiet)
									{
										CStr Color = pState->m_AnsiEncoding.f_StatusNormal();
										CStr Default = pState->m_AnsiEncoding.f_Default();

										fOutputThisTest("{}{fe1} s{}   {}/{} done"_f << Color << RunTime << Default << pState->m_nDone << pState->m_nTotalLaunches, true);
									}

									auto pFlakyState = pState->m_FlakyStateStates.f_FindEqual(iFlakyID);

									if
										(
											pState->m_Settings.m_bLaunchPerSuite
											&& ExitCode != 0
											&&
											(
												fg_StrMatchesAnyWildcardInContainer(Suite.m_Suite, pState->m_Settings.m_FlakySuites)
												|| bHasFlakyError
											)
											&& pFlakyState
											&& pFlakyState->m_nTries < c_MaxFlakyTries
										)
									{
										fOutputThisTest("Test suite is flaky, rescheduling {}/{}"_f << pFlakyState->m_nTries << c_MaxFlakyTries, true);
										++pFlakyState->m_nTries;
										pState->m_NotLaunched.f_Insert(pFlakyState->m_Params);
									}
									else
									{
										if (ExitCode != 0)
										{
											if (pFlakyState && pFlakyState->m_nTries > 1)
												fOutputThisTest("Flaky test suite failed on all {} tries"_f << c_MaxFlakyTries, true);

											++pState->m_nFailed;
										}
										else
										{
											if (pFlakyState && pFlakyState->m_nTries > 1)
											{
												if (pState->m_Settings.m_bQuiet)
													pOutput->f_Clear();
												fOutputThisTest("Flaky test suite succeeded after {}/{} tries"_f << pFlakyState->m_nTries << c_MaxFlakyTries, true);
											}
										}

										pState->m_FlakyStateStates.f_Remove(iFlakyID);

										++pState->m_nDone;
										pState->m_CombinedExitCode = fg_Max(pState->m_CombinedExitCode, ExitCode);
									}

									if (pState->m_fAddLaunches)
										pState->m_fAddLaunches();

									return;
								}
								else if (_StateChange.f_GetTypeID() == EProcessLaunchState_LaunchFailed)
								{
									--pState->m_nRunning;
									++pState->m_nDone;
									++pState->m_nFailed;
									pState->m_CombinedExitCode = fg_Max(pState->m_CombinedExitCode, uint32(254));
									CStr Color = pState->m_AnsiEncoding.f_StatusError();
									CStr Default = pState->m_AnsiEncoding.f_Default();

									fOutputThisTest("{}Failed to launch{}: {}"_f << Color << Default << _StateChange.f_Get<EProcessLaunchState_LaunchFailed>(), true);
								}
								if (pState->m_fAddLaunches)
									pState->m_fAddLaunches();
							}
							, [pExited, &ToDispatch, &DispatchEvent](NFunction::TCFunction<void ()> const &_Functor)
							{
								if (*pExited)
									return;
								ToDispatch.f_Push(_Functor);
								DispatchEvent.f_Signal();
							}
						)
					;

					Params.m_bCreateNewProcessGroup = true;

					OutputDeferredOutput.f_Insert() = fOutputThisTest;

					Params.m_fOnOutput = [&, pOutput, pStopwatch](EProcessLaunchOutputType _OutputType, CStr const &_Output)
						{
							*pOutput += _Output;
						}
					;

					fModifyEnvironment(Params, Executable);

					FlakyState.m_Params = Params;
					pState->m_NotLaunched.f_Insert(fg_Move(Params));
				}
				pState->m_nTotalLaunches = pState->m_NotLaunched.f_GetLen();
				while (fAddLaunches())
					;

				{
					while (!pState->m_NotLaunched.f_IsEmpty() || pState->m_nRunning > 0)
					{
						while (auto Entry = ToDispatch.f_Pop())
							(*Entry)();

						if (!pState->m_NotLaunched.f_IsEmpty() || pState->m_nRunning > 0)
							DispatchEvent.f_WaitTimeout(bShouldOutput ? 0.05 : 1.0);

						if (bShouldOutput && (SignalStopwatch.f_GetTime() - LastSignal > 0.25))
						{
							bShouldOutput = false;
							for (auto &fOutput : OutputDeferredOutput)
								fOutput("Intermediate output", false);
						}

						if (bSignalled)
						{
							bSignalled = false;

							if ((LastSignal && (SignalStopwatch.f_GetTime() - LastSignal < 0.25)) || bRunningCI)
							{
								pState->m_CombinedExitCode = fg_Max(pState->m_CombinedExitCode, uint32(255));
								bCancelled = true;
								break;
							}
							else
								bShouldOutput = true;

							LastSignal = SignalStopwatch.f_GetTime();
						}

						if (_Settings.m_Timeout != fp64::fs_Inf() && TimeoutStopwatch.f_GetTime() > _Settings.m_Timeout)
						{
							pState->m_CombinedExitCode = fg_Max(pState->m_CombinedExitCode, uint32(255));
							DMibConOut("Timed out - aborting remaining tests{\n}");
							bCancelled = true;
							break;
						}
					}
				}
				while (auto Entry = ToDispatch.f_Pop())
					(*Entry)();

				LaunchHandler.f_StopAll();
				LaunchHandler.f_BlockOnExit(1.0, 0, &MemoryStats);
				LaunchHandler.f_TerminateAll(true, &MemoryStats);
			}
		}

		if (!_Settings.m_bQuietStats)
		{
			struct CSuiteMemory
			{
				auto operator <=> (CSuiteMemory const &_Right) const
				{
					return _Right.m_Memory <=> m_Memory;
				}

				CStr m_Name;
				fp64 m_Memory = 0.0;
			};
			TCVector<CSuiteMemory> AllUsages;

			mint iStat = 0;
			for (auto &Statistics : MemoryStats)
			{
				fp64 TotalMemoryUsage = 0.0;
				auto fGetValue = [&](ch8 const *_pName) -> fp64
					{
						auto *pValue = Statistics.m_Statistics.f_FindEqual(_pName);
						if (pValue)
							return pValue->m_Value / (1024.0 * 1024.0);
						return 0.0;
					}
				;
				TotalMemoryUsage += fg_Max(fGetValue("Peak page file usage"), fGetValue("Peak working set size"), fGetValue("Max resident size"));
				TotalMemoryUsage += fGetValue("Peak non paged pool usage");
				TotalMemoryUsage += fGetValue("Peak paged pool usage");
				auto &Usage = AllUsages.f_Insert();

				Usage.m_Memory = TotalMemoryUsage;
				if (SortedTestSuites.f_IsPosValid(iStat))
					Usage.m_Name = SortedTestSuites[iStat].m_pTestSuite->m_Suite;

				++iStat;
			}
			AllUsages.f_Sort();
			if (!AllUsages.f_IsEmpty())
			{
				mint nUsages = 0;
				fp64 WorstCaseUsage = 0;
				fp64 WorstCaseUsage2Core = 0;
				for (auto &Usage : AllUsages)
				{
					WorstCaseUsage += Usage.m_Memory;
					if (nUsages < 2)
						WorstCaseUsage2Core += Usage.m_Memory;

					++nUsages;
					if (nUsages >= nThreads)
						break;
				}

				DMibConOut("Max memory          {sj8,ns,} MiB{\n}", AllUsages.f_GetFirst().m_Memory.f_ToInt());
				DMibConOut("Worst case usage    {sj8,ns,} MiB{\n}", WorstCaseUsage.f_ToInt());
				DMibConOut("Worst case usage 2c {sj8,ns,} MiB{\n}", WorstCaseUsage2Core.f_ToInt());
				DMibConOut("Suggested memory    {sj8,ns,} MiB{\n}", ((WorstCaseUsage * 1.5) / fp64 (fg_Min(nThreads, AllUsages.f_GetLen()))).f_ToInt());
				DMibConOut("Suggested memory 2c {sj8,ns,} MiB{\n}", ((WorstCaseUsage2Core * 1.5) / fg_Min(2u, AllUsages.f_GetLen())).f_ToInt());
				DMibConOut("Top ten{\n}");

				mint nLogged = 0;
				for (auto &Usage : AllUsages)
				{
					DMibConOut("{sj8,ns,} MiB   {}{\n}", Usage.m_Memory.f_ToInt(), Usage.m_Name);
					if (++nLogged >= 10)
						break;
				}
			}
		}

		if (pState->m_nFailed)
		{
			CStr Color = _AnsiEncoding.f_StatusError();
			CStr Default = _AnsiEncoding.f_Default();
			if (_Settings.m_bLaunchPerSuite)
				DMibConErrOut("{}{} out of {} suites failed{}\n", Color, pState->m_nFailed, pState->m_nTotalLaunches, Default);
			else
				DMibConErrOut("{}{} out of {} executables failed{}\n", Color, pState->m_nFailed, pState->m_nTotalLaunches, Default);
		}
		else if (_Settings.m_bReportSuccess)
		{
			CStr Color = _AnsiEncoding.f_StatusNormal();
			CStr Default = _AnsiEncoding.f_Default();

			if (_Settings.m_bLaunchPerSuite)
				DMibConErrOut("{}All {} suites were successful{}\n", Color, pState->m_nTotalLaunches, Default);
			else
				DMibConErrOut("{}All {} launches were successful{}\n", Color, pState->m_nTotalLaunches, Default);
		}

		{
			CEJsonSorted OutputJson = EJsonType_Object;
			for (auto &RuntimeEntry : pState->m_RunTimes.f_Entries())
				OutputJson[RuntimeEntry.f_Key().m_Executable][RuntimeEntry.f_Key().m_Suite] = RuntimeEntry.f_Value();

			CFile::fs_CreateDirectoryForFile(RuntimesPath);
			CFile::fs_WriteStringToFile(RuntimesPath, OutputJson.f_ToString(), false);
		}

		return pState->m_CombinedExitCode;
	}

	aint fp_RunTests(NEncoding::CEJsonSorted const &_Parameters, CAnsiEncoding const &_AnsiEncoding, bool _bList)
	{
		CSettings Settings(_Parameters, _bList);

		uint32 Result = 0;
#if DMalterlibCodeCoverage
		if (!Settings.m_bCoverageOnly)
		{
			if (Settings.m_bCoverage && CFile::fs_FileExists(Settings.m_CoverageDirectory))
				CFile::fs_DeleteDirectoryRecursive(Settings.m_CoverageDirectory);
			Result = fp_ExecuteTests(Settings, _AnsiEncoding);
		}
		if (Settings.m_bCoverage)
			fp_DisplayCoverage(Settings);
#else
		Result = fp_ExecuteTests(Settings, _AnsiEncoding);
#endif
		return Result;
	}
};

DMibAppImplement(CRunAllTestsApplication);
