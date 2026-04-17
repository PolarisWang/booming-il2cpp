---
task_id: 20260417-04-aot-skill-flow-and-token-optimization-plan
title: AOT Skill Flow And Token Optimization Plan
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 23:22:10 +08:00
updated_at: 2026-04-17 23:54:00 +08:00
latest_stop_point: authority/docs/skill optimization is complete; validation passed and the task has been archived to completed
current_dir: docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan
parent_task_id:
source_task_id: 20260417-01-test-process-and-skill-governance
source_relation: follow-up-on-aot-skill-flow-review-and-token-budget-audit
design_doc: docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/design-v1-01.md
plan_doc: docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/plan-v1-01.md
total_tasks: 6
current_task: all-tasks-complete
active: false
---

## 关键文档

- `docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/design-v1-01.md`
- `docs/dev/completed/20260417-04-aot-skill-flow-and-token-optimization-plan/plan-v1-01.md`
- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- `wiki/02-Skill体系/04-质量保障/project-test-governance.md`
- `docs/dev/completed/20260417-01-test-process-and-skill-governance/STATUS.md`

## 当前结论

- authority 分工已冻结：
  - `docs/architecture/managed-native-hotupdate-test-pipeline.md` 拥有 pipeline / layering / contract 边界
  - `wiki/06-测试验证/AOT新Feature接入自测规范.md` 拥有 capability intake、owner/proof/benchmark/hotupdate obligation 与 formal verification 顺序
  - `wiki/06-测试验证/INDEX.md` 拥有正式验证入口、对象导航与 completion 前对象优先级
- `writing-plans` 已把 `capabilityFamily`、`capabilityItem`、`ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact`、`formalVerificationObjects`、`requiredGates` 纳入显式 intake contract。
- `executing-plans`、`subagent-driven-development` 与 `verification-before-completion` 已把 obligation-driven formal verification gate 接入 `completed` 判定。
- 高频 skill 热路径总体已从 `18380` 字符降到 `17088` 字符，净减少 `1292` 字符，约 `7.0%`。

## 最近摘要

- 2026-04-17 23:54:00 +08:00: 已完成 authority、skill、wiki 镜像与索引改造；文本级验证、关键词链路校验与 3 组 scenario-based 审查通过，任务进入 completed 收口。
- 2026-04-17 23:41:13 +08:00: 用户要求按 `20260417-04` 计划直接执行到底；任务已从 `planning` 切到 `executing`，当前批次先补执行态并冻结 authority / obligation intake 基线。
- 2026-04-17 23:22:10 +08:00: 基于 AOT skill 流程审查，冻结 follow-up 计划，目标是把 capability/owner/proof/benchmark obligation、formal verification gate 与 token slimming 三条线收成一套可执行改造。

## 下一步

- next_action: 本任务已完成；后续如需 git 收口，进入 `dev:finishing-a-development-branch`。
- owner: codex
- trigger: completed 归档后的本地提交与分支收口

## 风险 / 阻塞

### risks

- 本次完成判断基于 skill / wiki / architecture 的文本级验证与 scenario 审查，不包含任何新的 IL2CPP 运行时代码回归。
- 当前仓库仍存在用户侧并行脏改动；若继续做 git 收口，必须避免把无关改动一并提交。

### blockers

- 当前无硬阻塞。
- 说明: 本任务是规则与文档治理改造，不依赖当前 AOT 代码主线收口完成；但仓库存在用户侧并行脏改动，实施时必须避免覆盖无关文件。

## 验证

- 文本一致性：
  - `git diff --check -- .codex/skills docs/architecture wiki docs/dev`
  - 结果：通过；无 whitespace error，仅有 Git 行尾转换警告
- 关键词链路：
  - 已确认 `ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact`、`formalVerificationObjects`、`requiredGates` 贯通到高频 skill 与 authority 文档
  - 已确认 `collector -> registry -> workspace`、`managed proof`、`native proof`、`hotupdate proof`、`canonicalCommand`、`required-before-completion` 的关键链路存在
- token 预算复算：
  - 热路径组合：`using-booming + project-test-governance + writing-plans + executing-plans + verification-before-completion + test-driven-development`
  - 改造前：`18380` 字符
  - 改造后：`17088` 字符
- scenario-based 审查：
  - 场景 1：新增 AOT feature，需 owner/proof/benchmark/hotupdate obligation
  - 场景 2：benchmark 异常排查，需 routing 到 debugging + governance + TDD
  - 场景 3：formal verification object 缺失时尝试归档 `completed`
  - 结果：三组场景均能从当前 skill 与 authority 文档得到明确 routing / gate / stop 条件

## wiki

- 本轮 wiki 更新已完成
- 范围: `wiki/06-测试验证/`、`wiki/02-Skill体系/` 与 `skill-registry`
- 原因: 当前执行内容属于长期有效的 authority / skill 规则沉淀
