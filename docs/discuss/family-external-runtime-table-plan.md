# Full External Runtime Table — Implementation Plan

## 核心理念

把 entry 方法本身变成纯委托：`MethodN() { kChaosExternalRuntimeFnTable[entry_index](); }`。
所有实际逻辑从 entry 方法的 inline IL→C++ 移到 native C++ 函数。Hotupdate 不再路由到 interpreter，而是保留 native 执行（或替换 fn table 指针）。

## 架构变化

```
当前:
  Method0() {
    auto arr1 = NewByteArray(4); auto arr2 = NewByteArray(4);
    chaos_external_runtime_Copy(arr1, arr2, 4);
    if (arr2[0] != 0) chaos_result |= 1;
  }

方案 A:
  Method0() { kChaosExternalRuntimeFnTable[entry_index](); }
  // entry_index = 0 指向 native C++ 函数实现完整测试逻辑

Hotupdate:
  当前: s_hotpatch_entries[0].flags |= kHotpatchActive → InterpreterEntryDirect(method_key)
        → 解释执行 patch 的复杂 IL → CRASH
  
  方案 A: s_hotpatch_entries[0].flags 不变 (keep native)
          fn table 的指针可以在需要时替换
```

## 分步实施

### Step 1: 修复现有的 external runtime stubs（清理 + 补全）

目前 `native-aot.generated.cpp` 中的外部函数：

| Index | 方法 | 当前状态 | 修复 |
|-------|------|---------|------|
| 3 | Array.Clear | `ChaosArrayClear` ✓ | 无需修改 |
| 4 | Copy(3-param) | `(void)args` 空操作 | 改为调用 `ChaosArrayCopy(src, 0, dst, 0, len)` |
| 5 | Copy(5-param) | `ChaosArrayCopy` ✓ | 无需修改 |
| 6 | GetLength | `ChaosArrayGetLength` ✓ | 无需修改 |
| 7 | GetValue | `ChaosArrayGetValue` ✓ | 无需修改 |
| 1 | BinarySearch(2-param) | `ChaosArrayBinarySearch` ✓ | 无需修改 |
| 2 | BinarySearch(4-param) | `ChaosArrayBinarySearchRange` ✓ | 无需修改 |
| 8 | IndexOf | `ChaosArrayIndexOf` ✓ | 已在调用但不走 fn table |
| 9 | LastIndexOf | `ChaosArrayLastIndexOf` ✓ | 同上 |
| 10 | Reverse | `ChaosArrayReverse` ✓ | 同上 |
| 11 | Sort(1-param) | `ChaosArraySort` ✓ | 已在调用 |
| 12 | Sort(IComparer) | `(void)args` 空操作 | 改为转发到 `ChaosArraySort(array)` |
| — | Array.Resize | Method3 = `return;` 空的生成代码 | 由 issue owner 跟踪 |

修复位置：`RuntimeHelperShapeRegistry.cs` 中 Copy(3-param) 和 Sort(IComparer) 的 Shape 定义。

### Step 2: 将 entry 方法改为 thin delegation 模式

目前每个 MethodN() 生成完整的 IL→C++ inline 代码（创建数组、调用方法、验证结果）。改为：

```cpp
// 生成方式
void ArrayIndexingCopyNativeEntry_ArrayIndexingCopyNativeEntry_Method0() {
    kChaosExternalRuntimeFnTable[FACT_BASE + 0]();
}
```

每个 entry 方法只有一条指令：通过 fn table 索引调用 native C++ 函数。
- `FACT_BASE` = 13（entry 方法数），与 `kChaosExternalRuntimeSubjects` 的索引空间分离
- 对 `_exitCode` 的访问也在 stub 内部完成

这样带来的好处：
1. **Hotupdate 模式不再需要 interpreter** — `RunNativeAotAll()` 检测到 `Method0()` 是纯委托，不需要 patch
2. **Interpreter 可以处理** — `Call` 单指令，interpreter 能通过 `ResolveDirectFn` 安全解析到 native C++
3. **性能零损失** — 一次间接调用 vs 现在的方法体执行，差异可忽略

### Step 3: Hotupdate dispatch 逻辑变更

修改 `RunNativeAotAll()` 和 `RunNativeAot()` 的逻辑：

```cpp
// 方案：对于 external-routable 的 entry，hotupdate 不切换到 interpreter
RunNativeAotAll() {
    for (int i = 0; i < kAotMethodCount; i++) {
        auto& entry = s_hotpatch_entries[i];
        if (entry.flags & kHotpatchActive && !IsExternalRoutable(i)) {
            // Only non-external-routable methods go to interpreter
            InterpreterEntryDirect(entry.method_key, args, ret);
        } else {
            // External-routable methods ALWAYS run native
            reinterpret_cast<void(*)()>(entry.direct_ptr)();
        }
    }
}
```

`IsExternalRoutable(i)` 由 codegen 在构建时决定——代码生成器在生成 `s_hotpatch_entries` 时标注哪些 entry 可安全使用 native 路径。

或者更彻底：**entry 方法的所有方法（不仅是 external-routable 的）都走 thin delegation**。这样 interpreter 路径永远只需要处理 `Call` 指令，然后 delegate 到 native C++。

### Step 4: 重写 entry 方法生成逻辑

修改 `NativeAotLoweringPlanner`（`Emission/` 下的 codegen emitter）：
- 检测方法是否可以优化为 thin delegation（所有方法都可以，但需要 C++ 端有对应的 native stub）
- 对每个 entry 方法，生成 `kChaosExternalRuntimeFnTable[base + N]()` 而非内联 IL→C++
- 自动注册 `FN_BASE + N` 对应的 native 函数

**注意：** 这是 codegen 层改动，不是 scala/pipeline 改动。不会影响现有 pipeline。

### Step 5: 为验证 stub 实现 native C++ 函数

新的 C++ 函数（在 `_gen_external_runtime_stubs.py` 或手动生成）：

```cpp
// 每个 entry 方法对应的完整测试逻辑
// 输入：由 entry 方法负责 setup（创建测试数据）
// 输出：_exitCode 位标记

// 对 Method0: Copy(3-param)
static void chaos_entry_0() {
    // 创建测试数组 + 调用 + 验证
    auto src = ChaosArrayCreateInstance(typeof(byte), 4);
    auto dst = ChaosArrayCreateInstance(typeof(byte), 4);
    ChaosArrayCopy(src, 0, dst, 0, 4);
    // verify
    if (ChaosArrayGetValue(dst, 0) != 0) {
        extern CHAOS_IL2CPP_INT32 chaos_static_..._exitCode;
        chaos_static_..._exitCode |= (1 << 0);
    }
}
```

这和目前 Method0 的 inline C++ 代码几乎一样，只是不再通过 codegen 生成，而是作为有名字的 C 函数存在。

### Step 6: 构建系统修复

重新编译 `chaos_runtime_core.lib` 时修复 RuntimeLibrary 不匹配问题。有两种方式：
- **方式 A (推荐):** 修改 `CMakeLists.txt` 统一 `/MD` 
- **方式 B (hack):** 手动编译 `interpreter_entry.cpp` 时加 `/MD` 参数

选择方式 A，系统性的修复，不会在生产中泄漏。

## 影响范围

| 组件 | 改动量 | 说明 |
|------|--------|------|
| RuntimeHelperShapeRegistry.cs | 小 | 修复 Copy 3-param / Sort(IComparer) 空 stub |
| NativeAotLoweringPlanner (Emission) | 中 | 新增 thin delegation 代码生成策略 |
| _gen_external_runtime_stubs.py | 小 | 支持 entry 方法的 stub 生成 |
| array_stubs.cpp | 中 | 新增 entry-level native 函数(~13个) |
| interpreter_entry.cpp / hotupdate 逻辑 | 小 | 修改 dispatch 判断逻辑 |
| cmake 构建配置 | 小 | 统一 /MD 问题 |
| 其他 family | 无 | 不改变已有 family 行为 |

## 优缺点

| 方面 | 评分 | 说明 |
|------|------|------|
| 性能 (P1) | 最优 | 始终 native C++，零 interpreter 开销 |
| 架构完整性 (P2) | 良好 | 统一 dispatch，消除 interpreter dual-path |
| Hotupdate (P3) | 有限 | 只支持函数指针替换，不支持任意 IL patch |
| 实施工作量 | 中 | codegen + 运行时 + stub 三处改动 |
| 对其他 family 的影响 | 无 | 增量实施，只影响迁移的 family |