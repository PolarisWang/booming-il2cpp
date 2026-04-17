# test-driven-development

> 项目中的 TDD 基线，要求功能改动、规则调整和 bug 修复先有失败测试，再改实现。

## 基本信息

- 分层：质量保障
- 项目职责：为代码、skill、planner、runner 和 dashboard 改动提供红绿重构纪律
- 实现路径：`.codex/skills/dev-test-driven-development/SKILL.md`
- 状态：`project`

## 本项目补充规则

- IL2CPP compiler / loader / linker / semantic world / codegen 改动，必须先补失败测试。
- `Chaos.TestFramework.Sdk` / `Chaos.TestFramework.Runtime` 改动，必须先补失败测试。
- UnitTest / Benchmark / HotUpdate collection schema、loader、runner、dashboard 改动，必须先补失败测试。
- subject planner、generated solution pipeline、native/hotupdate test 主线改动，必须先补失败测试。

## 测试优先级

1. `tests/unit/**` 下的 Python 模板化单测
2. `tests/contracts/**` 下的 contract / snapshot / schema 测试
3. `tests/integration/**` 下的端到端流程测试
4. 必要时新增 canonical template 或 canonical subject

## 仓库约束

- Python 测试优先模板化，尽量少保留手写 C# fixture。
- 手工 benchmark、dashboard 结果和控制台输出都不能替代自动化测试。
- managed 层测试结果优先使用 `Chaos.TestFramework.Sdk.Assert`，不要依赖 `Console.WriteLine` 再外部判定。

## 最近变更

- `2026-04-17`：补充仓库级 TDD 规则，正式覆盖 `Chaos.TestFramework`、collection file、generated runner、dashboard 和 IL2CPP codegen 主线。
