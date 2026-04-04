# 进展记录 v1-03

**时间：** 2026-04-04 23:25 +08:00

## 本轮产出

- 收紧 `build/toolchains/run/manifest.py` 的 CLI 解析，只允许公开命令直接解析，`run verify ...` 不再作为公共入口生效。
- 完成 `run test` 第二段切换的测试补强，覆盖：
  - `test suite/module/system/pipeline`
  - `test registry list/refresh/check-consistency`
  - `verify` 旧入口失效
  - TUI 新菜单与 selector 路由
  - wrapper `list --json` 与交互菜单
- 调整 `tests/tooling/run/test_run_cli.ps1`，确认能力列表暴露的是新 `run test` 对象入口，而不是隐藏的旧 `verify` / smoke build 入口。

## 已验证

- `python -B -m unittest tests.tooling.run.test_command_manifest tests.integration.run.test_command_manifest tests.tooling.run.test_tui tests.integration.run.test_tui tests.tooling.run.test_registry_command tests.integration.run.test_registry_scan`
- `python -B build/toolchains/run/run.py verify roadmap-0 --host windows`
- `python -B build/toolchains/run/run.py test registry list --json`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- `python -B build/toolchains/run/run.py test suite --id smoke/HelloWorld --json`
- `powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File tests/tooling/run/test_run_cli.ps1`

## 下一步

- 继续清理正式帮助、技能引用链和错误定位链路中的旧 `run verify` 痕迹。
- 把 skill 推荐与错误定位接到 registry / selector / object ownership 模型上。
