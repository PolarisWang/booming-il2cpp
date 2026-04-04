# 进度记录 v1-21

**时间** 2026-04-05

## 本轮产出

- 收束用户可见文档中的历史 `verify` 入口表述：
  - `wiki/04-工具与集成/统一入口脚本.md`
  - `docs/architecture/roadmap-0/local-verification.md`
  - `wiki/06-测试验证/整体验证场景/roadmap-0-runtime.md`
  - `wiki/06-测试验证/模块/analysis.md`
- 明确边界：
  - 正式入口继续统一为 `run test ...`
  - `build/scripts/verify-roadmap-0.*` 仅保留为底层实现与调试路径
  - `artifacts/verify-roadmap-0/` 作为历史产物目录名继续保留
- 同步修正 dotnet bootstrap 两组测试中的旧命令示例：
  - `tests/integration/run/test_dotnet_bootstrap.py`
  - `tests/tooling/run/test_dotnet_bootstrap.py`
  - 非交互 macOS 缺少 `dotnet` 时，提示语境改为 `test workflow roadmap-0-macos`

## 已验证

- `python -m unittest tests.integration.run.test_command_manifest tests.tooling.run.test_command_manifest tests.integration.run.test_unified_test_command tests.tooling.run.test_unified_test_command tests.integration.run.test_dotnet_bootstrap tests.tooling.run.test_dotnet_bootstrap`
  - `Ran 30 tests ... OK`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
  - `status: ok`

## 下一步

- 当前 Windows 宿主上的公开入口、文档入口和核心断言已收口到 `run test ...`
- 后续切到 macOS 宿主时，补跑：
  - `python -B build/toolchains/run/run.py test workflow roadmap-0-macos --json`
  - `python -B build/toolchains/run/run.py test pipeline --id pipeline/completion-runtime-trace-macos --json`
