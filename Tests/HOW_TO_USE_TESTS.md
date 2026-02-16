# Unit Test System

This directory contains the unit test suite for the Volumetric Synth project. Tests use **JUCE's built-in `UnitTest` framework**, which is automatically discovered and run by a single console application.

## Directory Structure

```
Tests/
├── README.md           # This file
├── CMakeLists.txt      # Build configuration for the test executable
├── Main.cpp            # Entry point — runs all registered tests
└── UnitTests/          # Individual test source files
    └── SampleTest.cpp  # Example test
```

## Running Tests

1. **Build** the test target (same as the rest of the project, e.g. via your IDE or CMake):
   - The test executable is the `VolumetricSynth_Tests` target.
   - Typical build output name: **Volumetric Synth Tests** (or `VolumetricSynth_Tests` depending on generator).

2. **Execute** the test binary:
   - Run the built executable; it will run all registered tests and print results to the console.
   - Exit code is 0 on success; non-zero on failure (JUCE `UnitTestRunner` reports failures).

### CMake commands

From the project root, configure and build the test target:

```bash
# Configure (from a build directory, e.g. Build or build)
cmake -B build -S .

# Build only the test executable
cmake --build build --target VolumetricSynth_Tests
```

Run the tests (path may vary by generator and config — Debug/Release):

```bash
# Linux/macOS (Debug)
./build/VolumetricSynth_artefacts/Debug/Volumetric\ Synth\ Tests

# Or using ctest if the target is registered (optional)
ctest --test-dir build -R VolumetricSynth_Tests --output-on-failure
```

Or from your IDE: run the **VolumetricSynth_Tests** (or “Volumetric Synth Tests”) run configuration.

## Writing a New Test

### 1. Create a test class

- Subclass `juce::UnitTest`.
- Pass a short display name to the base constructor.
- Override `runTest()` and add your checks.

```cpp
#include <JuceHeader.h>

class MyFeatureTest : public juce::UnitTest
{
public:
    MyFeatureTest() : juce::UnitTest ("My Feature") {}

    void runTest() override
    {
        beginTest ("First check");
        expect (true, "Optional failure message");

        beginTest ("Equality");
        expectEquals (computeSomething(), 42);
    }
};
```

### 2. Register the test

Register the test so the runner can find it by creating a **static instance** at file scope:

```cpp
static MyFeatureTest myFeatureTest;
```

### 3. Add the file to the build

In `Tests/CMakeLists.txt`, add your new `.cpp` under `target_sources`:

```cmake
target_sources(VolumetricSynth_Tests
    PRIVATE
    Main.cpp
    UnitTests/SampleTest.cpp
    UnitTests/MyFeatureTest.cpp   # add this line
)
```

Rebuild; your test will run with all others when you execute the test app.

## Test API (JUCE UnitTest)

- **`beginTest (name)`** — Start a named subsection. Use to group related checks and get clearer failure reports.
- **`expect (condition, optionalMessage)`** — Passes if `condition` is true; otherwise fails and prints the message.
- **`expectEquals (actual, expected)`** — Passes if `actual == expected`; useful for numeric or value comparison.
- **`expectNotEquals (actual, unexpected)`** — Passes when the values differ.
- **`expectWithinAbsoluteError (actual, expected, tolerance)`** — For floats/doubles when you need a fixed tolerance.
- **`logMessage (message)`** — Print a message in the test output (e.g. for debugging).

All of these are used inside `runTest()` (and optionally inside helper methods called from `runTest()`).

>See SampleTest.cpp for an example.


## Linking and Dependencies

The test target links against:

- **VolumetricSynth** — main plugin/library code under test.
- **juce::juce_audio_plugin_client** and other JUCE libs as in the main app.

You can use any public API from the main project and from JUCE in your tests. For tests that need GUI or audio device setup, use JUCE’s test helpers or create scoped objects inside `runTest()` as needed.


For more on JUCE’s testing API, see the JUCE documentation for `UnitTest` and `UnitTestRunner`.
