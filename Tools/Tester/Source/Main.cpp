// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/Application>
#include <Mib/Test/Test>

class CTestApplication : public NMib::CApplication
{
public:
	CTestApplication()
	{
		
	}

	aint f_Main()
	{
		NMib::NContainer::TCVector<NMib::NStr::CStr> CommandLine;
		NMib::NSys::fg_Process_GetCommandLineArgs(CommandLine);

		NMib::fg_GetSys()->f_RemoveAllLoggers();

		for (auto iArg = CommandLine.f_GetIterator(); iArg; ++iArg)
		{
			if (*iArg == "--just-exit")
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
			if (*iArg == "--std-out-exit")
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
		
		auto MemoryManagerCheckout = NMib::fg_GetSys()->f_MemoryManager_Checkout();
		auto Ret = NMib::NTest::fg_RunTests();

		return Ret;
	}
};

DMibAppImplement(CTestApplication);

