# Progress v1.05

Date: 2026-04-08 00:03:55 +08:00

## Summary

- 已修复 Windows subject native build 链路：`build-target` 使用绝对 `cmake.exe`、VS developer environment、VS bundled `ninja.exe` 与 `Ninja Multi-Config`。
- 已修复 `windows_developer_environment()` 对 `VsDevCmd.bat` 的调用方式，并按大小写去重环境变量，保留真实的 `PATH` 而不是被旧 `Path` 覆盖。
- 已修复 `subjects/HelloWorldObject/validation/proof/native-reference/CMakeLists.txt`：`REPO_ROOT` 指向仓库根，generated cpp 缺失时立即失败，不再把它声明成 CMake 生成物。
- 已补 focused 回归测试，覆盖 VS dev env、Ninja 解析、subject worker Windows configure 参数和 native reference CMake 布局约束。
- 端到端 `run test subject --id subject/HelloWorldObject --json` 已完整通过，`analysis -> codegen -> native -> runtime -> report` 主链闭环。

## Verification

- `python -B -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_fingerprints tests.unit.run.test_subject_workers tests.unit.run.test_subject_reporting tests.tooling.run.test_subject_command tests.integration.run.test_reporting_layout tests.tooling.run.test_cmake_bootstrap tests.unit.run.test_repo_layout`
  - `Ran 45 tests in 0.913s`
  - `OK`
- `python -B build/toolchains/run/run.py test subject --id subject/HelloWorldObject --json`
  - 在沙箱外执行后完整通过
  - `runId`: `20260408-000057-windows-2ead`
  - `build-target`: `ok`
  - `runtime-observe`: `ok`
  - `finalStatus`: `ok`

## Evidence

- run summary: `artifacts/subjects/HelloWorldObject/runs/20260408-000057-windows-2ead/run-report/summary.json`
- build manifest: `artifacts/subjects/HelloWorldObject/runs/20260408-000057-windows-2ead/matrices/windows-dev-output/build/build.manifest.json`
- runtime manifest: `artifacts/subjects/HelloWorldObject/runs/20260408-000057-windows-2ead/matrices/windows-dev-output/runtime/runtime.manifest.json`
- runtime stdout: `artifacts/subjects/HelloWorldObject/runs/20260408-000057-windows-2ead/matrices/windows-dev-output/runtime/stdout.log`

## Note

- 受限沙箱中的 Windows 原生验证可能在 compiler ABI probe 阶段长时间停滞；这是环境执行限制，不是当前仓库逻辑缺陷。正式 Windows native 端到端验证应优先在沙箱外执行，但 scratch `cmake` 目录依旧只是临时目录，正式证据仍写回 `artifacts/subjects/<id>/runs/<run-id>/...`。
