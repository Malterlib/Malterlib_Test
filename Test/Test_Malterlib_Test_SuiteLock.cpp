// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Test/Exception>
#include <Mib/Process/ProcessLaunch>
#include <Mib/File/File>
#include <Mib/Cryptography/UUID>
#include <Mib/Core/OnScopeExitCatch>

namespace
{
	using namespace NMib;
	using namespace NMib::NStr;
	using namespace NMib::NTest;

	struct CShutdownLockProbe
	{
		~CShutdownLockProbe()
		{
			try
			{
				NFile::CLockFile Probe(m_LockPath);
				auto Result = Probe.f_Lock(0.0);

				NFile::CFile::fs_WriteStringToFile(m_ResultPath, Result == NFile::CLockFile::ELockResult_TimedOut ? "locked" : "unlocked", false);
			}
			catch (...)
			{
				// The parent fails if shutdown cannot write the probe result.
			}
		}

		CStr m_LockPath;
		CStr m_ResultPath;
	};

	struct CSuiteLock_Tests : CTest
	{
		void f_DoTests()
		{
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
					auto Directory = NFile::CFile::fs_GetTemporaryDirectory() / "MalterlibSuiteLock" / NCryptography::fg_GetRandomUuidString();
					NFile::CFile::fs_CreateDirectory(Directory);
					auto Cleanup = g_OnScopeExitCatch / [&]
						{
							NFile::CFile::fs_DeleteDirectoryRecursive(Directory);
						}
					;

					auto ResultPath = Directory / "shutdown.txt";
					Params.m_Environment["MalterlibSuiteLockProbeFile"] = ResultPath;
					Params.m_Environment["MalterlibCleanupTestFiles"] = "true";

					CStr StdOut, StdErr;
					uint32 ExitCode = 0;

					bool bLaunched = NProcess::CProcessLaunch::fs_LaunchBlock
						(
							NFile::CFile::fs_GetProgramPath(), {"--test", NFile::CFile::fs_GetPath(Suite) / "IndependentHelper", "--groups", "Manual"}
							, StdOut, StdErr, ExitCode, Params
						)
					;

					DMibAssertTrue(bLaunched);
					DMibExpect(ExitCode, ==, 0);
					DMibExpect(StdOut.f_Find("SuiteLockIndependentEntered"), >=, 0);
					DMibAssertTrue(NFile::CFile::fs_FileExists(ResultPath));
					DMibExpect(NFile::CFile::fs_ReadStringFromFile(ResultPath), ==, CStr("locked"));
					DMibExpectFalse(NFile::CFile::fs_FileExists(ResultPath + ".cleanup"));
				};
			};

			DMibTestSuite(CTestCategory("IndependentHelper") << CTestGroup("Manual"))
			{
				auto ResultPath = fg_GetSys()->f_GetEnvironmentVariable("MalterlibSuiteLockProbeFile", "");
				if (!ResultPath.f_IsEmpty())
				{
					NStorage::TCSharedPointer<CShutdownLockProbe> pProbe = fg_Construct();
					pProbe->m_LockPath = NTest::NPrivate::fg_GetSuiteLockPath(fg_TestGetCurrentPath());
					pProbe->m_ResultPath = ResultPath;

					NFile::CFile::fs_CreateDirectory(ResultPath + ".cleanup");
					fg_TestAddCleanupPath(ResultPath + ".cleanup");

					// Exception-info generators outlive test-file cleanup but are released before suite locks.
					fg_TestAddExceptionInfoGenerator
						(
							[pProbe]() -> CStr
							{
								return {};
							}
						)
					;
				}

				DMibConOut("SuiteLockIndependentEntered\n");
			};
		}
	};
}

DMibTestRegister(CSuiteLock_Tests, Malterlib::Test);
