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

### 2.1 `pin_set` 的特殊严重性（比 `g_stream_state` 更值得先修）

`pin_set` 记录「已被 pin 的对象」，`GcIsPinnedObject()` 被 GC **用来决定是否搬移**：

```cpp
// engine_lifecycle.cpp:665/671/677
pin_set()[obj] = true;        // GcAddPinnedObject
pin_set().erase(obj);         // GcRemovePinnedObject
return pin_set().contains(obj); // GcIsPinnedObject
```

**该表的键自己会因搬移而失配** → `GcIsPinnedObject` 对被搬移的 pinned 对象返回
`false` → **违反 pin 契约**（本该固定不动的对象被搬走）。

这不是「表数据过期」，而是**破坏 GC 自身的正确性判据**。

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

## 4. 关键未知（必须先解决，否则方案无法定）

**⚠️ 当前没有任何已知测试命中该缺陷。** 这意味着：

1. **严重度无法用现成 fact 数据量化** —— 不知道 threading/XML/其他 chunk 里
   是否已有 subject 因此返回错值
2. **修复无法验证** —— 需要一个能稳定触发 Gen1/Gen2 压缩的测试装置

这是本任务**最大的 blocking question**：先造出可复现的触发条件，还是先按
代码证据直接修？

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

### blocking_questions（**未清零**）

| # | 问题 | 状态 |
|:-:|:-----|:-----|
| 1 | 是否先构造可复现触发装置？ | ❌ **未定** —— 影响方案可验证性 |
| 2 | `pin_set` 改用 pinned handle 是否与 GC 自身 pin 机制冲突？ | ❌ **未查** |
| 3 | `g_stream_state` 的 handle 生命周期如何管理（谁释放）？ | ❌ **未定** |
| 4 | 严重度：现网是否有实际命中？ | ❌ **无数据** |
| 5 | 死代码 `g_sync_block_stripes` 删除还是保留？ | ❌ 未定 |

### watch_items
- W-a：`GcRelocateHandles` 对 **strong handle** 是否也重定位（当前只在 LOH/Gen1/Gen2
  压缩路径见到调用，需确认覆盖面）

---

## 8. 下一步入口

**blocking_questions 非空 → 不得进入 writing-plans。**

需继续 brainstorm 清零，重点：
1. 先回答 §4（可复现性），它决定 A 能否被验证
2. 若短期无触发手段 → 是否降级为"仅修 `pin_set`"（严重度最高且证据最硬）

---

## 9. 诚实标注

- **零测试命中** —— 本缺陷目前**只有代码路径证据**，无运行时复现
- `com_rcw` 经核实**不受影响**（键是 COM 指针），初版记录中"三张表"的说法已修正
- 证据 B 的 `RelocateGen1References` 注释是**代码作者自述**，非我实测；
  但搬移代码（证据 A）与键语义（证据 D）都是我直接读到的
