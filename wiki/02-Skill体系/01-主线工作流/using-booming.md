# using-booming

> 项目中的全局 skill 入口，负责在任何行动前先决定应该走哪条技能链。

## 基本信息

- 分层：主线工作流
- 项目职责：统一 skill 分流，避免跳过测试治理、计划 intake、formal verification 与完成前验证流程
- 实现路径：`.codex/skills/dev-using-booming/SKILL.md`
- 状态：`active`

## 在本项目中的特殊规则

- 只要任务涉及 `subjects/`、`tests/`、`Chaos.TestFramework`、collection file、manifest、runner、dashboard、generated codegen，就必须先走 [`project-test-governance`](../04-%E8%B4%A8%E9%87%8F%E4%BF%9D%E9%9A%9C/project-test-governance.md)。
- AOT / IL2CPP 新 feature 接入，以及 `ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact`、`formalVerificationObjects`、`requiredGates` 之类的 obligation 变更，也必须先走测试治理入口。
- bug、回归、benchmark 异常或测试阶段 `dotnet` 编译崩溃，必须优先走 `systematic-debugging`。
- 进入正式任务主线后，由计划/执行技能负责维护 `docs/dev/ACTIVE.md` 和任务状态。

## 常见分流

- 新功能、规范、架构设计：`brainstorming -> writing-plans`，复杂主线转 `roadmap`
- 继续执行现有计划：`executing-plans`
- bug / 回归 / benchmark 异常 / `dotnet` 编译崩溃：`systematic-debugging -> project-test-governance -> test-driven-development`
- AOT 新 feature / owner subject / proof-benchmark obligation 调整：`project-test-governance -> writing-plans`
- 测试主线、subject、runner、codegen 调整：`project-test-governance -> writing-plans` 或 `executing-plans`

## 最近变更

- `2026-04-18`：扩展测试治理触发条件，新增 AOT obligation 字段与 formal verification gate 的显式分流。
