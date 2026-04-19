---
task_id: 20260418-08-hotupdate-consumer-validation
title: HotUpdate Consumer Validation
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-19 10:06:00 +08:00
updated_at: 2026-04-19 10:16:00 +08:00
latest_stop_point: validated HotUpdateHostPack consumer lane with `7/7` proof, `4/4` benchmark, and `8/8` engineering/workload checks ok
current_dir: docs/dev/in-progress/20260418-08-hotupdate-consumer-validation
parent_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_task_id: 20260418-01-dotnet-foundation-pack-nativeization-roadmap
source_relation: roadmap-child-hotupdate-consumer-validation
active: false
---

## 2026-04-19 10:16 Closeout

- `HotUpdateHostPack` consumer lane 已完成正式对象验证。
- proof: `7/7 ok`
- benchmark: `4/4 ok`
- engineering validation / workload: `8/8 ok`
- 修复了 `patch-callback-flow-proof` 与 `hot-update-dispatch-bench` 的假阳性风险：两者现在都会先 `LoadPackage`，确认 `RuntimeMode.Mixed` 后再执行 `DispatchInt32Unary`。

## 产物

- `hotupdate-consumer-validation-summary-v1-01.json`

## 边界说明

- 本 lane 验证的是 hotupdate 作为 certified foundation 的 managed consumer。
- 本 lane 的 declared benchmark 是 `HotUpdateHostPack` managed consumer benchmark，不作为 native benchmark proof。

## 验证

- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateSkeletonProofEntry::Run()`
  - 结果: `ok`
- `python build/toolchains/run/run.py test declared-unit-test --id declared-unit-test/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.PatchCallbackFlowProofEntry::Run()`
  - 结果: `ok`
- `python build/toolchains/run/run.py test declared-benchmark --id declared-benchmark/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry::RunWorkload()`
  - 结果: `ok`
- `python -m pytest tests/unit/compatibility/test_hot_update_e2e_flows.py::Phase9HotUpdateE2ETests::test_patch_callback_flow_and_dispatch_benchmark_use_loaded_mixed_runtime_path -q`
  - 结果: `1 passed`
- `dotnet build subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release -m:1`
  - 结果: build succeeded

## wiki

- 当前不写入 wiki。
- 原因: 本轮修复和验证沉淀在 HotUpdateHostPack 正式 proof/benchmark 及回归测试中，没有新增长期规则。
