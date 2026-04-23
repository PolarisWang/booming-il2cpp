# Loader/IR-First Canonical Generic Hard Cut Design v1.02

Date: 2026-04-22 00:23:44 +08:00
Status: drafted

## 1. Architecture Objective

This design hard-cuts the repository's generic execution model from:

- `closed clone/materialization`
- `subject-id string matching`
- `per-instantiation native body`
- ad-hoc hotupdate registry/string dispatch

to:

- `canonical generic definition`
- `GenericInstantiationKey`
- `SharedGenericBodyId`
- `InstantiationStubId`
- `RuntimeGenericContextArtifact`
- `GenericDiagnosticArtifact`
- `shared body + runtime context + thin stub`
- host/patch dual registry with bridge

The execution authority is no longer `subjectId`, metadata token, or `type_id` switch. Those remain diagnostic, reflection, or runtime layout surfaces, but not the final generic execution authority.

## 2. Frozen Decisions

### 2.1 Core Route

- Route: `loader/IR-first hard cut`
- No long-lived dual-path
- No code-level emergency rollback / kill switch
- `Task<T>`, `ValueTask<T>`, pointer/byref, and `ref struct` are in first-round scope
- Patch side supports dynamic generic instantiation

### 2.2 Identity / Authority / Diagnostics

- `GenericInstantiationKey` is the canonical semantic identity
- `SharedGenericBodyId` identifies reusable generic body families
- `InstantiationStubId` is the final execution authority
- `subjectId` remains managed-facing and diagnostic-facing, not execution-facing
- `GenericDiagnosticArtifact` stores human-readable identity/decision context
- `RuntimeGenericContextArtifact` is execution-only; diagnostics do not live inside it

### 2.3 Specialization Policy

- Runtime path prefers shared body first
- Unsupported shared cases fall back to runtime specialization
- Pointer/byref/ref struct use the formal matrix:
  - `legal`
  - `shared`
  - `specialized`
  - `forbidden`

### 2.4 HotUpdate / Versioning / Lifecycle

- Host registry and patch registry are independent authorities
- Bridge maps canonical identities across the two registries without rewriting the peer registry
- Dynamic body/stub/context registration uses immutable snapshot plus atomic swap
- Handles are generation-scoped; unload/rollback makes old handles deterministically stale
- Versioning is two-layered:
  - `PackageFormatVersion` for hotupdate package/container format
  - `KernelArtifactVersion` for body/stub/context/bridge/metadata execution ABI
- `TargetAotVersion` remains runtime build compatibility input, not execution ABI authority
- HotUpdate verification and mode naming must not use external product names; canonical mode names are:
  - `HotUpdateNone`
  - `HotUpdateMetadataAugmented`
  - `HotUpdateGenericSharing`

### 2.5 Reflection / Dispatch / Carrier Policy

- Reflection first round supports construction and invoke
- Reflection invoke must support `byref`, pointer, and `ref struct`
- HotUpdate bridge must also support advanced carriers; this requires explicit carrier schema, generated thunks, write-back rules, pinning rules, and lifetime rules
- Runtime surfaces use stable reason codes/status; verbose diagnostics are separated

## 3. Component Model

### 3.1 Contracts / Naming

Primary files:

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureModels.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedNaming.cs`
- `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs`

Responsibilities:

- Define `GenericInstantiationKey`, `SharedGenericBodyId`, `InstantiationStubId`
- Replace `GenericContextArtifact` with runtime/diagnostic split artifacts
- Make parser and naming authority singular
- Define reason-code and support-matrix facing contract fields

### 3.2 Loader

Primary files:

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericMaterialization.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.CrossAssemblyInstantiation.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.MetadataResolution.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.AssemblyLoading.cs`

Responsibilities:

- Replace clone/materialization as primary path with instantiation demand graph
- Emit open definition, canonical instantiation, specialization policy, and family classification inputs
- Stop treating `MethodSpec`/`MemberReference` clone output as final generic execution payload

### 3.3 AOT Core IR

Responsibilities:

- Represent open generic definition explicitly
- Represent shared body, runtime context, and thin stub explicitly
- Make call sites consume `InstantiationStubId`
- Preserve diagnostic surfaces without giving them execution authority

### 3.4 Planner / Emitter / Runtime

Primary files:

- `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ReflectionObjectEmission.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/**`

Responsibilities:

- Consume shared-generic IR instead of family-specific string pattern logic
- Generate shared body and thin stub outputs from family registry/policy
- Route dispatch and reflection through runtime stub/context authority
- Keep generated native surfaces auditable

### 3.5 HotUpdate

Primary files:

- `src/managed/Chaos.IL2CPP.HotUpdate/RuntimeManager.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateMethodRegistry.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/BridgeDispatcher.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/BridgeGenerator.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/SupplementalMetadataWriter.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/SupplementalMetadataRegistry.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/PackageReader.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/PackageValidator.cs`

Responsibilities:

- Maintain host and patch local registries
- Bridge canonical generic identities and generated carrier thunks
- Enforce `PackageFormatVersion` and `KernelArtifactVersion`
- Carry canonical instantiation plus runtime hints in hotupdate artifacts

## 4. Data Flow

1. Managed closure discovers open generic definitions and instantiation demand.
2. Contracts normalize each candidate into `GenericInstantiationKey`.
3. Loader produces instantiation demand graph and family classification inputs.
4. AOT IR emits:
   - `SharedGenericBodyId`
   - `InstantiationStubId`
   - `RuntimeGenericContextArtifact`
   - support/specialization policy
5. Planner/emitter generate shared body, thin stub, runtime support, manifest, and audit artifacts.
6. Runtime executes through `InstantiationStubId`, resolving generation-scoped body/context.
7. Dispatch/reflection/hotupdate use the same authority model instead of parallel legacy routing.

## 5. Runtime Authority Rules

- `subjectId` is not allowed to be the final execution key
- Metadata token is not allowed to be the final execution key
- `type_id` may still participate in runtime layout/object compatibility, but not as the generic execution authority
- `InstantiationStubId + generation` is the final execution identity for runtime entry resolution
- Stale handles return stable reason codes

## 6. HotUpdate Topology

### 6.1 Registry Model

- Host registry owns host-local body/stub/context assets
- Patch registry owns patch-local body/stub/context assets
- Bridge maps canonical identities and generated carrier thunks between the two registries
- Runtime specialization fallback caches stay domain-local:
  - host-local cache
  - patch-local cache

### 6.2 Version Model

- `PackageFormatVersion` validates manifest/container layout
- `KernelArtifactVersion` validates execution ABI compatibility across:
  - runtime context payload
  - bridge thunk payload
  - generated carrier schema
  - supplemental metadata/runtime hint contract
- `TargetAotVersion` validates build/runtime alignment only

### 6.3 Advanced Carrier Rules

- `byref` requires explicit slot/write-back semantics
- Pointer requires explicit borrowed/pinned rules
- `ref struct` is call-bounded and must not be captured into long-lived registry state or async state
- Generated HotUpdate thunks must materialize carrier conversion explicitly; `object?[]` is transitional only and not acceptable as final authority

## 7. Error Model

- Runtime surfaces expose stable status/reason codes
- Detailed diagnostics live in `GenericDiagnosticArtifact` and logs
- Unsupported matrix states must fail explicitly, not silently fall back to legacy clone path
- Reflection/hotupdate failures must be distinguishable from package/version/capability failures

## 8. Verification Model

Formal verification must cover:

- contracts/parser unit tests
- loader/IR shape gates
- generic matrix
- native owner proofs
- HotUpdate proofs
- generated code review
- generated size/symbol/memory benchmarks

Verification order is fixed:

`contracts -> loader/IR gates -> native proof -> HotUpdate proof -> benchmark`

## 9. Cutover Invariants

- No long-lived dual-path
- No reopening `GenericContextArtifact` as execution contract
- No keeping metadata-only consumer compatibility layer
- No external-product naming in HotUpdate gates/modes
- No marking the roadmap complete until generated code review and formal verification are part of the checklist, not optional follow-up work
