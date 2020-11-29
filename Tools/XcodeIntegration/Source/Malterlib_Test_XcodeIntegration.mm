// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Process/ProcessLaunch>
#include <Mib/Test/Test>
#include <Mib/Test/ResultParser>
#include <Mib/Container/Vector>
#include <Mib/Container/Set>
#include <Mib/File/File>
#include <Mib/Function/Function>
#include <Mib/Concurrency/ConcurrencyManager>

#import <XCTest/XCTest.h>
#import <XCTest/XCTIssue.h>
#import <XCTest/XCTSourceCodeContext.h>
#import <objc/runtime.h>

using namespace NMib;
using namespace NMib::NStr;
using namespace NMib::NContainer;
using namespace NMib::NFile;
using namespace NMib::NProcess;
using namespace NMib::NFunction;
using namespace NMib::NTest;
using namespace NMib::NConcurrency;

extern ch8 const *g_AllTests[];
extern mint g_nAllTests;

struct CTestSuiteRecord
{
	CStr m_Executable;
	CStr m_TestPath;
	CTestLocation m_Location;
	mint m_Thread;
	TCSet<CStr> m_Groups;
};

struct CTestExecutable
{
	CStr m_Name;
	CStr m_Executable;
	TCVector<CTestSuiteRecord> m_Tests;
	TCMap<CStr, CStr> m_NameToPath;
};

constinit TCAggregate<TCVector<CTestExecutable>> g_TestExecutables = {DAggregateInit};
constinit TCAggregate<TCMap<Class, CTestExecutable *>> g_TestClassToExecutable = {DAggregateInit};
constinit TCAggregate<TCVector<CStr>> g_FilteredCommandLine = {DAggregateInit};

#define XCTestCaseHidden XCTestCase

@interface MalterlibTestLoader : NSObject
@end

@interface MalterlibTestCase : XCTestCaseHidden
@end

@implementation MalterlibTestCase

+ (NSBundle *)bundleForClass
{
    return [NSBundle bundleForClass:[MalterlibTestLoader class]];
}

+ (NSArray *)testInvocations
{
    NSMutableArray *invocations = [NSMutableArray array];

    unsigned int methodCount = 0;
    Method *methods = class_copyMethodList([self class], &methodCount);

    for (unsigned int i = 0; i < methodCount; i++) {
        SEL sel = method_getName(methods[i]);
        NSMethodSignature *sig = [self instanceMethodSignatureForSelector:sel];
        NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:sig];
        [invocation setSelector:sel];
        [invocations addObject:invocation];
    }

    free(methods);

    return invocations;
}

@end

static void fg_RunTests
	(
		CStr const &_TestApp
		, TCVector<CStr> const &_Params
		, TCFunction<void (CStr const &_TestPath, CTestLocation const &_Location, mint _Thread, TCSet<CStr> const &_Groups)> const &_fOnCategory
		, TCFunction<void (CTestResult const &_TestResult)> const &_fOnTestResult
	)
{
	CTestMemoryResult MemoryResult;
	struct CLocalParser : public CTestResultParser
	{
		CLocalParser
			(
				TCFunction<void (CStr const &_TestPath, CTestLocation const &_Location, mint _Thread, TCSet<CStr> const &_Groups)> const &_fOnCategory
				, TCFunction<void (CTestResult const &_TestResult)> const &_fOnTestResult
			)
			: mp_fOnCategory(_fOnCategory)
			, mp_fOnTestResult(_fOnTestResult)
		{
		}
		virtual void f_HandleHeader(NMib::NContainer::CRegistry const &_Reg) override
		{
		}
		virtual void f_HandleFooter(NMib::NContainer::CRegistry const &_Reg) override
		{
		}
		virtual void f_HandleCategory(NMib::NContainer::CRegistry const &_Reg) override
		{
			CStr TestPath = _Reg.f_GetValue("Path", "");
			CTestLocation Location;
			Location.m_File = _Reg.f_GetValue("File", "");
			Location.m_Line = _Reg.f_GetValue("Line", "-1").f_ToInt(int32(-1));
			mint Thread = _Reg.f_GetValue("Line", "0").f_ToInt(mint(0));
			TCSet<CStr> Groups;
			if (auto *pGroups = _Reg.f_GetChild("Groups"))
			{
				for (auto &Child : pGroups->f_GetChildren())
					Groups[Child.f_GetThisValue()];
			}
			mp_fOnCategory(TestPath, Location, Thread, Groups);
		}
		virtual void f_HandleResult(NMib::NContainer::CRegistry const &_Reg) override
		{
			CTestResult Result;
			CTestResultParser::fs_DecodeResult(_Reg, Result);
			mp_fOnTestResult(Result);
		}
		virtual void f_HandlePerformanceResult(NMib::NContainer::CRegistry const &_Reg) override
		{
		}
		virtual void f_HandleMemoryResult(NMib::NContainer::CRegistry const &_Reg) override
		{
		}
	private:
		TCFunction<void (CStr const &_TestPath, CTestLocation const &_Location, mint _Thread, TCSet<CStr> const &_Groups)> mp_fOnCategory;
		TCFunction<void (CTestResult const &_TestResult)> mp_fOnTestResult;
	};

	CLocalParser ResultParser{_fOnCategory, _fOnTestResult};

	void *pProcess = nullptr;
	NMib::NThread::CEvent Exited;
	Exited.f_ResetSignaled();

	CProcessLaunchParams Params = CProcessLaunchParams::fs_LaunchExecutable
		(
			_TestApp
			, _Params
			, CFile::fs_GetPath(_TestApp)
			, [&](CProcessLaunchStateChangeVariant const &_State, fp64 _TimeSinceStart)
			{
				switch (_State.f_GetTypeID())
				{
				case EProcessLaunchState_Exited:
					{
						//uint32 ExitCode = _State.f_Get<EProcessLaunchState_Exited>();
						Exited.f_SetSignaled();
					}
					break;
				case EProcessLaunchState_Launched:
					{
						pProcess = _State.f_Get<EProcessLaunchState_Launched>();
					}
					break;
				case EProcessLaunchState_LaunchFailed:
					{
						DMibConErrOut("Error: {}\r\n", _State.f_Get<EProcessLaunchState_LaunchFailed>());
						Exited.f_SetSignaled();
					}
					break;
				}
			}
		)
	;

	NMib::NStorage::TCUniquePointer<CProcessLaunch> pProcessLaunch;

	Params.m_fOnOutput =
		[&](EProcessLaunchOutputType _OutputType, NMib::NStr::CStr const &_Output)
		{
			if (_OutputType == EProcessLaunchOutputType_StdOut)
				ResultParser.f_FeedText(_Output);
			else
				DMibConErrOut("{}\n", _Output);
		}
	;
	{
		pProcessLaunch = NMib::fg_Construct(Params, EProcessLaunchCloseFlag_BlockOnExit);
	}

	Exited.f_Wait();

	pProcessLaunch.f_Clear();
}

CStr fg_MakeNiceName(CUStr const &_String)
{
	CUStr Return;
	ch32 const *pParse = _String.f_GetStr();
	fg_ParseWhiteSpace(pParse);
	if (fg_CharIsNumber(*pParse))
		Return.f_AddChar('_');
	while (*pParse)
	{
		ch32 Char = *pParse;
		if (fg_CharIsAlphabetical(Char) || fg_CharIsNumber(Char) || Char == '_')
			Return.f_AddChar(Char);
		else
			Return.f_AddChar('_');
		++pParse;
	}
	return Return;
}

static void fg_RunTest(XCTestCase *_pSelf, SEL _Command)
{
	CStr TestPath = [NSStringFromSelector(_Command) UTF8String];

	auto pTestExecutable = (*g_TestClassToExecutable).f_FindEqual([_pSelf class]);
	[[maybe_unused]] auto self = _pSelf;
	XCTAssertTrue(pTestExecutable, @"No test executable found for class %@", [_pSelf class]);
	auto &TestExecutable = **pTestExecutable;

	auto *pTestPath = TestExecutable.m_NameToPath.f_FindEqual(TestPath);
	XCTAssertTrue(pTestPath, @"No test path found for function name %@", NSStringFromSelector(_Command));
	TestPath = *pTestPath;

	TCVector<CStr> TestParams;
	TestParams.f_Insert("--test");
	TestParams.f_Insert("--no-color");
	TestParams.f_Insert("--logger");
	TestParams.f_Insert("Registry");
	TestParams.f_Insert(TestPath);
	TestParams.f_Insert(*g_FilteredCommandLine);

	fg_RunTests
		(
			TestExecutable.m_Executable
			, TestParams
			, [&](CStr const &_TestPath, CTestLocation const &_Location, mint _Thread, TCSet<CStr> const &_Groups)
			{
			}
			, [&](CTestResult const &_TestResult)
			{
				CStr OutputText;
				auto fReportText = [&](CStr const &_Text)
					{
						if (!OutputText.f_IsEmpty())
							OutputText += DMibNewLine;
						OutputText += _Text;
					}
				;
				auto fAddReport = [&]
					(
						const NStr::CStr &_Result
						, const NStr::CStr &_TestPath
						, const NStr::CStr &_Expression
						, const NStr::CStr &_Values
					)
					{
						fReportText(fg_Format("{}   {}   [{}]", _Result, _Expression, _Values));
					}
				;

				auto fReport = [&](NStr::CStr const &_MessageStr)
					{
						fAddReport(_MessageStr, _TestResult.m_TestPath, _TestResult.m_Message, _TestResult.m_Values);
						if (!_TestResult.m_ExtraMultiLineReportData.f_IsEmpty())
						{
							fReportText("");
							NStr::CStr Temp = _TestResult.m_ExtraMultiLineReportData;
							while (!Temp.f_IsEmpty())
							{
								NStr::CStr Line = NStr::fg_GetStrLineSep(Temp);
								fReportText(NStr::CStr::CFormat("	{}") << Line);
							}
							fReportText("");
						}
					}
				;
				if (_TestResult.m_Result == ETestResult_Fail)
				{
					switch (_TestResult.m_FailureAction)
					{
					case ETest_Warn:
						fReport("Warning");
						break;
					case ETest_Fail:
						fReport("FAILED");
						break;
					case ETest_FailAndStop:
						fReport("FAILED and ABORTED");
						break;
					case ETest_None:
					case ETest_ExpectFail:
					case ETest_ExpectFailAndStop:
						break;
					}
				}
				else if (_TestResult.m_Result == ETestResult_Success)
				{
					switch (_TestResult.m_FailureAction)
					{
					case ETest_ExpectFail:
					case ETest_ExpectFailAndStop:
						fReport("Success (UNEXPECTED)");
						break;
					case ETest_None:
					case ETest_Warn:
					case ETest_Fail:
					case ETest_FailAndStop:
						break;
					}
				}
				if (!OutputText.f_IsEmpty())
				{
					auto LineNumber = _TestResult.m_Line;
					ch8 const *pFileName = _TestResult.m_File;
					NSString *pPath = pFileName ? [@(pFileName) stringByStandardizingPath] : nil;
					NSString *pDescription = @(OutputText.f_GetStr());
					XCTSourceCodeContext *pSourceCodeContext;

					if (pPath)
					{
						pSourceCodeContext =
							[
								[XCTSourceCodeContext alloc]
								initWithLocation: [[XCTSourceCodeLocation alloc] initWithFilePath: pPath lineNumber: (LineNumber >= 0 ? (NSUInteger)LineNumber : 0)]
							]
						;
					}
					else
						pSourceCodeContext = [[XCTSourceCodeContext alloc] init];

					XCTIssue *pIssue =
						[
							[XCTIssue alloc]
							initWithType: XCTIssueTypeAssertionFailure
							compactDescription: pDescription
							detailedDescription: nil
							sourceCodeContext: pSourceCodeContext
							associatedError: nil
							attachments: @[]
						]
					;
					[_pSelf recordIssue: pIssue];
				}
			}
		)
	;
}

@implementation MalterlibTestLoader

namespace NMib::NSys
{
	void fg_CreateSystem();
}

- (id)init
{
	TCVector<CStr> CommandLine = fg_GetSys()->f_GetCommandLineArgs();

	CommandLine.f_Remove(0);

	for (auto iCommand = CommandLine.f_GetIterator(); iCommand;)
	{
		if (*iCommand == "-NSTreatUnknownArgumentsAsOpen")
		{
			++iCommand;
			if (iCommand)
				++iCommand;
			continue;
		}

		if (*iCommand == "-ApplePersistenceIgnoreState")
		{
			++iCommand;
			if (iCommand)
				++iCommand;
			continue;
		}

		if (*iCommand == "--test")
		{
			++iCommand;
			continue;
		}

		(*g_FilteredCommandLine).f_Insert(*iCommand);
		++iCommand;
	}

	TCVector<CStr> TestParams;
	TestParams.f_Insert("--test-list");
	TestParams.f_Insert("--no-color");
	TestParams.f_Insert("--logger");
	TestParams.f_Insert("Registry");
	//TestParams.f_Insert(*g_FilteredCommandLine);

	NTime::CClock Clock{true};

	TCActorResultVector<CTestExecutable> ConcurrentTests;

	bool bConcurrent = true;

	auto SeparateActor = fg_ConcurrentActor();

	CStr ConfigSuffix = DConfigSuffix;

	for (mint i = 0; i < g_nAllTests; ++i)
	{
		CTestExecutable TestExecutable;
		TestExecutable.m_Executable = CFile::fs_AppendPath(CStr(DMalterlibTestDeployPath), g_AllTests[i]);
		CStr FileName = CFile::fs_GetFile(CStr(g_AllTests[i]));
		CStr Path = CFile::fs_GetPath(CStr(g_AllTests[i]));
		if (FileName.f_StartsWith("Test_"))
			FileName = FileName.f_Extract(5);
		if (FileName.f_FindReverse(ConfigSuffix) == (FileName.f_GetLen() - ConfigSuffix.f_GetLen()))
			FileName = FileName.f_Left(FileName.f_GetLen() - ConfigSuffix.f_GetLen());

		TestExecutable.m_Name = fg_MakeNiceName(CFile::fs_AppendPath(Path, FileName));

		auto fRunTest = [TestExecutable, &TestParams]() mutable
			{
				fg_RunTests
					(
						TestExecutable.m_Executable
						, TestParams
						, [&](CStr const &_TestPath, CTestLocation const &_Location, mint _Thread, TCSet<CStr> const &_Groups)
						{
							auto &Test = TestExecutable.m_Tests.f_Insert();
							Test.m_TestPath = _TestPath;
							Test.m_Executable = TestExecutable.m_Executable;
							Test.m_Location = _Location;
							Test.m_Thread = _Thread;
							Test.m_Groups = _Groups;
						}
						, [&](CTestResult const &_TestResult)
						{
						}
					)
				;
				return fg_Move(TestExecutable);
			}
		;

		if (bConcurrent)
			fg_ConcurrentDispatch(fg_Move(fRunTest)) > ConcurrentTests.f_AddResult();
		else
			fg_Dispatch(SeparateActor, fg_Move(fRunTest)) > ConcurrentTests.f_AddResult();
	}

	mint nTests = 0;
	for (auto &Results : ConcurrentTests.f_GetResults().f_CallSync())
	{
		nTests += Results->m_Tests.f_GetLen();
		g_TestExecutables->f_Insert(*Results);
	}

	fp64 Runtime = Clock.f_GetTime();
	DConErrOut2("Enumerated {} tests in {fe1} s\n", nTests, Runtime);

	for (auto &Executable : *g_TestExecutables)
	{
		DConErrOut("Register test executable: {}\n", Executable.m_Name);
        Class pTestClass = objc_allocateClassPair([MalterlibTestCase class], Executable.m_Name.f_GetStr(), 0);
        NSAssert1
			(
				pTestClass
				, @"Failed to register Malterlb Test class \"%@\""
				, @(Executable.m_Name.f_GetStr())
			)
		;
		(*g_TestClassToExecutable)[pTestClass] = &Executable;
		TCMap<CStr, CTestSuiteRecord *> AllPaths;
		TCSet<CStr> UsedNames;
		for (auto &Test : Executable.m_Tests)
		{
			if (!AllPaths(Test.m_TestPath, &Test).f_WasCreated())
			{
				auto &OtherTest = *AllPaths[Test.m_TestPath];
				DConErrOut2(DMibPFileLineFormat " Duplicate test suite{\n}", Test.m_Location.m_File, Test.m_Location.m_Line);
				DConErrOut2(DMibPFileLineFormat "	Other test suite{\n}", OtherTest.m_Location.m_File, OtherTest.m_Location.m_Line);
				continue;
			}
			uint32 Distinguisher = 1;
			CStr Name = fg_MakeNiceName(Test.m_TestPath);
			while (!UsedNames(Name).f_WasCreated())
				Name = fg_MakeNiceName(fg_Format("{}{}", Distinguisher++, Test.m_TestPath));
			Executable.m_NameToPath[Name] = Test.m_TestPath;
            SEL selector = sel_registerName(Name.f_GetStr());
            NSAssert1(selector, @"Failed to register select \"%@\"", @(Name.f_GetStr()));
            BOOL bAdded = class_addMethod(pTestClass, selector, (IMP)fg_RunTest, "v@:");
            NSAssert1(bAdded, @"Failed to add method \"%@\"", @(Name.f_GetStr()));
		}
		objc_registerClassPair(pTestClass);
	}

	return self;
}

@end
