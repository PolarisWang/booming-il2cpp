# Build & Verification Pipeline — Current Status

Last updated: 2026-06-19

## Overall Status

| Metric | Status |
|--------|--------|
| DLLs verified | 26/26 ✅ |
| Total chunks | 61 |
| CI pipeline duration | ~53 min (all 26 DLLs, 8 stages each) |
| Known fact failures (pattern filtered) | ~600 methods across CoreLib chunks |

## Key Changes This Session

### Build Optimizations
- `autoPageSize`: 150 → 500 → 150 (reverted, TagList incompatibility)
- `/Gy /Gw`: Link-time code generation + function-level linking
- PCH: `chaos_pch.h` with all stable runtime headers, CMake `target_precompile_headers`
- Parallel builds: `--parallel N` flag in `chunk_pipeline.py`

### Pipeline Hardening
- `fact_chunk.py`: `_KNOWN_FACT_FAILURES` + `_KNOWN_FACT_FAILURE_PATTERNS` skip lists
- `build.py`: Auto-fixup for TagList `->field` access on opaque value types (reinterpret_cast)
- `build.py`: UnicodeEncodeError fix for GBK terminal
- `runtime-entry.cpp` (system-5): SEH `__try/__except` wrapper for `OverrideUnresolvedExternalRuntimeEntries`

### Codegen Fixes
- TagList: Removed `_TryFindExternalValueTypeFields` (struct incompatible with template)
- scriban templates: PCH include paths corrected
- `NativeAotLoweringPlanner.cs`: Individual includes → `#include "chaos_pch.h"`

### Runtime Fixes
- `interop_stubs.cpp`: Added `IsFinite` sentinel stub (returns 1 for zero inputs)
- `chaos_runtime_host.h`: Removed VirtualProtect (failed for large external runtime tables)
- CMake template: `interop_stubs.cpp` compiled from source tree, not SDK prebuilt

## Known Issues

| Issue | Status | Notes |
|-------|--------|-------|
| system-5 init hang | Workaround | SEH wrapper catches AV from .rdata array write |
| Convert/Half/Double stubs | Not fixable | `ChaosExternalRuntimeFallback` has no argument access |
| System.Runtime/Intrinsics/ReaderWriter | No chunks | Pre-existing, assemblies not configured |
| Coverage-audit mismatch | Some chunks | `declared` ≠ `covered`, needs separate investigation |
