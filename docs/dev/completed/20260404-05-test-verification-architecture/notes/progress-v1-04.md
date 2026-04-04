# 进展记录 v1-04

**时间：** 2026-04-04 23:55 +08:00

## 本轮产出

- 修复 `tests.tooling.run.test_tui_routing` 与 `tests.integration.run.test_tui_routing` 的假慢问题：
  - integration 用例补齐 `run_inline_menu` mock，避免真实 TUI 等待输入。
  - tooling 用例改为仓库内手动创建临时目录，绕开当前 Windows + Python 3.13 环境下 `tempfile` 目录 ACL 异常。
  - `verify` 断言收紧为“禁止公开 `run verify ...`”，不再误伤 `prepare verify ...`。
- 把 registry 对象补齐为 skill/机器可直接消费的结构：
  - 新增 `canonicalCommand`
  - 新增 `skillRecommendations.recommended`
  - 新增 `skillRecommendations.requiredBeforeCompletion`
  - 新增 `skillRecommendations.requiredForPipelineRelease`
- 把 `run test module/system/pipeline` 的结果补齐对象上下文：
  - `selectedObject`
  - `objectOwnership`
  - 失败时默认补齐 `failureCode` / `parserHint` / `humanHint`
- 清理隐藏兼容层里的旧公开文案：
  - `run_manifest.json` 里的隐藏 `verify-roadmap-0-*` 示例改为 `run test workflow ...`
  - `commands/verify.py` 的失败提示切换为 workflow 语义，不再返回 `verify roadmap-0 failed`
- 修复 `tests/tooling/run/test_verify_roadmap0.py` 的环境敏感问题，使其在 Windows 环境下稳定：
  - 无 `sh` 时跳过 POSIX wrapper 用例
  - 路径断言改为 `Path(...)` 比较

## 已验证

- `python -B -m unittest tests.tooling.run.test_registry_command tests.tooling.run.test_unified_test_command tests.tooling.run.test_verify_roadmap0 tests.tooling.run.test_command_manifest tests.integration.run.test_command_manifest`
- `python -B -m unittest tests.tooling.run.test_tui_routing tests.integration.run.test_tui_routing`
- `python -B build/toolchains/run/run.py test registry list --json`
- `python -B build/toolchains/run/run.py test module --id module/managed-smoke/basic --json`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`

## 下一步

- 继续清理 live 文档和入口说明里的旧 `run verify` / `verify` 语义，尤其是仍可能被机器消费的帮助与示例。
- 评估是否需要把 `skillRecommendations` 的生成规则再收紧，避免“相关对象过宽”导致推荐噪声偏大。
