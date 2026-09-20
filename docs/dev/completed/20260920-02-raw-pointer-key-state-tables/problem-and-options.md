# 裸托管指针键状态表 — GC 搬移下的失效（W3）

> **日期**: 2026-09-20
> **来源**: ctor 句柄 ABI 修复（`5184a0e59`）收尾时记录的 watch item W3
> **轮次**: 第 7 轮立项

---

## 1. 问题陈述

多张 native 侧状态表以**裸托管对象地址**（`void*`）为键。GC 会在 Gen1 压缩
（CRAG）与 Gen2 压缩（BGC `StwCompact` / `CrossPageCompact`）时**物理搬移对象**，
搬移后这些键全部指向**已回收内存**。

### 1.1 判决性证据（一手，非推断）

**证据 A — GC 确实搬移对象**

| 位置 | 代码 |
|:-----|:-----|
| `gc_old_gen.cpp:1978` | `std::memmove(it->new_addr, it->old_addr, it->size)` |
| `gc_old_gen.cpp:2589` | `std::memcpy(e.new_addr, e.old_addr, e.size)`（CrossPageCompact） |
| `gc_gen1.cpp:713` | `std::memcpy(s_begin, compact_buf, local_bytes_compacted)`（Gen1 压缩） |

**证据 B — GC 只重写它认识的引用形式，漏掉裸指针键**

`gc_gen1.h:127-131` 的 `RelocateGen1References` 注释（原文）：

> *"Rewrite every external reference (**old-gen slot, static root, thread stack,
> surviving-Gen1 interior, GCHandle**) that EXACTLY matches a moved Gen1 object's
> old address to its new address."*

`g_stream_state` / `pin_set` 的键**不在这个清单里** → 搬移后不被重写。
该注释自己称此类为 **cross-gen UAF**（GC-N6 mode3）。

**证据 C — 触发门槛低**

- `gc_config.h:53` `Gen1MinPromotionAge = 1` —— 对象**存活一轮 Gen0 GC 即入 Gen1**
- `gc_gen1.h` `GcGen1ShouldCollect()` = Gen1 占用 >80% **或** 碎片 >50%

即：不需要长生命周期对象，活过一轮 Gen0 再赶上一次 Gen1 压缩即可失效。

**证据 D — 键的确是托管对象地址**

```cpp
// native-aot.page-0015.cpp:5008
chaos_external_runtime_..._StringWriter___ctor_System_Void__(
    reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));   // ← NEW_GC 分配的托管对象
```
```cpp
// stream_stubs.cpp:128
auto& state = get_or_create_state(reinterpret_cast<void*>(instance));
```

---

## 2. 影响面（全仓库枚举，非抽样）

对 `UNORDERED_DENSE_MAP_IDENTITY(void*, ...)` 做了全仓库扫描：

| 表 | 文件 | 键语义 | 判定 |
|:---|:-----|:-------|:-----|
| `g_stream_state` | `runtime_stubs/stream_stubs.cpp:20` | **托管对象地址** | 🔴 受影响 |
| `pin_set` | `core/engine_lifecycle.cpp:137` | **托管对象地址** | 🔴 受影响（见 §2.1） |
| `s_rcw_table` | `com_rcw.cpp:18` | **COM IUnknown 指针** | ✅ 不受影响（非托管对象） |
| `g_sync_block_stripes` | `core/sync_mutex.cpp:48` | — | ⚪ **死代码**（仅声明，无读写） |

### 2.1 `pin_set` 的精确机制（🔴 本轮修正 —— 原文有过度推断）

**原文错误**：曾写「`GcIsPinnedObject` 被 GC 用来决定是否搬移，键失效 → 违反 pin 契约」。

**核实后的真实机制**（证据见下）：

1. GC 的搬移判定走 **`MarkSweepOldGen::pinned_roots_` → `pinned_compact_skip_`**
   （`gc_old_gen.cpp:1901/2293/2534/3146`），通过 `AddPinnedRoot()` 注册
   —— **与 `pin_set` 完全无关**。
   `grep GcIsPinnedObject src/native/runtime-core/gc/*.cpp` → **零结果**。

   → 所以键失效**不会**导致「pinned 对象被搬走」。GC 的搬移正确性不受影响。

2. 但 `pin_set` **确实会因搬移而失效**，机制是：
   - `engine_lifecycle.cpp:321-329` 的 `GcSetHandleTarget` **有正确的维护逻辑**
     （旧地址 `GcRemovePinnedObject` → 新地址 `GcAddPinnedObject`）
   - **但 `GcSetHandleTarget` 全仓库零调用者**（`grep` 仅命中定义与声明）
   - 而 GC 搬移实际走的是 `GcRelocateHandles`（`engine_lifecycle.cpp:643-657`），
     它**只更新 `kv.second.object_instance`**，**不碰 `pin_set`**

   → **一条从未被接线的维护路径**：pin_set 的同步逻辑写对了，但没人调它。

3. **实际后果**：`GcIsPinnedObject(搬移后的对象)` 返回 `false`（键停在旧地址）。
   调用方只有 `gc_events_test.cpp`（测试）；生产路径暂无消费者。
   `GcAllocatePinned`（POH）走的是 `PohAllocate`，其对象本就不参与普通压缩。

**严重度重估**：`pin_set` 从「🔴 破坏 GC 正确性判据」下调为
「🟡 **维护路径未接线的记账缺陷**」—— 仍应修（逻辑已写好，接线成本低），
但不是正确性事故。

> ⚠️ 本轮同时发现 `AddPinnedRoot()` 亦**无调用者**（`pinned_roots_` 恒空）——
> 这属 GC 域独立问题，**不在 W3 范围**，仅记录。

---

## 3. 边界拍板

### 3.1 In scope
- `g_stream_state`、`pin_set` 两表的键改为 GC 感知形式
- 死表 `g_sync_block_stripes` 的处置（删除或标注）
- 建立**可复现的触发/验证手段**（当前无已知测试命中）

### 3.2 Out of scope（明确非目标）
- 不改 GC 搬移逻辑本身
- 不改 `RelocateGen1References` 的重写清单（除非方案选定为"扩展清单"）
- 不涉及托管侧

---

## 4. 关键未知 —— 本轮已解决

### 4.1 可复现性 ✅ 可构造（不再是 blocker）

**既有可复用装置**：`tests/contracts/native/runtime-core/gc_gen1_test.cpp`

它已验证的 Gen1 搬移原语：

```cpp
TestSingleLiveObject():
    NurseryAllocate(...)                // 造 Gen0 引用持有 Gen1 对象
    GcGen1Collect(...)                  // 触发收集
    GC_CHECK(r.objects_promoted == 1)   // ← 对象被搬移
    GC_CHECK(r.bytes_promoted >= 64)    // ← 地址变化
```

**复现方案**：新建测试 TU（或扩展 `gc_gen1_test.cpp`），
同时 include `runtime_stubs/stream_state.h`，构造：

```
1. ChaosStringWriterCtor(obj)        // 向 g_stream_state 写入 obj 地址为键
2. 保持 obj 存活 + NurseryAllocate 引用
3. 触发 Gen1 收集（对象被搬移）
4. 断言 find_state(新地址) == 原 state，或 find_state(旧地址) 已失效
```

⚠️ **限制**：该装置是 native 单测，验证的是「键失效」这一机制；
**不等于**能复现端到端 fact 影响（§4.4 严重度仍需 fact 层数据）。

### 4.2 `pin_set` 与 GC pin 机制的关系 ✅ 已澄清
见 §2.1 —— 两者**无关**。GC 走 `pinned_roots_`；`pin_set` 是独立记账表。
修 `pin_set` 不会与 GC 的 pin 机制打架。

### 4.3 `g_stream_state` 的 handle 生命周期 ✅ 已澄清（反而简化）
- `remove_state()` **定义了但零调用者**（`stream_stubs.cpp:91`）
- `g_stream_state` **无任何外部清理路径**（全仓库 grep 仅命中定义）

→ 该表**只增不减**，键在对象回收后成为悬垂键。
→ **不需要设计释放逻辑**（本来就没有），只需保证搬移后键可解析。

### 4.4 严重度 / 现网命中 ⚠️ 仍无数据
无已知 fact 命中。但 §4.1 的装置可用于**主动构造**验证。
本项从「blocking」降级为「执行期观察项」。

### 4.5 死代码 `g_sync_block_stripes` ✅ 建议保留

`sync_mutex.cpp:42` 注释自述新代码走 `ThinLockTable::Inflate`，旧 stripe
仅 `DrainSyncBlocksForDomain`（域卸载路径）引用。**建议保留 + 标注**，
删除需确认域卸载路径确实不用 —— 超出 W3 范围。

---

## 4bis. 本轮新发现（非 W3 范围，仅记录）

- **`AddPinnedRoot()` 零调用者** → `MarkSweepOldGen::pinned_roots_` 恒空。
  GC 的 pin 判定机制**未被接线**。（GC 域独立问题）
- **`GcSetHandleTarget()` 零调用者** → handle 目标变更时的
  `pin_set` 同步 + SATB 写屏障维护**从未生效**。（本项是 §2.1 的根因，属 W3）
- **`remove_state()` 零调用者**（stream 表）→ 见 §4.3。（属 W3）

---

## 5. 候选方案

### 方案 A — 键改为 `GCHandle`（推荐）

用 `GcCreateStrongHandle` / `GcCreatePinnedHandle` 取代裸指针，表存 handle ID；
GC 的 `GcRelocateHandles`（`gc_old_gen.cpp:2217/2491/3130`、
`gc_gen1_relocate.cpp:198`、`gc_demotion.cpp:260` 均调用）**会自动重定位**
handle 指向的对象。

- **优点**：复用既有成熟机制（handle 重定位是 GC 已实现且被调用的路径）；
  一次性消掉整类缺陷；`pin_set` 可用 pinned handle 表达 pin 语义
- **缺点**：每个 entry 增加 handle 分配/释放开销；需处理 handle 泄漏；
  `pin_set` 用 pinned handle 表达「pin」会与 GC 自身的 pin 机制打架（需确认语义）
- **风险**：🟡 中 —— 机制正确，但需确认 handle 表与 GC 压缩的交互

### 方案 B — 扩展 `RelocateGen1References` 重写清单

让 GC 在搬移时回调这些表，重写键。

- **优点**：不引入 handle 开销
- **缺点**：**反向依赖** —— 要求 GC 域知道每个子系统有哪些表，破坏分层；
  每新增一张表都要改 GC（`pin_set` 本身就在 `engine_lifecycle.cpp`，会成环）
- **风险**：🔴 高（架构上不可接受）

### 方案 C — 不修，加防御性检测

在 `find_state` / `GcIsPinnedObject` 中校验键是否指向有效对象头，失效则丢弃。

- **优点**：改动小
- **缺点**：只把 UAF 转成静默丢数据（`pin_set` 会静默"取消 pin"）；
  **不解决正确性**，只是让崩溃变成错误答案
- **风险**：🔴 高（掩盖问题，不符合 P2）

---

## 6. 三优先级权衡

| 优先级 | 方案 A | 方案 B | 方案 C |
|:-------|:------:|:------:|:------:|
| **P1 性能** | 🟡 每次查表多一次 handle 解引用 | ✅ 无额外开销 | ✅ 无额外开销 |
| **P2 架构** | ✅ 复用 GC 既有机制，分层清晰 | ❌ 破坏分层，GC 反向依赖子系统 | ❌ 掩盖缺陷 |
| **P3 热更** | ✅ 中性 | ✅ 中性 | ✅ 中性 |

**推荐 A**：P1 的降级是真实的，但 B 违反 P2 分层（且 `pin_set` 会成环），
C 不解决问题。P1 > P2 的让位在此**不适用** —— 因为 B/C 的问题不在性能，
而在正确性与分层；A 的性能代价可通过复用 `GcAllocateFast` 的 handle 池摊薄。

⚠️ 但 A 的 P1 代价**需要实测**才能接受（§4 的未知）。

---

## 7. 问题清零

### blocking_questions: ✅ 已清零（本轮）

| # | 问题 | 结论 |
|:-:|:-----|:-----|
| 1 | 是否先构造可复现触发装置？ | ✅ **可构造** —— 复用 `gc_gen1_test.cpp` 的 Gen1 原语（§4.1）。建议**先建装置再修**，理由：本缺陷零 fact 命中，没有装置就无法证明修复有效 |
| 2 | `pin_set` 改用 pinned handle 是否与 GC pin 冲突？ | ✅ **不冲突** —— GC 走 `pinned_roots_`，与 `pin_set` 无关（§2.1） |
| 3 | `g_stream_state` handle 生命周期（谁释放） | ✅ **无需设计** —— 该表只增不减，本就无释放（§4.3） |
| 4 | 严重度：现网是否有命中 | ⚠️ **降级为 watch_item** —— 无数据，但 §4.1 装置可主动验证 |
| 5 | 死代码 `g_sync_block_stripes` 处置 | ✅ **保留 + 标注**（§4.5），删除超出范围 |

### 方案收敛（严重度修正后）

`pin_set` 从「正确性事故」下调为「维护路径未接线」后，**两个目标的最优修法不同**：

| 目标 | 推荐修法 | 理由 |
|:-----|:---------|:-----|
| **`pin_set`** | **接线 `GcSetHandleTarget`**（非改键！） | 维护逻辑（:321-329）**已经写对了**，只是没人调。让 `GcRelocateHandles` 走它即可 —— **改动量最小、风险最低** |
| **`g_stream_state`** | 方案 A：键改 GCHandle | 无现成维护路径可利用，必须换键形式 |

⚠️ **这修正了原文档「统一改 GCHandle」的方案** —— `pin_set` 有更便宜的修法。

### watch_items

- W-a：`GcRelocateHandles` 对 strong handle 的重定位覆盖面（已见调用，需确认无遗漏路径）
- W-b：fan-out 到 fact 层的影响（延迟时间/错值），当前无数据
- W-c：`AddPinnedRoot()` 零调用者（GC 域独立问题，不在 W3）
- W-d：`remove_state()` 零调用者 → `g_stream_state` 无界增长（W3 附带，可一并修）

---

## 8. 下一步入口

**blocking_questions = []**（W-c 除外，已明确划出范围）

建议执行顺序（待用户确认）：

1. **先建可复现装置**（§4.1）—— 用 `gc_gen1_test.cpp` 原语 + `stream_state.h`
2. **修 `pin_set`**：接线 `GcSetHandleTarget`（最小改动）
3. **修 `g_stream_state`**：键改 GCHandle（方案 A）
4. 用装置验证两者，再跑相关 chunk 确认无回归

⚠️ **仍不得直接进 writing-plans** —— 需用户确认上述边界与顺序。

---

## 9. 诚实标注

- **零测试命中** —— 本缺陷目前**只有代码路径证据**，无运行时复现
- `com_rcw` 经核实**不受影响**（键是 COM 指针），初版记录中"三张表"的说法已修正
- 证据 B 的 `RelocateGen1References` 注释是**代码作者自述**，非我实测；
  但搬移代码（证据 A）与键语义（证据 D）都是我直接读到的
