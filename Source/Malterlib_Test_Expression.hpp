// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTest::NExpression::NPrivate
{
	template <typename tf_CType>
	bool fg_ConvertToBool(tf_CType &&_Value)
		requires requires (tf_CType &&_Value)
		{
			bool(_Value);
		}
	{
		return bool(_Value);
	}

	template <typename tf_CType>
	bool fg_ConvertToBool(tf_CType &&_Value)
	{
		return false;
	}

	template <typename tf_CType>
	TCEnableIf<NTraits::cIsSame<NTraits::TCRemoveReference<tf_CType>, bool>, bool> fg_IsVariableBoolean(tf_CType &&_Value)
	{
		return _Value == 1 || _Value == 0;
	}

	template <typename tf_CType>
	TCDisableIf<NTraits::cIsSame<NTraits::TCRemoveReference<tf_CType>, bool>, bool> fg_IsVariableBoolean(tf_CType &&_Value)
	{
		return false;
	}
}

namespace NMib::NTest::NExpression
{
	template <typename t_CLeft, typename t_CRight>
	TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>::TCExpressionWithOperator(t_CLeft const &_Left, t_CRight const &_Right)
		: m_Left(_Left)
		, m_Right(_Right)
	{
	}

	template <typename t_CLeft, typename t_CRight>
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>::f_GetDesc() const
	{
		return m_Left.f_GetDescRecursive() + " && " + m_Right.f_GetDescRecursive();
	}

	template <typename t_CLeft, typename t_CRight>
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>::f_GetDescRecursive() const
	{
		return "(" + m_Left.f_GetDescRecursive() + " && " + m_Right.f_GetDescRecursive() + ")";
	}

	template <typename t_CLeft, typename t_CRight>
	bool TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>::f_OnlyEvalOnce() const
	{
		return m_Left.f_OnlyEvalOnce() || m_Right.f_OnlyEvalOnce();
	}

	template <typename t_CLeft, typename t_CRight>
	auto TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
		-> decltype(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) && fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive))
		requires requires ()
		{
			DMibTemp_RequiresCheck(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) && fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive));
		}
	{
		if (!o_pValueDesc)
			return m_Left.f_GetVariable(nullptr, true) && m_Right.f_GetVariable(nullptr, true);
		using namespace NStr;
		CStr LeftDesc;
		CStr RightDesc = "NoEval";
		auto Return = m_Left.f_GetVariable(&LeftDesc, true) && m_Right.f_GetVariable(&RightDesc, true);

		CStr ThisDesc;
		if (!NPrivate::fg_IsVariableBoolean(Return))
			ThisDesc = CStr::fs_ToStr(Return);

		if (ThisDesc.f_IsEmpty())
		{
			if (_bRecursive)
				*o_pValueDesc = "(";
			*o_pValueDesc += NPrivate::fg_LineBreakDescLeft(LeftDesc);
			*o_pValueDesc += " && ";
			*o_pValueDesc += NPrivate::fg_LineBreakDescRight(RightDesc);
			if (_bRecursive)
				*o_pValueDesc += ")";
		}
		else
		{
			*o_pValueDesc = ThisDesc;
			*o_pValueDesc += " [";
			*o_pValueDesc += NPrivate::fg_LineBreakDescLeft(LeftDesc);
			*o_pValueDesc += " && ";
			*o_pValueDesc += NPrivate::fg_LineBreakDescRight(RightDesc);
			*o_pValueDesc += "]";
		}

		return Return;
	}

	template <typename t_CLeft, typename t_CRight>
	TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>::TCExpressionWithOperator(t_CLeft const &_Left, t_CRight const &_Right)
		: m_Left(_Left)
		, m_Right(_Right)
	{
	}

	template <typename t_CLeft, typename t_CRight>
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>::f_GetDesc() const
	{
		return m_Left.f_GetDescRecursive() + " || " + m_Right.f_GetDescRecursive();
	}

	template <typename t_CLeft, typename t_CRight>
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>::f_GetDescRecursive() const
	{
		return "(" + m_Left.f_GetDescRecursive() + " || " + m_Right.f_GetDescRecursive() + ")";
	}

	template <typename t_CLeft, typename t_CRight>
	bool TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>::f_OnlyEvalOnce() const
	{
		return m_Left.f_OnlyEvalOnce() || m_Right.f_OnlyEvalOnce();
	}

	template <typename t_CLeft, typename t_CRight>
	auto TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
		-> decltype(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) || fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive))
		requires requires ()
		{
			DMibTemp_RequiresCheck(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) || fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive));
		}
	{
		if (!o_pValueDesc)
			return m_Left.f_GetVariable(nullptr, true) || m_Right.f_GetVariable(nullptr, true);

		NStr::CStr LeftDesc;
		NStr::CStr RightDesc = "NoEval";
		auto Return = m_Left.f_GetVariable(&LeftDesc, true) || m_Right.f_GetVariable(&RightDesc, true);

		NStr::CStr ThisDesc;
		if (!NPrivate::fg_IsVariableBoolean(Return))
			ThisDesc = NStr::CStr::fs_ToStr(Return);

		if (ThisDesc.f_IsEmpty())
		{
			if (_bRecursive)
				*o_pValueDesc = "(";
			*o_pValueDesc += NPrivate::fg_LineBreakDescLeft(LeftDesc);
			*o_pValueDesc += " || ";
			*o_pValueDesc += NPrivate::fg_LineBreakDescRight(RightDesc);
			if (_bRecursive)
				*o_pValueDesc += ")";
		}
		else
		{
			*o_pValueDesc = ThisDesc;
			*o_pValueDesc += " [";
			*o_pValueDesc += NPrivate::fg_LineBreakDescLeft(LeftDesc);
			*o_pValueDesc += " || ";
			*o_pValueDesc += NPrivate::fg_LineBreakDescRight(RightDesc);
			*o_pValueDesc += "]";
		}

		return Return;
	}

#define DMibTemp_DefineBinaryOperator(d_OperatorType, d_OperatorDesc, d_OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right)\
		: m_Left(_Left)\
		, m_Right(_Right)\
	{\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetDesc() const\
	{\
		return m_Left.f_GetDescRecursive() + d_OperatorDesc + m_Right.f_GetDescRecursive();\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetDescRecursive() const\
	{\
		return "(" + m_Left.f_GetDescRecursive() + d_OperatorDesc + m_Right.f_GetDescRecursive() + ")";\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	bool TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_OnlyEvalOnce() const\
	{\
		return m_Left.f_OnlyEvalOnce() || m_Right.f_OnlyEvalOnce();\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	auto TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const\
		requires requires()\
		{\
			DMibTemp_RequiresCheck(this->fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) d_OperatorCpp this->fsp_GetRight().f_GetVariable(o_pValueDesc, _bRecursive));\
		}\
	{\
		if (!o_pValueDesc) \
			return m_Left.f_GetVariable(nullptr, true) d_OperatorCpp m_Right.f_GetVariable(nullptr, true);\
		NStr::CStr LeftDesc;\
		NStr::CStr RightDesc;\
		auto Return = m_Left.f_GetVariable(&LeftDesc, true) d_OperatorCpp m_Right.f_GetVariable(&RightDesc, true);\
		NStr::CStr ThisValue;\
		if (!NPrivate::fg_IsVariableBoolean(Return))\
			ThisValue = NStr::CStr::fs_ToStr(Return);\
		if (_bRecursive)\
		{\
			if (ThisValue.f_IsEmpty())\
				*o_pValueDesc = "(" + NPrivate::fg_LineBreakDescLeft(LeftDesc) + d_OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc) + ")"; \
			else\
				*o_pValueDesc = ThisValue + " [" + NPrivate::fg_LineBreakDescLeft(LeftDesc) + d_OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc) + "]"; \
		}\
		else\
		{\
			if (ThisValue.f_IsEmpty())\
				*o_pValueDesc = NPrivate::fg_LineBreakDescLeft(LeftDesc) + d_OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc); \
			else\
				*o_pValueDesc = ThisValue + " [" + NPrivate::fg_LineBreakDescLeft(LeftDesc) + d_OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc) + "]"; \
		}\
		return Return;\
	}

#define DMibTemp_DefineUnaryOperator(d_OperatorType, d_OperatorDesc, d_OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::TCExpressionWithOperator(const t_CLeft &_Left)\
		: m_Left(_Left)\
	{\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetDesc() const\
	{\
		return d_OperatorDesc + m_Left.f_GetDescRecursive();\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetDescRecursive() const\
	{\
		return d_OperatorDesc + m_Left.f_GetDescRecursive();\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	bool TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_OnlyEvalOnce() const\
	{\
		return m_Left.f_OnlyEvalOnce();\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	auto TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const\
		-> decltype (d_OperatorCpp fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive))\
		requires requires()\
		{\
			DMibTemp_RequiresCheck(d_OperatorCpp this->fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive));\
		}\
	{\
		if (!o_pValueDesc)\
			return d_OperatorCpp m_Left.f_GetVariable(nullptr, true);\
		NStr::CStr LeftDesc;\
		auto Return = d_OperatorCpp m_Left.f_GetVariable(&LeftDesc, true);\
		*o_pValueDesc = d_OperatorDesc + LeftDesc;\
		return Return;\
	}

#define DMibTemp_DefineUnaryPostfixOperator(d_OperatorType, d_OperatorDesc, d_OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::TCExpressionWithOperator(const t_CLeft &_Left)\
		: m_Left(_Left)\
	{\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetDesc() const\
	{\
		return m_Left.f_GetDescRecursive() + d_OperatorDesc;\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	NStr::CStr TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetDescRecursive() const\
	{\
		return m_Left.f_GetDescRecursive() + d_OperatorDesc;\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	bool TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_OnlyEvalOnce() const\
	{\
		return m_Left.f_OnlyEvalOnce();\
	}\
	\
	template <typename t_CLeft, typename t_CRight>\
	auto TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const\
		-> decltype (fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) d_OperatorCpp)\
		requires requires()\
		{\
			DMibTemp_RequiresCheck(this->fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) d_OperatorCpp);\
		}\
	{\
		if (!o_pValueDesc)\
			return m_Left.f_GetVariable(nullptr, true) d_OperatorCpp;\
		NStr::CStr LeftDesc;\
		auto Return = m_Left.f_GetVariable(&LeftDesc, true) d_OperatorCpp;\
		*o_pValueDesc = LeftDesc + d_OperatorDesc;\
		return Return;\
	}

	DMibTemp_DefineBinaryOperator(EOperator_Spaceship, " <=> ", <=>);
	DMibTemp_DefineBinaryOperator(EOperator_LessThan, " < ", <);
	DMibTemp_DefineBinaryOperator(EOperator_LessThanEqual, " <= ", <=);
	DMibTemp_DefineBinaryOperator(EOperator_GreaterThan, " > ", >);
	DMibTemp_DefineBinaryOperator(EOperator_GreaterThanEqual, " >= ", >=);
	DMibTemp_DefineBinaryOperator(EOperator_Equal, " == ", ==);
	DMibTemp_DefineBinaryOperator(EOperator_NotEqual, " != ", !=);
	// DMibTemp_DefineBinaryOperator(EOperator_Logical_And, " && ", &&);
	// DMibTemp_DefineBinaryOperator(EOperator_Logical_Or, " || ", ||);
	DMibTemp_DefineBinaryOperator(EOperator_Arithmetic_Addition, " + ", +);
	DMibTemp_DefineBinaryOperator(EOperator_Arithmetic_Subtraction, " - ", -);
	DMibTemp_DefineBinaryOperator(EOperator_Arithmetic_Multiplication, " * ", *);
	DMibTemp_DefineBinaryOperator(EOperator_Arithmetic_Division, " / ", /);
	DMibTemp_DefineBinaryOperator(EOperator_Arithmetic_Modulu, " % ", %);
	DMibTemp_DefineBinaryOperator(EOperator_Bitwise_ShiftLeft, " << ", <<);
	DMibTemp_DefineBinaryOperator(EOperator_Bitwise_ShiftRight, " >> ", >>);
	DMibTemp_DefineBinaryOperator(EOperator_Bitwise_And, " & ", &);
	DMibTemp_DefineBinaryOperator(EOperator_Bitwise_Or, " | ", |);
	DMibTemp_DefineBinaryOperator(EOperator_Bitwise_Xor, " ^ ", ^);

	DMibTemp_DefineUnaryOperator(EOperator_Logical_Not, "!", !);
	DMibTemp_DefineUnaryOperator(EOperator_Unary_Plus, "+", +);
	DMibTemp_DefineUnaryOperator(EOperator_Unary_Minus, "-", -);
	DMibTemp_DefineUnaryOperator(EOperator_Bitwise_Not, "~", ~);

	DMibTemp_DefineUnaryOperator(EOperator_Unary_Indirection, "*", *);
	DMibTemp_DefineUnaryOperator(EOperator_Unary_PrefixIncrement, "++", ++);
	DMibTemp_DefineUnaryOperator(EOperator_Unary_PrefixDecrement, "--", --);

	DMibTemp_DefineUnaryPostfixOperator(EOperator_Unary_PostfixIncrement, "++", ++);
	DMibTemp_DefineUnaryPostfixOperator(EOperator_Unary_PostfixDecrement, "--", --);

#undef DMibTemp_DefineBinaryOperator
#undef DMibTemp_DefineUnaryOperator
#undef DMibTemp_DefineUnaryPostfixOperator
#undef DMibTemp_RequiresCheck

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	bool TCExpression<t_CType, t_bLambda, t_CLambdaReturn>::f_Eval(NStr::CStr *o_pValueDesc) const
	{
		DMibFastCheck(o_pValueDesc); // Should only be called once

		using CReturnType = decltype (mp_Lambda());

		if constexpr (NTraits::cIsVoid<CReturnType>)
		{
			*o_pValueDesc = "void";
			return false;
		}
		else
		{
			auto Value = mp_Lambda();
			*o_pValueDesc = NStr::CStr::fs_ToStr(Value);
			return NPrivate::fg_ConvertToBool(Value);
		}
	}

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	NStr::CStr TCExpression<t_CType, t_bLambda, t_CLambdaReturn>::f_GetDesc() const
	{
		return mp_pDescription;
	}

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	NStr::CStr TCExpression<t_CType, t_bLambda, t_CLambdaReturn>::f_GetDescRecursive() const
	{
		return mp_pDescription;
	}

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	bool TCExpression<t_CType, t_bLambda, t_CLambdaReturn>::f_OnlyEvalOnce() const
	{
		return true;
	}

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	NStr::CStr TCExpression<t_CType, t_bLambda, t_CLambdaReturn>::f_GetValueDescRecursive() const
	{
		using CReturnType = decltype (mp_Lambda());

		return fs_ValueDesc<CReturnType>(mp_Lambda);
	}

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	t_CLambdaReturn TCExpression<t_CType, t_bLambda, t_CLambdaReturn>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
	{
		DMibFastCheck(o_pValueDesc);

		using CReturnType = decltype (mp_Lambda());

		if constexpr (NTraits::cIsVoid<CReturnType>)
		{
			*o_pValueDesc = "void";
			return false;
		}
		else
		{
			auto Return = mp_Lambda();
			*o_pValueDesc = NStr::CStr::fs_ToStr(Return);
			return Return;
		}
	}

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	TCExpression<t_CType, t_bLambda, t_CLambdaReturn>::TCExpression(t_CType _Lambda, ch8 const *_pDescription)
		: mp_pDescription(_pDescription)
		, mp_Lambda(_Lambda)
	{
	}

	template <typename t_CType, typename t_CLambdaReturn>
	bool TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>::f_Eval(NStr::CStr *o_pValueDesc) const
	{
		return false;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	NStr::CStr TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>::f_GetDesc() const
	{
		return mp_pDescription;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	NStr::CStr TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>::f_GetDescRecursive() const
	{
		return mp_pDescription;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	bool TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>::f_OnlyEvalOnce() const
	{
		return true;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	t_CType TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
	{
		DMibFastCheck(o_pValueDesc);
		NTest::NPrivate::fg_RegisterDynamicValue(o_pValueDesc);
		return mp_Lambda;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>::TCExpression(t_CType _Lambda, ch8 const *_pDescription)
		: mp_pDescription(_pDescription)
		, mp_Lambda(_Lambda)
	{
	}

	template <typename t_CType, typename t_CLambdaReturn>
	NStr::CStr TCExpression<t_CType, ELambda_None, t_CLambdaReturn>::f_GetDesc() const
	{
		return mp_pDescription;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	NStr::CStr TCExpression<t_CType, ELambda_None, t_CLambdaReturn>::f_GetDescRecursive() const
	{
		return mp_pDescription;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	bool TCExpression<t_CType, ELambda_None, t_CLambdaReturn>::f_OnlyEvalOnce() const
	{
		return false;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	t_CType const &TCExpression<t_CType, ELambda_None, t_CLambdaReturn>::f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
	{
		if (o_pValueDesc)
			*o_pValueDesc = NStr::CStr::fs_ToStr(mp_Variable);
		return mp_Variable;
	}

	template <typename t_CType, typename t_CLambdaReturn>
	template <typename tf_CType>
	TCExpression<t_CType, ELambda_None, t_CLambdaReturn>::TCExpression(tf_CType &&_Variable, ch8 const *_pDescription)
		: mp_pDescription(_pDescription)
		, mp_Variable(fg_Forward<tf_CType>(_Variable))
	{
	}

	template <typename tf_CType, CLambdaUnderlying tf_bLambda>
	CExpression::CExpression(TCExpression<tf_CType, tf_bLambda> const &_Expression)
	{
		mp_pExpression = fg_ConstructObject<TCExpressionImpl<TCExpression<tf_CType, tf_bLambda>>>(NMemory::CDefaultAllocator(), _Expression);
	}

	template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>
	CExpression::CExpression(TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Expression)
	{
		mp_pExpression = fg_ConstructObject<TCExpressionImpl<TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>>>(NMemory::CDefaultAllocator(), _Expression);
	}

	template <typename t_CImpl>
	CExpression::TCExpressionImpl<t_CImpl>::TCExpressionImpl(t_CImpl const &_Impl)
		: mp_Impl(_Impl)
	{
	}

	template <typename t_CImpl>
	bool CExpression::TCExpressionImpl<t_CImpl>::f_Eval(NStr::CStr *o_pValueDesc) const
	{
		return mp_Impl.f_Eval(o_pValueDesc);
	}

	template <typename t_CImpl>
	bool CExpression::TCExpressionImpl<t_CImpl>::f_OnlyEvalOnce() const
	{
		return mp_Impl.f_OnlyEvalOnce();
	}

	template <typename t_CImpl>
	NStr::CStr CExpression::TCExpressionImpl<t_CImpl>::f_GetDesc() const
	{
		return mp_Impl.f_GetDesc();
	}

	template <typename t_CImpl>
	NStr::CStr CExpression::TCExpressionImpl<t_CImpl>::f_GetDescRecursive() const
	{
		return mp_Impl.f_GetDescRecursive();
	}

	template <typename t_CImpl>
	NStr::CStr CExpression::TCExpressionImpl<t_CImpl>::f_GetValueDescRecursive() const
	{
		return mp_Impl.f_GetValueDescRecursive();
	}

	template <typename tf_CType>
	inline auto fg_GetExpression(tf_CType &&_Type, ch8 const *_pDescription) -> TCExpression<tf_CType, ELambda_None>
	{
		return TCExpression<tf_CType, ELambda_None>(fg_Forward<tf_CType>(_Type), _pDescription);
	}

	template <typename tf_CType>
	inline_small auto fg_GetLambdaExpression(tf_CType _Type, ch8 const *_pDescription) -> TCExpression<tf_CType, ELambda_Normal, decltype(_Type())>
	{
		return TCExpression<tf_CType, ELambda_Normal, decltype(_Type())>(_Type, _pDescription);
	}

	template <typename tf_CType>
	inline_small auto fg_GetLambdaExpressionNoEval(tf_CType _Type, ch8 const *_pDescription) -> TCExpression<tf_CType, ELambda_NoEval, decltype(_Type())>
	{
		return TCExpression<tf_CType, ELambda_NoEval, decltype(_Type())>(_Type, _pDescription);
	}
}
