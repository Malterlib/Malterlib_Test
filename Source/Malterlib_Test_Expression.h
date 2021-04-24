// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTest::NPrivate
{
	NStr::CStr const &fg_GetDynamicValue();
}

namespace NMib::NTest::NExpression
{
	using COperatorUnderlying = int32;
	enum EOperator : int32
	{
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
		DMibPrivateTypeTraitsImplement_MemberTraitsWithNameTraits(Operator, Bool, operator bool, 0, (), , bool, ;, CBaseMixin::*, Normal)
		DMibPrivateTypeTraitsImplement_MemberTraitsWithNameHas(Operator, Bool, operator bool, 0, (), , ;, CBaseMixin::*, Normal)

		template <typename t_CType>
		typename TCEnableIf<NTraits::TCIsConvertible<t_CType, bool>::mc_Value || TCHasOperator_Bool<typename NTraits::TCRemoveReference<t_CType>::CType>::mc_Value, bool>::CType
		fg_ConvertToBool(t_CType &&_Value)
		{
			return bool(_Value);
		}

		template <typename t_CType>
		typename TCEnableIf<!(NTraits::TCIsConvertible<t_CType, bool>::mc_Value || TCHasOperator_Bool<typename NTraits::TCRemoveReference<t_CType>::CType>::mc_Value), bool>::CType
		fg_ConvertToBool(t_CType &&_Value)
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

	}
	template <typename t_CLeft, typename t_CRight, COperatorUnderlying _Operator>
	class TCExpressionWithOperator;
	template <typename t_CType, CLambdaUnderlying t_bLambda, typename t_CLambdaReturn>
	class TCExpression;


#define DImplementExpressionInterfaceNoEval(_ThisType) \
	public:\
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
		bool f_Eval() const\
		{\
			return NPrivate::fg_ConvertToBool(f_GetVariable());\
		}\
	DImplementExpressionInterfaceNoEval(_ThisType)
#define DMibTemp_ImplementBinaryOperator(_OperatorType, _OperatorDesc, _OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	class TCExpressionWithOperator<t_CLeft, t_CRight, _OperatorType>\
	{\
		static const t_CLeft &fsp_GetLeft();\
		static const t_CRight &fsp_GetRight();\
		NStr::CStr fp_GetThisValueDesc() const\
		{\
			if (NPrivate::fg_IsVariableBoolean(f_GetVariable()))\
				return NStr::CStr();\
			return NStr::CStr::fs_ToStr(f_GetVariable());\
		}\
	public:\
		const t_CLeft &m_Left;\
		const t_CRight &m_Right;\
		TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right)	: m_Left(_Left), m_Right(_Right){}\
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + _OperatorDesc + m_Right.f_GetDescRecursive(); }\
		NStr::CStr f_GetDescRecursive() const { return "(" + m_Left.f_GetDescRecursive() + _OperatorDesc + m_Right.f_GetDescRecursive() + ")"; }\
		NStr::CStr f_GetValueDesc() const \
		{ \
			NStr::CStr ThisValue = fp_GetThisValueDesc();\
			if (ThisValue.f_IsEmpty())\
				return m_Left.f_GetValueDescRecursive() + _OperatorDesc + m_Right.f_GetValueDescRecursive(); \
			else\
				return fp_GetThisValueDesc() + " ["+m_Left.f_GetValueDescRecursive() + _OperatorDesc + m_Right.f_GetValueDescRecursive() + "]"; \
		}\
		bool f_GetDisableValues() const \
		{ \
			return m_Left.f_GetDisableValues() || m_Right.f_GetDisableValues(); \
		}\
		NStr::CStr f_GetValueDescRecursive() const \
		{ \
			NStr::CStr ThisValue = fp_GetThisValueDesc();\
			if (ThisValue.f_IsEmpty())\
				return "(" + m_Left.f_GetValueDescRecursive() + _OperatorDesc + m_Right.f_GetValueDescRecursive() + ")"; \
			else\
				return fp_GetThisValueDesc() + " ["+m_Left.f_GetValueDescRecursive() + _OperatorDesc + m_Right.f_GetValueDescRecursive() + "]"; \
		}\
		auto f_GetVariable() -> decltype (fsp_GetLeft().f_GetVariable() _OperatorCpp fsp_GetRight().f_GetVariable()) {	return m_Left.f_GetVariable() _OperatorCpp m_Right.f_GetVariable(); }\
		auto f_GetVariable() const -> decltype (fsp_GetLeft().f_GetVariable() _OperatorCpp fsp_GetRight().f_GetVariable()) { return m_Left.f_GetVariable() _OperatorCpp m_Right.f_GetVariable(); }\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

#define DMibTemp_ImplementUnaryOperator(_OperatorType, _OperatorDesc, _OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	class TCExpressionWithOperator<t_CLeft, t_CRight, _OperatorType>\
	{\
		static const t_CLeft &fsp_GetLeft();\
		NStr::CStr fp_GetThisValueDesc() const\
		{\
			if (NPrivate::fg_IsVariableBoolean(f_GetVariable()))\
				return f_GetVariable() ? "true" : "false";\
			return NStr::CStr::fs_ToStr(f_GetVariable());\
		}\
	public:\
		const t_CLeft &m_Left;\
		TCExpressionWithOperator(const t_CLeft &_Left)	: m_Left(_Left) {}\
		NStr::CStr f_GetDesc() const { return _OperatorDesc + m_Left.f_GetDescRecursive();	}\
		NStr::CStr f_GetDescRecursive() const { return _OperatorDesc + m_Left.f_GetDescRecursive();	}\
		NStr::CStr f_GetValueDesc() const \
		{ \
			return _OperatorDesc + m_Left.f_GetValueDescRecursive(); \
		}\
		bool f_GetDisableValues() const \
		{ \
			return m_Left.f_GetDisableValues(); \
		}\
		NStr::CStr f_GetValueDescRecursive() const \
		{ \
			return _OperatorDesc + m_Left.f_GetValueDescRecursive(); \
		}\
		auto f_GetVariable() -> decltype (_OperatorCpp fsp_GetLeft().f_GetVariable()) {	return _OperatorCpp m_Left.f_GetVariable(); }\
		auto f_GetVariable() const -> decltype (_OperatorCpp fsp_GetLeft().f_GetVariable()) { return _OperatorCpp m_Left.f_GetVariable(); }\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

#define DMibTemp_ImplementUnaryPostfixOperator(_OperatorType, _OperatorDesc, _OperatorCpp)\
	template <typename t_CLeft, typename t_CRight>\
	class TCExpressionWithOperator<t_CLeft, t_CRight, _OperatorType>\
	{\
		static const t_CLeft &fsp_GetLeft();\
		NStr::CStr fp_GetThisValueDesc() const\
		{\
			if (NPrivate::fg_IsVariableBoolean(f_GetVariable()))\
				return f_GetVariable() ? "true" : "false";\
			return NStr::CStr::fs_ToStr(f_GetVariable());\
		}\
	public:\
		const t_CLeft &m_Left;\
		TCExpressionWithOperator(const t_CLeft &_Left)	: m_Left(_Left) {}\
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + _OperatorDesc;}\
		NStr::CStr f_GetDescRecursive() const { return m_Left.f_GetDescRecursive() + _OperatorDesc;}\
		NStr::CStr f_GetValueDesc() const \
		{ \
			return m_Left.f_GetValueDescRecursive() + _OperatorDesc; \
		}\
		bool f_GetDisableValues() const \
		{ \
			return m_Left.f_GetDisableValues(); \
		}\
		NStr::CStr f_GetValueDescRecursive() const \
		{ \
			return m_Left.f_GetValueDescRecursive() + _OperatorDesc; \
		}\
		auto f_GetVariable() -> decltype (fsp_GetLeft().f_GetVariable() _OperatorCpp) {	return m_Left.f_GetVariable() _OperatorCpp; }\
		auto f_GetVariable() const -> decltype (fsp_GetLeft().f_GetVariable() _OperatorCpp) { return m_Left.f_GetVariable() _OperatorCpp; }\
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);\
	};

	template <typename t_CLeft, typename t_CRight>
	class TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_And>
	{
		static const t_CLeft &fsp_GetLeft();
		static const t_CRight &fsp_GetRight();
		NStr::CStr fp_GetThisValueDesc() const
		{
			if (NPrivate::fg_IsVariableBoolean(f_GetVariable()))
				return NStr::CStr();
			return NStr::CStr::fs_ToStr(f_GetVariable());
		}
	public:
		const t_CLeft &m_Left;
		const t_CRight &m_Right;
		TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right)	: m_Left(_Left), m_Right(_Right){}
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + " && " + m_Right.f_GetDescRecursive(); }
		NStr::CStr f_GetDescRecursive() const { return "(" + m_Left.f_GetDescRecursive() + " && " + m_Right.f_GetDescRecursive() + ")"; }
		NStr::CStr f_GetValueDesc() const
		{
			NStr::CStr ThisValue = fp_GetThisValueDesc();
			if (ThisValue.f_IsEmpty())
			{
				if (m_Left.f_GetVariable())
					return m_Left.f_GetValueDescRecursive() + " && " + m_Right.f_GetValueDescRecursive();
				else
					return m_Left.f_GetValueDescRecursive() + " && NoEval";
			}
			else
			{
				if (m_Left.f_GetVariable())
					return fp_GetThisValueDesc() + " ["+m_Left.f_GetValueDescRecursive() + " && " + m_Right.f_GetValueDescRecursive() + "]";
				else
					return fp_GetThisValueDesc() + " ["+m_Left.f_GetValueDescRecursive() + " && NoEval]";
			}
		}
		bool f_GetDisableValues() const
		{
			return m_Left.f_GetDisableValues() || m_Right.f_GetDisableValues();
		}
		NStr::CStr f_GetValueDescRecursive() const
		{
			NStr::CStr ThisValue = fp_GetThisValueDesc();
			if (ThisValue.f_IsEmpty())
			{
				if (m_Left.f_GetVariable())
					return "(" + m_Left.f_GetValueDescRecursive() + " && " + m_Right.f_GetValueDescRecursive() + ")";
				else
					return "(" + m_Left.f_GetValueDescRecursive() + " && NoEval)";
			}
			else
			{
				if (m_Left.f_GetVariable())
					return fp_GetThisValueDesc() + " ["+m_Left.f_GetValueDescRecursive() + " && " + m_Right.f_GetValueDescRecursive() + "]";
				else
					return fp_GetThisValueDesc() + " ["+m_Left.f_GetValueDescRecursive() + " && NoEval]";
			}
		}
		auto f_GetVariable() -> decltype (fsp_GetLeft().f_GetVariable() && fsp_GetRight().f_GetVariable()) {	return m_Left.f_GetVariable() && m_Right.f_GetVariable(); }
		auto f_GetVariable() const -> decltype (fsp_GetLeft().f_GetVariable() && fsp_GetRight().f_GetVariable()) { return m_Left.f_GetVariable() && m_Right.f_GetVariable(); }
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);
	};

	template <typename t_CLeft, typename t_CRight>
	class TCExpressionWithOperator<t_CLeft, t_CRight, EOperator_Logical_Or>
	{
		static const t_CLeft &fsp_GetLeft();
		static const t_CRight &fsp_GetRight();
		NStr::CStr fp_GetThisValueDesc() const
		{
			if (NPrivate::fg_IsVariableBoolean(f_GetVariable()))
				return NStr::CStr();
			return NStr::CStr::fs_ToStr(f_GetVariable());
		}
	public:
		const t_CLeft &m_Left;
		const t_CRight &m_Right;
		TCExpressionWithOperator(const t_CLeft &_Left, const t_CRight &_Right)	: m_Left(_Left), m_Right(_Right){}
		NStr::CStr f_GetDesc() const { return m_Left.f_GetDescRecursive() + " || " + m_Right.f_GetDescRecursive(); }
		NStr::CStr f_GetDescRecursive() const { return "(" + m_Left.f_GetDescRecursive() + " || " + m_Right.f_GetDescRecursive() + ")"; }
		NStr::CStr f_GetValueDesc() const
		{
			NStr::CStr ThisValue = fp_GetThisValueDesc();
			if (ThisValue.f_IsEmpty())
			{
				if (!m_Left.f_GetVariable())
					return m_Left.f_GetValueDescRecursive() + " || " + m_Right.f_GetValueDescRecursive();
				else
					return m_Left.f_GetValueDescRecursive() + " || NoEval";
			}
			else
			{
				if (!m_Left.f_GetVariable())
					return fp_GetThisValueDesc() + " ("+m_Left.f_GetValueDescRecursive() + " || " + m_Right.f_GetValueDescRecursive() + ")";
				else
					return fp_GetThisValueDesc() + " ("+m_Left.f_GetValueDescRecursive() + " || NoEval)";
			}
		}
		bool f_GetDisableValues() const
		{
			return m_Left.f_GetDisableValues() || m_Right.f_GetDisableValues();
		}
		NStr::CStr f_GetValueDescRecursive() const
		{
			NStr::CStr ThisValue = fp_GetThisValueDesc();
			if (ThisValue.f_IsEmpty())
			{
				if (!m_Left.f_GetVariable())
					return "(" + m_Left.f_GetValueDescRecursive() + " || " + m_Right.f_GetValueDescRecursive() + ")";
				else
					return "(" + m_Left.f_GetValueDescRecursive() + " || NoEval)";
			}
			else
			{
				if (!m_Left.f_GetVariable())
					return fp_GetThisValueDesc() + " ("+m_Left.f_GetValueDescRecursive() + " || " + m_Right.f_GetValueDescRecursive() + ")";
				else
					return fp_GetThisValueDesc() + " ("+m_Left.f_GetValueDescRecursive() + " || NoEval)";
			}
		}
		auto f_GetVariable() -> decltype (fsp_GetLeft().f_GetVariable() || fsp_GetRight().f_GetVariable()) {	return m_Left.f_GetVariable() || m_Right.f_GetVariable(); }
		auto f_GetVariable() const -> decltype (fsp_GetLeft().f_GetVariable() || fsp_GetRight().f_GetVariable()) { return m_Left.f_GetVariable() || m_Right.f_GetVariable(); }
		DMibTemp_ImplementExpressionInterface(TCExpressionWithOperator);
	};

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

		static t_CType &fs_GetLambda();

//			typedef decltype(fs_GetLamba()()) CReturnType;

		template <typename t_CReturnType, typename t_CLambda>
		static typename TCEnableIf<NTraits::TCIsVoid<t_CReturnType>::mc_Value, bool>::CType fs_ConvertToBool(t_CLambda _Lambda)
		{
			_Lambda();
			return false;
		}

		template <typename t_CReturnType, typename t_CLambda>
		static typename TCEnableIf<!NTraits::TCIsVoid<t_CReturnType>::mc_Value, bool>::CType fs_ConvertToBool(t_CLambda _Lambda)
		{
			return NPrivate::fg_ConvertToBool(_Lambda());
		}

		template <typename t_CReturnType, typename t_CLambda>
		static typename TCEnableIf<NTraits::TCIsVoid<t_CReturnType>::mc_Value, NStr::CStr>::CType fs_ValueDesc(t_CLambda _Lambda)
		{
			_Lambda();
			return "void";
		}

		template <typename t_CReturnType, typename t_CLambda>
		static typename TCEnableIf<!NTraits::TCIsVoid<t_CReturnType>::mc_Value, NStr::CStr>::CType fs_ValueDesc(t_CLambda _Lambda)
		{
			return NStr::CStr::fs_ToStr(_Lambda());
		}

	public:

		bool f_Eval() const
		{
			typedef decltype (m_Lambda()) CReturnType;
			return fs_ConvertToBool<CReturnType>(m_Lambda);
		}

		NStr::CStr f_GetDesc() const
		{
			return m_pDescription;
		}

		NStr::CStr f_GetDescRecursive() const
		{
			return m_pDescription;
		}

		NStr::CStr f_GetValueDesc() const
		{
			typedef decltype (m_Lambda()) CReturnType;
			return fs_ValueDesc<CReturnType>(m_Lambda);
		}

		bool f_GetDisableValues() const
		{
			return true;
		}

		NStr::CStr f_GetValueDescRecursive() const
		{
			typedef decltype (m_Lambda()) CReturnType;
			return fs_ValueDesc<CReturnType>(m_Lambda);
		}

		t_CLambdaReturn f_GetVariable() const
		{
			return m_Lambda();
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

		bool f_Eval() const
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

		NStr::CStr f_GetValueDesc() const
		{
			return NTest::NPrivate::fg_GetDynamicValue();
		}

		bool f_GetDisableValues() const
		{
			// This should be kept so exceptions are shown correctly
			return NTest::NPrivate::fg_GetDynamicValue().f_IsEmpty();
		}

		NStr::CStr f_GetValueDescRecursive() const
		{
			return NTest::NPrivate::fg_GetDynamicValue();
		}

		t_CType f_GetVariable() const
		{
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

		NStr::CStr f_GetValueDesc() const
		{
			return NStr::CStr::fs_ToStr(f_GetVariable());
		}

		bool f_GetDisableValues() const
		{
			return false;
		}

		NStr::CStr f_GetValueDescRecursive() const
		{
			return NStr::CStr::fs_ToStr(f_GetVariable());
		}

		t_CType const &f_GetVariable() const
		{
			return m_Variable;
		}

		TCExpression(t_CType const &_Variable, const ch8 *_pDescription)
			: m_pDescription(_pDescription)
			, m_Variable(_Variable)
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
			virtual bool f_Eval() const = 0;
			virtual NStr::CStr f_GetDesc() const = 0;
			virtual NStr::CStr f_GetDescRecursive() const = 0;
			virtual NStr::CStr f_GetValueDesc() const = 0;
			virtual bool f_GetDisableValues() const = 0;
			virtual NStr::CStr f_GetValueDescRecursive() const = 0;
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
			virtual bool f_Eval() const
			{
				return m_Impl.f_Eval();
			}
			virtual NStr::CStr f_GetDesc() const
			{
				return m_Impl.f_GetDesc();
			}
			virtual NStr::CStr f_GetDescRecursive() const
			{
				return m_Impl.f_GetDescRecursive();
			}
			virtual NStr::CStr f_GetValueDesc() const
			{
				return m_Impl.f_GetValueDesc();
			}
			virtual bool f_GetDisableValues() const
			{
				return m_Impl.f_GetDisableValues();
			}
			virtual NStr::CStr f_GetValueDescRecursive() const
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

		NStr::CStr f_GetValueDesc() const
		{
			return m_pExpression->f_GetValueDesc();
		}

		bool f_GetDisabelValues() const
		{
			return m_pExpression->f_GetDisableValues();
		}

		NStr::CStr f_GetValueDescRecursive() const
		{
			return m_pExpression->f_GetValueDescRecursive();
		}

		bool f_Eval() const
		{
			return m_pExpression->f_Eval();
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

