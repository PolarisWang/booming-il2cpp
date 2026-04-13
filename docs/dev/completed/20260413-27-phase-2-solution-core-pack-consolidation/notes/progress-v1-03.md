# 进度记录 v1.03

Date: 2026-04-13 11:07:49 +08:00

## 本次完成

- 补齐 solution-mode subject 的 multi-assembly host-input / analysis / codegen / native AOT 执行链路：
  - `host-input-build` 现在记录 `primaryProjectPath` 与 `additionalAssemblyPaths`
  - `additionalAssemblyPaths` 收敛为 `.sln` 内 project output，而不是 host-input 目录里的全部 DLL
  - legacy driver CLI 与 loader / native AOT reload 都支持多程序集入口解析
- 把 Windows native proof build 从手工 `cl` 直编切回现有 reference CMake 路由：
  - `build-target` 对 `native-reference` 现在走 `windows-x64-reference`
  - `SolutionCorePack` 新增 subject-owned `validation/proof/native-reference/CMakeLists.txt`
  - 新增 `RunNativeReferenceProof.cmake`
- 修正 `SolutionCorePack` proof host 的 image name，使其对齐 retained subject 本身，而不是旧的 `MainlineFeaturePack`
- 新增 / 更新回归测试：
  - `tests/unit/execution/test_subject_workers.py`
  - `tests/unit/compatibility/test_solution_core_pack_subject.py`

## 关键结论

- `SolutionCorePack` 已经不只是 skeleton；它现在能在一个 solution-mode retained subject 下同时承接：
  - 三个 `Solution*` archetype slice
  - `MainlineFeaturePack` proof 入口
  - `PerformanceFeaturePack` benchmark 入口
- `source.entry` 位于非 primary assembly 的场景已经完成真实闭环，不再只是 planner / manifest 级别的静态设计。
- Windows proof build 现在复用了仓库里现成的 `chaos_runtime_core` / `chaos_hot_update` / `chaos_bootstrap` / `chaos_bdwgc` 参考组装方式，避免再次漂移成不完整的 ad-hoc 直编链路。

## 验证

- `python -m pytest tests/unit/execution/test_subject_workers.py -q`
- `python -m pytest tests/unit/execution/test_subject_executor.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/planning/test_solution_core_pack_planner.py tests/integration/registry/test_solution_core_pack_registry.py tests/unit/compatibility/test_multi_assembly_entry_override.py tests/unit/planning/test_project_graph.py tests/unit/compatibility/test_managed_closure_contract_bundle.py -q`
- 真实 proof 运行：
  - run id: `20260413-solutioncorepack-native-check-cmake-proof`
  - matrix / goal: `windows-native-check` / `correctness.dev`
- 真实 benchmark 运行：
  - run id: `20260413-solutioncorepack-native-perf-post-proof-fix`
  - matrix / goal: `windows-native-perf` / `perf.release`

## 下一步建议

- 回到父 roadmap，开始 Phase 3：`HotUpdateHostPack` / `MixedExecutionFeaturePack` retained-subject consolidation。
