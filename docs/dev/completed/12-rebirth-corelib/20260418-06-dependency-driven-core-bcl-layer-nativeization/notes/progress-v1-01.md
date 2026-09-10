# 进展记录 v1.01

## 时间

- 2026-04-18 11:33:19 +08:00
- 2026-04-18 13:06:12 +08:00

## 本轮完成

- 新建 dependency-layer child task，并冻结命名、输入 authority 与 formal verification object。
- 新增 `dependency-layer` 命令入口与 `DependencyLayerArtifactsBuilder`，生成 `layer-plan-v1-01.json`、`proof-benchmark-lane-v1-01.json` 与 `assembly-nativeization-plan/*.json`。
- 将 `CoreLibReferenceSolution` 的官方 ref DLL 引用迁到 `src/dll/dotnet-foundation/`，并补齐 `SolutionCorePack` 兼容性断言。
- 将任务归档到 `docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/`，同步父 roadmap 与 `docs/dev` 索引，并删除过期的 `docs/dev/ACTIVE.md`。

## 当前判断

- dependency-layer authority 已冻结完成，`core-bcl-layer-01` 的 `29` 个 ready DLL 可以作为 roadmap 下一条 execution child 的输入。
- 当前没有活动 child；后续应优先围绕 `System.Collections`、`System.Memory`、`System.Threading`、`System.Console` 创建真实 proof / benchmark 执行计划。

## 验证

- `python -m pytest tests/tooling/foundation/test_dependency_layer_plan.py tests/tooling/foundation/test_phase1_contract_lane.py tests/tooling/foundation/test_phase2_corelib_substrate.py tests/tooling/foundation/test_phase3_corelib_family_plan.py tests/tooling/foundation/test_corelib_family_execution_entry.py -q`
  - 结果: `11 passed`
- `python -m pytest tests/unit/compatibility/test_solution_core_pack_subject.py -q`
  - 结果: `6 passed`
- `Get-Content docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/layer-plan-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/completed/20260418-06-dependency-driven-core-bcl-layer-nativeization/proof-benchmark-lane-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
