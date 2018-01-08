// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/Application>
#include <Mib/Process/ProcessLaunch>
#include <Mib/Process/VirtualProcessLaunch>
#include <Mib/Concurrency/ThreadSafeQueue>

using namespace NMib;
using namespace NMib::NProcess;
using namespace NMib::NFunction;
using namespace NMib::NStr;

extern ch8 const *g_AllTests[];
extern mint g_nAllTests;


class CRunAllTestsApplication : public NMib::CApplication
{
public:
	CRunAllTestsApplication()
	{
		
	}
	aint f_Main()
	{
		NContainer::TCVector<CStr> CommandLine;
		NContainer::TCVector<CStr> NewCommandLine;
		NSys::fg_Process_GetCommandLineArgs(CommandLine);
		
		bool bSingleCore = false;
		bool bLoopTests = false;
		bool bQuiet = false;
		
		for (auto iArg = CommandLine.f_GetIterator(); iArg; ++iArg)
		{
			if (*iArg == "--JustExit")
			{
				++iArg;
				if (iArg)
				{
					uint8 ExitCode = iArg->f_ToInt(uint8(0));
					return ExitCode;
				}
				else
					return 0;
			}
			if (*iArg == "--single-core")
			{
				bSingleCore = true;
				continue;
			}
			if (*iArg == "--quiet")
			{
				bQuiet = true;
				continue;
			}
			if (*iArg == "--loop-tests")
			{
				bLoopTests = true;
				continue;
			}
			if (*iArg == "--StdOutExit")
			{
				DMibConOut("Footer\n", 0);
				++iArg;
				if (iArg)
				{
					uint8 ExitCode = iArg->f_ToInt(uint8(0));
					return ExitCode;
				}
				else
					return 0;
			}
			NewCommandLine.f_Insert(*iArg);
		}
		
		NewCommandLine.f_Remove(0);

		uint32 CombinedExitCode = 0;

		while (true)
		{
			if (bSingleCore)
			{
				for (mint i = 0; i < g_nAllTests; ++i)
				{
					CStr Test = g_AllTests[i];

					if (!bQuiet)
						DMibConOut("Running: {}{\n}", Test);

					uint32 ExitCode;
					NProcess::CProcessLaunch::fs_LaunchBlock
						(
							NFile::CFile::fs_AppendPath(NFile::CFile::fs_GetProgramDirectory(), Test)
							, NewCommandLine
							, [&](CStr const &_Output)
							{
								DMibConOutRaw(_Output);
							}
							, [&](CStr const &_Output)
							{
								DMibConErrOutRaw(_Output);
							}
							, ExitCode
						)
					;
				
					CombinedExitCode = fg_Max(CombinedExitCode, fg_Min(ExitCode, 254u));
				}
			}
			else
			{
				TCThreadSafeQueue<TCFunction<void ()>> ToDispatch;
				NThread::CEventAutoReset DispatchEvent;
				TCLinkedList<CProcessLaunchParams> NotLaunched;
				mint nRunning = 0;
				mint nDone = 0;
				mint nTotalLaunches = 0;
				mint nMaxRunning = NSys::fg_Thread_GetVirtualCores();

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
					auto CleanupExited = g_OnScopeExit > [&]
						{
							*pExited = true;
						}
					;

					for (mint i = 0; i < g_nAllTests; ++i)
					{
						CStr Test = g_AllTests[i];
					
						NPtr::TCSharedPointer<NTime::CClock> pClock = fg_Construct();
						CStr LaunchPath = NFile::CFile::fs_AppendPath(NFile::CFile::fs_GetProgramDirectory(), Test);

						TCSharedPointer<CStr> pOutput = fg_Construct();

						auto fOutputThisTest = [pOutput, Test, MaxTestLen]
							{
								for (auto &Line : pOutput->f_Trim().f_SplitLine())
									DMibConOut2(" {sz*,a-}  {}\n", "", MaxTestLen, Line);
								pOutput->f_Clear();
							}
						;

						auto Params = NProcess::CProcessLaunchParams::fs_LaunchExecutable
							(
								LaunchPath
								, NewCommandLine
								, CFile::fs_GetPath(LaunchPath)
								, [&, pExited, Test, pClock, pOutput, fOutputThisTest](CProcessLaunchStateChangeVariant const &_StateChange, fp64 _TimeSinceLaunch)
								{
									if (*pExited)
										return;
									if (_StateChange.f_GetTypeID() == EProcessLaunchState_Launched)
									{
										if (!bQuiet)
											DMibConOut2(" {sz*,a-}  Launched{\n}", Test, MaxTestLen);
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
											DMibConOut2(" {sz*,a-}  Exited uncleanly with {}{\n}", Test, MaxTestLen, ExitCode);
											fOutputThisTest();
										}
										else if (!bQuiet)
										{
											DMibConOut2(" {sz*,a-}  {fe1} s   {}/{} done{\n}", Test, MaxTestLen, pClock->f_GetTime(), (nDone), nTotalLaunches);
											fOutputThisTest();
										}

										fAddLaunches();

										return;
									}
									else if (_StateChange.f_GetTypeID() == EProcessLaunchState_LaunchFailed)
									{
										--nRunning;
										++nDone;
										CombinedExitCode = fg_Max(CombinedExitCode, uint32(254));
										DMibConOut2(" {sz*,a-}  Failed to launch: {}{\n}", Test, MaxTestLen, _StateChange.f_Get<EProcessLaunchState_LaunchFailed>());
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

						Params.m_fOnOutput = [&, pOutput, pClock, Test](EProcessLaunchOutputType _OutputType, CStr const &_Output)
							{
								*pOutput += _Output;
							}
						;
					
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
			}
			if (!bLoopTests)
				break;
		}
		return CombinedExitCode;
	}

};

DMibAppImplement(CRunAllTestsApplication);
