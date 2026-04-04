# 进度记录 v1-22

**时间** 2026-04-05

## 本轮产出

- 继续收口 `verify-roadmap-0` 的底层兼容层语义：
  - `build/toolchains/run/commands/verify.py`
  - `tests/tooling/run/test_verify_roadmap0.py`
- 调整原则：
  - 底层实现仍保留在 `commands/verify.py` 与 `build/scripts/verify-roadmap-0.*`
  - 但其内部进度与失败语义不再借用已废弃的公开 `verify roadmap-0 --host ...` 命令文本
  - 统一改为 `roadmap-0 low-level script (...)` 语义，明确它只是底层实现与调试路径
- 这样做之后，当前仓库中的 `verify` 残留边界进一步明确：
  - 公开入口层：统一为 `run test ...`
  - 兼容层：脚本文件名、产物目录名、prepare scope 名
  - 底层实现测试：只验证脚本调度与产物行为，不再伪装成公开命令

## 已验证

- `python -m unittest tests.tooling.run.test_verify_roadmap0`
  - `Ran 8 tests ... OK (skipped=1)`
- `python -m unittest tests.integration.run.test_unified_test_command tests.integration.run.test_command_manifest tests.tooling.run.test_unified_test_command tests.tooling.run.test_command_manifest tests.integration.run.test_dotnet_bootstrap tests.tooling.run.test_dotnet_bootstrap tests.tooling.run.test_verify_roadmap0`
  - `Ran 38 tests ... OK (skipped=1)`
- `powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/integration/run/test_run_cli.ps1`
  - `run CLI tests passed`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
  - `status: ok`

## 下一步

- 当前 Windows 宿主上的“公开入口收口 + 底层兼容层边界澄清 + 核心回归”已完成
- 后续高价值动作切换为 macOS 宿主上的真实命令补跑：
  - `python -B build/toolchains/run/run.py test workflow roadmap-0-macos --json`
  - `python -B build/toolchains/run/run.py test pipeline --id pipeline/completion-runtime-trace-macos --json`
