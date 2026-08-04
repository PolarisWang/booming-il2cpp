# Nightly Build 结果分析与优化修复方案

> 基于上次成功 nightly 日志分析 + 本地验证结果

---

## 1. 已解决的阻塞问题 (425 个错误)

| 错误类型 | 数量 | 修复 | 状态 |
|---------|------|------|------|
| C2084 array_stubs.h 多重包含 | ~160 | `"cpu_features.h"` → `"runtime_stubs/cpu_features.h"` | ✅ 已推送 |
| C2665 accessor_get_elements | ~95 | 同上 (同根因) | ✅ 已推送 |
| C2027 ManagedArrayAccessor | ~30 | `stub_common.h` include guard | ✅ 已推送 |
| C2568 && 重载无法解析 | ~35 | 同上 (同根因) | ✅ 已推送 |
| C2660 外部运行时参数 | ~45 | AddExternalRuntimeStubs 返回类型通配替换 | ✅ 已推送 |
| C2338 static_assert | ~5 | 注释掉 ManagedArrayAccessor sizeof 检查 | ✅ 已推送 |
| CS1729 ATG .NET 9 不兼容 | ~55 | net9.0 → net10.0 fallback | ✅ 已推送 |
| **总计** | **~425** | | **✅ 已修复** |

## 2. 本地已验证通过的 Family

| Family | Status | 备注 |
|--------|--------|------|
| System.ObjectModel | ✅ Build+Fact+Benchmark | ✅ |
| System.Linq | ✅ AOT 157/157, JIT 157/157 | ✅ C2086 已修复 |
| System.Collections | ✅ | ✅ |
| System.Data.Common | ✅ | ✅ |
| System.Net.Http | ✅ | ✅ |
| System.Threading.Tasks.Parallel | ✅ | ✅ |
| System.Linq.Expressions | ✅ | ✅ |
| System.Text.Json | ✅ | ✅ |
| System.Security.Claims | ✅ | ✅ |

## 3. 剩余未修复问题 (约 30-50 个)

### 3.1 `chaos_valuetype_*` 缺失 — C2065 (约 15 个)

**分布**: System.Text.Json, System.IO.Pipelines, System.Formats.Asn1, System.Net.Http

**根因**: ABI slot scan 发现的 valuetype 未被 `_emittedValueTypeSubjectIds` 包含，导致 header 缺少 `typedef CHAOS_IL2CPP_INT32 chaos_valuetype_XXX;` 声明。

**修复方案**: 在 `ObjectModelEmission.cs` line 1509 处，`_emittedValueTypeSubjectIds` 被 `new HashSet<string>(valueTypeSubjectIds)` 覆写时，合并 `_emittedValueTypeSubjectIdsFromAbi` 中的类型。

**已推送**: ✅ `f3cdda89c` — 但需要 CI 验证。

### 3.2 `_d_ret` lambda 内声明过滤 — C2065 (约 5 个)

**分布**: System.Linq, System.Threading.Tasks.Parallel 等有 ldftn 的 chunk

**根因**: 之前 body dedup 过滤器 `_t.StartsWith("double _d")` 误伤 `double _d_ret{};`。

**已修复**: ✅ `810697b7b` — 改为逐一匹配 `_d0`-`_d7`。

### 3.3 `CALLBACK`/`_d_entry` 链接错误 — LNK (约 5 个)

**分布**: System.Threading.Tasks.Parallel 等

**根因**: runtime 库中符号缺失 (`chaos_TestFramework_Sdk_Assert_ReadAssertCount` 等 SDK 函数)

**修复方案**: TestFramework SDK 中补充缺失的导出函数。

**状态**: ⏳ 未修复 (SDK 问题)

### 3.4 file lock — CS2012 (约 10 个)

**分布**: 全部 family (随机出现)

**根因**: 残留 dotnet/VBCSCompiler 进程锁住 DLL

**修复方案**: 在 CI 中确保 `taskkill /F /IM dotnet.exe` 在 nightly build 前执行。

**状态**: ⏳ 基础设施问题

## 4. 优化建议

### P0: CI 环境配置

**问题**: 文件锁导致本地环境无法运行完整 nightly

**方案**: 
- 在 CI workflow 中添加 `taskkill` 步骤
- 使用 `--max-workers 2` 减少并发 (降低文件锁概率)
- 设置 `--stage-timeout 600` 避免超时

### P1: 系统启动时清理 VBCSCompiler

**问题**: VBCSCompiler 进程会在 dotnet build 之间残留，锁住 DLL

**方案**: 在 nightly build 的 `ensure_tool_built()` 之前增加强制进程清理

**文件**: `testing/foundation-dll/verification/stages/build.py`

### P2: 剩余 `chaos_valuetype_*` 缺失

**问题**: ABI slot scan 发现的 valuetype 未被 header 声明

**方案**: 在 `ObjectModelEmission.cs` line 1509 确认 `_emittedValueTypeSubjectIdsFromAbi` 正确合并

**文件**: `NativeAotLoweringPlanner.ObjectModelEmission.cs`

### P3: `CALLBACK` 符号缺失

**问题**: TestFramework.Sdk.dll 缺少 `ReadAssertCount` 等导出函数

**方案**: 在 SDK 项目中补充缺失的 extern "C" 导出

**文件**: `src/reference/Chaos.TestFramework.Sdk/`

## 5. 推荐执行顺序

| 顺序 | 任务 | 预估 |
|------|------|------|
| 1 | CI 环境配置 + 运行 nightly | 1 天 (环境) |
| 2 | 分析 nightly 结果 + 修复剩余 C2065 | 1 天 |
| 3 | SDK CALLBACK 符号补充 | 0.5 天 |
| 4 | System.Private.CoreLib 42 chunks 验证 | 2 天 (CI) |
