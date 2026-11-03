// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "Malterlib_Test_Expression.h"

namespace NMib::NTest::NExpression::NPrivate
{
	NStr::CStr fg_LineBreakDescLeft(NStr::CStr const &_Desc)
	{
		ch8 const *pParse = _Desc.f_GetStr();

		while (*pParse)
		{
			NStr::fg_ParseToEndOfLine(pParse);
			if (NStr::fg_ParseEndOfLine(pParse))
				return _Desc + "\n";
		}

		return _Desc;
	}

	NStr::CStr fg_LineBreakDescRight(NStr::CStr const &_Desc)
	{
		ch8 const *pParse = _Desc.f_GetStr();

		while (*pParse)
		{
			NStr::fg_ParseToEndOfLine(pParse);
			if (NStr::fg_ParseEndOfLine(pParse))
				return "\n" + _Desc;
		}

		return _Desc;
	}
}

namespace NMib::NTest::NExpression
{
	CExpression::~CExpression()
	{
		fg_Delete(mp_pExpression);
	}

	NStr::CStr CExpression::f_GetDesc() const
	{
		return mp_pExpression->f_GetDesc();
	}

	NStr::CStr CExpression::f_GetDescRecursive() const
	{
		return mp_pExpression->f_GetDescRecursive();
	}

	NStr::CStr CExpression::f_GetValueDescRecursive() const
	{
		return mp_pExpression->f_GetValueDescRecursive();
	}

	bool CExpression::f_Eval(NStr::CStr *o_pValueDesc) const
	{
		return mp_pExpression->f_Eval(o_pValueDesc);
	}

	bool CExpression::f_OnlyEvalOnce() const
	{
		return mp_pExpression->f_OnlyEvalOnce();
	}

	CExpression::CExpressionInternal::~CExpressionInternal()
	{
	}
}
