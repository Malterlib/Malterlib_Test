// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/Application>
#include <Mib/Process/ProcessLaunch>

using namespace NMib;

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
		NContainer::TCVector<NStr::CStr> CommandLine;
		NSys::fg_Process_GetCommandLineArgs(CommandLine);
		
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
		}
		
		CommandLine.f_Remove(0);

		uint32 CombinedExitCode = 0;
		
		for (mint i = 0; i < g_nAllTests; ++i)
		{
			NStr::CStr Test = g_AllTests[i];
			
			DMibConOut("Running: {}{\n}", Test);

			uint32 ExitCode;
			NProcess::CProcessLaunch::fs_LaunchBlock
				(
					NFile::CFile::fs_AppendPath(NFile::CFile::fs_GetProgramDirectory(), Test)
					, CommandLine
					, [&](NStr::CStr const &_Output)
					{
						DMibConOutRaw(_Output);
					}
					, [&](NStr::CStr const &_Output)
					{
						DMibConErrOutRaw(_Output);
					}
					, ExitCode
				)
			;
			
			CombinedExitCode = fg_Max(CombinedExitCode, ExitCode);
			
		}
		return CombinedExitCode;
	}

};

DMibAppImplement(CRunAllTestsApplication);
