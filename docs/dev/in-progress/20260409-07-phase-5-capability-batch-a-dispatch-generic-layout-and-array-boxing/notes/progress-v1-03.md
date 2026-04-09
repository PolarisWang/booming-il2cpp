# Progress v1.03

Date: 2026-04-09 21:22:07 +08:00

## 本轮更新

- 三条 focused correctness 已全部通过：
  - `windows-dispatch-check` -> `20260409-204130-windows-a5e2`
  - `windows-generic-layout-check` -> `20260409-205624-windows-af35`
  - `windows-array-boxing-check` -> `20260409-210422-windows-71c3`
- Windows 直接编译路径已修正到与 `VsDevCmd` 一致的 MSVC toolset：
  - `find_visual_cpp_executable()` 现在优先读 `VCToolsInstallDir`
  - 否则回退到 `vswhere` 发现结果里的最新 toolset
  - 已避免旧 `14.29.30133` 与开发环境 `14.38.33130` 不一致导致的 `STL1001`
- generic-layout 的 lowering contract 已对齐真实产物：
  - `static-forwarder`
  - `instance-field-getter`
- `current.json` 假运行根因已定位并修复：
  - 不是 subject 还在跑，而是 Windows 工作区里删除 freshly-written pointer 会报 `Access is denied`
  - `reporting.py` / `operation_reporting.py` 已改为“优先删除，失败则回写最终状态”
  - 已把工作区里遗留的 `current.json` 同步纠正为 `status=ok`

## 本轮验证

- `python -m unittest tests.integration.run.test_reporting_layout`
- `python -m unittest tests.tooling.run.test_operation_reporting`
- `python -m unittest tests.tooling.run.test_subject_command`
- `python -m unittest tests.tooling.run.test_cmake_bootstrap`
- `python -m unittest tests.unit.run.test_subject_workers`
- `python -m unittest tests.unit.run.test_phase5_capability_batch_a`
- `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_subject_manifest_schema tests.unit.run.test_phase5_capability_batch_a tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf tests.tooling.run.test_subject_command tests.tooling.run.test_cmake_bootstrap tests.integration.run.test_reporting_layout tests.tooling.run.test_operation_reporting`

## 当前阻塞

- `perf.profile` 仍未执行完成。
- 当前机器上有一批 `20:54/21:01` 启动的旧 `MSBuild/cl.exe` 在持续占 CPU。
- 在确认这些进程不是用户仍在使用的构建之前，不应直接拿当前环境做最终性能验证。

## 下一步

- 与用户确认旧 `MSBuild/cl.exe` 是否可回收。
- 环境清理后执行：
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`
- perf 通过后，补齐最终验证汇总并推进 `Phase 5` 归档。
