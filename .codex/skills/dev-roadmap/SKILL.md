---
name: roadmap
description: 当 brainstorming 判定复杂任务需要多阶段推进或拆分为多个独立子任务时使用
---

# 编写 Roadmap

## 概述

当复杂任务已经完成设计拍板、但仍需要多阶段推进或拆成多个独立子任务时，用 roadmap 把它拆成阶段、边界和可派生子任务。roadmap 是父任务文档，不承担具体长期执行；具体实施通过派生子任务目录完成。roadmap 不是继续澄清 open questions 的地方，只承接已经确认完的问题与设计结论。除非用户明确要求手动推进，roadmap 获批后默认进入 `child_execution_mode = auto`：控制器应按 `recommended_next_child` 自动续跑子任务，并在每次启动前做一次最小 `preflight self-review`；只有命中真实阻塞条件才停下来确认。

**开始时宣布：** "我正在使用 roadmap 技能来创建阶段路线图。"

**父任务恢复原则：**

- 父 roadmap 的 `STATUS.md` 是恢复真相源
- `roadmap-v1-01.md` 提供阶段与子任务结构
- 上游存在 `design-v1-01.md` / `brainstorm-v1-01.md` 时按需读取
- 上游没有额外设计文档时，必须以 `STATUS.md` 中的设计摘要作为权威输入
- 创建 roadmap 前，必须能确认上游已经写明 `blocking_questions: []` 或等价的“问题已清零”结论；若无法确认，返回 `brainstorming`

**子任务生命周期原则：**

- roadmap 派生出的 child task 是独立任务，不是只存在于 roadmap 表格里的状态行
- child task 一旦进入终态，必须把任务目录物理移出 `docs/dev/in-progress/`
- `completed` -> `docs/dev/completed/`
- `hanging` -> `docs/dev/hanging/`
- `abandoned` -> `docs/dev/abandoned/`
- 父 roadmap 只负责同步 child mapping、摘要和下一步；不能用父任务文案替代 child task 的归档动作
- 绝不允许子任务目录仍留在 `docs/dev/in-progress/`，但 roadmap 表或 child `STATUS.md` 正文已经写成 `completed` / `hanging` / `abandoned`

**保存位置：** 当前任务目录下的 `roadmap-v1-01.md`
- 默认路径示例：`docs/dev/in-progress/<task_id>/roadmap-v1-01.md`

## 使用时机

brainstorming 结束后，只要满足以下任一条件，就应进入本技能：

- 需要分 2 个及以上阶段推进
- 会拆成 2 个及以上相对独立的子任务
- 任务会跨多次会话持续推进
- 设计与执行前提已经确认，只是需要父级阶段拆分和子任务编排

## 用户确认门

如果分流结论是 `roadmap`，必须先向用户说明原因并等待确认，才能继续本技能。这里确认的是“已拍板设计需要拆成多阶段/多子任务推进”，而不是把未决问题留到 roadmap 中再慢慢确认。除非用户明确要求 `manual`，这次确认同时视为对子任务自动推进策略的授权，不需要在每个 child 启动前重复确认。

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

### 阶段列表

- 默认按顺序推进
- 允许在必要时标记“并行准备”或“并行执行”

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

- `task_id`
- `phase`
- `status`
- `owner`
- `purpose`
- `depends_on`

`status` 枚举：

- `planned`
- `ready`
- `in-progress`
- `blocked`
- `hanging`
- `completed`
- `abandoned`

其中：

- `planned` = 子任务已被识别，但尚未满足启动条件
- `ready` = 前置依赖已满足，可以正式创建或恢复为 active 子任务

### 子任务执行策略

- roadmap 获批后，默认写入：
  - `child_execution_mode: auto`
  - `auto_continue: true`
  - `auto_stop_policy: blocking-only`
- `recommended_next_child` 应尽量始终指向唯一、可启动的下一子任务；如果当前没有唯一下一项，必须在 `下一步` 明确原因
- `auto` 模式下，控制器应默认自动启动或恢复 `recommended_next_child`
- 自动启动前必须先做最小 `preflight self-review`
- `preflight self-review` 只允许三种结果：
  - `pass`：没有实质风险，直接继续
  - `warn`：只有实现级风险或观察项，写入 `STATUS.md` 后继续
  - `stop`：存在新的执行前问题、边界漂移、authority 变化、依赖未满足或需要改父 roadmap，必须停下来确认
- 只有用户明确要求 `manual`，或 `preflight self-review = stop`，才为 child 启动单独停下来确认

## 流程

1. 先读取当前任务目录下的 `STATUS.md`
   - 提取 `phase`、`roadmap_or_plan`、`关键文档`、最近摘要、下一步、已有设计摘要、`blocking_questions`、`question_clearance`、`clearance_confirmed_by_user`
   - 如果 `关键文档` 指向 `design-v1-01.md` / `brainstorm-v1-01.md`，再按需读取这些文档
   - 如果没有额外设计文档，直接以 `STATUS.md` 中的设计摘要作为 roadmap 输入
2. 先确认上游问题已经清零；如果 `blocking_questions` 非空、`question_clearance` 不是 `cleared`、`clearance_confirmed_by_user` 不是 `true`，或无法确认用户已完成拍板，停止本技能并返回 `brainstorming`
3. 明确写出为什么该任务需要 roadmap，而不是单一 plan
4. 产出 roadmap 文档结构；不得在 roadmap 中新增执行前未决问题
5. 更新 `STATUS.md`
   - `task_type` 设为 `roadmap`
   - `phase` 设为 `roadmap`
   - `roadmap_or_plan` 指向新 roadmap 文档
   - 写入 `child_execution_mode: auto`（除非用户明确要求 `manual`）
   - 写入 `auto_continue: true`
   - 写入 `auto_stop_policy: blocking-only`
   - 写入 `最近摘要`
   - 写入 `latest_stop_point`
   - 写入 `下一步`
   - 写入 `recommended_next_child`（如存在）
   - `关键文档` 至少应包含当前 roadmap 文档；若仍有可用设计文档，也一并保留引用
6. 如果 `child_execution_mode = auto` 且存在 `recommended_next_child`：
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
   - 子任务默认先进入 `writing-plans` 做最小 `preflight self-review`
   - 如果 `preflight self-review = pass|warn`：继续自动执行
   - 如果 `preflight self-review = stop`：停止自动推进，并向用户说明问题后等待确认
   - 子任务后续必须通过 `dev:executing-plans` 或 `dev:subagent-driven-development` 执行到终态
   - 子任务进入终态后，必须同步父 roadmap 的 child mapping、`latest_stop_point`、`下一步` 与新的 `recommended_next_child`
7. 如果 `child_execution_mode = manual`，或当前不存在唯一可启动的 `recommended_next_child`：
   - 说明原因并等待用户确认，再决定推进哪个子任务
8. 如果当前不继续推进父 roadmap：
   - 将父任务移动到 `docs/dev/hanging/`
   - 删除 `docs/dev/ACTIVE.md`
   - 保留后续恢复入口
   - 确保父任务 `STATUS.md` 已写明 `最近摘要`、`latest_stop_point`、`下一步` 与 `recommended_next_child`

## 完成判定

父 roadmap 任务只有在以下条件全部满足时才允许完成：

- roadmap 的阶段、边界、任务映射已经稳定
- 没有待继续拆分或待决策事项；如果出现新的执行前未决问题，必须回到 `brainstorming`
- 所有已创建子任务都进入终态：`completed`、`hanging` 或 `abandoned`
- 所有已进入终态的子任务都已物理移动到对应生命周期目录，不再停留在 `docs/dev/in-progress/`

