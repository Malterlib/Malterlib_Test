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
