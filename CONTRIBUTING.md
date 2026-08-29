# Contributing

Thanks for your interest in **chaos-il2cpp** — a C++ IL2CPP runtime with NativeAOT
codegen, precise generational GC, and hot-update support.

This project is under active development. All are welcome to report bugs,
request features, and open pull requests.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [How to Contribute](#how-to-contribute)
- [Development Conventions](#development-conventions)
- [Committing](#committing)
- [Running the Test Suite](#running-the-test-suite)
- [Reporting Issues](#reporting-issues)

## Code of Conduct

This project follows the [Contributor Covenant](.github/CODE_OF_CONDUCT.md).
Please read it before participating.

## Getting Started

Requirements:
- **Windows 10+** (primary target), Linux x64, or Android arm64
- CMake 3.20+
- .NET 10 SDK
- Visual Studio 2022+ (Windows) or clang 15+ (Linux)
- Python 3.10+

```bash
# Configure and build the native runtime
cmake --preset debug
cmake --build artifacts/presets/debug --target chaos_runtime_core

# Build the managed codegen pipeline
dotnet build src/managed/Chaos.IL2CPP.Generator

# Run the fast unit tier (~50s)
python tests/runner/test_driver.py --layer unit
```

## How to Contribute

1. **Fork** the repository and create a feature branch off `main`.
2. **Make focused, incremental changes.** Prefer many small, reviewable commits
   over one huge one.
3. **Run the test suite** for your layer before opening a PR (see below).
4. **Open a pull request.** The PR template will guide you. CI runs a full
   matrix (unit, native contracts, JIT, GC, foundation-dll) plus a hygiene gate.

Your PR is automatically checked by a `pre-commit` quality gate and reviewed by
CI. Keep changes scoped to what the issue/feature asks for.

## Development Conventions

These conventions keep the codebase consistent. New contributors are expected
to follow them for merged changes.

- **Architecture-first**: before implementing a new IL translation path, consult
  the architecture docs in [`wiki/03-功能模块/06-il2cpp核心架构/`](wiki/03-功能模块/06-il2cpp核心架构/)
  to match existing patterns.
- **4-layer boundary**: the codebase is separated into ATG / CodeGen / TPG /
  Python layers, each with a strict output boundary. Cross-layer writes require
  an explicit override marker. Relevant for changes touching code generation.
- **Priority**: P1 (performance) > P2 (design) > P3 (hot-update support).
  High-priority choices should not deliberately break lower-priority ones.
- **Memory**: no global `operator new/delete` overrides; codegen output must use
  the `CHAOS_IL2CPP_` allocator macros; keep GC / domain / raw allocations
  domain-consistent.
- **Trace-first debugging**: on failure, use the trace system before adding
  print/log statements.

## Committing

- Write clear, scoped commit messages.
- For **bug fixes**, include a `root_cause` / `fix_strategy` / `regression_check`
  summary in the message body.
- Do **not** commit build artifacts (bin/, obj/, build/, *.dll/*.o/*.a/log files,
  test stdout). These are gitignored and enforced by the pre-commit hygiene gate.

## Running the Test Suite

One command runs the whole pyramid (unit → integration → e2e):

```bash
# Full suite (can be slow)
python tests/runner/test_driver.py --layer all --quick

# Fast unit tier only
python tests/runner/test_driver.py --layer unit

# CI wrapper (preset selection)
python scripts/ci_test.py --preset managed-full
```

Reports go to `tests/runner/test-report.json`. See
[`docs/dev/tests/architecture.md`](docs/dev/tests/architecture.md) for the test
pyramid and the no-skip rule.

## Reporting Issues

Use the **Bug Report** and **Feature Request** templates in the issue tracker.
Please include:
- The `VERSION` (run `cat VERSION` or `git describe --tags`)
- Your platform (OS, architecture, toolchain)
- A minimal reproduction, if possible

**Security vulnerabilities**: do **not** open a public issue. Report privately
via `SECURITY.md`.

## Codebase Overview

See the [README](README.md) for the module layout and key dependencies.

## License

Contributions are licensed under the same [MIT](LICENSE) terms as the project.
By submitting a pull request, you agree to license your contribution under the
project's MIT license.
