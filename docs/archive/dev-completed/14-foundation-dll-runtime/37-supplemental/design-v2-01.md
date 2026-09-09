# CoreLib Runtime Skeleton Capability Closure Design v2.01

Date: 2026-04-26 10:35:00 +08:00
Status: draft-approved-for-execution

> **勘误与追溯 (2026-09-09, review 补注)**：本文件为归档复盘快照。下列补注仅订正文档内部自洽性/口径，**不改写原设计判断**；凡原文未记载处，不虚构来源，仅如实标注。
>
> - **#11 审批依据**：`Status: draft-approved-for-execution` 的审批人 / 时点 / 关联 issue / PR 未在归档内记载。此处仅标注字面状态，读者须自行回溯实际批准来源。
> - **#5 "Family" 双义**：本文两处使用 `Family` 后缀但层级不同——§1 四阶段名（`LowLevelMemoryFamily` 等，对应 §5 Phase）是**执行阶段分组名**；§3 三语义层（`ManagedInvokeFamily` / `KernelFamily` / `Platform+IntrinsicFamily`）是**架构层分类**。二者命名碰撞，非同一概念。
> - **#6 大小写**：原文中 `hotupdate` / `HotUpdate` / `Hotupdate` 大小写不一致。统一口径：正式名词用 `HotUpdate`（标题已为 `§4 HotUpdate Rule`），技术标签 / 路径 / 代号沿用 `hotupdate`（如规则 2 的 hotupdate-stable）。本注统一说明，不改动原文各句。
> - **#3 / #9 / #10**：是设计快照固有属性，非笔误，具体见正文对应位置的就地勘误。

## 1. Goal

After `System.Convert` and `System.String` have reached `0 uncovered`, the next objective is no longer method-by-method widening. The objective is to finish the remaining CoreLib supplemental runtime-skeleton closure through a stable capability-family architecture that remains compatible with future hotupdate.

This design freezes a four-phase execution order:

1. `LowLevelMemoryFamily`
2. `NumberAndFormattingFamily`
3. `TaskContinuationFamily`
4. `PlatformThenIntrinsics`

## 2. Current Truth

Current canonical truth is the latest `System.Private.CoreLib` supplemental runtime-skeleton coverage produced by:

- `artifacts/subjects/SolutionCorePack/runs/20260426-143601-windows-4240/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`

> **就地勘误 #9（2026-09-09）**：该路径含具体运行时间戳/ID（`20260426-143601-windows-4240`），是归档日**当次快照路径**。随 CI 运行清理，此精确制品副本通常已不可直接访问。应视为「当时参考坐标系」，而非预期可打开的文件路径。

Current state:

- `emittedMethodCount = 4658`（读法：前值 2261 → 现值 4658，增量 +2397）
- `uncoveredMethodCount = 50148`
- `System.Convert` uncovered = `0`
- `System.String` uncovered = `0`

> **就地勘误 #7（2026-09-09）**：原文 `(previously 2261 +2397)` 表意不清。订正读法：emitted count 前值 2261 → 现值 4658，增量为 +2397（非「两个历史累计之和」）。

The dominant remaining families are now:

- `System.Runtime.CompilerServices.Unsafe`
- `System.MemoryExtensions`
- `System.SpanHelpers`
- `System.Array`
- `System.Runtime.InteropServices.MemoryMarshal`
- `System.Number`
- `System.Decimal`
- `System.Half`
- `System.Threading.Tasks.Task`
- `System.Enum`
- `System.DateTime`
- `Interop+Kernel32`
- `System.Runtime.Intrinsics.*`

## 3. Architecture

The runtime-skeleton widening path is divided into three semantic layers:

1. `ManagedInvokeFamily`
2. `KernelFamily`
3. `Platform/IntrinsicFamily`

### ManagedInvokeFamily

Use this when the target method should keep managed semantic authority and the native side should only:

- resolve metadata token
- marshal carriers
- call `RuntimeAbiV0.method_invoke`
- return through a stable ABI

This is the preferred layer for hotupdate compatibility.

### KernelFamily

Use this when the target behavior is fundamentally low-level and should not be routed through managed token invoke each time, for example:

- span traversal
- pointer arithmetic
- bulk copy / clear / reverse
- parser / formatter internal kernels

### Platform/IntrinsicFamily

Use this only after platform capability boundaries are frozen.

This layer owns:

- feature probing
- supported / partial / unsupported gating
- ISA-specific lowering

It must never leak raw ISA assumptions upward into earlier families.

## 4. HotUpdate Rule

HotUpdate compatibility requires:

- patch sees stable capability/helper contract only
- patch does not depend on CPU/ISA details
- managed semantic ownership remains in managed bodies whenever possible
- platform capability and intrinsic availability are host/runtime facts, not patch-owned facts

Therefore:

- `ManagedInvokeFamily` is the first-choice layer whenever practical
- `KernelFamily` is used only where invoke-based ownership would be structurally wrong
- `Platform/IntrinsicFamily` comes last

## 5. Phase Plan

### Phase 1: LowLevelMemoryFamily

Scope:

- `System.Runtime.CompilerServices.Unsafe`
- `System.MemoryExtensions`
- `System.SpanHelpers`
- `System.Array`
- `System.Runtime.InteropServices.MemoryMarshal`

Architecture:

- `Unsafe` and the public wrapper-heavy subset of `MemoryExtensions` first go through `ManagedInvokeFamily`
- `SpanHelpers`, structural `Array` helpers, and the low-level subset of `MemoryMarshal` belong to `KernelFamily`

> 注意：Phase 分组名 `LowLevelMemoryFamily` 是执行阶段，与 §3 语义层 `…Family` 不同（见顶部 #5 勘误）。

### Phase 2: NumberAndFormattingFamily

Scope:

- `System.Number`
- `System.Decimal`
- `System.Half`
- `System.Char`
- `System.Enum`
- `System.DateTime`
- `System.Globalization.CultureData`
- `System.Globalization.DateTimeFormatInfo`

### Phase 3: TaskContinuationFamily

Scope:

- `System.Threading.Tasks.Task`
- continuation / awaiter / scheduler-facing runtime-skeleton residuals not already covered by the async family

> **就地勘误 #3（2026-09-09）**：`not already covered by the async family` 的精确物理边界由「async family」后续实际翻译进度决定，而该系列在本设计冻结之后另行维护 —— 本快照阶段无法静态钉死哪些 Task 方法最终落 Phase 3。Phase 3 开工时应以执行时刻的 uncovered-by-async 快照为唯一权威口径，本行仅为范围描述非冻结清单。


### Phase 4: PlatformThenIntrinsics

#### Phase 4A: Platform Capability

Scope:

- `Interop+Kernel32`
- `System.Runtime.InteropServices.Marshal`
- `System.Runtime.InteropServices.NFloat`
- `System.Diagnostics.Tracing.NativeRuntimeEventSource`
- other host/platform-dependent residuals

#### Phase 4B: Intrinsics

Scope:

- `System.Runtime.Intrinsics.Vector64/128/256/512`
- `System.Runtime.Intrinsics.X86.*`
- `System.Runtime.Intrinsics.Arm.*`
- `System.Runtime.Intrinsics.Wasm.*`

This means `Intrinsics-first` is not the total architecture. Its content is absorbed into Phase 4B after Phase 4A is frozen.

## 6. Why This Avoids Rework

If phase ordering follows this design:

- `LowLevelMemoryFamily` does not assume ISA-specific implementation details
- `NumberAndFormattingFamily` depends only on stable memory/kernel contracts
- `TaskContinuationFamily` depends only on stable invoke/kernel contracts
- `PlatformThenIntrinsics` extends the bottom, not rewrites the middle

Therefore later intrinsics work becomes additive rather than destructive.

## 7. Execution Rules

1. Every new family must declare whether it is `ManagedInvokeFamily`, `KernelFamily`, or `Platform/IntrinsicFamily`.
2. Every new helper ABI must explicitly state whether it is hotupdate-stable.
3. No widening is allowed to reintroduce legacy single-point emitter growth.
4. Phase 4A must complete before 4B starts.

> **就地勘误 #10（2026-09-09）**：原文未定义规则 4 的「4A complete」验收标准。补充验收口径（作为解释性约定，非原判定）：Phase 4A 视为完成，当且仅当 (a) §3 语义分层对 4A 范围内所有方法的归类决策冻结，且 (b) 任一后续 Phase 开始前重跑覆盖普查并核对 §4A 主类型缺口已按 4A 范围闭合。未达成 4A 冻结前不得启动 4B dispatch 落地。验收时点以执行 agent 在当前覆盖栅格绿灯为准。
