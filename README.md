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
│   ├── tools/            — Test project generator (TPG), build helpers
└── dll/                — Foundation DLL test source

contracts/              — Native ABI contract headers + engine interface (v0)
tests/                  — Snapshot / integration / e2e test suites + runner
tests/e2e/translation/    — Foundation-DLL verification pipeline (build → fact → benchmark)
wiki/                   — Project wiki: architecture, function modules, skill system
docs/                   — Design docs, assessments, and archived historical records
scripts/                — CI and build orchestration scripts
cmake/                  — Shared CMake configuration (incl. native test factory)
third_party/            — External dependencies (fmt, scriban, unordered_dense)
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

## Install chaos-il2cpp (dotnet global tool)

```bash
# Install from local nupkg
dotnet tool install --global chaos-il2cpp --add-source <path-to-nupkg-dir>

# Verify
chaos-il2cpp --help
```

## Publish a .NET application to native

```bash
chaos-il2cpp publish MyApp.csproj --mode app --output ./out

# Run the native executable
./out/build/RelWithDebInfo/chaos_entry.exe
```

The `publish` command:
1. Builds the managed project (if `.csproj`)
2. Translates IL → C++ via the NativeAOT codegen pipeline
3. Emits `app_main.cpp` + `CMakeLists.txt`
4. Links against the prebuilt chaos runtime SDK (embedded in the tool package)
5. Produces a standalone native executable (`chaos_entry.exe`)

### Options

| Flag | Description |
|------|-------------|
| `--mode app` | Pure application entry (default) |
| `--mode test` | Test harness with `--fact-json` / `--benchmark-all` |
| `--output <dir>` | Output directory (default: `<input>/output`) |
| `--config-tier check\|profile\|ship` | Build config tier (default: check) |
| `--source-only` | Emit C++ source only, skip native build |
| `--clean` | Clean output directory before build |
| `--jit` | Enable JIT mode (default: AOT) |

### Example: HelloWorld

```bash
chaos-il2cpp publish tests/fixtures/public-smoke/HelloWorld/HelloWorld.csproj \
  --mode app --output /tmp/hello --clean
/tmp/hello/build/RelWithDebInfo/chaos_entry.exe
# Output: HelloWorld smoke entry reached.
# Exit code: 0
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
- [Scriban](https://github.com/scriban/scriban) — Template engine for code generation
- [system.reflection.metadata](https://github.com/dotnet/runtime) — .NET metadata reader (vendored)
- [unordered_dense](https://github.com/ankerl/unordered_dense) — Fast hash map (header-only, vendored)

## Product Release

A versioned release is orchestrated by `scripts/release.sh` (bump branch + sdk + nupkg + checksums + sbom + GitHub Release), delegated to the shared stage helpers under `scripts/`:

```bash
# Dry run (shows every step, changes nothing)
scripts/release.sh 0.2.0 --dry-run

# Real publish (release/0.2.x branch + v0.2.0 tag + SDK + nupkg + GitHub Release)
scripts/release.sh 0.2.0 --publish
```

The embedded-SDK tool package is built by `scripts/build-tool-package.sh` (a `.nupkg` containing the `chaos-il2cpp` CLI + the native runtime libs + headers).

The internal verification pipeline (`tests/e2e/verification/chunk_pipeline.py`) validates that .NET assemblies translate + run correctly (fact / benchmark / coverage / hot-update) — it is the testing half of the same IL→C++ capability that `publish` exposes to external users.

## License

MIT — see [LICENSE](LICENSE).

## Contributing

See [CLAUDE.md](CLAUDE.md) for development conventions and architecture-first development workflow.
