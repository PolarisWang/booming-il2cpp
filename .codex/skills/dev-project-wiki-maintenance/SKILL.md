---
name: project-wiki-maintenance
description: 当任务、技能或项目规则产生长期有效知识，需要写入项目 wiki 并维护 `INDEX.md` 时使用
---

# 项目 Wiki 维护

## 概述

本技能负责将长期有效的项目知识写入 `wiki/`，并维护各层 `INDEX.md`。它只处理结论化知识，不处理任务目录中的执行流水。

**核心原则：** `docs` 记录过程，`wiki` 记录长期知识。

## 文档语言要求

除非用户明确要求其他语言，所有新增或更新的 wiki 页面、`INDEX.md`、`skill-registry.md` 和其他知识沉淀文档都必须使用中文。代码、命令、路径、标识符和必要的专有名词保持原文。

## 不要用于

- 单次执行日志
- 对话原文
- 临时尝试
- 尚未稳定的草稿结论

这些内容应保留在对应任务目录下的 `docs/dev/<lifecycle>/<task_id>/`，而不是 `wiki/`。

## 写入边界

| 内容类型 | 写入位置 |
|------|------|
| 执行日志、当前状态、阻塞 | `docs/dev/<lifecycle>/<task_id>/` |
| 某个功能本体知识 | `wiki/03-功能模块/` |
| Skill 的项目职责与边界 | `wiki/02-Skill体系/` |
| 跨功能复用经验 | `wiki/05-项目经验/` |
| 工具入口与集成规则 | `wiki/04-工具与集成/` |

## 更新流程

1. 确定知识的目标目录
2. 更新目标知识页面，写入结论化内容
3. 更新该目录的 `INDEX.md`
4. 如果新增类别或新页面影响父层导航，再更新父级 `INDEX.md`
5. 如果变更涉及项目 skill，额外更新 `wiki/02-Skill体系/skill-registry.md`

## 任务后规则

如果一个任务**确实**产生了长期有效知识，就必须更新 `wiki/`。

如果一个任务**没有**产生长期知识，不在本技能里补空页面，而是在任务目录的 `STATUS.md` 最近摘要或新的 `notes/progress-*.md` 中记录“本任务无 wiki 更新”。

## 红旗

**绝不：**

- 把执行日志写进 `wiki`
- 只改正文页面，不改 `INDEX.md`
- 在 skill 变更后忘记更新 `skill-registry.md`

## 集成

- `dev:executing-plans`
- `dev:subagent-driven-development`
