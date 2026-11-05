// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#if DMibConfig_Tests_Enable

#include <Mib/Test/ReporterText>

namespace NMib::NTest
{
	template <typename t_CFunction>
	struct TCTestRecursive
	{
		struct CLocalResults : public CTextTestResults
		{
			CLocalResults(ETestReportFlag _ReportFlags);

			ETestNeedReportFlag f_NeedReport(ETestResult _Result, ETest _FailureAction, ECheckType _CheckType, ETestFlag _Flags) override;

			bool m_bSuccess;
			NStr::CStr m_ReportText;

		private:
			void fp_ReportText(NStr::CStr const &_Text, ETestSeverity _Severity) override;
		};

		TCTestRecursive(t_CFunction const &_Function);
		~TCTestRecursive();

		operator bool () const;

		NStr::CStr f_GetExtraData() const;

		template <typename tf_CFormatter>
		int f_GetStringFormatType(tf_CFormatter &_Formatter);

		template <typename tf_CFormatter>
		// This crashes GCC
		// auto f_CreateStringFormatter(tf_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, ""))
		NStr::CStrFormatTypeClassifier_String f_CreateStringFormatter(tf_CFormatter &_Formatter) const;

		t_CFunction m_Function;
		mutable NStr::CStr m_ReportText;
	};

	template <typename tf_CFunction>
	TCTestRecursive<tf_CFunction> fg_TestRecursive(tf_CFunction const &_Function);

#define DMibTestRecursive(d_Expression) DMibExpr(fg_TestRecursive([&]{d_Expression;}))

#ifndef DMibPNoShortCuts
#	define DTestRecursive DMibTestRecursive
#endif
}

#include "Malterlib_Test_Recursive.hpp"

#endif
