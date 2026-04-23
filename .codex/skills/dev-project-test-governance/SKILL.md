---
name: project-test-governance
description: Use when changing subject/test workflow, Chaos.TestFramework, generated managed/native/hotupdate runners, benchmark or unit verification flow, or project codegen structure in this repository
---

# 项目测试治理

## 概览

这是本仓�?`subjects/test/Chaos.TestFramework/collection/runner/codegen` 主线的治理入口，不负责重复抄写全�?authority 文档�?

本技能负责两件事�?

- 识别当前任务是否命中测试治理 / AOT obligation 主线
- 强制任务回到正式 authority、planning intake �?completion gate

## 文档语言要求

除非用户明确要求其他语言，所有由本技能产出的设计文档、计划、roadmap、wiki、规范说明默认使用中文。代码、命令、路径、标识符保持原文�?

## Authority 边界

- [`docs/architecture/subject-test-framework-v1/INDEX.md`](../../../docs/architecture/subject-test-framework-v1/INDEX.md)
  - 拥有统一测试主线、collection/manifest/codegen 分层�?subject test framework 的长期边�?- [`docs/architecture/verification-v1/spec.md`](../../../docs/architecture/verification-v1/spec.md)
  - 拥有 formal verification、归并归档、projection 与权责图审核边界
- [`wiki/06-测试验证/INDEX.md`](../../../wiki/06-测试验证/INDEX.md)
  - 拥有正式验证入口、对象导航与 completion 前的对象优先�?- [`wiki/06-测试验证/AOT新Feature接入自测规范.md`](../../../wiki/06-测试验证/AOT新Feature接入自测规范.md)
  - 拥有 capability intake、owner subject、proof/benchmark/hotupdate obligation、formal verification 顺序
- 本技能本身：
  - 只负责把任务路由到这�?authority，并要求计划和执行阶段消费它�?

## 使用时机

以下任务必须先使用本技能：

- 调整 `subjects/` 的测试入口、runner、registry、generated solution 或统一入口
- 修改 `Chaos.TestFramework`
- 修改 managed/native/hotupdate 测试主线
- 修改 UnitTest / Benchmark / HotUpdate collection 的生成或消费逻辑
- 修改 native dispatch manifest �?hotupdate binding manifest
- 修改 benchmark 数据生成与展示逻辑
- 修改生成整文件的 test/codegen emitter
- 新增或调�?AOT / IL2CPP feature onboarding、owner subject、proof / benchmark obligation、formal verification gate

## 命中 AOT / IL2CPP / test-flow 任务时，先冻�?intake

进入计划或实现前，必须显式冻结以下字段：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

不允许只写“后面跑测试看看”或“按实现时决定”�?

## 强制规则

### 1. 先有自动化测试，再动实现

- bugfix 不能只靠手工复跑、dashboard 或控制台观察
- 优先证据顺序固定为：
  1. `tests/unit/**`
  2. `tests/contracts/**`
  3. `tests/integration/**`
  4. subject 级正式实�?

### 2. `subject.features.json` �?owner / obligation authority

- `subject.features.json` �?completed feature �?owner subject �?proof / benchmark obligation authority
- canonical proof / benchmark / host correctness 不允许依�?`Console.WriteLine` �?`ChaosEvidenceKind.Stdout`
- AOT onboarding 的深规则�?`wiki/06-测试验证/AOT新Feature接入自测规范.md` 为准

### 3. 测试阶段 `dotnet` 编译崩溃是阻断缺�?

在任何测试阶段，只要 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃�?

- 当前验证立即视为失败
- 必须保留并检�?`stderr`、`binlog`、崩溃堆栈或 dump 信息（如果可用）
- 必须�?`dev:systematic-debugging` 查明根因并修�?
- 根因未修复前，不得继续归档、提交或宣称通过

### 4. pipeline / codegen 不变�?
- 正式主线固定�?`managed solution -> collector -> collection -> managed/native/hotupdate hosts`
- native / hotupdate 专有绑定必须�?manifest 分层，不污染 collection contract
- file-level codegen 默认 Scriban
- cutover 完成后删除旧 alias、旧命名和旧双轨逻辑

### 5. 新验证记录与新数据必须落�?
- 如果任务目标包含“与新测试流程打通”、需要新的验证通过记录、需要新�?verification 数据，或本轮改动触及 `subjects/test/runner/benchmark/codegen` 主线，则不能只停�?formal object 跑�?- formal object 与受影响 regression 通过后，必须刷新 `verification-v1` 正式产物；默认命令为 `run verify verification-v1 --json`
- 至少确认本轮生成或更新：
  - `verification/archive/latest/*`
  - `verification/archive/master/*`
  - `verification/archive/reports/<closure-kind>/<scope>/summary.md`
  - `verification/evidence/owners/*/codegen-stubs/*`（命�?codegen 主线时）
- `artifacts/**` 下的 `summaryPath` / `eventsPath` / `consolePath` 只是过程证据，不等于新的 formal verification 数据

## 输出要求

如果本次任务改变了长期规则，至少同步更新�?
- `docs/architecture/subject-test-framework-v1/INDEX.md`
- `docs/architecture/verification-v1/spec.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- `wiki/06-测试验证/INDEX.md`
- `wiki/02-Skill体系/04-质量保障/project-test-governance.md`
- `wiki/02-Skill体系/skill-registry.md`

## 关联技�?

- 上游：`brainstorming`、`systematic-debugging`
- 协作：`test-driven-development`、`project-wiki-maintenance`
- 下游：`writing-plans`、`executing-plans`、`verification-before-completion`

