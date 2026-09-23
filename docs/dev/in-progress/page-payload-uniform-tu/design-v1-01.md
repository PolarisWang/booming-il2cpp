# design v1-01 — page 0 大载荷的彻底均匀化

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-22
> 上游：`page-payload-split`（已交付，C1002 已消除但 page 0 仍 31 MB）
> 阶段：brainstorm，待用户审查
> 三优先级：P1 性能最优 > P2 架构完美 > P3 热更适配

## 一、根因（已实测坐实）

### 1.1 真正根因：`ObjectModel` 从未被登记为 payload section

`page-payload-split` 的段拆分实现为一系列 `AddSection(...)` 调用，
共登记 **14 个段**。但 **`ObjectModel` 不在其中** —— 它走的是另一条路径：

```csharp
// NativeAotEmitter.Shared.cs:465
includeObjectModel: true,   // 直接内联进 page 0，不参与分段
```

**后果**：`ObjectModel` 是 page 0 的 **96%**，却完全不参与分段。

### 1.2 page 0 实测构成

| 段 | 行数 | 占比 | 在 `PayloadSections` 中 |
|---|---|---|---|
| Late type-id definitions | 13,207 | 0.7% | ❌ |
| **Virtual method table arrays** | **1,855,482** | **96.0%** | ❌ **未登记** |
| GcTypeLayout pointer offset arrays | 3,562 | 0.2% | ❌ |
| Registration calls | 60,474 | 3.1% | ❌ |
| **合计** | **1,932,755** | 100% | — |

`native-aot.generated.cpp` = **30.96 MB / 1,932,755 行**。

### 1.3 C1002 的真实触发条件（重要修正）

| | 行数 | 结果 |
|---|---|---|
| 历史 C1002 触发 | 1,236,192 | ❌ 失败 |
| **当前 page 0** | **1,932,755** | ✅ **通过** |

**当前比历史触发值还大 1.56×，却能编译。**

→ C1002 是**编译期内存峰值**问题，受 `/MP` 并行度、可用内存、MSVC 版本影响，
**不是确定性的文件大小阈值**。这意味着它**随时可能复发**，且不看单一文件大小。


### 1.4 深度调查：VTable 段的真实构成（2026-09-22 实测）

> ⚠️ **本节修正了 1.2 的初测数据。** 初测用 python 读 `
` 文件时把行数算成
> 两倍，得到「1,932,755 行 / 96% 是 Virtual method table arrays」，**该数字错误**。
> 以下为用 `wc -l` 与 `awk` 复核后的**正确数据**。

**page 0 正确行数：972,873 行 / 30.96 MB**

| 段 | 行数 | 占比 |
|---|---|---|
| Late type-id definitions | 13,067 | 1.3% |
| **Virtual method table arrays** | **927,748** | **95.4%** |
| GcTypeLayout pointer offset arrays | 1,781 | 0.2% |
| Registration calls | 30,246 | 3.1% |

**VTable arrays 段（927,748 行）的构成**：

| 形态 | 行数 | 占比 |
|---|---|---|
| **空槽位**（`{ 0u, nullptr }` / `nullptr,`） | **764,018** | **82.4%** |
| 实际方法槽（`reinterpret_cast<void*>(...)`） | 12,770 | 1.4% |
| `extern "C"` 方法声明 | 6,390 | 0.7% |
| 其余（数组外壳、空行等） | 144,570 | 15.6% |

**实/空比 = 1 : 59.8** —— 即每个真实方法槽平均伴随 ~60 个空槽位。

**vtable 数组规模分布**（127 个数组）：

| 项 | 值 |
|---|---|
| 数组数量 | 127 |
| **最大数组** | **11,770 行（≈0.4 MB）** |
| 合计 | 777,296 行 |

> 🔑 **关键结论**：**没有巨型单数组**。最大数组仅 0.4 MB。
> 初测误判的「107 万行单数组」是 python 行为错误导致的假象。

### 1.5 runtime 如何消费 vtable（决定能否稀疏化）

```cpp
// ChaosGeneratedRuntimePrelude.h:140
inline void* chaos_vtable_resolve(const void** vtable, CHAOS_IL2CPP_UINT32 slot) noexcept
{
    if (vtable == nullptr) CHAOS_IL2CPP_FAIL();
    if (vtable[slot] == nullptr) CHAOS_IL2CPP_FAIL();   // 裸数组索引
    return const_cast<void*>(vtable[slot]);
}
```

- `vtable[slot]` 要求 `void*[]` **物理连续**
- 调用方持有 `TypeInfoHot::vtable_array`（8B 指针）+ slot 索引
- 空槽位触发 `CHAOS_IL2CPP_FAIL()` → 空槽位是「未实现」语义，非合法空值

→ **稀疏编码会破坏此契约**：需改 `chaos_vtable_resolve` 及所有调用点，
   而 vtable 解析是**虚方法分派热路径** → **直接伤 P1**。

### 1.6 由 1.4/1.5 得出的方案修正

既然**最大数组仅 0.4 MB**，则：

> **只要把 `ObjectModel` 纳入分段 + 按 vtable 数组边界切 TU，
> 全部 TU 自然 ≤ 8 MB —— 无需稀疏编码，也无需 C3 的 ABI 改动。**

| 段 | 现状 | B+C2a 后 |
|---|---|---|
| page 0（ObjectModel） | 30.96 MB | **每 TU ~0.4 MB** ✅ |
| GC Slot Map | 10.42 MB | ~6.8 MB ✅ |
| Hotpatch | 6.68 MB | 6.68 MB ✅ |

**结论**：B + C2a 之后**所有 TU 已 ≤ 8 MB**，C3 的收益依据**归零**。

> 📌 **C3 的保留理由（用户决定）**：作为**架构储备** —— 为未来更大的 chunk
> 预置「ABI 段可切」能力。此决定**不以当前的超标段为依据**，
> 而是以架构一致性为目标。设计中据此如实标注。

## 二、边界拍板（用户已确认）

| # | 裁决 |
|---|---|
| 目标 | **B + C2a + C3 都做**（彻底均匀） |
| 验收 | **每个 TU ≤ 8 MB**（硬阈值） |
| C3 路线 | **路线 1**：扩展现有结构（加 `struct_size` + chunks）|
| C3 依据 | **架构储备**（实测 B+C2a 后已无超标段；用户明确保留 C3 为未来能力）|
| P1/P2 冲突 | **用 O(1) chunk 定位避开性能损失**（不让位） |

## 三、方案

### 3.1 B —— ObjectModel 纳入分段

**做法**：把 `ObjectModelCode` 按内部结构切分并 `AddSection`，与既有 14 段同构。

**实测可行性**：`ObjectModel` 内部是 **15,983 个重复块**（每块 ~17 行），
即 `chaos_reflection_create_reference_array(...)` 调用序列。

⚠️ **关键约束**：这些块**全部位于同一个函数体内**
（`chaos_reflection_get_parameters_b3`，见 `page-0001.cpp:130971`）。
函数不可跨 TU → **必须按分支组拆成多个函数**（`..._b3_part1/part2/…`），
再由入口分发。

**产出**：page 0 从 31 MB 降到与方法页同量级（~2 MB）。

### 3.2 C2a —— GC Slot Map 消除类型声明重复

**实测冗余**：

| 组成 | 行数 | 占比 |
|---|---|---|
| 匿名 struct 类型声明（机械重复） | **61,506** | **35%** |
| 初始化值 | ~102,510 | 59% |
| includes / 前言 | ~10,551 | 6% |

单 entry 形态：
```cpp
struct {
    CHAOS_IL2CPP_UINT32 entry_total_size;
    const void*         code_address;
    CHAOS_IL2CPP_UINT32 frame_size;
    CHAOS_IL2CPP_UINT32 num_gc_slots;
    CHAOS_IL2CPP_UINT32 slots[6];      // ← 25 种不同维度
} entry0;
```

**做法**：类型声明只写一次（布局数组 + 注释），消除重复。

**收益**：10.42 MB × 0.65 ≈ **6.8 MB** → **达标（≤8MB）**。

> runtime 消费端按**字节偏移**扫描（`reinterpret_cast<const GcSlotMapV0*>(ptr + 12)`），
> 不读 C++ 类型 → 消除类型重复**不影响运行时语义**。

### 3.3 C3 —— ABI 原子段分块（路线 1）

#### 3.3.1 GC Slot Map（代价低）

`CodeRegistrationV0` **已有 `struct_size`** → 可平滑演进：

```c
// 新增字段（不影响既有字段偏移）
const GcSlotMapChunkV0* slot_map_chunks;
uint32_t slot_map_chunk_count;
```

消费端从 `GcRegisterSlotMapsFromSection(begin, end)` 改为遍历 chunks。

#### 3.3.2 Hotpatch（代价高，且无兼容路径）

**约束**（已实测）：
- `HotpatchModuleV0` **无 `struct_size`** → 任何字段变更都是破坏性的
- 生成侧是**位置式初始化**：`{ "name", s_types, count, s_methods, ... }`
- **5 个文件 / 34 处访问**

```c
typedef struct HotpatchModuleV0 {
    uint32_t struct_size;                    // ← 新增（**首位**）
    // ... 现有 9 个字段保持不变 ...
    const HotpatchMethodEntryV0* const* method_entry_chunks;
    uint32_t method_entry_chunk_count;
    // 同理 slot / entry 的 chunks
} HotpatchModuleV0;
```

**分块依据**（实测）：

| 数组 | 行数 | 分块方式 |
|---|---|---|
| `s_hotpatch_methods` | 10,259 | 保持**全局索引空间**（`first_method_index` 是全局下标）|
| `s_hotpatch_slots` | 10,259 | 按 token 二分查找 |
| `s_hotpatch_entries` | 10,259 | 按 slot 索引 |
| `s_hotpatch_types` | 131 | 不分块 |

#### 3.3.3 O(1) chunk 定位（用户拍板）

三种热点访问形态与对策：

| 形态 | 位置 | 对策 |
|---|---|---|
| `method_entries[first_method_index + mi]` | `hotpatch_table.cpp:81,138` | **chunk_base[] 前缀和**，均匀分块时整除 → **O(1)** |
| `entry_table[slot]` | `hotpatch_table.cpp:231` | 同上，slot 连续 → **O(1)** |
| `GetMethodName` 线性扫描 | `hotpatch_table.cpp:210` | 扫描本身不变；先用 O(1) 定位 chunk，再扫描该 chunk |

**净效果**：不引入额外渐进复杂度。

## 四、架构映射

- il_feature: n/a（发射结构，非 IL 翻译路径）
- translation_path: n/a
- planner: `NativeAotLoweringPlanner.Methods.cs`（ObjectModel 分段）、`GcSlotMap.cs`、`ModuleRegistration.cs`
- emitter: `NativeAotEmitter.Shared.cs`（分段下发）
- runtime_support: **`hotpatch_table.cpp` / `hotpatch_table.h` / `chaos/hotpatch_dispatch.h` / `method_table.cpp` / `bootstrap.cpp`**（C3 触及）
- contracts: **`contracts/native/v0/codegen_bridge.h`**（`HotpatchModuleV0` / `CodeRegistrationV0`）
- constraints_followed: 段边界 = ABI 边界；不改符号命名
- new_constraints: `HotpatchModuleV0` 引入 `struct_size` 兼容机制
- consistency_check: C3 需同步 contracts + runtime + codegen 三侧

## 五、三优先级权衡

| 优先级 | B | C2a | C3 |
|---|---|---|---|
| **P1 性能最优** | ✅ 运行期零影响 | ✅ 零影响（纯文本压缩） | ⚠️ 有风险 → **O(1) 定位抵消** |
| **P2 架构完美** | ✅ 补齐遗漏 | ✅ 消除冗余 | ✅ 段边界=ABI边界，所有段可切 |
| **P3 热更适配** | ✅ 不碰 | ✅ 不碰 | ❌ **改 `HotpatchModuleV0`**（hotpatch 核心结构）|

**P1/P2 与 P3 冲突裁决**（用户已确认）：**接受 P3 让位**，并用手法把 P1 损失降到零。

## 六、风险评估

| # | 风险 | 触发 | 后果 | 缓解 |
|---|---|---|---|---|
| **R1** | `struct_size` 放首位破坏所有构造点 | 位置式初始化 | 编译失败/静默错值 | 同步改模板为**指定初始化器**；全仓搜构造点 |
| **R2** | `first_method_index` 全局索引空间被破坏 | 分块时重索引 | **静默解析到错误方法**（最危险） | 保留全局索引空间；负控验证 |
| **R3** | hotpatch 热路径性能退化 | 分块引入间接层 | P1 回退 | **O(1) chunk 定位**（用户拍板） |
| **R4** | ObjectModel 拆分破坏块间引用 | 同一函数内拆函数 | 编译失败或行为变化 | 先测量依赖图；内容守恒断言 |
| **R5** | 8 MB 阈值仍不达标 | 某个块自身过大 | 验收失败 | 分块前先测分布；必要时调粒度 |
| **R6** | 并发会话共用 artifacts 根 | 两会话跑同 chunk | 假数字 | 跑前核对产物时间戳 |

## 七、验证策略

1. **B/C2a**：产物实测每 TU ≤ 8 MB；方法页与 payload 页同量级
2. **C3**：runtime 单测（hotpatch 名字查找 / dispatch）+ fact 回归
3. **负控**：C3 的索引改动必须证明"重索引"会红
4. **端到端**：system / reflection / threading 三 chunk 构建 + fact
5. **对照**：改 vs 不改的构建时间对比（P1 验证）

## 八、问题清零

```
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## 九、非目标

- 不改符号命名
- 不重构与目标无关的发射路径
- 不改 `GcSlotMapSectionEntryHdrV0` 的字节布局（C2a 只消除**文本冗余**）
