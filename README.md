# G++ / libgpp

Lightweight C++ library that provides cooperative user-level context switching primitives (a small "green threads" toolkit) for learning and experiments. The repository builds a static archive `libgpp.a` and includes a few simple test programs that demonstrate thread/process-like primitives and TLS support.

## Highlights

- Small, self-contained C++ source files: `gpp.cpp`, `proc.cpp`, `tls.cpp` and a small assembly startup `start.s`.
- Produces a static library `libgpp.a` via the top-level `Makefile`.
- Includes basic tests/examples under `test/` (hello, ping-pong, wait).

## Requirements

- Linux (or POSIX-like environment)
- GNU toolchain: `g++`, `make`, `ar`, `ld` (standard development toolchain)

## Build

From the project root, simply run:

```sh
make
```

Targets of interest:

- `all` (default): builds `libgpp.a`.
- `test`: builds the library and runs the top-level test driver which in turn runs test programs.
- `clean`: removes object files and the library and runs `make clean` in `test/`.
- `install`: installs the library and header to `/usr/local` by default (honors `PREFIX`).

To build and run the tests:

```sh
make test
```

or you can run the test script directly (it ensures `test/` is built):

```sh
./test/test.sh
```

## Running the example tests

The `test/` directory contains small example programs:

- `test/hello.cpp` — basic usage example.
- `test/pingpong.cpp` — basic context-switching or coroutine exchange demonstration.
- `test/wait.cpp` — synchronization/wait example.

The test harness builds the examples and runs a set of test scripts `test/test_*.sh`. Use `make -C test` to build only the tests.

If you prefer to build an example manually against the produced static library:

```sh
g++ -I. -o hello test/hello.cpp libgpp.a
./hello
```

(The project test Makefile may provide the exact flags; consult `test/Makefile` for details.)

## API usage

This section documents the public API exposed in `gpp.hpp` and shows short usage examples.

Top-level helper:

- `void go(void (*fn)())` — start a new lightweight gpp routine that will run `fn` concurrently. The function receives no arguments and returns void. The new routine runs cooperatively and will be scheduled by the library.

Core functions (in namespace `gpp`):

- `void Sched()` — voluntarily yield/schedule the current gpp routine. Use this when you want to let other routines run.
- `void Exit()` — terminate the current gpp routine (never returns to the caller).
- `void SetNProcs(int64_t n)` — set the maximum number of underlying OS threads (worker `M` instances) used by the runtime. Use to limit concurrency (e.g. `gpp::SetNProcs(1)` for single-OS-thread scheduling).

Condition variable (cooperative):

The `gpp::ConditionVariable` type provides basic wait/signal/broadcast functionality for gpp routines. It integrates with `std::mutex` so usage is similar to standard condition variables:

- `void wait(std::mutex &mtx)` — the caller must hold `mtx` before calling; `wait` will add the caller to the waiters list, unlock the mutex, park the routine, and then re-lock the mutex on wake.
- `void signal()` — wake a single waiting routine (if any).
- `void broadcast()` — wake all waiting routines.

Example pattern:

```cpp
std::mutex m;
gpp::ConditionVariable cv;
bool ready = false;

// consumer:
m.lock();
while (!ready) cv.wait(m); // unlocked while parked, re-locks on wake
// ... proceed

// producer:
{
	std::lock_guard<std::mutex> lg(m);
	ready = true;
}
cv.signal(); // or cv.broadcast();
```

Wait group (join-style):

`gpp::WaitGroup` implements a simple counter-based synchronization primitive resembling Go's `sync.WaitGroup`:

- `void add(int delta)` — add `delta` to the internal counter. Calling with a negative `delta` decreases the counter; assertions check against underflow.
- `void done()` — convenience for `add(-1)`.
- `void wait()` — block until the counter reaches zero.

Example (similar to `test/wait.cpp`):

```cpp
gpp::WaitGroup wg;

void worker() {
	// do work
	wg.done();
}

int main() {
	wg.add(1);
	go(worker);
	wg.wait(); // block until worker calls done()
	return 0;
}
```

Lower-level notes:

- The runtime creates user-level contexts (`GPP`) with their own stacks and schedules them onto a pool of OS threads (`M`).
- Prefer the higher-level helpers (`go`, `gpp::WaitGroup`, and `gpp::ConditionVariable`) for common patterns.
- `gpp::Exit()` should be used to terminate the current routine when you need to ensure cleanup on the system stack; otherwise returning from the goroutine's entry function will also terminate it.

For complete examples, see `test/pingpong.cpp` and `test/wait.cpp` which demonstrate `go`, `gpp::Sched()`, and `gpp::WaitGroup` usage.

## File overview

- `gpp.hpp`, `gpp.cpp` — public header and implementation for the library API.
- `proc.hpp`, `proc.cpp` — process/context management implementation.
- `tls.hpp`, `tls.cpp` — thread-local storage support.
- `start.s` — small assembly startup helper used when creating contexts.
- `Makefile` — top-level build rules (creates `libgpp.a`).
- `test/` — examples, test Makefile and shell scripts.

## Contributing

Contributions and issues are welcome. Please keep changes focused and include tests for new behavior where practical. Small, well-documented patches are easiest to review.

Suggested workflow:

1. Fork the repository.
2. Create a branch for your change.
3. Add/modify tests under `test/` if applicable and ensure `make test` passes.
4. Open a pull request describing the change.

## License

This repository does not include an explicit license file. If you want to use or distribute this code, please add a `LICENSE` file or ask the maintainers to declare a license.

## Notes

- The project is intended for educational and experimental use. It is not hardened for production.
- If you want me to expand this README with API examples (small code snippets showing how to create contexts and switch them), tell me which functions you want documented or I can scan the headers and add usage examples.

---
Generated/updated on 2026-01-05.
