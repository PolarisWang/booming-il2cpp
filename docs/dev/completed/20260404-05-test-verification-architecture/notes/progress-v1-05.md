# 进展记录 v1-05

**时间：** 2026-04-04 22:35 +08:00

## 本轮产出

- 将 completion 测试管线从样例 `pipeline/completion-managed-smoke` 收束为真实对象 `pipeline/completion-runtime-core`
- 为 `completion-runtime-core` 补齐正式 wiki 页面，并把 `wiki/06-测试验证/测试管线/INDEX.md` 的公开入口切换到新对象
- 更新 `run test pipeline` 的帮助示例、TUI 提示词、项目文档和命令清单，统一使用 `completion-runtime-core`
- 在 registry 中增加 `deprecated: true` 机制，旧的 `completion-managed-smoke` 仍可保留历史文件，但不会再进入公开注册列表
- 修正 `expand_execution_plan`，同一条测试管线内的 suite 只按首次出现顺序执行一次，避免 `module/system` 递归导致重复运行
- 新增回归测试，确认 `pipeline/completion-runtime-core` 的执行计划只包含 5 个唯一 suite

## 已验证

- `python -m unittest tests.integration.run.test_registry_scan tests.tooling.run.test_registry_command tests.integration.run.test_command_manifest tests.tooling.run.test_command_manifest tests.tooling.run.test_tui`
- `python -B build/toolchains/run/run.py test registry list --json`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- `python -B build/toolchains/run/run.py test pipeline --id pipeline/completion-runtime-core --json`

## 下一步

- 继续把更多真实模块接入 `module/system/pipeline` 对象体系
- 评估是否需要把测试管线执行结果进一步提升为 phase-aware 报告，而不只是去重后的 suite 级执行
