// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/Application>
#include <Mib/Process/ProcessLaunch>
#include <Mib/Process/VirtualProcessLaunch>
#include <Mib/Concurrency/ThreadSafeQueue>
#include <Mib/Cryptography/UUID>
#include <Mib/CommandLine/CommandLineClient>
#include <Mib/Encoding/JSONShortcuts>

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
	CRunAllTestsApplication()
	{
#		if DMibPPtrBits <= 32
			mp_bParallelDefault = false;
#		else
			mp_bParallelDefault = true;
#			if defined(DMibSanitizerEnabled)
				if (NProcess::NPlatform::fg_Process_GetPhysicalMemory() < 24_uint64 * 1024 * 1024 * 1024)
					mp_bParallelDefault = false;
#			endif
#		endif
	}

	aint f_Main()
	{
		NStorage::TCSharedPointer<NMib::NCommandLine::CCommandLineSpecification> pCommandLineSpec = fg_Construct();
		pCommandLineSpec->f_AddHelpCommand();
		pCommandLineSpec->f_AddTerminalOptions();

		auto Section = pCommandLineSpec->f_AddSection("Test", "Run tests");
		auto GroupsList = NCommandLine::COneOf{"Default", "Performance", "Torture", "Memory", "Unfinished", "Expensive", "Manual", "SuperUser"};
		auto RunAllTestsCommand = Section.f_RegisterDirectCommand
			(
				{
					"Names"_= {"--run-all-tests"}
					, "Description"_= "List test suites contained in this binary.\n"
					, "Options"_=
					{
						"Parallel?"_=
						{
							"Names"_= {"--parallel", "-p"}
							, "Default"_= mp_bParallelDefault
							, "Description"_= "Run tests in paralell utilizing all cores.\n"
						}
						, "Quiet?"_=
						{
							"Names"_= {"--quiet"}
							, "Default"_= true
							, "Description"_= "Don't output test results unless a failure occurs.\n"
						}
						, "Loop?"_=
						{
							"Names"_= {"--loop"}
							, "Default"_= false
							, "Description"_= "Loop tests until aborted.\n"
						}
						, "Timeout?"_=
						{
							"Names"_= {"--timeout"}
							, "Default"_= fp64::fs_Inf()
							, "Description"_= "Stop tests after this timeout.\n"
						}
						, "LoopIterations?"_=
						{
							"Names"_= {"--iterations", "-i"}
							, "Default"_= 0
							, "Description"_= "Abort loop after iterations.\n"
						}
						, "LoopAbortOnFailure?"_=
						{
							"Names"_= {"--abort-on-failure", "-a"}
							, "Default"_= false
							, "Description"_= "Abort loop after first failure.\n"
						}
						, "LaunchPerSuite?"_=
						{
							"Names"_= {"--launch-per-suite"}
							, "Default"_= true
							, "Description"_= "Launch the executable per suite.\n"
						}
						, "Groups?"_=
						{
							"Names"_= {"--groups", "-g"}
							, "Default"_= {"Default"}
							, "Type"_= {GroupsList}
							, "Description"_= "Specify the groups to include in test.\n"
							"@Indent=17\r"
							"   Default:      Run tests without a group specified.\r"
							"   Performance:  Run tests with Performance group specified.\r"
							"   Torture:      Run tests with Torgutre group specified.\r"
							"   Memory:       Run tests with Performance group specified.\r"
							"   Unfinished:   Run tests with Unfinished group specified.\r"
							"   Expensive:    Run tests with Expensive group specified.\r"
							"   Manual:       Run tests with Manual group specified.\r"
							"   SuperUser:    Run tests with SuperUser group specified.\r"
							"\r"
						}
#if DMalterlibCodeCoverage
 						, "Coverage?"_=
						{
							"Names"_= {"--coverage"}
							, "Default"_= true
							, "Description"_= "Record code coverage and report results.\n"
						}
 						, "CoverageOnly?"_=
						{
							"Names"_= {"--coverage-only"}
							, "Default"_= false
							, "Description"_= "Only display coverage results from previous run, don't run tests.\n"
						}
 						, "CoverageSources?"_=
						{
							"Names"_= {"--coverage-sources"}
							, "Default"_= _[_]
							, "Type"_= {""}
							, "Description"_= "Only display coverage for these source files. Can use wildcarcds.\n"
						}
						, "CoverageExecutable?"_=
						{
							"Names"_= {"--coverage-executable"}
							, "Default"_= ""
							, "Description"_= "Specify the executable used to display code coverage results\n"
						}
#endif
					}
					, "Parameters"_=
					{
						"TestParams...?"_=
						{
							"Type"_= {""}
							, "Default"_= _[_]
							, "Description"_= "The parameters to forward to the individual tests."
						}
					}
					, "ErrorOnCommandAsParameter"_= false
					, "ErrorOnOptionAsParameter"_= false
					, "GreedyDefaultCommandParameters"_= true
				}
				, [this](NEncoding::CEJSON const &_Parameters, NCommandLine::CCommandLineClient &_CommandLineClient)
				{
					return fp_RunTests(_Parameters, _CommandLineClient.f_AnsiEncoding());
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
		CSettings(NEncoding::CEJSON const &_Parameters)
			: m_TestParams(_Parameters["TestParams"].f_StringArray())
			, m_TestGroups(_Parameters["Groups"].f_StringArray())
			, m_bParallel(_Parameters["Parallel"].f_Boolean())
			, m_bLoopTests(_Parameters["Loop"].f_Boolean())
			, m_bQuiet(_Parameters["Quiet"].f_Boolean())
			, m_bLaunchPerSuite(_Parameters["LaunchPerSuite"].f_Boolean())
			, m_bAbortOnFailure(_Parameters["LoopAbortOnFailure"].f_Boolean())
			, m_nLoops(_Parameters["LoopIterations"].f_Integer())
			, m_Timeout(_Parameters["Timeout"].f_Float())
#if DMalterlibCodeCoverage
			, m_bCoverage(_Parameters["Coverage"].f_Boolean())
			, m_bCoverageOnly(_Parameters["CoverageOnly"].f_Boolean())
			, m_CoverageExecutable(_Parameters["CoverageExecutable"].f_String())
			, m_CoverageSources(_Parameters["CoverageSources"].f_StringArray())
#endif
		{
			if (!m_bLoopTests)
				m_nLoops = 1;
		}

		TCVector<CStr> m_TestParams;
		TCVector<CStr> m_TestGroups;

		int64 m_nLoops = 0;
		fp64 m_Timeout = fp64::fs_Inf();
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
		bool m_bLaunchPerSuite = false;
		bool m_bAbortOnFailure = false;
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
							DConOut2("{}{\n}", Line);
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
							DConOut2("{}{\n}", Line);
							continue;
						}

						if (File == "TOTAL")
							continue;

						for (auto &Source : _Settings.m_CoverageSources)
						{
							if (fg_StrMatchWildcard(Line.f_GetStr(), Source.f_GetStr()) == EMatchWildcardResult_WholeStringMatchedAndPatternExhausted)
							{
								DConOut2("{}{\n}", Line);
								TotalRegions += Regions;
								TotalMissedRegions += MissedRegions;
								//DConOut2("{} {} {} {\n}", Total, Uncovered, Line);
								break;
							}
						}
					}

					DConOut2
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

		uint32 CombinedExitCode = 0;
		mint nFailed = 0;
		mint nTotalLaunches = 0;
		bool bCancelled = false;
		bool bSignalled = false;
		bool bShouldOutput = false;
		CClock SignalClock;
		CClock TimeoutClock;
		fp64 LastSignal = 0.0;
		SignalClock.f_Start();
		TimeoutClock.f_Start();
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

		while (!bCancelled && (!_Settings.m_nLoops || (nLoops < _Settings.m_nLoops)) && (!_Settings.m_bAbortOnFailure || !nFailed))
		{
			++nLoops;
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

			TCLinkedList<CProcessLaunchParams> NotLaunched;
			mint nRunning = 0;
			mint nDone = 0;
			nFailed = 0;
			nTotalLaunches = 0;
			mint nMaxRunning = _Settings.m_bParallel ? NSys::fg_Thread_GetVirtualCores() : 1;

			{
				auto fAddLaunches = [&]
					{
						if (bCancelled || NotLaunched.f_IsEmpty() || nRunning >= nMaxRunning)
							return false;

						if (_Settings.m_bAbortOnFailure && nFailed)
						{
							NotLaunched.f_Clear();
							return false;
						}

						auto Params = NotLaunched.f_Pop();

						++nRunning;
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
				TCVector<TCFunction<void (CStr const &_Description, bool _bForceOutput)>> OutputDeferredOutput;
				mint MaxTestLen = 0;
				for (mint i = 0; i < g_nAllTests; ++i)
				{
					CStr Test = g_AllTests[i];
					MaxTestLen = fg_Max(MaxTestLen, (mint)Test.f_GetLen());
				}

				struct CTestSuite
				{
					CStr m_Executable;
					CStr m_Suite;
				};

				TCVector<CTestSuite> TestSuites;

				CClock PerfClock{true};
				if (_Settings.m_bLaunchPerSuite)
				{
					CProcessLaunchHandler LaunchHandler;

					struct CExecutableResult
					{
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

					CStr ProgramDirectory = NFile::CFile::fs_GetProgramDirectory();

					for (mint i = 0; i < g_nAllTests; ++i)
					{
						CStr ExecutableName = g_AllTests[i];

						auto Params = NProcess::CProcessLaunchParams::fs_LaunchExecutable
							(
								ProgramDirectory / ExecutableName
								, {"-l", "-g", CStr::fs_Join(_Settings.m_TestGroups, ",")}
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

						Params.m_fOnOutput = [pTestState, ExecutableName](EProcessLaunchOutputType _OutputType, CStr const &_Output)
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
								Params
								, false
								, [&](CProcessLaunchParams const &_Params, EProcessLaunchCloseFlag _DestructFlags) -> TCUniquePointer<CVirtualProcessLaunch>
								{
									return fg_Construct<CVirtualProcessLaunch_Default>(_Params, _DestructFlags);
								}
							)
						;

						if (!mp_bParallelDefault)
							LaunchHandler.f_BlockOnExit();
					}

					LaunchHandler.f_BlockOnExit();

					bool bFailed = false;
					for (auto &Result : pTestState->m_Executables)
					{
						auto &ExecutableName = pTestState->m_Executables.fs_GetKey(Result);
						if (Result.m_ExitResult != 0)
						{
							DMibConOut2(" {sz*,a-}  Failed enumerate tests:{\n}{}{\n}n", ExecutableName, MaxTestLen, Result.m_OutputWithErrors.f_Trim());
							bFailed = true;
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

				for (auto &Suite : TestSuites)
				{
					CStr Executable = Suite.m_Executable;

					NStorage::TCSharedPointer<NTime::CClock> pClock = fg_Construct();
					CStr LaunchPath = NFile::CFile::fs_GetProgramDirectory() / Executable;

					TCSharedPointer<CStr> pOutput = fg_Construct();

					auto fOutputThisTest = [pOutput, MaxTestLen, _Settings, Executable, Suite](CStr const &_Description, bool _bForceOutput)
						{
							if (pOutput->f_IsEmpty() && !_bForceOutput)
								return;

							if (_Settings.m_bLaunchPerSuite)
								DMibConOut2(" {sz*,a-}  {} ({}){\n}", Executable, MaxTestLen, _Description, Suite.m_Suite);
							else
								DMibConOut2(" {sz*,a-}  {}{\n}", Executable, MaxTestLen, _Description);

							for (auto &Line : pOutput->f_Trim().f_SplitLine<>())
								DMibConOut2(" {sz*,a-}  {}\n", "", MaxTestLen, Line);
							pOutput->f_Clear();
						}
					;

					auto TestParams = _Settings.m_TestParams;

					if (_Settings.m_bLaunchPerSuite)
						TestParams.f_Insert(Suite.m_Suite);

					TestParams.f_Insert("-g");
					TestParams.f_Insert(CStr::fs_Join(_Settings.m_TestGroups, ","));

					auto Params = NProcess::CProcessLaunchParams::fs_LaunchExecutable
						(
							LaunchPath
							, TestParams
							, CFile::fs_GetPath(LaunchPath)
							, [&, pExited, Executable, pClock, pOutput, fOutputThisTest, Suite](CProcessLaunchStateChangeVariant const &_StateChange, fp64 _TimeSinceLaunch)
							{
								if (*pExited)
									return;
								if (_StateChange.f_GetTypeID() == EProcessLaunchState_Launched)
								{
									if (!_Settings.m_bQuiet)
									{
										if (_Settings.m_bLaunchPerSuite)
											DMibConOut2(" {sz*,a-}  Launched ({}){\n}", Executable, MaxTestLen, Suite.m_Suite);
										else
											DMibConOut2(" {sz*,a-}  Launched{\n}", Executable, MaxTestLen);
									}
									pClock->f_Start();
								}
								else if (_StateChange.f_GetTypeID() == EProcessLaunchState_Exited)
								{
									--nRunning;
									++nDone;

									auto ExitCode = _StateChange.f_Get<EProcessLaunchState_Exited>();
									CombinedExitCode = fg_Max(CombinedExitCode, ExitCode);
									if (ExitCode != 0)
									{
										++nFailed;
										CStr Color = _AnsiEncoding.f_StatusError();
										CStr Default = _AnsiEncoding.f_Default();
										fOutputThisTest("{}Exited uncleanly with{} {}"_f << Color << Default << ExitCode, true);
									}
									else if (!_Settings.m_bQuiet)
									{
										CStr Color = _AnsiEncoding.f_StatusNormal();
										CStr Default = _AnsiEncoding.f_Default();

										fOutputThisTest("{}{fe1} s{}   {}/{} done"_f << Color << pClock->f_GetTime() << Default << nDone << nTotalLaunches, true);
									}

									fAddLaunches();

									return;
								}
								else if (_StateChange.f_GetTypeID() == EProcessLaunchState_LaunchFailed)
								{
									--nRunning;
									++nDone;
									++nFailed;
									CombinedExitCode = fg_Max(CombinedExitCode, uint32(254));
									CStr Color = _AnsiEncoding.f_StatusError();
									CStr Default = _AnsiEncoding.f_Default();

									fOutputThisTest("{}Failed to launch{}: {}"_f << Color << Default << _StateChange.f_Get<EProcessLaunchState_LaunchFailed>(), true);
								}
								fAddLaunches();
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

					Params.m_fOnOutput = [&, pOutput, pClock](EProcessLaunchOutputType _OutputType, CStr const &_Output)
						{
							*pOutput += _Output;
						}
					;

					fModifyEnvironment(Params, Executable);

					NotLaunched.f_Insert(fg_Move(Params));
				}
				nTotalLaunches = NotLaunched.f_GetLen();
				while (fAddLaunches())
					;

				{
					while (!NotLaunched.f_IsEmpty() || nRunning > 0)
					{
						while (auto Entry = ToDispatch.f_Pop())
							(*Entry)();

						if (!NotLaunched.f_IsEmpty() || nRunning > 0)
							DispatchEvent.f_WaitTimeout(bShouldOutput ? 0.05 : 1.0);

						if (bShouldOutput && (SignalClock.f_GetTime() - LastSignal > 0.25))
						{
							bShouldOutput = false;
							for (auto &fOutput : OutputDeferredOutput)
								fOutput("Intermediate output", false);
						}

						if (bSignalled)
						{
							bSignalled = false;

							if ((LastSignal && (SignalClock.f_GetTime() - LastSignal < 0.25)) || bRunningCI)
							{
								CombinedExitCode = fg_Max(CombinedExitCode, uint32(255));
								bCancelled = true;
								break;
							}
							else
								bShouldOutput = true;

							LastSignal = SignalClock.f_GetTime();
						}

						if (_Settings.m_Timeout != fp64::fs_Inf() && TimeoutClock.f_GetTime() > _Settings.m_Timeout)
						{
							CombinedExitCode = fg_Max(CombinedExitCode, uint32(255));
							bCancelled = true;
							break;
						}
					}
				}
				while (auto Entry = ToDispatch.f_Pop())
					(*Entry)();

				LaunchHandler.f_StopAll();
				LaunchHandler.f_BlockOnExit(1.0);
				LaunchHandler.f_TerminateAll(true);
			}
		}

		if (nFailed)
		{
			CStr Color = _AnsiEncoding.f_StatusError();
			CStr Default = _AnsiEncoding.f_Default();
			if (_Settings.m_bLaunchPerSuite)
				DMibConErrOut2("{}{} ouf of {} suites failed{}\n", Color, nFailed, nTotalLaunches, Default);
			else
				DMibConErrOut2("{}{} ouf of {} executables failed{}\n", Color, nFailed, nTotalLaunches, Default);
		}

		return CombinedExitCode;
	}

	aint fp_RunTests(NEncoding::CEJSON const &_Parameters, CAnsiEncoding const &_AnsiEncoding)
	{
		CSettings Settings(_Parameters);

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

private:
	bool mp_bParallelDefault = false;
};

DMibAppImplement(CRunAllTestsApplication);
