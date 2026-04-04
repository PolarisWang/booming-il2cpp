# 进度记录 v1-23

**时间** 2026-04-05

## 本轮产出

- 为已废弃的 `run verify roadmap-0 --host ...` 增加明确迁移提示：
  - `build/toolchains/run/run.py`
- 新行为：
  - 当用户或旧脚本调用 `run verify roadmap-0 --host macos`
  - 不再只返回 `unknown command`
  - 而是返回结构化迁移信息，明确替代为 `run test workflow roadmap-0-macos`
- 同理：
  - `--host windows` -> `run test workflow roadmap-0-windows`
- 这样旧入口的失败模式从“不可解释”收束为“可迁移”，便于 skill、harness 和人工脚本快速修正

## 已验证

- `python -m unittest tests.integration.run.test_unified_test_command tests.tooling.run.test_unified_test_command`
  - `Ran 8 tests ... OK`
- `python -m unittest tests.integration.run.test_unified_test_command tests.integration.run.test_command_manifest tests.tooling.run.test_unified_test_command tests.tooling.run.test_command_manifest tests.integration.run.test_dotnet_bootstrap tests.tooling.run.test_dotnet_bootstrap tests.tooling.run.test_verify_roadmap0`
  - `Ran 40 tests ... OK (skipped=1)`
- 真实 CLI：
  - `python -B build/toolchains/run/run.py verify roadmap-0 --host macos --json`
  - 返回 `status=error`，并携带 `migration.replacementSyntax=test workflow roadmap-0-macos`
- 真实 CLI：
  - `python -B build/toolchains/run/run.py verify roadmap-0 --host windows --json`
  - 返回 `status=error`，并携带 `migration.replacementSyntax=test workflow roadmap-0-windows`

## 下一步

- 当前 Windows 宿主侧的迁移收口已基本完成：
  - 正式入口统一为 `run test ...`
  - 废弃入口具备迁移提示
  - 底层兼容层边界已澄清
- 后续高价值动作仍是 macOS 宿主真实补跑：
  - `python -B build/toolchains/run/run.py test workflow roadmap-0-macos --json`
  - `python -B build/toolchains/run/run.py test pipeline --id pipeline/completion-runtime-trace-macos --json`
