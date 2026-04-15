# CLAUDE.md - Test Module

This file provides guidance for writing tests with the Malterlib test framework.

## Module Overview

The Test module provides the `DMibTest*` macros and the `NMib::NTest::CTest` base class used across the repository. Tests are usually organized as one registered test class per file, with `f_DoTests()` containing categories, suites, and assertions.

## Basic Structure

Typical structure:

```cpp
namespace
{
	class CMyTests : public NMib::NTest::CTest
	{
	public:
		void f_DoTests()
		{
			DMibTestSuite("General")
			{
				DMibExpect(Value, ==, 5);
			};
		}
	};
}

DMibTestRegister(CMyTests, Malterlib::MyModule);
```

## Test Organization Rules

### Use suites for local setup and assertions

- Put setup work inside `DMibTestSuite(...)` or `DMibTestCategory(...)` blocks.
- Do not perform non-trivial test setup outside suites in `f_DoTests()`.
- If multiple suites need the same setup, repeat the small setup locally instead of hoisting a large mutable test fixture above all suites unless there is a clear need.

Good:

```cpp
DMibTestSuite("FindOwner")
{
	TCMap<CStr, int32> Paths;
	Paths["/Root"] = 1;
	DMibExpectTrue(!Paths.f_IsEmpty());
};
```

Avoid:

```cpp
TCMap<CStr, int32> Paths;
Paths["/Root"] = 1;

DMibTestSuite("FindOwner")
{
	DMibExpectTrue(!Paths.f_IsEmpty());
};
```

### Use `DMibTestSuite` for normal grouping

- `DMibTestSuite("...")` is the normal building block for test cases.
- Prefer a small number of meaningful suites over one suite per trivial assertion.
- Group closely related success cases together and closely related failure cases together.

### Use `DMibTestCategory` for higher-level grouping

- `DMibTestCategory("...")` is for broader buckets above suites.
- Use it when a whole section of suites belongs to a named group such as `Performance` or `Conditional`.
- Match the style in `Malterlib/Test/Test/Test_Malterlib_Test.cpp`, where categories contain one or more suites.

Example:

```cpp
DMibTestCategory("Performance")
{
	DMibTestSuite("Expressions")
	{
		// performance-oriented checks
	};

	DMibTestSuite("Raw")
	{
		// related performance-oriented checks
	};
};
```

### Async tests are first-class

- `DMibTestSuite(...)` and `DMibTestCategory(...)` can be used with coroutine bodies.
- For async tests, append `-> TCFuture<void>` to the block lambda and use `co_await` / `co_return {}` inside.
- The canonical style is in `Malterlib/Concurrency/Test/Test_Malterlib_Concurrency_CoRoutines.cpp`.

Example:

```cpp
DMibTestCategory("AsyncCase") -> TCFuture<void>
{
	auto Value = co_await f_DoAsyncWork();
	DMibExpect(Value, ==, 5);
	co_return {};
};
```

Async suites are also valid, for example performance tests often use:

```cpp
DMibTestSuite(CTestCategory("ThreadSelf") << CTestGroup("Performance")) -> TCFuture<void>
{
	co_await f_RunBenchmark();
	co_return {};
};
```

### Categories can be tagged with groups

- Use `CTestCategory("...") << CTestGroup("...")` when a test belongs to a specific group.
- You can pass that tagged category to `DMibTestSuite(...)`, `DMibTestCategory(...)`, or `DMibTestCategoryFlags(...)`.
- Valid built-in group names are:
  - `Performance`
  - `Torture`
  - `Memory`
  - `Unfinished`
  - `Expensive`
  - `Manual`
  - `SuperUser`

Examples from the tree:

```cpp
DMibTestSuite(CTestCategory("ThreadSelf") << CTestGroup("Performance")) -> TCFuture<void>
{
	co_return {};
};

DMibTestSuite(CTestCategory("U2F") << CTestGroup("Manual"))
{
	// manual test
};
```

Use groups to control when special tests run rather than inventing ad-hoc category names.

## Unique Test Paths

### Every executed assertion must end up with a unique path

- The framework asserts on duplicate test paths.
- If the same helper or lambda emits the same `DMibExpect*` line multiple times in one suite, you must disambiguate those cases.
- Use `DMibTestPath("...")` inside the helper before the assertions to make each call site unique.

Good:

```cpp
auto fCheck = [&](CStr const &_CaseName, int _Value)
	{
		DMibTestPath(_CaseName);
		DMibExpect(_Value, ==, 5);
	}
;

fCheck("CaseA", 5);
fCheck("CaseB", 5);
```

Avoid calling the same helper repeatedly without `DMibTestPath(...)` when it contains assertions on fixed source lines.

### Keep path names stable and descriptive

- Use short names like `ExactRoot`, `NearestAncestor`, `NoOwner`.
- Prefer semantic names over raw data dumps.
- Keep names stable so failures are easy to compare across runs.

## Assertion Guidelines

- Prefer `DMibExpect(...)`, `DMibExpectTrue(...)`, and `DMibExpectFalse(...)` for normal checks.
- Guard dependent assertions after pointer/null checks to avoid secondary crashes or noisy failures.
- Use `DMibExpectException(...)`, `DMibExpectViolatesRequire(...)`, and related helpers for exception/contract behavior.
- Use `DMibTestExpr(...)` when the test is specifically about expression evaluation behavior.
- Use `DMibAssert*` when later checks in the same block would be meaningless or unsafe after a failure.

### `DMibAssert*` vs `DMibExpect*`

- `DMibExpect*` records a failure and continues.
- `DMibAssert*` is the fail-and-stop form for the current test path.
- Prefer `DMibAssert*` for preconditions inside a test, such as iterator validity, pointer validity, or setup that must succeed before the rest of the checks make sense.

Available assert helpers include:

- `DMibAssert(...)`
- `DMibAssertTrue(...)`
- `DMibAssertFalse(...)`
- `DMibAssertException(...)`
- `DMibAssertExceptionType(...)`
- `DMibAssertNoException(...)`
- `DMibAssertViolatesRequire(...)`
- `DMibAssertViolatesCheck(...)`
- `DMibAssertViolatesSafeCheck(...)`

Example:

```cpp
auto *pValue = f_FindValue();
DMibAssertTrue(pValue != nullptr);
if (pValue)
	DMibExpect(*pValue, ==, 42);
```

In async tests this is especially useful after `co_await` when a missing value would make later checks misleading.

## Performance Tests and Groups

- Put benchmarks and timing-sensitive checks in the `Performance` group with `CTestGroup("Performance")`.
- Follow existing patterns such as `Malterlib/Test/Test/Test_Malterlib_Test.cpp` and `Malterlib/Concurrency/Test/Test_Malterlib_Concurrency_CoRoutinesPerformance.cpp`.
- Performance tests should usually be isolated in their own suite or category, not mixed into ordinary correctness suites.
- When a performance assertion should only run if that group is active, gate it with `fg_GroupActive("Performance")`.

Example:

```cpp
DMibTestCategory("Performance")
{
	DMibTestSuite("Expressions")
	{
		// gather timings
		if (NMib::NTest::fg_GroupActive("Performance"))
			DMibExpect(TimeCycles, <, ExpectedCycles);
	};
};
```

For heavier or opt-in tests, use the appropriate built-in group instead:

- `Manual` for tests requiring user action or special environment
- `SuperUser` for elevated privileges
- `Memory` for memory-focused validation
- `Expensive` or `Torture` for long-running stress coverage
- `Unfinished` for intentionally incomplete or exploratory tests

## Naming and Style

- Test classes follow normal class naming, e.g. `CGeneral_Tests`.
- Local lambdas/function objects should follow the normal function-object naming convention, e.g. `fCheckOwner`.
- Keep test code small and direct; avoid building unnecessary helpers.
- Use realistic repository paths, filenames, and data when practical, especially for path- and build-system-related tests.

## Practical Patterns

### Group several success cases in one suite

This is preferred when the setup is shared and each case gets its own `DMibTestPath(...)`.

```cpp
DMibTestSuite("FindContainingPath_FindsOwner")
{
	auto fCheckOwner = [&](CStr const &_Case, CStr const &_Path)
		{
			DMibTestPath(_Case);
			DMibExpectTrue(fg_HasOwner(_Path));
		}
	;

	fCheckOwner("ExactRoot", "/Root");
	fCheckOwner("NestedRepo", "/Root/Sub");
};
```

### Split success and failure behavior into separate suites

This usually reads better than mixing both in the same suite.

## Running Tests

Common commands:

```bash
# Build a specific test target
MalterlibBuildShowProgress=false ./mib build-target Tests Com_Test_Malterlib_Test

# Run one test executable directly
/opt/Deploy/Tests/Test_Malterlib_Test

# Run all tests through the runner
/opt/Deploy/Tests/RunAllTests --paths '["Malterlib/Test/*"]'
```

When building and then running a test binary manually, run them sequentially so you do not accidentally execute a stale binary.

## Examples To Follow

- `Malterlib/Concurrency/Test/Test_Malterlib_Concurrency_CoRoutines.cpp`
- `Malterlib/Concurrency/Test/Test_Malterlib_Concurrency_CoRoutinesPerformance.cpp`

- `Test_Malterlib_Concurrency_CoRoutines.cpp` shows the canonical async test style using `-> TCFuture<void>` as well as sync tests.
- `Test_Malterlib_Concurrency_CoRoutinesPerformance.cpp` shows grouped async performance suites with `CTestCategory(...) << CTestGroup("Performance")`.
