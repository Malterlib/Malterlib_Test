// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>
#include <Mib/Test/Exception>

namespace NMib::NTest
{
#if defined DMibContractConfigure_RequireEnabled
	TCThrowsException<NContract::CContractException_Require> fg_ViolatesRequire()
	{
		return TCThrowsException<NContract::CContractException_Require>();
	}
	TCThrowsExceptionExact<NContract::CContractException_Require> fg_ViolatesRequire(const ch8 *_pError)
	{
		return NContract::CContractException_Require("CContractException_Require", DMibPFile, DMibPLine, DMibPFunction, _pError, true);
	}
#else
	TCThrowsException<> fg_ViolatesRequire()
	{
		return TCThrowsException<>();
	}
	TCThrowsException<> fg_ViolatesRequire(const ch8 *_pError)
	{
		return TCThrowsException<>();
	}
#endif

#if DMibEnableSafeCheck > 0
	TCThrowsException<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck()
	{
		return TCThrowsException<NException::CExceptionSafeCheck>();
	}

	TCThrowsExceptionExact<NException::CExceptionSafeCheck> fg_ViolatesSafeCheck(const ch8 *_pError)
	{
		return NException::CExceptionSafeCheck("CExceptionSafeCheck", DMibPFile, DMibPLine, DMibPFunction, _pError, true);
	}
#else
	TCThrowsException<> fg_ViolatesSafeCheck()
	{
		return TCThrowsException<>();
	}
	TCThrowsException<> fg_ViolatesSafeCheck(const ch8 *_pError)
	{
		return TCThrowsException<>();
	}
#endif

}

namespace NMib::NTest::NPrivate
{
#if DMibConfig_Tests_Enable
	void CTestExceptionFilter::f_SetDumpFiles(const NContainer::TCVector<NStr::CStr> &_Files)
	{
		mint nDumpFiles = m_DumpFiles.f_GetLen();

		for (mint i = 0; i < nDumpFiles; ++i)
		{
			try
			{
				NFile::CFile::fs_DeleteFile(m_DumpFiles[i]);
			}
			catch (NException::CException)
			{
			}
		}
		m_DumpFiles = _Files;
	}

	NContainer::TCVector<NStr::CStr> CTestExceptionFilter::f_DetachDumpFiles()
	{
		return fg_Move(m_DumpFiles);
	}

	const ch8 *CTestExceptionFilter::f_GetFile()
	{
		if (!m_pStackTraceInfo || !(*m_pStackTraceInfo->m_pSourceFileName))
			return nullptr;
		return m_pStackTraceInfo->m_pSourceFileName;
	}

	int32 CTestExceptionFilter::f_GetLine()
	{
		if (!m_pStackTraceInfo)
			return 0;
		return m_pStackTraceInfo->m_SourceLine;
	}

	void CTestExceptionFilter::f_Exception(void *_pExceptionData)
	{
		if (!_pExceptionData)
			return; // Rethrow?

		NException::CExceptionBase *pExceptionBase = fg_AutoStaticCast(_pExceptionData);
		if (pExceptionBase && pExceptionBase->f_IsValid())
		{
			if (NStr::fg_StrCmp(pExceptionBase->f_GetClass(), "CExceptionCoroutineWrapper") == 0)
				return;
		}
		else
			return; // Not our exception

		if (m_pStackTraceInfo)
			NSys::fg_Debug_ReleaseStackTraceInfo(m_pStackTraceInfo);
		m_pStackTraceInfo = nullptr;

		CReportTestAbortException *pException = (CReportTestAbortException *)_pExceptionData;
		if (!pException || !pException->f_IsValid())
		{
			NContainer::TCVector<NStr::CStr> DumpFiles = fg_DumpTestException();
			f_SetDumpFiles(DumpFiles);
		}

		CMibCodeAddress Stack[64];
		mint nStack = NSys::fg_System_GetStackTrace(Stack, 64);
		for (mint i = 0; i < nStack; ++i)
		{
			CStackTraceInfo *pInfo = NSys::fg_Debug_AquireStackTraceInfo(Stack[i]);
			if (pInfo)
			{
				if (*pInfo->m_pSourceFileName && NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "core\\source\\platform") < 0
					&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "core/source/platform") < 0
					&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "throw.cpp") < 0
					&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_test.cpp") < 0
					&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_contract.cpp") < 0
					&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_contract.h") < 0
					&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "malterlib_debug.cpp") < 0
					&& NStr::fg_StrFindNoCase(pInfo->m_pSourceFileName, "include\\xx") < 0
					)
				{
					//DMibTrace(DMibPFileLineFormat " {}" DMibNewLine, pInfo->m_pSourceFileName << pInfo->m_SourceLine << pInfo->m_pFunctionName);
					m_pStackTraceInfo = pInfo;
					break;
				}
				NSys::fg_Debug_ReleaseStackTraceInfo(pInfo);
			}
		}
	}
	
	NContainer::TCVector<NStr::CStr> fg_DumpTestException()
	{
		NContainer::TCVector<NStr::CStr> GeneratedLogs;
		NSys::fg_Debug_GenerateCrashDump("", "", GeneratedLogs, false);
		return GeneratedLogs;
	}

	void fg_ReportTestException(const NContainer::TCVector<NStr::CStr> &_DumpFiles, const ch8 *_pFile, int32 _Line)
	{
		using namespace NStr;

		const ch8 *pFile = nullptr;
		int32 Line = 0;
		fg_GetTestLastLocation(pFile, Line);
		NStr::CStr ExtraReportData;

		if (pFile)
			ExtraReportData += DMibPFileLineFormat " Last know test location{\n}"_f << pFile << Line;

		mint nLogs = _DumpFiles.f_GetLen();
		if (nLogs != 0)
		{
			ExtraReportData += "The following crash dump files were generated:" DMibNewLine;
			for (mint i = 0; i < nLogs; ++i)
			{
				ExtraReportData += "{}{\n}"_f << _DumpFiles[i];
			}
		}

		if (ExtraReportData.f_Trim())
			ExtraReportData = NStr::CStr("{\n}{\n}{}"_f << ExtraReportData);

		try
		{
			try
			{
				throw;
			}
			catch (NContract::CContractException_Require const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " {}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, "Require contract violation"
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
			catch (NContract::CContractException_Check const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " {}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, "Check contract violation"
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
			catch (NContract::CContractException_Ensure const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " {}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, "Ensure contract violation"
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
			catch (NContract::CContractException_Invariant const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " {}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, "Invariant contract violation"
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
			catch (NContract::CContractException_NeverGetHere const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " {}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, "Never get here contract violation"
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
			catch (NContract::CContractException const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " {}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, NStr::CStr::CFormat("Contract violation: {}") << _Exception.f_GetClass()
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
#if DMibEnableSafeCheck > 0
			catch (NException::CExceptionSafeCheck const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " {}{}"_f << _Exception.f_GetFile() << _Exception.f_GetLine() << _Exception.f_GetErrorStr() << ExtraReportData;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, NStr::CStr::CFormat("Assert violation")
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
#endif
			catch (NException::CException const &_Exception)
			{
				CStr ReportData = DMibPFileLineFormat " Uncaught {} exception:{\n}{}{}"_f
					<< _Exception.f_GetFile()
					<< _Exception.f_GetLine()
					<< _Exception.f_GetClass()
					<< _Exception.f_GetErrorStr()
					<< ExtraReportData
				;
				fg_ReportTestResult
					(
						ETestResult_Fail
						, NStr::CStr::CFormat("Uncaught exception")
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
			catch (std::exception const& _Exception)
			{
				CStr ReportData = " Uncaught {} exception{}"_f << NStr::CStr(_Exception.what()) << NStr::CStr(ExtraReportData);
				fg_ReportTestResult
					(
						ETestResult_Fail
						, NStr::CStr("Uncaught exception")
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
			catch (...)
			{
				fg_ReportTestResult
					(
						ETestResult_Fail
						, "Uncaught exception"
						, ""
						, ETest_FailAndStop
						, ECheckType_Message
						, _pFile
						, _Line
						, ExtraReportData
						, ETestFlag_None
						, ETestResultReportFlag_AskReport
					)
				;
			}
		}
		catch (CReportTestAbortException)
		{
		}
	}
#endif
}
