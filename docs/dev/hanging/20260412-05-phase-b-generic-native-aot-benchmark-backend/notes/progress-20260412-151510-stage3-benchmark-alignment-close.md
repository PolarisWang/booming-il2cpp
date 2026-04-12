# 2026-04-12 15:15:10 Stage 3 Benchmark Alignment Close

## 背景

`BenchArithmetic` 的 generic native AOT MVP 在 14:31 已经把 native record 跑通，但后续补跑 managed / interpreter 时暴露出两个问题：

- 外置 perf harness 默认 `10000` 次 workload 迭代，导致 `RunWorkload()` 在 managed / interpreter 上跑到约 97 秒。
- native host 每个 sample 只调用一次 `RunNativeAot()`，而 managed / interpreter 每个 sample 会在 harness 内批量调用多次，三种模式的 duration / checksum 口径不一致。

## 本轮完成

- 在 `subjects/BenchArithmetic/subject.manifest.json` 中新增 `validation.perf.harnessIterations: 10`。
- `build/toolchains/run/testing/subject_workers.py` 现在支持 subject 级 perf iteration override：
  - managed / interpreter 继续复用现有 harness CLI。
  - native runtime 也会把同一个 override 传给外置 native host。
- `src/native/benchmark-host/native_aot_main.cpp` 新增 `--iterations` 支持，并把 native checksum 口径对齐为 batch 执行后的归一结果。
- 更新相关回归，覆盖：
  - managed / interpreter 读取 `harnessIterations`
  - native runtime 把 `harnessIterations` 传给 host
  - native perf 现有工件输出不回退

## 验证

- 单测：
  - `python -m pytest tests/unit/run/test_subject_workers.py tests/unit/run/test_subject_workers_perf.py tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_benchmark_subject_sources.py tests/unit/run/test_benchmark_command.py tests/unit/run/test_phase_b_native_aot_workload_entry_bundle.py -q`
  - 结果：`41 passed, 1 warning`
- 端到端 benchmark：
  - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode managed --record`
  - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode interpreter --record`
  - `python build/toolchains/run/run.py benchmark --subject BenchArithmetic --mode native --record`

## 最新结果

- managed: `20260412-065556-BenchArithmetic-managed`
  - `meanDurationMs = 63.484`
  - `meanChecksum = 7010`
- interpreter: `20260412-065652-BenchArithmetic-interpreter`
  - `meanDurationMs = 63.640`
  - `meanChecksum = 7010`
- native: `20260412-071431-BenchArithmetic-native`
  - `meanDurationMs = 59.961`
  - `meanChecksum = 7010`

## 结论

`BenchArithmetic` 当前这条 generic native AOT benchmark backend 已经从“只有 native 有数据”推进到“三种模式同口径可比较”。AOT dashboard 的 `AOT (Generated C++)` 不再缺数，且最新比较结果不再被 iteration 口径错位污染。
