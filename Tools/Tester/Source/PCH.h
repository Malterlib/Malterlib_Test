/*
	Copyright © 2015 Hansoft AB
	Distributed under the MIT license, see license text in LICENSE.Malterlib
*/

#ifdef _MSC_VER
#ifndef NTDDI_VERSION
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#define NTDDI_VERSION NTDDI_VISTA
#endif
#endif

#if _MSC_VER == 1700
#define _VARIADIC_MAX 10
#endif


#ifdef __cplusplus
#include "Malterlib_Tester.h"
#else
#include <stdlib.h>
#endif
