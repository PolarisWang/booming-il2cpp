# 进展记录 v1-10

**时间：** 2026-04-05 00:18 +08:00

## 本轮产出

- 把 `trace-export` 从模块对象继续提升到整体验证与测试管线：
  - 新增 `system/trace-export-windows-smoke`
  - 新增 `system/trace-export-macos-smoke`
  - 新增 `pipeline/trace-export-windows-runtime`
  - 新增 `pipeline/trace-export-macos-runtime`
- 新增项目 wiki 入口与说明：
  - `wiki/06-测试验证/整体验证场景/trace-export-smoke.md`
  - `wiki/06-测试验证/测试管线/trace-export-runtime.md`
  - 同步更新对应 `INDEX.md`
- 保持对象分层一致：
  - `code`：`smoke/HostEmbeddingLite`
  - `module`：`module/hosted-runtime/basic` + `module/trace-export/<host>`
  - `system`：`system/trace-export-<host>-smoke`

## 已验证

- `python -m unittest tests.integration.run.test_registry_scan tests.tooling.run.test_registry_command`
- `python -m unittest tests.tooling.run.test_registry_command tests.integration.run.test_registry_scan tests.tooling.run.test_test_watch_summary tests.integration.run.test_reporting_layout tests.tooling.run.test_tui`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- `python -B build/toolchains/run/run.py test pipeline --id pipeline/trace-export-windows-runtime`

## 下一步

- 评估是否把 `trace-export` 纳入更高层的 release/completion 类测试管线
- 继续把下一批真实模块按同样方式提升到 `system/pipeline`
