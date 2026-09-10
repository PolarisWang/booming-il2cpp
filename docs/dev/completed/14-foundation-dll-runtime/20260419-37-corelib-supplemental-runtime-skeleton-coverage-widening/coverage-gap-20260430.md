# Coverage Gap Report — 2026-04-30

## Overview

Baseline: canonical formal `20260429-233753-windows-ae47`

> **术语口径（本报告内固定含义）**
> - `SpanHelpers:: core` — 仅指 `System.SpanHelpers` 主类型自身的直接方法，**不含** `System.PackedSpanHelpers`
> - `MemoryMarshal:: direct` — 仅指 `System.Runtime.InteropServices.MemoryMarshal` 的直接方法，**不含**其编译器生成的嵌套迭代器类型 `MemoryMarshal+<>`

| Metric | Value |
|--------|-------|
| Uncovered method count | 41,179 |
| SpanHelpers:: core uncovered | 0 ✅ |
| MemoryMarshal:: direct uncovered | 0 ✅ |
| MemoryMarshal nested iterators | 369 |
| PackedSpanHelpers | 41 |

**数字闭合校验**：三个剩余簇 41 + 369 + ~40,769 = 41,179，与 `Uncovered method count` 一致。

## Line B: MemoryMarshal Coverage Fix (已完成)

**Root cause**: `RuntimeSkeletonArrayAndMemoryMarshalAbiFactory.TryCreateReturnShape()` rejected methods returning `!!0&`, `!0&`, `!!0`, `!0` (generic by-ref and value returns). These generic return type tokens appear in MemoryMarshal methods like `GetReference<T>(Span<T>)`, `Cast<TFrom,TTo>`, etc.

**Fix**: Added pattern matching in `TryCreateReturnShape()`:
- `!!0&` / `!0&` → treated as `void*` indirect return (return_value_is_indirect=true)
- `!!0` / `!0` → treated as blittable value return (return_value_is_indirect=false)

**Effect**: 144 previously uncovered MemoryMarshal direct methods are now emitted. Remaining 369 `MemoryMarshal+<>` entries are compiler-generated nested iterator types (`<ToEnumerable>g__FromArray|18_2>d<!!0>`), which belong to a different family scope (state machine / iterator types).

## Line A: SpanHelpers Batch 2 (已完成)

**Direct kernel coverage** (11 methods, BufferMemory-style C++ helpers):

> 表中 **Batch 1** 的 3 个方法（`ClearWithoutReferences` / `ClearWithReferences` / `Fill`）落地于前一批次，
> 证据见同目录 `STATUS.md` 时间线 `2026-04-26 12:15:00 +08:00` 条目（`UnsafeManagedInvokeFamily` 完成记录）
> 与 `plan-v2-01.md` 的 `Batch 1: UnsafeManagedInvoke`。本次新增为 **Batch 2 的 8 个方法**；
> 表内 11 = Batch 1 (3) + Batch 2 (8) 的**累计**视图。

| Method | Helper | Status |
|--------|--------|--------|
| `ClearWithoutReferences` | `SpanHelpersClearWithoutReferences` | Batch 1 ✅ |
| `ClearWithReferences` | `SpanHelpersClearWithReferences` | Batch 1 ✅ |
| `Fill` | `SpanHelpersFillByte` | Batch 1 ✅ |
| `IndexOf(System.Byte&..)` | `SpanHelpersIndexOfByte` | Batch 2 ✅ |
| `IndexOf(System.Char&..)` | `SpanHelpersIndexOfChar` | Batch 2 ✅ |
| `SequenceEqual(System.Byte&..)` | `SpanHelpersSequenceEqualByte` | Batch 2 ✅ |
| `SequenceEqual(System.Char&..)` | `SpanHelpersSequenceEqualChar` | Batch 2 ✅ |
| `Reverse(System.Byte&..)` | `SpanHelpersReverseByte` | Batch 2 ✅ |
| `Reverse(System.Char&..)` | `SpanHelpersReverseChar` | Batch 2 ✅ |
| `LastIndexOf(System.Byte&..)` | `SpanHelpersLastIndexOfByte` | Batch 2 ✅ |
| `LastIndexOf(System.Char&..)` | `SpanHelpersLastIndexOfChar` | Batch 2 ✅ |

Remaining uncovered outside `System.SpanHelpers` itself: **41** methods of the separate type `System.PackedSpanHelpers` — intrinsics-heavy helpers that use `Vector128/256/512<T>`. `System.PackedSpanHelpers` is a distinct type and is not managed by the `SpanHelpersKernelFamily` router, so it is not part of the `SpanHelpers:: core` figure above.

## Remaining Uncovered Clusters (Sorted by Priority)

| Cluster | Count | Notes |
|---------|-------|-------|
| `System.PackedSpanHelpers` | 41 | Intrinsics-based, uses Vector128/256/512. Low ROI without intrinsics kernel |
| `MemoryMarshal+<>` iterators | 369 | Compiler-generated state machine types. Would need iterator/routing family |
| Other System.Private.CoreLib | ~40,769 | Broad residual (intrinsics, threading, reflection, etc.) |

## Files Changed

| File | Change |
|------|--------|
| `runtime_core.h` | +11 SpanHelpers helper declarations (**累计**：Batch 1 的 3 + Batch 2 的 8) |
| `runtime_core.cpp` | +8 **本次 Batch 2** helper implementations (IndexOf, SequenceEqual, Reverse, LastIndexOf) |
| `RuntimeSkeletonSpanHelpersKernelAbi.cs` | +8 switch cases + TryCreate methods in DirectKernelAbiFactory |
| `RuntimeSkeletonArrayAndMemoryMarshalAbi.cs` | Added `!!0`/`!!0&`/`!0`/`!0&` return type support in TryCreateReturnShape() |

## Verification

- `dotnet build src/managed/Chaos.IL2CPP.CodeGen` — 0 errors ✅
- `pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance_reference.py` — passed ✅
- `pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py` — passed ✅
- `foundation-dll full --scope all --promote-approved true --mode quick` — exit 0 ✅

### 覆盖率普查数字的复现入口

上表 `Uncovered method count` / 各簇计数 / `144 个新发射` **不来自上述 build/pytest**，它们
来自 supplemental-full-closures 覆盖率普查产物：

```bash
# 1. 生成/刷新覆盖率普查产物（写入 artifacts/subjects/SolutionCorePack/runs/<run-id>/…）
foundation-dll full --scope all --mode quick

# 2. 读取普查结果（本报告基线 run-id = 20260429-233753-windows-ae47）
python -c "import json;d=json.load(open('artifacts/subjects/SolutionCorePack/runs/20260429-233753-windows-ae47/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json'));print(d['emittedMethodCount'],d['uncoveredMethodCount'])"
```

> `run-id` 是**每次运行新生成**的，上文的 `20260429-233753-windows-ae47` 只是撰写本报告时的基线标识，
> 后续接手者重跑会得到**新的 run-id**，请以最新 run 为准而不是硬编码本报告的值。
> 各簇计数（369 / 41 / ~40,769）由该 coverage.json 按类型名聚合得到，`emittedMethodCount` 的增量
> （本次 +144 MemoryMarshal direct）同样取自该文件。
