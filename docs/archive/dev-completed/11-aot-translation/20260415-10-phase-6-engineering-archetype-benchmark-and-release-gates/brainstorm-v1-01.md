# Phase 6 Engineering Archetype Benchmark And Release Gates Brainstorm v1.01

Date: 2026-04-15 20:25:00 +08:00

## 问题定义

Phase 5 已经把 capability truth 收口到 retained owner subjects，但 benchmark/dashboard/unsupported report/release evidence 仍然沿用旧 gate 假设：

- diagnostics `47/48/49/50` 需要按 `proofOptional` reporting contract 消费，而不是继续显示为 subject proof 缺口。
- benchmark 页面需要基于新的 owner subject、archetype、managed baseline 与 support-state truth 展示结果。
- release evidence 需要以新的 full-solution + owner-subject gate 为准，而不是历史零散基线。

## 目标拆解

1. benchmark / dashboard 真值统一
   - 让 benchmark 页面直接消费 registry + capability coverage 真值。
   - 缺数据时明确展示缺失原因、适用 execution mode 与 support-state。
2. unsupported / deferred 语义统一
   - diagnostics proof-optional contract 进入 unsupported report，而不是错误计入 proof gap。
   - reporting 输出需要区分 `missingProof`、`missingBenchmark`、`proofOptional`、`unsupportedState`。
3. release evidence 统一
   - engineering archetype gate 需要能覆盖 full-solution archetype、owner-subject declared benchmark 与最终 release evidence summary。

## 方案判断

- 采用方案：先改 reporting/data contract，再改 dashboard 布局与 benchmark gate，最后收口 release evidence。
- 不采用方案：直接改页面样式而不先改数据语义；那会继续把旧 contract 误显示成新真值。

## 执行提示

- Phase 6 的首要输入是 `build/toolchains/run/testing/capability_coverage.py` 与 registry declared metadata。
- 不要在 reporting 层重新发明 capability ownership / support-state 规则。
- benchmark 页面默认以 `managed` 为 baseline 展示，对 `native` / `interpreter` 给出相对对比与缺失原因。
