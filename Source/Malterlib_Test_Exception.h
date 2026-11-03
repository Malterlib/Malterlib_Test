// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Test.h"

namespace NMib::NTest
{
	template <bool t_bOuter, typename ...tp_CExceptions>
	struct TCThrowsExceptionImpl;

	template <bool t_bOuter>
	struct TCThrowsExceptionImpl<t_bOuter>
	{
		template <typename tf_FFunctor>
		bool operator == (tf_FFunctor const &_fFunctor) const;

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const;
	};

	template <bool t_bOuter, typename t_CException>
	struct TCThrowsExceptionImpl<t_bOuter, t_CException>
	{
		template <typename tf_FFunctor>
		bool operator == (tf_FFunctor const &_fFunctor) const;

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const;
	};

	template <bool t_bOuter, typename t_CException, typename ...tp_CExceptions>
	struct TCThrowsExceptionImpl<t_bOuter, t_CException, tp_CExceptions...> : public TCThrowsExceptionImpl<false, tp_CExceptions...>
	{
		template <typename tf_FFunctor>
		bool operator == (tf_FFunctor const &_fFunctor) const;

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const;
	};

	template <typename ...tp_CExceptions>
	using TCThrowsException = TCThrowsExceptionImpl<true, tp_CExceptions...>;

	template <bool t_bOuter, typename ...tp_CExceptions>
	struct TCThrowsExceptionExactImpl;

	template <bool t_bOuter, typename t_CException>
	struct TCThrowsExceptionExactImpl<t_bOuter, t_CException>
	{
		TCThrowsExceptionExactImpl(t_CException const &_Exception);
		TCThrowsExceptionExactImpl(t_CException &&_Exception);

		template <typename tf_FFunctor>
		bool operator == (tf_FFunctor const &_fFunctor) const;

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const;

		t_CException m_Exception;
	};

	template <bool t_bOuter, typename t_CException, typename ...tp_CExceptions>
	struct TCThrowsExceptionExactImpl<t_bOuter, t_CException, tp_CExceptions...> : private TCThrowsExceptionExactImpl<false, tp_CExceptions...>
	{
		TCThrowsExceptionExactImpl(t_CException const &_Exception, tp_CExceptions const & ...p_Exceptions);

		template <typename tf_FFunctor>
		bool operator == (tf_FFunctor const &_fFunctor) const;

		template <typename tf_CString>
		void f_Format(tf_CString &o_String) const;

		t_CException m_Exception;
	};

	template <typename ...tp_CExceptions>
	using TCThrowsExceptionExact = TCThrowsExceptionExactImpl<true, tp_CExceptions...>;

	template <typename ...tfp_CException>
	auto fg_ThrowsException(tfp_CException && ...p_Exceptions);

#if defined DMibContractConfigure_RequireEnabled
	TCThrowsException<NContract::CContractException_Require> fg_ViolatesRequire();
	TCThrowsExceptionExact<NContract::CContractException_Require> fg_ViolatesRequire(ch8 const *_pError);
#else
	TCThrowsException<> fg_ViolatesRequire();
	TCThrowsException<> fg_ViolatesRequire(ch8 const *_pError);
#endif

#if defined DMibContractConfigure_CheckEnabled
	TCThrowsException<NContract::CContractException_Check> fg_ViolatesCheck();
	TCThrowsExceptionExact<NContract::CContractException_Check> fg_ViolatesCheck(ch8 const *_pError);
#else
	TCThrowsException<> fg_ViolatesCheck();
	TCThrowsException<> fg_ViolatesCheck(ch8 const *_pError);
#endif

#if DMibEnableSafeCheck > 0
	TCThrowsException<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck();
	TCThrowsExceptionExact<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck(ch8 const *_pError);
#else
	TCThrowsException<> fg_ViolatesSafeCheck();
	TCThrowsException<> fg_ViolatesSafeCheck(ch8 const *_pError);
#endif
}

#include "Malterlib_Test_Exception.hpp"
