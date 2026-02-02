# CLion/CMake Testcase Support (Summary)

## Goal
Enable CLion indexing and allow running/debugging each testcase via CMake/CTest without converting the whole project away from Makefile.

## What was added
- A top-level `CMakeLists.txt` to provide an index-only configuration plus per-module targets.
- Per-testcase executables under the CMake build tree: `cmake-build/testcase/<name>`.
- CTest integration: each testcase has a `nemu_tc_<name>` test.
- A `check` target that builds all testcase binaries and runs the NEMU tests.
- NEMU run targets: `run_nemu_tc_<name>`.

## Key behaviors
- Testcases are built as ET_EXEC (non-PIE) and linked without system libs, matching the Makefile flow.
- NEMU tests are invoked with input `c\nq` to auto-continue and quit (same as `test.sh`).
- `mov.S` is handled by a custom rule using `ld` directly to avoid `libgcc` dependency.

## How to use
### Configure and build
```
cmake -S . -B cmake-build
cmake --build cmake-build
```

### Run all testcase checks (NEMU)
```
cmake --build cmake-build --target check
```

### Run a single testcase with CTest
```
ctest --test-dir cmake-build -R nemu_tc_add --output-on-failure
```

### Run a single testcase directly
- Native target: `tc_<name>` (e.g. `tc_add`) builds the binary.
- NEMU run target: `run_nemu_tc_<name>` (e.g. `run_nemu_tc_add`).

### Optional variables
- `TJU_NEMU_BIN` (default: `obj/nemu/nemu`) to point to NEMU.
- `TJU_NEMU_ARGS` to pass extra args to NEMU.
- `TJU_GAME_VARIANT` (`typing` or `nemu-pal`) for game indexing.

## Notes
- The system warnings about `libcurl.so.4` are unrelated to build/test results.
- Makefile workflow remains the source of truth; CMake here is for indexing and convenient test runs.

## Update (run_nemu targets)
- Added `scripts/run_nemu.sh` to ensure NEMU receives `c\nq` reliably under CMake.
- `run_nemu_tc_<name>` and `ctest` now use this script, so single-test runs work in CLion and CLI.

### Example
```
cmake --build cmake-build-debug --target run_nemu_tc_hello
```
