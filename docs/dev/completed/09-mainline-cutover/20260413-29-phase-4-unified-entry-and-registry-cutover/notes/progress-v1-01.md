# 进度记录 v1.01

Date: 2026-04-13 12:14:06 +08:00

## 本次完成

- 激活 Phase 4 子任务并对齐父 roadmap 状态。
- 明确本阶段边界：
  - 做统一入口与 registry cutover
  - 不删除 compatibility alias
  - 不提前展开 `tests/` vs `subjects/` 最终 cleanup
  - 不把 mixed native benchmark 收口扩大成通用 backend 重构

## 当前结论

- 三个 retained subject 已经拥有真实 proof / benchmark 运行证据，当前主要问题已经从“能不能跑”转为“入口与默认路由是否统一”。
- `tests/unit/planning/test_subject_planner.py` 之类仍带 subject-heavy 逻辑，是之前 roadmap 刻意后置的 cleanup 范围，不属于当前 Phase 4 的主线阻塞。

## 下一步建议

- 先做入口与 registry 差异审计，再直接进入 typed dispatch 实现。
