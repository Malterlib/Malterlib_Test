// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTest::NPrivate
{
#if DMibConfig_Tests_Enable
	template <bool t_bFailureOnly, typename t_CExpression>
	bool TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::fp_Evaluate() const
#	ifndef DCompiler_MSVC_Workaround
		requires NTraits::cIsSame<t_CExpression, bool> || requires ()
		{
			this->m_Expression.f_Eval(nullptr);
		}
#	endif
	{
		if constexpr (NTraits::cIsSame<t_CExpression, bool>)
		{
			if (t_bFailureOnly && m_Expression)
				return m_Expression;
			ETestResultReportFlag ReportFlags = fg_NeedReport(m_CustomMessage, (ETestResult)m_Expression, m_FailureAction, ECheckType_Message, m_pFile, m_Line, m_Flags);
			if (ReportFlags)
				fg_ReportTestResult((ETestResult)m_Expression, m_CustomMessage, "", m_FailureAction, ECheckType_Message, m_pFile, m_Line, NStr::CStr(), m_Flags, ReportFlags);
			return m_Expression;
		}
		else
		{
			ETestResult Result;
			NStr::CStr ValueDesc;
			bool bPotentianlyNeedValues = fg_GetEnableValues() && !(m_Flags & ETestFlag_NoValues) && (fg_TestReportFlags() & ETestReportFlag_ReportValues);
			bool bOnlyEvalOnce = m_Expression.f_OnlyEvalOnce();
			bool bHasValue = false;
			if (bOnlyEvalOnce && bPotentianlyNeedValues)
			{
				bHasValue = true;
				Result = m_Expression.f_Eval(&ValueDesc) ? ETestResult_Success : ETestResult_Fail;
			}
			else
				Result = m_Expression.f_Eval(nullptr) ? ETestResult_Success : ETestResult_Fail;

			if constexpr
			(
				requires ()
				{
					m_Expression.f_GetVariable(nullptr, false).f_IsIgnored();
				}
			)
			{
				if (m_Expression.f_GetVariable(nullptr, false).f_IsIgnored())
					Result = ETestResult_Ignored;
			}

			if (t_bFailureOnly && Result != ETestResult_Fail)
				return Result != ETestResult_Fail;

			NStr::CStr Desc;

			if constexpr
			(
				requires ()
				{
					m_Expression.f_GetVariable(nullptr, false).f_ModifyDescription(m_Expression.f_GetDesc());
				}
			)
			{
				Desc = m_Expression.f_GetVariable(nullptr, false).f_ModifyDescription(m_Expression.f_GetDesc());
			}
			else
				Desc = m_Expression.f_GetDesc();

			if (!m_CustomMessage.f_IsEmpty())
			{
				if (m_bOnlyCustom)
					Desc = m_CustomMessage;
				else
				{
					Desc += " ";
					Desc += m_CustomMessage;
				}
			}

			ETestResultReportFlag ReportFlags = fg_NeedReport(Desc, Result, m_FailureAction, ECheckType_Predicate, m_pFile, m_Line, m_Flags);
			auto &ResultReporter = fg_GetResultReporter();

			if (ReportFlags)
			{
				bool bIsSuccess = Result == ETestResult_Success || ((Result == ETestResult_Fail) && (m_FailureAction == ETest_ExpectFail || m_FailureAction == ETest_ExpectFailAndStop));
				bool bNeedValues = bPotentianlyNeedValues && (!(m_Flags & ETestFlag_NoValuesOnSuccess) || !bIsSuccess);

				NStr::CStr ExtraData;
				if constexpr
				(
					requires ()
					{
						m_Expression.f_GetVariable(nullptr, false).f_GetExtraData();
					}
				)
				{
					ExtraData = m_Expression.f_GetVariable(nullptr, false).f_GetExtraData();
				}

				if (bNeedValues && !bHasValue)
					m_Expression.f_Eval(&ValueDesc);

				fg_ReportTestResult(Result, Desc, bNeedValues ? ValueDesc : NStr::CStr(), m_FailureAction, ECheckType_Predicate, m_pFile, m_Line, ExtraData, m_Flags, ReportFlags);

				if constexpr
				(
					requires ()
					{
						m_Expression.f_GetVariable(nullptr, false).f_TestReport(ResultReporter);
					}
				)
				{
					m_Expression.f_GetVariable(nullptr, false).f_TestReport(ResultReporter);
				}
			}
			else if (ResultReporter.f_GetReportFlags() & (ETestReportFlag_DetailedPerformance | ETestReportFlag_DetailedMemory))
			{
				if constexpr
				(
					requires ()
					{
						m_Expression.f_GetVariable(nullptr, false).f_TestReport(ResultReporter);
					}
				)
				{
					m_Expression.f_GetVariable(nullptr, false).f_TestReport(ResultReporter);
				}
			}

			return Result != ETestResult_Fail;
		}
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::TCTestFunctionHelper(ch8 const *_pFile, int32 _Line, t_CExpression &&_Expression)
		: m_pFile(_pFile)
		, m_Line(_Line)
		, m_Expression(fg_Move(_Expression))
		, m_FailureAction(ETest_Fail)
		, m_Flags(ETestFlag_None)
		, m_bValid(false)
		, m_bOnlyCustom(false)
	{
		fg_SetTestLastLocation(_pFile, _Line);
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::TCTestFunctionHelper(TCTestFunctionHelper &&_Right)
		: m_Expression(fg_Move(_Right.m_Expression))
		, m_FailureAction(_Right.m_FailureAction)
		, m_Flags(_Right.m_Flags)
		, m_pFile(_Right.m_pFile)
		, m_Line(_Right.m_Line)
		, m_CustomMessage(fg_Move(_Right.m_CustomMessage))
		, m_bValid(_Right.m_bValid)
		, m_bOnlyCustom(_Right.m_bOnlyCustom)
	{
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	template <bool tf_bFailureOnly, typename tf_CExpression>
	TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::TCTestFunctionHelper(TCTestFunctionHelper<tf_bFailureOnly, tf_CExpression> &&_Right, t_CExpression const &_NewExpression)
		: m_Expression(_NewExpression)
		, m_FailureAction(_Right.m_FailureAction)
		, m_Flags(_Right.m_Flags)
		, m_pFile(_Right.m_pFile)
		, m_Line(_Right.m_Line)
		, m_CustomMessage(fg_Move(_Right.m_CustomMessage))
		, m_bValid(true)
		, m_bOnlyCustom(_Right.m_bOnlyCustom)
	{
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::~TCTestFunctionHelper()
	{
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator bool () const
#	ifndef DCompiler_MSVC_Workaround
		requires requires ()
		{
			fp_Evaluate();
		}
#	endif
	{
		DMibFastCheck(m_bValid);
		DMibFastCheck(NMib::NTest::NPrivate::fg_InsideTestSuite());
		if (m_Flags & ETestFlag_NoExceptionFilter)
		{
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
			return fp_Evaluate();
		}
		else
			return fp_Evaluate();
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	inline_always auto TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator () (ETest _FailureAction) -> TCTestFunctionHelper &
	{
		m_FailureAction = _FailureAction;
		return *this;
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	inline_always auto TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator () (ETestFlag _Flags) -> TCTestFunctionHelper &
	{
		m_Flags |= _Flags;
		return *this;
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	inline_always auto TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator () (ch8 const *_pMessage) -> TCTestFunctionHelper &
	{
		DMibFastCheck(m_CustomMessage.f_IsEmpty()); // Custom message already specified
		m_CustomMessage = _pMessage;
		if constexpr (NTraits::cIsSame<t_CExpression, CDummyExpression>)
			m_bOnlyCustom = true;
		return *this;
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	inline_always auto TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator () (NStr::CStr const &_Message) -> TCTestFunctionHelper &
	{
		DMibFastCheck(m_CustomMessage.f_IsEmpty()); // Custom message already specified
		m_CustomMessage = _Message;
		if constexpr (NTraits::cIsSame<t_CExpression, CDummyExpression>)
			m_bOnlyCustom = true;
		return *this;
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	inline_always auto TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator () (NStr::CStr &&_Message) -> TCTestFunctionHelper &
	{
		DMibFastCheck(m_CustomMessage.f_IsEmpty()); // Custom message already specified
		m_CustomMessage = fg_Move(_Message);
		if constexpr (NTraits::cIsSame<t_CExpression, CDummyExpression>)
			m_bOnlyCustom = true;
		return *this;
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	template <typename tf_CExpression>
	inline TCTestFunctionHelper<t_bFailureOnly, tf_CExpression> TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator () (tf_CExpression const &_Expression)
	{
		static_assert(NTraits::cIsSame<t_CExpression, CDummyExpression>, "You can only specify expressions once");
		return TCTestFunctionHelper<t_bFailureOnly, tf_CExpression>(fg_Move(*this), _Expression);
	}

	template <bool t_bFailureOnly, typename t_CExpression>
	inline TCTestFunctionHelper<t_bFailureOnly, bool> TCTestFunctionHelper<t_bFailureOnly, t_CExpression>::operator () (bool _bExpression)
	{
		static_assert(NTraits::cIsSame<t_CExpression, CDummyExpression>, "You can only specify expressions once");
		return TCTestFunctionHelper<t_bFailureOnly, bool>(fg_Move(*this), _bExpression);
	}

	template <typename tf_FFunction>
	void CTestCategoryScope::operator % (tf_FFunction &&_fCategory)
	{
		if constexpr (NConcurrency::NPrivate::TCIsFuture<NTraits::TCCallableReturnTypeFor<NTraits::TCRemoveReferenceAndQualifiers<tf_FFunction>, void ()>>::mc_Value)
			f_ProcessAsyncCategory(_fCategory);
		else
			f_ProcessCategory(_fCategory);
	}
#endif
}
