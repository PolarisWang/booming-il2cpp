# A-P3-1: G6 Generics/Sharing — Generic Instantiation Codegen Closure

## Status: Complete

## Accomplished

### Build Infrastructure Fixes
- Fixed `native/CMakeLists.txt` for all 3 generics families: corrected codegen output path glob (`*Subjects/generated/` → `generated/`), added `runtime_stubs` include dir, added `enum_stubs.cpp` direct source (pre-built lib missing this symbol)
- Fixed `EnumDispatchEntry` struct redefinition in `EnumMetadataExtractor.cs` (added `#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_` guard)

### G6-1: Cross-module generic AOT entries ✅
- **Root cause identified**: Module ID mismatch between registration (AOT root module ID) and query (definition's module ID) caused cross-module AOT entries to be silently skipped
- **Managed codegen** (`GenericRegistration.cs`): `BuildMethodAotRegistration` now separates same-module and cross-module entries, sorted by (definition assembly name, open_token) for deterministic output
- **Scriban templates**: `GenericRegistration.cpp.scriban` emits `s_cross_module_aot_entries[]` and `s_cross_module_aot_entry_args[]`; `GenericAotRegistration.cpp.scriban` emits `RegisterCrossModuleMethodAotEntries()` call
- **C++ runtime** (`aot_method_map.cpp`): Added `CrossModuleAotEntry` struct, fixed-size fallback array (4096 entries, linear scan), `RegisterCrossModuleMethodAotEntries()` and `QuerySingleModule()` helper; `QueryAotMethod()` falls through to cross-module scan on per-module miss
- **C++ header** (`instantiation_engine.h`): Added `RegisterCrossModuleMethodAotEntries()` declaration

Files modified:
- `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GenericRegistration.cs`
- `src/managed/Chaos.IL2CPP.Generator/Templates/NativeAot.GenericRegistration.cpp.scriban`
- `src/managed/Chaos.IL2CPP.Generator/Templates/NativeAot.GenericAotRegistration.cpp.scriban`
- `src/native/runtime-core/instantiation_engine.h`
- `src/native/runtime-core/instantiation/aot_method_map.cpp`

### G6-2: RuntimeInstantiationBridge (Pseudo-token fallback) ✅
- **Problem**: `BuildGenericTypeRegistration`, `BuildGenericMethodRegistration`, and `BuildMethodAotRegistration` skipped entries when type arguments couldn't be resolved to concrete metadata tokens
- **Solution**: `MetadataTokenLookup.TryGetTypeToken` now falls back to a deterministic FNV-1a pseudo-token (`0x02HHHHHH`) when the type isn't in the metadata registration. This matches the existing `CreatePseudoTypeToken` function in `NativeReferenceProofEmitter.cs` and `ShapeValidation.cs`, ensuring consistent token generation.
- **Impact**: No entries are silently dropped. Previously skipped entries now get valid pseudo-tokens and are included in the GenericContextRegistry and AOT method map.

Files modified:
- `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.GenericRegistration.cs` — `MetadataTokenLookup` class, 3 registration methods simplified

### G6-6: Generics test family verification ✅
| Family | Fact Result | HotUpdate AOT Bench | Status |
|--------|-------------|---------------------|--------|
| collections-generic-core | 16/16 ✅ | 16/16 ✅ | ✅ Pass |
| generic-nullable-value | 10/10 ✅ | 10/10 ✅ | ✅ Pass |
| reflection-generics | 12/12 ✅ | 12/12 ✅ | ✅ Pass |

### G6-4: Reflection boundary handling ✅
- Reflection boundary detection already exists in `GenericCapabilityMatrixBuilder.cs`
- The `reflection-generics` test family (12/12) validates correct behavior of reflection-based generic type resolution
- Existing infrastructure (GenericContextRegistry, interpreter fallback) handles reflection boundaries correctly

### G6-5: Structure review ✅
1. **File responsibility**: `GenericRegistration.cs` is well-scoped — type/method registration, AOT entry building, and token lookup are related responsibilities
2. **Duplicate code**: `CreatePseudoTypeToken` exists in 3 files (pre-existing); token resolution pattern is now unified across all 3 registration methods via the `MetadataTokenLookup.TryGetTypeToken` fallback
3. **Scriban layering**: Templates remain pure rendering; cross-module entries use consistent `GenericMethodAotEntryV0` format

## Summary

All G6 sub-tasks are complete:

| Task | Status | Description |
|------|--------|-------------|
| G6-1 | ✅ Complete | Cross-module AOT entries — managed codegen + Scriban + C++ runtime fallback map |
| G6-2 | ✅ Complete | Pseudo-token fallback for unresolvable type parameters — no entries silently dropped |
| G6-3 | ✅ N/A | Generic value type specialization (maintain existing behavior, no change needed) |
| G6-4 | ✅ Complete | Reflection boundary handling verified working via existing infrastructure |
| G6-5 | ✅ Complete | Structure review completed — no major issues |
| G6-6 | ✅ Complete | All 3 generics test families verified passing in foundation-dll pipeline |
