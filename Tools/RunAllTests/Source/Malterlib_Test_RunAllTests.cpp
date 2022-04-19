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
	aint f_Main()
	{
		NStorage::TCSharedPointer<NMib::NCommandLine::CCommandLineSpecification> pCommandLineSpec = fg_Construct();
		pCommandLineSpec->f_AddHelpCommand();
		pCommandLineSpec->f_AddTerminalOptions();

		auto Section = pCommandLineSpec->f_AddSection("Test", "Run tests");
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
#if DMibPPtrBits <= 32
							, "Default"_= false
#else
							, "Default"_= true
#endif

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
						, "LaunchPerSuite?"_=
						{
							"Names"_= {"--launch-per-suite"}
							, "Default"_= false
							, "Description"_= "Launch the executable per suite.\n"
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
			, m_bParallel(_Parameters["Parallel"].f_Boolean())
			, m_bLoopTests(_Parameters["Loop"].f_Boolean())
			, m_bQuiet(_Parameters["Quiet"].f_Boolean())
			, m_bLaunchPerSuite(_Parameters["LaunchPerSuite"].f_Boolean())
#if DMalterlibCodeCoverage
			, m_bCoverage(_Parameters["Coverage"].f_Boolean())
			, m_bCoverageOnly(_Parameters["CoverageOnly"].f_Boolean())
			, m_CoverageExecutable(_Parameters["CoverageExecutable"].f_String())
			, m_CoverageSources(_Parameters["CoverageSources"].f_StringArray())
#endif
		{
		}

		TCVector<CStr> m_TestParams;
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

		uint32 CombinedExitCode = 0;
		mint nFailed = 0;
		mint nTotalLaunches = 0;

		while (true)
		{
			TCThreadSafeQueue<TCFunction<void ()>> ToDispatch;
			NThread::CEventAutoReset DispatchEvent;
			TCLinkedList<CProcessLaunchParams> NotLaunched;
			mint nRunning = 0;
			mint nDone = 0;
			nFailed = 0;
			nTotalLaunches = 0;
			mint nMaxRunning = _Settings.m_bParallel ? NSys::fg_Thread_GetVirtualCores() : 1;

			{
				CProcessLaunchHandler LaunchHandler;
				auto fAddLaunches = [&]
					{
						if (NotLaunched.f_IsEmpty() || nRunning >= nMaxRunning)
							return false;
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
				TCVector<TCFunction<void ()>> OutputDeferredOutput;
				mint MaxTestLen = 0;
				for (mint i = 0; i < g_nAllTests; ++i)
				{
					CStr Test = g_AllTests[i];
					MaxTestLen = fg_Max(MaxTestLen, (mint)Test.f_GetLen());
				}

				TCSharedPointer<bool> pExited = fg_Construct(false);
				auto CleanupExited = g_OnScopeExit / [&]
					{
						*pExited = true;
					}
				;

				struct CTestSuite
				{
					CStr m_Executable;
					CStr m_Suite;
				};

				TCVector<CTestSuite> TestSuites;

				for (mint i = 0; i < g_nAllTests; ++i)
				{
					CStr Executable = g_AllTests[i];

					if (_Settings.m_bLaunchPerSuite)
					{
						auto Suites = CProcessLaunch::fs_LaunchTool(Executable, {"-l"});
						for (auto &Suite : Suites.f_SplitLine<true>())
							TestSuites.f_Insert({Executable, Suite});
					}
					else
						TestSuites.f_Insert({Executable, ""});
				}

				for (auto &Suite : TestSuites)
				{
					CStr Executable = Suite.m_Executable;

					NStorage::TCSharedPointer<NTime::CClock> pClock = fg_Construct();
					CStr LaunchPath = NFile::CFile::fs_GetProgramDirectory() / Executable;

					TCSharedPointer<CStr> pOutput = fg_Construct();

					auto fOutputThisTest = [pOutput, MaxTestLen]
						{
							for (auto &Line : pOutput->f_Trim().f_SplitLine())
								DMibConOut2(" {sz*,a-}  {}\n", "", MaxTestLen, Line);
							pOutput->f_Clear();
						}
					;

					auto TestParams = _Settings.m_TestParams;

					if (_Settings.m_bLaunchPerSuite)
						TestParams.f_Insert(Suite.m_Suite);

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
											DMibConOut2(" {sz*,a-}  Launched: {}{\n}", Executable, MaxTestLen, Suite.m_Suite);
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
										if (_Settings.m_bLaunchPerSuite)
											DMibConOut2(" {sz*,a-}  {}Exited uncleanly with{} {} ({}){\n}", Executable, MaxTestLen, Color, Default, ExitCode, Suite.m_Suite);
										else
											DMibConOut2(" {sz*,a-}  {}Exited uncleanly with{} {}{\n}", Executable, MaxTestLen, Color, Default, ExitCode);
										fOutputThisTest();
									}
									else if (!_Settings.m_bQuiet)
									{
										CStr Color = _AnsiEncoding.f_StatusNormal();
										CStr Default = _AnsiEncoding.f_Default();

										if (_Settings.m_bLaunchPerSuite)
											DMibConOut2(" {sz*,a-}  {}{fe1} s{}   {}/{} done ({}){\n}", Executable, MaxTestLen, Color, pClock->f_GetTime(), Default, (nDone), nTotalLaunches, Suite.m_Suite);
										else
											DMibConOut2(" {sz*,a-}  {}{fe1} s{}   {}/{} done{\n}", Executable, MaxTestLen, Color, pClock->f_GetTime(), Default, (nDone), nTotalLaunches);
										fOutputThisTest();
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
									if (_Settings.m_bLaunchPerSuite)
										DMibConOut2(" {sz*,a-}  {}Failed to launch{} ({}): {}{\n}", Executable, MaxTestLen, Color, Default, Suite.m_Suite, _StateChange.f_Get<EProcessLaunchState_LaunchFailed>());
									else
										DMibConOut2(" {sz*,a-}  {}Failed to launch{}: {}{\n}", Executable, MaxTestLen, Color, Default, _StateChange.f_Get<EProcessLaunchState_LaunchFailed>());
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
					bool bSignalled = false;
					CClock SigtalClock;
					fp64 LastSignal = 0.0;
					SigtalClock.f_Start();

					auto Cleaunup = NProcess::NPlatform::fg_Process_WaitForTermination
						(
							[&]
							{
								bSignalled = true;
								DispatchEvent.f_Signal();
							}
						)
					;

					while (!NotLaunched.f_IsEmpty() || nRunning > 0)
					{
						while (auto Entry = ToDispatch.f_Pop())
							(*Entry)();

						if (!NotLaunched.f_IsEmpty() || nRunning > 0)
							DispatchEvent.f_WaitTimeout(1.0);

						if (bSignalled)
						{
							bSignalled = false;

							for (auto &fOutput : OutputDeferredOutput)
								fOutput();

							if (SigtalClock.f_GetTime() - LastSignal < 1.0)
							{
								DConOut2("\nTerminating launched tests\n");
								CombinedExitCode = fg_Max(CombinedExitCode, uint32(255));
								break;
							}
							LastSignal = SigtalClock.f_GetTime();
						}
					}
				}
				while (auto Entry = ToDispatch.f_Pop())
					(*Entry)();

				LaunchHandler.f_StopAll();
				LaunchHandler.f_BlockOnExit(1.0);
				LaunchHandler.f_TerminateAll(true);
			}
			if (!_Settings.m_bLoopTests)
				break;
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
};

DMibAppImplement(CRunAllTestsApplication);
