---
task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
title: HybridCLR 对齐的 AOT 与热更开发计划
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-14 23:34:39 +08:00
updated_at: 2026-04-14 23:34:39 +08:00
current_dir: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
parent_task_id:
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: follow-up-after-core-capability-roadmap
active: true
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- review: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/review-v1-01.md`

## 当前结论

- 长期主线不再是“继续给当前 `NativeAotEmitter` 补 opcode”，而是转向 HybridCLR 对齐的混合执行架构。
- `typed-il` 必须升级为共享输入契约，同时服务 AOT codegen、解释执行、bridge/thunk 生成和 supplemental metadata。
- `subjects/` 内的源码继续收敛为 solution 模式；历史语法点按单文件机械拆分，但尽量归并到少量 owner project 中，而不是继续膨胀顶层 subject。
- `ChaosUnitTest` / `ChaosBenchmark` 将作为 proof / benchmark 的声明入口；测试框架引用路径固定在 `src/reference/Chaos.TestFramework`。
- 继续压缩 string 协议面：公共契约优先使用紧凑枚举和稳定 numeric identity，不继续扩大 subject 内部字符串调度。
- 不能只规划“已支持能力”；Phase 0 必须同时冻结 `aot-translation-surface-ledger` 与 `aot-unsupported-surface-ledger`。
- Q0-Q8 已冻结为默认决策，当前剩余问题不再是架构路线问题，而是实现拆批和落地细节问题。

## 最近摘要

- 2026-04-14 23:34:39 +08:00: 基于前序讨论，新建 follow-up roadmap，用于统一整理 AOT、热更、subject 组织和 capability 覆盖的后续开发计划。
- 2026-04-14 23:34:39 +08:00: 当前阶段先产出正式文档，不进入实现；下一步转入 Phase 0，先讨论 Q0 reference bundle 与 Q1 二级 capability taxonomy。
- 2026-04-15 00:08:00 +08:00: 已将 Q0-Q8 冻结进 roadmap，并补充 `review-v1-01.md`，明确阶段任务清单、验证出口与测试工程新增能力。

## 下一步

- next_action: 按 `review-v1-01.md` 进入 Phase 0，先落 `reference bundle`、taxonomy、stable id、body availability 与 AOT translation surface 双台账。
- owner: codex
- trigger: 用户确认文档方向后，开始 Phase 0 讨论。

## 风险 / 阻塞

### risks

- 如果 Phase 0 不先冻结 reference bundle、capability taxonomy 和 stable identity，后续实现仍会继续边做边改契约。
- 如果 subject 继续用字符串入口扩张内部调度面，后续 benchmark/proof/native host 会继续出现 identity 漂移和 pipeline 分叉。

### blockers

- 当前无外部 blocker；等待进入 Phase 0 对齐。

## wiki

- 本任务当前仅新增 `docs/dev` 规划文档，暂不修改 `wiki/`。
