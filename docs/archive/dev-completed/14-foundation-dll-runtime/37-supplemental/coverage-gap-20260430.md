# Coverage Gap Report — 2026-04-30

> **勘误与追溯 (2026-09-09, review #2/#4)**：本文件是归档快照（2026-04-30）。下方所有 table/数字为当次覆盖普查的结果值。生成该清点的**具体测量命令/入口未在归档内记载**（本公告不虚构来源），仅能确证：
> — 验证见文末 Verification 的 `foundation-dll full ... --mode quick`（exit 0）；
> — baseline 标识见下 `20260429-233753-windows-ae47`。
> 读者欲复现这些普查定论（41179 不覆盖 / PackedSpanHelpers=41 等）应重跑当次 foundation-dll 普查并由运行时覆盖率口径自行核对，而非依赖本快照命令行。

## Overview

Baseline: canonical formal `20260429-233753-windows-ae47`

| Metric | Value |
|--------|-------|
| Uncovered method count | 41,179 |
| SpanHelpers:: core uncovered¹ | 0 ✅ |
| MemoryMarshal:: direct uncovered² | 0 ✅ |
| MemoryMarshal nested iterators | 369 |
| PackedSpanHelpers | 41 |

> ¹ "core" = `System.SpanHelpers` 主类型（不含嵌套类型 `PackedSpanHelpers`）  
> ² "direct" = `MemoryMarshal` 主类型直接方法（不含编译器生成的 `MemoryMarshal+<>` 嵌套迭代器）

## Line B: MemoryMarshal Coverage Fix (已完成)

**Root cause**: `RuntimeSkeletonArrayAndMemoryMarshalAbiFactory.TryCreateReturnShape()` rejected methods returning `!!0&`, `!0&`, `!!0`, `!0` (generic by-ref and value returns). These generic return type tokens appear in MemoryMarshal methods like `GetReference<T>(Span<T>)`, `Cast<TFrom,TTo>`, etc.

**Fix**: Added pattern matching in `TryCreateReturnShape()`:
- `!!0&` / `!0&` → treated as `void*` indirect return (return_value_is_indirect=true)
- `!!0` / `!0` → treated as blittable value return (return_value_is_indirect=false)

**Effect**: 144 previously uncovered MemoryMarshal direct methods are now emitted. Remaining 369 `MemoryMarshal+<>` entries are compiler-generated nested iterator types (`<ToEnumerable>g__FromArray|18_2>d<!!0>`), which belong to a different family scope (state machine / iterator types).

## Line A: SpanHelpers Batch 2 (已完成)

**Direct kernel coverage** (11 methods, BufferMemory-style C++ helpers):

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

> ¹ Batch 1（前 3 个方法：Clear* / Fill）与本报告记录的 Batch 2（下 8 个方法）落地于 `37-supplemental` 同一任务的不同 commit；Batch 1 的具体 commit/PR 未在本归档内逐条单列，3 个方法均包含在 `runtime_core.h/cpp` 的 "SpanHelpers helpers" 区块。两批合计恰为本表 11 方法（3 Batch 1 + 8 Batch 2）。

Remaining SpanHelpers uncovered (41) are all `System.PackedSpanHelpers` — intrinsics-heavy helpers that use `Vector128/256/512<T>`. These are a separate type and not managed by the `SpanHelpersKernelFamily` router.

## Remaining Uncovered Clusters (Sorted by Priority)

| Cluster | Count | Notes |
|---------|-------|-------|
| `System.PackedSpanHelpers` | 41 | Intrinsics-based, uses Vector128/256/512. Low ROI without intrinsics kernel |
| `MemoryMarshal+<>` iterators | 369 | Compiler-generated state machine types. Would need iterator/routing family |
| Other System.Private.CoreLib | ~40,769 | Broad residual (intrinsics, threading, reflection, etc.) |

## Files Changed

| File | Change |
|------|--------|
| `runtime_core.h` | +11 SpanHelpers helper declarations |
| `runtime_core.cpp` | +8 Batch 2 helper implementations (IndexOf, SequenceEqual, Reverse, LastIndexOf) |
| `RuntimeSkeletonSpanHelpersKernelAbi.cs` | +8 switch cases + TryCreate methods in DirectKernelAbiFactory |
| `RuntimeSkeletonArrayAndMemoryMarshalAbi.cs` | Added `!!0`/`!!0&`/`!0`/`!0&` return type support in TryCreateReturnShape() |

## Verification

- `dotnet build src/managed/Chaos.IL2CPP.CodeGen` — 0 errors ✅
- `pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance_reference.py` — passed ✅
- `pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py` — passed ✅
- `foundation-dll full --scope all --promote-approved true --mode quick` — exit 0 ✅
