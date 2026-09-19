// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Exception>
#include <Mib/Process/ProcessLaunch>
#include <Mib/File/File>

namespace
{
	using namespace NMib;
	using namespace NMib::NStr;
	using namespace NMib::NTest;

	struct CSuiteLock_Tests : CTest
	{
		void f_DoTests()
		{
			if (fg_GetSys()->f_GetEnvironmentVariable("MalterlibSuiteLockChild", "") == "different")
			{
				DMibTestSuite("IndependentHelper")
				{
					DMibConOut("SuiteLockIndependentEntered\n");
				};
				return;
			}

			DMibTestSuite("ProcessExclusion")
			{
				if (fg_GetSys()->f_GetEnvironmentVariable("MalterlibSuiteLockChild", "") == "true")
				{
					DMibConOut("SuiteLockChildEntered\n");
					return;
				}

				NProcess::CProcessLaunchParams Params;
				Params.m_Environment["MalterlibSuiteLockChild"] = "true";
				Params.m_bMergeEnvironment = true;
				auto Suite = fg_TestGetCurrentPath();

				DMibTestCategory("OverlappingSuite")
				{
					CStr StdOut, StdErr;
					uint32 ExitCode = 0;
					bool bLaunched = NProcess::CProcessLaunch::fs_LaunchBlock
						(
							NFile::CFile::fs_GetProgramPath(), {"--test", Suite}
							, StdOut, StdErr, ExitCode, Params
						)
					;

					DMibAssertTrue(bLaunched);
					DMibExpect(ExitCode, !=, 0);
					DMibExpect((StdOut + StdErr).f_Find("Test suite is already running"), >=, 0);
					DMibExpect((StdOut + StdErr).f_Find("SuiteLockChildEntered"), <, 0);
				};

				DMibTestCategory("RecursiveHelper")
				{
					CStr StdOut, StdErr;
					uint32 ExitCode = 0;
					bool bLaunched = NProcess::CProcessLaunch::fs_LaunchBlock
						(
							NFile::CFile::fs_GetProgramPath(), {"--test", Suite, "--process-recursive"}
							, StdOut, StdErr, ExitCode, Params
						)
					;

					DMibAssertTrue(bLaunched);
					DMibExpect(ExitCode, ==, 0);
					DMibExpect(StdOut.f_Find("SuiteLockChildEntered"), >=, 0);
				};

				DMibTestCategory("DifferentSuite")
				{
					Params.m_Environment["MalterlibSuiteLockChild"] = "different";
					CStr StdOut, StdErr;
					uint32 ExitCode = 0;
					bool bLaunched = NProcess::CProcessLaunch::fs_LaunchBlock
						(
							NFile::CFile::fs_GetProgramPath(), {"--test", NFile::CFile::fs_GetPath(Suite) / "IndependentHelper"}
							, StdOut, StdErr, ExitCode, Params
						)
					;

					DMibAssertTrue(bLaunched);
					DMibExpect(ExitCode, ==, 0);
					DMibExpect(StdOut.f_Find("SuiteLockIndependentEntered"), >=, 0);
				};
			};
		}
	};
}

DMibTestRegister(CSuiteLock_Tests, Malterlib::Test);
