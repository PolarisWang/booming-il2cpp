# 进展记录 v1-17

**时间** 2026-04-05 00:52 +08:00

## 本轮产出

- 按用户确认的 `A` 方案，把 `reference desktop gate` 直接并入现有 completion 管线：
  - `pipeline/completion-runtime-trace-windows`
  - `pipeline/completion-runtime-trace-macos`
- 更新对应 pipeline manifest：
  - `system/roadmap-0-windows-reference-gate`
  - `system/roadmap-0-macos-reference-gate`
  已进入 `system` phase
- 扩展 completion trace 管线的归属范围：
  - `moduleIds` 增加 `roadmap-0`
  - `subsystemIds` 增加 `workflow`
  让 registry / skill 能把 reference gate 正确识别为 completion 前默认验证的一部分
- 更新 `wiki/06-测试验证/测试管线/completion-runtime-trace.md`，补充本管线已覆盖 reference desktop gate。

## 已验证

- `python -m unittest tests.tooling.run.test_platform_gate_validate_only tests.tooling.run.test_verify_roadmap0 tests.tooling.run.test_registry_command tests.integration.run.test_unified_test_command tests.integration.run.test_registry_scan tests.tooling.run.test_command_manifest tests.integration.run.test_command_manifest`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- `python -B build/toolchains/run/run.py test pipeline --id pipeline/completion-runtime-trace-windows --json`
  - sandbox 内失败，失败点位于新增的 `system/roadmap-0-windows-reference-gate`
  - 无沙箱重跑后整条 pipeline 通过

## 说明

- 当前 Windows 宿主下，`completion-runtime-trace-windows` 已真实覆盖：
  - code smoke
  - trace compare
  - reference desktop gate
- `completion-runtime-trace-macos` 的对象接入、registry 推荐和 phase 结果已完成，但真实命令仍需 macOS 宿主补跑。

## 下一步

- 继续收束剩余隐式验证链路。
- 后续切到 macOS 宿主时，补跑 `pipeline/completion-runtime-trace-macos` 的真实命令验证。
