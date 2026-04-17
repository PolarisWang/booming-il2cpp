---
name: brainstorming
description: Use when defining a new feature, behavior change, component, workflow, or other creative work before planning or implementation, especially when intent, scope, constraints, or success criteria still need to be clarified.
---

# 将想法转化为设计

通过协作式对话，把模糊想法收敛成可审查的设计与规格，并在规格获批后明确分流到 `roadmap` 或 `writing-plans`。

<HARD-GATE>
在你展示设计并且用户批准之前，不要调用任何实现技能、编写任何代码、搭建任何项目或采取任何实现行动。无论任务看起来多简单，都不能跳过这个门。
</HARD-GATE>

## 反模式：这太简单，不需要设计

任何任务都要经过这个过程。真正简单的任务可以只有几句话设计，但仍然必须明确假设、边界和成功标准，并得到用户批准。

## 文档语言要求

除非用户明确要求其他语言，所有由本技能产出的 brainstorm 文档、设计文档、方案对比、修改稿和面向用户的书面说明都必须使用中文。代码、命令、路径、配置键、接口名和其他专有标识保持原文。

## 检查清单

1. **探索项目上下文** — 检查文件、文档、最近提交、现有结构和已有模式
2. **评估任务规模** — 如果请求包含多个相对独立的子系统，先拆分边界、关系和顺序，不要立即深挖实现细节
3. **提供可视化伴侣**（如适用）— 必须单独发一条征求同意的消息；用户同意后，先读取 `visual-companion.md`
4. **提出澄清问题** — 一次一个，优先选择题，聚焦 purpose、constraints、success criteria
5. **提出 2-3 种方案** — 附带权衡分析、推荐理由和 YAGNI 裁剪
6. **展示设计** — 按复杂度分段展示；每段确认无误后再继续
7. **编写 brainstorm 与设计文档** — 保存到当前任务目录，例如 `brainstorm-v1-01.md` 与 `design-v1-01.md`
8. **规格审查循环** — 先自查，必要时使用 `spec-document-reviewer-prompt.md`
9. **用户审查已写规格**
10. **过渡到规划分流** — 只能进入 `roadmap` 或 `writing-plans`

## 设计质量要求

设计至少覆盖以下内容：

- `architecture`
- `components` 与职责边界
- `data flow` 或关键控制流
- `error handling`、边缘情况与集成点
- `testing` 与验证方式

设计应尽量把系统拆成边界清晰的单元。对每个单元，至少要能回答：

- 它做什么
- 如何被使用
- 依赖什么
- 是否可以被独立理解和测试

## 现有代码库中的设计规则

- 先探索当前结构，再提出变更
- 优先沿用现有模式，避免凭空发明新结构
- 如果现有边界问题会直接影响当前目标，可以把针对性改善纳入设计
- 不要提出与当前目标无关的重构

## 任务目录规则

- 复杂任务必须有任务目录
- 默认任务目录为 `docs/dev/in-progress/<task_id>/`
- 当前任务目录内至少应存在：
  - `STATUS.md`
  - `brainstorm-v1-01.md`
  - `design-v1-01.md`

## 设计完成后

### 文档

- 将 brainstorm 结论写入当前任务目录下的 `brainstorm-v1-01.md`
- 将经过验证的设计写入当前任务目录下的 `design-v1-01.md`
- 更新 `STATUS.md`
  - `phase` 应体现当前仍处于 `brainstorming` 或即将进入下一阶段
  - `关键文档` 应指向新的 brainstorm/design 文档

### 规格审查循环

写完规格后，至少检查以下问题并直接修正文档：

1. **占位符检查** — `TODO`、`TBD`、未完成章节、模糊占位语
2. **一致性检查** — 架构、需求、边界是否互相冲突
3. **歧义检查** — 是否存在两种合理解读；如果有，必须明确拍板
4. **范围检查** — 是否仍适合单一 plan；如果不适合，应明确转入 `roadmap`
5. **设计质量检查** — 是否覆盖 `architecture / components / data flow / error handling / testing`

如果需要派发规格审查者，使用 `spec-document-reviewer-prompt.md` 模板。审查只拦会实质影响 roadmap 或实现规划的问题，不拦措辞偏好。

### 用户审查门

规格审查循环通过后，在继续之前请用户审查已写规格：

> "规格已写入 `<path>`。请审查它，如果在进入下一步规划之前想做任何更改，请告知。"

## 规划分流

规格获批后，必须做一次明确的分流判断。brainstorming 的唯一合法下游是 `roadmap` 或 `writing-plans`。

满足以下任一条件，进入 `roadmap`：

- 需要分 2 个及以上阶段推进
- 会拆成 2 个及以上相对独立的子任务
- 当前还不能直接写出稳定执行计划，必须先定优先级或阶段边界
- 任务会跨多次会话持续推进

其余进入 `writing-plans`。

### 分流拍板规则

- 若结论是 `plan`：可直接调用 `writing-plans`
- 若结论是 `roadmap`：必须先向用户说明理由并等待确认，再调用 `roadmap`

## 核心原则

- 一次一个问题
- 优先选择题
- 无情地应用 YAGNI
- 先拆大任务，再深挖细节
- 在确定前始终给出 2-3 种方案
- 增量式验证：展示一段，确认一段
- 设计通过后再进入规划
