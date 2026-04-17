---
name: executing-plans
description: 当你有一个已写好的实现计划，需要在当前会话中按批次执行、维护任务状态，并在必要时处理活动任务冲突时使用
---

# 执行计划

## 概述

加载计划，批判性地审查，执行所有任务，并维护当前任务目录中的 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md` 与索引文件。

**开始时宣布：** "我正在使用 executing-plans 技能来实现这个计划。"

**注意：** `dev:subagent-driven-development` 是另一条执行路径。只有当它具备与本技能等价的活动任务处理和状态维护规则时，才应切换过去。

**状态原则：** 任务目录中的 `STATUS.md` 是执行真相源；`docs/dev/ACTIVE.md` 只是当前活动任务指针。

## 文档语言要求

除非用户明确要求其他语言，由本技能创建或更新的 `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md`、索引摘要、风险记录、阻塞说明都必须使用中文。代码、命令、路径、标识符保持原文。

## 第零步：检查活动任务并决定是否进入执行

仅在你即将开始新的复杂计划执行、切换到另一份计划，或恢复已有计划时使用这一步。不要把它误用成对当前会话里小修、小查、小验证的全局拦截器。

1. 检查 `docs/dev/ACTIVE.md` 是否存在。
2. 如果不存在：继续进入“加载并审查计划”。
3. 如果存在：
   - 读取 `docs/dev/ACTIVE.md`
   - 根据其中的 `status_file` 读取对应任务目录下的 `STATUS.md`（如果能定位）
   - 提取任务名、`task_id`、`task_dir`、`lifecycle_status`、`phase`、最近摘要、下一步
4. 判断它是否就是当前要执行的任务：
   - 如果 `task_id`、`task_dir`、`status_file` 或当前计划文件明确指向同一任务：将本次执行视为恢复当前任务，直接继续，不要弹出选项
   - 如果它指向另一个任务：在处理完成前，不要开始新的计划执行
5. 如果 `ACTIVE.md` 指向另一个任务，使用类似下面的格式提示用户：

```text
检测到当前正在进行的任务：
- 任务：<任务名称>
- task_id：<task_id>
- lifecycle_status：<当前生命周期状态>
- phase：<当前阶段>
- 最近摘要：<最近摘要>
- 下一步：<下一步>

请选择：
1. 继续
2. 挂起
3. 放弃
```

6. 即使 `ACTIVE.md` 不完整，也仍然代表有活动任务，不能忽略。
7. 本技能不提供 `完成` 作为用户可选项。`completed` 只能在完成条件满足后自动产生。

## 第零点五步：处理用户选择

**如果用户选择 `继续`：**

- 回到 `ACTIVE.md` 对应的任务执行路径
- 如果当前活动任务本来就属于 `executing-plans`，基于 `ACTIVE.md` 与 `STATUS.md` 恢复执行
- 如果当前活动任务属于其他主流程，如 `roadmap` 或 `subagent-driven-development`，先恢复那条主线，不要先开始新的计划执行
- 终止当前“新计划执行入口”，不要同时推进两条主线

**如果用户选择 `挂起`：**

- 更新任务目录中的 `STATUS.md`
  - `lifecycle_status` 设为 `hanging`
  - `active` 设为 `false`
  - 补充最近摘要与下一步
- 在任务目录的 `notes/` 下追加新的 `progress-*.md`，记录挂起原因和恢复提示
- 将任务目录移动到 `docs/dev/hanging/`
- 更新 `docs/dev/INDEX.md` 与 `docs/dev/hanging/INDEX.md`
- 删除 `docs/dev/ACTIVE.md`
- 完成后回到当前请求；如果用户这次请求仍然是执行新计划，再继续进入“加载并审查计划”

**如果用户选择 `放弃`：**

- 更新任务目录中的 `STATUS.md`
  - `lifecycle_status` 设为 `abandoned`
  - `active` 设为 `false`
  - 补充最近摘要与放弃原因
- 在任务目录的 `notes/` 下追加新的 `progress-*.md`，记录放弃原因
- 将任务目录移动到 `docs/dev/abandoned/`
- 更新 `docs/dev/INDEX.md` 与 `docs/dev/abandoned/INDEX.md`
- 删除 `docs/dev/ACTIVE.md`
- 完成后回到当前请求；如果用户这次请求仍然是执行新计划，再继续进入“加载并审查计划”

## 第一步：加载并审查计划

1. 读取当前任务目录中的计划文件一次
2. 明确识别计划中的任务总数
3. 提取执行需要的关键引用：
   - 设计文档路径
   - 计划文档路径
   - 每个任务的完整文本
   - 每个任务的知识沉淀目标
4. 批判性审查计划，识别任何疑虑
   - 如存在关键缺口、互相冲突的要求或无法执行的前提，先向用户澄清，不要直接进入执行
5. 如无疑虑：创建 TodoWrite，并创建或更新：
   - 当前任务目录中的 `STATUS.md`
   - `docs/dev/ACTIVE.md`
   - `docs/dev/INDEX.md` 与对应生命周期索引

## STATUS.md 与 ACTIVE.md 的最低要求

在开始执行前，至少记录以下结构化信息。

`STATUS.md` 至少应包含：

- `task_id`
- `title`
- 设计文档路径
- 计划文档路径
- `total_tasks`
- 当前任务编号或当前任务名称
- `lifecycle_status`
- `phase`
- `active`
- 最近摘要
- 下一步
- 风险与阻塞
- wiki 目标或“本任务无 wiki 更新”

`docs/dev/ACTIVE.md` 至少应包含：

- `task_id`
- `title`
- `task_dir`
- `status_file`
- `plan_file`
- `lifecycle_status`
- `phase`
- `total_tasks`
- 当前任务编号或当前任务名称
- 最近摘要
- 下一步

## 第二步：执行任务

对每个任务：

1. 在开始编码前，先把当前任务编号或任务名称写入 `STATUS.md` 与 `docs/dev/ACTIVE.md`，并标记进入 `executing`
2. 严格按照计划步骤执行
3. 按规定运行验证
4. 更新 `STATUS.md`
   - 最近摘要
   - 下一步
   - 验证结果
   - 风险与阻塞
   - wiki 决策
5. 更新 `docs/dev/ACTIVE.md`
   - 当前任务编号或任务名称
   - 最近摘要
   - 下一步
6. 追加 `notes/progress-*.md`
7. 更新索引文件
8. 判断是否产生长期有效知识
   - 如果有：使用 `dev:project-wiki-maintenance`
   - 如果没有：在 `STATUS.md` 的最近摘要或新的 `notes/progress-*.md` 中明确记录“本任务无 wiki 更新”

## 第三步：完成开发

所有任务完成后，先运行项目测试套件，确认当前分支在任务整体完成后的最终状态是可验证的。

1. 运行项目级测试套件，而不是只依赖单任务验证
   - 例如：`npm test`、`cargo test`、`pytest`、`go test ./...`
   - 应优先选择项目约定中的完整套件命令
2. 将完整套件结果写入 `STATUS.md`、`docs/dev/ACTIVE.md` 与最新 `notes/progress-*.md`
3. 如果完整套件失败：
   - 记录失败摘要、定位信息、下一步
   - 停止，不要归档为 `completed`
   - 先修复问题，再重新运行完整套件

只有在以下条件都满足时，才允许归档为 `completed`：

- 计划任务全部完成
- 必要验证全部通过
- 项目测试套件通过
- 本轮应写入的 wiki 更新已完成

满足后：

- 更新 `STATUS.md`
  - `lifecycle_status` 设为 `completed`
  - `active` 设为 `false`
  - 记录项目测试套件通过
- 在 `notes/` 中追加最终进展记录
- 将任务目录移动到 `docs/dev/completed/`
- 删除 `docs/dev/ACTIVE.md`
- 更新索引文件
- 宣布："我正在使用 finishing-a-development-branch 技能来完成这项工作。"
- 使用 `dev:finishing-a-development-branch`

## 何时停下来寻求帮助

遇到以下情况立即停止执行：

- 遇到阻塞（缺少依赖、测试失败、指令不清晰）
- 计划存在关键缺口导致无法开始
- 你不理解某条指令
- 验证反复失败

在停下来之前，把新的风险、阻塞和下一步记录进 `STATUS.md` 与最新 `notes/progress-*.md`。

## 注意事项

- 先处理活动任务冲突，再开始新的计划执行
- 如果 `ACTIVE.md` 指向当前任务，应将本次执行视为恢复，而不是新开任务
- 先批判性审查计划
- 在开始时确认任务总数
- 严格按照计划步骤执行
- 不要跳过验证
- 任务全部完成后，不要省略项目测试套件
- 不要静默忽略不完整的 `ACTIVE.md`
- 不要让 `STATUS.md`、`ACTIVE.md` 或索引落后于真实状态
- 长期知识进入 `wiki/`，执行流水留在任务目录的 `notes/`

## 集成

- `dev:using-git-worktrees`
- `dev:writing-plans`
- `dev:project-wiki-maintenance`
- `dev:finishing-a-development-branch`
