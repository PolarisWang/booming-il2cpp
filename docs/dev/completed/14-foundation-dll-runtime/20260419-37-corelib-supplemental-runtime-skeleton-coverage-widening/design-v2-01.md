# CoreLib Runtime Skeleton Capability Closure Design v2.01

Date: 2026-04-26 10:35:00 +08:00
Status: draft-approved-for-execution

> **审批说明**：本文档由 [@PolarisWong] 于 `2026-04-26` 初步定稿，作为 14-foundation-dll-runtime 阶段
> 的核心设计参考。无单独 issue/PR 编号；审批以同目录 `STATUS.md` 时间线 `2026-04-26 12:15:00` 的
> "Batch 1 UnsafeManagedInvoke complete" 执行验收为准。接手者如有争议应重启 `dev-brainstorm`。

## 1. Goal

After `System.Convert` and `System.String` have reached `0 uncovered`, the next objective is no longer method-by-method widening. The objective is to finish the remaining CoreLib supplemental runtime-skeleton closure through a stable capability-family architecture that remains compatible with future HotUpdate.

This design freezes a four-phase execution order:

1. `LowLevelMemoryFamily`
2. `NumberAndFormattingFamily`
3. `TaskContinuationFamily`
4. `PlatformThenIntrinsics`

## 2. Current Truth

Current canonical truth is the latest `System.Private.CoreLib` supplemental runtime-skeleton coverage produced by:

- `<repo>/artifacts/subjects/SolutionCorePack/runs/<run-id>/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`

> 撰写本文档时使用的基线 `<run-id>` = `20260426-143601-windows-4240`（相对路径见上）。
> `run-id` 每次运行新生成，**接手者重跑请取最新 run**，不要硬编码下标；复现命令见同目录
> `coverage-gap-20260430.md` 的「覆盖率普查数字的复现入口」小节。

Current state (baseline `20260426-143601-windows-4240`):

- `emittedMethodCount = 4658`（前序累计 `2261`，本阶段新增 `+2397`）
- `uncoveredMethodCount = 50148`
- `System.Convert` uncovered = `0`
- `System.String` uncovered = `0`

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

> **术语区分（全文固定）**
> - **Layer Family**（架构层）— 描述"目标方法该走哪条语义路径"，共 3 个：
>   `ManagedInvokeFamily` / `KernelFamily` / `Platform/IntrinsicFamily`（本节定义）。
> - **Phase Group**（执行阶段分组）— 描述"按什么顺序推进"，见 §5：
>   `LowLevelMemoryFamily` / `NumberAndFormattingFamily` / `TaskContinuationFamily` / `PlatformThenIntrinsics`。
>
> 两者**不在同一分类轴上**：一个 Phase Group 内部可以同时使用多个 Layer Family
> （例：`LowLevelMemoryFamily` 里 `Unsafe` 走 ManagedInvoke，`SpanHelpers` 走 Kernel）。
> 阅读 §5 时请勿把 Phase Group 名当成架构层名。

The runtime-skeleton widening path is divided into three semantic layers (Layer Family):

1. `ManagedInvokeFamily`
2. `KernelFamily`
3. `Platform/IntrinsicFamily`

### ManagedInvokeFamily

Use this when the target method should keep managed semantic authority and the native side should only:

- resolve metadata token
- marshal carriers
- call `RuntimeAbiV0.method_invoke`
- return through a stable ABI

This is the preferred layer for HotUpdate compatibility.

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

## 4. HotUpdate 规则

> **大小写约定**：本文档统一使用 `HotUpdate`（大H大U）作为专有名词冠词形式
> （如 `HotUpdate compatibility`、`HotUpdate-stable`），使用全小写 `hotupdate`
> 作为文件/配置/路径中的裸引用。§6 之后所有引用均统一为此约定。

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

> **Phase 3 的精确边界**：async family 的覆盖范围是动态的（该类型组可能后续追加新方法），
> Phase 3 的范围随之扩张/收缩。**本文档冻结时的基准**以同目录 `coverage-gap-20260430.md` 的
> 基线（`20260429-233753-windows-ae47`）为准：凡该基线中 `async` 相关 family 已覆盖的方法，
> Phase 3 不重复覆盖；基线中仍在 uncovered 中的 Task 方法则属于 Phase 3。
> 接手者若发现 async family 已变更，需重新运行覆盖率普查后更新 Phase 3 范围。

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
2. Every new helper ABI must explicitly state whether it is HotUpdate-stable.
3. No widening is allowed to reintroduce legacy single-point emitter growth.
4. Phase 4A must complete before 4B starts.

   > **"complete" 的验收标准**：Phase 4A 范围内所有类型（`Interop+Kernel32` / `Marshal` / `NFloat`
   > / `NativeRuntimeEventSource` 等）的 `uncoveredCount = 0`（由覆盖率普查产物确认），
   > 且对应的 runtime-skeleton helper 已通过 foundation-dll full pipeline 验证。
   > 4A 所有 method 落地 after 才可进入 4B。

