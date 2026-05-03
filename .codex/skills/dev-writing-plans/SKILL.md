---
name: dev-writing-plans
description: 当你有规格说明或多步骤任务的需求时，在接触代码之前使用
---

# 编写计划

## 概述

编写全面的实现计划，假设执行者对代码库没有上下文。只有跨会话、多个可验证步骤、或需要稳定交接的任务才应该进入本技能。执行阶段以 `STATUS.md` 作为主记录；`docs/dev/ACTIVE.md` 仅保留活动任务指针；`notes/progress-*.md` 只在阻塞、重要决策或验证失败时追加，并为后续 wiki 知识沉淀留出明确入口。本技能只承接已经完成执行前问题清零的输入，不负责继续澄清边界、authority 或阶段切分问题。

**开始时宣布：** "我正在使用 writing-plans 技能来创建实现计划。"

**保存计划到：** 当前任务目录下的 `plan-v1-01.md`
- 默认路径示例：`docs/dev/in-progress/<task_id>/plan-v1-01.md`

## 文档语言要求

除非用户明确要求其他语言，计划文档及其相关书面产物必须使用中文。代码、命令、路径、标识符、技能名保持原文。

## 适用边界

- 本技能只用于 `brainstorm` 分流结果为 `plan` 的情况
- 只有跨会话或多步骤任务才需要 `plan`
- 边界清晰、单会话、单目标任务不应为了走流程而创建 `plan`；如已存在正式任务目录，默认只维护 `STATUS.md`
- 如果分流结论是 `roadmap`，不要继续本技能，改为调用 `roadmap`
- 如果任务最初以 `STATUS-only` 推进，但现在已跨会话、多步骤或需要稳定交接，必须在原任务目录补 `plan`，并在 `STATUS.md` 记录升级原因

## 输入清零门

- 写 `plan` 前，必须先读取当前任务的 `STATUS.md`
- 默认要求上游已经明确写出：
  - `blocking_questions: []`
  - `question_clearance: cleared`
  - `clearance_confirmed_by_user: true`
- 如果这些字段来自 `brainstorming` 或已批准设计，直接消费即可
- 如果当前任务带有 `source_relation: roadmap-child*` 且父 roadmap 已获批：
  - 允许继承父任务的清零结果
  - 在当前 `STATUS.md` 中写入 `clearance_source: parent-roadmap`
- 如果任务属于 `status-only-upgrade` 或其它直接进 `plan` 的路径：
  - 先检查是否仍存在任何影响执行启动的问题
  - 如果存在：停止本技能，返回 `brainstorming`
  - 如果不存在：先把等价的清零结论写入 `STATUS.md`，并取得用户明确确认，再继续写 `plan`
- 本技能不承担继续澄清以下问题：
  - 边界与非目标是否已定
  - authority / owner / 职责分层是否已定
  - 关键结构方案、依赖集成方式、接口方向是否已定
  - 验收口径、退出条件、阶段切分、启动条件是否已定
- 只要仍有上述未决项，就不能保存 `plan-v1-01.md`

## roadmap-child Preflight Self-Review

- 当任务带有 `source_relation: roadmap-child*`，或父 roadmap 声明 `child_execution_mode = auto` 时，写 plan 前必须先做一次最小 `preflight self-review`
- 检查项、结果字段与判定规则同 `dev:executing-plans` 中的定义，聚焦是否还能安全自动继续

## 计划要求

- 在定义任务前，先列出要创建/修改的文件及其职责
- 每个任务应独立、可验证、边界清晰
- 写 `plan` 前，先确认下游权威输入：
  - 当前 `STATUS.md` 中的 `blocking_questions` / `question_clearance` / `clearance_confirmed_by_user`
  - 若当前任务是 roadmap-child：父任务中的 `child_execution_mode` / `auto_continue` / `auto_stop_policy` / `recommended_next_child`
  - 若存在 `design-v1-01.md` 或其它设计文档，读取它
  - 若没有独立设计文档，必须确认 `STATUS.md` 已记录足够的设计摘要/边界拍板，不能默认假设上游一定有 spec 文件
- 计划中必须写明：
  - 设计文档路径（没有边界或 authority 变化时可写 `n/a`）
  - 问题清零来源（例如 `brainstorm-approved STATUS.md`、`design-v1-01.md`、`direct user confirmation`、`parent-roadmap`）
  - 计划文档路径
  - 预期知识沉淀位置（默认可写 `n/a`）
  - 关键测试与验证命令
  - 执行时如何维护 `STATUS.md` / `ACTIVE.md` / `notes/progress-*.md`，并明确 `STATUS.md` 是主记录、`notes/progress-*.md` 不是例行更新点
- 对 roadmap-child，不要把 `preflight self-review` 写成长篇说明；只在 `STATUS.md` 中保留结构化结论与摘要
- 计划必须显式声明 `arch_review_mode: normal|critical`
- 当任务触及 `AOT`、`IL2CPP`、`codegen`、`contracts`、`loader`、`test governance` 任一主线时，`arch_review_mode` 必须为 `critical`
- 计划必须写明本轮结构告警与架构审视关注点：重复代码、结构/抽象调整、文件职责与拆分、`codegen` 与 `Scriban` 分层
- 如果该计划对应 roadmap-child 或其它独立子任务，必须写明：达到终态时要物理移动目录并回写父任务；不能只在 `STATUS.md` 正文或父 roadmap 表格里写 `completed`
- 计划必须显式考虑固定收尾链路：`结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交`

## 架构审核 Intake

- `arch_review_mode = normal`：默认模式，只要求结构告警与简要架构审视
- `arch_review_mode = critical`：用于 `AOT`、`IL2CPP`、`codegen`、`contracts`、`loader`、`test governance` 主线，除结构告警外，还必须预留最小 `权责图审核`
- 结构告警不使用固定行数作为 gate。文件大小只能作为上下文，真正要判断的是：
  - 是否出现重复逻辑、平行 helper、重复 emitter/mapper
  - 是否应该调整结构、补抽象或回收过度抽象
  - 是否单个文件混入多种职责，尤其是策略决策、数据映射、输出拼装混在一起
  - `codegen` 逻辑是否放错层：宿主代码里堆积输出拼装，或 `Scriban` 模板中堆积语义决策
- `critical` 计划必须写明 `权责图审核` 主题，至少说明要审的 authority 主体与预期 proof

## AOT / IL2CPP / Test Governance Intake

如果任务命中 `project-test-governance`，计划头部必须显式冻结 obligation，不能省略。

固定字段如下：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

约束：

- 非适用字段也要明确写 `n/a`、`false` 或 `[]`
- `formalVerificationObjects` 要写正式对象或 declared entry，而不是“后续跑测试”
- `requiredGates` 默认至少覆盖 `collector -> registry -> workspace`
- 命中 AOT onboarding 的计划，不能只写“跑测试”，必须写清 managed/native/hotupdate/proof/benchmark obligation

## 计划文档头部

```markdown
# [功能名称] 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。若当前任务带有 `parent_task_id` 或 `source_relation: roadmap-child*`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** [一句话描述这要构建什么]

**架构：** [2-3 句关于方法的描述]

**技术栈：** [关键技术/库]

**架构审核模式：** [normal/critical]

**结构告警重点：** [重复代码 / 结构与抽象 / 文件职责与拆分 / codegen 与 Scriban 分层]

**权责图审核主题：** [critical 任务填写 authority 主题；非 critical 填 n/a]

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: [值或 n/a]
- capabilityItem: [值或 n/a]
- ownerSubjectId: [值或 n/a]
- proofRequired: [true/false]
- benchmarkRequired: [true/false]
- hotupdateImpact: [None/Smoke/Proof 或 n/a]
- formalVerificationObjects: [对象列表或 []]
- requiredGates: [例如 collector -> registry -> workspace]

**设计文档：** [design 文档路径或 n/a]

**问题清零来源：** [brainstorm-approved STATUS.md / design-v1-01.md / direct user confirmation / parent-roadmap]

**计划来源：** [direct-plan / status-only-upgrade / roadmap-child-auto]

**预期知识沉淀：** [主要 wiki 目标路径，或 n/a]

**收尾约束：** 执行完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---
```

## 执行交接

保存计划后：

- 如果当前任务是 `roadmap-child*`，且父 roadmap 为 `child_execution_mode = auto`：
  - 若 `preflight_review = pass|warn`：直接继续执行，不等待额外用户确认
  - 若 `preflight_review = warn`：先把摘要写入 `STATUS.md`，再继续执行
  - 若 `preflight_review = stop`：停止并向用户说明问题，等待确认
- 其他计划可继续使用以下交接语：

**"计划已完成并保存到 `<当前任务目录>/plan-v1-01.md`。本计划只基于已确认并清零的问题输入；如果要改动边界或关键拍板，请先返回 brainstorming。准备好执行了吗？"**

## 执行路径

- 如果执行环境有子 Agent：使用 `dev:subagent-driven-development`
- 如果执行环境没有子 Agent：使用 `dev:executing-plans`
