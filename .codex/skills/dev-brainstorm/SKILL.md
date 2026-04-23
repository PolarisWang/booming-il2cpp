---
name: brainstorming
description: Use when defining a new feature, behavior change, component, workflow, or other creative work before planning or implementation, especially when intent, scope, constraints, or success criteria still need to be clarified.
---

# 将想法转化为设计

通过协作式对话，把模糊想法收敛成可审查的设计与规格，并在规格获批后明确分流到 `roadmap` 或 `writing-plans`。本技能只用于确实需要拍板边界、authority、结构策略或多方案权衡的任务，不用于默认的边界清晰小改动。`brainstorming` 也是执行前问题清零的唯一入口：在正式落 `design`、创建 `roadmap` 或进入 `writing-plans` 之前，所有影响执行的问题都必须在这里被确认完。

<HARD-GATE>
在你展示设计并且用户批准之前，不要调用任何实现技能、编写任何代码、搭建任何项目或采取任何实现行动。只要已经进入本技能，就不能跳过这个门。
</HARD-GATE>

<QUESTION-CLEARANCE-GATE>
在正式写入 `design-v1-01.md`、把设计摘要落入 `STATUS.md` 作为下游权威输入、或分流到 `roadmap` / `writing-plans` 之前，必须先把所有影响执行的问题清零，并得到用户明确确认。未清零的问题不能留给 roadmap、plan 或执行阶段继续确认。
</QUESTION-CLEARANCE-GATE>

## 反模式：这太简单，不需要设计

不是任何任务都要进入本技能。真正边界清晰、单会话、单目标的小任务，应直接实现；只有当边界、authority、职责分层、结构策略或成功标准仍不稳定时，才进入本技能。

## 文档语言要求

除非用户明确要求其他语言，所有由本技能产出的 brainstorm 文档、设计文档、方案对比、修改稿和面向用户的书面说明都必须使用中文。代码、命令、路径、配置键、接口名和其他专有标识保持原文。

## 检查清单

1. **探索项目上下文** — 检查文件、文档、最近提交、现有结构和已有模式
2. **评估任务规模** — 如果请求包含多个相对独立的子系统，先拆分边界、关系和顺序，不要立即深挖实现细节
3. **提供可视化伴侣**（如适用）— 必须单独发一条征求同意的消息；用户同意后，先读取 `visual-companion.md`
4. **提出澄清问题** — 一次一个，优先选择题，聚焦 purpose、constraints、success criteria
5. **维护问题清单** — 显式区分 `blocking_questions` 与 `watch_items`；前者必须清零，后者只能是执行中观察项，不能是待拍板问题
6. **提出 2-3 种方案** — 附带权衡分析、推荐理由和 YAGNI 裁剪
7. **问题清零** — 持续提问、比较方案、拍板边界，直到 `blocking_questions = []`
8. **展示已拍板设计** — 只展示没有执行前未决项的设计；按复杂度分段展示；每段确认无误后再继续
9. **按需编写设计文档** — 只有在需要跨消息保留方案权衡或结构拍板时才保存文档；默认优先只落 `design-v1-01.md`，`brainstorm-v1-01.md` 仅在确有必要保留方案对比时创建
10. **规格审查循环** — 先自查，必要时使用 `spec-document-reviewer-prompt.md`
11. **用户审查并确认问题已清零**
12. **过渡到规划分流** — 只能进入 `roadmap` 或 `writing-plans`

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
- 进入本技能后，当前任务目录至少应存在 `STATUS.md`
- `design-v1-01.md` 只在边界、authority、职责结构或关键方案需要沉淀时创建
- `brainstorm-v1-01.md` 默认不强制；只有在需要保留多方案对比、取舍记录或跨会话设计上下文时才创建

## 问题清零规则

- 进入本技能后，先维护一个最小的 `blocking_questions` 清单；允许只写在对话或临时思考中，但在落正式文档前必须明确清零结果
- `blocking_questions` 指任何会影响执行启动的未决事项，例如：
  - 边界与非目标是否已定
  - authority / owner / 职责分层是否已定
  - 关键结构方案、依赖集成方式、接口方向是否已定
  - 验收口径、退出条件、主线 obligation 是否已定
  - roadmap 的阶段切分、子任务切分、启动条件是否已定
- `watch_items` 只允许记录“执行中需要观察的风险或信号”，不能伪装成待确认的设计问题
- 只要 `blocking_questions` 非空，就继续停留在 brainstorming；不得把这些问题留给 `roadmap`、`writing-plans` 或执行阶段
- 只有当 `blocking_questions = []` 且用户明确确认这些拍板后，才允许：
  - 正式写入 `design-v1-01.md`
  - 把设计摘要写入 `STATUS.md` 作为下游权威输入
  - 分流到 `roadmap` 或 `writing-plans`

## 设计完成后

### 文档

- 如果本轮需要保留方案对比，再将 brainstorm 结论写入当前任务目录下的 `brainstorm-v1-01.md`
- 如果本轮存在需要沉淀的边界、authority 或结构拍板，再将经过验证的设计写入当前任务目录下的 `design-v1-01.md`
- 更新 `STATUS.md`
  - `phase` 应体现当前仍处于 `brainstorming` 或即将进入下一阶段
  - 明确写入 `blocking_questions: []`
  - 明确写入 `question_clearance: cleared`
  - 明确写入 `clearance_confirmed_by_user: true`
  - `关键文档` 只指向本轮实际创建的设计文档；没有额外文档时不强制补空字段
  - 如果没有单独的 `design-v1-01.md`，必须把本轮可供下游恢复的最小设计摘要写入 `STATUS.md`
    - `边界拍板`
    - `authority 决策`
    - `当前结论`
    - `下一步入口`

### 规格审查循环

写完规格后，至少检查以下问题并直接修正文档：

1. **占位符检查** — `TODO`、`TBD`、未完成章节、模糊占位语
2. **一致性检查** — 架构、需求、边界是否互相冲突
3. **歧义检查** — 是否存在两种合理解读；如果有，必须明确拍板
4. **范围检查** — 是否仍适合单一 plan；如果不适合，应明确转入 `roadmap`
5. **问题清零检查** — 不允许仍存在会影响执行启动的未决问题；若有，回到澄清阶段
6. **设计质量检查** — 是否覆盖 `architecture / components / data flow / error handling / testing`

如果需要派发规格审查者，使用 `spec-document-reviewer-prompt.md` 模板。审查只拦会实质影响 roadmap 或实现规划的问题，不拦措辞偏好。

### 用户审查门

规格审查循环通过后，在继续之前请用户审查已写规格，并确认执行前问题已经清零：

- 如果创建了独立设计文档：

> "规格已写入 `<path>`。请审查它，并确认执行前问题已经全部关闭；如果在进入下一步规划之前还想修改任何拍板，请告知。"

- 如果没有创建独立设计文档，而是把设计摘要写入了 `STATUS.md`：

> "设计摘要已写入当前任务的 `STATUS.md`。请先审查这些拍板内容，并确认执行前问题已经全部关闭；如果在进入下一步规划之前还想修改任何拍板，请告知。"

## 规划分流

只有在 `blocking_questions = []` 且用户明确确认之后，才允许做分流判断。brainstorming 的唯一合法下游是 `roadmap` 或 `writing-plans`。

下游交接原则：

- `STATUS.md` 始终是最小恢复入口
- 如果存在 `design-v1-01.md` 或 `brainstorm-v1-01.md`，下游按需读取
- 如果没有额外设计文档，下游必须以 `STATUS.md` 中的设计摘要作为权威输入，而不是假定文档一定存在

满足以下任一条件，进入 `roadmap`：

- 需要分 2 个及以上阶段推进
- 会拆成 2 个及以上相对独立的子任务
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
