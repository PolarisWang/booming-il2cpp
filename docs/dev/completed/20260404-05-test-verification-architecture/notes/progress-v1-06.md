# 进展记录 v1-06

**时间：** 2026-04-04 22:37 +08:00

## 本轮产出

- 新增 `trace-export` 模块的 host-specific 验证对象：
  - `module/trace-export/windows`
  - `module/trace-export/macos`
- 为 `trace-export` 补齐模块说明页，并更新 `wiki/06-测试验证/模块/INDEX.md`
- 将 `contract/trace-compare-windows` 与 `contract/trace-compare-macos` 正式接入模块级验证对象，补上“重要验证升格为 case/object”链路
- 修正 registry 扫描行为，模块/系统/测试管线对象现在会按当前 `hostPlatform` 过滤，避免把跨平台对象错误加入当前注册表

## 已验证

- `python -m unittest tests.integration.run.test_registry_scan tests.tooling.run.test_registry_command`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- `python -B build/toolchains/run/run.py test module --id module/trace-export/windows --json`

## 下一步

- 继续挑选下一批真实模块接入 registry
- 或开始把 `phase` 级结果写进测试管线报告，避免当前只保留 suite 执行视角
