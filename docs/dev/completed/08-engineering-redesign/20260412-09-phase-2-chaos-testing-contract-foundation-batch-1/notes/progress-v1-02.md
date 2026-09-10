# Progress v1.02

日期：2026-04-12 22:32:38 +08:00

## 本轮完成

- 新增 `src/reference/Chaos.TestFramework/Chaos.TestFramework.csproj` 与 `Chaos.TestFramework.cs`，冻结 `ChaosUnitTestAttribute`、`ChaosBenchmarkAttribute` 以及紧凑枚举公开面。
- 新增 `build/toolchains/run/testing/declarations.py`，落地 `testDeclarationMode`、stable declared test id 与声明扫描结论的第一层 typed helper。
- 在 `build/toolchains/run/testing/subjects.py` 中补上 `testDeclarationMode` capability 默认值。
- 新增/更新自动化测试，覆盖 reference project、合同 surface、declaration mode 与现有 subject manifest 默认行为。

## 验证结果

- `python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py -q` -> `3 passed`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q` -> `9 passed`
- `dotnet build src/reference/Chaos.TestFramework/Chaos.TestFramework.csproj -c Release` -> `Build succeeded. 0 Warning(s) 0 Error(s)`
- `python -m pytest -q` -> `688 passed, 40 skipped`

## 下一步提示

- 下一批进入真实 declaration discovery：程序集 metadata 扫描、catalog compiled model 与 registry/selector 接线。
