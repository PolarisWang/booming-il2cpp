# P0 性能优化经验记录

## 概述

本轮 P0 优化涉及 **dispatch 开销消除、SubjectWrapper 异常路径修复、BenchmarkRunner 改造、HotUpdate 稳定性修复**等多个方向。本文记录关键根因链和修复方案。

---

## 1. dispatch 根因链

### 问题现象

CoreLib runtime-interop chunk 对比 .NET 8 慢 **-30,900%**（旧数据），实际真实差距约 **-88%~-300%**（归一化后）。

### 根因链

```
SubjectWrapper 方法用 default(T)! 创建实例
  → 引用类型 default(T) = null
    → 方法调用抛 NullReferenceException
      → try/catch 拦截异常 (SEH)
        → SEH ~50-100+ 额外指令/方法
          → return 42 的方法耗时从 0.000ms → 51ms
            → Benchmark 报告 "dispatch_overhead"
```

**三层问题叠加**：
1. `default(T)!`  → 所有引用类型方法必然抛 NRE
2. `try/catch(...)` → MSVC 下触发 SEH（不是 C++ EH），每个方法 50-100 条额外指令
3. `chaos_runtime_get_abi_v0()->object_get_type_info()` → ABI 表函数指针间接调用

### 修复方案

| 层 | 修复 | 效果 |
|----|------|------|
| 1a | ABI 间接层消除 — `object_get_type_info` 替换为 inline `chaos_object_get_type_info` | ~5-15ns/call |
| 1b | `vtable_resolve` 消除 — `chaos_vtable_resolve(vtable, slot)` → `vtable[slot]` | ~2-5ns/call |
| 2 | try/catch 跳过 — 无 callee 的 SubjectWrapper 不生成 try/catch | return 42 5x 提升 |
| 3 | SubjectFactory — `RuntimeHelpers.GetUninitializedObject` 创建有效实例 | Asn1 88x, Brotli 290x |

### 关键文件

- `NativeAotLoweringPlanner.MethodEmission.cs` — try/catch 生成逻辑
- `NativeAotLoweringPlanner.ExceptionEmission.cs` — vtable dispatch 生成
- `ChaosGeneratedRuntimePrelude.h` — vtable_resolve 定义

---

## 2. SubjectInstanceFactory

### 问题

AutoTestGenerator 生成的 SubjectWrapper 方法使用 `default(T)!` 作为实例表达式。对于引用类型，default = null，调用方法抛 NRE。

### 方案

在 `Chaos.TestFramework.Sdk` 中新增 `SubjectInstanceFactory.Create<T>()`：

```csharp
public static T Create<T>() {
    if (typeof(T).IsValueType) return default(T);
    return (T)RuntimeHelpers.GetUninitializedObject(typeof(T))!;
}
```

在 `CSharpExpressionBuilder.GetInstanceExpression()` 中，namespace-qualified 类型的 fallback 从 `default(global::T)!` 改为 `SubjectInstanceFactory.Create<global::T>()`。

### 注意事项

- **ref struct** 不能作为泛型类型参数 → fallback to `default(T)!`
- **无法通过 Type.GetType() 解析的类型** → fallback to `default(T)!`

### 效果

| Assembly | Before | After |
|----------|--------|-------|
| System.Formats.Asn1 | -36,192% | **-409%** (88x) |
| System.IO.Compression.Brotli | -183,380% | **-36,356%** (5x, subject 方法 290x) |

### 关键文件

- `src/reference/Chaos.TestFramework.Sdk/SubjectInstanceFactory.cs`
- `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/CSharpExpressionBuilder.cs`

---

## 3. REVERT_FAIL (HotUpdate Unpatch Crash)

### 问题现象

3 assembly 在 hotupdate 测试中 REVERT_FAIL，exit code 3221225477 = **STATUS_ACCESS_VIOLATION (0xC0000005)**

### 根因

```
ApplyPatchFromMemory → SetPatchedBySlot(slot, true)
  → 保存 original_aot_ptr = entry->direct_ptr
  → DP1-a transfer 更新 direct_ptr 到 JIT 代码

Unpatch → SetPatchedBySlot(slot, false)
  → 清除 kHotpatchActive flag
  → 清除 method_key
  → ❌ 未恢复 entry->direct_ptr 到原始 AOT 代码

dispatch 检查 kHotpatchActive = false
  → 使用 direct_ptr 分派
  → direct_ptr 仍指向已释放的 JIT 代码
  → JMP 到释放内存 → ACCESS_VIOLATION
```

### 修复

在 `hotpatch_table.cpp` 的 SetPatchedBySlot unpatch 分支增加：

```cpp
if (auto* cb = GetOriginalAotPtrCallback()) {
    if (void* orig_ptr = cb(entry); orig_ptr && orig_ptr != entry->direct_ptr)
        entry->direct_ptr = orig_ptr;
}
```

与 patch 时的 Gap2 使用同一回调恢复 direct_ptr。

### 关键文件

- `src/native/runtime-core/hotpatch_table.cpp` — SetPatchedBySlot unpatch 分支

---

## 4. BenchmarkRunner 改造

### 问题

managed_benchmark 阶段无法产生 .NET 8 基线数据（一直返回 `{"results":[]}`）。

### 根因

BenchmarkRunner 只搜索 `[Benchmark]` 属性的静态方法。但 ATG 生成的 SubjectWrapper 方法：
1. 是**实例方法**（非 static）
2. 只有 `[Fact]` 和 `[HotUpdate]` 属性（不生成 `[Benchmark]`）

### 修复

1. 搜索范围从 `Static` → `Static | Instance`
2. 实例方法需要 `Activator.CreateInstance(type)` 创建实例
3. 过滤条件从 `[Benchmark]` → `[Fact]` 或 `[HotUpdate]`
4. 实例传给 `method.Invoke(instance, null)`

### 效果

基线覆盖从 13 assembly → **23 assembly**，方法数从 ~1,200 → **~8,000+**

### 关键文件

- `src/reference/Chaos.TestFramework.Runtime/BenchmarkRunner.cs`

---

## 5. benchmark_report 归一化

### 问题

对比数据被校准偏差污染：AOT benchmark 用自适迭代（目标 ~50ms），managed benchmark 用固定 1000 迭代。比较原始总时间导致 False Outlier（如 Parallel -39,214% → 归一化后 -306%）。

### 修复

1. `_get_elapsed()` 返回 `elapsedMilliseconds / iterations`（每迭代时间）
2. `_read_jsonl_technology_map()` 用 MAX elapsedMs 替代最新时间戳（避免计时器下限 0.001ms）

### 关键文件

- `testing/foundation-dll/verification/stages/benchmark_report.py`

---

## 6. PATCH_FAIL (ATG --patch-mode using 缺失)

### 问题

ATG `--patch-mode` 生成的 patch C# 代码引用 `JsonNode`、`XmlMapping` 等类型但缺少对应的 `using` 指令，导致 6 chunks 的 patch DLL 构建失败。

### 修复

在 `Program.cs` 的 RunPatchMode 中增加 assembly 级别的 using 指令：

| Assembly | 增加的 using |
|----------|-------------|
| System.Text.Json | `System.Text.Json.Nodes`, `System.Text.Json.Schema` |
| System.Private.Xml | `System.Xml`, `System.Xml.Schema`, `System.Xml.Serialization`, `System.Xml.Xsl`, `System.Xml.XPath`, `System.Xml.Linq` |
| System.Security.Cryptography | `System.Security.Cryptography` |

### 关键文件

- `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/Program.cs`

---

## 7. 外部运行时 hash 查找

### 问题

`ChaosExternalRuntimeFallback` Phase 2 用 O(n) 线性 strcmp 扫描 + PalTryCallNoExcept SEH 包裹查找外部运行时表（108 条目）。

### 修复

替换为 `ankerl::unordered_dense::map` hash 表，懒初始化，O(1) 查找。

### 关键文件

- `src/native/runtime-core/runtime_stubs/interop_stubs.cpp`

---

## 8. IsPow2 原生桩函数

### 问题

`BitOperations::IsPow2` 在外部运行时表中无对应条目，回退到解释器执行（2.7ms/call vs Net8 0.014ms）。

### 修复

在 `hotpatch_resolve.cpp` 的 fallback 循环中注册原生函数指针：
```cpp
CHAOS_IL2CPP_INTPTR ChaosBitOperationsIsPow2Impl(CHAOS_IL2CPP_INTPTR value) noexcept {
    auto v = static_cast<uint64_t>(value);
    return (v != 0) && ((v & (v - 1)) == 0) ? 1 : 0;
}
```

效果：**2.7ms → 0.23μs（11,800x）**

### 关键文件

- `src/native/interpreter/interp_entry/hotpatch_resolve.cpp`
- `src/native/runtime-core/runtime_stubs/interop_stubs.cpp`

---

## 当前 Benchmark 数据

归一化后 23 assembly 有可信基线：
- **接近 Net8** (< 500%)：Collections, Immutable, Runtime.InteropServices, Claims, Principal.Windows, Formatters, ZipFile
- **中等差距** (500-3000%)：Parallel, Asn1, Linq
- **CoreLib 整体**：-138%（3,083 方法，归一化后）
