# Canonical Generic Runtime Kernel Roadmap v1.02

Date: 2026-04-22 00:23:44 +08:00
Status: completed

## 1. Goal

Establish a `loader/IR-first` canonical generic runtime kernel and hard-cut the repository from clone/materialization-driven generic execution to:

- canonical generic identity
- instantiation demand graph
- shared generic AOT IR
- runtime generic context
- thin instantiation stub
- HotUpdate dual-registry bridge

Completion is defined by unified execution authority and formal verification coverage, not by passing a few family-level proofs.

## 2. Scope

This roadmap is responsible for:

- contracts and naming unification
- loader instantiation graph hard cut
- shared-generic AOT IR
- planner/emitter/runtime cutover
- dispatch/reflection/HotUpdate cutover
- generic matrix and gates
- generated artifact review
- legacy generic path purge

## 3. Non-Goals

- onboarding every BCL generic family in one pass
- unrelated codegen cleanup outside the generic kernel route
- keeping long-lived compatibility paths for legacy generic execution

## 4. Frozen Decision Matrix

### 4.1 Core Route

- Route: `loader/IR-first hard cut`
- First-round scope includes `Task<T>`, `ValueTask<T>`, pointer/byref, `ref struct`
- No metadata-only compatibility layer
- No emergency kill switch / code-level rollback
- Patch side supports dynamic generic instantiation

### 4.2 Authority / Identity

- `GenericInstantiationKey` = semantic identity
- `SharedGenericBodyId` = reusable body family identity
- `InstantiationStubId` = final execution authority
- `subjectId` = diagnostic and managed-facing identity only

### 4.3 Runtime / Diagnostics / Error

- Diagnostics are split from execution contract
- Runtime uses `RuntimeGenericContextArtifact`
- Diagnostics use `GenericDiagnosticArtifact`
- Runtime surfaces expose stable reason codes/status

### 4.4 Specialization / Matrix

- shared body first, runtime specialization fallback second
- support matrix uses:
  - `legal`
  - `shared`
  - `specialized`
  - `forbidden`
- each family owns its own growth budget

### 4.5 HotUpdate / Lifecycle / Versioning

- host registry and patch registry are independent
- bridge only maps canonical identities and generated carriers
- body/stub/context registration is snapshot-based atomic swap
- handles are generation-scoped and deterministically stale after unload/rollback
- version topology is two-layered:
  - `PackageFormatVersion`
  - `KernelArtifactVersion`
- HotUpdate mode/gate names do not contain external product names:
  - `HotUpdateNone`
  - `HotUpdateMetadataAugmented`
  - `HotUpdateGenericSharing`

### 4.6 Reflection / Carrier Policy

- reflection first round supports construction + invoke
- reflection invoke supports `byref`, pointer, and `ref struct`
- HotUpdate bridge also supports advanced carriers
- advanced carriers require explicit carrier schema, generated thunk, write-back, pinning, and lifetime rules

## 5. Global Cutover Rules

- Do not keep long-lived dual-path execution
- Do not let `subjectId`, metadata token, or `type_id` become the final generic execution authority
- Do not reintroduce `GenericContextArtifact` as authoritative runtime contract
- Do not mark any phase complete without generated artifact review and formal verification evidence
- If a workstream grows beyond one clear authority boundary, split it into more child roadmaps; roadmap count is not constrained

## 6. Child Task Map

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260421-03-canonical-generic-contracts-and-parser-unification` | `contracts-and-naming` | `completed` | `codex` | unify parser, canonical identity, runtime/diagnostic contracts | `20260421-02` |
| `20260421-04-loader-instantiation-graph-hard-cut` | `loader-instantiation-graph` | `completed` | `codex` | replace clone/materialization with instantiation demand graph | `20260421-03-canonical-generic-contracts-and-parser-unification` |
| `20260421-05-shared-generic-aot-ir-hard-cut` | `shared-generic-ir` | `completed` | `codex` | introduce shared body/runtime context/thin stub into AOT IR | `20260421-03-canonical-generic-contracts-and-parser-unification`, `20260421-04-loader-instantiation-graph-hard-cut` |
| `20260421-06-generic-matrix-and-ir-gates` | `verification-and-gates` | `completed` | `codex` | build support matrix, IR gates, budgets, generated review gates | `20260421-03-canonical-generic-contracts-and-parser-unification`, `20260421-05-shared-generic-aot-ir-hard-cut` |
| `20260421-01-aot-generic-task-lowering-normalization` | `planner-emitter-runtime-cutover` | `completed` | `codex` | first family-level entry: align async generic family with new kernel | `20260421-05-shared-generic-aot-ir-hard-cut`, `20260421-06-generic-matrix-and-ir-gates` |
| `20260421-07-dispatch-reflection-hotupdate-cutover` | `dispatch-reflection-hotupdate` | `completed` | `codex` | move dispatch/reflection/HotUpdate to stub/context authority | `20260421-01-aot-generic-task-lowering-normalization` |
| `20260421-08-legacy-generic-path-purge` | `legacy-purge` | `completed` | `codex` | remove clone/materialization and scattered generic fallback logic | `20260421-07-dispatch-reflection-hotupdate-cutover` |

## 7. Executable Checklist

### 7.1 `20260421-03-canonical-generic-contracts-and-parser-unification`

#### Functional Requirements

- [ ] Freeze a single parser authority for generic type/method/type+method identity
- [ ] Introduce or finalize `GenericInstantiationKey`, `SharedGenericBodyId`, `InstantiationStubId`
- [ ] Introduce or finalize `RuntimeGenericContextArtifact`
- [ ] Introduce or finalize `GenericDiagnosticArtifact`
- [ ] Remove `GenericContextArtifact` from authoritative execution-facing contract surfaces
- [ ] Preserve managed-facing `subjectId` only as diagnostic/UX surface
- [ ] Encode support matrix, specialization kind, and reason-code-facing fields in contracts

#### File Audit

- [ ] Audit and update `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureModels.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.Contracts/ManagedNaming.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs`
- [ ] Audit parser/helper files that still duplicate generic parsing logic
- [ ] Mark legacy contract fields as removed, transitional, or blocked with explicit follow-up

#### Architecture Review

- [ ] Verify parser authority is singular
- [ ] Verify diagnostic contract and execution contract are separated
- [ ] Verify no new contract makes `subjectId` the execution authority
- [ ] Verify naming contract supports both diagnostics and execution IDs without ambiguity

#### Generated Artifact Review

- [ ] Review contract serialization shape for AOT IR and hotupdate artifact compatibility
- [ ] Review generated JSON/artifact fields to ensure no legacy `GenericContextArtifact` payload remains authoritative
- [ ] Review naming output to ensure body/stub/context IDs are stable and diffable

#### Validation

- [ ] Add/update unit tests for parser and identity normalization
- [ ] Add/update contract serialization tests
- [ ] Add/update negative tests for ambiguous/unsupported generic identity shapes
- [ ] Exit only with contracts tests green

### 7.2 `20260421-04-loader-instantiation-graph-hard-cut`

#### Functional Requirements

- [ ] Replace clone/materialization as primary loader output with instantiation demand graph
- [ ] Collect cross-assembly generic instantiation demand explicitly
- [ ] Emit open definition, canonical instantiation, specialization policy, and family taxonomy inputs
- [ ] Stop using `SubstituteText`/`SubstituteMethodBody` as primary generic execution path
- [ ] Stop treating `MemberReference`/`MethodSpec` materialization as final runtime payload

#### File Audit

- [ ] Audit and update `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericMaterialization.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.Loader/LoaderStage.CrossAssemblyInstantiation.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.Loader/LoaderStage.MetadataResolution.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.Loader/LoaderStage.AssemblyLoading.cs`
- [ ] Audit loader outputs that still assume closed clone/materialization semantics

#### Architecture Review

- [ ] Verify loader emits graph inputs instead of clone-first payloads
- [ ] Verify no metadata-only compatibility layer is reintroduced
- [ ] Verify cross-assembly demand is explicit and reproducible
- [ ] Verify family taxonomy input can be consumed by later IR/runtime stages

#### Generated Artifact Review

- [ ] Review loader-emitted semantic artifacts for graph completeness
- [ ] Review loader outputs to ensure canonical instantiation and specialization data are present
- [ ] Review diffs to ensure clone count no longer grows as the primary success signal

#### Validation

- [ ] Add/update loader unit tests for instantiation graph
- [ ] Add/update cross-assembly demand tests
- [ ] Add/update negative tests for unsupported generic shapes
- [ ] Exit only with loader gates green

### 7.3 `20260421-05-shared-generic-aot-ir-hard-cut`

#### Functional Requirements

- [ ] Extend AOT IR to model open definition explicitly
- [ ] Extend AOT IR to model `SharedGenericBodyId`
- [ ] Extend AOT IR to model `InstantiationStubId`
- [ ] Extend AOT IR to model `RuntimeGenericContextArtifact`
- [ ] Make call sites consume body/stub/context semantics instead of closed body equivalence
- [ ] Carry support-matrix and specialization hints in IR

#### File Audit

- [ ] Audit and update IR contract sources in `src/managed/Chaos.IL2CPP.Contracts/**`
- [ ] Audit planners/serializers that emit `aot-core-ir.json`
- [ ] Audit any generated manifest/summary shapes that still assume closed-body execution

#### Architecture Review

- [ ] Verify IR has one authoritative way to represent shared generic execution
- [ ] Verify stub/context authority is explicit at call sites
- [ ] Verify diagnostics do not regain execution authority through backdoor fields

#### Generated Artifact Review

- [ ] Review `aot-core-ir.json` shape to ensure body/stub/context are independently visible
- [ ] Review generated summaries/sidecars to ensure canonical family and specialization data survive emission
- [ ] Review that no closed generic method is silently treated as the full runtime body when it should be a stub

#### Validation

- [ ] Add/update IR shape tests
- [ ] Add/update artifact serialization tests
- [ ] Add/update negative tests for malformed body/stub/context combinations
- [ ] Exit only with IR gates green

### 7.4 `20260421-06-generic-matrix-and-ir-gates`

#### Functional Requirements

- [ ] Build formal matrix for type/method/type+method generic cases
- [ ] Cover `ref/value/nested/byref/pointer/ref struct`
- [ ] Cover async family: `Task<T>`, `ValueTask<T>`, builders, awaiters
- [ ] Cover dispatch/reflection/cross-assembly/HotUpdate boundaries
- [ ] Encode per-family growth budgets
- [ ] Encode HotUpdate gate naming using `HotUpdate*` names only

#### File Audit

- [ ] Audit matrix/gate definitions in tests and documentation
- [ ] Audit owner proof assets and gate entry points
- [ ] Audit benchmark baselines tied to generic family growth

#### Architecture Review

- [ ] Verify matrix states align with specialization policy
- [ ] Verify every supported family has explicit budget and proof owner
- [ ] Verify no capability remains “implicitly supported” outside the matrix

#### Generated Artifact Review

- [ ] Review generated matrix/summary artifacts for family coverage
- [ ] Review generated audit outputs for unsupported or leaking families
- [ ] Review that HotUpdate modes/gates use canonical naming only

#### Validation

- [ ] Add/update matrix unit tests
- [ ] Add/update gate tests
- [ ] Capture benchmark baseline for size/symbol/peak-memory
- [ ] Exit only with matrix and gate evidence checked in

### 7.5 `20260421-01-aot-generic-task-lowering-normalization`

#### Functional Requirements

- [ ] Align async generic family with shared-generic IR
- [ ] Replace family-specific prefix/pattern logic with registry/policy-driven planning
- [ ] Generate shared body and thin stub outputs for async family
- [ ] Carry runtime context through planner and emitter
- [ ] Ensure specialization fallback is explicit, not accidental

#### File Audit

- [ ] Audit and update `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.*`
- [ ] Audit runtime support files that still key behavior off closed generic strings

#### Architecture Review

- [ ] Verify planner consumes body/stub/context authority from IR
- [ ] Verify async family no longer depends on per-instantiation body explosion as the main execution model
- [ ] Verify family registry semantics match matrix policy

#### Generated Artifact Review

- [ ] Review `generated/native-aot.generated.cpp` for shared body/stub structure
- [ ] Review runtime skeleton outputs and sidecars for family grouping correctness
- [ ] Review generated symbol count and duplicate body count to detect fallback abuse

#### Validation

- [ ] Add/update async family unit tests
- [ ] Run owner native proof for async families
- [ ] Compare generated artifact diffs before/after cutover
- [ ] Exit only with native proof and generated-code review complete

### 7.6 `20260421-07-dispatch-reflection-hotupdate-cutover`

#### Functional Requirements

- [ ] Move dispatch to stub/context authority instead of `subjectId` and legacy slot string logic
- [ ] Move reflection construction/invoke to canonical generic model
- [ ] Support generation-scoped handles and stale-handle reason codes
- [ ] Introduce HotUpdate dual registry + bridge semantics
- [ ] Introduce `PackageFormatVersion` + `KernelArtifactVersion`
- [ ] Introduce HotUpdate carrier schema and generated thunks for advanced carriers
- [ ] Support `byref`, pointer, and `ref struct` across HotUpdate bridge under explicit lifetime rules

#### File Audit

- [ ] Audit and update `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ReflectionObjectEmission.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/RuntimeManager.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateMethodRegistry.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/BridgeDispatcher.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/BridgeGenerator.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateMethodBindings.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/SupplementalMetadataWriter.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/SupplementalMetadataRegistry.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/PackageReader.cs`
- [ ] Audit and update `src/managed/Chaos.IL2CPP.HotUpdate/PackageValidator.cs`

#### Architecture Review

- [ ] Verify final execution authority is `InstantiationStubId + generation`
- [ ] Verify reflection handle lifecycle matches generation model
- [ ] Verify host and patch registries are independent and bridge-only coupled
- [ ] Verify `KernelArtifactVersion` is execution ABI authority
- [ ] Verify `TargetAotVersion` remains build/runtime alignment only
- [ ] Verify HotUpdate mode names and gates use canonical naming only
- [ ] Verify advanced carrier rules are explicit for write-back, pinning, and `ref struct` lifetime

#### Generated Artifact Review

- [ ] Review generated native dispatch paths to ensure generic execution no longer depends on string-matching authority
- [ ] Review reflection-related generated code to ensure stale handle/version behavior is explicit
- [ ] Review HotUpdate manifest/metadata artifacts for canonical instantiation plus runtime hints
- [ ] Review generated bridge thunks and carrier schemas for advanced carrier handling

#### Validation

- [ ] Add/update dispatch proof cases
- [ ] Add/update reflection construction/invoke proof cases
- [ ] Add/update HotUpdate proof cases for:
  - host->patch
  - patch->host
  - stale handle
  - version mismatch
  - advanced carriers
- [ ] Exit only with HotUpdate proof green

### 7.7 `20260421-08-legacy-generic-path-purge`

#### Functional Requirements

- [ ] Remove legacy clone/materialization primary path
- [ ] Remove scattered prefix/pattern generic execution logic
- [ ] Remove obsolete tests/helpers/assertions tied only to legacy path
- [ ] Remove obsolete compatibility surfaces from docs and manifests

#### File Audit

- [ ] Audit loader legacy paths in `src/managed/Chaos.IL2CPP.Loader/**`
- [ ] Audit codegen legacy paths in `src/managed/Chaos.IL2CPP.CodeGen/**`
- [ ] Audit HotUpdate legacy string/registry shortcuts that violate final authority model
- [ ] Audit tests and generated artifact checks that still encode old assumptions
- [ ] Audit docs for stale “clone/materialization as main path” language

#### Architecture Review

- [ ] Verify no final execution path depends on legacy clone/materialization
- [ ] Verify no hidden fallback silently revives old semantics
- [ ] Verify all retained compatibility surfaces are explicitly justified and temporary

#### Generated Artifact Review

- [ ] Review generated code for absence of legacy family-specific fallback scaffolding
- [ ] Review manifests/sidecars for absence of old authority fields
- [ ] Review artifact diffs to confirm legacy outputs are gone rather than merely unused

#### Validation

- [ ] Re-run contracts, IR gates, native proof, HotUpdate proof, benchmark
- [ ] Re-run generated code review checklist
- [ ] Exit only with final closeout evidence captured

## 8. Shared Review Checklist

Each child task is not complete until all of the following are true:

- [ ] functional requirements implemented
- [ ] file audit completed and recorded
- [ ] architecture review completed and no authority conflict remains
- [ ] generated code review completed and key issues resolved or explicitly tracked
- [ ] validation executed with evidence
- [ ] roadmap/design/authority docs updated if long-lived rules changed

## 9. Dependencies

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- existing `Loader / Contracts / CodeGen / NativeAotEmitter / HotUpdate` mainlines
- owner subject proofs and benchmark assets

## 10. Risks

- Contracts/IR mistakes will multiply rework cost in later runtime/HotUpdate cutovers
- Advanced carrier support across HotUpdate bridge is materially more expensive than same-domain reflection invoke
- If matrix/gates are delayed, false positives will accumulate from proof-only success
- Version topology drift between package/container and execution ABI will create late-stage integration failures

## 11. Recommended Execution Order

1. Complete `20260421-03-canonical-generic-contracts-and-parser-unification`
2. Complete `20260421-04-loader-instantiation-graph-hard-cut`
3. Complete `20260421-05-shared-generic-aot-ir-hard-cut`
4. Complete `20260421-06-generic-matrix-and-ir-gates`
5. Realign `20260421-01-aot-generic-task-lowering-normalization`
6. Complete `20260421-07-dispatch-reflection-hotupdate-cutover`
7. Complete `20260421-08-legacy-generic-path-purge`
