# STATUS — 裸托管指针键状态表（W3）

```yaml
task_id: raw-pointer-key-state-tables
phase: completed
lifecycle_status: completed
current_dir: docs/dev/completed/20260920-02-raw-pointer-key-state-tables
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
关键文档:
  - problem-and-options.md
  - plan-v1-01.md
  - notes-execution.md
```

## 完成结论（2026-09-20）

**提交**：`b459a9a50` fix(gc): 快路径漏调 RelocateGen1References + 裸指针键表搬迁维护

### 落地内容（三个缺陷，同一族）

| # | 缺陷 | 修法 |
|:-:|:-----|:-----|
| 1 | **GC 快路径漏调 `RelocateGen1References`**（执行中发现，比原计划更根本） | `gc_gen1.cpp` 三条快路径（Tier1/Tier2/drain）各自收集 moves 并调用 |
| 2 | `pin_set` 键失配 | `GcRelocateHandles` 内就地同步（`GcSetHandleTarget` 有逻辑但零调用者） |
| 3 | `g_stream_state` 键失配 | 新增 `GcRegisterMoveCallback` 通用搬迁通知 + stream 侧注册 |

### 🔴 执行中的重大发现（超出原计划范围）

**缺陷 1 是本次最重要的产出**，且**不在原 W3 范围内** —— 它是建装置时才暴露的：

`gc_gen1.cpp` 的三条快路径在 `memcpy` 搬移对象后**直接 return**，从不调用
`RelocateGen1References`（正常路径 Phase 4e 会调）。后果：对象确实搬到 Gen2
新地址，但**所有** old-gen slot / 静态根 / 线程栈 / GCHandle 仍指向已腾空的
Gen1 地址 —— cross-gen UAF。

**触发门槛极低**：测试里 64 字节对象即命中 `early_exit`。

### 验证

| 测试 | 修复前 | 修复后 |
|:-----|:------:|:------:|
| Sanity（证明搬移发生） | — | ✅ |
| A_PinSetKeySurvivesRelocation | ❌ 红 | ✅ |
| B_StreamStateKeyFollowsObjectAddress | ❌ 红 | ✅ |

回归：`test_gc_events` 7/7、`test_gc_managed_api` 10/10、
`test_gc_gen1` 13/14（1 项**预存在**失败，已用干净树复现排除）。

详见 `notes-execution.md`。

### 诚实标注

- 本缺陷**零 fact 命中** —— 装置是主动构造的复现，不等于现网已发生
- `RelocateGen1References` 的引用清单来自**代码作者注释**，非实测
- `AddPinnedRoot()` 亦零调用者（`pinned_roots_` 恒空）—— **GC 域独立问题，
  不在本次范围**，仅记录
- 执行中两处**装置设计自我修正**（不能用 pinned handle 观测搬移；局部变量被
  栈槽复用覆盖）已记入 `notes-execution.md`

## 原始任务陈述（保留）

修复 native 侧以裸托管对象地址为键的状态表在 GC 搬移后失效的缺陷。

## 边界拍板

**In scope**：`g_stream_state` 与 `pin_set` 两表的修复；死表 `g_sync_block_stripes`
保留并标注；建立可复现验证装置。

**非目标**：不改 GC 搬移逻辑；不改 `RelocateGen1References` 重写清单；
不修 `AddPinnedRoot()` 零调用者（GC 域独立问题）；不涉托管侧。

## 当前结论（第 8 轮 brainstorm 清零后）

### 缺陷（代码证据）

native 状态表以**裸托管对象地址**为键，GC 搬移对象后键失效：

- GC 搬移实证：`gc_old_gen.cpp:1978`（memmove）、`:2589`（CrossPageCompact）、
  `gc_gen1.cpp:713`（Gen1 压缩）
- `RelocateGen1References` 只重写 *old-gen slot / 静态根 / 线程栈 / GCHandle*
  （`gc_gen1.h:127-131` 注释），裸指针键不在清单内

### 影响面（全仓库枚举）

| 表 | 键语义 | 判定 |
|:---|:-------|:-----|
| `g_stream_state`（stream_stubs.cpp:20） | 托管对象地址 | 🔴 受影响 |
| `pin_set`（engine_lifecycle.cpp:137） | 托管对象地址 | 🟡 维护路径未接线 |
| `s_rcw_table`（com_rcw.cpp:18） | COM IUnknown 指针 | ✅ 不受影响 |
| `g_sync_block_stripes`（sync_mutex.cpp:48） | — | ⚪ 保留 + 标注 |

### 🔴 本轮两处重大修正（推翻初版结论）

**修正 1 — `pin_set` 严重度下调**

初版称「`GcIsPinnedObject` 被 GC 用来决定搬移 → 违反 pin 契约」。
核实后：GC 走 `pinned_roots_`/`pinned_compact_skip_`，
`grep GcIsPinnedObject src/native/runtime-core/gc/*.cpp` **零结果** —— 与 GC 搬移无关。

真实机制是**一条从未接线的维护路径**：`GcSetHandleTarget`（:321-329）
**有正确的 pin_set 同步逻辑，但全仓库零调用者**；GC 实际走的
`GcRelocateHandles`（:643-657）**只更新 handle 表，不碰 pin_set**。

**修正 2 — 方案收敛**

`pin_set` 的最优修法**不是改 GCHandle 键**，而是**接线 `GcSetHandleTarget`**
（逻辑已写对，改动最小）。原「统一改 GCHandle」方案已修正。

## 风险评估摘要（修正后）

| 方案 | 风险 | 说明 |
|:-----|:----:|:-----|
| `pin_set`：接线 `GcSetHandleTarget` | 🟢 低 | 维护逻辑已存在且正确，只需让搬移路径调用 |
| `g_stream_state`：键改 GCHandle | 🟡 中 | 复用 `GcRelocateHandles`；无释放逻辑需设计（表只增不减） |
| ~~统一改 GCHandle~~ | — | 已否决：对 pin_set 是过度改动 |
| ~~扩展 GC 重写清单~~ | 🔴 高 | 破坏分层；`pin_set` 在 engine_lifecycle 会成环 |
| ~~防御性检测~~ | 🔴 高 | 只把 UAF 转成静默错误答案 |

## 三优先级权衡结论

P1 性能 > P2 架构 > P3 热更。两目标均为**复用既有机制**（`GcSetHandleTarget` /
`GcRelocateHandles`），无新增分配、无分层破坏。P1/P2 双优，无冲突。

## 可复现性（原最大 blocker，已解）

既有装置 `tests/contracts/native/runtime-core/gc_gen1_test.cpp` 提供 Gen1 搬移原语
（`NurseryAllocate` + 收集 + `objects_promoted` 断言）。可新建 TU 同时 include
`stream_state.h` 构造复现。**建议先建装置再修**。

## 未验证 / 诚实标注

- **零 fact 命中** —— 本缺陷只有代码路径证据 + 可构造的单测装置，无端到端复现
- 初版「三张表受影响」「破坏 GC 正确性判据」两处均为**过度推断，已修正**
- 本轮新发现 `AddPinnedRoot()` 亦零调用者（`pinned_roots_` 恒空）——
  **GC 域独立问题，已明确划出 W3 范围**
- `RelocateGen1References` 的引用清单来自**代码作者注释**，非实测

## 下一步入口

**blocking_questions = []，但 `clearance_confirmed_by_user: false`** ——
需用户确认以下执行顺序后方可进 `writing-plans`：

1. 先建可复现装置（复用 gc_gen1_test 原语）
2. 修 `pin_set`：接线 `GcSetHandleTarget`（最小改动）
3. 修 `g_stream_state`：键改 GCHandle
4. 装置验证 + chunk 回归
