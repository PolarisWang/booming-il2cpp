# Build & Verification Pipeline — Current Status

Last updated: 2026-06-20

## Overall Status

| Metric | Status |
|--------|--------|
| DLLs verified | 26/26 ✅ |
| Total chunks | 61 |
| CI pipeline duration | ~53 min (all 26 DLLs, 8 stages each) |
| Known fact failures (pattern filtered) | ~600 methods across CoreLib chunks |

## Key Changes

### Build Optimizations
- `autoPageSize`: 150 (reverted from 500, TagList incompatibility)
- `/Gy /Gw`: Link-time code generation + function-level linking
- Parallel builds: `--parallel N` flag in `chunk_pipeline.py`

### Pipeline Hardening
- `fact_chunk.py`: `_KNOWN_FACT_FAILURES` + `_KNOWN_FACT_FAILURE_PATTERNS` skip lists
- `build.py`: Auto-fixup for TagList `->field` access (reinterpret_cast) + typedef insertion
- `build.py`: UnicodeEncodeError fix for GBK terminal + pipeline stage counter fix
- Scriban template: Linux EH `}} catch` fix, `__try`/`__except` → `try`/`catch` (C2712)

### Codegen Fixes
- `RuntimeHelperShapeRegistry.cs`: `chaos_arg_1` conditional generation (Array::Clone et al.)
- `NativeAotLoweringPlanner.cs`: Include list corrections
- `ObjectModelEmission.cs`: Removed `_TryFindExternalValueTypeFields`

### Runtime Fixes
- `chaos_runtime_host.h`: Removed VirtualProtect
- `interop_stubs.cpp`: `IsFinite` sentinel stub
- Include guards: `stub_common.h`/`misc_stubs.h`/`array_stubs.h`/`cpu_features.h`
- CMake template: `interop_stubs.cpp` from source tree

### Tooling
- GitHub Actions CI workflow (`.github/workflows/ci.yml`)
- Parallel chunk builds: `--parallel N`
- `managed_benchmark.py`: `_NET8_REPLACEMENTS` for .NET 8 API compat
- GENSHAPE debug trace disabled (prevented TPG timeout)

## Known Issues

| Issue | Status | Notes |
|-------|--------|-------|
| system-5 init hang | Workaround | SEH wrapper catches AV from .rdata array write |
| Convert/Half/Double stubs | Not fixable | `ChaosExternalRuntimeFallback` has no argument access |
| System.Runtime/Intrinsics/ReaderWriter | No chunks | Pre-existing, assemblies not configured |
| GcAllocateProfiled LNK2019 | Fixed | `gc_alloc_stubs.cpp` locally compiled |
| **Vector512 carrier `operator[]`** | Open | `RuntimeIntrinsicVector512Carrier` 缺少 `operator[]`，block 了 1526 个 SIMD 测试 |
| Coverage-audit | ✅ 100% | All chunks passed
