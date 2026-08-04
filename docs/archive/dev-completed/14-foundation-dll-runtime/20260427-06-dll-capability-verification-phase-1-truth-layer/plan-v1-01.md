# DLL Capability Verification Phase 1 Truth Layer Plan

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。本任务是 `20260427-04` 的 roadmap child，完成时必须先归档当前子任务目录，再回写父任务。

**目标：** 落地 Truth Layer 正式对象、目录与 skeleton 生成链，使新增 DLL 进入 scope 时 capability / family / method authority 一次性冻结。

**架构：** 本阶段实现 Truth Layer 的正式对象与生成器，不进入 benchmark / hotupdate / review bundle 的执行逻辑，只为后续 Evidence/Execution/Projection 提供唯一输入。

**技术栈：** Python generators / JSON contract / existing foundation-dll audit pipeline / verification-v1 authority

**架构审核模式：** critical

**结构告警重点：** denominator authority 去重、Truth 与 Evidence 边界、现有 family ledger 与新 contracts 的兼容迁移、旧推断逻辑回收

**权责图审核主题：** Truth Layer authority and denominator freeze

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: `foundation-dll-verification-closure`
- capabilityItem: `truth-layer-authority`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects:
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/foundation-dll-capability-verification-closure-v1.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
  - `verification/projections/foundation-dll-audit/*.json`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

**设计文档：** n/a

**问题清零来源：** `docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm/brainstorm-v1-01.md`

**计划来源：** `roadmap-child-auto`

**预期知识沉淀：**
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/foundation-dll-capability-verification-closure-v1.md`

**收尾约束：** 完成后必须进入“结构告警与架构审视 -> 测试通过 -> 归档 completed -> 回写父 roadmap”固定链路。

## 文件与职责

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/foundation-dll-capability-verification-closure-v1.md`
  - Truth Layer authority
- `build/toolchains/run/testing/foundation_dll/**`
  - family claim / verification kernel / future Truth Layer generators
- `build/toolchains/run/testing/foundation_dll_audit_generator.py`
  - current projection consumer that must be adapted to Truth Layer contracts
- `verification/foundation-dll/**`
  - new truth-layer skeleton output root

## 执行步骤

- [ ] 审查现有 foundation-dll family ledger、claims snapshot、kernel 输入之间的 truth overlap
- [ ] 设计并落地 `DllCapabilityManifest` 数据结构
- [ ] 设计并落地 `CapabilityFamilyVerificationContract` 数据结构
- [ ] 设计并落地 `MethodCapabilityContract` 数据结构
- [ ] 定义 Truth Layer 输出目录与文件命名 contract
- [ ] 为新增 DLL 生成 truth skeleton 的 generator 入口
- [ ] 为新增 family 生成 truth skeleton 的 generator 入口
- [ ] 为新增 method universe 生成 `MethodCapabilityContract` skeleton
- [ ] 让 Truth Layer 能与当前 foundation-dll capability/family authority 对齐，不重复维护 denominator
- [ ] 为后续 Phase 2 预留 annotation 生成所需的最小字段
- [ ] 识别并列出需要在后续阶段清理的旧 truth 推断逻辑
- [ ] 更新当前子任务 `STATUS.md` 与父任务 `STATUS.md`

## 本阶段验收标准

### 自动化/结构验收

- [ ] `DllCapabilityManifest` contract 明确存在
- [ ] `CapabilityFamilyVerificationContract` contract 明确存在
- [ ] `MethodCapabilityContract` contract 明确存在
- [ ] 新增 DLL truth skeleton 可以生成
- [ ] 新增 family truth skeleton 可以生成
- [ ] capability / family / method denominator authority 没有重复来源

### 用户验收

- [ ] 用户可以直接看到新增 DLL 时会生成哪些 truth 对象
- [ ] 用户可以直接看到每个 family / method 的 authority 从哪里来
- [ ] 用户可以直接看到后续 test / benchmark / hotupdate case 会消费哪些 truth 字段

## 验证命令

- `python -m pytest tests/unit/compatibility/test_declared_metadata_discovery_contract.py -q`
- `python build/toolchains/run/run.py verify verification-v1 --json`

## 归档条件

- [ ] 本计划所有复选框完成
- [ ] 结构审视无未解决 blocker
- [ ] 用户验收标准全部满足
- [ ] 当前子任务目录归档时，父 roadmap 已同步回写 phase-1 完成状态与下一 child
