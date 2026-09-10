# Phase 2 收口记录

时间：2026-04-14 11:04:15 +08:00

## 已完成
- 完成 `SolutionCorePack` canonical rename，移除 `MainlineFeaturePack` / `PerformanceFeaturePack` compatibility identity，切换到 `CoreRuntimeFeatures` / `CoreRuntimeBenchmarks`。
- 对齐 `subjects/SolutionCorePack/subject.manifest.json`、`source/SolutionCorePack.sln` 与 `source/Launcher/Program.cs` 的真实入口和 dispatch。
- 修复 `build/toolchains/run/testing/compiled_catalog.py` 对 solution subject 的 sibling project `bin/Debug` 输出发现，避免只扫描 `Launcher/bin` 导致 declared metadata 缺失。
- 收口广泛引用点与回归测试，确认 planner / registry / benchmark source / compatibility 口径与新命名一致。

## 验证
- `python -m pytest tests/unit/compatibility/test_solution_core_pack_subject.py -q`
  - 结果：`5 passed`
- `python -m pytest tests/unit/planning/test_solution_core_pack_planner.py -q`
  - 结果：`8 passed`
- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -q`
  - 结果：`4 passed`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
  - 结果：`8 passed`

## 结论
- Phase 2 达到退出标准。
- 下一步进入 `20260414-08-phase-3-hotupdate-host-pack-full-project-workflow`，收口 `HotUpdateHostPack` 的完整热更 workflow 与 capability metadata。
