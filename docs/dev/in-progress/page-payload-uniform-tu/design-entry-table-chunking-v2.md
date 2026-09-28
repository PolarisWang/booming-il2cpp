# 设计 — entry_table / kMethodTable 统一分块（ABI 演进）

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-28
> 性质：**ABI + runtime + codegen 三域演进**，跨域
> 上游：Workflow `w4i7oews5` 的 26 步计划**已被双重证伪**（verdict FLAWED ×2）；
>       本文是基于证伪结论与新一轮实测的**重设计**
> 状态：**待审**（按约定先出方案，不直接实现）

## 一、为什么重设计

上一轮 Workflow 产出的 26 步计划被两个对抗性评审双双判 **FLAWED**，致命项包括：

| # | 致命缺陷 |
|---|---|
| F1 | 步骤 6 的"单元素视图"让 `ChaosDispatchMethodAllModules` 用 `index 0` 派发**每个** slot；`hotpatch_dispatch.h:108` 是 `thunks[index]` 不是 `thunks[0]` → 全部走错函数，且对 kDefaultArgThunks 越界 |
| F2 | **23 处** harness 调用点 `GetHotpatchEntries(), kAotMethodCount, i` 未转换 |
| F3 | 三个方法体发射点硬编码 `s_hotpatch_entries[N]`，运行在 sectioning **之前**，计划里没有对应的 hoist |
| F4 | `method_table.cpp` 列为必改但**未给替代机制** |
| — | `FindChunk` 是 `O(chunk_count·log₂ chunk_count)` 而非 O(1)（违反 P1 热路径） |
| — | **目标本身不成立**：只做 entry_table 消不掉 dispatch |

本文的设计逐条回应上述缺陷。

## 二、决定性实测（本轮，读产物）

### 2.1 两个"大段"共享同一个索引域

实测 `kMethodTable[i]` 与 `s_hotpatch_entries[i]` **逐条相同 1720/1720**：

```
kt[0] = Chaos_TestFramework_..._Assert_AreEqual_System_Boolean...
he[0] = Chaos_TestFramework_..._Assert_AreEqual_System_Boolean...   (相同)
```

⇒ 它们是**同一符号序列**（`s_hotpatch_entries` 是 `HotpatchEntryV0[]`，
`kMethodTable` 是它的 `direct_ptr` 投影）。**必须共享 chunk 边界**，
否则 `entries[i]` 与 `kMethodTable[i]` 指向不同方法。

**`kDefaultArgThunks[i]` 内联引用 `kMethodTable[i]`**（模板 `DispatchEntryCode:20`），
因此也落在同一网格上。

### 2.2 与 `s_hotpatch_methods` 是**不同**索引域

`s_hotpatch_methods` 按 type_name 排序（grouped），`entries` 按原始 collection 序，
实测一致率 **0/10257**。它已有自己的 `method_chunks`，**与本设计互不干扰**。

### 2.3 实测尺寸

| 段 | 数组 | 字符 |
|---|---|---:|
| `hotpatch`（875,914） | `s_hotpatch_entries[1720]` | **603,879** |
| | `s_hotpatch_methods[1720]` | 217,720（已分块）|
| | `s_hotpatch_slots[1720]` | 47,486 |
| | `s_hotpatch_types[45]` | 5,370 |
| `dispatch`（557,416） | `kMethodTable[1720]` | 254,775 |
| | `kDefaultArgThunks[1720]` | 192,294 |
| | `kSubjectSubjectIds[742]` | 98,248 |
| | `kSubjectSlotMap/ContractMap[742]` | 10,336 |

## 三、设计

### 3.1 一个共享 chunk 网格，覆盖四个数组

因为 2.1，这四个数组共用**同一组切分点**：

```
entry_chunks[] 上的网格 G  ⇒  kMethodTable / kDefaultArgThunks /
                              s_hotpatch_entries  同切于 G
```

**这是本设计最大的简化**：不需要"边界对齐"协商，因为它们本就同源。

### 3.2 ABI（`contracts/native/v0/codegen_bridge.h`）

尾部追加（`struct_size` 仍在首位，老模块靠 `ModuleHasChunkFields` 门控走扁平路径）：

```c
const ChaosAbiChunkV0* entry_chunks;        /* null = flat */
uint32_t               entry_chunk_count;
const ChaosAbiChunkV0* slot_chunks;         /* null = flat; s_hotpatch_slots 自己的网格 */
uint32_t               slot_chunk_count;
```

> `kMethodTable` / `kDefaultArgThunks` **不进 ABI** —— 它们是 codegen 内部产物，
> 消费者是 TPG harness 与热路径，通过**新访问器**取，而非直接索引数组。

### 3.3 runtime（`hotpatch_table.cpp/.h`）

```c
HotpatchEntryV0* HotpatchEntryAt(const HotpatchModuleV0* mod, uint32_t index) noexcept;
const HotpatchSlotEntryV0* HotpatchSlotAt(const HotpatchModuleV0* mod, uint32_t index) noexcept;
```

**必须修 F4**：`method_table.cpp` 的 3 处、`hotpatch_table.cpp:379/389`、
`chaos/hotpatch_dispatch.h:149-155` 全部改走访问器，**并同时把
`entry_table == nullptr` 守卫改为 `entry_table_size == 0`**（chunked 时扁平指针为 null）。

**必须修 O(1)**（证伪项）：`FindChunk` 现在每次二分探测都内层累加前缀和。
改为**在 `RegisterModule` 时预计算前缀和数组**（模块注册是一次性的，
不在热路径），`FindChunk` 退化为对前缀和的二分 → 真 O(log n)，或
缓存 O(1) 直查。

**必须修 F1**：`ChaosDispatchMethodAllModules` 的循环不能传"单元素视图"。
改为**逐 slot 用访问器取 entry**，`thunks` 参数改为**访问器**而非裸数组：

```c
// 旧: ChaosDispatchMethod(entries, count, index, thunks)  → thunks[index]
// 新: ChaosDispatchMethodEntry(entry, index, thunksAt)     → thunksAt(index)
```
即把"按下标取 thunk"从数组算术改为**回调/访问器调用**，从根上消除
"全局下标 vs chunk 内下标"的错位。

### 3.4 codegen

| 子步 | 文件 | 内容 |
|---|---|---|
| 1 | `ModuleRegistration.cs` `BuildHotpatchTable` | chunk 循环的 `groupChars` **计入 entries 文本**（现只算 type+method，所以永远切不到 entries）；产出 `entry_chunks` 模型 |
| 2 | 同 | `s_hotpatch_slots` 按**自身 token 序**独立切（不要求与 entries 对齐） |
| 3 | `Templates/NativeAot.HotpatchTable.cpp.scriban` | 发射 `s_hotpatch_entries_<i>[]` + `s_hotpatch_entry_chunks[]`；chunked 时扁平指针置 null |
| 4 | `Templates/NativeAot.DispatchEntryCode.cpp.scriban` | `kMethodTable` / `kDefaultArgThunks` 按**同一网格**切片：`kMethodTable_<i>[]`，`kDefaultArgThunks_<i>[]` |
| 5 | `Methods.Hotpatch.cs` / `ExceptionEmission.*` / `Methods.Remaining.cs` | **修 F3**：三个硬编码 `s_hotpatch_entries[N]` 的发射点改为经访问器或 chunk-local 引用；需要一个**把 chunk 网格在 method-body 发射前就绪**的 hoist（当前 `_nativeSymbolToDispatchSlot` 在 848 行建，sectioning 在 1259+） |
| 6 | TPG `TestProject.Dispatch.cpp.scriban` / `RuntimeEntry.cpp.scriban` / `CppProjectEmitter.cs` | **修 F2**：23 处 `GetHotpatchEntries(), kAotMethodCount, i` 改走逐 slot 访问器。注意 `CppProjectEmitter.cs:671` 是**字符串后处理不是模板** |

### 3.5 可独立先做、零 ABI 的一项

`kSubjectSubjectIds[742]` = **98,248 字符（17.6%）**，是纯字符串表。

**已核实可独立切**（本轮读消费方）：它按 **`si`（subject index, 0..741）** 索引，
与 method slot `i` 是**不同且互不相关**的域：

```cpp
// TestProject.RuntimeEntry.cpp.scriban:456, 467
const char* _sid = (si < kSubjectEntryCount && ...) ? kSubjectSubjectIds[si] : "";
```

且**只用于诊断输出**（`[SEH-FAULT]` / `[ABORT-FAULT]` 日志行），
不参与任何派发决策。因此可以：

- 加一个 `ChaosSubjectIdAt(uint32_t si)` 访问器（或让 harness 改走该访问器）
- 把 742 条字符串切成多个 chunk

#### 🔴 实测修正：这项**不能让 dispatch 达标**

已实现并跑通完整构建，实测：

| | 字符 |
|---|---:|
| dispatch TU 实测 | 560,799 |
| 其中 `kMethodTable` | 254,775 |
| `kDefaultArgThunks` | 192,294 |
| `kSubjectSubjectIds` | 98,248 |
| slot/contract maps | 10,336 |

**切走 98,248 后 TU 仍剩 ~462,551，是 350,000 预算的 1.32 倍。**
⇒ 3.5 **单独不足以让 dispatch 达标**；唯一出路是把
`kMethodTable` + `kDefaultArgThunks`（447K）一起切小，那需要 §3.1–3.4 的 ABI 工作。

**3.5 的正确定位**：主体工作（6.1–6.8）的**配套项** —— 当那两个大数组被切小、
TU 腾出空间后，这张 98K 的表如果仍整块存在，会再次顶到预算。届时它必须一起切。

#### 实现中踩到并修正的口径错误（值得记录）

第一版把 `PayloadSectionPartitioner.DefaultBudgetChars`（350,000，**整个 TU** 的预算）
当成这张表的切分阈值。该表实测 ~98,248 < 350,000，于是 splitter
"正确但无用"地判定"一个 chunk 装得下"——**一个字都没切**，
构建却通过了（访问器照样发射）。

这与 `ReflectionDispatchPartitioner` 那次是**同一类错误**：
两个**量纲不同**的数被当成同一个量比较。
修正为**显式的表级目标**（`subjectIdTableTargetChars = 64_000`），
并补了守卫 `Split_AtRealSystemChunkSize_ProducesMoreThanOneChunk`
+ 负控 `NegativeControl_PerTuBudgetAsTarget_YieldsOneChunk`
（后者复现该 bug：用 TU 预算当目标 → 恰好 1 个 chunk）。

## 四、验收

1. `cl` 编译 `HotpatchModuleV0` 的 `offsetof` 静态断言（老字段偏移不变）
2. 单测：`Σ chunk.count == 总数`；chunk 内元素序 == 原序；非 chunked 路径逐字节不变
3. **端到端**：system chunk 构建 + `entry.exe` + fact 跑通
4. 实测 `hotpatch` 与 `dispatch` 两个 TU 均 < 350,000
5. 负控：故意错开一个 chunk 边界（让 entries 与 kMethodTable 不共享网格）→ 守卫必须变红

## 五、风险

| # | 风险 | 对策 |
|---|---|---|
| R1 | 热路径退化（新增间接层） | 前缀和预计算 + 访问器内联；profile 验证 |
| R2 | F2 的 23 处 harness 改漏 → 静默错方法 | 全仓 grep 兜底 + 端到端 fact 跑起来才信 |
| R3 | F3 的 hoist 缺失 → 编译期或运行期错位 | 先做 3.4-5 的 hoist，**单独一步可验证** |
| R4 | 老 runtime + 新模块 | `struct_size` 门控（C3-4 已建立）；新模块+老 runtime 不可解（已知） |
| R5 | 与 C2712（KNOWN-ISSUE-1）交织 | 用 `verify-payload-tu.sh` 单 TU 编译绕过 |

## 六、执行顺序（每步可独立验证）

```
6.1  ABI 加 4 字段 + offsetof 静态断言              （编译可验）
6.2  runtime 加 2 访问器 + FindChunk 前缀和化        （单测可验）
6.3  runtime 消费点全改访问器 + F4/F1 修正           （单测 + 编译）
6.4  codegen: chunk 循环计入 entries + slots 独立切   （单测：守恒）
6.5  模板: 发射 entry/slot/ktable/thunks 分块         （渲染后 grep）
6.6  F3 的 hoist（网格在 method-body 发射前就绪）     （编译 + 产物核对）
6.7  F2 的 23 处 harness 转换                         （全仓 grep + 端到端）
6.8  端到端验证 + 负控
```

## 七、与证伪结论的逐条对应

| 证伪项 | 本设计的处置 |
|---|---|
| F1 单元素视图 → thunks[0] | 3.3：改为 `thunksAt(index)` 访问器，取消"视图"概念 |
| F2 23 处未转换 | 3.4-6：列为独立步骤 6.7，含 `CppProjectEmitter.cs` |
| F3 路由不闭合 | 3.4-5 + 6.6：显式列为一步 |
| F4 method_table 无替代 | 3.3：明确给出访问器 |
| O(1) 不成立 | 3.3：前缀和预计算 |
| 目标不成立（只做 entry） | 3.1：共享网格，hotpatch + dispatch **同批**解决 |
| 79 字符常量未实测 | 本设计**不依赖估算常量**：按网格切，尺寸由实测产物核对 |

## 八、问题清零

```
blocking_questions: []
question_clearance: cleared
```

已清零：`kSubjectSubjectIds` 的独立可切性已实测确认（§3.5）。
