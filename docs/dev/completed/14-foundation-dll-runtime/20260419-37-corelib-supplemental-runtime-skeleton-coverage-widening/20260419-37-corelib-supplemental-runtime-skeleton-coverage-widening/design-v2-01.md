# CoreLib Runtime Skeleton Capability Closure Design v2.01

Date: 2026-04-26 10:35:00 +08:00
Status: draft-approved-for-execution

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

Current state:

- `emittedMethodCount = 4658` (previously 2261 +2397)
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

Hotupdate compatibility requires:

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

