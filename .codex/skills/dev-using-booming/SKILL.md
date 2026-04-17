---
name: using-booming
description: 在开始任何对话时使用，负责先判断应该激活哪些技能，再进入设计、计划、实现或验证
---

<SUBAGENT-STOP>
如果你是作为子 Agent 被派发来执行一个已经明确边界的子任务，跳过本技能。
</SUBAGENT-STOP>

<EXTREMELY-IMPORTANT>
只要有任何一个技能与当前任务存在明显关联，就必须先使用该技能。
不要先开始分析、读代码、写计划或写实现，再回头补技能。
</EXTREMELY-IMPORTANT>

## 指令优先级

1. 用户明确指令
2. 项目技能规则
3. 默认系统行为

## 核心规则

### 1. 在任何响应或行动前先选技能

- 新功能、行为调整、流程重构：先用 `brainstorming`
- 已有明确多阶段任务：用 `writing-plans` 或 `roadmap`
- 已有计划并开始推进：用 `executing-plans`
- bug、回归、异常结果：先用 `systematic-debugging`
- 完成前的结果确认：用 `verification-before-completion`

### 2. 命中测试治理或 AOT obligation 任务时，先走 `project-test-governance`

只要任务涉及以下任一内容，在进入计划或实现前必须使用 `project-test-governance`：

- `subjects/`、`tests/`、`Chaos.TestFramework`、collection file、manifest、runner、dashboard、generated codegen
- managed / native / hotupdate 测试主线或 `.NET 8` collector
- AOT / IL2CPP 新 feature 接入
- `ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact` 之类的 obligation 变更
- `formalVerificationObjects`、`requiredGates`、`completed` gate 或正式验证对象选择逻辑

### 3. 工作流映射

- “设计一个新功能 / 规范 / 架构”：
  `brainstorming -> writing-plans`，如果是多阶段主线则转 `roadmap`
- “继续执行现有 roadmap / plan”：
  `executing-plans`
- “修这个 bug / 回归 / benchmark 异常 / 测试阶段 dotnet 崩溃”：
  `systematic-debugging -> project-test-governance -> test-driven-development`
- “AOT 新 feature / owner subject / proof-benchmark obligation 调整”：
  `project-test-governance -> writing-plans`
- “调整 subject/test/runner/codegen 主线”：
  `project-test-governance -> writing-plans` 或 `executing-plans`

### 4. 不要把 `docs/dev/ACTIVE.md` 当成每一步都要拦截用户的总开关

- 小范围阅读、局部核对、低风险验证可以直接处理
- 一旦进入新的正式主线任务，再由计划/执行类技能负责更新 `docs/dev/ACTIVE.md`、`STATUS.md` 和索引

## 红旗

以下想法说明你在绕过流程：

- “我先看几眼代码再决定要不要用技能”
- “这只是测试目录的小改动，不算测试治理”
- “先把 benchmark 跑通，之后再补自动化测试”
- “先把 emitter 拼出来，模板后面再说”
- “formal verification object 后面再补，现在先归档 completed”

## 关联技能

- 上游：无
- 协作：`project-test-governance`、`test-driven-development`、`project-wiki-maintenance`
- 下游：`brainstorming`、`roadmap`、`writing-plans`、`executing-plans`、`systematic-debugging`
