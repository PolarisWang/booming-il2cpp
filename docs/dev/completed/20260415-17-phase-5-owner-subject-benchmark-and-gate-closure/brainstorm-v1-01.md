# Phase 5 Owner Subject / Benchmark / Gate Closure Brainstorm v1.01

Date: 2026-04-15 23:18:43 +08:00
Status: active

## 1. 问题

Phase 1-4 已把 typed-il -> AOT core IR -> planner -> emitter 的 widened capability 打通，但如果 owner subject、benchmark/dashboard、unsupported/reporting、release gate 不同步收口，外层工程证据会继续失真。

## 2. 已知现状

- `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 的 capability owner 映射已经需要稳定化。
- benchmark dashboard 需要理解 capability contract，而不是只看运行记录。
- unsupported report 与 release evidence 需要共享同一份 contract 分类，不应各自维护字符串规则。

## 3. 本阶段边界

- 不再回到底层扩新的 AOT translation surface。
- 只做 owner evidence 收口、benchmark/dashboard 展示收口、unsupported/reporting/release gate 收口。
- 继续保持 `Chaos.TestFramework` 与 AOT core/planner/emitter 解耦。

## 4. 预期结果

- capability contract 能稳定给出 `ownerSubjectId`、`supportStates`、`proofRequired`、`benchmarkRequired`。
- benchmark/dashboard/registry/reporting/release evidence 基于同一 contract 工作。
- 新增 widened capability 在 owner subject、benchmark 与 gate 上都有可验证证据。
