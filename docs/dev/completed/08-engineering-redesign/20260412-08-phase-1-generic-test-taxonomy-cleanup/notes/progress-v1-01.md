# Phase 1 进展记录 v1.01

日期: 2026-04-12 18:39:06 +08:00

## 本批落地

- 新增 canonical fixtures 目录：`tests/fixtures/integration/catalog/repo`
- 新增 canonical unit bucket：`tests/unit/registry/`
- 新增 canonical integration bucket：`tests/integration/registry/`
- 新增 canonical platform host bucket：`tests/integration/platform-hosts/`
- 完成以下文件迁移：
  - `tests/unit/run/test_adapter_registry.py` -> `tests/unit/registry/test_adapter_registry.py`
  - `tests/unit/run/test_suite_manifest.py` -> `tests/unit/registry/test_suite_manifest.py`
  - `tests/integration/run/test_catalog_scan.py` -> `tests/integration/registry/test_catalog_scan.py`
  - `tests/integration/run/test_case_discovery.py` -> `tests/integration/registry/test_case_discovery.py`
  - `tests/integration/run/test_registry_scan.py` -> `tests/integration/registry/test_registry_scan.py`
- 完成以下 host 工程 canonicalization：
  - `tests/gate/android-smoke` -> `tests/integration/platform-hosts/android-smoke`
  - `tests/gate/ios-smoke` -> `tests/integration/platform-hosts/ios-smoke`
  - `tests/gate/linux-packaging` -> `tests/integration/platform-hosts/linux-packaging`
  - `tests/platform/*` 与 `tests/gate/*` 旧目录改为 wrapper
- 旧 `run/` 路径保留兼容 shim，避免当前命令入口直接失效。
- `tests/unit/run/test_repo_layout.py` 新增断言，锁定第一批 canonical 目录。
- `tests/integration/registry/test_registry_scan.py` 对齐当前 canonical system id，去掉对 `roadmap-0-*` 旧 id 的依赖。
- 根 `CMakeLists.txt` 已切到 `tests/integration/platform-hosts/*` canonical 路径。

## 验证

- `python -m pytest tests/unit/run/test_repo_layout.py -k phase1_registry_buckets_and_integration_fixtures_exist -q`
  - 结果：1 passed
- `python -m pytest tests/unit/registry/test_adapter_registry.py tests/unit/registry/test_suite_manifest.py tests/integration/registry/test_catalog_scan.py tests/integration/registry/test_case_discovery.py tests/integration/registry/test_registry_scan.py -q`
  - 结果：14 passed
- `python -m pytest tests/unit/run/test_adapter_registry.py tests/unit/run/test_suite_manifest.py tests/integration/run/test_catalog_scan.py tests/integration/run/test_case_discovery.py tests/integration/run/test_registry_scan.py -q`
  - 结果：14 passed
- `python -m pytest tests/unit/run/test_repo_layout.py tests/unit/run/test_adapter_registry.py tests/unit/run/test_suite_manifest.py tests/integration/run/test_catalog_scan.py tests/integration/run/test_case_discovery.py tests/integration/run/test_registry_scan.py -q`
  - 结果：33 passed
- `python -m pytest tests/unit/run/test_repo_layout.py -q`
  - 结果：20 passed
- `python -m pytest tests/unit/run/test_repo_layout.py tests/unit/run/test_adapter_registry.py tests/unit/run/test_suite_manifest.py tests/integration/run/test_catalog_scan.py tests/integration/run/test_case_discovery.py tests/integration/run/test_registry_scan.py -q`
  - 结果：34 passed

## 下一步建议

- 下一批优先处理 `tests/registry` manifest 的目录归类与 loader 兼容。
- 再处理更多 `tests/unit/run` -> `tests/unit/{planning,execution,reporting,performance,...}`。
- 最后逐批拆掉剩余 `run/` / wrapper 兼容层与 `phase/stage` 命名文件。
