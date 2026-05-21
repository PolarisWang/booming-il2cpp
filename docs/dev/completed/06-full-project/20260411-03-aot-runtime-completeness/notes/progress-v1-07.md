# 进展记录 v1-07

## 时间

- 2026-04-11 12:48:19 +08:00

## 本轮完成

- 完成 Batch 5 的真实收口与边界澄清：
  - `LinkerStrippingProof` 的真实 convert 已稳定产出 `preserve-descriptor.json`
  - `CodeGenStage` 对 preserve-aware subject 改为回退到 `generic-analysis-only`
  - 明确 Batch 5 只完成 analysis 侧 stripping/preserve proof，不把 warning 状态误报为 native/runtime 闭环
- 完成 Batch 6 perf governance：
  - `subject_workers.py` 支持从 perf payload 的 `metrics` 对象吸收自定义数值字段，并汇总 `mean|min|max<Metric>`
  - `InterfaceDispatchProof` 新增 `perf-profile`、`native-runtime-perf` pipeline、`windows-native-profile` matrix 与 `meanDispatchNanoseconds <= 5.0` baseline
  - `InterfaceDispatchProof` PROFILE native proof 输出 `dispatchNanoseconds`
  - `GoldenMultiProject` 新增 suite 级 convert perf 校验脚本与 `convertDurationMs <= 10000` baseline
- 新增文档：
  - `brainstorm-v1-06.md`
  - `design-v1-06.md`

## 验证结果

- `python -m pytest tests/unit/run/test_subject_workers_perf.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase2_perf_governance.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_subject_manifest_schema.py -v` -> 8 passed
- `python -m pytest tests/unit/run/test_subject_planner.py -v` -> 17 passed
- `python -m pytest tests/unit/run/test_subject_reporting.py -v` -> 5 passed
- `python -m pytest tests/unit/run -v` -> 175 passed
- `dotnet build subjects/InterfaceDispatchProof/source/InterfaceDispatchProof.csproj -c Debug` -> succeeded
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug` -> succeeded
- `python build/toolchains/run/run.py --json test subject --id subject/InterfaceDispatchProof --goal perf.profile --validation-profile perf-profile --variant PROFILE` -> succeeded
  - `meanDispatchNanoseconds = 3.057`
  - `summaryPath = artifacts/subjects/InterfaceDispatchProof/runs/20260411-124508-windows-8bcb/run-report/summary.json`
- `python tests/perf/convert-golden-multi-project/check.py` -> succeeded
  - `convertDurationMs = 4901.549`

## 备注

- Batch 5 仍未完成 debug baseline、stack trace recovery 与完整 stripped report。
- Batch 6 的 convert perf 仍是 suite 级入口，不等于把 `subjects/golden/*` 全量并入 subject planner。

## Wiki

- 已新增长期知识页，沉淀 perf payload 自定义指标与 suite 级 convert perf baseline 规则。

## 下一步

- 归档 `20260411-03-aot-runtime-completeness`，切换到 `20260411-04-engine-binding-contract`。
