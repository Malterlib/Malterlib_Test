// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTest
{
	template <typename t_CFunction>
	TCTestRecursive<t_CFunction>::TCTestRecursive(t_CFunction const &_Function)
		: m_Function(_Function)
	{
	}

	template <typename t_CFunction>
	TCTestRecursive<t_CFunction>::~TCTestRecursive()
	{
	}

	template <typename t_CFunction>
	TCTestRecursive<t_CFunction>::CLocalResults::CLocalResults(ETestReportFlag _ReportFlags)
		: m_bSuccess(true)
		, CTextTestResults(NCommandLine::EAnsiEncodingFlag_None)
	{
		mp_ReportFlags = ETestReportFlag_Fail | ETestReportFlag_FailAndStop | ETestReportFlag_ExpectFailAndStop;
		if (_ReportFlags & ETestReportFlag_BreakOnFail)
			mp_ReportFlags |= ETestReportFlag_BreakOnFail;
	}

	template <typename t_CFunction>
	ETestNeedReportFlag TCTestRecursive<t_CFunction>::CLocalResults::f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags)
	{
		// Only report failures
		if (_Result == ETestResult_Fail)
		{
			if (mp_ReportFlags & ETestReportFlag_BreakOnFail)
				DMibPDebugBreak;
			m_bSuccess = false;
			return ETestNeedReportFlag_AllowDuplicates | ETestNeedReportFlag_Report; // | ETestNeedReportFlag_Abort;
		}
		else if (_Result == ETestResult_Success && (_FailureAction == ETest_ExpectFail || _FailureAction == ETest_ExpectFailAndStop))
		{
			if (mp_ReportFlags & ETestReportFlag_BreakOnFail)
				DMibPDebugBreak;
			m_bSuccess = false;
			return ETestNeedReportFlag_AllowDuplicates | ETestNeedReportFlag_Report; // | ETestNeedReportFlag_Abort;
		}
		return ETestNeedReportFlag_AllowDuplicates;
	}

	template <typename t_CFunction>
	void TCTestRecursive<t_CFunction>::CLocalResults::fp_ReportText(NStr::CStr const &_Text, ETestSeverity _Severity)
	{
		m_ReportText += _Text;
	}

	template <typename t_CFunction>
	TCTestRecursive<t_CFunction>::operator bool () const
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

	template <typename t_CFunction>
	NStr::CStr TCTestRecursive<t_CFunction>::f_GetExtraData() const
	{
		return m_ReportText;
	}

	template <typename t_CFunction>
	template <typename tf_CFormatter>
	NStr::CStrFormatTypeClassifier_String TCTestRecursive<t_CFunction>::f_CreateStringFormatter(tf_CFormatter &_Formatter) const
	{
		static constexpr ch8 const *c_pData = "";
		return NStr::fg_CreateStringFormatter(_Formatter, c_pData);
	}

	template <typename tf_CFunction>
	TCTestRecursive<tf_CFunction> fg_TestRecursive(tf_CFunction const &_Function)
	{
		return TCTestRecursive<tf_CFunction>(_Function);
	}
}
