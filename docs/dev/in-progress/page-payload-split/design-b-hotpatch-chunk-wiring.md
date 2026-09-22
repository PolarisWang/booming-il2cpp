# 设计 B — hotpatch 段分段接线（pps-3 收口）

> 日期：2026-09-21
> 上游：`roadmap-v1-01.md` §Phase 2、`design-a-payload-split.md`
> 状态：**待用户批准**（批准后才实现）
> 触发：`PagePayloadSplitTests.PageZeroPayload_StaysWithinTuBudget` 红

## 一、问题

pps-3b 已把 page-0 载荷按**段边界**切成命名 section 并按预算分页，但
`PayloadSectionPartitioner` **永不切分单个 section**（设计如此：段内表↔描述符有
引用关系）。`hotpatch` 段整体 2,302,268 字符 → 独占一个超大 TU → 守卫红：

```
a payload translation unit is 2,302,268 chars, over the 1,400,000 limit
(budget 350,000 x 4). Sections in it: hotpatch(2,302,268)
```

pps-3c 已交付 `PlanHotpatchMethodChunks` 计划器（8 条单测 + 负控），**但未接线**。
本文给出接线设计。

## 二、🔴 关键发现：计划器目前**不足以解决**问题

实测 real chunk 产物（`tmp_verify/codegen/generated/native-aot.generated.cpp`）
hotpatch 段内部构成：

| 数组 | 字节 | 元素数 | 计划器是否切 |
|---|---|---|---|
| `s_hotpatch_methods` | 238,433 | 1,855 | ✅ 切 |
| `s_hotpatch_types` | 5,189 | 45 | ❌ 不切 |
| `s_hotpatch_slots` | 50,869 | 1,855 | ❌ 不切 |
| **`s_hotpatch_entries`** | **574,361** | 1,855 | ❌ **不切** |
| 合计 | **868,852** | | |

**只有 `s_hotpatch_methods`（238KB）被切，占该段 27%。** 即使切成 1 块，
`s_hotpatch_entries`（574KB 单数组）仍远超 350KB 预算。

而测试失败的 2,302,268 是**合成 fixture**（9,000 方法）尺度，与 real chunk 的
868,852 不同源 —— 但**结论一致**：只切 methods 解决不了。

**因此：不能只接线 pps-3c 的现有计划器。** 需要按「段内可切分数组」重新设计。

## 三、🔴 硬约束：`s_hotpatch_entries` 不可切分（连续性契约）

runtime `src/native/runtime-core/hotpatch_table.cpp` 对这几张表的访问形态：

```cpp
// :81  索引访问 —— 依赖连续数组
mod->method_entries[type_entry.first_method_index + mi]

// :138 指针算术 —— 依赖连续数组
const HotpatchMethodEntryV0* method_base =
    mod->method_entries + type_entry->first_method_index;

// :210 线性扫描全表 —— 依赖连续数组
for (uint32_t i = 0; i < mod->method_entry_count; ++i)
    if (mod->method_entries[i].method_token == method_token) ...
```

契约见 `contracts/native/v0/codegen_bridge.h`：
```c
typedef struct HotpatchModuleV0 {
    const HotpatchMethodEntryV0* method_entries;   // ← 单指针 + 计数
    uint32_t                     method_entry_count;
    HotpatchEntryV0*             entry_table;      // ← 单指针 + 计数
    uint32_t                     entry_table_size;
    ...
} HotpatchModuleV0;
```

**`HotpatchModuleV0` 是单指针 + 计数的 C ABI 结构。** 切成 N 块后 runtime
无法寻址第 2..N 块。要支持分块必须**改 ABI + 改 runtime**，这**违反 roadmap
§三非目标**（"不改 runtime 任何代码、不改 ABI"）。

### 3.1 这不是死路 —— 有两条可行路径

| 路径 | 做法 | 代价 |
|---|---|---|
| **B1：换分页单位** | hotpatch 段**不再作为单个 section**；把 `s_hotpatch_entries` 的**元素**按预算切进多个 section，但**每个 section 仍是完整数组**，用**间接表**串起来 | 需改 `HotpatchModuleV0` → ❌ 同 ABI 问题 |
| **B2：承认 hotpatch 段整体作为一个 TU，但把它降到预算内** | 不切数组，而是**减少该段的字符量** | 需查为何 1,855 方法产生 868KB |

> B2 更值得先查：**1,855 个方法产生 574KB 的 entries 数组 = 每方法 ~310 字节**，
> 而 `HotpatchEntryV0` 结构本身只是 4 个指针/整数（~32 字节）。**每条目 ~310 字节
> 意味着大量是符号名字面量**。若能缩短符号名表示，量级可降。

## 四、建议方案：B3（务实，不改 ABI）

**把 hotpatch 段的两个大数组改为「多块 + 编译期拼接」，保持对 runtime 呈现为连续内存。**

技术要点：`s_hotpatch_entries` 的 1,855 个元素分散到 N 个 `.cpp`，
但在**一个 TU 内**通过 `#include` 拼成单个数组定义。即：

```
native-aot.hotpatch.page-0001.cpp   →  定义 entries 的第 0..K 块（纯元素文本，无数组壳）
native-aot.hotpatch.page-0002.cpp
...
native-aot.generated.cpp            →  #include 各块，组成一个完整数组：
                                        extern "C" HotpatchEntryV0 s_hotpatch_entries[] = {
                                        #include "…part1.inc"
                                        #include "…part2.inc"
                                        };
```

**但 MSVC 的 C1002 是单 TU 堆耗尽**，`#include` 拼接后**仍是同一个 TU** → 不解决问题。

→ **B3 无效。** 记录在此以免重复探索。

## 五、结论：需要用户裁决

实测与 ABI 分析表明，hotpatch 段是**真正的无界载荷**，且**受 ABI 约束无法分块**：

1. 段内 4 个数组里最大的 `s_hotpatch_entries`（574KB）**不可切**（连续性契约）
2. 只切 `s_hotpatch_methods`（238KB，27%）**不足以**降到 350KB 预算
3. 要真正分块 → 必须**改 `HotpatchModuleV0` ABI + 改 runtime 寻址逻辑**
   → **违反 roadmap §三非目标**

**三个可选方向（需用户拍板）：**

| 选项 | 内容 | 影响 |
|---|---|---|
| **① 放宽预算** | 承认 hotpatch 段整体作为**一个豁免 TU**，把守卫改为「豁免清单内的段允许超预算」 | 最小改动；但 C1002 风险仍在（real chunk 的 hotpatch 段若达 ~870KB 单 TU，MSVC 通常可编译；71MB 才是问题） |
| **② 改 ABI 支持分块** | `HotpatchModuleV0` 增加 `method_entry_chunks[]` 数组 | 触及 runtime + ABI，需重开 roadmap §三非目标 |
| **③ 先量化再定** | 实测 real chunk 拆分后**每个 TU 的真实字节**，看 hotpatch 那个 TU 是否真的还会 C1002 | 需跑一次完整 pipeline（~10 分钟） |

**我的建议：③ → ①**。理由：设计目标是「消除 71MB 无界 TU」，而不是「每个 TU 都
≤350KB」。hotpatch 段受 ABI 约束天然是个不可分单元，**若它单独作为一个 TU 能编译
通过，就达成了真正的目标**（C1002 根治）。当前的守卫用的是合成 fixture 的
2.3M 尺度，**夸大了真实风险** —— real chunk 该段仅 868KB。

→ 建议先把守卫改为**对不可分段的处理策略**，再用 real chunk 实测确认。

## 六、待用户确认的开放问题

```
Q1: hotpatch 段是否接受「作为单个不可分 TU」（选项 ①）？
Q2: 若不接受，是否授权改 HotpatchModuleV0 ABI（选项 ②，需重开非目标）？
Q3: 是否先跑 real chunk 全量实测（选项 ③）来量化？
```

**blocking_questions: 3（未清零）**
**clearance_confirmed_by_user: pending**
