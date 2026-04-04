# 进度记录 v1-20

**时间** 2026-04-05 01:44 +08:00

## 本轮产出

- 按用户确认，彻底废弃内部 `verify-roadmap-0-*` 命令入口：
  - 已从 `build/toolchains/run/run_manifest.json` 删除 `verify-roadmap-0-windows`
  - 已从 `build/toolchains/run/run_manifest.json` 删除 `verify-roadmap-0-macos`
- 清理运行分发中的 `verify.dispatch` 路径：
  - `build/toolchains/run/run.py`
  - `build/toolchains/run/commands/prepare.py`
  - `build/toolchains/run/commands/test.py`
- 保留底层脚本与模块文件本身：
  - `build/scripts/verify-roadmap-0.py`
  - `build/scripts/verify-roadmap-0.ps1`
  - `build/toolchains/run/commands/verify.py`
  这些现在只作为底层实现/调试入口存在，不再是 `run` 命令体系里的正式命令。
- 同步修正命令清单、CLI 与 dotnet gate 相关测试，使其全部以 `run test workflow ...` 为唯一 workflow 公开入口。

## 已验证

- `python -m unittest tests.integration.run.test_unified_test_command tests.integration.run.test_command_manifest tests.tooling.run.test_unified_test_command tests.tooling.run.test_command_manifest tests.integration.run.test_dotnet_bootstrap tests.tooling.run.test_dotnet_bootstrap tests.tooling.run.test_verify_roadmap0`
  - `Ran 38 tests ... OK (skipped=1)`
- `powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/integration/run/test_run_cli.ps1`
  - `run CLI tests passed`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
  - `status: ok`
- 无沙箱真实回归：
  - `python -B build/toolchains/run/run.py test workflow roadmap-0-windows --json`
  - 结果通过，确认在删除 verify 命令后，公开 workflow 入口仍能走通完整对象链路

## 真实验证产物

- runId: `20260405-012329-windows-c8bb`
- summary: `artifacts/logs/tests/20260405-012329-windows-c8bb/summary.json`
- console: `artifacts/logs/tests/20260405-012329-windows-c8bb/console.log`

## 下一步

- 后续切到 macOS 宿主时，补跑 `test workflow roadmap-0-macos` 与 `pipeline/completion-runtime-trace-macos` 的真实命令验证。
- 继续收束其余文档/测试里仍带有“verify”历史措辞但不影响行为的残留描述。
