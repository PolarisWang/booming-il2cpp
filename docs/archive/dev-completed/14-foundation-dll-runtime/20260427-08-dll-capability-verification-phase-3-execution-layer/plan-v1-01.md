# DLL Capability Verification Phase 3 Execution Layer Plan

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。本任务是 `20260427-04` 的 roadmap child，完成时必须先归档当前子任务目录，再回写父任务。

**目标：** 生成完整的 family `test/native/benchmark/host/patch` 执行入口，并统一挂进正式主 `.sln`，同时提供 DLL / family 局部执行入口。

**架构：** 本阶段聚焦执行入口和 solution wiring，不重新定义 truth/evidence 规则；它消费已有 Truth Layer 和 Evidence Layer 产物，把生成结果变成可执行的正式主线。

**技术栈：** generated C# projects / `.sln` wiring / foundation-dll runners / existing Chaos.TestFramework entry patterns

**架构审核模式：** critical

**结构告警重点：** 单一正式主入口、防止 family 局部入口升级成新的 formal 主入口、generated project 与 existing tests/families 双轨残留、execution 与 projection 混层

**权责图审核主题：** Execution Layer formal main entry and local accelerator boundary

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `foundation-dll-verification-closure`
- capabilityItem: `execution-layer-main-entry`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects:
  - `subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln`
  - `verification/foundation-dll/**/test/`
  - `verification/foundation-dll/**/native/`
  - `verification/foundation-dll/**/benchmark/`
  - `verification/foundation-dll/**/host/`
  - `verification/foundation-dll/**/patch/`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

**设计文档：** n/a

**问题清零来源：** `docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm/brainstorm-v1-01.md`

**计划来源：** `roadmap-child-auto`

**预期知识沉淀：**
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/subject-test-framework-v1/INDEX.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/foundation-dll-capability-verification-closure-v1.md`

**收尾约束：** 完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 回写父 roadmap”固定链路。

## 文件与职责

- `subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln`
  - 正式主入口
- `verification/foundation-dll/**/test/`
  - functional execution sources / projects
- `verification/foundation-dll/**/native/`
  - native execution entry
- `verification/foundation-dll/**/benchmark/`
  - benchmark execution entry
- `verification/foundation-dll/**/host/`
  - hotupdate host entry
- `verification/foundation-dll/**/patch/`
  - hotupdate patch entry
- `build/toolchains/run/testing/foundation_dll/**`
  - solution / project generation and local execution hooks

## 执行步骤

- [ ] 审查正式主 `.sln` 当前状态与现有 foundation-dll family projects 的接线关系
- [ ] 定义并落地 family `test/` execution project 生成规则
- [ ] 定义并落地 family `native/` execution entry 生成规则
- [ ] 定义并落地 family `benchmark/` execution entry 生成规则
- [ ] 定义并落地 family `host/` execution entry 生成规则
- [ ] 定义并落地 family `patch/` execution entry 生成规则
- [ ] 把 generated family execution projects 挂进正式主 `.sln`
- [ ] 提供按 DLL 局部执行入口
- [ ] 提供按 family 局部执行入口
- [ ] 明确局部执行入口仅作开发加速，不作为新的 formal 主入口
- [ ] 识别并列出后续需要清理的旧 execution / old path 逻辑
- [ ] 更新当前子任务 `STATUS.md` 与父任务 `STATUS.md`

## 本阶段验收标准

### 自动化/结构验收

- [ ] 正式主 `.sln` 存在
- [ ] family `test/native/benchmark/host/patch` 执行入口都可生成
- [ ] 新增 DLL 时 execution layer 工程直接生成完整成品
- [ ] DLL / family 局部执行入口存在
- [ ] 局部执行入口没有成为新的 formal 主入口

### 用户验收

- [ ] 用户可以直接通过正式 `.sln` 找到执行入口
- [ ] 用户可以直接按 DLL 局部执行
- [ ] 用户可以直接按 family 局部执行
- [ ] 用户可以直接理解哪些入口是 formal、哪些只是加速入口

## 验证命令

- `python -m pytest tests/verification/unit/projections/foundation_dll/test_foundation_dll_test_code_generator.py tests/verification/unit/projections/foundation_dll/test_foundation_dll_gap_analyzer.py -q`
- `python build/toolchains/run/run.py verify verification-v1 --json`

## 归档条件

- [ ] 本计划所有复选框完成
- [ ] 结构审视无未解决 blocker
- [ ] 用户验收标准全部满足
- [ ] 当前子任务目录归档时，父 roadmap 已同步回写 phase-3 完成状态与下一 child
