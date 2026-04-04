# 进展记录 v1-02

**时间：** 2026-04-04 22:30 +08:00

## 本轮产出

- 新增 `build/toolchains/run/testing/public_specs.py`
- 新增 `build/toolchains/run/testing/selectors.py`
- 新增 `build/toolchains/run/testing/registry.py`
- 扩展 `run test`，新增 `suite/module/system/pipeline/registry` 子域
- 新增 `tests/registry/**` 注册样板对象
- 新增 `wiki/06-测试验证/` 作为项目级唯一正式入口

## 已验证

- `python -m unittest tests.tooling.run.test_registry_command tests.tooling.run.test_command_manifest tests.integration.run.test_command_manifest tests.integration.run.test_registry_scan`
- `python build/toolchains/run/run.py test registry list --json`
- `python build/toolchains/run/run.py test registry check-consistency --json`

## 下一步

- 迁出 `run verify` 的正式公开入口与帮助/TUI 暴露。
- 将 TUI、skill 推荐链路与错误定位链路切换到 registry 快照。
