# DLL Capability Verification Phase 0 Authority Freeze Plan

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。本任务是 `20260427-04` 的 roadmap child，完成时必须先归档当前子任务目录，再回写父任务。

**目标：** 冻结 DLL capability verification closure 的四层 authority、目录、annotation、gate 与全量 checklist，为后续实现批次提供稳定输入。

**架构：** 本阶段不进入大规模业务代码实现，只落 formal authority 文档、目录 contract、phase checklist 与用户验收 contract。它是后续 Truth/Evidence/Execution/Projection 四层实现的唯一输入。

**技术栈：** Markdown / JSON schema contract / 现有 verification-v1 authority / foundation-dll projection contract

**架构审核模式：** critical

**结构告警重点：** authority 去重、旧目录/旧 case contract 残留、文档与 projection contract 分层、codegen 与 execution contract 的职责边界

**权责图审核主题：** DLL capability verification closure authority freeze

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `foundation-dll-verification-closure`
- capabilityItem: `dll-capability-authority-freeze`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects:
  - `docs/dev/in-progress/20260427-04-dll-capability-verification-closure-roadmap/roadmap-v1-01.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
  - `wiki/06-测试验证/AOT新Feature接入自测规范.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/INDEX.md`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

**设计文档：** n/a

**问题清零来源：** `docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm/brainstorm-v1-01.md`

**计划来源：** `roadmap-child-auto`

**预期知识沉淀：**
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/INDEX.md`

**收尾约束：** 完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 回写父 roadmap”固定链路。

## 文件与职责

- `docs/dev/in-progress/20260427-04-dll-capability-verification-closure-roadmap/roadmap-v1-01.md`
  - 父级路线图与总 checklist authority
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
  - formal verification / projection / aggregate authority
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
  - AOT / IL2CPP / proof / benchmark / hotupdate onboarding authority
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/INDEX.md`
  - solution / runner / generated project / execution entry authority
- `verification/foundation-dll/**`
  - 本阶段只冻结目录 contract，不进入大规模内容生成

## 执行步骤

- [ ] 审查并明确写出四层对象模型边界：Truth / Evidence / Execution / Projection
- [ ] 审查并明确写出新增 DLL onboarding freeze 的最小输入集合
- [ ] 审查并明确写出 family 目录 contract：`test/native/benchmark/host/patch/review`
- [ ] 审查并明确写出 A2 annotation contract：哪些字段必须枚举化，哪些字段允许 string
- [ ] 审查并明确写出 annotation scanner authority 边界：只读 generated source / metadata，不从运行结果反推 truth
- [ ] 审查并明确写出 completed-before gate = `G2 reviewable-bundle` 的正式定义
- [ ] 审查并明确写出单一正式 `.sln` 主入口 + family 局部加速入口的 contract
- [ ] 把上述 authority 逐条落进父 roadmap 的 checklist 和阶段 exit criteria
- [ ] 补齐每个后续阶段的“用户可验收步骤”，不能只保留内部技术验收
- [ ] 做第一轮旧逻辑清理清单：列出需要在后续阶段删除的旧目录/旧入口/旧 consumer
- [ ] 做第二轮 authority 复查：确认无 follow-up 尾项、无未拍板边界
- [ ] 更新当前子任务 `STATUS.md` 与父任务 `STATUS.md`

## 本阶段验收标准

### 自动化/结构验收

- [ ] authority freeze 文本已体现在父 roadmap，而不是只存在于临时对话
- [ ] phase-1 到 phase-5 的 checklist 与 exit criteria 全部可追踪
- [ ] 没有未定义的“后续再决定”条目

### 用户验收

- [ ] 用户可以从 roadmap 中直接看懂四层分别负责什么
- [ ] 用户可以直接看懂新增 DLL 时必须生成哪些内容
- [ ] 用户可以直接看懂 `Review Bundle` 到什么程度才算通过
- [ ] 用户可以直接看懂正式主 `.sln` 与 family 局部入口的关系
- [ ] 用户确认本阶段没有 follow-up 尾项

## 验证命令

- `python -m pytest tests/unit/compatibility/test_declared_metadata_discovery_contract.py -q`
- `python build/toolchains/run/run.py verify verification-v1 --json`

## 归档条件

- [ ] 本计划所有复选框完成
- [ ] 结构审视无未解决 blocker
- [ ] 用户验收标准全部满足
- [ ] 当前子任务目录归档时，父 roadmap 已同步回写 phase-0 完成状态与下一 child
