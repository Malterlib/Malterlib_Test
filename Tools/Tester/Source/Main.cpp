// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/Application>
#include <Mib/Test/Test>

#define DEnableLogging 0

#if DEnableLogging
#	include <Mib/Log/Destinations>
#	include <Mib/Concurrency/ConcurrencyManager>
#endif

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
#if DMibSysLogSeverities && DEnableLogging
		using namespace NMib;
		using namespace NMib::NLog;
		auto LoggerActor = NMib::NConcurrency::fg_ConstructActor<NMib::NConcurrency::CSeparateThreadActor>(fg_Construct("Log"));
		NMib::fg_GetSys()->f_GetLogger().f_PushGlobalDestination
			(
				[LoggerActor]
				(
					mint _ThreadID
					, NTime::CTime const& _Time
					, ESeverity _Sev
					, CLogStr const& _Message
					, NContainer::TCVector<NStr::CStr> const &_Categories
					, NContainer::TCVector<NStr::CStr> const &_Operations
					, CLogLocationTag const& _Loc
				)
				{
					auto Category = (_Categories.f_IsEmpty() ? NStr::CStrNonTracked() : NStr::fg_Format<NStr::CStrNonTracked>("<{}>", _Categories.f_GetFirst()));
					fg_Dispatch
						(
							LoggerActor
							, [=]
							{
								NTime::CTimeConvert::CDateTime DateTime;
								NTime::CTimeConvert(_Time.f_ToLocal()).f_ExtractDateTime(DateTime);
								DMibConErrOut
									(
										"{}-{sj2,sf0}-{sj2,sf0} {sj2,sf0}:{sj2,sf0}:{sj2,sf0}.{fr1,fe3} {sj32} {sj10} {}{\n}"
										, DateTime.m_Year
										<< DateTime.m_Month
										<< DateTime.m_DayOfMonth
										<< DateTime.m_Hour
										<< DateTime.m_Minute
										<< DateTime.m_Second
										<< DateTime.m_Fraction
										<< Category 
										<< NStr::fg_Format<NStr::CStrNonTracked>("[{}]", fg_GetSeverityName(_Sev))
										<< _Message
									)
								;
							}
						)
						> NMib::NConcurrency::fg_DiscardResult()
					;
				}
			)
		;
#endif
		
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
		
		auto MemoryManagerCheckout = NMib::fg_GetSys()->f_MemoryManager_Checkout();
		auto Ret = NMib::NTest::fg_RunTests();

#if DMibSysLogSeverities && DEnableLogging
		NMib::fg_GetSys()->f_RemoveAllLoggers();
		LoggerActor->f_BlockDestroy();
		NMib::fg_GetSys()->f_AddStdErrLogger();
#endif
		return Ret;
	}

};

DMibAppImplement(CTestApplication);

