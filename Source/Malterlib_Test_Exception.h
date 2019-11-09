// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Test.h"

namespace NMib::NTest
{
	template <bool t_bOuter, typename ...tp_CExceptions>
	struct TCThrowsExceptionImpl;

	template <bool t_bOuter>
	struct TCThrowsExceptionImpl<t_bOuter>
	{
		template <typename t_FFunctor>
		bool operator == (t_FFunctor const &_fFunctor) const
		{
#if DMibConfig_Tests_Enable
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
#endif
			try
			{
				NPrivate::fg_SetDynamicValue({});
				_fFunctor();
				NPrivate::fg_SetDynamicValue("No exception thrown");
			}
			catch (NException::CExceptionBase const &_Exception)
			{
				NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
				return false;
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const
		{
			o_String += typename tf_CString::CFormat("noexcept");
		}
	};

	template <bool t_bOuter, typename t_CException>
	struct TCThrowsExceptionImpl<t_bOuter, t_CException>
	{
		template <typename t_FFunctor>
		bool operator == (t_FFunctor const &_fFunctor) const
		{
#if DMibConfig_Tests_Enable
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
#endif
			if constexpr(t_bOuter)
			{
				try
				{
					try
					{
						NPrivate::fg_SetDynamicValue({});
						_fFunctor();
						NPrivate::fg_SetDynamicValue("No exception thrown");
					}
					catch (t_CException const &_Exception)
					{
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
						return true;
					}
				}
				catch (NException::CExceptionBase const &_Exception)
				{
					NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
					return false;
				}
			}
			else
			{
				try
				{
					NPrivate::fg_SetDynamicValue({});
					_fFunctor();
					NPrivate::fg_SetDynamicValue("No exception thrown");
				}
				catch (t_CException const &_Exception)
				{
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
					return true;
				}
			}
			return false;
		}

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const
		{
			o_String += typename tf_CString::CFormat("{}") << fg_GetTypeName<t_CException>();
		}
	};

	template <bool t_bOuter, typename t_CException, typename ...tp_CExceptions>
	struct TCThrowsExceptionImpl<t_bOuter, t_CException, tp_CExceptions...> : public TCThrowsExceptionImpl<false, tp_CExceptions...>
	{
	public:
		template <typename t_FFunctor>
		bool operator == (t_FFunctor const &_fFunctor) const
		{
#if DMibConfig_Tests_Enable
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
#endif
			if constexpr (t_bOuter)
			{
				try
				{
					try
					{
						return TCThrowsExceptionImpl<false, tp_CExceptions...>::operator == (_fFunctor);
					}
					catch (t_CException const &_Exception)
					{
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
						return true;
					}
				}
				catch (NException::CExceptionBase const &_Exception)
				{
					NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
					return false;
				}
			}
			else
			{
				try
				{
					return TCThrowsExceptionImpl<false, tp_CExceptions...>::operator == (_fFunctor);
				}
				catch (t_CException const &_Exception)
				{
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
					return true;
				}
			}

			return false;
		}

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const
		{
			TCThrowsExceptionImpl<false, tp_CExceptions...>::f_Format(o_String);

			o_String += typename tf_CString::CFormat("\n{}") << fg_GetTypeName<t_CException>();
		}
	};

	template <typename ...tp_CExceptions>
	using TCThrowsException = TCThrowsExceptionImpl<true, tp_CExceptions...>;

	template <bool t_bOuter, typename ...tp_CExceptions>
	struct TCThrowsExceptionExactImpl;

	template <bool t_bOuter, typename t_CException>
	struct TCThrowsExceptionExactImpl<t_bOuter, t_CException>
	{
		TCThrowsExceptionExactImpl(t_CException const &_Exception)
			: m_Exception(_Exception)
		{
		}

		template <typename t_FFunctor>
		bool operator == (t_FFunctor const &_fFunctor) const
		{
#if DMibConfig_Tests_Enable
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
#endif
			if constexpr (t_bOuter)
			{
				try
				{
					try
					{
						NPrivate::fg_SetDynamicValue({});
						_fFunctor();
						NPrivate::fg_SetDynamicValue("No exception thrown");
					}
					catch (t_CException const &_Exception)
					{
						using namespace NStr;
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue("{} {}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
						return _Exception == m_Exception;
					}
				}
				catch (NException::CExceptionBase const &_Exception)
				{
					using namespace NStr;
					NPrivate::fg_SetDynamicValue("{} {}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
					return false;
				}
			}
			else
			{
				try
				{
					NPrivate::fg_SetDynamicValue({});
					_fFunctor();
					NPrivate::fg_SetDynamicValue("No exception thrown");
				}
				catch (t_CException const &_Exception)
				{
					using namespace NStr;
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue("{} {}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
					return _Exception == m_Exception;
				}
			}

			return false;
		}

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const
		{
			if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
				o_String += typename tf_CString::CFormat("{} {}") << m_Exception.f_GetClass() << m_Exception.f_GetErrorCharPointer();
			else
				o_String += typename tf_CString::CFormat("{}") << fg_GetTypeName<t_CException>();
		}

		t_CException m_Exception;
	};

	template <bool t_bOuter, typename t_CException, typename ...tp_CExceptions>
	struct TCThrowsExceptionExactImpl<t_bOuter, t_CException, tp_CExceptions...> : private TCThrowsExceptionExactImpl<false, tp_CExceptions...>
	{
	public:

		TCThrowsExceptionExactImpl
			(
				t_CException const &_Exception
				, tp_CExceptions const &...p_Exceptions
			)
			: TCThrowsExceptionExactImpl<false, tp_CExceptions...>(p_Exceptions...)
			, m_Exception(_Exception)
		{
		}

		template <typename t_FFunctor>
		bool operator == (t_FFunctor const &_fFunctor) const
		{
#if DMibConfig_Tests_Enable
			NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
			DMibExceptionFilter(TestExceptionFilter);
#endif
			if constexpr (t_bOuter)
			{
				try
				{
					try
					{
						return TCThrowsExceptionExactImpl<false, tp_CExceptions...>::operator == (_fFunctor);
					}
					catch (t_CException const &_Exception)
					{
						using namespace NStr;
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue("{} {}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
						return _Exception == m_Exception;
					}
				}
				catch (NException::CExceptionBase const &_Exception)
				{
					using namespace NStr;
					NPrivate::fg_SetDynamicValue("{} {}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
					return false;
				}
			}
			else
			{
				try
				{
					return TCThrowsExceptionExactImpl<false, tp_CExceptions...>::operator == (_fFunctor);
				}
				catch (t_CException const &_Exception)
				{
					using namespace NStr;
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue("{} {}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
					return _Exception == m_Exception;
				}
			}

			return false;
		}

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const
		{
			TCThrowsExceptionExactImpl<false, tp_CExceptions...>::f_Format(o_String);

			if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
				o_String += typename tf_CString::CFormat("\n{} {}") << m_Exception.f_GetClass() << m_Exception.f_GetErrorCharPointer();
			else
				o_String += typename tf_CString::CFormat("{}") << fg_GetTypeName<t_CException>();
		}

		t_CException m_Exception;
	};

	template <typename ...tp_CExceptions>
	using TCThrowsExceptionExact = TCThrowsExceptionExactImpl<true, tp_CExceptions...>;

	template <typename ...tfp_CException>
	TCThrowsExceptionExact<tfp_CException...> fg_ThrowsException(tfp_CException const & ...p_Exceptions)
	{
		return TCThrowsExceptionExact<tfp_CException...>(p_Exceptions...);
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
