---
name: dev-project-test-governance
description: Use when changing subject/test workflow, Chaos.TestFramework, generated managed/native/hotupdate runners, benchmark or unit verification flow, or project codegen structure in this repository
---

# 项目测试治理

## 概览

这个技能用于约束仓库里的 `subjects/test/runner/codegen/verification` 主链改动。
它不代替长期 authority 文档，但会强制任务在进入设计、实现和完成前回到正式约束。

## Authority 边界

- [`docs/archive/architecture/subject-test-framework-v1/INDEX.md`](../../../docs/archive/architecture/subject-test-framework-v1/INDEX.md)
- [`docs/archive/architecture/verification-v1/spec.md`](../../../docs/archive/architecture/verification-v1/spec.md)
- [`wiki/06-测试验证/INDEX.md`](../../../wiki/06-测试验证/INDEX.md)
- [`wiki/06-测试验证/AOT新Feature接入自测规范.md`](../../../wiki/06-测试验证/AOT新Feature接入自测规范.md)
- [`wiki/06-测试验证/CodeGen快照测试规范.md`](../../../wiki/06-测试验证/CodeGen快照测试规范.md)

## 何时必须使用

以下任务在进入计划或实现前必须先使用本技能：

- 调整 `subjects/` 下的测试入口、runner、registry、生成入口或统一命令
- 修改 `Chaos.TestFramework`
- 修改 managed/native/hotupdate 测试主线
- 修改 UnitTest / Benchmark / HotUpdate collection 的生成或消费逻辑
- 修改 manifest、collector、dashboard、generated codegen
- 新增或调整 AOT / IL2CPP feature onboarding、owner subject、proof / benchmark obligation、formal verification gate

## AOT / obligation intake

进入计划或实现前，必须显式冻结这些字段：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

不允许只写“后面跑测试看看”或“按实现时决定”。

## 强制规则

### 1. 先有自动化测试，再动实现

- bugfix 不能只靠手工复跑、dashboard 观察或控制台输出
- 证据顺序默认是：
  1. `tests/unit/**`
  2. `tests/contracts/**`（含快照测试）
  3. `tests/integration/**`
  4. subject 级正式验证

### 2. `subject.features.json` 是 owner / obligation authority

- `subject.features.json` 是 completed feature、owner subject、proof / benchmark obligation authority
- canonical proof / benchmark / host correctness 不能依赖 `Console.WriteLine` 或 `ChaosEvidenceKind.Stdout`
- AOT onboarding 细则以 [`wiki/06-测试验证/AOT新Feature接入自测规范.md`](../../../wiki/06-测试验证/AOT新Feature接入自测规范.md) 为准

### 3. 测试阶段的 `dotnet` 编译崩溃是 blocker

只要 `dotnet build` / `dotnet test` / `msbuild` 在测试阶段崩溃：

- 当前验证立即视为失败
- 必须保留并检查 `stderr`、`binlog`、堆栈或 dump
- 必须走 `dev-systematic-debugging`
- 根因未修复前，不得归档、提交或宣称通过

### 4. pipeline / codegen 不能漂移

- 正式主链固定为 `managed solution -> collector -> collection -> managed/native/hotupdate hosts`
- native / hotupdate 专有绑定必须分层，不污染 collection contract
- file-level codegen 默认 Scriban
- cutover 完成后要删除旧 alias、旧命名和旧双轨逻辑

### 5. 新验证记录与新数据必须落地

- 如果任务目标包含“打通新的测试流程 / 需要新的验证通过记录 / 需要新的 verification 数据”，或本轮改动触及 `subjects/test/runner/benchmark/codegen` 主线，则不能只停在 formal object 跑通
- formal object 与受影响 regression 通过后，默认还要执行 `run test inventory --json`
- 如果本轮改动触及 formal report / projection contract，例如 `Program / DLL / Verification Project / Artifact` 报告对象、`latest/master/reports` 字段、`testing-inventory` / `benchmark` 页面字段，或证据链接规则，也必须刷新 unified test framework 正式产物
- `run test inventory` 只是内部命令，不是 public verification entry
- `benchmark --record` 只写 raw benchmark records，不等于 formal refresh
- 至少确认本轮更新：
  - `verification/archive/latest/*`
  - `verification/archive/master/*`
  - `verification/archive/reports/<closure-kind>/<scope>/summary.md`
  - 命中 projection / report contract 时，对应 `verification/projections/**`
  - 命中 codegen 主线时，`verification/evidence/owners/*/codegen-stubs/*`
- `artifacts/**` 下的 `summaryPath` / `eventsPath` / `consolePath` 只是过程证据，不等于新的 formal verification 数据

### 6. CodeGen 快照测试

以下改动必须先过快照测试：

- 修改 `NativeAotLoweringPlanner` 的任何 Emission / Planning / RuntimeSupport 文件
- 修改 `NativeAotEmitter` 核心逻辑
- 新增或修改 Scriban 模板
- 其他会改变生成 C++ 形状的改动

快照测试的夹具（Fixtures）和基线（Baselines）必须随实现代码一同维护。

- 新增 IL 模式时必须补对应夹具和基线
- 修改生成逻辑后必须运行快照测试，审查基线变更后再更新
- 夹具须遵循精简原则（1-3 种 IL 模式、1-3 个方法）
- 基线更新通过 `SNAPSHOT_UPDATE=1` 环境变量触发

详见 [`wiki/06-测试验证/CodeGen快照测试规范.md`](../../../wiki/06-测试验证/CodeGen快照测试规范.md)。

### 7. DLL-first reporting 的 primary evidence 约束

- 只有 `artifacts/**` 下的真实产物允许进入 primary evidence / artifact index
- `docs/**`、`subjects/**`、`verification/**` 等引用只能作为 support refs
- support refs 不能把项目状态置为 `passed`，也不能混入 primary artifact table

## 输出要求

如果本次任务改变了长期规则，至少同步更新：

- `docs/archive/architecture/subject-test-framework-v1/INDEX.md`
- `docs/archive/architecture/verification-v1/spec.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- `wiki/06-测试验证/CodeGen快照测试规范.md`
- `wiki/06-测试验证/INDEX.md`
- `wiki/02-Skill体系/04-质量保障/project-test-governance.md`
- `wiki/02-Skill体系/skill-registry.md`

## 关联技能

- 上游：`brainstorming`、`systematic-debugging`
- 协作：`test-driven-development`、`project-wiki-maintenance`
- 下游：`writing-plans`、`executing-plans`、`verification-before-completion`
