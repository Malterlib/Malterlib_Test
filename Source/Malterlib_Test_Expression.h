// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

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
		EOperator_Spaceship,
		EOperator_LessThan,
		EOperator_LessThanEqual,
		EOperator_GreaterThan,
		EOperator_GreaterThanEqual,
		EOperator_Equal,
		EOperator_NotEqual,

		EOperator_Logical_Not,
		EOperator_Logical_And,
		EOperator_Logical_Or,

		EOperator_Unary_Plus,
		EOperator_Unary_Minus,
		EOperator_Unary_Indirection,

		EOperator_Unary_PostfixIncrement,
		EOperator_Unary_PostfixDecrement,
		EOperator_Unary_PrefixIncrement,
		EOperator_Unary_PrefixDecrement,

		EOperator_Arithmetic_Addition,
		EOperator_Arithmetic_Subtraction,
		EOperator_Arithmetic_Multiplication,
		EOperator_Arithmetic_Division,
		EOperator_Arithmetic_Modulu,

		EOperator_Bitwise_ShiftLeft,
		EOperator_Bitwise_ShiftRight,
		EOperator_Bitwise_Not,
		EOperator_Bitwise_And,
		EOperator_Bitwise_Or,
		EOperator_Bitwise_Xor,

	};

	using CLambdaUnderlying = int32;
	enum ELambda : int32
	{
		ELambda_None,
		ELambda_Normal,
		ELambda_NoEval,
	};

	namespace NPrivate
	{
		template <typename t_CType>
		bool fg_ConvertToBool(t_CType &&_Value)
			requires requires (t_CType &&_Value)
			{
				bool(_Value);
			}
		{
			return bool(_Value);
		}

		template <typename t_CType>
		bool fg_ConvertToBool(t_CType &&_Value)
		{
			return false;
		}

		template <typename t_CType>
		typename TCEnableIf<NTraits::TCIsSame<typename NTraits::TCRemoveReference<t_CType>::CType, bool>::mc_Value, bool>::CType fg_IsVariableBoolean(t_CType &&_Value)
		{
			return _Value == 1 || _Value == 0;
		}

		template <typename t_CType>
		typename TCDisableIf<NTraits::TCIsSame<typename NTraits::TCRemoveReference<t_CType>::CType, bool>::mc_Value, bool>::CType fg_IsVariableBoolean(t_CType &&_Value)
		{
			return false;
		}
		
		NStr::CStr fg_LineBreakDescLeft(NStr::CStr const &_Desc);
		NStr::CStr fg_LineBreakDescRight(NStr::CStr const &_Desc);
	}
	template <typename t_CLeft, typename t_CRight, COperatorUnderlying _Operator>
	class TCExpressionWithOperator;
	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	class TCExpression;


#define DImplementExpressionInterfaceNoEval(_ThisType) \
	public:\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Spaceship> operator <=> (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Spaceship>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Spaceship> operator <=> (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Spaceship>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_LessThan> operator < (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_LessThan>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_LessThan> operator < (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_LessThan>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_LessThanEqual> operator <= (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_LessThanEqual>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_LessThanEqual> operator <= (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_LessThanEqual>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_GreaterThan> operator > (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_GreaterThan>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_GreaterThan> operator > (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_GreaterThan>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_GreaterThanEqual> operator >= (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_GreaterThanEqual>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_GreaterThanEqual> operator >= (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_GreaterThanEqual>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Equal> operator == (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Equal>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Equal> operator == (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Equal>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_NotEqual> operator != (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_NotEqual>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_NotEqual> operator != (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_NotEqual>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Logical_And> operator && (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Logical_And>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Logical_And> operator && (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Logical_And>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Logical_Or> operator || (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Logical_Or>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Logical_Or> operator || (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Logical_Or>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Addition> operator + (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Addition>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Addition> operator + (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Addition>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Subtraction> operator - (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Subtraction>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Subtraction> operator - (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Subtraction>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Multiplication> operator * (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Multiplication>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Multiplication> operator * (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Multiplication>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Division> operator / (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Division>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Division> operator / (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Division>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Modulu> operator % (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Arithmetic_Modulu>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Modulu> operator % (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Arithmetic_Modulu>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_ShiftLeft> operator << (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_ShiftLeft>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_ShiftLeft> operator << (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_ShiftLeft>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_ShiftRight> operator >> (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_ShiftRight>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_ShiftRight> operator >> (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_ShiftRight>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_Xor> operator ^ (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_Xor>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_Xor> operator ^ (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_Xor>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_And> operator & (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_And>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_And> operator & (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_And>(*this, _Other);\
		}\
		template <typename t_CType2, CLambdaUnderlying t_bLambda2, typename t_CLambdaReturn2>\
		TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_Or> operator | (const TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpression<t_CType2, t_bLambda2, t_CLambdaReturn2>, EOperator_Bitwise_Or>(*this, _Other);\
		}\
		template <typename t_CLeft2, typename t_CRight2, COperatorUnderlying _Operator2>\
		TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_Or> operator | (const TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2> &_Other) const\
		{\
			return TCExpressionWithOperator<_ThisType, TCExpressionWithOperator<t_CLeft2, t_CRight2, _Operator2>, EOperator_Bitwise_Or>(*this, _Other);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Bitwise_Not> operator ~ () const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Bitwise_Not>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Logical_Not> operator ! () const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Logical_Not>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_Plus> operator + () const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_Plus>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_Minus> operator - () const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_Minus>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_Indirection> operator * () const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_Indirection>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PrefixIncrement> operator ++ () const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PrefixIncrement>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PrefixDecrement> operator -- () const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PrefixDecrement>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PostfixIncrement> operator ++ (int) const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PostfixIncrement>(*this);\
		}\
		TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PostfixDecrement> operator -- (int) const\
		{\
			return TCExpressionWithOperator<_ThisType, CEmpty, EOperator_Unary_PostfixDecrement>(*this);\
		}


#define DMibTemp_ImplementExpressionInterface(_ThisType) \
	public:\
		bool f_Eval(NStr::CStr *o_pValueDesc) const\
		{\
			return NPrivate::fg_ConvertToBool(f_GetVariable(o_pValueDesc, false));\
		}\
	DImplementExpressionInterfaceNoEval(_ThisType)
#define DMibTemp_ImplementBinaryOperator(_OperatorType, _OperatorDesc, _OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	class TCExpressionWithOperator<t_CLeft, t_CRight, _OperatorType>\
	{\
		static const t_CLeft &fsp_GetLeft();\
		static const t_CRight &fsp_GetRight();\
	public:\
		const t_CLeft &m_Left;\
		const t_CRight &m_Right;\
		TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right)	: m_Left(_Left), m_Right(_Right){}\
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + _OperatorDesc + m_Right.f_GetDescRecursive(); }\
		NStr::CStr f_GetDescRecursive() const { return "(" + m_Left.f_GetDescRecursive() + _OperatorDesc + m_Right.f_GetDescRecursive() + ")"; }\
		bool f_OnlyEvalOnce() const { return m_Left.f_OnlyEvalOnce() || m_Right.f_OnlyEvalOnce(); }\
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const -> decltype (fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) _OperatorCpp fsp_GetRight().f_GetVariable(o_pValueDesc, _bRecursive))\
		{\
			if (!o_pValueDesc) \
				return m_Left.f_GetVariable(nullptr, true) _OperatorCpp m_Right.f_GetVariable(nullptr, true);\
			NStr::CStr LeftDesc;\
			NStr::CStr RightDesc;\
			auto Return = m_Left.f_GetVariable(&LeftDesc, true) _OperatorCpp m_Right.f_GetVariable(&RightDesc, true);\
			NStr::CStr ThisValue;\
			if (!NPrivate::fg_IsVariableBoolean(Return))\
				ThisValue = NStr::CStr::fs_ToStr(Return);\
			if (_bRecursive)\
			{\
				if (ThisValue.f_IsEmpty())\
					*o_pValueDesc = "(" + NPrivate::fg_LineBreakDescLeft(LeftDesc) + _OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc) + ")"; \
				else\
					*o_pValueDesc = ThisValue + " [" + NPrivate::fg_LineBreakDescLeft(LeftDesc) + _OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc) + "]"; \
			}\
			else\
			{\
				if (ThisValue.f_IsEmpty())\
					*o_pValueDesc = NPrivate::fg_LineBreakDescLeft(LeftDesc) + _OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc); \
				else\
					*o_pValueDesc = ThisValue + " [" + NPrivate::fg_LineBreakDescLeft(LeftDesc) + _OperatorDesc + NPrivate::fg_LineBreakDescRight(RightDesc) + "]"; \
			}\
			return Return;\
		}\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

#define DMibTemp_ImplementUnaryOperator(_OperatorType, _OperatorDesc, _OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	class TCExpressionWithOperator<t_CLeft, t_CRight, _OperatorType>\
	{\
		static const t_CLeft &fsp_GetLeft();\
	public:\
		const t_CLeft &m_Left;\
		TCExpressionWithOperator(const t_CLeft &_Left)	: m_Left(_Left) {}\
		NStr::CStr f_GetDesc() const { return _OperatorDesc + m_Left.f_GetDescRecursive();	}\
		NStr::CStr f_GetDescRecursive() const { return _OperatorDesc + m_Left.f_GetDescRecursive();	}\
		bool f_OnlyEvalOnce() const { return m_Left.f_OnlyEvalOnce(); }\
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const -> decltype (_OperatorCpp fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive))\
		{\
			if (!o_pValueDesc)\
				return _OperatorCpp m_Left.f_GetVariable(nullptr, true);\
			NStr::CStr LeftDesc;\
			auto Return = _OperatorCpp m_Left.f_GetVariable(&LeftDesc, true);\
			*o_pValueDesc = _OperatorDesc + LeftDesc;\
			return Return;\
		}\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

#define DMibTemp_ImplementUnaryPostfixOperator(_OperatorType, _OperatorDesc, _OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	class TCExpressionWithOperator<t_CLeft, t_CRight, _OperatorType>\
	{\
		static const t_CLeft &fsp_GetLeft();\
	public:\
		const t_CLeft &m_Left;\
		TCExpressionWithOperator(const t_CLeft &_Left)	: m_Left(_Left) {}\
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + _OperatorDesc;}\
		NStr::CStr f_GetDescRecursive() const { return m_Left.f_GetDescRecursive() + _OperatorDesc;}\
		bool f_OnlyEvalOnce() const { return m_Left.f_OnlyEvalOnce(); }\
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const -> decltype (fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) _OperatorCpp)\
		{\
			if (!o_pValueDesc)\
				return m_Left.f_GetVariable(nullptr, true) _OperatorCpp;\
			NStr::CStr LeftDesc;\
			auto Return = m_Left.f_GetVariable(&LeftDesc, true) _OperatorCpp;\
			*o_pValueDesc = LeftDesc + _OperatorDesc;\
			return Return;\
		}\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

	template <typename t_CLeft, typename t_CRight>
	class TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>
	{
		static const t_CLeft &fsp_GetLeft();
		static const t_CRight &fsp_GetRight();
	public:
		const t_CLeft &m_Left;
		const t_CRight &m_Right;
		TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right)	: m_Left(_Left), m_Right(_Right){}
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + " && " + m_Right.f_GetDescRecursive(); }
		NStr::CStr f_GetDescRecursive() const { return "(" + m_Left.f_GetDescRecursive() + " && " + m_Right.f_GetDescRecursive() + ")"; }
		bool f_OnlyEvalOnce() const { return m_Left.f_OnlyEvalOnce() || m_Right.f_OnlyEvalOnce(); }
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const -> decltype (fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) && fsp_GetRight().f_GetVariable(o_pValueDesc, _bRecursive))
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

		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);
	};

	template <typename t_CLeft, typename t_CRight>
	class TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>
	{
		static const t_CLeft &fsp_GetLeft();
		static const t_CRight &fsp_GetRight();
	public:
		const t_CLeft &m_Left;
		const t_CRight &m_Right;
		TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right)	: m_Left(_Left), m_Right(_Right){}
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + " || " + m_Right.f_GetDescRecursive(); }
		NStr::CStr f_GetDescRecursive() const { return "(" + m_Left.f_GetDescRecursive() + " || " + m_Right.f_GetDescRecursive() + ")"; }
		bool f_OnlyEvalOnce() const { return m_Left.f_OnlyEvalOnce() || m_Right.f_OnlyEvalOnce(); }
		auto f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const -> decltype (fsp_GetLeft().f_GetVariable(o_pValueDesc, _bRecursive) || fsp_GetRight().f_GetVariable(o_pValueDesc, _bRecursive))
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
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);
	};

	DMibTemp_ImplementBinaryOperator(EOperator_Spaceship, " <=> ", <=>);
	DMibTemp_ImplementBinaryOperator(EOperator_LessThan, " < ", <);
	DMibTemp_ImplementBinaryOperator(EOperator_LessThanEqual, " <= ", <=);
	DMibTemp_ImplementBinaryOperator(EOperator_GreaterThan, " > ", >);
	DMibTemp_ImplementBinaryOperator(EOperator_GreaterThanEqual, " >= ", >=);
	DMibTemp_ImplementBinaryOperator(EOperator_Equal, " == ", ==);
	DMibTemp_ImplementBinaryOperator(EOperator_NotEqual, " != ", !=);
//		DMibTemp_ImplementBinaryOperator(EOperator_Logical_And, " && ", &&);
//		DMibTemp_ImplementBinaryOperator(EOperator_Logical_Or, " || ", ||);
	DMibTemp_ImplementBinaryOperator(EOperator_Arithmetic_Addition, " + ", +);
	DMibTemp_ImplementBinaryOperator(EOperator_Arithmetic_Subtraction, " - ", -);
	DMibTemp_ImplementBinaryOperator(EOperator_Arithmetic_Multiplication, " * ", *);
	DMibTemp_ImplementBinaryOperator(EOperator_Arithmetic_Division, " / ", /);
	DMibTemp_ImplementBinaryOperator(EOperator_Arithmetic_Modulu, " % ", %);
	DMibTemp_ImplementBinaryOperator(EOperator_Bitwise_ShiftLeft, " << ", <<);
	DMibTemp_ImplementBinaryOperator(EOperator_Bitwise_ShiftRight, " >> ", >>);
	DMibTemp_ImplementBinaryOperator(EOperator_Bitwise_And, " & ", &);
	DMibTemp_ImplementBinaryOperator(EOperator_Bitwise_Or, " | ", |);
	DMibTemp_ImplementBinaryOperator(EOperator_Bitwise_Xor, " ^ ", ^);

	DMibTemp_ImplementUnaryOperator(EOperator_Logical_Not, "!", !);
	DMibTemp_ImplementUnaryOperator(EOperator_Unary_Plus, "+", +);
	DMibTemp_ImplementUnaryOperator(EOperator_Unary_Minus, "-", -);
	DMibTemp_ImplementUnaryOperator(EOperator_Bitwise_Not, "~", ~);

	DMibTemp_ImplementUnaryOperator(EOperator_Unary_Indirection, "*", *);
	DMibTemp_ImplementUnaryOperator(EOperator_Unary_PrefixIncrement, "++", ++);
	DMibTemp_ImplementUnaryOperator(EOperator_Unary_PrefixDecrement, "--", --);

	DMibTemp_ImplementUnaryPostfixOperator(EOperator_Unary_PostfixIncrement, "++", ++);
	DMibTemp_ImplementUnaryPostfixOperator(EOperator_Unary_PostfixDecrement, "--", --);


	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn = void>
	class TCExpression
	{
		t_CType m_Lambda;
		const ch8 *m_pDescription;
	public:

		bool f_Eval(NStr::CStr *o_pValueDesc) const
		{
			DMibFastCheck(o_pValueDesc); // Should only be called once

			typedef decltype (m_Lambda()) CReturnType;
			if constexpr (NTraits::TCIsVoid<CReturnType>::mc_Value)
			{
				*o_pValueDesc = "void";
				return false;
			}
			else
			{
				auto Value = m_Lambda();
				*o_pValueDesc = NStr::CStr::fs_ToStr(Value);
				return NPrivate::fg_ConvertToBool(Value);
			}
		}

		NStr::CStr f_GetDesc() const
		{
			return m_pDescription;
		}

		NStr::CStr f_GetDescRecursive() const
		{
			return m_pDescription;
		}

		bool f_OnlyEvalOnce() const
		{
			return true;
		}

		NStr::CStr f_GetValueDescRecursive() const
		{
			typedef decltype (m_Lambda()) CReturnType;
			return fs_ValueDesc<CReturnType>(m_Lambda);
		}

		t_CLambdaReturn f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
		{
			DMibFastCheck(o_pValueDesc);

			typedef decltype (m_Lambda()) CReturnType;
			if constexpr (NTraits::TCIsVoid<CReturnType>::mc_Value)
			{
				*o_pValueDesc = "void";
				return false;
			}
			else
			{
				auto Return = m_Lambda();
				*o_pValueDesc = NStr::CStr::fs_ToStr(Return);
				return Return;
			}
		}

		TCExpression(t_CType _Lambda, const ch8 *_pDescription)
			: m_pDescription(_pDescription)
			, m_Lambda(_Lambda)
		{
		}
		DImplementExpressionInterfaceNoEval(TCExpression);
	};

	template <typename t_CType, typename t_CLambdaReturn>
	class TCExpression<t_CType, ELambda_NoEval, t_CLambdaReturn>
	{
		t_CType m_Lambda;
		const ch8 *m_pDescription;
	public:

		bool f_Eval(NStr::CStr *o_pValueDesc) const
		{
			return false;
		}

		NStr::CStr f_GetDesc() const
		{
			return m_pDescription;
		}

		NStr::CStr f_GetDescRecursive() const
		{
			return m_pDescription;
		}

		bool f_OnlyEvalOnce() const
		{
			return true;
		}

		t_CType f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
		{
			DMibFastCheck(o_pValueDesc);
			NTest::NPrivate::fg_RegisterDynamicValue(o_pValueDesc);
			return m_Lambda;
		}

		TCExpression(t_CType _Lambda, const ch8 *_pDescription)
			: m_pDescription(_pDescription)
			, m_Lambda(_Lambda)
		{
		}
		DImplementExpressionInterfaceNoEval(TCExpression);
	};

	template <typename t_CType, typename t_CLambdaReturn>
	class TCExpression<t_CType, ELambda_None, t_CLambdaReturn>
	{
		t_CType m_Variable;
		const ch8 *m_pDescription;

	public:
		NStr::CStr f_GetDesc() const
		{
			return m_pDescription;
		}

		NStr::CStr f_GetDescRecursive() const
		{
			return m_pDescription;
		}

		bool f_OnlyEvalOnce() const
		{
			return false;
		}

		t_CType const &f_GetVariable(NStr::CStr *o_pValueDesc, bool _bRecursive) const
		{
			if (o_pValueDesc)
				*o_pValueDesc = NStr::CStr::fs_ToStr(m_Variable);
			return m_Variable;
		}

		template <typename tf_CType>
		TCExpression(tf_CType &&_Variable, const ch8 *_pDescription)
			: m_pDescription(_pDescription)
			, m_Variable(fg_Forward<tf_CType>(_Variable))
		{
		}

		DMibTemp_ImplementExpressionInterface(TCExpression);
	};

#undef DMibTemp_ImplementExpressionInterface
#undef DMibTemp_ImplementBinaryOperator
#undef DMibTemp_ImplementUnaryOperator


	class CExpression
	{
		class CExpressionInternal
		{
		public:
			virtual ~CExpressionInternal()
			{
			}
			virtual bool f_Eval(NStr::CStr *o_pValueDesc) const = 0;
			virtual NStr::CStr f_GetDesc() const = 0;
			virtual NStr::CStr f_GetDescRecursive() const = 0;
			virtual NStr::CStr f_GetValueDescRecursive() const = 0;
			virtual bool f_OnlyEvalOnce() const = 0;
		};

		template <typename t_CImpl>
		class TCExpressionImpl : public CExpressionInternal
		{
			t_CImpl const &m_Impl;
		public:
			TCExpressionImpl(t_CImpl const &_Impl)
				: m_Impl(_Impl)
			{
			}
			bool f_Eval(NStr::CStr *o_pValueDesc) const override
			{
				return m_Impl.f_Eval(o_pValueDesc);
			}
			bool f_OnlyEvalOnce() const override
			{
				return m_Impl.f_OnlyEvalOnce();
			}
			NStr::CStr f_GetDesc() const override
			{
				return m_Impl.f_GetDesc();
			}
			NStr::CStr f_GetDescRecursive() const override
			{
				return m_Impl.f_GetDescRecursive();
			}
			NStr::CStr f_GetValueDescRecursive() const override
			{
				return m_Impl.f_GetValueDescRecursive();
			}
		};

		CExpressionInternal *m_pExpression;
	public:

		template <typename t_CType, CLambdaUnderlying t_bLambda>
		CExpression(TCExpression<t_CType, t_bLambda> const&_Expression)
		{
			m_pExpression = DMibNew TCExpressionImpl<TCExpression<t_CType, t_bLambda>>(_Expression);
		}

		template <typename t_CLeft, typename t_CRight, COperatorUnderlying t_Operator>
		CExpression(TCExpressionWithOperator<t_CLeft, t_CRight, t_Operator> const&_Expression)
		{
			m_pExpression = DMibNew TCExpressionImpl<TCExpressionWithOperator<t_CLeft, t_CRight, t_Operator>>(_Expression);
		}

		~CExpression()
		{
			fg_Delete(m_pExpression);
		}

		NStr::CStr f_GetDesc() const
		{
			return m_pExpression->f_GetDesc();
		}

		NStr::CStr f_GetDescRecursive() const
		{
			return m_pExpression->f_GetDescRecursive();
		}

		NStr::CStr f_GetValueDescRecursive() const
		{
			return m_pExpression->f_GetValueDescRecursive();
		}

		bool f_Eval(NStr::CStr *o_pValueDesc) const
		{
			return m_pExpression->f_Eval(o_pValueDesc);
		}

		bool f_OnlyEvalOnce() const
		{
			return m_pExpression->f_OnlyEvalOnce();
		}
	};

	template <typename t_CType>
	inline auto fg_GetExpression(t_CType &&_Type, const ch8 *_pDescription) -> TCExpression<t_CType, ELambda_None>
	{
		return TCExpression<t_CType, ELambda_None>(fg_Forward<t_CType>(_Type), _pDescription);
	}

	template <typename t_CType>
	inline_small auto fg_GetLambdaExpression(t_CType _Type, const ch8 *_pDescription) -> TCExpression<t_CType, ELambda_Normal, decltype(_Type())>
	{
		return TCExpression<t_CType, ELambda_Normal, decltype(_Type())>(_Type, _pDescription);
	}

	template <typename t_CType>
	inline_small auto fg_GetLambdaExpressionNoEval(t_CType _Type, const ch8 *_pDescription) -> TCExpression<t_CType, ELambda_NoEval, decltype(_Type())>
	{
		return TCExpression<t_CType, ELambda_NoEval, decltype(_Type())>(_Type, _pDescription);
	}

	/*template <typename t_CType>
	TCExpression<t_CType> fg_GetExpression(const t_CType &_Type, const NStr::CStr &_Description)
	{
		return TCExpression<t_CType>(_Type, _Description);
	}*/

#	define DMibExpr(_Expression) NMib::NTest::NExpression::fg_GetExpression(_Expression, DMibStringizeUTF8(_Expression))
#	define DMibELExpr(_Expression) NMib::NTest::NExpression::fg_GetLambdaExpression([&]() {return _Expression;}, DMibStringizeUTF8(_Expression))
#	define DMibLExpr(_Expression) NMib::NTest::NExpression::fg_GetLambdaExpressionNoEval([&]() {return _Expression;}, DMibStringizeUTF8(_Expression))

#	ifndef DMibPNoShortCuts
#		define DExpr DMibExpr
#		define DELExpr DMibELExpr
#		define DLExpr DMibLExpr
#	endif
}

#ifndef DMibPNoShortCuts
	using namespace NMib::NTest::NExpression;
#endif

