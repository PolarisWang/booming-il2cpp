# 进展记录 v1-16

**时间** 2026-04-05 00:36 +08:00

## 本轮产出

- 把 `reference desktop gate` 正式接入统一测试体系：
  - 新增 public suite `gate/windows-reference-desktop`
  - 新增 public suite `gate/macos-reference-desktop`
  - 新增 system 对象 `system/roadmap-0-windows-reference-gate`
  - 新增 system 对象 `system/roadmap-0-macos-reference-gate`
- 为这两条 gate 补齐真实 legacy command：
  - `build-platform-windows-reference-desktop`
  - `build-platform-macos-reference-desktop`
- 新增 `reference-desktop-gate` 执行器，正式覆盖：
  - reference preset 构建
  - `HostEmbeddingLite` trace export
  - warmup trace compare
  - gate record 落盘
- 更新 `wiki/06-测试验证/整体验证场景/roadmap-0-platform-gates.md`
  与 `wiki/06-测试验证/整体验证场景/INDEX.md`，把 Windows / macOS reference desktop gate 纳入唯一正式入口。

## 已验证

- `python -m unittest tests.tooling.run.test_platform_gate_validate_only tests.tooling.run.test_verify_roadmap0 tests.tooling.run.test_registry_command tests.integration.run.test_unified_test_command tests.integration.run.test_registry_scan tests.tooling.run.test_command_manifest tests.integration.run.test_command_manifest`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- `python -B build/toolchains/run/run.py test system --id system/roadmap-0-windows-reference-gate --json`
  - sandbox 内失败，`console.log` 明确显示 VS/MSBuild `tlog` 删除被拒绝
  - 无沙箱重跑后通过

## 说明

- `system/roadmap-0-windows-reference-gate` 已在当前 Windows 宿主上做过真实命令验证。
- `system/roadmap-0-macos-reference-gate` 当前只完成 registry / dispatch / command / wiki 接入验证；真实命令仍需 macOS 宿主补跑。

## 下一步

- 继续收束剩余隐式验证链路，优先评估是否把 reference desktop gate 纳入更高层 completion / release 级测试管线。
