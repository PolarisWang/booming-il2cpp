---
name: test-driven-development
description: 在实现任何功能、修复任何 bug、调整任何长期规则之前使用，先补失败测试，再改实现
---

# 测试驱动开发

## 核心纪律

先写会失败的自动化测试，再写实现。

如果你没有亲眼看到测试先失败，再因为你的改动变绿，就不要声称这次改动是可靠的。

## 边界

本技能只拥有 RED-GREEN-REFACTOR 纪律。

以下 authority 不由本技能定义，而是由 `dev:project-test-governance` 与 `wiki/06-测试验证/AOT新Feature接入自测规范.md` 持有：

- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

## 使用时机

- 新功能
- bug 修复
- 回归修复
- 重构导致的行为变化
- skill / planner / runner / dashboard / schema 的长期规则调整

## 仓库补充规则

### 1. 以下改动必须先补失败测试

- IL2CPP compiler / loader / linker / semantic world / codegen
- `Chaos.TestFramework.Sdk` / `Chaos.TestFramework.Runtime`
- UnitTest / Benchmark / HotUpdate collection file schema、loader、runner
- benchmark dashboard 数据聚合或展示逻辑
- subject planner、subject discovery、generated solution pipeline

### 2. 测试优先级

1. `tests/unit/**` 下的 Python 模板化单测
2. `tests/contracts/**` 下的 contract / snapshot / schema 测试
3. `tests/integration/**` 下的端到端流程测试
4. 必要时新增 canonical template 或 canonical subject

### 3. 手工结果不能替代测试

- 手工 benchmark、dashboard 结果和控制台输出都不能替代自动化测试
- managed 层验证优先使用 `Chaos.TestFramework.Sdk.Assert`
- 不要依赖 `Console.WriteLine` 再做外部判定

### 4. 测试阶段遇到 `dotnet` 编译崩溃时，必须先查根因

- `dotnet build` / `dotnet test` / `msbuild` 在测试阶段崩溃，视为阻断性失败
- 先保留失败的 project / target / task、退出码、`stderr`、`binlog` 与崩溃堆栈或 dump 信息（如果可用）
- 立即进入 `dev:systematic-debugging`
- 根因未修复前，不得继续后续测试

## 最小循环

1. 写失败测试
2. 运行，确认是因为目标能力缺失而失败
3. 写最小实现
4. 运行，确认目标测试变绿
5. 必要时重构，并保持测试继续变绿

## 红旗

- 先改实现，之后再补测试
- 只补手工脚本，不补自动化用例
- 只修 dashboard 显示，不补 collection 或数据生产端测试
- 只看 benchmark 页面，不验证数据生产链路
- `dotnet` 编译崩溃后直接重试或把它降级成“环境偶发问题”
