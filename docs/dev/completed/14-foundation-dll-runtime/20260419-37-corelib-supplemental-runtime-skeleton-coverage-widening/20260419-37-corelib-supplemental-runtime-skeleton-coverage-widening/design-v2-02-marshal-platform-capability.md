# CoreLib Marshal Platform Capability Design v2.02

Date: 2026-04-26 18:55:00 +08:00
Status: draft-approved-for-execution

## 1. Goal

Freeze the `System.Runtime.InteropServices.Marshal` architecture under `Phase 4A: Platform Capability` with:

- native-first performance on memory / string / layout / delegate hot paths
- stable hotupdate-facing ABI boundaries
- no future rewrite when COM / Variant capability expands

This design is a focused supplement to [design-v2-01.md](D:/agent/booming-il2cpp/docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/design-v2-01.md).

## 2. Current Truth

Canonical source:

- `artifacts/subjects/SolutionCorePack/runs/20260426-181841-windows-9c9b/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`

Current `System.Runtime.InteropServices.Marshal` residuals: `204`

Observed semantic buckets:

- `memory-block`: `77`
- `metadata-interop`: `31`
- `com-activation`: `30`
- `string-marshaling`: `20`
- `variant`: `6`
- `handle-runtime`: `6`
- `other`: `34`

Conclusion:

- `Marshal` is not a single family
- `Marshal` must be treated as a platform capability surface with multiple semantic lanes

## 3. Top-Level Architecture

`Marshal` is split into semantic lanes, but all hotupdate consumers bind through a single root ABI:

- `MarshalPlatformAbiRootV1`
- `MarshalMemoryBlockAbiV1`
- `MarshalStringMarshalingAbiV1`
- `MarshalStructureLayoutAbiV1`
- `MarshalDelegateBridgeAbiV1`
- `ComInteropAbiV1`
- `VariantInteropAbiV1`

Root responsibilities:

- capability discovery
- ABI version negotiation
- sub-ABI lookup
- provider lifecycle authority

Sub-ABI responsibilities:

- own one semantic lane only
- keep call carriers lane-local
- expose provider-specialized native fast paths

## 4. Root ABI

`MarshalPlatformAbiRootV1` uses:

- capability bitset for fast checks
- function-table lookup for actual binding

Required capabilities:

- `memory_block`
- `string_marshaling`
- `structure_layout`
- `delegate_bridge`
- `com_core`
- `com_activation`
- `variant_scalar`
- `variant_bulk`
- `variant_extended`

Rules:

- patch binds only to `MarshalPlatformAbiRootV1`
- sub-ABIs are discovered through root lookup
- absence of a capability must never break patch load
- unsupported capabilities fail at call time with precise managed exceptions

## 5. Structure Marshalling

`MarshalStructureLayoutAbiV1` uses a three-path model:

### Path A: `FullyBlittableFastPath`

Use when the type is fully blittable and can be transferred by:

- `memcpy`
- `memmove`
- fixed-size load/store

Performance target:

- identical class of native cost as a raw memory block move

### Path B: `LayoutFixedMarshallerPath`

Use when the type is not classified as path A but still has a precomputable fixed marshalling plan.

Path B must satisfy all of the following:

- no GC reference fields
- no `string`, `object`, `array`, or reference-like fields
- no custom marshaller
- no object-graph traversal
- all fields recursively belong to approved `layout-fixed field families`
- the full marshalling op sequence is precompiled from the canonical type descriptor

Approved field families:

- integer family
- floating family
- enum
- `bool`
- `char`
- `IntPtr / UIntPtr`
- `Guid`
- `DateTime`
- `TimeSpan`
- `Decimal`
- primitive fixed buffers

Path B is explicitly **not** a general fallback.

Performance target:

- near-blittable execution with deterministic per-field ops only

### Path C: `DescriptorDrivenMarshallerPath`

Use for everything that is not admitted by path A or B, including:

- non-blittable fields
- object/reference/string/array fields
- custom marshalers
- recursive object graphs

Performance target:

- correctness first
- still descriptor-cached, but not a native fast path

## 6. Classification Authority

Classification authority is:

- `CanonicalTypeDescriptor`

Runtime fast lookup is:

- `RuntimeTypeHandle -> CanonicalTypeDescriptor -> MarshallingClass(A/B/C) -> PlanCache`

Rules:

- codegen must not hardcode A/B/C by type name alone
- runtime handle is a cache index, not semantic authority
- hotupdate-added types must be classified from runtime descriptor truth

## 7. Memory and Allocation

`MarshalMemoryBlockAbiV1` owns:

- `AllocHGlobal / FreeHGlobal`
- `AllocCoTaskMem / FreeCoTaskMem`
- `AllocBSTR / FreeBSTR`
- `Read* / Write*`
- `Copy`

Allocation policy:

- `HGlobal`, `CoTaskMem`, and `BSTR` remain distinct semantic kinds
- underlying provider implementation may share allocator internals
- every allocated block must carry allocation-kind metadata

Required representation:

- lightweight allocation header tag

Rejected designs:

- no-tag symmetry-only ownership
- side-table ownership tracking for hot paths

Reason:

- header tags preserve semantic correctness without losing native locality

## 8. Delegate and Function Pointer Bridge

`MarshalDelegateBridgeAbiV1` owns:

- `GetFunctionPointerForDelegate`
- `GetDelegateForFunctionPointer`

Required cache structure:

- thunk template cache
- bound callable cache

Bound callable identity must include:

- delegate runtime type
- target method identity
- target instance identity
- open/closed/static shape
- hotupdate generation or equivalent callable version identity

Rules:

- no per-call thunk regeneration on hot paths
- no address-only reverse lookup
- delegate keepalive / thunk lifetime must be runtime-owned

## 9. COM Interop ABI

`ComInteropAbiV1` is required in V1, but it must be sectioned:

- `core`
- `activation`

### `core`

Includes:

- `QueryInterface`
- `AddRef`
- `Release`
- `GetIUnknownForObject`
- `GetObjectForIUnknown`
- `GetIDispatchForObject`
- `GetComInterfaceForObject`

### `activation`

Includes:

- `GetTypeFromCLSID`
- `GetTypeFromProgID`
- `BindMoniker`
- `CreateBindCtx`
- `MkParseDisplayName`

Rules:

- `activation` is capability-gated independently from `core`
- non-Windows platforms may expose `core` absence or `activation` absence explicitly
- unsupported `activation` must map to `PlatformNotSupportedException`

## 10. Variant Interop ABI

`VariantInteropAbiV1` is also required in V1, and must be sectioned:

- `scalar`
- `bulk`
- `extended`

### `scalar`

Includes:

- object <-> native variant single-value conversion

### `bulk`

Includes:

- `GetObjectsForNativeVariants`
- contiguous native variant array conversion

### `extended`

Includes:

- higher-complexity and platform-specialized variant mappings

Rules:

- `bulk` is the main native performance surface
- `extended` must not pollute `scalar` or `bulk` fast paths
- unsupported `extended` must map to `PlatformNotSupportedException`

## 11. Platform Gating

For:

- `ComInteropAbiV1.activation`
- `VariantInteropAbiV1.extended`

required behavior is:

- root capability bit fast-check
- provider-level defensive check
- precise managed exception mapping

Rejected:

- compile-time removal that changes patch-visible ABI
- silent default return values
- lazy empty-provider fallback without explicit capability semantics

## 12. Performance Priorities

Native-first priority order:

1. `MarshalMemoryBlockAbiV1`
2. `MarshalStringMarshalingAbiV1`
3. `MarshalStructureLayoutAbiV1` path A/B
4. `MarshalDelegateBridgeAbiV1`
5. `VariantInteropAbiV1.bulk`
6. `ComInteropAbiV1.core`
7. `ComInteropAbiV1.activation`
8. `VariantInteropAbiV1.extended`

This ordering is also the recommended implementation order inside the `Marshal` family.

## 13. Verification Obligations

Freeze:

- `capabilityFamily = platform-capability`
- `capabilityItem = system-runtime-interopservices-marshal`
- `ownerSubjectId = subject/SolutionCorePack`
- `proofRequired = true`
- `benchmarkRequired = true`
- `hotupdateImpact = Proof`

Required gates:

- `collector`
- `registry`
- `workspace`
- `managed-proof`
- `native-proof`
- `hotupdate-proof`

Benchmark-required first-wave items:

- `Marshal.Copy`
- `ReadInt32 / ReadInt64 / WriteInt32 / WriteInt64`
- `PtrToStringUTF8`
- `StringToCoTaskMemUTF8`
- `PtrToStructure<T> / StructureToPtr<T>` path A/B cases
- delegate function-pointer bridge hot path

## 14. Immediate Implementation Entry

The first implementation batch should:

1. introduce `MarshalPlatformAbiRootV1`
2. carve out `MarshalMemoryBlockAbiV1`
3. freeze allocation-kind headers
4. establish `CanonicalTypeDescriptor -> MarshallingClass(A/B/C)` classification
5. add structure path A/B scaffolding

Only after that should COM / Variant sections be attached.
