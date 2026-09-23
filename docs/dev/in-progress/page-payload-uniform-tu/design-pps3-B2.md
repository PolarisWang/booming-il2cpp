# pps3-B2 设计 — modulereg 数据段拆分

> task_id: `page-payload-uniform-tu` / 子任务 `pps3-B2`
> 创建：2026-09-23
> 上游：`design-pps3-A.md`（分段机制）、pps3-B1（已交付）
> 目标：page-0001 从 8.91 MB 压到 ≤ 8 MB

## 一、B1 交付后的现状

page-0001 = **8.91 MB**（= preamble 1.73 MB + `modulereg` 段 7.62 MB）

`modulereg` 段内的 B3 两个巨型函数已由 **B1** 拆出（13.9 MB 移走）。剩余 7.62 MB 全是**数据数组**：

| 数组 | 大小 | 数量 |
|---|---|---|
| `kRefl_methods_*[]` | **4.06 MB** | 384 |
| `kRefl_params_*[]` | **2.87 MB** | 5,291 |
| `kRefl_props_*` / `kRefl_fields_*` / `kRefl_events_*` | 0.13 MB | 286 |
| `s_type_flags` 等 | ~0 | — |

## 二、Blocker：两组数组是 static 且互相引用

```cpp
static const ReflectionQueryParameterDescriptor kRefl_params_Foo[] = { ... };   // ← static
static const ReflectionQueryMethodDescriptor    kRefl_methods_Bar[] = {
    { token, "...", "...", "...", 1, kRefl_params_Foo, 1u, nullptr, flags },     // ← 引用
};
```

- 两者都是 **`static`（internal linkage）** —— 一旦分到不同 TU，`kRefl_methods_*` 引用的
  `kRefl_params_*` 就没有声明，C2065。
- 分页器**从不拆段**（「一个段整体属于一个 TU」是分页不变式），所以整段 7.62 MB 只能独占一个 TU。

## 三、方案

**按声明类型（declaring type）分组，每组登记为一个独立 payload 段。**

```
kRefl_methods_T1 + kRefl_params_T1.*   → section "reflmembers0"
kRefl_methods_T2 + kRefl_params_T2.*   → section "reflmembers1"
...
```

**关键约束**：一个 `kRefl_params_X` 必须与**引用它的** `kRefl_methods_*` 落在同一个段
（与 pps3-1 的「vtable 数组 + kSlots 表必须同 TU」同构）。分组以 **`kRefl_methods`** 为单位，
把该类型 methods 引用到的所有 params 一并纳入该组。

### 链接性处理

| 符号 | 现状 | 改为 |
|---|---|---|
| `kRefl_methods_*[]` | `static const` | **`extern const`** + `RegisterCrossSectionSymbol` |
| `kRefl_params_*[]` | `static const` | **`extern const`** + `RegisterCrossSectionSymbol` |

声明进共享头（pps3-A 已验证 page 0 与所有 payload TU 都会 include）。

## 四、风险

### 🔴 依赖图（实测，三条耦合，缺一即 C2065/LNK）

```
ChaosRegisterReflectionMembers()          ← 注册函数，留在 modulereg 段
   └─ &kRefl_desc_T                       ← 取【地址】，desc 必须在同一 TU 或已声明
        └─ kRefl_methods_T / props / fields / events   ← desc 按名引用这四个
             └─ kRefl_params_M            ← methods 按名引用 params
```

**「同一 TU」判据**：一个 `kRefl_desc_T` 及其直接引用的 `kRefl_{methods,props,fields,events}_T`，
加上这些 methods 引用到的所有 `kRefl_params_M`，构成一个**不可分割的组**。

| # | 风险 | 对策 |
|---|---|---|
| **B2-R1** | 分组漏项 → `kRefl_params_M` 与其引用方分居两 TU | 以 `kRefl_desc_T` 为**分组单位**，闭包式纳入所有被引用符号；守卫断言「每个 params 符号与其所有引用方同段」 |
| **B2-R2** | 数据顺序改变 → 运行时按索引读取错位 | 🔴 **不动数组内部顺序、不动组间相对顺序**；仅换承载 TU |
| **B2-R3** | `ChaosRegisterReflectionMembers` 取 desc 地址，desc 若移出则该 TU 需要声明 | desc 走 `extern const` + 共享头声明（与 methods/params 同样处理）|
| **B2-R4** | 某单组 > 8 MB | 实测最大组；超标则组内再切（须保证 params 与其 methods 同组）|
| **B2-R5** | `s_type_flags[128]` 等小数组 | 留在 modulereg 段（体积可忽略）|

## 五、验收

| 项 | 判据 |
|---|---|
| page-0001 ≤ 8 MB | 实测 |
| 构建通过 | system chunk → entry.exe |
| **内容守恒** | `kRefl_methods_*` / `kRefl_params_*` 数组数量与元素总数不变 |
| **fact 无回归** | reflection chunk fact 与基线逐 subject 比对 |
| 守卫 | 新增「params 与其引用方同段」断言 |

## 六、问题清零

```
blocking_questions: []
question_clearance: cleared
```
