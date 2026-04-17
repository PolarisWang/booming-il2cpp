---
name: writing-plans
description: 当你有规格说明或多步骤任务的需求时，在接触代码之前使用
---

# 编写计划

## 概述

编写全面的实现计划，假设执行者对代码库没有上下文。计划不仅要告诉执行者“做什么”，还要让执行阶段能够稳定维护任务目录中的 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md` 与后续的 wiki 知识沉淀。

**开始时宣布：** "我正在使用 writing-plans 技能来创建实现计划。"

**保存计划到：** 当前任务目录下的 `plan-v1-01.md`
- 默认路径示例：`docs/dev/in-progress/<task_id>/plan-v1-01.md`

## 文档语言要求

除非用户明确要求其他语言，计划文档及其相关书面产物必须使用中文。代码、命令、路径、标识符、技能名保持原文。

## 适用边界

- 本技能只用于 `brainstorm` 分流结果为 `plan` 的情况
- 如果分流结论是 `roadmap`，不要继续本技能，改为调用 `roadmap`

## 计划要求

- 在定义任务前，先列出要创建/修改的文件及其职责
- 每个任务应独立、可验证、边界清晰
- 计划中必须写明：
  - 设计文档路径
  - 计划文档路径
  - 预期知识沉淀位置
  - 关键测试与验证命令
  - 执行时如何维护 `STATUS.md` / `ACTIVE.md` / `notes/progress-*.md`

## 计划文档头部

```markdown
# [功能名称] 实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** [一句话描述这要构建什么]

**架构：** [2-3 句关于方法的描述]

**技术栈：** [关键技术/库]

**设计文档：** [design 文档路径]

**预期知识沉淀：** [主要 wiki 目标路径，或“按任务决定”]

---
```

## 执行交接

保存计划后：

**"计划已完成并保存到 `<当前任务目录>/plan-v1-01.md`。准备好执行了吗？"**

## 执行路径

- 如果执行环境有子 Agent：使用 `dev:subagent-driven-development`
- 如果执行环境没有子 Agent：使用 `dev:executing-plans`
