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

		if (bSingleCore)
		{
			for (mint i = 0; i < g_nAllTests; ++i)
			{
				CStr Test = g_AllTests[i];
				
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
				
				CombinedExitCode = fg_Max(CombinedExitCode, ExitCode);
				
			}
		}
		else
		{
			TCThreadSafeQueue<TCFunction<void ()>> ToDispatch;
			NThread::CEventAutoReset DispatchEvent;
			TCLinkedList<CProcessLaunchParams> NotLaunched;
			mint nRunning = 0;
			mint nMaxRunning = NSys::fg_Thread_GetPhysicalCores();

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
								, true
								, [&](CProcessLaunchParams const &_Params, EProcessLaunchCloseFlag _DestructFlags) -> TCUniquePointer<CVirtualProcessLaunch>
								{
									return fg_Construct<CVirtualProcessLaunch_Default>(_Params, _DestructFlags);
								}
							)
						;
						return true;
					}
				;
				for (mint i = 0; i < g_nAllTests; ++i)
				{
					CStr Test = g_AllTests[i];
					
					NPtr::TCSharedPointer<bool> pFirst = fg_Construct(true);
					CStr LaunchPath = NFile::CFile::fs_AppendPath(NFile::CFile::fs_GetProgramDirectory(), Test);
					auto Params = NProcess::CProcessLaunchParams::fs_LaunchExecutable
						(
							LaunchPath
							, NewCommandLine
							, CFile::fs_GetPath(LaunchPath)
							, [&, Test, pFirst](CProcessLaunchStateChangeVariant const &_StateChange, fp64 _TimeSinceLaunch)
							{
								if (_StateChange.f_GetTypeID() == EProcessLaunchState_Launched)
								{
									DMibConOut("Launched {}{\n}", Test);
								}
								else if (_StateChange.f_GetTypeID() == EProcessLaunchState_Exited)
								{
									--nRunning;								
									CombinedExitCode = fg_Max(CombinedExitCode, _StateChange.f_Get<EProcessLaunchState_Exited>());
								}
								else if (_StateChange.f_GetTypeID() == EProcessLaunchState_LaunchFailed)
								{
									--nRunning;								
									CombinedExitCode = fg_Max(CombinedExitCode, 255);
									if (*pFirst)
									{
										DMibConOut("Failed to launch '{}': {}{\n}", Test << _StateChange.f_Get<EProcessLaunchState_LaunchFailed>());
										*pFirst = false;
									}
								}
								fAddLaunches();
							}
							, [&ToDispatch, &DispatchEvent](NFunction::TCFunction<void ()> const &_Functor)
							{
								ToDispatch.f_Push(_Functor);
								DispatchEvent.f_Signal();
							}
						)
					;

					Params.m_fOnOutput = [pFirst, Test](EProcessLaunchOutputType _OutputType, CStr const &_Output)
						{
							if (*pFirst)
							{
								DMibConOut("Finished: {}{\n}", Test);
								*pFirst = false;
							}

							if (_OutputType == EProcessLaunchOutputType_StdOut)
								DMibConOutRaw(_Output);
							else
								DMibConErrOutRaw(_Output);
						}
					;
					
					NotLaunched.f_Insert(fg_Move(Params));
				}
				while (fAddLaunches())
					;
				
				while (!NotLaunched.f_IsEmpty() || nRunning > 0)
				{
					while (auto Entry = ToDispatch.f_Pop())
						(*Entry)();
					DispatchEvent.f_WaitTimeout(1.0);
				}
				while (auto Entry = ToDispatch.f_Pop())
					(*Entry)();
			}
			
		}
		return CombinedExitCode;
	}

};

DMibAppImplement(CRunAllTestsApplication);
