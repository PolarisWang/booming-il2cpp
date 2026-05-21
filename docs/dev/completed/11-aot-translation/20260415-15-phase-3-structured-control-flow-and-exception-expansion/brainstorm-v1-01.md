# Phase 3 Structured Control Flow And Exception Expansion Brainstorm v1.01

Date: 2026-04-15 22:04:44 +08:00
Status: active

## 背景

- 父 roadmap 已完成 Phase 0-2，当前 native AOT 的主缺口从 scalar carrier 转为 structured EH / control-flow。
- catch-only minimal exception shape 已经绿色；Phase 3 的目标是继续往 finally/filter/leave/switch 扩。

## 当前事实

- `FinallyAndFilterProofEntry::Run()` 是最接近真实 owner subject 的 retained proof。
- 当前 proof 在 native AOT 路径先撞到 `ArgumentException.get_ParamName()` 的 unresolved dispatch slot，而不是 finally/filter 本身。
- 这说明 proof 本身带入了与 Phase 3 无关的 dispatch 噪音。

## 结论

- Phase 3 Batch A 先把 owner proof 去噪，得到 dispatch-free finally/filter 形状。
- 随后用这个 proof 推进 planner 对 `finally / filter / leave / endfinally` 的 widening。
- `switch` 与更广义的 structured branch 作为同一 child task 的下一批，但不在 Batch A 开始时混入。
