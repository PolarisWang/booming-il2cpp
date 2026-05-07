# test-driven-development

> 项目中的 TDD 基线，要求功能改动、规则调整和 bug 修复先有失败测试，再改实现。

## 基本信息

- 分层：质量保障
- 项目职责：提供 RED-GREEN-REFACTOR 纪律，不持有 owner / obligation authority
- 实现路径：`.codex/skills/dev-test-driven-development/SKILL.md`
- 状态：`project`

## 本项目补充规则

- IL2CPP compiler / loader / linker / semantic world / codegen 改动，必须先补失败测试
  - 对于 codegen 改动，除了单元测试外，应考虑补快照测试（详见 [`../../06-测试验证/CodeGen快照测试规范.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/CodeGen%E5%BF%AB%E7%85%A7%E6%B5%8B%E8%AF%95%E8%A7%84%E8%8C%83.md)）
- `Chaos.TestFramework.Sdk` / `Chaos.TestFramework.Runtime` 改动，必须先补失败测试
- UnitTest / Benchmark / HotUpdate collection schema、loader、runner、dashboard 改动，必须先补失败测试
- subject planner、generated solution pipeline、native/hotupdate test 主线改动，必须先补失败测试
- 手工 benchmark、dashboard 结果和控制台输出都不能替代自动化测试
- 测试阶段遇到 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃时，必须先查根因并修复

## Authority 划分

以下字段不由本 skill 定义，而由 `project-test-governance` 与 `wiki/06-测试验证/AOT新Feature接入自测规范.md` 持有：

- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

## 最近变更

- `2026-04-18`：明确 TDD 只负责红绿重构纪律，不再承担 owner / obligation authority。
