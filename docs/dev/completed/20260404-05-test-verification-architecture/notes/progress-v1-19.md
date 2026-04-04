# 进度记录 v1-19

**时间** 2026-04-05 01:37 +08:00

## 本轮产出

- 继续收束 `workflow/roadmap-0-windows|macos`：
  - `workflow/roadmap-0-windows` 现映射到隐藏代理命令 `test-workflow-roadmap-0-windows`
  - `workflow/roadmap-0-macos` 现映射到隐藏代理命令 `test-workflow-roadmap-0-macos`
- 新增 `registry-object-alias` 调度类型，允许公开 suite 语法内部转发到正式 registry 对象。
- `system/roadmap-0-windows` 不再引用 `workflow/roadmap-0-windows`，而是显式组合：
  - `pipeline/completion-runtime-trace-windows`
  - `system/roadmap-0-android-startup-gate`
  - `system/roadmap-0-linux-packaging-gate`
- `system/roadmap-0-macos` 不再引用 `workflow/roadmap-0-macos`，而是显式组合：
  - `pipeline/completion-runtime-trace-macos`
  - `system/roadmap-0-ios-packaging-gate`
  - `system/roadmap-0-linux-packaging-gate`
- 更新 `wiki/06-测试验证/整体验证场景/roadmap-0-runtime.md`，明确：
  - `workflow` 只是公开语法兼容入口
  - 真正的正式对象边界已经收口到 `system/roadmap-0-*`

## 已验证

- `python -m unittest tests.integration.run.test_unified_test_command tests.integration.run.test_command_manifest tests.tooling.run.test_registry_command`
  - `Ran 23 tests ... OK`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
  - `status: ok`
- `python -B build/toolchains/run/run.py test workflow roadmap-0-windows --json`
  - sandbox 内因 Windows 构建产物目录权限清理失败
  - 无沙箱重跑通过，确认新的 workflow 公开入口已经走到正式 registry 对象链路

## 真实验证产物

- runId: `20260405-011510-windows-d32b`
- summary: `artifacts/logs/tests/20260405-011510-windows-d32b/summary.json`
- console: `artifacts/logs/tests/20260405-011510-windows-d32b/console.log`

## 下一步

- 后续可继续评估是否彻底废除内部 `verify-roadmap-0-*` 隐藏命令，或仅保留为最低层实现兼容。
- 切到 macOS 宿主时，补跑 `test workflow roadmap-0-macos` 与 `pipeline/completion-runtime-trace-macos` 的真实命令验证。
