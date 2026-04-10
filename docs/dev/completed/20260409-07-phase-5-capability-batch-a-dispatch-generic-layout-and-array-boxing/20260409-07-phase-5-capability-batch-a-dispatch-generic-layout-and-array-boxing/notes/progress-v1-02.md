# Progress v1.02

Date: 2026-04-09 19:24:03 +08:00

## 本轮更新

- `dispatch` 主 blocker 已解除。
  - `LinkerStage` reachable closure 已补入 `callvirt` override 目标。
  - 最新 `native-proof.plan.json` 已解析为 `devirtualized-direct`，目标实例方法为 `MainlineFeaturePack_DispatchLeaf_BuildMessage`。
- 补回了两处纯 Python 规划层回归：
  - `perf-profile` 在显式 `variant=PROFILE` 下允许跨 `proof/perf` 多 validation kind 共同存在。
  - `MainlineFeaturePack` schema 断言已同步到 Phase 5 之后的 capability matrices 集合。
- `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_subject_manifest_schema tests.unit.run.test_phase5_capability_batch_a tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf tests.tooling.run.test_subject_command` 已通过。

## 新发现

- 当前 native build 长时间卡住的直接症状不再是 `dispatch` 语义，而是 Windows `CMake try_compile` 在清理 `CMakeScratch/TryCompile-*` 时持续报 `Access is denied`。
- 用户明确说明当前正在使用 `msbuild`，因此本轮暂停所有依赖 `msbuild` 的 focused subject/native 验证，避免继续占用构建环境。

## 下一步

- 待用户释放 `msbuild` 环境后，恢复 `windows-dispatch-check`。
- 如果 `dispatch` focused run 通过，则继续 `windows-generic-layout-check`、`windows-array-boxing-check` 与 `perf.profile`。
- 如果 native build 仍停在 `try_compile`，则单独开一轮工具链调用层排障，不再把它与 `Phase 5` 语义闭环混在一起。
