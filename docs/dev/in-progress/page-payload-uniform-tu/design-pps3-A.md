# pps3-A 设计 — 巨型反射 dispatch 函数拆分

> task_id: `page-payload-uniform-tu` / 子任务 `pps3-A`
> 创建：2026-09-22
> 上游：`design-v1-02-giant-dispatch-split.md`（本文件是其**可执行化**）
> 目标：page 0 的两个巨型反射函数拆为分包，使 page 0 ≤ 8 MB

## 一、实测基线（字节级，非行数估算）

当前 page 0 = **14.12 MB / 188,799 行**：

| 区域 | 字节 | 行 | 占比 |
|---|---|---|---|
| 前导（2,557 type-id + 10,342 条 `extern "C"` 声明 + 杂项） | 5.66 MB | 44,848 | 40% |
| **`chaos_reflection_get_parameters_managed`** | **4.35 MB** | 62,546 | **31%** |
| `chaos_reflection_resolve_method_handle` | **1.97 MB** | 47,519 | **14%** |
| 尾部（`ChaosRegisterGcLayouts` 等） | 2.20 MB | 31,988 | 16% |

**结论**：拆两个函数 → 6.32 MB 离开 page 0 → page 0 ≈ **8.4 MB**。
**仍超 8 MB**，但已从 3.9× 降到 1.05×。本子任务**不承诺独立达标**（与 design-v1-02 的 R8 同构）。

## 二、两个函数的 shape

### 2.1 `get_parameters_managed`（4.35 MB / 10,258 分支）

```cpp
extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_parameters_managed(CHAOS_IL2CPP_INTPTR chaos_method_value) noexcept
{
    if (chaos_method_value == 0) { return ...empty array...; }
    auto* chaos_header = ...;
    CHAOS_IL2CPP_INTPTR chaos_method_handle = 0;
    if (MethodInfo)      { chaos_method_handle = ...; }
    else if (ConstructorInfo) { chaos_method_handle = ...; }
    // ↓ 10,258 条自包含分支
    if (chaos_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>(NNN))
    {
        const auto chaos_array_value = chaos_reflection_create_reference_array(...);
        auto* chaos_array = ...;
        auto* chaos_parameter_0 = CHAOS_IL2CPP_NEW_GC(...);   // 逐参数
        ...
        return chaos_array_value;                              // ← 以 return 结尾
    }
    // (重复 10,258 次)
    { // b3 回退
        const auto chaos_b3_params = chaos_reflection_get_parameters_b3(chaos_method_handle);
        if (chaos_b3_params != 0) { return chaos_b3_params; }
    }
    return ...empty array...;
}
```

**关键性质**：
- 前缀（`chaos_method_value==0` / 解析 `chaos_method_handle`）必须在**主函数**里，产出 `chaos_method_handle`
- 后继（b3 回退 + 空数组兜底）必须**恰好执行一次**，也只能在主函数里
- 每条分支：只读 `chaos_method_handle`、以 `return` 结尾、**无跨分支状态** → **分组完全自由**
- 顺序无关（10,258 个字面量互不相同，无重复键）

### 2.2 `resolve_method_handle`（1.97 MB / 9,438 STRCMP / 89 case）

```cpp
CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
    if (chaos_method_name == nullptr) { return 0; }
    switch (chaos_type_handle)
    {
        case T1:
            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "AreEqual") == 0) { return tok1; }
            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, "AreEqual") == 0) { return tok2; }  // ← 同条件不同值
            ...
            break;
        case T2: ...
        default: break;
    }
    return 0;
}
```

**关键性质**：
- 是 `switch`，不是扁平 if 链 → 分包必须**以 case 为最小单元**（见 R2）
- 🔴 **同一 case 内实测有 16~20 个重复 `"AreEqual"`**，语义 = **取第一个命中**
- `chaos_method_name == nullptr` 短路在 switch 之前 → 留在主函数

## 三、拆分方案

### 3.1 模式：主壳 + 分包

```
chaos_reflection_get_parameters_managed_part0(...)   // 分支 [0, k)
chaos_reflection_get_parameters_managed_part1(...)   // 分支 [k, 2k)
...
chaos_reflection_get_parameters_managed(...)         // 主：前缀 + 依序调用 part0..partN + 兜底
```

主函数前缀产出 `chaos_method_handle`，比对该 handle 落在哪个分包的**位置区间**，
命中即调用；未命中继续下一个 part；全 miss 走原兜底。
**由位置区间决定归属，而非运行时比对**——分包内仍是原来的 OR 链。

⚠️ 但这会**把 10,258 次比较变成 O(N/包数)**：每个分包要重扫自己那 ~1000 条。
若包数 = 16，最坏 16 次函数调用、每次 ~1000 比较 —— **与原来等价量级**，可接受。

**更优（采用）**：不引入位置区间判定，改为**让分包返回值三态**：
分包返回 `chaos_reflection_create_reference_array(...)` 数组值 或 **0 表示未命中**。
主函数：
```cpp
auto chaos_r = chaos_reflection_get_parameters_managed_part0(chaos_method_handle);
if (chaos_r != 0) return chaos_r;
chaos_r = ..._part1(chaos_method_handle);
...
```
**代价**：原本命中分支直接 `return chaos_array_value`；现在要先返回 0 判定。
由于数组值**永不为 0**（GC 分配），该判别是安全的。
**收益**：分包内不比自身区间，语义与原文**逐条等价**（顺序、重复键都保持）。

### 3.2 `resolve_method_handle` 的分包：以 case 为单元

```cpp
CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle_part0(CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name) noexcept
{
    switch (chaos_type_handle)
    {
        case T1: ...原样整块... break;
        case T2: ...原样整块... break;
        ...
        default: break;   // 未命中 → 落到下一个 part
    }
    return 0;            // 0 = 未命中（原语义 default 也是 break→return 0）
}
```
主函数：`if (name==nullptr) return 0;` + 依序调 part + `return 0;`。

🔴 **R2 保障**：case 块**整体搬运、不重排、不去重、不按名排序**。
同一个 case 内的 16 个 `"AreEqual"` 必须**保持原相对顺序**落在同一个 part 中。

### 3.3 分包粒度

| 函数 | 总分支 | 每包分支 | 包数 | 每包预估 |
|---|---|---|---|---|
| `get_parameters_managed` | 10,258 | 1,500 | 7 | ~0.62 MB |
| `resolve_method_handle` | 9,438 STRCMP / 89 case | 按 case 累积到 ~1,500 STRCMP | ~7 | ~0.28 MB |

按 **STRCMP/分支条数**累积切分（非字节），因为字节与条数强相关且条数可精确计数。
用 `PayloadSectionPartitioner.DefaultBudgetChars` 同量级的**条目预算** `1500`。

## 四、风险与对策

| # | 风险 | 对策 |
|---|---|---|
| **R1** | 分包函数是 internal linkage → 跨 TU 不可见 | **`extern "C"`** + 复用 pps2-4 登记器写共享头 |
| **R2** | 🔴 case 内重复 STRCMP 乱序 → **静默错值** | case 块整体搬运；**负控**：故意颠倒同 case 两条 → reflection fact 必须变红 |
| **R3** | 分包返回值 0 与"真返回 0"混淆 | `get_parameters_managed` 返回的是**数组值**（GC 分配，非 0）；`resolve_method_handle` 原语义 miss 就是 `break`→`return 0`，**语义本就一致** |
| **R4** | 兜底执行次数改变 | `get_parameters_managed` 的 b3 回退 + 空数组**只在主函数末尾执行一次**；分包内不含 |
| **R5** | 命名冲突 | `<原函数名>_partN`；与 pps3-1 的 `vtable<N>` section 名不冲突 |
| **R6** | 拆完仍超 8 MB | 明确**不承诺独立达标**（预计 8.4 MB）；前导段拆分为 **pps3-A2**（见 §六） |

## 五、验收

| 项 | 判据 |
|---|---|
| 主函数符号仍在 | `chaos_reflection_get_parameters_managed` / `resolve_method_handle` 定义存在 |
| 分包符号进共享头 | 每个 `_partN` 在 `.header.h` 有 `extern "C"` 声明 |
| **内容守恒** | Σ 分包分支数 == 原分支数（10,258 / 9,438） |
| 构建通过 | system chunk → entry.exe |
| **fact 无回归** | reflection chunk fact 无新增 failed |
| page 0 下降 | 实测 ≤ 8.5 MB |
| **R2 负控** | 颠倒同 case 内两条 STRCMP → 必须变红 |

## 六、范围修正

原 design-v1-02 §五 说 "pps3-A 可独立达标 page 0 ≤ 8 MB" —— **实测证伪**：
拆完 6.32 MB 后 page 0 ≈ 8.4 MB。前导 5.66 MB（40%）**未在 pps3-A 范围内**。

新增 **pps3-A2**：前导段拆分（10,342 条 `extern "C"` 方法声明 + 2,557 type-id）。
但前导是**声明**而非定义，能否移出 page 0 取决于它们是否被 page 0 之外的 TU 需要 —— 待测。

## 七、问题清零

```
blocking_questions: []
question_clearance: cleared
```
