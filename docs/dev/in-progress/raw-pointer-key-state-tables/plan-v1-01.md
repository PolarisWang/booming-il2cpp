# 裸托管指针键状态表修复 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 修复 native 侧以裸托管对象地址为键的状态表在 GC 搬移后失效的缺陷，覆盖 `pin_set`（维护路径未接线）与 `g_stream_state`（键需 GC 感知）。

**架构：** `pin_set` 复用**已存在但从未被调用**的 `GcSetHandleTarget` 维护逻辑，在 `GcRelocateHandles` 的搬移路径上同步；`g_stream_state` 的键由裸指针改为 `GCHandle`，复用 GC 既有的 handle 重定位机制。**两者都不新增搬移判定逻辑**。

**技术栈：** MSVC C++ / chaos_runtime_core / GoogleTest 单测

**架构审核模式：** critical（触及 GC + runtime-core 主线）

**结构告警重点：**
- **重复逻辑**：`GcRelocateHandles` 与 `GcSetHandleTarget` 都遍历/更新 handle 表 → 需判断是否应统一为一个内部辅助函数
- **文件职责**：`engine_lifecycle.cpp` 已同时承载 handle 表、pin 集合、POH、COM apartment（1300+ 行）→ 本次是否应拆分
- **codegen 与 Scriban 分层**：本改动**完全在 native 侧**，不涉及 codegen

**权责图审核主题：**
`GC 搬移时的引用维护 authority` —— 谁有权决定「哪些外部引用需要在搬移时重写」？
预期 proof：新增的 pin_set 同步不改变 GC 搬移决策本身（`pinned_roots_` 路径不动），
仅修复记账表的一致性。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `runtime-core-gc`
- capabilityItem: `gc-relocation-reference-maintenance`
- ownerSubjectId: `n/a`（native 单测，非 subject 级）
- proofRequired: `true`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects: `[tests/contracts/native/runtime-core/ 新增 pin_set/g_stream_state 搬移复现单测]`
- requiredGates: `collector -> registry -> workspace`

**设计文档：** `docs/dev/in-progress/raw-pointer-key-state-tables/problem-and-options.md`

**问题清零来源：** brainstorm-approved STATUS.md（`blocking_questions: []` / `question_clearance: cleared` / 用户确认执行顺序）

**计划来源：** direct-plan

**预期知识沉淀：** `wiki/` GC 域文档（具体路径执行期确认）+ 更新记忆
`raw-pointer-key-state-tables`

**收尾约束：** 执行完成后必须进入「结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交」固定链路。

---

## 0. 前置准备

- [ ] 确认工作树无并行 agent 未提交改动（`git status` 逐文件核对）
  ⚠️ 本仓库有并发会话（见记忆 `concurrent-wip-clobbers-attribution`）
- [ ] 记录 `tests/contracts/native/runtime-core/` 基线测试结果（跑一次现有 GC 单测）
- [ ] 确认 `gc_gen1_test.cpp` 可独立构建运行

---

## 1. 建立可复现装置（🔴 必做，先于修复）

**文件：** 新建 `tests/contracts/native/runtime-core/gc_raw_key_relocation_test.cpp`
（或扩展 `gc_gen1_test.cpp`，执行者按既有 CMake 组织方式择一）

**职责：** 在 GC 搬移真实发生的前提下，断言裸指针键的失效/有效性。

**为什么先做：** 本缺陷**零 fact 命中**，没有装置就无法证明修复有效
（见设计文档 §4.1/§4.4）。

- [ ] 复用 `gc_gen1_test.cpp` 的 Gen1 触发原语：
  ```cpp
  NurseryAllocate(...)   // 造 Gen0 引用持有目标对象
  GcGen1Collect(...)     // 触发收集
  // 断言 objects_promoted >= 1 与对象地址变化
  ```
- [ ] **实验 A（pin_set）**：
  - `GcCreatePinnedHandle(obj)` → `GcIsPinnedObject(obj) == true`
  - 触发 Gen1 搬移使 obj 换址
  - 断言：`GcIsPinnedObject(新地址) == true`（**修复前应失败**）
  - ⚠️ 该实验必须**先跑出红色**，否则说明搬移未真实发生，装置无效
- [ ] **实验 B（g_stream_state）**：
  - `include "runtime_stubs/stream_state.h"`
  - `ChaosStringWriterCtor(obj)` 写入以 obj 为键的状态
  - 触发搬移
  - 断言：新地址可解析到原 state（**修复前应失败**）
- [ ] **记录红色基线**到 `notes/`（证明装置真的能捕获缺陷）

**验证：** 两个实验在修复前**必须失败**；若通过，装置无效，不得继续。

---

## 2. 修复 `pin_set` — 接线 `GcSetHandleTarget`

**文件：** `src/native/runtime-core/core/engine_lifecycle.cpp`

**职责：** 让 GC 搬移路径同步 `pin_set`。

**根因回顾**：`GcSetHandleTarget`（:321-329）**有正确的 pin_set 同步逻辑**，
但**零调用者**；GC 实际走的 `GcRelocateHandles`（:643-657）只更新 handle 表。

- [ ] 在 `GcRelocateHandles` 更新 `kv.second.object_instance` 处，同步维护
      `pin_set`（旧地址 `GcRemovePinnedObject` → 新地址 `GcAddPinnedObject`），
      仅对 `pinned || async_pinned` 的 entry

- [ ] **🔴 死锁警告（实现约束）**：
  `GcRelocateHandles` 持有 `s_gc_handle_shard_mutexes[s]`，而 `GcSetHandleTarget`
  也获取同一把锁 → **不可直接嵌套调用 `GcSetHandleTarget`**。
  必须：
  - 在 `GcRelocateHandles` 内**就地**更新 pin_set（它用的是 `pin_set_mutex()`，独立锁），**或**
  - 抽取一个假定调用方已持锁的内部辅助函数

  ⚠️ 锁顺序必须固定，避免与 `GcSetHandleTarget` 路径产生 ABBA 死锁。

- [ ] 核对 `GcSetHandleTarget` 是否仍有必要保留公有（它现在有测试调用，
      不可删除）

**验证：** 实验 A 转绿。

---

## 3. 修复 `g_stream_state` — 键改 GCHandle

**文件：** `src/native/runtime-core/runtime_stubs/stream_state.h`、
`src/native/runtime-core/runtime_stubs/stream_stubs.cpp`

**职责：** 键由裸 `void*` 改为 GC 感知的 handle。

- [ ] `StreamState` 表键改为 `CHAOS_IL2CPP_UINT64`（GCHandle id）
- [ ] 工厂函数（`ChaosStringWriterCtor` 等）改为
      `GcCreateStrongHandle(instance)` 取 handle 作键
- [ ] `find_state` / `get_or_create_state` 的实参从裸指针改为 handle
- [ ] **无需实现释放**：设计文档 §4.3 已确认 `remove_state()` 零调用者、表只增不减。
      但需在注释中**显式记录**这一取舍（避免后续误以为遗漏）
- [ ] 可选（建议一并做）：清理死代码 `remove_state()`，或接线到 `Close`/`Dispose`

**验证：** 实验 B 转绿。

---

## 4. 验证

- [ ] 两个实验装置**全部通过**（修复前红色 → 修复后绿色）
- [ ] 跑 `tests/contracts/native/runtime-core/` 全量 GC 单测，无回归
- [ ] 跑至少一个 chunk 的 build+fact 确认无副作用
      ⚠️ **threading chunk 当前挂死**（并行 agent 的 `PARK`/`TP` 调试钩子），
      避免选择它；优先 `System.Private.Xml/xml`
- [ ] 产物级确认：无新增编译警告

---

## 5. 收尾

- [ ] 结构告警与架构审视（按计划头部关注点逐条过）
      - 重点判断：`GcRelocateHandles` 与 `GcSetHandleTarget` 是否应合并
      - 重点判断：`engine_lifecycle.cpp` 是否应拆分
- [ ] 权责图审核：确认 GC 搬移决策路径未被改动
- [ ] 测试通过
- [ ] 更新 `STATUS.md`（phase / 结果 / 未验证项诚实标注）
- [ ] 归档到 `docs/dev/completed/`
- [ ] `git diff --cached --name-only` 逐文件核对
- [ ] 提交（commit message 含 `root_cause` / `fix_strategy` / `regression_check`）
- [ ] 知识沉淀：更新记忆 `raw-pointer-key-state-tables`

---

## 6. 风险与回退

| 风险 | 触发信号 | 回退动作 |
|:-----|:---------|:---------|
| **R1 死锁** | 单测挂起 | 检查锁顺序；改用「就地更新 + 独立锁」方案 |
| **R2 装置无效** | 实验修复前就通过 | 说明搬移未发生 → 回到 §1 换触发方式，**不得跳过** |
| **R3 handle 泄漏** | 长时间运行 handle 数增长 | 记录为已知取舍（表本就只增不减）；必要时接线释放 |
| **R4 并发 WIP 污染** | 提交时出现无关文件 | `git reset --soft` 重做，逐文件 add |
| **R5 陈旧产物** | 测试结果与预期不符 | 重建 SDK + 清 cache + 校验时间戳 |

---

## 7. 未验证 / 诚实标注（执行时必须复核）

- **零 fact 命中** —— 本缺陷目前只有代码路径证据，装置是**主动构造**的复现，
  不等于现网已发生
- 设计文档中「`RelocateGen1References` 的引用清单」来自**代码作者注释**，非实测
- `AddPinnedRoot()` 零调用者（`pinned_roots_` 恒空）是 **GC 域独立问题**，
  **不在本计划范围**，仅在收尾时记录
- `g_sync_block_stripes` 保留不删（设计 §4.5）
