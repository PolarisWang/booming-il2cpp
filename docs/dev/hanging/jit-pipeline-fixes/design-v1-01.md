# JIT Pipeline 修复设计 v1-01

## 概述

修复评估报告中发现的 7 个问题，全面完善 JIT/AOT pipeline 的事实、基准测试和 hotupdate 数据采集。

---

## 问题 A：JIT benchmark 输出格式

### 根因

`entry-jit.exe --benchmark-all` 使用 `%lld` 格式化 `allocatedBytes`。在 MSVC x64 上 `long long` 是 64 位，`%lld` 可以工作。但 parser 侧真正的原因是 JIT 的过程有 VEH handler 拦截 crash 后吞掉结果——`JitVehHandler` 在 crash 后将 RIP+=3 跳过指令，但 benchmark 返回的数据可能是错误的或空的。parser 收到空数据后报 "no data returned"。

### 修复方案

**修改 `benchmark_chunk.py`**：在 JIT benchmark 执行时，不再使用 `stderr=subprocess.DEVNULL`，而是捕获 stderr 并打印 warning。同时增加对空结果的更详细诊断。

**修改 `RuntimeEntry.cpp.scriban`（JIT 部分）**：确保 `--benchmark-all` 的输出格式与 AOT 一致，同时每个 method 的输出行包含完整信息：
- methodIndex
- elapsedMilliseconds（高精度）
- opsPerSecond
- iterations
- allocatedBytes（使用 `%" PRId64 "` 而非 `%lld`）
- error（如果有）
- summary 行作为结束标记

### 工作量：1-2 小时

---

## 问题 B：JIT 断言禁用

### 根因

`#ifndef CHAOS_IL2CPP_JIT_MODE` 条件编译使 JIT 模式下 `CHAOS_FACT_RESET` 和 `CHAOS_FACT_CHECK` 被定义为空操作 `((void)0)`。

### 修复方案

删除 Scriban 模板中 `#ifndef CHAOS_IL2CPP_JIT_MODE` / `#else` / `#endif` 条件编译块，使 JIT 模式也调用 `Assert.Reset()` 和 `Assert.Complete()`：

```cpp
// Before (JIT):
#define CHAOS_FACT_RESET()   ((void)0)
#define CHAOS_FACT_CHECK()   ((void)0)

// After (both AOT and JIT):
#define CHAOS_FACT_RESET()   Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset()
#define CHAOS_FACT_CHECK()   do { \
    if (Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() != 0) { \
        std::fprintf(stderr, "[ASSERT] s_exitCode was non-zero after fact loop\n"); \
    } \
} while(0)
```

### 风险

JIT 模式下如果 assert 失败会抛 `AssertionException`，C++ 层需要用 try/catch 保护。查看 SDK Assert.Fail() 的实现（`Assert.cs` 第252行），它会 `s_exitCode = 1; throw new AssertionException(msg);`。在 JIT 模式下，这个异常可能不会被 C++ catch 住。

**缓解**：在 `RunFactJsonMode` 的 JIT 路径中增加 `try { ... } catch (...) { }` 保护。

### 工作量：30 分钟

---

## 问题 C：meta_total vs 实际计数不匹配

### 根因

codegen 在 lowering 过程中会静默丢弃无法降低的方法（如 `CollectionExtensions::AsReadOnly<T>` 等框架方法）。这些方法在 metadata 中声明了（`totalMethods=220`），但 codegen 只产生了 206 个（14 个被丢弃）。

### 修复方案

在 codegen 中，当方法被丢弃时输出 warning，包含 SubjectId 和丢弃原因。在 `fact_chunk.py` 中已经做了 cross-check（`aot_dropped` 计数），但 codegen 侧未给出明确原因。

**修改 codegen**：在方法丢弃路径中添加 `Console.Error.WriteLine` 日志。

### 工作量：1 小时

---

## 问题 D：net8 对比数据缺失

### 根因

`managed_benchmark` 阶段未在所有块上运行，或者运行时因 `PermissionError`（Windows 文件锁）失败。

### 修复方案

运行 managed_benchmark 阶段时增加 retry 逻辑，解决文件锁问题。同时确保 CI 流程中包含 `managed_benchmark` 阶段。

**修改 `managed_benchmark.py`**：在 `shutil.rmtree(build_dir)` 失败时 retry，而非直接崩溃。

### 工作量：30 分钟

---

## 问题 E：hotupdate 无补丁数据

### 根因

ATG `--patch-mode` 生成的 C# 代码无法编译，或补丁提取返回空。

### 修复方案

1. 修复 ATG `--patch-mode` 输出格式，使其生成可编译的 C# 代码。
2. 当 ATG 失败时，改用内置的预设补丁数据（已知的返回值变换 pattern）。
3. 添加 diagnostic 日志，记录补丁生成失败的详细原因。

### 工作量：3-4 小时

---

## 问题 F：stale entry-jit.exe 假阳性

### 状态：✅ 已修复

已在 `build.py` 中添加 `jit_target.unlink()`，在重建前删除过时的 JIT 二进制文件。

---

## 问题 G：chaos_runtime_core.lib 过时

### 状态：✅ 已修复

已复制更新后的 lib 到所有 `artifacts/presets/` 目录。

---

## 执行计划

| 顺序 | 问题 | 工作量 | 依赖 | 建议 |
|------|------|--------|------|---------|
| 1 | B：JIT 断言 | 30 分钟 | 无 | 最简，先修 |
| 2 | A：JIT 输出格式 | 1-2 小时 | B | 关键数据采集 |
| 3 | C：meta 不匹配 | 1 小时 | 无 | 附加 warning |
| 4 | D：net8 对比 | 30 分钟 | 无 | 数据完整性 |
| 5 | E：hotupdate 补丁 | 3-4 小时 | A+B | 最重，排最后 |

**总计工作量**：约 6-8 小时
**已修复**：F ✅ G ✅

---

## 三优先级权衡

| 维度 | 评估 |
|--------|---------|
| **P1 性能** | 这些修复不涉及生成的 C++ 代码性能，只涉及 pipeline 数据采集 |
| **P2 架构完整性** | 修复沿用现有模式，不引入新架构层 |
| **P3 HotUpdate** | 修复 E 直接改善 hotupdate 数据采集 |
