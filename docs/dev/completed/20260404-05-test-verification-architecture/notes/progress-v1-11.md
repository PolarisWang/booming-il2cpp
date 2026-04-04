# 进展记录 v1-11

**时间：** 2026-04-05 00:26 +08:00

## 本轮产出

- 将 `trace-export` 正式纳入 completion 级测试管线：
  - 新增 `pipeline/completion-runtime-trace-windows`
  - 新增 `pipeline/completion-runtime-trace-macos`
- 保持收口策略最小化：
  - 不替换现有 `pipeline/completion-runtime-core`
  - 继续保留 Linux 可直接复用的 completion core
  - 仅在支持 trace-export 的 Windows/macOS 上新增更完整的 completion 入口
- 同步补齐项目 wiki：
  - `wiki/06-测试验证/测试管线/completion-runtime-trace.md`
  - `wiki/06-测试验证/测试管线/INDEX.md`
  - `wiki/06-测试验证/测试管线/completion-runtime-core.md`
  - `wiki/06-测试验证/模块/trace-export.md`

## 已验证

- `python -m unittest tests.integration.run.test_registry_scan tests.tooling.run.test_registry_command`
- `python -m unittest tests.tooling.run.test_registry_command tests.integration.run.test_registry_scan tests.tooling.run.test_test_watch_summary tests.integration.run.test_reporting_layout tests.tooling.run.test_tui`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- `python -B build/toolchains/run/run.py test pipeline --id pipeline/completion-runtime-trace-windows`

## 下一步

- 继续接入下一批真实模块，优先处理已经具备稳定项目规则、但尚未升格为正式对象的验证链路
