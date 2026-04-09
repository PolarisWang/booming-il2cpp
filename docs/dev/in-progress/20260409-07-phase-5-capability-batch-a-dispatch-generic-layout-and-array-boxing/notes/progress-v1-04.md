# Progress v1.04

Date: 2026-04-09 21:34:59 +08:00

## 本轮更新

- 统一 perf 入口已执行完成：
  - 命令：`python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`
  - run id：`20260409-212758-windows-1a96`
  - run summary：`ok`
- 本轮 perf 关键结果：
  - `meanDurationMs = 52.327`
  - `maxDurationMs = 180.221`
  - baseline：`meanDurationMs = 50.0`，`maxDurationMs = 80.0`
  - compare 结果：`regressed`
- 样本拆开看更像“首样本冷启动 + 并发构建负载污染”：
  - 5 次样本：`180.221 / 29.14 / 27.339 / 13.214 / 11.723`
  - 去掉首样本后，剩余 4 次均值约 `20.354ms`
  - 去掉首样本后，剩余 4 次最大值约 `29.14ms`
- `current.json` 本轮已正确回写为 `status=ok`，说明前面修复的 reporting fallback 已生效。

## 当前判断

- `Phase 5` 的功能闭环、focused correctness、统一 perf 入口都已经落地。
- 还不能自动宣称“性能验证通过”，因为本次 perf compare 仍是 `regressed`。
- 结合用户明确要求“不要动正在进行的构建”，当前更合理的解释是：
  - 这次 run 足以证明 perf pipeline 可运行
  - 但不足以作为干净环境下的性能结论

## 需要用户确认

- 是否接受当前状态：
  - `Phase 5` 作为功能与 correctness 阶段完成
  - perf baseline 回归留作后续在空闲环境复核
- 如果不接受，则下一步应在机器空闲时重跑 perf，或先调整 perf 采样/基线规则再验收。
