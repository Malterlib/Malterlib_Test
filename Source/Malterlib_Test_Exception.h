// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Test_Exception_Private.h"

namespace NMib::NTest
{
	template <typename t_CException0 = NPrivate::CExceptionDummyTag0, typename t_CException1 = NPrivate::CExceptionDummyTag1, typename t_CException2 = NPrivate::CExceptionDummyTag2, typename t_CException3 = NPrivate::CExceptionDummyTag3, typename t_CException4 = NPrivate::CExceptionDummyTag4, typename t_CException5 = NPrivate::CExceptionDummyTag5, typename t_CException6 = NPrivate::CExceptionDummyTag6, typename t_CException7 = NPrivate::CExceptionDummyTag7>
	class TCThrowsException
	{
	public:
		template <typename t_CObject>
		bint operator == (t_CObject const &_FunctionObject) const
		{
#if DMibConfig_Tests_Enable
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
#endif
			try
			{
				_FunctionObject();
			}
			catch (t_CException0 const &)
			{
				return true;
			}
			catch (t_CException1 const &)
			{
				return true;
			}
			catch (t_CException2 const &)
			{
				return true;
			}
			catch (t_CException3 const &)
			{
				return true;
			}
			catch (t_CException4 const &)
			{
				return true;
			}
			catch (t_CException5 const &)
			{
				return true;
			}
			catch (t_CException6 const &)
			{
				return true;
			}
			catch (t_CException7 const &)
			{
				return true;
			}
			return NTraits::TCIsSame<t_CException0, NPrivate::CExceptionDummyTag0>::mc_Value;
		}

		template <typename t_CFormatter>
		int f_GetStringFormatType(t_CFormatter &_Formatter);

		template <typename t_CFormatter>
		// This crashes GCC
		// auto f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, ""))
		NStr::CStrFormatTypeClassifier_String f_CreateStringFormatter(t_CFormatter &_Formatter) const
		{
			static const ch8 * pRet = "NoEval";
			return NStr::fg_CreateStringFormatter(_Formatter, pRet);
		}
	};

	template <typename t_CException0, typename t_CException1 = NPrivate::CExceptionDummyTag1 const &, typename t_CException2 = NPrivate::CExceptionDummyTag2 const &, typename t_CException3 = NPrivate::CExceptionDummyTag3 const &, typename t_CException4 = NPrivate::CExceptionDummyTag4 const &, typename t_CException5 = NPrivate::CExceptionDummyTag5 const &, typename t_CException6 = NPrivate::CExceptionDummyTag6 const &, typename t_CException7 = NPrivate::CExceptionDummyTag7 const &>
	class TCThrowsExceptionExact
	{
		t_CException0 m_Value0;
		t_CException1 m_Value1;
		t_CException2 m_Value2;
		t_CException3 m_Value3;
		t_CException4 m_Value4;
		t_CException5 m_Value5;
		t_CException6 m_Value6;
		t_CException7 m_Value7;
	public:

		TCThrowsExceptionExact(t_CException0 const &_Value0
			, t_CException1 _Value1 = NPrivate::CExceptionDummyTag1()
			, t_CException2 _Value2 = NPrivate::CExceptionDummyTag2()
			, t_CException3 _Value3 = NPrivate::CExceptionDummyTag3()
			, t_CException4 _Value4 = NPrivate::CExceptionDummyTag4()
			, t_CException5 _Value5 = NPrivate::CExceptionDummyTag5()
			, t_CException6 _Value6 = NPrivate::CExceptionDummyTag6()
			, t_CException7 _Value7 = NPrivate::CExceptionDummyTag7()
			)
			: m_Value0(_Value0)
			, m_Value1(_Value1)
			, m_Value2(_Value2)
			, m_Value3(_Value3)
			, m_Value4(_Value4)
			, m_Value5(_Value5)
			, m_Value6(_Value6)
			, m_Value7(_Value7)
		{
		}

		template <typename t_CObject>
		bint operator == (t_CObject const &_FunctionObject) const
		{
#if DMibConfig_Tests_Enable
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
#endif
			try
			{
				_FunctionObject();
			}
			catch (t_CException0 const &_Exception)
			{
				return _Exception == m_Value0;
			}
			catch (t_CException1 const &_Exception)
			{
				return _Exception == m_Value1;
			}
			catch (t_CException2 const &_Exception)
			{
				return _Exception == m_Value2;
			}
			catch (t_CException3 const &_Exception)
			{
				return _Exception == m_Value3;
			}
			catch (t_CException4 const &_Exception)
			{
				return _Exception == m_Value4;
			}
			catch (t_CException5 const &_Exception)
			{
				return _Exception == m_Value5;
			}
			catch (t_CException6 const &_Exception)
			{
				return _Exception == m_Value6;
			}
			catch (t_CException7 const &_Exception)
			{
				return _Exception == m_Value7;
			}
			return false;
		}

		template <typename t_CFormatter>
		int f_GetStringFormatType(t_CFormatter &_Formatter);

		template <typename t_CFormatter>
		// This crashes GCC
		// auto f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, ""))
		NStr::CStrFormatTypeClassifier_String f_CreateStringFormatter(t_CFormatter &_Formatter) const
		{
			static const ch8 * pRet = "NoEval";
			return NStr::fg_CreateStringFormatter(_Formatter, pRet);
		}
	};

	template <typename t_CException0, typename t_CException1, typename t_CException2, typename t_CException3, typename t_CException4, typename t_CException5, typename t_CException6, typename t_CException7>
	TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &, t_CException5 const &, t_CException6 const &, t_CException7 const &> fg_ThrowsException(t_CException0 const &_Value0, t_CException1 const &_Value1, t_CException2 const &_Value2, t_CException3 const &_Value3, t_CException4 const &_Value4, t_CException5 const &_Value5, t_CException6 const &_Value6, t_CException7 const &_Value7)
	{
		return TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &, t_CException5 const &, t_CException6 const &, t_CException7 const &>(_Value0, _Value1, _Value2, _Value3, _Value4, _Value5, _Value6, _Value7);
	}

	template <typename t_CException0, typename t_CException1, typename t_CException2, typename t_CException3, typename t_CException4, typename t_CException5, typename t_CException6>
	TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &, t_CException5 const &, t_CException6 const &> fg_ThrowsException(t_CException0 const &_Value0, t_CException1 const &_Value1, t_CException2 const &_Value2, t_CException3 const &_Value3, t_CException4 const &_Value4, t_CException5 const &_Value5, t_CException6 const &_Value6)
	{
		return TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &, t_CException5 const &, t_CException6 const &>(_Value0, _Value1, _Value2, _Value3, _Value4, _Value5, _Value6);
	}

	template <typename t_CException0, typename t_CException1, typename t_CException2, typename t_CException3, typename t_CException4, typename t_CException5>
	TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &, t_CException5 const &> fg_ThrowsException(t_CException0 const &_Value0, t_CException1 const &_Value1, t_CException2 const &_Value2, t_CException3 const &_Value3, t_CException4 const &_Value4, t_CException5 const &_Value5)
	{
		return TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &, t_CException5 const &>(_Value0, _Value1, _Value2, _Value3, _Value4, _Value5);
	}

	template <typename t_CException0, typename t_CException1, typename t_CException2, typename t_CException3, typename t_CException4>
	TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &> fg_ThrowsException(t_CException0 const &_Value0, t_CException1 const &_Value1, t_CException2 const &_Value2, t_CException3 const &_Value3, t_CException4 const &_Value4)
	{
		return TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &, t_CException4 const &>(_Value0, _Value1, _Value2, _Value3, _Value4);
	}

	template <typename t_CException0, typename t_CException1, typename t_CException2, typename t_CException3>
	TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &> fg_ThrowsException(t_CException0 const &_Value0, t_CException1 const &_Value1, t_CException2 const &_Value2, t_CException3 const &_Value3)
	{
		return TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &, t_CException3 const &>(_Value0, _Value1, _Value2, _Value3);
	}

	template <typename t_CException0, typename t_CException1, typename t_CException2>
	TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &> fg_ThrowsException(t_CException0 const &_Value0, t_CException1 const &_Value1, t_CException2 const &_Value2)
	{
		return TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &, t_CException2 const &>(_Value0, _Value1, _Value2);
	}

	template <typename t_CException0, typename t_CException1>
	TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &> fg_ThrowsException(t_CException0 const &_Value0, t_CException1 const &_Value1)
	{
		return TCThrowsExceptionExact<t_CException0 const &, t_CException1 const &>(_Value0, _Value1);
	}

	template <typename t_CException0>
	TCThrowsExceptionExact<t_CException0 const &> fg_ThrowsException(t_CException0 const &_Value0)
	{
		return TCThrowsExceptionExact<t_CException0 const &>(_Value0);
	}

#if defined DMibContractConfigure_RequireEnabled
	TCThrowsException<NContract::CContractException_Require> fg_ViolatesRequire();
	TCThrowsExceptionExact<NContract::CContractException_Require> fg_ViolatesRequire(const ch8 *_pError);
#else
	TCThrowsException<> fg_ViolatesRequire();
	TCThrowsException<> fg_ViolatesRequire(const ch8 *_pError);
#endif

#if DMibEnableSafeCheck > 0
	TCThrowsException<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck();
	TCThrowsExceptionExact<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck(const ch8 *_pError);
#else
	TCThrowsException<> fg_ViolatesSafeCheck();
	TCThrowsException<> fg_ViolatesSafeCheck(const ch8 *_pError);
#endif
}
