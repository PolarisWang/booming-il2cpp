# CodeGen D3-C Pluginization Roadmap Design v2.04

Date: 2026-04-26 23:50:00 +08:00
Status: draft-approved-for-execution

## 1. Goal

Freeze a source-generator-driven `D3-C` pluginization architecture for `Chaos.IL2CPP.CodeGen` that:

- keeps runtime and generated native performance equivalent to static registration
- prevents further growth of root switchboard files
- preserves clean hotupdate-facing authority boundaries
- covers all four plugin classes:
  - family
  - lowering plan
  - template bundle
  - runtime provider

This is a codegen architecture roadmap, not a runtime semantics roadmap.

## 2. Non-Negotiable Constraints

The approved `D3-C` shape is valid only if all of the following hold:

1. registration is generated at compile time only
2. generated registries are static and explicit
3. there is zero runtime reflection-based plugin discovery
4. source generators do collection only, not business decision-making
5. registries are split by product line, not globally merged

Rejected:

- runtime scanning
- reflection discovery
- generator-driven backend selection
- generator-driven template selection
- generator-driven provider capability inference

## 3. Plugin Classes

### 3.1 Family Plugin

Owns:

- method / subject matcher
- semantic descriptor production
- carrier schema selection
- stub plan creation

Examples:

- `Convert family`
- `Marshal platform family`
- `Task continuation family`
- `Interop+Kernel32 platform family`

### 3.2 Lowering Plan Plugin

Owns:

- entry-point-level lowering plan creation

Examples:

- reflection interop closure
- pinvoke direct call
- marshaling utf8 export
- async proof minimal

### 3.3 Template Bundle Plugin

Owns:

- template asset publication for one domain
- template lookup surface for one plugin family or plan family

This is the long-term replacement direction for global template catalog expansion.

### 3.4 Runtime Provider Plugin

Owns:

- runtime/provider registration surface on the codegen side
- provider metadata only

Examples:

- `MarshalPlatformAbiRootV1`
- `TaskRuntimeKernelV1`
- future intrinsic/platform providers

Important:

- provider semantics remain owned by provider implementations
- generator only registers providers

## 4. Product-Line Split

Registries must be separated by product line.

Minimum split:

- `NativeReference`
- `NativeAot`

Rejected:

- single global `AllPluginsRegistry`
- cross-product automatic merge

## 5. Generated Registry Set

For `NativeReference`:

- `NativeReferenceRuntimeSkeletonFamilyRegistry.g.cs`
- `NativeReferenceLoweringPlanRegistry.g.cs`
- `NativeReferenceTemplateBundleRegistry.g.cs`
- `NativeReferenceRuntimeProviderRegistry.g.cs`

For `NativeAot`:

- `NativeAotFamilyRegistry.g.cs`
- `NativeAotLoweringPlanRegistry.g.cs`
- `NativeAotTemplateBundleRegistry.g.cs`
- `NativeAotRuntimeProviderRegistry.g.cs`

Generated registries must be:

- readable
- auditable
- deterministic in ordering
- free of reflection

## 6. Attribute Metadata

### 6.1 Family Plugin Attribute

Required metadata:

- `Id`
- `ProductLine`
- `Order`
- `Kind`

`Kind` values:

- `ManagedInvoke`
- `Kernel`
- `PlatformCapability`
- `ImportedBridge`
- `Hybrid`

### 6.2 Lowering Plan Plugin Attribute

Required metadata:

- `Id`
- `ProductLine`
- `Order`

### 6.3 Template Bundle Plugin Attribute

Required metadata:

- `Id`
- `ProductLine`
- `Domain`

### 6.4 Runtime Provider Plugin Attribute

Required metadata:

- `Id`
- `ProductLine`
- `CapabilityFamily`
- `Version`
- `Order`

## 7. Generator Scope Boundary

The source generator is allowed to:

- discover attributed plugin types
- validate required metadata presence
- group plugins by product line and registry type
- generate static registration code

The source generator is not allowed to:

- infer plugin priority beyond explicit metadata
- choose runtime backend
- choose template at call time
- infer provider capability
- rewrite semantic ownership

## 8. Runtime and Performance Rule

Generated registries must have no runtime-discovery overhead.

Accepted form:

```csharp
internal static partial class NativeReferenceRuntimeSkeletonFamilyRegistry
{
    public static readonly IRuntimeSkeletonFamilyPlugin[] Plugins =
    [
        new ConvertFamilyPlugin(),
        new MarshalPlatformFamilyPlugin(),
        new TaskContinuationFamilyPlugin(),
        new InteropKernel32PlatformFamilyPlugin(),
    ];
}
```

Rejected:

- reflection enumeration
- service locator lookup
- dynamic plugin composition
- string-key runtime dispatch as the primary path

## 9. HotUpdate Rule

Pluginization is a codegen organization change only.

Patch-visible authority must remain:

- family-emitted ABI contracts
- runtime provider ABI contracts
- generated native surface

Plugin registry generation must not become a patch-visible semantic layer.

## 10. Migration Targets

Primary current debt targets:

- `NativeReferenceProofEmitter.cs`
- `NativeAotLoweringPlanner.cs`
- `NativeReferenceLoweringPlanner.PlanBuilders.cs`
- `NativeReferenceProofCatalog.cs`

### 10.1 Root Emitter Debt

Current debt:

- root emitter still owns dispatcher, metadata services, page orchestration, and shared rendering utilities

Pluginization target:

- family registration leaves the root file
- root file becomes orchestration only

### 10.2 Root Planner Debt

Current debt:

- root `NativeAotLoweringPlanner.cs` accumulates cross-domain subject constants and helper ownership

Pluginization target:

- domain constants move toward domain/plugin ownership

### 10.3 Plan Builder Debt

Current debt:

- `PlanBuilders.cs` behaves as a total bus of `TryCreate*LoweringPlan`

Pluginization target:

- lowering plan registration becomes generated

### 10.4 Template Catalog Debt

Current debt:

- `NativeReferenceProofCatalog.cs` grows as an all-template warehouse

Pluginization target:

- bundle-specific template publication replaces unbounded catalog growth

## 11. Execution Phases

### Phase 1: Family Registry Generator

Scope:

- runtime skeleton family registration only

Outcome:

- remove hand-maintained family lists from the root dispatcher

### Phase 2: Lowering Plan Registry Generator

Scope:

- `NativeReference` and `NativeAot` lowering-plan registration

Outcome:

- remove bus-style `TryCreate*` registration from root plan orchestration

### Phase 3: Template Bundle Registry Generator

Scope:

- template publication and lookup registration

Outcome:

- reduce global catalog pressure

### Phase 4: Runtime Provider Registry Generator

Scope:

- provider declaration registration only

Outcome:

- complete `D3-C`

This is intentionally last because provider registration is closest to runtime authority.

## 12. Verification Obligations

Each phase must prove:

- root dispatch still resolves families/plans deterministically
- generated registries are stable in order
- template lookup still resolves required assets
- runtime provider registration remains explicit and reviewable

Required verification classes:

- structure governance tests
- runtime template contract tests
- build green
- canonical formal for affected product line when registry wiring changes emitted outputs

## 13. Immediate Next Step

Start `Phase 1: Family Registry Generator`.

Rationale:

- highest structural payoff
- lowest semantic risk
- strongest leverage against future root-emitter re-growth
