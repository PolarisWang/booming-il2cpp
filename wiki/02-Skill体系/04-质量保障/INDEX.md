# 04-质量保障 INDEX

> 收录项目中负责测试、调试、验证和测试治理的 skill。

## 重要文档

| 文档 | 主题 | 说明 |
| --- | --- | --- |
| [`project-test-governance.md`](./project-test-governance.md) | 测试治理 | 统一 `managed -> collection -> managed/native/hotupdate` 主线、`Chaos.TestFramework` 分层与 codegen 约束 |
| [`test-driven-development.md`](./test-driven-development.md) | TDD | 说明功能改动、流程修复和规则调整都要先有失败测试 |
| [`systematic-debugging.md`](./systematic-debugging.md) | 调试 | 说明根因优先与证据优先 |
| [`verification-before-completion.md`](./verification-before-completion.md) | 验证 | 说明完成前必须有自动化或命令级证据 |

## 本层规则

- 质量保障 skill 负责“先补测试、再改实现、最后验证”。
- `project-test-governance` 是本仓库测试主线与测试框架的正式治理入口。
- `subjects/`、`tests/`、`Chaos.TestFramework`、collection file、runner、dashboard、generated codegen 相关任务，必须优先参考本层。

## 最近更新

- `2026-04-17`：新增 `project-test-governance`，把统一测试主线和 Scriban/codegen 规则正式纳入质量保障层。
