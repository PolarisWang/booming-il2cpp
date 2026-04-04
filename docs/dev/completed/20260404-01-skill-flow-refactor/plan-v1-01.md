# `.codex` Skill 流程重构实施计划 v1

> **面向执行 Agent：** 本任务按用户明确指令采用 `plan` 方式执行，但其规则目标仍是为后续复杂任务建立 `brainstorm -> roadmap/plan -> docs/dev` 的统一流程。步骤使用复选框（`- [ ]`）跟踪。

**目标：** 将 skill 主流程切换到 `docs/dev` 任务目录、`ACTIVE.md` 指针和新 guard/roadmap 语义，并同步迁移 wiki 与测试。

**架构：** 先建立 `docs/dev` 底座与当前任务文档，再以测试为约束改 workflow skills，随后迁移 wiki 和 Claude Code tests，最后运行验证。新流程运行时只认 `docs/dev` 体系，旧 `docs/executions/` 仅保留历史资料。

**技术栈：** Markdown、shell tests、Claude Code skill docs

**设计文档：** `docs/dev/in-progress/20260404-01-skill-flow-refactor/design-v1-01.md`

**预期知识沉淀：** `wiki/02-Skill体系/`、`wiki/03-功能模块/01-执行系统/`、`wiki/01-项目总览/`

---

## Chunk 1: 新流程底座

### 任务 1：建立 docs/dev 目录与模板

**文件：**
- 创建：`docs/dev/INDEX.md`
- 创建：`docs/dev/in-progress/INDEX.md`
- 创建：`docs/dev/hanging/INDEX.md`
- 创建：`docs/dev/ACTIVE.md`
- 创建：`docs/dev/in-progress/20260404-01-skill-flow-refactor/STATUS.md`
- 创建：`docs/dev/in-progress/20260404-01-skill-flow-refactor/brainstorm-v1-01.md`
- 创建：`docs/dev/in-progress/20260404-01-skill-flow-refactor/design-v1-01.md`
- 创建：`docs/dev/in-progress/20260404-01-skill-flow-refactor/plan-v1-01.md`

**知识沉淀：**
- 目标：`wiki/03-功能模块/01-执行系统/`
- 原因：新的任务目录、状态页、active 指针属于长期有效流程知识

- [x] 步骤 1：创建 `docs/dev` 目录、索引和当前任务目录
- [x] 步骤 2：写入 `STATUS.md`、`ACTIVE.md` 与当前任务设计/计划文档
- [x] 步骤 3：自查文档是否与已确认流程一致

## Chunk 2: 流程 skills

### 任务 2：引入 roadmap skill 并改规划链入口

**文件：**
- 创建：`.codex/skills/dev-roadmap/SKILL.md`
- 修改：`.codex/skills/dev-using-booming/SKILL.md`
- 修改：`.codex/skills/dev-active-execution-guard/SKILL.md`
- 修改：`.codex/skills/dev-brainstorm/SKILL.md`
- 修改：`.codex/skills/dev-writing-plans/SKILL.md`

**知识沉淀：**
- 目标：`wiki/02-Skill体系/`
- 原因：技能职责、分流规则和 guard 语义均发生长期变化

- [x] 步骤 1：先修改相关测试与契约摘要，使其描述新 `ACTIVE.md`、guard 选项和 roadmap/plan 分流
- [x] 步骤 2：新增 `dev-roadmap` skill，并明确其文档落点、阶段结构和父任务职责
- [x] 步骤 3：更新 `using-booming`，将全局入口从 `docs/executions/CURRENT.md` 切换到 `docs/dev/ACTIVE.md`
- [x] 步骤 4：更新 `active-execution-guard`，引入 `继续 / 挂起 / 放弃` 和“挂起/放弃后继续本次请求”的语义
- [x] 步骤 5：更新 `brainstorm` 与 `writing-plans`，将默认文档落点改为当前任务目录，并接入 roadmap/plan 分流

## Chunk 3: 执行 skills

### 任务 3：切换执行状态真源

**文件：**
- 修改：`.codex/skills/dev-executing-plans/SKILL.md`
- 修改：`.codex/skills/dev-subagent-driven-development/SKILL.md`
- 修改：`.codex/skills/dev-project-wiki-maintenance/SKILL.md`

**知识沉淀：**
- 目标：`wiki/03-功能模块/01-执行系统/`
- 原因：执行状态真源从 `CURRENT.md` 变为任务目录，属于核心长期知识

- [x] 步骤 1：先让现有 clean tests 针对新状态文件失败，确认旧语义已被覆盖
- [x] 步骤 2：更新 `executing-plans`，以 `STATUS.md + ACTIVE.md + notes/progress-*.md + 索引` 取代 `CURRENT.md + history`
- [x] 步骤 3：更新 `subagent-driven-development`，同步采用新状态模型，并要求任务后更新索引
- [x] 步骤 4：更新 `project-wiki-maintenance`，将执行过程边界从 `docs/executions/` 调整为“任务目录内过程文档”

## Chunk 4: Wiki 与测试迁移

### 任务 4：迁移项目知识与测试

**文件：**
- 修改：`wiki/INDEX.md`
- 修改：`wiki/01-项目总览/知识边界.md`
- 修改：`wiki/02-Skill体系/INDEX.md`
- 修改：`wiki/02-Skill体系/skill-registry.md`
- 修改：`wiki/02-Skill体系/01-主线工作流/executing-plans.md`
- 修改：`wiki/03-功能模块/01-执行系统/INDEX.md`
- 修改：`wiki/03-功能模块/01-执行系统/CURRENT执行上下文机制.md`
- 修改：`.codex/tests/claude-code/README.md`
- 修改：`.codex/tests/claude-code/test-active-execution-guard-clean.sh`
- 修改：`.codex/tests/claude-code/test-executing-plans-clean.sh`
- 修改：`.codex/tests/claude-code/test-subagent-driven-development.sh`
- 修改：`.codex/tests/claude-code/test-helpers.sh`

**知识沉淀：**
- 目标：`wiki/01-项目总览/`、`wiki/02-Skill体系/`、`wiki/03-功能模块/01-执行系统/`
- 原因：新流程会改变项目知识边界、skill 注册表和执行系统说明

- [x] 步骤 1：更新 wiki，明确 `docs/dev` 是现行流程主路径，`docs/executions/` 仅保留历史资料
- [x] 步骤 2：更新 skill registry 和主线工作流说明，加入 roadmap skill 与新 guard 语义
- [x] 步骤 3：修改 Claude Code tests 与契约摘要，覆盖 `ACTIVE.md`、挂起恢复、任务目录真源和 roadmap/plan 分流

## Chunk 5: 验证与收尾

### 任务 5：验证新流程

**文件：**
- 修改：`docs/dev/in-progress/20260404-01-skill-flow-refactor/STATUS.md`
- 创建：`docs/dev/in-progress/20260404-01-skill-flow-refactor/notes/progress-v1-01.md`

**知识沉淀：**
- 目标：`无`
- 原因：本任务的验证过程属于一次性执行记录

- [x] 步骤 1：运行相关 skill tests，确认新契约通过
- [x] 步骤 2：记录验证结果到 `notes/progress-v1-01.md`
- [x] 步骤 3：更新 `STATUS.md` 的最新摘要与下一步
