// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTest::NPrivate
{
	void fg_RegisterDynamicValue(NStr::CStr *_pValue);
}

namespace NMib::NTest::NExpression
{
	using COperatorUnderlying = int32;
	enum EOperator : int32
	{
		EOperator_Spaceship
		, EOperator_LessThan
		, EOperator_LessThanEqual
		, EOperator_GreaterThan
		, EOperator_GreaterThanEqual
		, EOperator_Equal
		, EOperator_NotEqual

		, EOperator_Logical_Not
		, EOperator_Logical_And
		, EOperator_Logical_Or

		, EOperator_Unary_Plus
		, EOperator_Unary_Minus
		, EOperator_Unary_Indirection

		, EOperator_Unary_PostfixIncrement
		, EOperator_Unary_PostfixDecrement
		, EOperator_Unary_PrefixIncrement
		, EOperator_Unary_PrefixDecrement

		, EOperator_Arithmetic_Addition
		, EOperator_Arithmetic_Subtraction
		, EOperator_Arithmetic_Multiplication
		, EOperator_Arithmetic_Division
		, EOperator_Arithmetic_Modulu

		, EOperator_Bitwise_ShiftLeft
		, EOperator_Bitwise_ShiftRight
		, EOperator_Bitwise_Not
		, EOperator_Bitwise_And
		, EOperator_Bitwise_Or
		, EOperator_Bitwise_Xor
	};

	using CLambdaUnderlying = int32;
	enum ELambda : int32
	{
		ELambda_None
		, ELambda_Normal
		, ELambda_NoEval
	};

	namespace NPrivate
	{
		template <typename tf_CType>
		bool fg_ConvertToBool(tf_CType &&_Value)
			requires requires (tf_CType &&_Value)
			{
				bool(_Value);
			}
		;

		template <typename tf_CType>
		bool fg_ConvertToBool(tf_CType &&_Value);

		template <typename tf_CType>
		TCEnableIf<NTraits::cIsSame<NTraits::TCRemoveReference<tf_CType>, bool>, bool> fg_IsVariableBoolean(tf_CType &&_Value);
		template <typename tf_CType>
		TCDisableIf<NTraits::cIsSame<NTraits::TCRemoveReference<tf_CType>, bool>, bool> fg_IsVariableBoolean(tf_CType &&_Value);

		NStr::CStr fg_LineBreakDescLeft(NStr::CStr const &_Desc);
		NStr::CStr fg_LineBreakDescRight(NStr::CStr const &_Desc);
	}
	template <typename t_CLeft, typename t_CRight, COperatorUnderlying t_Operator>
	struct TCExpressionWithOperator;
	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	struct TCExpression;

#define DImplementExpressionInterfaceNoEval(d_ThisType) \
	public:\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator <=> (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Spaceship>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Spaceship>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator <=> (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Spaceship>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Spaceship>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator < (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_LessThan>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_LessThan>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator < (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_LessThan>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_LessThan>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator <= (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_LessThanEqual>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_LessThanEqual>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator <= (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_LessThanEqual>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_LessThanEqual>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator > (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_GreaterThan>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_GreaterThan>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator > (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_GreaterThan>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_GreaterThan>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator >= (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_GreaterThanEqual>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_GreaterThanEqual>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator >= (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_GreaterThanEqual>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_GreaterThanEqual>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator == (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Equal>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Equal>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator == (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Equal>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Equal>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator != (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_NotEqual>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_NotEqual>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator != (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_NotEqual>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_NotEqual>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator && (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Logical_And>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Logical_And>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator && (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Logical_And>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Logical_And>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator || (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Logical_Or>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Logical_Or>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator || (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Logical_Or>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Logical_Or>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator + (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Addition>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Addition>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator + (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Addition>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Addition>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator - (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Subtraction>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Subtraction>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator - (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Subtraction>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Subtraction>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator * (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Multiplication>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Multiplication>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator * (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Multiplication>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Multiplication>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator / (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Division>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Division>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator / (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Division>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Division>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator % (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Modulu>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Arithmetic_Modulu>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator % (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Modulu>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Arithmetic_Modulu>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator << (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_ShiftLeft>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_ShiftLeft>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator << (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_ShiftLeft>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_ShiftLeft>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator >> (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_ShiftRight>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_ShiftRight>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator >> (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_ShiftRight>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_ShiftRight>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator ^ (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_Xor>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_Xor>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator ^ (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_Xor>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_Xor>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator & (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_And>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_And>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator & (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_And>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_And>(*this, _Other);\
		}\
		template <typename tf_CType, CLambdaUnderlying tf_bLambda, typename tf_CLambdaReturn>\
		auto operator | (TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_Or>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpression<tf_CType, tf_bLambda, tf_CLambdaReturn>, EOperator_Bitwise_Or>(*this, _Other);\
		}\
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>\
		auto operator | (TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Other) const\
			-> TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_Or>\
		{\
			return TCExpressionWithOperator<d_ThisType, TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator>, EOperator_Bitwise_Or>(*this, _Other);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Bitwise_Not> operator ~ () const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Bitwise_Not>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Logical_Not> operator ! () const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Logical_Not>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_Plus> operator + () const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_Plus>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_Minus> operator - () const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_Minus>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_Indirection> operator * () const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_Indirection>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PrefixIncrement> operator ++ () const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PrefixIncrement>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PrefixDecrement> operator -- () const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PrefixDecrement>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PostfixIncrement> operator ++ (int) const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PostfixIncrement>(*this);\
		}\
		TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PostfixDecrement> operator -- (int) const\
		{\
			return TCExpressionWithOperator<d_ThisType, CEmpty, EOperator_Unary_PostfixDecrement>(*this);\
		}

#ifdef DCompiler_MSVC_Workaround
#	define DMibTemp_RequiresCheck(...) true
#else
#	define DMibTemp_RequiresCheck(...) __VA_ARGS__
#endif

#define DMibTemp_ImplementExpressionInterface(d_ThisType) \
	public:\
		bool f_Eval(NStr::CStr *o_pValueDesc) const\
			requires requires() \
			{\
				DMibTemp_RequiresCheck(NPrivate::fg_ConvertToBool(this->f_GetVariable(o_pValueDesc, false)));\
			}\
		{\
			return NPrivate::fg_ConvertToBool(f_GetVariable(o_pValueDesc, false));\
		}\
	DImplementExpressionInterfaceNoEval(d_ThisType)
#define DMibTemp_DeclareBinaryOperator(d_OperatorType, d_OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	struct TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>\
	{\
	private:\
		static const t_CLeft &fsp_GetLeft();\
		static const t_CRight &fsp_GetRight();\
	public:\
		const t_CLeft &m_Left;\
		const t_CRight &m_Right;\
		TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right);\
		NStr::CStr f_GetDesc() const;\
		NStr::CStr f_GetDescRecursive() const;\
		bool f_OnlyEvalOnce() const;\
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const\
			requires requires()\
			{\
				DMibTemp_RequiresCheck(this->fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) d_OperatorCpp this->fsp_GetRight().f_GetVariable(o_pValueDesc, _bRecursive));\
			}\
		;\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

#define DMibTemp_DeclareUnaryOperator(d_OperatorType, d_OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	struct TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>\
	{\
	private:\
		static const t_CLeft &fsp_GetLeft();\
	public:\
		const t_CLeft &m_Left;\
		TCExpressionWithOperator(const t_CLeft &_Left);\
		NStr::CStr f_GetDesc() const;\
		NStr::CStr f_GetDescRecursive() const;\
		bool f_OnlyEvalOnce() const;\
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const -> decltype (d_OperatorCpp fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive))\
			requires requires()\
			{\
				DMibTemp_RequiresCheck(d_OperatorCpp this->fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive));\
			}\
		;\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

#define DMibTemp_DeclareUnaryPostfixOperator(d_OperatorType, d_OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	struct TCExpressionWithOperator<t_CLeft, t_CRight, d_OperatorType>\
	{\
	private:\
		static const t_CLeft &fsp_GetLeft();\
	public:\
		const t_CLeft &m_Left;\
		TCExpressionWithOperator(const t_CLeft &_Left);\
		NStr::CStr f_GetDesc() const;\
		NStr::CStr f_GetDescRecursive() const;\
		bool f_OnlyEvalOnce() const;\
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const -> decltype (fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) d_OperatorCpp)\
			requires requires()\
			{\
				DMibTemp_RequiresCheck(this->fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) d_OperatorCpp);\
			}\
		;\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

	template <typename t_CLeft, typename t_CRight>
	struct TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>
	{
		TCExpressionWithOperator(t_CLeft const &_Left, t_CRight const &_Right);

		NStr::CStr f_GetDesc() const;
		NStr::CStr f_GetDescRecursive() const;
		bool f_OnlyEvalOnce() const;

		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
			-> decltype(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) && fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive))
			requires requires ()
			{
				DMibTemp_RequiresCheck(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) && fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive));
			}
		;

		t_CLeft const &m_Left;
		t_CRight const &m_Right;

		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);
	};

	template <typename t_CLeft, typename t_CRight>
	struct TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>
	{
		TCExpressionWithOperator(t_CLeft const &_Left, t_CRight const &_Right);

		NStr::CStr f_GetDesc() const;
		NStr::CStr f_GetDescRecursive() const;
		bool f_OnlyEvalOnce() const;

		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
			-> decltype(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) || fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive))
			requires requires ()
			{
				DMibTemp_RequiresCheck(fg_GetType<t_CLeft const &>().f_GetVariable(o_pValueDesc, _bRecursive) || fg_GetType<t_CRight const &>().f_GetVariable(o_pValueDesc, _bRecursive));
			}
		;

		t_CLeft const &m_Left;
		t_CRight const &m_Right;

		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);
	};

	DMibTemp_DeclareBinaryOperator(EOperator_Spaceship, <=>);
	DMibTemp_DeclareBinaryOperator(EOperator_LessThan, <);
	DMibTemp_DeclareBinaryOperator(EOperator_LessThanEqual, <=);
	DMibTemp_DeclareBinaryOperator(EOperator_GreaterThan, >);
	DMibTemp_DeclareBinaryOperator(EOperator_GreaterThanEqual, >=);
	DMibTemp_DeclareBinaryOperator(EOperator_Equal, ==);
	DMibTemp_DeclareBinaryOperator(EOperator_NotEqual, !=);
	// DMibTemp_DeclareBinaryOperator(EOperator_Logical_And, &&);
	// DMibTemp_DeclareBinaryOperator(EOperator_Logical_Or, ||);
	DMibTemp_DeclareBinaryOperator(EOperator_Arithmetic_Addition, +);
	DMibTemp_DeclareBinaryOperator(EOperator_Arithmetic_Subtraction, -);
	DMibTemp_DeclareBinaryOperator(EOperator_Arithmetic_Multiplication, *);
	DMibTemp_DeclareBinaryOperator(EOperator_Arithmetic_Division, /);
	DMibTemp_DeclareBinaryOperator(EOperator_Arithmetic_Modulu, %);
	DMibTemp_DeclareBinaryOperator(EOperator_Bitwise_ShiftLeft, <<);
	DMibTemp_DeclareBinaryOperator(EOperator_Bitwise_ShiftRight, >>);
	DMibTemp_DeclareBinaryOperator(EOperator_Bitwise_And, &);
	DMibTemp_DeclareBinaryOperator(EOperator_Bitwise_Or, |);
	DMibTemp_DeclareBinaryOperator(EOperator_Bitwise_Xor, ^);

	DMibTemp_DeclareUnaryOperator(EOperator_Logical_Not, !);
	DMibTemp_DeclareUnaryOperator(EOperator_Unary_Plus, +);
	DMibTemp_DeclareUnaryOperator(EOperator_Unary_Minus, -);
	DMibTemp_DeclareUnaryOperator(EOperator_Bitwise_Not, ~);

	DMibTemp_DeclareUnaryOperator(EOperator_Unary_Indirection, *);
	DMibTemp_DeclareUnaryOperator(EOperator_Unary_PrefixIncrement, ++);
	DMibTemp_DeclareUnaryOperator(EOperator_Unary_PrefixDecrement, --);

	DMibTemp_DeclareUnaryPostfixOperator(EOperator_Unary_PostfixIncrement, ++);
	DMibTemp_DeclareUnaryPostfixOperator(EOperator_Unary_PostfixDecrement, --);

	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn = void>
	struct TCExpression
	{
		TCExpression(t_CType _Lambda, ch8 const *_pDescription);

		bool f_Eval(NStr::CStr *o_pValueDesc) const;
		NStr::CStr f_GetDesc() const;
		NStr::CStr f_GetDescRecursive() const;
		bool f_OnlyEvalOnce() const;
		NStr::CStr f_GetValueDescRecursive() const;
		t_CLambdaReturn f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const;

		DImplementExpressionInterfaceNoEval(TCExpression);

	private:
		t_CType mp_Lambda;
		ch8 const *mp_pDescription;
	};

	template <typename t_CType, typename t_CLambdaReturn>
	struct TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>
	{
		TCExpression(t_CType _Lambda, ch8 const *_pDescription);

		bool f_Eval(NStr::CStr *o_pValueDesc) const;
		NStr::CStr f_GetDesc() const;
		NStr::CStr f_GetDescRecursive() const;
		bool f_OnlyEvalOnce() const;
		t_CType f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const;

		DImplementExpressionInterfaceNoEval(TCExpression);

	private:
		t_CType mp_Lambda;
		ch8 const *mp_pDescription;
	};

	template <typename t_CType, typename t_CLambdaReturn>
	struct TCExpression<t_CType, ELambda_None, t_CLambdaReturn>
	{
		template <typename tf_CType>
		TCExpression(tf_CType &&_Variable, ch8 const *_pDescription);

		NStr::CStr f_GetDesc() const;
		NStr::CStr f_GetDescRecursive() const;
		bool f_OnlyEvalOnce() const;
		t_CType const &f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const;

		DMibTemp_ImplementExpressionInterface(TCExpression);

	private:
		t_CType mp_Variable;
		ch8 const *mp_pDescription;
	};

#undef DMibTemp_ImplementExpressionInterface
#undef DMibTemp_DeclareBinaryOperator
#undef DMibTemp_DeclareUnaryOperator
#undef DMibTemp_DeclareUnaryPostfixOperator

	struct CExpression
	{
		template <typename tf_CType, CLambdaUnderlying tf_bLambda>
		CExpression(TCExpression<tf_CType, tf_bLambda> const &_Expression);
		template <typename tf_CLeft, typename tf_CRight, COperatorUnderlying tf_Operator>
		CExpression(TCExpressionWithOperator<tf_CLeft, tf_CRight, tf_Operator> const &_Expression);
		~CExpression();

		NStr::CStr f_GetDesc() const;
		NStr::CStr f_GetDescRecursive() const;
		NStr::CStr f_GetValueDescRecursive() const;
		bool f_Eval(NStr::CStr *o_pValueDesc) const;
		bool f_OnlyEvalOnce() const;

	private:
		struct CExpressionInternal
		{
			virtual ~CExpressionInternal();

			virtual bool f_Eval(NStr::CStr *o_pValueDesc) const = 0;
			virtual NStr::CStr f_GetDesc() const = 0;
			virtual NStr::CStr f_GetDescRecursive() const = 0;
			virtual NStr::CStr f_GetValueDescRecursive() const = 0;
			virtual bool f_OnlyEvalOnce() const = 0;
		};

		template <typename t_CImpl>
		struct TCExpressionImpl : public CExpressionInternal
		{
			TCExpressionImpl(t_CImpl const &_Impl);

			bool f_Eval(NStr::CStr *o_pValueDesc) const override;
			bool f_OnlyEvalOnce() const override;
			NStr::CStr f_GetDesc() const override;
			NStr::CStr f_GetDescRecursive() const override;
			NStr::CStr f_GetValueDescRecursive() const override;

		private:
			t_CImpl const &mp_Impl;
		};

		CExpressionInternal *mp_pExpression;
	};

	template <typename tf_CType>
	auto fg_GetExpression(tf_CType &&_Type, ch8 const *_pDescription) -> TCExpression<tf_CType, ELambda_None>;
	template <typename tf_CType>
	auto fg_GetLambdaExpression(tf_CType _Type, ch8 const *_pDescription) -> TCExpression<tf_CType, ELambda_Normal, decltype(_Type())>;
	template <typename tf_CType>
	auto fg_GetLambdaExpressionNoEval(tf_CType _Type, ch8 const *_pDescription) -> TCExpression<tf_CType, ELambda_NoEval, decltype(_Type())>;

	/*template <typename t_CType>
	TCExpression<t_CType> fg_GetExpression(const t_CType &_Type, const NStr::CStr &_Description)
	{
		return TCExpression<t_CType>(_Type, _Description);
	}*/

#define DMibExpr(d_Expression) NMib::NTest::NExpression::fg_GetExpression(d_Expression, DMibStringizeUTF8(d_Expression))
#define DMibELExpr(d_Expression) NMib::NTest::NExpression::fg_GetLambdaExpression([&]() {return d_Expression;}, DMibStringizeUTF8(d_Expression))
#define DMibLExpr(d_Expression) NMib::NTest::NExpression::fg_GetLambdaExpressionNoEval([&]() {return d_Expression;}, DMibStringizeUTF8(d_Expression))

#ifndef DMibPNoShortCuts
#	define DExpr DMibExpr
#	define DELExpr DMibELExpr
#	define DLExpr DMibLExpr
#endif
}

#ifndef DMibPNoShortCuts
using namespace NMib::NTest::NExpression;
#endif

#include "Malterlib_Test_Expression.hpp"
