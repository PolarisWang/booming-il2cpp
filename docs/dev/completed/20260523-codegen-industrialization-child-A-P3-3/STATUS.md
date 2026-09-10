# A-P3-3: G9-G12 — Final Thread A Codegen Gaps

> **Parent:** 20260523-codegen-industrialization
> **Child Execution:** auto (roadmap-child-auto)
> **Clearance Source:** parent-roadmap

---

## Phase

`completed`

## Lifecycle Status

`completed`

## Source Relation

`roadmap-child`

## Summary

A-P3-3 completed all four remaining codegen gaps before codegen industrialization can be declared complete:

### G9: Bridge/Import Thunk Lowering ✅
- `BridgeImportThunkDefinition` record in Models.cs
- `CollectBridgeImportThunks` heuristic InternalCall detection in InvocationPlanning.cs
- `EmitBridgeImportThunk` with GC transition + parameter forwarding in ExceptionEmission.cs
- Integration into `NativeAotLoweringPlanner.Create` pipeline

### G10: Irreducible CFG pc-dispatch Emitter ✅
- `IRPcDispatch`/`PcDispatchCase` IR nodes in StructuredIR.cs
- `BuildPcDispatchBody` + `EmitPcDispatch` with `int32_t chaos_pc` state machine
- `TryBuildStructuredMethodBody` fallback for irreducible CFGs (replaces goto emission)
- `s_pcDispatchCount` metric counter

### G11: Multi-assembly Compilation Hardening ✅
- `CrossAssemblySymbolRegistry` — FNV-1a based symbol conflict detection with disambiguation suffixes
- `AssemblyExportRegistry` — cross-assembly call export tracking
- Enhanced `FilterResultPerAssembly` in CodeGenStage.cs with global coordination phase
- Filtered CodeRegistration + per-assembly NativeAotLoweringPlan generation
- `NativeAot.CrossAssemblyExports.cpp.scriban` Scriban template for extern "C" exports

### G12: D3-C Phase 2-4 Pluginization ✅ (Phase 2 complete)
- Enhanced `LoweringPlanPluginAttribute` with `TargetCapabilityFamilies`
- `LoweringPlanRegistry` with capability-matched plugin dispatch
- Build verified: 0 errors, 0 warnings

## New Files Created

| File | Purpose |
|------|---------|
| `CrossAssemblySymbolRegistry.cs` | Cross-assembly symbol conflict detection |
| `AssemblyExportRegistry.cs` | Cross-assembly call export tracking |
| `LoweringPlanRegistry.cs` | Capability-matched lowering plan dispatch |
| `Templates/NativeAot.CrossAssemblyExports.cpp.scriban` | Extern "C" export declarations template |

## Modified Files

| File | Changes |
|------|---------|
| `CodeGenStage.cs` | Enhanced FilterResultPerAssembly with registries |
| `NativeAotTemplateCatalog.cs` | Cross-assembly exports template constant |
| `CodeGenPluginAttributes.cs` | TargetCapabilityFamilies in LoweringPlanPluginAttribute |
| NativeAotLoweringPlanner (multiple files) | Bridge thunks, pc-dispatch |

## Blockers

None.

## Next

Codegen industrialization roadmap全部收官。Parent STATUS.md should be updated to reflect all Thread A tasks completed.
