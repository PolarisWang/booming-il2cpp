---
name: dev-roadmap
description: 当 brainstorming 判定复杂任务需要多阶段推进或拆分为多个独立子任务时使用
---

# 编写 Roadmap

## 概述

当复杂任务已经完成设计拍板、但仍需要多阶段推进或拆成多个独立子任务时，用 roadmap 把它拆成阶段、边界和可派生子任务。roadmap 是父任务文档，不承担具体长期执行；具体实施通过派生子任务目录完成。roadmap 不是继续澄清 open questions 的地方，只承接已经确认完的问题与设计结论。

roadmap 支持三种调度模式：
- `sequential`：默认串行推进，每次一个子任务
- `parallel-batch`：按批次并行派发到多个独立终端
- `hybrid`：混合模式，部分批次并行、部分串行

**开始时宣布：** "我正在使用 roadmap 技能来创建阶段路线图。"

**父任务恢复原则：**

- 父 roadmap 的 `STATUS.md` 是恢复真相源
- `roadmap-v1-01.md` 提供阶段与子任务结构
- `DISPATCH.md` 提供调度批次划分
- 上游存在 `design-v1-01.md` / `brainstorm-v1-01.md` 时按需读取
- 上游没有额外设计文档时，必须以 `STATUS.md` 中的设计摘要作为权威输入
- 创建 roadmap 前，必须能确认上游已经写明 `blocking_questions: []` 或等价的"问题已清零"结论；若无法确认，返回 `brainstorming`

**子任务生命周期原则：**

- roadmap 派生出的 child task 是独立任务，不是只存在于 roadmap 表格里的状态行
- child task 一旦进入终态，必须把任务目录**物理移出** `docs/dev/in-progress/`
- `completed` -> `docs/dev/completed/`
- `hanging` -> `docs/dev/hanging/`
- `abandoned` -> `docs/dev/abandoned/`
- 父 roadmap 只负责同步 child mapping、摘要和下一步；不能用父任务文案替代 child task 的归档动作
- **绝不允许子任务目录仍留在 `docs/dev/in-progress/`，但 STATUS.md 或 roadmap 表已经写成终态**。修改 STATUS.md 后必须同步执行 `mv` 物理移动目录，不得只改文案。
- **父 roadmap 被取消/废弃时，所有仍在 `in-progress` 的子任务必须同步移入 `abandoned/`**，不允许以"父任务已取消"为理由留下孤儿子任务。

**保存位置：** 当前任务目录下的 `roadmap-v1-01.md`
- 默认路径示例：`docs/dev/in-progress/<task_id>/roadmap-v1-01.md`
- 并行批次调度文件：`docs/dev/in-progress/<task_id>/DISPATCH.md`（仅 parallel-batch / hybrid 模式需要）

## 使用时机

brainstorming 结束后，只要满足以下任一条件，就应进入本技能：

- 需要分 2 个及以上阶段推进
- 会拆成 2 个及以上相对独立的子任务
- 任务会跨多次会话持续推进
- 设计与执行前提已经确认，只是需要父级阶段拆分和子任务编排

## 用户确认门

如果分流结论是 `roadmap`，必须先向用户说明原因并等待确认，才能继续本技能。这里确认的是"已拍板设计需要拆成多阶段/多子任务推进"，而不是把未决问题留到 roadmap 中再慢慢确认。除非用户明确要求 `manual`，这次确认同时视为对子任务推进策略的授权。

当 `dispatch_model` 为 `parallel-batch` 或 `hybrid` 时，必须在用户确认门中说明：并行派发需要用户提供多个终端（Claude Code 实例）来执行，父 roadmap 只负责调度协调和 merge。

## 文档语言要求

除非用户明确要求其他语言，roadmap 文档及其相关书面产物必须使用中文。代码、命令、路径、标识符保持原文。

## 文档结构

roadmap 文档固定包含：

1. 目标
2. 范围边界
3. 非目标
4. 阶段列表
5. 每阶段完成定义
6. 子任务映射
7. 依赖
8. 风险
9. 备选路径
10. 当前建议推进顺序
11. 子任务执行策略
12. 调度策略（仅 parallel-batch / hybrid）

### 阶段列表

- **全局优先级约束**：在阶段切分前，必须用三条优先级（性能最优 > 方案完美性 > HotUpdate 支持）评估每个阶段的方案选择。如果某个阶段的方案违反优先级顺序，必须记录裁决理由。
- 默认按顺序推进
- 允许在必要时标记"并行准备"或"并行执行"

### 每阶段完成定义

每个阶段固定包含：

- `goal`
- `exit_criteria`
- `deliverables`
- `dependencies`
- `resolved_decisions`
- `watch_items`

其中：

- `resolved_decisions` 只记录已经确认完的阶段边界、启动条件或职责拍板
- `watch_items` 只记录执行中需要观察的风险信号，不能是待确认的设计问题

### 子任务映射

子任务表固定字段：

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|

字段说明：

- `task_id`：子任务唯一 ID
- `phase`：所属阶段
- `status`：`planned` / `ready` / `in-progress` / `blocked` / `hanging` / `completed` / `abandoned`
- `owner`：归属执行终端（parallel-batch 模式下为 `terminal-1` / `terminal-2` 等）
- `purpose`：一句话目标
- `depends_on`：前置依赖的 task_id
- `batch_id`：所属调度批次（对应 DISPATCH.md 中的 batch_id）
- `requirements`：具体产出义务（如 "managed proof + native proof + hotupdate proof"）
- `deliverables`：产物文件列表
- `exit_criteria`：完成条件
- `conflict_scope`：写出的文件路径前缀，用于校验同 batch task 是否无冲突
- `estimated_effort`：预估工作量

`status` 枚举：

- `planned` = 子任务已被识别，但尚未满足启动条件
- `ready` = 前置依赖已满足，可以正式创建或恢复为 active 子任务
- `in-progress` = 正在执行
- `blocked` = 被阻塞
- `hanging` = 暂停，后续可能恢复
- `completed` = 已完成
- `abandoned` = 已废弃

### 子任务执行策略

- roadmap 获批后默认写入：
  - `child_execution_mode: auto`
  - `auto_continue: true`
  - `auto_stop_policy: blocking-only`
- `recommended_next_child` 在串行模式下指向下一可启动子任务
- 自动启动前必须先做最小 `preflight self-review`（检查项、结果字段与判定规则同 `dev:executing-plans` 中的定义）

## 调度策略（DISPATCH.md）

当 `dispatch_model` 为 `parallel-batch` 或 `hybrid` 时，必须在 roadmap 目录下创建 `DISPATCH.md`。

### DISPATCH.md 结构

```yaml
# DISPATCH.md — 调度总纲
dispatch_model: hybrid        # sequential | parallel-batch | hybrid
parallel_merge_policy: independent-completed
# parallel_capacity 由运行时指定，不写入文档

batches:
  - batch_id: batch-1
    mode: parallel            # parallel | sequential
    description: "批次描述"
    tasks:
      - id: <task_id>
        conflict_check: no-shared-output  # 确认与同 batch 其他 task 无文件冲突
        wait_for: []                      # task 级前置依赖（跨 batch 用 batch 级 wait_for）
    wait_for: []              # batch 级前置依赖（依赖的 batch_id 列表）
    merge_after: all-completed # all-completed | continue-on-failure
```

### 调度模型选择

| 模式 | 适用场景 | 推进方式 |
|------|---------|---------|
| sequential | 子任务有强依赖、共享文件、必须串行 | 每次一个，自动推下一 child |
| parallel-batch | 子任务互相独立、无文件冲突、可并行 | 输出一批 task_id，用户自由分配到多个终端 |
| hybrid | 前段并行 + 后段串行，或有并行组 | 按 batch 定义混合推进 |

### 并行批次划分规则

1. **同 batch 内的 task 必须无文件冲突** — 由 `conflict_scope` 字段校验
2. **跨 batch 依赖通过 `wait_for` 声明** — batch 级粒度
3. **`merge_after` 决定 batch 完成后如何合并**：
   - `all-completed`：全部成功才算 batch 完成
   - `continue-on-failure`：记失败但 batch 整体继续推进；失败 task 自动记 blocker 到 STATUS.md

### 并行终端职责

- **只写 child STATUS.md**，不写父 roadmap 任何文件
- 启动时通过 task_id 指定执行哪个子任务
- 完成时更新 child STATUS.md 的 `lifecycle_status` 和 `phase`
- 遇到 blocker 时写入 child STATUS.md 的 blockers 节

### 合并（Merge）协议

当 batch 满足 merge_after 条件时，由主线 Agent 执行 merge：

1. 读取 batch 内所有 child 的 STATUS.md
2. 汇总 terminal notes 到父 STATUS.md 的 `## 调度状态`
3. 更新父 STATUS.md 的 `active_batches` / `completed_batches`
4. 检查下一 batch 的 `wait_for` 是否满足
5. 如果满足，推进到下一 batch
6. 如果不满足，输出当前阻塞原因

**硬规则：并行终端绝不写父 roadmap 文件。违反此规则的回滚处理。**

## 流程

1. 先读取当前任务目录下的 `STATUS.md`
   - 提取 `phase`、`roadmap_or_plan`、`关键文档`、最近摘要、下一步、已有设计摘要、`blocking_questions`、`question_clearance`、`clearance_confirmed_by_user`
   - 如果 `关键文档` 指向 `design-v1-01.md` / `brainstorm-v1-01.md`，再按需读取这些文档
   - 如果没有额外设计文档，直接以 `STATUS.md` 中的设计摘要作为 roadmap 输入
2. 先确认上游问题已经清零；如果 `blocking_questions` 非空、`question_clearance` 不是 `cleared`、`clearance_confirmed_by_user` 不是 `true`，或无法确认用户已完成拍板，停止本技能并返回 `brainstorming`
3. 明确写出为什么该任务需要 roadmap，而不是单一 plan
4. 根据任务规模和子任务依赖关系确定 `dispatch_model`：
   - 子任务有强依赖链或无独立执行边界 → `sequential`
   - 子任务互相独立且无文件冲突 → `parallel-batch`
   - 两者混合 → `hybrid`
5. 如果 `dispatch_model` 为 `parallel-batch` 或 `hybrid`：创建 `DISPATCH.md`
6. 产出 roadmap 文档结构，子任务表使用增强字段；不得在 roadmap 中新增执行前未决问题
7. 更新 `STATUS.md`
   - `task_type` 设为 `roadmap`
   - `phase` 设为 `roadmap`
   - `roadmap_or_plan` 指向新 roadmap 文档
   - 写入 `child_execution_mode: auto`（除非用户明确要求 `manual`）
   - 写入 `auto_continue: true`
   - 写入 `auto_stop_policy: blocking-only`
   - 写入 `dispatch_model`
   - 写入 `最近摘要`
   - 写入 `latest_stop_point`
   - 写入 `下一步`
   - 写入 `recommended_next_child`（如存在）
   - `关键文档` 至少应包含当前 roadmap 文档；若有 DISPATCH.md 也一并引用
8. 根据调度模式分流：
   - **sequential**：按原流程（第 9 步），自动启动 recommended_next_child
   - **parallel-batch / hybrid**：输出当前可派发 batch 的完整 task 列表，由用户分配到不同终端执行。不自动启动单个 child
9. 如果 `child_execution_mode = auto` 且存在 `recommended_next_child`：
   - 默认立即启动或恢复该子任务，不再为本次 child 单独询问用户确认
   - 若子任务目录不存在：创建新的任务目录
   - 在子任务 `STATUS.md` 中写入：
     - `parent_task_id`
     - `source_task_id`
     - `source_relation: roadmap-child`
     - `clearance_source: parent-roadmap`
     - `blocking_questions: []`
     - `question_clearance: cleared`
     - `clearance_confirmed_by_user: true`
     - `entry_skill`（子任务入口 skill，见下方规则）
     - `## Scope` / `## Inputs` / `## Expected Outputs` / `## Exit Criteria`（parallel-batch 模式下必须完整）
   - `entry_skill` 规则：
     - 大多数情况用 `dev-writing-plans`（先写 plan → 再执行）
     - 如果子任务本身就是独立的新功能，从 `dev-brainstorm` 开始
     - 如果子任务极小且边界清晰，可直接用 `dev-executing-plans`
     - 如果子任务继承父级充分上下文（边界、接口契约、架构结论已在父 roadmap/design 中明确），可用 `lightweight-brainstorm`：调用 brainstorm 技能但自动切入轻量级模式，跳过全流程
   - 子任务默认先进入 `writing-plans` 做最小 `preflight self-review`
   - 如果 `preflight self-review = pass|warn`：继续自动执行
   - 如果 `preflight self-review = stop`：停止自动推进，并向用户说明问题后等待确认
   - 子任务后续必须通过 `dev:executing-plans` 或 `dev:subagent-driven-development` 执行到终态
   - 子任务进入终态后，必须同步父 roadmap 的 child mapping、`latest_stop_point`、`下一步` 与新的 `recommended_next_child`
10. 如果 `child_execution_mode = manual`，或当前不存在唯一可启动的 `recommended_next_child`：
    - 说明原因并等待用户确认，再决定推进哪个子任务
11. 如果当前不继续推进父 roadmap，或父 roadmap 被取消/废弃：
    - `cancelled` / `abandoned`：将父任务移入 `docs/dev/abandoned/`，所有仍在 `in-progress` 的子任务同步移入 `docs/dev/abandoned/`
    - `hanging`（暂停，后续可能恢复）：将父任务移入 `docs/dev/hanging/`，子任务保留原目录不变
    - 删除 `docs/dev/ACTIVE.md`
    - 保留后续恢复入口
    - 确保父任务 `STATUS.md` 已写明 `最近摘要`、`latest_stop_point`、`下一步` 与 `recommended_next_child`

## STATUS.md 调度状态节

当 `dispatch_model` 为 `parallel-batch` 或 `hybrid` 时，STATUS.md 必须包含：

```yaml
## 调度状态

dispatch_doc: DISPATCH.md
dispatch_model: parallel-batch
active_batches: [batch-1]
completed_batches: []
terminals_active:
  - terminal-1: <task_id> (in-progress)
  - terminal-2: <task_id> (in-progress)
pending_batches: [batch-2, batch-3]
```

## 子任务 STATUS.md 自包含规则

当子任务属于 `parallel-batch` 批次时，其 STATUS.md 必须包含以下节：

```
## Scope
当前子任务的范围和边界

## Inputs
依赖的输入（产物的路径、版本、上游 task_id）

## Expected Outputs
产出的文件列表

## Exit Criteria
完成条件（proof 通过、build 通过、review 通过等）

## Terminal Notes
终端执行笔记（由执行终端填写执行摘要）
```

串行模式下不强制要求这些节，但建议保留 `## Scope` / `## Exit Criteria`。

## 完成判定

父 roadmap 任务只有在以下条件全部满足时才允许完成：

- roadmap 的阶段、边界、任务映射已经稳定
- 没有待继续拆分或待决策事项；如果出现新的执行前未决问题，必须回到 `brainstorming`
- 所有已创建子任务都进入终态：`completed`、`hanging` 或 `abandoned`
- 所有已进入终态的子任务都已物理移动到对应生命周期目录，不再停留在 `docs/dev/in-progress/`
