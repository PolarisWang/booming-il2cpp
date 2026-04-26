---
task_id: 20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
title: CoreLib Supplemental Runtime Skeleton Coverage Widening
task_type: plan
lifecycle_status: in-progress
phase: implementation
created_at: 2026-04-24 21:20:00 +08:00
updated_at: 2026-04-26 18:55:00 +08:00
current_dir: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: unblocker-for-corelib-full-verification
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: warn
preflight_summary: 当前 widening child 仍在父 roadmap 已批准边界内，可继续沿新的 4C `Convert family` 扩大 `System.Private.CoreLib` supplemental runtime-skeleton translated coverage；风险是 `ToString/object/provider/value-type` 族继续堆进单一 emitter 会放大结构债务，因此本轮继续坚持 family router 边界，不回流 legacy 入口。
auto_execution_decision: continue
design_doc: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/design-v2-02-marshal-platform-capability.md
plan_doc: docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/plan-v2-01.md
active: false
---

# 20260419-37 CoreLib Supplemental Runtime Skeleton Coverage Widening

## Current Decision

- `System.Convert` supplemental uncovered is now `0`.
- `System.String` supplemental uncovered is now `0`.
- `LowLevelMemoryFamily` is complete.
- `System.Half` supplemental uncovered is now `0`.
- `System.Number` supplemental uncovered is now `0`.
- `System.Decimal` supplemental uncovered is now `0`.
- `System.Char` supplemental uncovered is now `0`.
- `System.Single` supplemental uncovered is now `0`.
- `System.Double` supplemental uncovered is now `0`.
- `System.Int128` supplemental uncovered is now `0`.
- `System.UInt128` supplemental uncovered is now `0`.
- `System.IntPtr` supplemental uncovered is now `0`.
- `System.UIntPtr` supplemental uncovered is now `0`.
- `System.Byte` supplemental uncovered is now `0`.
- `System.SByte` supplemental uncovered is now `0`.
- `System.Int16` supplemental uncovered is now `0`.
- `System.UInt16` supplemental uncovered is now `0`.
- `System.Int32` supplemental uncovered is now `0`.
- `System.UInt32` supplemental uncovered is now `0`.
- `System.Int64` supplemental uncovered is now `0`.
- `System.UInt64` supplemental uncovered is now `0`.
- `System.Boolean` supplemental uncovered is now `0`.
- `System.Runtime.InteropServices.NFloat` supplemental uncovered is now `0`.
- `System.DateTime` supplemental uncovered is now `0`.
- `System.DateTimeOffset` supplemental uncovered is now `0`.
- `System.TimeSpan` supplemental uncovered is now `0`.
- `System.DateOnly` supplemental uncovered is now `0`.
- `System.TimeOnly` supplemental uncovered is now `0`.
- `System.Guid` supplemental uncovered is now `0`.
- `System.Index` supplemental uncovered is now `0`.
- `System.Range` supplemental uncovered is now `0`.
- `System.Text.Rune` supplemental uncovered is now `0`.
- `System.Enum` supplemental uncovered is now `0`.
- owner-attached companion lane is now complete.
- `System.DateTimeFormat` / `System.DateTimeParse` / `System.DateTimeRawInfo` / `System.DateTimeResult` supplemental uncovered are now `0`.
- `System.Globalization.CultureData` supplemental uncovered is now `0`.
- `System.Globalization.DateTimeFormatInfo` supplemental uncovered is now `0`.
- first valuetype kernel generic/intrinsic cutover is now active.
- **All `System.Convert` methods are now EMITTED** — 343 Convert methods in plan, 0 uncovered. Includes IConvertible-interfaced methods like `ToDateTime(valueType)` via `ConvertRuntimeHelperAbi`.
- `DecimalClusterManagedInvokeFamily` now owns:
  - `System.Decimal`
  - `System.Decimal+DecCalc`
  - `System.Decimal+DecCalc+Buf12`
  - `System.Decimal+DecCalc+Buf16`
  - `System.Decimal+DecCalc+Buf24`
  - `System.Decimal+DecCalc+PowerOvfl`
- Shared `ValueTypeManagedInvoke` extraction has now cleared the current CoreLib valuetype target set.
- Remaining uncovered items under `System.DateTime* / System.DateOnly* / System.TimeOnly* / System.Guid*` are companion/helper types, not valuetype roots.
- Phase order is frozen as:
  - `LowLevelMemoryFamily`
  - `NumberAndFormattingFamily`
  - `TaskContinuationFamily`
  - `PlatformThenIntrinsics`
- Inside phase 4, `Platform Capability` must complete before `Intrinsics`.

## Latest Summary

- 2026-04-26 18:55:00 +08:00: `System.Runtime.InteropServices.Marshal` is now formally frozen as the first `Phase 4A: Platform Capability` target. Approved design supplement:
  - `docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/design-v2-02-marshal-platform-capability.md`
  Key authority decisions:
  - `Marshal` is a platform capability surface, not a single family
  - hotupdate binds only through `MarshalPlatformAbiRootV1`
  - sub-ABIs are split as:
    - `MarshalMemoryBlockAbiV1`
    - `MarshalStringMarshalingAbiV1`
    - `MarshalStructureLayoutAbiV1`
    - `MarshalDelegateBridgeAbiV1`
    - `ComInteropAbiV1`
    - `VariantInteropAbiV1`
  - structure marshalling uses `A/B/C`:
    - `FullyBlittableFastPath`
    - `LayoutFixedMarshallerPath`
    - `DescriptorDrivenMarshallerPath`
  - `Path B` authority is frozen to canonical-type-descriptor-based classification plus approved field-family whitelist
  - `ComInteropAbiV1` is sectioned into:
    - `core`
    - `activation`
  - `VariantInteropAbiV1` is sectioned into:
    - `scalar`
    - `bulk`
    - `extended`
  - non-Windows `activation` and `extended` sections are capability-gated and map to `PlatformNotSupportedException`
  - `Marshal` implementation entry order is frozen as:
    1. root ABI
    2. memory-block ABI
    3. allocation-kind headers
    4. canonical descriptor -> A/B/C classification
    5. structure path A/B scaffolding
- 2026-04-26 18:21:00 +08:00: valuetype kernel registry is now fully lifted out of the old per-method spec table and widened again under canonical formal `20260426-181841-windows-9c9b`. This batch completes the current kernel cleanup target:
  - `RuntimeSkeletonKernelMethodSpec[]` is gone; `RuntimeSkeletonKernelCore` now owns:
    - semantic descriptors
    - carrier schemas
    - emission plans
    - centralized matcher -> plan generation
  - `Half/NFloat` operator semantics are corrected for `NaN`:
    - `Equals` / `CompareTo` keep .NET managed semantics
    - `== != < <= > >=` no longer reuse `CompareTo`-style helpers
  - char-classification kernel widened to cover hot predicates:
    - `IsAsciiDigit / IsAsciiHexDigit / IsAsciiLetter / IsAsciiLetterOrDigit`
    - `IsHighSurrogate / IsLowSurrogate / IsSurrogate / IsSurrogatePair`
    - `IsSeparatorLatin1 / IsWhiteSpaceLatin1 / IsBetween`
    - typed `CompareTo / Equals`
  - floating-classification kernel widened to cover typed compare/equality plus predicate families for:
    - `Half`
    - `Single`
    - `Double`
    - `System.Runtime.InteropServices.NFloat`
    including:
    - `IsNegative / IsPositive`
    - `IsNegativeInfinity / IsPositiveInfinity`
    - `IsNormal / IsSubnormal`
    - `IsInteger / IsEvenInteger / IsOddInteger / IsPow2`
    - `IsRealNumber / IsZero / IsNaNOrZero` where the managed type exposes them
  - generated runtime pages now prove real cutover sites such as:
    - `CharIsAsciiLetterOrDigit`
    - `CharIsWhiteSpaceLatin1`
    - `SingleCompare`
    - `DoubleCompare`
    - `HalfOperatorLessThan`
    - `NFloatIsNormal`
  - local gates:
    - `test_native_runtime_core.py` passed
    - `test_il2cpp_codegen_structure_governance_reference.py` passed
    - `test_full_assembly_closure_codegen_contracts_runtime_templates.py` passed
    - `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -v:minimal` passed
    - canonical formal `20260426-181841-windows-9c9b` passed for `windows-corelib-reference-native-hotupdate-proof`
- 2026-04-26 17:49:00 +08:00: kernel compare/equality cutover widened again. Canonical formal `20260426-174759-windows-e142` proves no coverage regression after adding:
  - `Half::CompareTo / Equals / relational operators`
  - `NFloat::CompareTo / Equals / relational operators`
  - `DateTime::CompareTo / Equals / relational operators`
  - `TimeSpan::CompareTo / Equals / relational operators`
  - `DateOnly::relational operators`
  - `TimeOnly::relational operators`
  - `Int128 / UInt128 relational operators`
  - `IntPtr / UIntPtr typed compare/equality and core relational operators`
- 2026-04-26 17:41:00 +08:00: valuetype kernel cutover is now live for the widened floating/tick slice after canonical formal `20260426-172636-windows-d35d`. Kernelized target roots staying at `0 uncovered` now include:
  - `System.Half`
  - `System.Runtime.InteropServices.NFloat`
  - `System.DateOnly`
  - `System.TimeOnly`
  This extends the existing kernel lane beyond `Char / Single / Double / Int128 / UInt128 / DateTime / TimeSpan`.
- 2026-04-26 17:28:00 +08:00: valuetype kernel cutover has expanded. Canonical formal `20260426-172636-windows-d35d` proves kernelized targets still keep `0 uncovered` after widening to:
  - `System.Half`
  - `System.Runtime.InteropServices.NFloat`
  - `System.DateOnly`
  - `System.TimeOnly`
  Runtime-core additions:
  - `HalfIsFinite / HalfIsNaN / HalfIsInfinity`
  - `NFloatIsFinite / NFloatIsNaN / NFloatIsInfinity`
  - `DateOnlyCompareDayNumber / DateOnlyEqualsDayNumber`
  - `TimeOnlyCompareTicksValue / TimeOnlyEqualsTicksValue`
  Backend staging remains:
  - `char-classification = Intrinsic`
  - `floating-classification = Intrinsic`
  - `bit-arithmetic = Intrinsic`
  - `tick-arithmetic = Generic`
- 2026-04-26 17:05:00 +08:00: first valuetype kernel execution lane is active through `ValueTypeKernelFamily`. Canonical formal `20260426-170213-windows-5990` proves the runtime skeleton still holds `0 uncovered` for the first cutover targets:
  - `System.Char`
  - `System.Single`
  - `System.Double`
  - `System.Int128`
  - `System.UInt128`
  - `System.DateTime`
  - `System.TimeSpan`
  Runtime-core now exposes:
  - `CharIsAscii / CharIsLatin1`
  - `Single/Double IsFinite / IsNaN / IsInfinity`
  - `Int128/UInt128 Compare / Equals`
  - `DateTimeCompareTicks / TimeSpanCompareTicks`
  Backend staging:
  - `char-classification = Intrinsic`
  - `floating-classification = Intrinsic`
  - `bit-arithmetic = Intrinsic`
  - `tick-arithmetic = Generic`
  This is the first real kernel cutover; further ISA-specialization and broader kernel coverage remain open.
- 2026-04-26 16:41:00 +08:00: helper-layer widening progressed through three new lanes on top of enhanced `C+`:
  - `CompanionManagedInvokeFamily`
  - `DateTimeSemanticEngineFamily`
  - `GlobalizationDateTimeSupportFamily`
  Canonical formal `20260426-163926-windows-e8a4` proves:
  - `System.DateTime+LeapSecondCache = 0`
  - `System.DateOnly+<>c = 0`
  - `System.TimeOnly+<>c = 0`
  - `System.Guid+GuidResult = 0`
  - `System.DateTimeFormat = 0`
  - `System.DateTimeParse = 0`
  - `System.DateTimeRawInfo = 0`
  - `System.DateTimeResult = 0`
  - `System.Globalization.CultureData = 0`
  - `System.Globalization.DateTimeFormatInfo = 0`
  Current dominant residuals have now moved away from valuetype/date helper territory and are mainly:
  - `System.Runtime.Intrinsics.*`
  - `Interop+Kernel32`
  - `System.Threading.Tasks.Task`
  - `System.Runtime.InteropServices.Marshal`
  - `System.Type`
  - `System.RuntimeType`
  - `System.Text.StringBuilder`
- 2026-04-26 16:13:00 +08:00: CoreLib valuetype target set is complete. New shared semantic families:
  - `PrimitiveScalarManagedInvokeFamily`
  - `WideNumericManagedInvokeFamily`
  - `CalendarStructManagedInvokeFamily`
  - `IdentityStructManagedInvokeFamily`
  `FloatingScalarManagedInvokeFamily` was also widened to `System.Runtime.InteropServices.NFloat`.
  Canonical formal `20260426-160840-windows-1480` proves the remaining valuetype roots are all `0`:
  - `System.Int128`
  - `System.UInt128`
  - `System.IntPtr`
  - `System.UIntPtr`
  - `System.Byte`
  - `System.SByte`
  - `System.Int16`
  - `System.UInt16`
  - `System.Int32`
  - `System.UInt32`
  - `System.Int64`
  - `System.UInt64`
  - `System.Boolean`
  - `System.Runtime.InteropServices.NFloat`
  - `System.DateTime`
  - `System.DateTimeOffset`
  - `System.TimeSpan`
  - `System.DateOnly`
  - `System.TimeOnly`
  - `System.Guid`
  - `System.Index`
  - `System.Range`
  - `System.Text.Rune`
  Stop condition for the current user request is reached: valuetype roots are complete. Residual uncovered companions include `System.DateTimeFormat`, `System.DateTimeParse`, `System.DateOnly+<>c`, `System.TimeOnly+<>c`, `System.Guid+GuidResult`, and `System.DateTime+LeapSecondCache`.
- 2026-04-26 15:57:00 +08:00: `System.Single` and `System.Double` are complete. A new `FloatingScalarManagedInvokeFamily` now covers `/System.Single::` and `/System.Double::` on top of shared `ValueTypeManagedInvoke`. Canonical formal `20260426-155428-windows-da69` proves both supplemental uncovered counts are now `0`.
- 2026-04-26 15:51:00 +08:00: `System.Char` is complete. `CharManagedInvokeFamily` now covers both `/System.Char::` and `/System.CharEnumerator::` on top of the shared `ValueTypeManagedInvoke` core. Canonical formal `20260426-154858-windows-db5c` proves `System.Char` supplemental uncovered is now `0`.
- 2026-04-26 15:46:00 +08:00: shared `ValueTypeManagedInvoke` extraction has started. New shared cores:
  - `RuntimeSkeletonValueTypeManagedInvokeCore`
  - `RuntimeSkeletonKernelCore`
  First sample lane:
  - `CharManagedInvokeFamily`
  Current gate status:
  - structure governance green
  - runtime template governance green
  - compatibility/build green
  This is architecture-only progress; no new canonical formal run has been taken for `System.Char` yet.
- 2026-04-26 15:22:00 +08:00: `System.Decimal` is complete through `DecimalClusterManagedInvokeFamily` on top of the enhanced `C+` skeleton (`ManagedInvokeAbiCore + SemanticRegistry + BackendSelector`). Canonical formal `20260426-152012-windows-091a` proves `System.Decimal`, `System.Decimal+DecCalc`, `Buf12/16/24`, and `PowerOvfl` supplemental uncovered are all `0`.
- 2026-04-26 14:42:00 +08:00: `System.Enum` is complete via `D2`: `EnumManagedInvokeFamily + EnumImportedBridgeLane` with page/runtime metadata-helper consumption through type descriptors. Canonical formal `20260426-143918-windows-54fe` proves `System.Enum` supplemental uncovered is now `0`.
- 2026-04-26 14:36:00 +08:00: **Breakthrough — emittedMethodCount = 4658 (+2397)**. Formal `20260426-143601-windows-4240` proves all `System.Convert` supplemental uncovered = 0. Root cause of prior stagnation: managed codegen DLL was modified but never rebuilt — changes from the previous 4 handler additions (`BoxedValueTypeInstanceCall`, `StaticValueTypeCallForwarder`, `InstanceValueTypeCallForwarder`, `ValueTypeIdentityForwarder`) plus `ConvertRuntimeHelperAbi` improvements were not compiled into the deployed binary. After `deploy core` rebuild, all 343 Convert methods and ~2000 additional value-type returning methods are now emitted. Top remaining uncovered clusters: `System.Enum` (220), `System.Threading.Tasks.Task` (247), intrinsics.
- 2026-04-26 14:00:00 +08:00: `NumberManagedInvokeFamily` is complete. Canonical formal `20260426-135444-windows-e72d` proves `System.Number` supplemental uncovered is now `0`.
- 2026-04-26 13:25:00 +08:00: `NumberAndFormattingFamily` has started. `HalfManagedInvokeFamily` is complete, and canonical formal `20260426-132239-windows-74dd` proves `System.Half` supplemental uncovered is now `0`.
- 2026-04-26 13:05:00 +08:00: `Batch 3: SpanHelpersKernel` is complete.
- 2026-04-26 13:25:00 +08:00: `NumberAndFormattingFamily` has started. `HalfManagedInvokeFamily` is complete, and canonical formal `20260426-132239-windows-74dd` proves `System.Half` supplemental uncovered is now `0`.
- 2026-04-26 13:05:00 +08:00: `Batch 3: SpanHelpersKernel` is complete. A dedicated `SpanHelpersKernelFamily` now owns the remaining `System.SpanHelpers::*` surface through a stable kernel-family contract and clears the residual audit set.
- Canonical formal `20260426-130239-windows-858e` proves `System.SpanHelpers` supplemental uncovered is now `0`.
- 2026-04-26 12:31:00 +08:00: `Batch 2: MemoryExtensionsManagedInvoke` is complete. A dedicated `MemoryExtensionsManagedInvokeFamily` now routes the remaining `System.MemoryExtensions::*` wrapper-heavy surface through a hotupdate-stable managed-invoke contract with pointer-to-storage arguments and direct/indirect return selection.
- Canonical formal `20260426-122859-windows-8484` proves `System.MemoryExtensions` supplemental uncovered is now `0`.
- 2026-04-26 12:15:00 +08:00: `Batch 1: UnsafeManagedInvoke` is complete. A dedicated `UnsafeManagedInvokeFamily` now routes `System.Runtime.CompilerServices.Unsafe::*` through a hotupdate-stable managed-invoke contract with universal pointer-carried arguments and explicit `return_value_is_indirect` return selection.
- Canonical formal `20260426-121227-windows-a011` proves `System.Runtime.CompilerServices.Unsafe` supplemental uncovered is now `0`.
- 2026-04-26 10:35:00 +08:00: `System.Convert` and `System.String` have both reached `0 uncovered` in the current canonical formal evidence.
- New design authority:
  - `docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/design-v2-01.md`
- New execution plan:
  - `docs/dev/in-progress/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening/plan-v2-01.md`
- `LowLevelMemoryFamily` is now complete.
- The next canonical widening target is `System.Decimal`, as the next `NumberAndFormattingFamily` slice.

## Next Step

- next_action: start `System.Runtime.InteropServices.Marshal` implementation with `MarshalPlatformAbiRootV1 + MarshalMemoryBlockAbiV1 + allocation-kind headers + canonical descriptor A/B/C classification`
- owner: codex
- trigger: immediate

## Canonical Evidence

- string/convert closure formal:
  - `runId = 20260426-102617-windows-23c6`
- unsafe managed-invoke closure formal:
  - `runId = 20260426-121227-windows-a011`
- memory-extensions managed-invoke closure formal:
  - `runId = 20260426-122859-windows-8484`
- spanhelpers kernel closure formal:
  - `runId = 20260426-130239-windows-858e`
- half managed-invoke closure formal:
  - `runId = 20260426-132239-windows-74dd`
- number managed-invoke closure formal:
  - `runId = 20260426-135444-windows-e72d`
- decimal managed-invoke closure formal:
  - `runId = 20260426-152012-windows-091a`
- char managed-invoke closure formal:
  - `runId = 20260426-154858-windows-db5c`
- floating-scalar managed-invoke closure formal:
  - `runId = 20260426-155428-windows-da69`
- valuetype completion formal:
  - `runId = 20260426-160840-windows-1480`
- helper-layer completion formal:
  - `runId = 20260426-163926-windows-e8a4`
- first valuetype kernel cutover formal:
  - `runId = 20260426-170213-windows-5990`
- kernel registry + widened char/floating kernel formal:
  - `runId = 20260426-181841-windows-9c9b`
- enum D2 closure formal:
  - `runId = 20260426-143918-windows-54fe`
- number managed-invoke closure formal:
  - `runId = 20260426-135444-windows-e72d`
- convert-all-emitted closure formal:
  - `runId = 20260426-143601-windows-4240`
- latest planning truth snapshot:
  - `artifacts/subjects/SolutionCorePack/runs/20260426-143601-windows-4240/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`

## 当前状态

- 最新 supplemental runtime-skeleton coverage 基线：
  - `requestedMethodCount = 59864`
  - `emittedMethodCount = 4658`
  - `uncoveredMethodCount = 50148`
- **`System.Convert` 未覆盖数：0**（343 methods in plan, all EMITTED）
- 已完成并真实消化的 family：
  - `Convert` — 全部收口（含 IConvertible 接口模式 + 全部 value-type 返回）
  - `String` — 全部收口
  - `LowLevelMemoryFamily` — 全部收口
  - `Half` — 全部收口
  - `Number` — 全部收口
- 顶部未覆盖类型簇（排除 platform intrinsics）：
  - `System.Threading.Tasks.Task`: 247
  - `System.Numerics.Vector`: 246
  - `System.Enum`: 220
  - `System.Runtime.InteropServices.Marshal`: 217
  - `System.Decimal`: 183
- 结构前提：
  - `20260425-01-runtime-skeleton-4c-handler-framework-roadmap` 已完成
  - `20260425-02-runtime-skeleton-4c-framework-foundation-cutover` 已完成
  - 顶层 runtime-skeleton 已切到 4C family router，当前 widening 继续建立在新 family handler 之上
- 新增 4 个 family handler 和 4 个 Scriban 模板：
  - `TryBuildRuntimeSkeletonConvertBoxedValueTypeInstanceCallHandler` — `BoxedValueTypeInstanceCallStub.cpp.scriban`
  - `TryBuildRuntimeSkeletonConvertStaticValueTypeCallForwarderHandler` — `StaticValueTypeCallForwarderStub.cpp.scriban`
  - `TryBuildRuntimeSkeletonConvertInstanceValueTypeCallForwarderHandler` — `InstanceValueTypeCallForwarderStub.cpp.scriban`
  - `TryBuildRuntimeSkeletonConvertValueTypeIdentityForwarderHandler` — `StaticValueTypeIdentityForwarderStub.cpp.scriban`

## 当前判断

- 重新编译 managed DLL 后，所有 4 个 value-type handler 和 ConvertRuntimeHelperAbi 真实生效。
- `System.Convert` 全部 343 个方法已发射（+2000+ 其他 value-type 返回方法）。
- 顶部未覆盖簇为 platform intrinsics（不能也不应覆盖）+ `System.Enum`（220）、`System.Threading.Tasks.Task`（247）等。
- 下一步按计划切到 `System.Enum` widening。

## 最新摘要

- 2026-04-25 20:55:39 +08:00：新增 `System.Convert::ToString` 9 条 residual 的 runtime-skeleton emitter 支持。新增 `TryBuildRuntimeSkeletonConvertStaticStringCallForwarderHandler` 和 `StaticStringCallForwarderStub.cpp.scriban` 模板，通过 `bridge->resolve_method_by_token` + `abi->method_invoke(nullptr)` 生成自包含的静态方法调用桥接桩，覆盖 `ldarg → [ldarg] → call → ret` 模式的 string 返回方法。同时限制 handler 仅匹配 `System.Convert` 自身或转发到 `System.Convert::ToString` 的方法，避免误匹配其他 family。RED→GREEN 切换完成，formal `20260425-205823-windows-e19c` 通过，`System.Convert::ToString` canonical residual 归零。
- 2026-04-25 23:56:44 +08:00：新增 4 个 `System.Convert` value-type 返回的 runtime-skeleton emitter handler。初始版本限制为 `DateTime/Decimal` 返回类型。
  - （注：前次 STATUS 中 "ToSByte/ToInt16/ToInt32/ToUInt32 全部收口" 断言有误——handler 被 DateTime/Decimal 过滤限制，未真正匹配 primitive 返回的方法。）
- 2026-04-26 00:12:29 +08:00：移除 4 个 value-type handler 的 `DateTime/Decimal` 过滤，覆盖所有 value-type 返回类型。同时将 3 个 value-type handler（`BoxedValueTypeInstanceCall`、`StaticValueTypeCallForwarder`、`InstanceValueTypeCallForwarder`）+ `ValueTypeIdentityForwarder` 移到 `ByteForwarderHandler` 之后，避免截获 CheckedPrimitiveHandler 已处理的方法。formal `20260426-001144-windows-c733` 通过，`emittedMethodCount = 2261`（+361），`uncoveredMethodCount = 52542`。非 Convert 家族大量 value-type 返回方法被覆盖。`System.Convert` 残余量仍为 201（含内部辅助），IConvertible 出口中 shape 不匹配的 overload 未被消化。
  - **`BoxedValueTypeInstanceCallHandler`**：处理 `ldarga 0 → ldnull → callvirt → ret` 的 boxed value-type 实例调用（IConvertible 显式接口实现），新建 `StaticBoxedValueTypeInstanceCallStub.cpp.scriban` 模板，使用 `abi->method_invoke` + 类型化 return buffer（DateTime=8bytes/Decimal=16bytes）。
  - **`StaticValueTypeCallForwarderHandler`**：处理 `ldarg [ldarg] → call → ret` 静态方法调用返回 value-type，新建 `StaticValueTypeCallForwarderStub.cpp.scriban`。
  - **`InstanceValueTypeCallForwarderHandler`**：处理 `ldarg [ldnull/ldarg] → callvirt → ret` 实例方法调用返回 value-type，新建 `InstanceValueTypeCallForwarderStub.cpp.scriban`。
  - **`ValueTypeIdentityForwarderHandler`**：处理 `ldarg.0 → ret` 纯透传返回 value-type，新建 `StaticValueTypeIdentityForwarderStub.cpp.scriban`（无 method resolve/invoke，最小开销）。
  - 所有 handler 共享 cross-assembly token resolution 通过 `externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken()`。
  - `RuntimeSkeletonConvertFamilyHandlers` 链更新为 17 个 handler。
  - 修复 handler 链编辑时误覆盖 `TryBuildRuntimeSkeletonConvertBoxedIConvertibleCharInvalidCastHandler` 方法体的问题。
  - formal `20260425-235502-windows-0bbf` 通过，`emittedMethodCount = 1900`（+40），`uncoveredMethodCount = 52877`。`System.Convert` 残余从 242 降至 157。
- 2026-04-25 20:03:42 +08:00：已完成 `System.Convert::ToString` 纯 A 路径的 manifest authority 收口。`closure.manifest.json` 新增 `resolvedAssemblies`，full-closure direct DLL 现在会把解析后的 closure 程序集路径（含 `System.Private.CoreLib.dll`）落盘；`NativeReferenceProofEmitter` / `NativeAotLoweringPlanner` 已改为优先消费该 authority。期间修复 external metadata resolver 在并行 emit 下持有已释放 `MetadataReader` 导致的 `AccessViolationException`。定向 `convert_to_string_runtime_skeleton_methods`、相关 contract pytest、全量 audit pytest 与 formal `20260425-200102-windows-9f4c` 均已通过；最新 supplemental coverage 为 `requestedMethodCount = 59837`、`emittedMethodCount = 1858`、`uncoveredMethodCount = 52928`，`System.Convert::ToString` canonical residual 已收敛到 9 条，当前剩余集中在 `object(+provider)`、`radix` 系列（`Byte/Int16/Int32/Int64`）与少量 `bool/char` wrapper。
- 2026-04-25 16:20:56 +08:00：已补回本轮 `System.Convert::ToChar` 收口状态。最新 formal `20260425-153952-windows-ff29` 证明 `System.Convert::ToChar(...)` supplemental uncovered 已归零；`System.Convert` 总剩余降至 `251`，下一步切到 `System.Convert::ToString` widening。
- 2026-04-25 15:39:52 +08:00：补齐 `System.Convert::ToChar(string/object)` 与 boxed value-type / boxed `IConvertible` char invalid-cast 的最后几条 lane，并在 `TryResolveConcreteTypeCallTarget(...)` 中支持显式接口实现名后缀匹配，避免 `System.IConvertible.ToChar` 这类 canonical target 在 full-closure 下误判为缺失。
- 2026-04-25 13:13:19 +08:00：把 `System.Convert::ToChar(Boolean/Single/Double)` 的 boxed `IConvertible` invalid-cast lane 接进新的 `Convert family` 4C framework，并保持“有 canonical body 时严格做 target shape 校验；fixture/full-closure 缺 body 时仅对白名单 source type 走最小 fallback”的边界。
- 2026-04-25 12:24:10 +08:00：把 `System.Convert::ToChar(Int32/Int64)` 吃进 primitive return forwarder，并把原本偏 byte 命名的 forwarder 提升成 byte/char 共用 lane。
- 2026-04-25 11:57:41 +08:00：把 `checked byte` builder 收敛成共享 checked primitive convert 核心，并新增 `checked char` lane，真实消化 `ToChar(SByte/Int16/UInt16/UInt32/UInt64)`。

## 下一步

- `System.Convert` 全部收口，不需再排查。
- 按计划切到 `System.Enum` widening（220 uncovered）作为 `NumberAndFormattingFamily` 的下一个切片。
- next_action: start `System.Enum` widening inside `NumberAndFormattingFamily`
- owner: codex
- trigger: manual

## 验证

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -k object_to_char_runtime_skeleton_methods -q`
  - `1 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -k decimal_and_datetime_to_char_runtime_skeleton_methods -q`
  - `1 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -k "boxed_value_type_char_invalid_cast or object_char_provider" -q`
  - `2 passed`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
  - `9 passed in 0.10s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
  - `21 passed in 0.13s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - `17 passed in 53.16s`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260425-205823-windows-e19c` (9 ToString residual 收口)
  - `finalStatus = ok`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260425-205823-windows-e19c/run-report/summary.json`
  - `finalStatus = ok`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/summary.json`
  - `eventsPath = artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/events.jsonl`
  - `consolePath = artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/console.log`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -k convert_to_string_runtime_skeleton_methods -q`
  - `1 passed in 9.32s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -k convert_to_string_helpers_have_templates -q`
  - `1 passed in 0.13s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
  - `22 passed in 0.17s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
  - `9 passed in 0.15s`
- `python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py -q`
  - `6 passed in 10.17s`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
  - `18 passed in 83.77s`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260426-001144-windows-c733` (generalize value-type handlers to all types)
  - `finalStatus = ok`
  - `requestedMethodCount = 59854`, `emittedMethodCount = 2261`, `uncoveredMethodCount = 52542`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/run-report/summary.json`
  - `eventsPath = artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/run-report/events.jsonl`
  - `consolePath = artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/run-report/console.log`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260425-235502-windows-0bbf` (IConvertible value-type widening: ToDateTime/ToDecimal/ToSByte/ToInt16/ToInt32/ToUInt32)
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - `runId = 20260425-200102-windows-9f4c`
  - `finalStatus = ok`
  - `summaryPath = artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/summary.json`
  - `eventsPath = artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/events.jsonl`
  - `consolePath = artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/console.log`

## 风险 / 阻塞

### risks

- `System.Convert::ToString` 如果直接把 string/object/provider/value-type lane 混堆进 emitter 单点，会继续放大结构债务；本轮应优先抽出最小共享 helper，而不是为每个 primitive 单独堆 handler。
- 当前存在 `StaticByteForwarderStub`、`StaticCheckedByteConvertStub` 这类历史命名已经被 byte/char 共用的模板；继续向 string 家族扩面时，要避免再复制一套语义等价但命名分裂的模板层。
- `System.Convert::ToString(System.Object,System.IFormatProvider)` 仍涉及 `IConvertible`、`IFormattable`、`Object::ToString()` 与 `String::Empty` 分支；本轮 pure A blocker 已清掉，但 residual 收口仍应保持 family router 边界，避免把 object/radix 逻辑重新堆回单点 emitter。

### blockers

- none

## DLL-First Audit Authority Supplement

- recorded_at: `2026-04-26 00:12:29 +08:00`
- current_dll: `System.Private.CoreLib`
- canonical_run:
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/run-report/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/run-report/events.jsonl`
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/run-report/console.log`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-235502-windows-0bbf/run-report/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-235502-windows-0bbf/run-report/events.jsonl`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/events.jsonl`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/run-report/console.log`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/events.jsonl`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/run-report/console.log`
- proof_artifacts:
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/codegen-summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/declared/unit/corelib-reference-hotupdate-proof/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-235502-windows-0bbf/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-235502-windows-0bbf/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/codegen-summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-235502-windows-0bbf/declared/unit/corelib-reference-hotupdate-proof/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-235502-windows-0bbf/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/codegen-summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/declared/unit/corelib-reference-hotupdate-proof/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/codegen-summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/declared/unit/corelib-reference-hotupdate-proof/summary.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`
- supplemental_coverage_artifacts:
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/analysis/generated/supplemental-full-closures/system-private-corelib/native-aot/native-aot.plan.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260426-001144-windows-c733/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-235502-windows-0bbf/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/analysis/generated/supplemental-full-closures/system-private-corelib/native-aot/native-aot.plan.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-200102-windows-9f4c/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-aot/native-aot.plan.json`
  - `artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/`
- audit_conclusion:
  - `native-proof`: passed-with-current-canonical-evidence
  - `hotupdate-proof`: passed-with-current-canonical-evidence
  - `completion-certification`: blocked-by-coverage-widening
  - latest_evidence: `20260426-001144-windows-c733` — generalized value-type handlers, emittedMethodCount=2261
