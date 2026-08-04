# Nightly Build 综合分析与优化修复计划

> 生成日期: 2026-06-28
> 数据来源: 上次成功 nightly build 日志 + 本地关键 family 验证

---

## 1. 当前状态总览

### 已通过 (entry.exe 生成成功)

| Family | Chunk | C 错误数 | 备注 |
|--------|-------|---------|------|
| System.ObjectModel | global-ns | 0 | ✅ 已完整验证 |
| System.Collections | global-ns | 0 | ✅ |
| System.Data.Common | global-ns | 0 | ✅ |
| System.Net.Http | global-ns | 0 | ✅ |
| System.Threading.Tasks.Parallel | global-ns | 0 | ✅ |
| System.Linq.Expressions | global-ns | 0 | ✅ |
| System.Text.Json | global-ns | 0 | ✅ |
| System.Security.Claims | global-ns | 0 | ✅ |
| System.Collections.Immutable | global-ns | 0 | ✅ (hephaestus cache) |

### 未通过 (有 C 错误)

| Family | Chunk | 错误 | 类型 | 优先级 |
|--------|-------|------|------|--------|
| System.Linq | global-ns | C2086 `_s48` 重复 | Codegen — slot 声明冲突 | P1 |
| System.Collections.NonGeneric | global-ns | 文件锁 CS2012 | 基础设施 | P0 |
| System.ComponentModel.TypeConverter | 全部 3 chunks | 文件锁 | 基础设施 | P0 |
| System.Private.CoreLib | 全部 42 chunks | C2084/C2027/C2665 | array_stubs.h 多重包含 | 已修复 ✅ |
| System.Xml.ReaderWriter | xml/system-xml-schema | SDK 路径 | 基础设施 | P0 |

---

## 2. 待修复问题 (按优先级)

### P0: 基础设施问题 (当前环境无法验证, 需 CI 解决)

| # | 问题 | 详细 | 影响 |
|---|------|------|------|
| 1 | **文件锁** | VBCSCompiler 残留进程锁住 SDK DLL | 全部 94 chunks |
| 2 | **cmake 链接** | chaos_pal.lib.lib 双后缀 | 部分 chunk 链接失败 |
| 3 | **nightly build 超时** | orchestrator 在 build 失败后 30s 无响应退出 | 无法获取完整报告 |

**修复方案:**
- CI 环境运行: `taskkill /F /IM dotnet.exe` 后再启动 nightly
- cmake template: 确认 `find_package(chaos)` 路径拼接逻辑

### P1: Codegen 编译错误

#### #1: System.Linq — C2086 `_s48`/`_s49` 重复定义

**根因:** 结构化 IR 方法的 `EmitStructuredSlotDeclarations` 在 `usesStructuredSlots` 分支中发射 `_sN` 声明，但方法的 body 发射过程中也通过 flat-fallback 路径发射了 `chaos_eval_stack`。同一个方法内 `_sN` 被声明两次。

**文件:** `NativeAotLoweringPlanner.MethodEmission.cs`

**修复方案:**
在 `DeduplicateTypeIdMtSymbols` post-processing 中, 对每个方法的 slot 声明区域去重 — 记录已见过的 `_sN` 符号名，跳过重复。

**预估:** 1 天

#### #2: System.Private.CoreLib 42 chunks — (已修复) C2084 等

**状态:** 已通过 `array_stubs.h` include 路径修复 + `stub_common.h` include guard 修复。但需要 CI 验证。

### P2: 性能优化

#### #1: `EmitStructuredSlotDeclarations` 统一化

**当前:** 结构化 IR 方法使用 `EmitStructuredSlotDeclarations` 逐方法声明 `_sN`；flat-fallback 方法用 `chaos_eval_stack` 数组。两种模式不能混合。

**优化:** 对所有方法统一使用 `_s0.._s63` + `chaos_eval_stack[32]` 的安全网，删除 `EmitStructuredSlotDeclarations`。~800 bytes/方法，C++ 优化器移除未使用的。

**文件:** `MethodEmission.cs` — `EmitManagedMethod`

**风险:** 需要验证 C2086 不出现 (安全网 + `EmitStructuredSlotDeclarations` 重复)

**预估:** 0.5 天

#### #2: 方法体发射并行度调优

**当前:** `_maxParallelism = max(1, cpu-2)`。大 chunk 的 Parallel.For 因 `ThreadLocal` 状态切换有额外开销。

**优化:** 动态并行度 — 方法数 < 100 时串行，避免 Parallel.For 调度开销。

**文件:** `Methods.cs` line 1054-1058

**预估:** 0.5 天

### P3: 已修复待验证

| 修复 | 提交 | 说明 |
|------|------|------|
| `array_stubs.h` include 路径 | `6512351f7` | `#include "cpu_features.h"` → `"runtime_stubs/cpu_features.h"` |
| `stub_common.h` include guard | linter | `#ifndef CHAOS_STUB_COMMON_H_INCLUDED` |
| ATG TFM net9.0 fallback | `6512351f7` | net9.0 失败时自动重试 net10.0 |
| `_emittedValueTypeSubjectIds` 合并 | `f3cdda89c` | ABI 扫描 valuetype 保存到独立字段 |
| `AddExternalRuntimeStubs` 返回类型覆盖 | `6512351f7` | 通配替换修复 C2660 |
| ThreadLocal `__st` 优化 | `d9f81b242` | 224 处 `_state.Value` → `__st` (+6ms/方法?) |
| Phase 5 并行 | `42d40482a` | BuildHotpatchTable + BuildExternalRuntimeDispatchTable + BuildGcSlotMapSection 并行 |
| slot 安全网 | `44b480198` | `_s0.._s63` + `_i0.._i31` 通用声明防 C2065 |

---

## 3. 推荐修复优先级

| 优先级 | 任务 | 预估 | 收益 |
|--------|------|------|------|
| **P0** | CI 环境配置 (文件锁 + cmake) | 环境问题 | 解锁全部验证 |
| **P1** | C2086 `_s48` 重复 (System.Linq) | 1 天 | 解锁 System.Linq build |
| **P1** | `EmitStructuredSlotDeclarations` + 安全网冲突 | 0.5 天 | 消除 C2086+C2374 |
| **P2** | System.Private.CoreLib 42 chunks CI 验证 | 1 天 (CI) | 确认修复有效 |
| **P2** | 并行度动态调优 | 0.5 天 | 小 chunk 省 ~200ms |
| **P3** | Benchmark 全量跑 (CI) | 2 天 (CI) | 确认无性能回归 |

---

## 4. 已验证的成果

| 指标 | 数值 |
|------|------|
| 总 commits | ~120 |
| 已修复 C 错误种类 | 16+ |
| 已验证通过 family | 9 个 |
| 已验证 entry.exe | 9/9 |
| 性能优化 | 3 项 |
| 架构文档 | 5 份 |
