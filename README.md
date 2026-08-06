# chaos-il2cpp

A high-performance C++ IL2CPP runtime for .NET applications, designed with hot-update support, precise generational GC, and NativeAOT code generation.

## Overview

chaos-il2cpp translates .NET IL code to native C++ via a custom codegen pipeline, then compiles it into native binaries. It features:

- **NativeAOT Code Generation** — IL-to-C++ translation with lowering optimization
- **Precise Generational GC** — Bump-allocated young generation + mark-compact old generation
- **Hot Update Support** — Runtime method replacement without process restart
- **IL Interpreter** — Fallback execution for dynamic/hot-updated code paths
- **Full Reflection** — Runtime type/metadata resolution for managed code compatibility

## Project Structure

```
src/
├── managed/            — C# IL2CPP codegen pipeline (planning → emission → lowering)
├── native/             — C++ runtime core, GC, interpreter, bootstrap
│   ├── common/         — Zero-dependency foundation library (log, trace, span)
│   ├── runtime-core/   — Runtime core: memory, GC, reflection, vtable, marshaling
│   ├── interpreter/    — IL interpreter with fast dispatch
│   ├── bootstrap/      — Runtime bootstrapping
│   ├── hot-update/     — Method replacement infrastructure
│   ├── support/        — Runtime support layer
│   └── engine-bridge/  — Game engine integration bridge
├── tools/              — Test project generator (TPG), build helpers
└── dll/                — Foundation DLL test source

contracts/              — Native ABI contract headers + engine interface (v0)
tests/                  — Snapshot / integration / e2e test suites + runner
testing/                — Foundation-DLL verification pipeline (build → fact → benchmark)
tools/                  — Pipeline / benchmark / profile helper scripts
wiki/                   — Project wiki: architecture, function modules, skill system
docs/                   — Design docs, assessments, and archived historical records
scripts/                — CI and build orchestration scripts
cmake/                  — Shared CMake configuration (incl. native test factory)
third_party/            — External dependencies (fmt, mono.cecil, scriban, unordered_dense)
.ai/                    — Agent skill system (discovery, registry, hook runtime)
.claude/                — Claude Code harness configuration, skill routing stub, worktrees
schemas/                — JSON schemas for pipeline/codegen artifacts
```

## Prerequisites

- Windows 10+ (primary target) / Linux x64 / Android arm64
- CMake 3.20+
- .NET 10 SDK
- Visual Studio 2022+ (Windows) or clang 15+ (Linux)
- Python 3.10+

## Quick Start

```bash
# Configure
cmake --preset debug

# Build native runtime
cmake --build artifacts/presets/debug --target chaos_runtime_core

# Build managed codegen
dotnet build src/managed/Chaos.IL2CPP.Generator

# Run snapshot tests
dotnet test tests/snapshots/Chaos.IL2CPP.Generator.SnapshotTests
```

## Testing

One command runs the whole suite (unit → integration → e2e):

```bash
# Full suite
python tests/runner/test_driver.py --layer all --quick

# Fast unit tier only (~50s)
python tests/runner/test_driver.py --layer unit

# CI wrapper (same engine, preset selection)
python scripts/ci_test.py --preset managed-full
```

A unified JSON report is written to `tests/runner/test-report.json` (add
`--junit out.xml` for CI). See [docs/dev/tests/architecture.md](docs/dev/tests/architecture.md)
for the pyramid, the no-skip rule, and how to add tests.

## Build Configurations

| Preset | Log Level | Trace | Assert | Profile | Use Case |
|--------|-----------|-------|--------|---------|----------|
| `debug` | DEBUG (3) | On | On | On | Daily development |
| `profile` | INFO (2) | On | Off | On | Performance analysis |
| `ship` | ERROR (0) | Off | Off | Off | Release |

## Key Dependencies

- [fmt](https://github.com/fmtlib/fmt) — Type-safe string formatting
- [Mono.Cecil](https://github.com/jbevain/cecil) — .NET assembly inspection
- [Scriban](https://github.com/scriban/scriban) — Template engine for code generation

## License

MIT — see [LICENSE](LICENSE).

## Contributing

See [CLAUDE.md](CLAUDE.md) for development conventions and architecture-first development workflow.
