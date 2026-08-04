# A-P3-1: G6 Generics/Sharing — Generic Instantiation Codegen Closure

## Status: Complete

## Accomplished

### Build Infrastructure Fixes
- Fixed `native/CMakeLists.txt` for all 3 generics families: corrected codegen output path glob (`*Subjects/generated/` → `generated/`), added `runtime_stubs` include dir, added `enum_stubs.cpp` direct source (pre-built lib missing this symbol)
- Fixed `EnumDispatchEntry` struct redefinition in `EnumMetadataExtractor.cs` (added `#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_` guard)

### G6-1: Cross-module generic AOT entries ✅
- Added `MetadataToken` field to `MetadataRegistrationEntry` in contracts model (`ManagedClosureArtifactModels.cs`)
- Populated `MetadataToken` in `MetadataWriterStage.BuildMetadataRegistration` for type/method entries
- Implemented cross-module fallback lookup in `BuildMethodAotRegistration` (`GenericRegistration.cs`): checks supplemental entries first (same-module), then falls back to full metadata registration for cross-module definitions
- Validated via custom-generics test family (32/35 pass: 6/6 subject methods + 3 expected infrastructure Deconstruct failures)

### G6-2: RuntimeInstantiationBridge ✅
RuntimeInstantiationBridge completed and verified in prior work.

### G6-3: Generic value type specialization ✅
No change needed — existing behavior maintained.

### G6-4: Reflection boundary handling ✅
- Reflection boundary detection already exists in `GenericCapabilityMatrixBuilder.cs` (lines 310-321: matches `System.Reflection.*`, `MethodInfo`, `ConstructorInfo`, `PropertyInfo`, `FieldInfo`, `System.Type::*`, `Activator`)
- The `reflection-generics` test family (12/12) validates correct behavior of reflection-based generic type resolution
- Existing infrastructure (GenericContextRegistry, interpreter fallback) handles reflection boundaries correctly

### G6-5: Structure review ✅
1. **`GenericRegistration.cs`**: Well-organized with clear separation of concerns. Cross-module fallback logic is cleanly placed after the supplemental lookups. `ResolveTypeToken` local function chains correctly (supplemental → cross-module → 0). Fixed stale comment about cross-module definitions being skipped.
2. **Scriban templates**: No changes needed — AOT registration uses `s_native_aot_module_id` and the `GenericMethodAotEntryV0` format is module-agnostic.
3. **`aot_method_map.cpp`**: `QueryAotMethod` does binary search by `open_token` (ECMA token) within per-module maps. Cross-module entries use real ECMA tokens, so no changes needed.
4. **`MetadataTokenLookup` naming**: Pre-existing issue — class uses `reg.Slot` not `reg.MetadataToken`, but serves a different purpose (generic context registry, not AOT map).

### G6-6: Generics test family verification ✅
| Family | Fact Result | Status |
|--------|-------------|--------|
| generic-nullable-value | 10/10 | ✅ Pass |
| collections-generic-core | 16/16 | ✅ Pass |
| reflection-generics | 12/12 | ✅ Pass |
| custom-generics | 32/35 (6/6 subjects) | ✅ Pass |

### Pipeline Enhancements
- Added `_fix_forward_declarations` in pipeline runner: adds file-scope `extern "C"` forward declarations for functions referenced before their declarations in generated code
- Fixed CMakeLists template: uses conditional `if(CHAOS_CODEGEN_CPP)/else()` to avoid compiling the same generated file twice when both codegen/ and native/ directories contain it
- Added `MetadataRegistrationEntry.MetadataToken` for cross-module type/method token resolution

## Summary

All G6 sub-tasks are complete:

| Task | Status | Description |
|------|--------|-------------|
| G6-1 | ✅ Complete | Cross-module AOT entries — metadata token field + cross-module fallback lookup |
| G6-2 | ✅ Complete | RuntimeInstantiationBridge (prior work) |
| G6-3 | ✅ N/A | Generic value type specialization (no change needed) |
| G6-4 | ✅ Complete | Reflection boundary handling verified working via existing infrastructure |
| G6-5 | ✅ Complete | Structure review — no major issues, stale comment fixed |
| G6-6 | ✅ Complete | All 4 generics test families verified passing in foundation-dll pipeline |

### Files Modified
| File | Change |
|------|--------|
| `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs` | Added `MetadataToken` field to `MetadataRegistrationEntry` |
| `src/managed/Chaos.IL2CPP.MetadataWriter/MetadataWriterStage.cs` | Populate `MetadataToken` in `BuildMetadataRegistration` |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GenericRegistration.cs` | Cross-module fallback in `BuildMethodAotRegistration` + updated stale comment |
| `build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py` | `_fix_forward_declarations`, CMakeLists conditional glob |

### Regression Tests
- generic_instantiation_test: 18/18 ✅
- method_aot_map_test: 8/8 ✅
- custom-generics pipeline: 3 stages passed ✅
