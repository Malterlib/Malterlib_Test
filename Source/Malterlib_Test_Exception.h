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
				_fFunctor();
#if DMibConfig_Tests_Enable
				NPrivate::fg_SetDynamicValue("No exception thrown");
#endif
			}
			catch ([[maybe_unused]] NException::CExceptionBase const &_Exception)
			{
#if DMibConfig_Tests_Enable
				using namespace NStr;
				NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
#endif
				return false;
			}
			catch (...)
			{
#if DMibConfig_Tests_Enable
				NPrivate::fg_SetDynamicValue("...");
#endif
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
						_fFunctor();
#if DMibConfig_Tests_Enable
						NPrivate::fg_SetDynamicValue("No exception thrown");
#endif
					}
					catch ([[maybe_unused]] t_CException const &_Exception)
					{
#if DMibConfig_Tests_Enable
						using namespace NStr;
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
						else
							NPrivate::fg_SetDynamicValue("");
#endif
						return true;
					}
				}
				catch ([[maybe_unused]] NException::CExceptionBase const &_Exception)
				{
#if DMibConfig_Tests_Enable
					NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
#endif
					return false;
				}
			}
			else
			{
				try
				{
					_fFunctor();
#if DMibConfig_Tests_Enable
					NPrivate::fg_SetDynamicValue("No exception thrown");
#endif
				}
				catch ([[maybe_unused]] t_CException const &_Exception)
				{
#if DMibConfig_Tests_Enable
					using namespace NStr;
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
#endif
					return true;
				}
			}
#if DMibConfig_Tests_Enable
			NPrivate::fg_SetDynamicValue({});
#endif
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
					catch ([[maybe_unused]] t_CException const &_Exception)
					{
#if DMibConfig_Tests_Enable
						using namespace NStr;
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
						else
							NPrivate::fg_SetDynamicValue({});
#endif
						return true;
					}
				}
				catch ([[maybe_unused]] NException::CExceptionBase const &_Exception)
				{
#if DMibConfig_Tests_Enable
					NPrivate::fg_SetDynamicValue(_Exception.f_GetClass());
#endif
					return false;
				}
			}
			else
			{
				try
				{
					return TCThrowsExceptionImpl<false, tp_CExceptions...>::operator == (_fFunctor);
				}
				catch ([[maybe_unused]] t_CException const &_Exception)
				{
#if DMibConfig_Tests_Enable
					using namespace NStr;
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
					else
						NPrivate::fg_SetDynamicValue({});
#endif
					return true;
				}
			}

#if DMibConfig_Tests_Enable
			NPrivate::fg_SetDynamicValue({});
#endif
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

		TCThrowsExceptionExactImpl(t_CException &&_Exception)
			: m_Exception(fg_Move(_Exception))
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
						_fFunctor();
#if DMibConfig_Tests_Enable
						NPrivate::fg_SetDynamicValue("No exception thrown");
#endif
					}
					catch (t_CException const &_Exception)
					{
						using namespace NStr;
#if DMibConfig_Tests_Enable
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
						else
							NPrivate::fg_SetDynamicValue({});
#endif
						return _Exception == m_Exception;
					}
				}
				catch ([[maybe_unused]] NException::CExceptionBase const &_Exception)
				{
					using namespace NStr;
#if DMibConfig_Tests_Enable
					NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
#endif
					return false;
				}
			}
			else
			{
				try
				{
					_fFunctor();
#if DMibConfig_Tests_Enable
					NPrivate::fg_SetDynamicValue("No exception thrown");
#endif
				}
				catch (t_CException const &_Exception)
				{
					using namespace NStr;
#if DMibConfig_Tests_Enable
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
					else
						NPrivate::fg_SetDynamicValue({});
#endif
					return _Exception == m_Exception;
				}
			}
#if DMibConfig_Tests_Enable
			NPrivate::fg_SetDynamicValue({});
#endif

			return false;
		}

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const
		{
			if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
				o_String += typename tf_CString::CFormat("{}\n{}") << m_Exception.f_GetClass() << m_Exception.f_GetErrorCharPointer();
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
#if DMibConfig_Tests_Enable
						if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
							NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
						else
							NPrivate::fg_SetDynamicValue({});
#endif
						return _Exception == m_Exception;
					}
				}
				catch ([[maybe_unused]] NException::CExceptionBase const &_Exception)
				{
					using namespace NStr;
#if DMibConfig_Tests_Enable
					NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
#endif
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
#if DMibConfig_Tests_Enable
					if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
						NPrivate::fg_SetDynamicValue("{}\n{}"_f << _Exception.f_GetClass() << _Exception.f_GetErrorCharPointer());
					else
						NPrivate::fg_SetDynamicValue({});
#endif
					return _Exception == m_Exception;
				}
			}

#if DMibConfig_Tests_Enable
			NPrivate::fg_SetDynamicValue({});
#endif
			return false;
		}

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const
		{
			TCThrowsExceptionExactImpl<false, tp_CExceptions...>::f_Format(o_String);

			if constexpr (NTraits::TCIsBaseOf<t_CException, NException::CExceptionBase>::mc_Value)
				o_String += typename tf_CString::CFormat("\n{}\n{}") << m_Exception.f_GetClass() << m_Exception.f_GetErrorCharPointer();
			else
				o_String += typename tf_CString::CFormat("{}") << fg_GetTypeName<t_CException>();
		}

		t_CException m_Exception;
	};

	template <typename ...tp_CExceptions>
	using TCThrowsExceptionExact = TCThrowsExceptionExactImpl<true, tp_CExceptions...>;

	template <typename ...tfp_CException>
	auto fg_ThrowsException(tfp_CException && ...p_Exceptions)
	{
		if constexpr (sizeof...(p_Exceptions) == 0)
			return TCThrowsException<>();
		else
			return TCThrowsExceptionExact<typename NTraits::TCRemoveReferenceAndQualifiers<tfp_CException>::CType...>(fg_Forward<tfp_CException>(p_Exceptions)...);
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
