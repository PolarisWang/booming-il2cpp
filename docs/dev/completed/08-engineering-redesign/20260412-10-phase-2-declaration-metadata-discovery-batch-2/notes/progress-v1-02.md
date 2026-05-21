# Progress v1.02

日期：2026-04-13 00:03:00 +08:00

## 本轮完成

- 新增 `src/managed/Chaos.IL2CPP.DeclarationDiscovery/`，通过 `System.Reflection.Metadata` 对编译程序集做 custom attribute 扫描，不执行被测程序集。
- 新增 `build/toolchains/run/testing/compiled_catalog.py`，提供 managed discovery tool build/run wrapper 与 `build_declared_test_catalog(...)`。
- 扩展 `build/toolchains/run/testing/declarations.py`，让 declaration entry 可以承载稳定 ID 之外的紧凑 metadata 字段。
- 新增 `tests/fixtures/contracts/declared-metadata/` 三类 fixture，覆盖无框架引用、有框架无声明、有真实声明三种状态。
- 新增 `tests/integration/registry/test_declared_metadata_discovery.py`，冻结 compiled catalog 的输出形状、稳定 ID 和 metadata-only 扫描约束。

## 验证结果

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q` -> `3 passed`
- `python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py -q` -> `3 passed`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q` -> `9 passed`
- `dotnet build src/managed/Chaos.IL2CPP.DeclarationDiscovery/Chaos.IL2CPP.DeclarationDiscovery.csproj -c Release` -> `Build succeeded. 0 Warning(s) 0 Error(s)`
- `python -m pytest -q` -> `691 passed, 40 skipped`

## 下一步提示

- 下一批次进入 Phase 3，优先冻结 thin manifest 与 `engineeringProfile` 的 typed foundation，再考虑 shared profile 与 manifest cutover。
