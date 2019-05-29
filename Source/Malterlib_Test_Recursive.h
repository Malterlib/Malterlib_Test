// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#if DMibConfig_Tests_Enable

namespace NMib::NTest
{
	template <typename t_CFunction>
	class TCTestRecursive
	{
	public:

		t_CFunction m_Function;

		TCTestRecursive(t_CFunction const &_Function)
			: m_Function(_Function)
		{
		}

		~TCTestRecursive()
		{
		}

		mutable NStr::CStr m_ReportText;
		class CLocalResults : public CTextTestResults
		{

			void fp_ReportText(const NStr::CStr &_Text, NSys::EColor _Color) override
			{
				m_ReportText += _Text;
			}

		public:
			CLocalResults(ETestReportFlag _ReportFlags)
				: m_bSuccess(true)
			{
				mp_ReportFlags = ETestReportFlag_Fail | ETestReportFlag_FailAndStop | ETestReportFlag_ExpectFailAndStop;
				if (_ReportFlags & ETestReportFlag_BreakOnFail)
					mp_ReportFlags |= ETestReportFlag_BreakOnFail;
			}
			bool m_bSuccess;
			NStr::CStr m_ReportText;

			ETestNeedReportFlag f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags) override
			{
				// Only report failures
				if (_Result == ETestResult_Fail)
				{
					if (mp_ReportFlags & ETestReportFlag_BreakOnFail)
						DMibPDebugBreak;
					m_bSuccess = false;
					return ETestNeedReportFlag_AllowDuplicates | ETestNeedReportFlag_Report;// | ETestNeedReportFlag_Abort;
				}
				else if (_Result == ETestResult_Success && (_FailureAction == ETest_ExpectFail || _FailureAction == ETest_ExpectFailAndStop))
				{
					if (mp_ReportFlags & ETestReportFlag_BreakOnFail)
						DMibPDebugBreak;
					m_bSuccess = false;
					return ETestNeedReportFlag_AllowDuplicates | ETestNeedReportFlag_Report;// | ETestNeedReportFlag_Abort;
				}
				return ETestNeedReportFlag_AllowDuplicates;
			}
		};

		operator bool () const
		{
			CLocalResults Results(NPrivate::fg_GetResultReporter().f_GetReportFlags());
			{
				NPrivate::CTestReporterScope ResultScope(Results);

				{
					NMib::NTest::NPrivate::CTestExceptionFilter TestExceptionFilter;
					DMibExceptionFilter(TestExceptionFilter);
					if (fg_TestReportFlags() & ETestReportFlag_CrashOnException)
					{
						try
						{
							m_Function();
						}
						catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
						{
						}
					}
					else
					{
						try
						{
							m_Function();
						}
						catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
						{
						}
						catch (...)
						{
							NMib::NTest::NPrivate::fg_ReportTestException
								(
									TestExceptionFilter.f_DetachDumpFiles()
									, TestExceptionFilter.f_GetFile() ? TestExceptionFilter.f_GetFile() : "Unknown"
									, TestExceptionFilter.f_GetFile() ? TestExceptionFilter.f_GetLine() : 0
								)
							;
						}
					}
				}
			}

			if (!Results.m_ReportText.f_IsEmpty())
			{
				m_ReportText = "Subtests failed:" DMibNewLine;
				m_ReportText += Results.m_ReportText;
			}

			return Results.m_bSuccess;
		}

		NStr::CStr f_GetExtraData() const
		{
			return m_ReportText;
		}

		template <typename t_CFormatter>
		int f_GetStringFormatType(t_CFormatter &_Formatter);

		template <typename t_CFormatter>
		// This crashes GCC
		// auto f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, ""))
		NStr::CStrFormatTypeClassifier_String f_CreateStringFormatter(t_CFormatter &_Formatter) const
		{
			static const ch8 * const pData = "";
			return NStr::fg_CreateStringFormatter(_Formatter, pData);
		}
	};

	template <typename tf_CFunction>
	TCTestRecursive<tf_CFunction> fg_TestRecursive(tf_CFunction const &_Function)
	{
		return TCTestRecursive<tf_CFunction>(_Function);
	}

#	define DMibTestRecursive(_Expression) DMibExpr(fg_TestRecursive([&]{_Expression;}))

#	ifndef DMibPNoShortCuts
#		define DTestRecursive DMibTestRecursive
#	endif
}

#endif

