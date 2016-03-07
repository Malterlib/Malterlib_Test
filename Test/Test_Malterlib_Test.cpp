// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Test/Test>
#include <Mib/Test/Recursive>
#include <Mib/Test/Exception>

namespace
{
	class CTest_Tests : public NMib::NTest::CTest
	{
	public:

		int m_Test;
		CTest_Tests()
		{
			m_Test = 0;
		}

		class CObject
		{
		public:
			void f_Test0()
			{
				throw int32(2);
			}

			int f_Test1()
			{
				throw fp32(3.0);
				return 1;
			}

			int f_Test2(int)
			{
				throw fp64(4.0);
				return 2;
			}

			int f_Contract(int _Value)
			{
				DMibRequire(_Value < 100);
				return _Value;
			}

			int f_Contract2(int _Value)
			{
				DMibRequire(_Value < 100)("Should be");
				return _Value;
			}

			int f_Assert(int _Value)
			{
				DMibSafeCheck(_Value < 100, "");
				return _Value;
			}

			int f_Assert2(int _Value)
			{
				DMibSafeCheck(_Value < 100, "Should be");
				return _Value;
			}
		};

		void f_DoTests()
		{
			DMibTestSuite("General")
			{
				DMibTest(DMibExpr(true));
				DMibTest(DMibExpr(0) == DMibExpr(0));

			};
			DMibTestSuite("LambdasExpr")
			{
				DMibTest(DMibELExpr(1.0) == DMibExpr(1));
			};
			DMibTestSuite("Exceptions")
			{
				CObject Object;
				(void)Object;
				DMibTest(DMibExpr(NMib::NTest::TCThrowsException<int32>()) == DMibLExpr(Object.f_Test0()));
				DMibTest(DMibExpr(NMib::NTest::TCThrowsException<fp32>()) == DMibLExpr(Object.f_Test1()));
				DMibTest(DMibExpr(NMib::NTest::TCThrowsException<fp64>()) == DMibLExpr(Object.f_Test2(2)));
				DMibTest(DMibExpr((NMib::NTest::TCThrowsException<int32, fp32, fp64>())) == DMibLExpr(Object.f_Test0()));
				DMibTest(DMibExpr((NMib::NTest::TCThrowsException<int32, fp32, fp64>())) == DMibLExpr(Object.f_Test1()));
				DMibTest(DMibExpr((NMib::NTest::TCThrowsException<int32, fp32, fp64>())) == DMibLExpr(Object.f_Test2(2)));

				DMibTest(DMibExpr(fg_ThrowsException(int32(2))) == DMibLExpr(Object.f_Test0()));
				DMibTest(DMibExpr(fg_ThrowsException(fp32(3.0))) == DMibLExpr(Object.f_Test1()));
				DMibTest(DMibExpr(fg_ThrowsException(fp64(4.0))) == DMibLExpr(Object.f_Test2(2)));
				DMibTest(DMibExpr(fg_ThrowsException(int32(2), fp32(3.0), fp64(4.0))) == DMibLExpr(Object.f_Test0()));
				DMibTest(DMibExpr(fg_ThrowsException(int32(2), fp32(3.0), fp64(4.0))) == DMibLExpr(Object.f_Test1()));
				DMibTest(DMibExpr(fg_ThrowsException(int32(2), fp32(3.0), fp64(4.0))) == DMibLExpr(Object.f_Test2(2)));
			};
			DMibTestSuite("Contracts")
			{
				CObject Object;
				(void)Object;
				DMibTest(DMibExpr(NMib::NTest::fg_ViolatesRequire()) == DMibLExpr(Object.f_Contract(200)));
				DMibTest(DMibExpr(NMib::NTest::fg_ViolatesRequire("_Value < 100")) == DMibLExpr(Object.f_Contract(200)));

				DMibTest(DMibExpr(NMib::NTest::fg_ViolatesRequire("_Value < 100 where note: 'Should be'")) == DMibLExpr(Object.f_Contract2(200)));

				DMibTest(DMibExpr(NMib::NTest::fg_ViolatesAssert()) == DMibLExpr(Object.f_Assert(200)));
				DMibTest(DMibExpr(NMib::NTest::fg_ViolatesAssert("_Value < 100")) == DMibLExpr(Object.f_Assert(200)));
				DMibTest(DMibExpr(NMib::NTest::fg_ViolatesAssert("_Value < 100 'Should be'")) == DMibLExpr(Object.f_Assert2(200)));
			};
			DMibTestCategory("Performance")
			{
				DMibTestSuite("Expressions")
				{
#					ifdef DMibDebug
						const static mint nLoops = 10000;
						const static mint nTests = 2;
#					else
						const static mint nLoops = 100000;
						const static mint nTests = 10;
#					endif

					NMib::NTime::CCyclesMin TestTimer;
					for (mint i = 0; i < nTests; ++i)
					{
						TestTimer.f_Start();
						for (mint i = 0; i < nLoops; ++i)
						{
							DMibTest(DMibExpr(i) < DMibExpr(nLoops)) (ETestFlag_Aggregated);
						}
						TestTimer.f_Stop();
					}

					TestTimer /= nLoops;

					mint ExpectedCycles = 1000;
#					ifdef DMibDebug
						ExpectedCycles = 5000;
#					endif
					if (NMib::NTest::fg_GroupActive("Performance"))
						DMibTest(DMibExpr(TestTimer.f_GetCycles()) < DMibExpr(ExpectedCycles));
				};
				DMibTestSuite("Raw")
				{
#					ifdef DMibDebug
						const static mint nLoops = 10000;
						const static mint nTests = 2;
#					else
						const static mint nLoops = 100000;
						const static mint nTests = 10;
#					endif

					NMib::NTime::CCyclesMin TestTimer;
					for (mint i = 0; i < nTests; ++i)
					{
						TestTimer.f_Start();
						for (mint i = 0; i < nLoops; ++i)
						{
							DMibTest(i < nLoops) ("Loop test") (ETestFlag_Aggregated);
						}
						TestTimer.f_Stop();
					}

					TestTimer /= nLoops;

					mint ExpectedCycles = 1000;
#ifdef DMibDebug
					ExpectedCycles = 5000;
#endif
					if (NMib::NTest::fg_GroupActive("Performance"))
						DMibTest(DMibExpr(TestTimer.f_GetCycles()) < DMibExpr(ExpectedCycles));
				};
			};
			DMibTestCategory("Conditional")
			{
				DMibTestSuite("SuccessConditions")
				{
					bint bARun = 0, bBRun = 0;

					if (DMibTest(DMibExpr(bARun = 1) == DMibExpr(1)))
					{
						DMibTest(DMibExpr(bBRun = 1) == DMibExpr(1));
					}

					DMibTest(DMibExpr(bARun) == DMibExpr(1));
					DMibTest(DMibExpr(bBRun) == DMibExpr(1));
				};
				DMibTestSuite("FailureConditions")
				{
					bint bARun = 0, bBRun = 0;

					if (DMibTest(DMibExpr(bARun = 1) != DMibExpr(1)) (ETest_ExpectFail))
					{
						DMibTest(DMibExpr(bBRun = 1) == DMibExpr(1));
					}

					DMibTest(DMibExpr(bARun) == DMibExpr(1));
					DMibTest(DMibExpr(bBRun) == DMibExpr(0));
				};
			};
			DMibTestSuite("DMibTestExpr")
			{
				bint bTrue = true;
				DMibTestExpr(bTrue);
				bTrue = false;
				DMibTestExpr(bTrue) ("Second") (ETest_ExpectFail);
			};
			DMibTestSuite("Recursive tests")
			{
				auto fTest = []
					{
						bint bShouldNotBeVisible = true;
						DMibTestExpr(bShouldNotBeVisible);
					}
				;
				DMibTest(DMibExpr(fg_TestRecursive([&]{fTest();})));
				auto fTestFail = []
					{
						bint bShouldNotBeVisible = false;
						DMibTestExpr(bShouldNotBeVisible);
						DMibTestExpr(bShouldNotBeVisible && 2);
						DMibTestExpr(bShouldNotBeVisible && 3);
						DMibTestExpr(bShouldNotBeVisible && 4);
						DMibTestExpr(bShouldNotBeVisible && 5);
						DMibTestExpr(bShouldNotBeVisible && 6);
					}
				;
				DMibTest(DMibExpr(fg_TestRecursive([&]{fTestFail();}))) (ETest_ExpectFail);
			};
		}

	};
	
	DMibTestRegister(CTest_Tests, Malterlib::Test);
}


