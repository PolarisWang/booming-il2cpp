# design — hotpatch 分块覆盖全部三个数组

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-24
> 上游：C3-3（只切了 methods/types）—— 本设计修正其**覆盖面缺口**
> 性质：**跨 codegen + ABI 的重构**，有实质设计工作量

## 一、实测根因（读产物，非推断）

`hotpatch` 段 **5,251,512 字符**，但 C3-3 的 chunk 逻辑**只覆盖其中 31%**：

| 数组 | 字符 | 占比 | C3-3 是否切 |
|---|---|---|---|
| `s_hotpatch_methods` + `s_hotpatch_types` | ~1,344,000 | 26% | ✅ |
| `s_hotpatch_slots` | 286,343 | 5% | ❌ |
| **`s_hotpatch_entries`** | **3,624,315** | **69%** | ❌ **根本不在统计里** |

**所以 C3-3 分块在数学上不可能让 hotpatch 达标** —— 即使 methods/types 全切到 350K，
剩下的 entries 仍是 3.6 MB。

> 这正是上一轮"精确估算"未能解决它的原因：我修的是**估算精度**（覆盖 26% 的部分），
> 而瓶颈是**覆盖面**。精确测量 31% 的货物，忘记其余 69%。

## 二、三个数组的索引关系（🔴 实测修正）

> **原设计假设「entries 与 methods 同序、可共用 chunk 边界」—— 实测推翻。**

实测（对真实产物 `page-0093.cpp` 逐条比对 10,257 条）：

| 数组 | 顺序 |
|---|---|
| `s_hotpatch_methods` | 按 **type_name 排序**（grouped 后）|
| `s_hotpatch_entries` | 按 **原始 collection 顺序** |

**逐条一致率：0 / 10257（0%）** —— 完全不同序。

### 🔴 更强的事实：entries 的顺序**有语义，不可重排**

```
tokenSlotList = entries.Select((e, idx) => (Token: e.Token, Slot: (uint)idx))
                       .OrderBy(Token)                    ← slot 值 = entries 的下标
```

runtime 侧：
```
TokenToSlot(token) -> idx
GetDispatchEntry() -> &mod->entry_table[slot]             ← 用 idx 直接索引 entries
```

**即 slots 表把 entries 的下标硬编码进去了** —— entries 重排会让所有 slot 指向错误条目。

### 由此得出的修正设计

```
type_groups[i] = { first_method_index, method_count }
     └──> s_hotpatch_methods[...]        按类型边界切（现有 C3-3 逻辑）

s_hotpatch_entries[]    独立顺序 -> **自己的 chunk 边界**
s_hotpatch_slots[]      独立顺序 -> **自己的 chunk 边界**
```

**三者互不相关** ⇒ 不需要"边界对齐"这个约束 —— 原设计里最复杂的那部分消失了。

### 3.2 slots 独立分块

`slots` 按 token 排序，与 method 索引无关 ⇒ 需要**独立的切分遍历**：

```c
const ChaosAbiChunkV0* slot_chunks;        /* NEW */
uint32_t               slot_chunk_count;   /* NEW */
```

### 3.3 预算重定义

chunk 预算必须覆盖**三个数组之和**，而不是只算 methods/types：

```
groupChars = TypeEntryChars + Σ MethodEntryChars        (已有)
           + Σ EntryChars            (NEW: dispatch 表)
           + Σ SlotChars             (NEW: 按 token 归属)
```

> `slots` 与 type group 不同源，无法简单归属。**决策**：slots 按**自己的顺序**
> 独立分块，其 chunk 列表与 type/method/entry 的边界**不要求对齐**。

## 四、ABI 影响（`HotpatchModuleV0`）

新增 4 个字段（**尾部追加**，老模块靠 `struct_size` 门控仍走扁平路径）：

```c
const ChaosAbiChunkV0* entry_chunks;        /* null = flat */
uint32_t               entry_chunk_count;
const ChaosAbiChunkV0* slot_chunks;         /* null = flat */
uint32_t               slot_chunk_count;
```

**兼容性**：C3-4 已建立 `ModuleHasChunkFields()` 门控 → 老模块不受影响；
**新模块但老 runtime** 的场景已在 C3-4 说明为不可解（老 runtime 静默跳过）。

## 五、runtime 影响

`hotpatch_table.cpp` 的 accessor 已抽象（`HotpatchTypeEntryAt` / `HotpatchMethodEntryAt`）。
本步需补：
- `HotpatchEntryAt(mod, index)` —— dispatch 表
- `HotpatchSlotAt(mod, index)` —— 槽表

并让 3 处消费点改走它们（`entry_table[]` 的直接索引）。

## 六、风险

| # | 风险 | 对策 |
|---|---|---|
| ~~R1~~ | ~~entries 与 methods 同序~~ | **已实测推翻** —— 见 §二；设计据此修正，无需对齐边界 |
| **R3** | slots 乱序 → token 查找失效 | slots 按自身顺序切，不排序、不重排 |
| **R4** | ABI 变更破坏老模块 | `struct_size` 门控（C3-4 已建立）|
| **R5** | 无法端到端验证（C2712）| 单测 + `verify-payload-tu.sh` 编译真实产物 |

## 七、执行顺序（每步可验证）

```
7.1  ✅ 验证 R1 —— **已推翻**（0/10257 同序），设计已按 §二 修正
7.2  ABI: 加 4 字段（contracts）+ 门控（runtime）
7.3  runtime: 加 2 个 accessor + 改消费点
7.4  codegen: chunk 逻辑覆盖 entries + slots（各自独立边界）
7.5  模板: 发射 entry_chunks / slot_chunks
7.6  验证: 单测 + verify-payload-tu 编译真实产物
```

## 八、问题清零

```
blocking_questions: []
question_clearance: cleared
（R1 已实测推翻并据此修正设计；修正后方案比原设计更简单）
```
