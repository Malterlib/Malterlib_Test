// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>

namespace NMib::NTest::NPrivate
{
#if DMibConfig_Tests_Enable
	CTestPathScope::CTestPathScope(NStr::CStr const &_Path, ch8 const *_pFile, uint32 _Line)
		: m_Path(_Path)
		, m_pFile(_pFile)
		, m_Line(_Line)
	{
		fg_SetTestLastLocation(_pFile, _Line);
		m_PreviousPath = NMib::NTest::NPrivate::fg_PushCategory(_Path);
	}

	CTestPathScope::~CTestPathScope()
	{
		NMib::NTest::NPrivate::fg_PopCategory(m_PreviousPath);
	}

	void CTestPathScope::f_Suspend() noexcept
	{
	}

	void CTestPathScope::f_ResumeNoExcept() noexcept
	{
	}

	void CTestCategoryScope::f_ProcessCategory(NFunction::TCFunctionMovable<void ()> &&_Function)
	{
		bool bLeaf = (mp_Flags & ETestCategoryFlag_Tests) != 0;
		NContainer::TCSet<NStr::CStr> const &Groups = mp_Category.f_GetGroups();
		NContainer::TCSet<NStr::CStr> OldGroups;

		NStr::CStr PreviousPath = NMib::NTest::NPrivate::fg_PushCategory(mp_Category.f_GetCategory());
		if (!Groups.f_IsEmpty())
			OldGroups = NMib::NTest::NPrivate::fg_SetGroups(mp_Category.f_GetGroups());

		{
			if (NMib::NTest::NPrivate::fg_GetEnableExceptionFilter())
			{
				NMib::NTest::NPrivate::CTestExceptionFilter TestExceptionFilter;
				DMibExceptionFilter(TestExceptionFilter);
				if (fg_TestReportFlags() & ETestReportFlag_CrashOnException)
				{
					try
					{
						if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
						{
							if (f_ContinueEnumerating())
								_Function();
							if (bLeaf)
								f_ReportLeafCategory();
						}
					}
					catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
					{
					}
				}
				else
				{
					try
					{
						if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
						{
							if (f_ContinueEnumerating())
								_Function();
							if (bLeaf)
								f_ReportLeafCategory();
						}
					}
					catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
					{
					}
					catch (...)
					{
						NMib::NTest::NPrivate::fg_ReportTestException
							(
								TestExceptionFilter.f_DetachDumpFiles()
								, TestExceptionFilter.f_GetFile() ? TestExceptionFilter.f_GetFile()
								: mp_pFile, TestExceptionFilter.f_GetFile() ? TestExceptionFilter.f_GetLine()
								: mp_Line
							)
						;
					}
				}
			}
			else
			{
				NMib::NTest::NPrivate::CTestExceptionNoFilter TestExceptionFilter;
				DMibExceptionFilter(TestExceptionFilter);
				if (fg_TestReportFlags() & ETestReportFlag_CrashOnException)
				{
					try
					{
						if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
						{
							if (f_ContinueEnumerating())
								_Function();
							if (bLeaf)
								f_ReportLeafCategory();
						}
					}
					catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
					{
					}
				}
				else
				{
					try
					{
						if (NMib::NTest::NPrivate::fg_ShouldRunSubTest(bLeaf))
						{
							if (f_ContinueEnumerating())
								_Function();
							if (bLeaf)
								f_ReportLeafCategory();
						}
					}
					catch (NMib::NTest::NPrivate::CReportTestAbortException const &)
					{
					}
					catch (...)
					{
						NMib::NTest::NPrivate::fg_ReportTestException(NContainer::TCVector<NStr::CStr>(), mp_pFile, mp_Line);
					}
				}
			}
		}

		if (!Groups.f_IsEmpty())
			NMib::NTest::NPrivate::fg_SetGroups(OldGroups);
		NMib::NTest::NPrivate::fg_PopCategory(PreviousPath);
	}
#endif
}
