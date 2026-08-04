# `.codex` Skill 流程重构设计 v1

## 目标

将项目当前以 `docs/booming` 和 `docs/executions/CURRENT.md` 为核心的规划/执行流程，重构为以 `docs/dev` 任务目录为真源、以 `docs/dev/ACTIVE.md` 为轻量活动入口的新流程，使复杂任务的 brainstorm、design、roadmap/plan、execution 都落在同一个任务目录内，并支持显式挂起与恢复。

## 设计原则

1. 任务目录是真源，活动指针不是。
2. 目录表达生命周期，文档表达上下文与阶段。
3. 复杂任务必须显式立项，小请求不强制立项。
4. 主流程只能有一个 active 任务，避免 guard 语义分裂。
5. roadmap 和 plan 是两个不同层级：roadmap 负责拆阶段和派生子任务，plan 负责具体实施。

## 目标状态机

### 复杂任务入口

1. 复杂任务先走 `brainstorm`
2. brainstorm 后进入分流判断：
   - 满足以下任一条件进入 `roadmap`
     - 需要分 2 个及以上阶段推进
     - 会拆成 2 个及以上相对独立的子任务
     - 当前还不能直接写出稳定执行计划，必须先定优先级或阶段边界
     - 任务会跨多次会话持续推进
   - 其余进入 `plan`
3. 若结论为 `roadmap`，agent 先说明理由并等待用户确认；若结论为 `plan`，agent 可直接继续

### 生命周期

- 目录级生命周期：
  - `docs/dev/in-progress/`
  - `docs/dev/hanging/`
  - `docs/dev/completed/`
  - `docs/dev/abandoned/`
- 文档级工作阶段：
  - `brainstorming`
  - `roadmap`
  - `planning`
  - `executing`
  - `reviewing`
  - `blocked`
- 同一时刻只允许一个 active 任务，并由 `docs/dev/ACTIVE.md` 指向

### Guard 语义

当 `docs/dev/ACTIVE.md` 存在时，guard 必须阻止新的复杂流程直接开始，并先提示用户处理当前 active 任务。可选项固定为：

- `继续`：继续当前 active 任务，不处理新请求
- `挂起`：将当前 active 任务挂起并保存进度，然后继续处理用户本次请求
- `放弃`：将当前 active 任务标记为放弃，然后继续处理用户本次请求

如果没有 active 任务，但存在 `hanging` 任务，新的复杂任务开始前应提示已有挂起任务，但不强制恢复。

## 目录结构

```text
docs/dev/
  ACTIVE.md
  INDEX.md
  in-progress/
    INDEX.md
    <task_id>/
      STATUS.md
      brainstorm-v1-01.md
      design-v1-01.md
      roadmap-v1-01.md | plan-v1-01.md
      notes/
        progress-*.md
      attachments/
  hanging/
    INDEX.md
    <task_id>/
  completed/
    <task_id>/
  abandoned/
    <task_id>/
```

## 核心文档模型

### `STATUS.md`

`STATUS.md` 是任务状态真源。它保存：

- 结构化任务身份
- 当前生命周期与阶段
- 关键文档路径
- 当前判断
- 最近摘要
- 下一步
- 风险与阻塞
- 父/来源/子任务关联

它必须始终位于任务目录中，并在创建、分流、挂起、恢复、完成、放弃、主要阶段变化时更新。

### `ACTIVE.md`

`ACTIVE.md` 是当前 active 任务的轻量入口摘要，不承担完整执行上下文职责，只记录：

- `task_id`
- 标题
- 当前目录
- 生命周期与阶段
- 更新时间
- 状态文件路径
- 当前任务短说明
- 最近 1-2 条摘要
- 下一步

### 任务 ID 与文件命名

- 任务目录名采用 `task_id`
- `task_id` 格式：`日期 + 序号 + slug`
- 同类型文档文件名采用 `版本号 + 编号` 组合，例如：
  - `brainstorm-v1-01.md`
  - `design-v1-01.md`
  - `plan-v1-01.md`
- 只有结构性重写或用户确认后的重大修订才升版本号；编号只用于并列文档，正常保持 `01`

## roadmap 机制

### 父任务职责

roadmap 是父任务，负责：

- 定义目标、边界、非目标
- 拆分阶段
- 记录阶段完成定义
- 维护子任务映射
- 明确依赖、风险、备选路径与建议推进顺序

roadmap 父任务不承担具体长期执行。具体实施通过派生子任务目录完成。

### 子任务映射

roadmap 文档内需维护一张子任务表，字段为：

- `task_id`
- `phase`
- `status`
- `owner`
- `purpose`
- `depends_on`

状态枚举为：

- `planned`
- `ready`
- `in-progress`
- `blocked`
- `hanging`
- `completed`
- `abandoned`

其中：

- `planned`：子任务已识别，但尚未满足启动条件
- `ready`：前置依赖已满足，可以正式创建或恢复为 active 子任务

### 完成判定

父 roadmap 任务完成必须同时满足：

- 阶段、边界、任务映射已经稳定
- 没有待继续拆分或待决策事项
- 所有已创建子任务都进入终态：`completed` 或 `abandoned`

在不继续推进父 roadmap 时，父任务应进入 `hanging`，待需要继续规划时再恢复。

## 例外规则

按照新流程的默认规则，这次流程重构任务本身应倾向于 `roadmap`。但根据用户明确要求，本任务按 `plan` 方式执行，并在文档中记录为规则例外。这一例外只影响本任务，不改变通用判定规则。

## 迁移范围

### 需要改动的 skills

- `dev-using-booming`
- `dev-active-execution-guard`
- `dev-brainstorm`
- `dev-writing-plans`
- `dev-executing-plans`
- `dev-subagent-driven-development`
- `dev-project-wiki-maintenance`
- 新增 `roadmap` skill

### 需要改动的文档与测试

- `wiki/` 中关于 `CURRENT.md`、`docs/executions/history/`、执行真源的知识说明
- `.codex/tests/claude-code/` 中围绕 guard、executing-plans、subagent-driven-development 的契约测试

## 风险

1. 旧流程对 `docs/executions/CURRENT.md` 的引用分布在 skill、wiki、测试和既有设计文档中，迁移必须成组进行。
2. 旧历史资料仍会保留在仓库中，必须明确标记“历史参考，不再作为现行流程依据”。
3. 若 roadmap skill 不及时补上，`brainstorm -> roadmap` 这条新主链会只有规则没有落点。

## 验收标准

1. 新复杂任务能够在 `docs/dev/in-progress/<task_id>/` 下完成 brainstorm、design、plan/roadmap 文档落点。
2. guard 识别 `docs/dev/ACTIVE.md`，并提供 `继续 / 挂起 / 放弃`。
3. `挂起` 能将任务目录移至 `docs/dev/hanging/` 并保留恢复所需的 `STATUS.md + notes/progress-*.md`。
4. `executing-plans` 与 `subagent-driven-development` 改为维护 `STATUS.md + ACTIVE.md + 索引`。
5. 现行流程不再依赖 `docs/executions/history/`。

