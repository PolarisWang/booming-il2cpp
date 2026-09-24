# C3 设计 — ABI 原子段分块（路线 1）

> task_id: `page-payload-uniform-tu-phase2-4` / 子任务 `C3-1`
> 创建：2026-09-23
> 上游：`roadmap-phase2-4-c3.md`（用户拍板：**B —— 做 C3**）
> 性质：**破坏性 ABI 变更**，跨 contracts → runtime → codegen 三层

## 一、目标

给受 ABI 约束、**不能靠分段解决**的原子大段提供分块能力：
`HotpatchModuleV0`（hotpatch 名表）与 `CodeRegistrationV0`（方法指针/vtable 描述符）。

**当前收益为零**（实测超标 TU = 0）—— 这是**架构储备**，为未来更大的 chunk 铺路。
用户已明确拍板做（roadmap-v1-01 §二 / design-v1-01 §118-131）。

## 二、兼容机制（复用既有先例）

仓库**已有**这个模式，直接沿用，不发明新机制：

```cpp
// bootstrap.cpp:79-81 —— 现有的能力校验
code_registration->struct_size >= sizeof(CodeRegistrationV0) && ...
```

生成侧初始化：`.struct_size = sizeof(CodeRegistrationV0)`（`NativeAot.CodeRegistration.cpp.scriban:40`）。

**C3 采用同一套**：`struct_size` 置于**首位**（拍板），runtime 用
`struct_size >= sizeof(...)` 判断新字段是否可用；老 runtime 遇到新 codegen
按 size 判定 → **降级到单块路径**，而非错读。

## 三、结构变更

### 3.1 新增 chunk 描述符（通用）

```c
/* One contiguous slice of a logically single array.  `base` points at the
 * slice, `count` is its element count, and `element_size` is included so a
 * consumer can advance without knowing the element type. */
typedef struct ChaosAbiChunkV0 {
    const void* base;
    uint32_t    count;
    uint32_t    element_size;
} ChaosAbiChunkV0;
```

**为什么带 `element_size`**：runtime 的 O(1) 定位要靠步进算偏移，
若只给 count 则每种数组都要硬编码元素大小 —— 加一个字段即可通用化。

### 3.2 `HotpatchModuleV0`

```c
typedef struct HotpatchModuleV0 {
    uint32_t struct_size;          /* NEW —— 必须首位 */
    ... 现有 7 个字段保持不变 ...
    const ChaosAbiChunkV0* type_chunks;    /* NEW —— null = 单块（老行为）*/
    uint32_t               type_chunk_count;   /* NEW */
    const ChaosAbiChunkV0* method_chunks;  /* NEW */
    uint32_t               method_chunk_count; /* NEW */
} HotpatchModuleV0;
```

### 3.3 `CodeRegistrationV0`

已有 `struct_size`（首位）。尾部追加：

```c
    const ChaosAbiChunkV0* method_pointer_chunks;   /* NEW */
    uint32_t               method_pointer_chunk_count; /* NEW */
```

## 四、🔴 风险与对策

| # | 风险 | 等级 | 对策 |
|---|---|---|---|
| **R2** | **全局索引空间被破坏 → 静默错值** | 🔴 最高危 | `HotpatchTypeEntryV0.first_method_index` 是**跨 chunk 的全局索引**。分块后它必须**仍是全局索引**，runtime 的 O(1) 定位按 `chunk_base[]` 前缀和折算到 (chunk, offset)。**负控：故意打乱前缀和必须变红** |
| **R3** | 热路径退化 | 中 | O(1) 定位（前缀和二分/直接算），不做线性扫描 |
| **R1** | 构造点不唯一 | 中 | 全仓核对 `HotpatchModuleV0` 构造点 |
| **R5** | 老 runtime 读新 codegen | 中 | `struct_size` 判定 → 走单块路径 |
| **R6** | 并发 session 共用 artifacts | 中 | 跑前核对时间戳 |

### R2 详解（最危险）

```c
// hotpatch_table.cpp:81 —— 现在是直接数组索引
const auto& method_entry = mod->method_entries[type_entry.first_method_index + mi];
```

`first_method_index` 是**全局**方法下标。分块后 `method_entries` 变成 N 个 chunk：

```c
// 目标：仍用全局下标，O(1) 折算
const HotpatchMethodEntryV0* LookupMethod(const HotpatchModuleV0* mod, uint32_t global_index) {
    // chunk_base[] 前缀和：base[k] = 第 k 个 chunk 的起始全局下标
    // 二分或（count 小的时候）线性找 k，再 chunks[k].base + (global_index - base[k])
}
```

**判据**：分块前后的成员查找结果**逐位相同**。这是 R2 的核心断言。

## 五、阶段与判据

| 阶段 | 内容 | 判据 |
|---|---|---|
| **C3-1** | contracts 加 `struct_size` + `ChaosAbiChunkV0` + 各 chunks 字段 | contracts 编译/单测通过；**全仓构造点核对** |
| **C3-2** | runtime 消费端 chunk-aware，**O(1) 定位** | 热路径无退化；查找结果逐位不变 |
| **C3-3** | codegen 发射分块 | 生成分块与 runtime 契约一致；**每 TU ≤ 8MB 不回归** |
| **C3-4** | 三 chunk 端到端 + **负控** | system/reflection/threading 全通过；打乱索引必须变红 |

## 六、问题清零

```
blocking_questions: []
question_clearance: cleared
```
