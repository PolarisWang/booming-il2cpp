---
name: dev-il2cpp
description: 项目唯一入口 skill — 对话启动时加载，通过 skill-index.md 发现领域并路由到对应技能
---

<SUBAGENT-STOP>
如果你是作为子 Agent 被派发来执行一个已经明确边界的子任务，跳过本技能。
</SUBAGENT-STOP>

<EXTREMELY-IMPORTANT>
只要有任何一个技能与当前任务存在明显关联，就必须先使用该技能。
不要先开始分析、读代码、写计划或写实现，再回头补技能。
</EXTREMELY-IMPORTANT>

## 路由桩（Routing Stub）

本文件是 `.claude/skills/` 的注册入口桩。**完整技能内容在 library 中维护**，
所有路由通过 `expert-registry.json` 集中管理。

请通过 Read 工具或以下命令加载 library 版本：

```bash
cat .ai/skills/library/skills/dev-il2cpp/SKILL.md
```

然后按 library 版本中的路由协议、核心规则和加载确认执行。

> 架构说明：
> - `.claude/skills/dev-il2cpp/SKILL.md` 是 Skill 工具注册入口（仅路由桩）
> - `.ai/skills/library/skills/dev-il2cpp/SKILL.md` 是完整技能内容（v2.1.0+）
> - `.ai/skills/library/skills/dev-il2cpp/skill.manifest.json` 是权威清单
> - 所有路径使用 `.ai/skills/` 前缀

## 关联技能

- 上游：无
- 协作：`project-test-governance`、`test-driven-development`、`project-wiki-maintenance`、`verification-before-completion`
- 下游：`dev-brainstorm`、`roadmap`、`writing-plans`、`executing-plans`、`systematic-debugging`、`dev-il2cpp-core-agent`
