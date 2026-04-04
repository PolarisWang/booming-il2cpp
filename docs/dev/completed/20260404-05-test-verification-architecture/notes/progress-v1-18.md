# 进度记录 v1-18

**时间** 2026-04-05 01:28 +08:00

## 本轮产出

- 把原先仍只停留在 `verify-roadmap-0` 内部的 4 类重要验证正式对象化，并接入统一 `run test` 体系：
  - `contract/analysis-schema`
  - `contract/trace-schema`
  - `contract/native-abi`
  - `contract/native-bridge`
- 新增 `module/analysis/basic`，并把以下模块验证收束到正式对象：
  - `module/interop/basic` 现覆盖 `native ABI + native bridge + PInvokeLite`
  - `module/trace-export/windows|macos` 现覆盖 `trace schema + trace compare`
- 扩展 completion 管线，把 contract/module 收口到默认 completion 验证：
  - `pipeline/completion-runtime-core`
  - `pipeline/completion-runtime-trace-windows`
  - `pipeline/completion-runtime-trace-macos`
- 为新 contract suite 增加隐藏命令并统一走 `run test`：
  - `test-contract-analysis-schema`
  - `test-contract-trace-schema`
- 抽出共享 contract 校验逻辑到 `build/toolchains/run/testing/contracts.py`，并复用到：
  - `build/toolchains/run/commands/test.py`
  - `build/scripts/verify-roadmap-0.py`
- 同步更新 `wiki/06-测试验证/`：
  - 新增 `模块/analysis.md`
  - 更新 `模块/INDEX.md`
  - 更新 `模块映射.md`
  - 更新 `模块/interop.md`
  - 更新 `模块/trace-export.md`
  - 更新 `测试管线/completion-runtime-core.md`
  - 更新 `测试管线/completion-runtime-trace.md`
  - 更新 `测试管线/trace-export-runtime.md`

## 已验证

- `python -m unittest tests.tooling.run.test_contract_check tests.tooling.run.test_platform_gate_validate_only tests.tooling.run.test_verify_roadmap0 tests.tooling.run.test_registry_command tests.integration.run.test_unified_test_command tests.integration.run.test_registry_scan tests.tooling.run.test_command_manifest tests.integration.run.test_command_manifest`
  - `Ran 53 tests ... OK (skipped=5)`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
  - `status: ok`

## 结果说明

- skill / harness 现在可以直接从 registry 看到这些 contract/native 验证，不再只能从 `verify-roadmap-0` 的内部步骤里“隐式存在”。
- `completion-runtime-core` 与 `completion-runtime-trace-*` 的模块阶段，已经能自动带出 analysis/native/trace contract 的正式对象。
- `verify-roadmap-0.py` 仍保留 legacy workflow 入口，但其 analysis/trace contract 校验逻辑已与统一 `run test` 复用同一份实现，减少后续漂移。

## 下一步

- 继续收束 `workflow/roadmap-0-windows|macos` 这两个仍然直接映射 legacy verify suite 的对象，评估是否把 host workflow 也进一步改成 registry 可展开的正式 pipeline / system 组合入口。
- 后续切到 macOS 宿主时，补跑 `pipeline/completion-runtime-trace-macos` 的真实命令验证。
