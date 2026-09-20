# STATUS — 裸托管指针键状态表（W3）

```yaml
task_id: raw-pointer-key-state-tables
phase: brainstorming
lifecycle_status: in-progress
current_dir: docs/dev/in-progress/raw-pointer-key-state-tables
blocking_questions:
  - 是否先构造可复现触发装置（决定方案可验证性）
  - pin_set 改用 pinned handle 是否与 GC 自身 pin 机制冲突
  - g_stream_state 的 handle 生命周期（谁释放）
  - 严重度：现网是否有实际命中（当前无数据）
  - 死代码 g_sync_block_stripes 删除还是保留
question_clearance: NOT-cleared
clearance_confirmed_by_user: false
关键文档:
  - problem-and-options.md
```

## 边界拍板

**In scope**：`g_stream_state` 与 `pin_set` 两表的键改为 GC 感知形式；
死表 `g_sync_block_stripes` 的处置；建立可复现的触发/验证手段。

**非目标**：不改 GC 搬移逻辑；不改 `RelocateGen1References` 重写清单（除非
选定方案 B）；不涉及托管侧。

## 当前结论

### 缺陷（代码路径证据，非推断）

native 侧多张状态表以**裸托管对象地址**为键，而 GC 在 Gen1（CRAG）/ Gen2（BGC
`StwCompact`）压缩时**物理搬移对象**：

- `gc_old_gen.cpp:1978` `std::memmove(it->new_addr, it->old_addr, it->size)`
- `gc_old_gen.cpp:2589` `std::memcpy(e.new_addr, e.old_addr, e.size)`
- `gc_gen1.cpp:713` `std::memcpy(s_begin, compact_buf, local_bytes_compacted)`

而 `gc_gen1.h:127-131` 的 `RelocateGen1References` **只重写它认识的引用形式**
（old-gen slot / 静态根 / 线程栈 / GCHandle）—— 裸指针键**不在清单内**，搬移后
指向已回收内存（该注释自称 cross-gen UAF，GC-N6 mode3）。

**触发门槛低**：`Gen1MinPromotionAge=1`（存活一轮 Gen0 即入 Gen1）+
`GcGen1ShouldCollect()`（Gen1 占用 >80% 或碎片 >50%）。

### 影响面（全仓库枚举）

| 表 | 键语义 | 判定 |
|:---|:-------|:-----|
| `g_stream_state`（stream_stubs.cpp:20） | 托管对象地址 | 🔴 受影响 |
| `pin_set`（engine_lifecycle.cpp:137） | 托管对象地址 | 🔴 受影响（**最严重**） |
| `s_rcw_table`（com_rcw.cpp:18） | COM IUnknown 指针 | ✅ 不受影响 |
| `g_sync_block_stripes`（sync_mutex.cpp:48） | — | ⚪ 死代码 |

**`pin_set` 最严重**：`GcIsPinnedObject()` 被 GC 用来决定是否搬移，而该表的键
自己会因搬移失配 → 对被搬移的 pinned 对象返回 `false` → **违反 pin 契约**，
破坏 GC 自身正确性判据。

## 风险评估摘要

| 方案 | 风险 | 说明 |
|:-----|:----:|:-----|
| A 键改 GCHandle | 🟡 中 | 复用 `GcRelocateHandles` 既有机制；需确认 handle 与压缩的交互、生命周期 |
| B 扩展 GC 重写清单 | 🔴 高 | 破坏分层；`pin_set` 在 engine_lifecycle 会成环 |
| C 防御性检测 | 🔴 高 | 只把 UAF 转成静默错误答案，不解决正确性 |

## 三优先级权衡结论

P1 性能 > P2 架构 > P3 热更。推荐 **方案 A**：B 违反 P2 分层、C 不解决正确性，
而 A 的性能代价（每 entry 一次 handle 分配/解引用）可通过 handle 池摊薄。
⚠️ A 的 P1 代价需**实测**才能接受。

## 🔴 最大未知（blocking）

**当前零测试命中** —— 本缺陷只有代码路径证据，**无运行时复现**。这导致：
1. 严重度无法用现成 fact 数据量化
2. 修复无法验证（需要能稳定触发 Gen1/Gen2 压缩的装置）

## 未验证 / 诚实标注

- 证据中的 `RelocateGen1References` 注释是**代码作者自述**，非我实测；
  但搬移代码与键语义（`StringWriter___ctor(reinterpret_cast<INTPTR>(chaos_object))`）
  是我直接读到的
- 初版记录曾称"三张表受影响"，经核实 `com_rcw` 键是 COM 指针 —— **已修正为两张**

## 下一步入口

**blocking_questions 非空 → 不得进入 writing-plans。**
需继续 brainstorm，优先回答「可复现性」；若短期无触发手段，考虑降级为
**仅修 `pin_set`**（严重度最高、证据最硬）。
