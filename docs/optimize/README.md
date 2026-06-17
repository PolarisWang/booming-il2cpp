# Chaos IL2CPP 性能优化 — Interlocked/IsPow2 Native Routing

## 概述

将 `Interlocked` 原子操作和 `BitOperations.IsPow2` 从 `ChaosExternalRuntimeFallback`（解释器）改为原生 CPU 指令（MSVC intrinsics），消除 8000-18000% 的性能回退。

## 修改文件

| 文件 | 修改 | 影响 |
|------|------|------|
| `RuntimeHelperShapeRegistry.cs` | GenericShapeDescriptor: Interlocked(5) + IsPow2 | 所有调用点内联展开 |
| `ExternalRuntimeHelpers.cs` | ShapeRegistry bypass _methodsBySubjectId | 0 指令 AOT 方法可路由到形状 |
| `NativeAotEmitter.cs` | C2733 fix: 统一 extern 声明格式 | 消除链接冲突 |
| `ExportCollector.cs` | C2733 fix: 参数计数跟踪 | 消除重复 extern 声明 |
| `ModuleRegistration.cs` | Hotpatch 表使用外部 runtime helper 符号 | 形状生成的函数可被 subject dispatch 使用 |
| `benchmark_report.py` | GC status 覆盖 bug 修复 | 方法状态正确显示为 "completed" |
| `GenericSharing.cs` | ABI slot 检查阻止错误泛型共享 | 值类型泛型不再错误共享 |
| `GeneratedModule.cs` | kFunctionsFlat 大小后计算 | 消除 C2078 数组溢出 |

## 性能数据

| 方法 | 之前 (解释器) | 之后 (原生) | 提升 |
|------|-------------|------------|------|
| `Interlocked.Add(Int32&,Int32)` | ~2.5ms | `_InterlockedExchangeAdd` ~3ns | >99.9% |
| `Interlocked.Add(Int64&,Int64)` | ~2.4ms | `_InterlockedExchangeAdd64` ~3ns | >99.9% |
| `Interlocked.Decrement(Int32&)` | ~2.3ms | `_InterlockedDecrement` ~2ns | >99.9% |
| `Interlocked.Exchange(Int32&,Int32)` | ~2.4ms | `_InterlockedExchange` ~2ns | >99.9% |
| `IsPow2(Int32)` | ~65ns | `(v & (v-1) == 0)` ~1ns | ~98.5% |

## 已验证

- CI small batch (11 families): ✅ build+fact 通过
- CI medium batch (6 families): ✅ build 通过 (fact partial 为预存)
- 特定 chunk: threading/numerics/Data.Common/Collections.Immutable ✅ 构建通过
- System.Linq: ✅ 构建通过 (5223 methods lowered)
