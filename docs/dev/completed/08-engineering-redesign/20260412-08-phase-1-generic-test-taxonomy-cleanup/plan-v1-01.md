# Phase 1 Generic Test Taxonomy Cleanup 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划。

目标：把 `tests/` 从“按历史阶段和混合入口堆放”逐步迁到“按通用功能分类”的 canonical 布局，并在迁移期间保留必要兼容入口。

## 计划任务

- [x] Task 1: 选定第一批低风险迁移对象
  - 范围：
    - integration fixtures
    - registry / discovery 相关 unit tests
    - registry / discovery 相关 integration tests
  - 验收：
    - 不直接触碰 `tests/platform` / `tests/gate` / `tests/registry` 的底层 loader 约束
    - 先验证 canonical bucket 迁移流程可行

- [x] Task 2: 落地第一批 canonical 目录
  - 目标结构：
    - `tests/fixtures/integration/catalog/repo`
    - `tests/unit/registry/`
    - `tests/integration/registry/`
  - 迁移对象：
    - `tests/integration/run/fixtures/catalog/...`
    - `tests/unit/run/test_adapter_registry.py`
    - `tests/unit/run/test_suite_manifest.py`
    - `tests/integration/run/test_catalog_scan.py`
    - `tests/integration/run/test_case_discovery.py`
    - `tests/integration/run/test_registry_scan.py`
  - 验收：
    - canonical 路径存在
    - repo layout 测试能锁定新结构

- [x] Task 3: 保留兼容 shim 并完成首批验证
  - 动作：
    - 在旧 `run/` 路径放兼容 shim
    - 修正 fixture 路径引用
    - 对齐 registry scan 中的 canonical system id 断言
  - 验收：
    - canonical 路径测试通过
    - 旧 `run/` 入口测试也能通过

- [ ] Task 4: 继续下一批 taxonomy cleanup
  - 候选范围：
    - `tests/platform` / `tests/gate`
    - `tests/registry` manifests 的 canonical 归类
    - 更多 `tests/unit/run` -> `tests/unit/{planning,execution,reporting,performance,...}`
    - `tests/tooling/run` / `tests/integration/run` 的 CLI 与 TUI 归类
  - 验收：
    - 形成第二批迁移清单
    - 明确需要兼容层的代码入口和 loader

## 验证

- `python -m pytest tests/unit/run/test_repo_layout.py -k phase1_registry_buckets_and_integration_fixtures_exist -q`
- `python -m pytest tests/unit/registry/test_adapter_registry.py tests/unit/registry/test_suite_manifest.py tests/integration/registry/test_catalog_scan.py tests/integration/registry/test_case_discovery.py tests/integration/registry/test_registry_scan.py -q`
- `python -m pytest tests/unit/run/test_adapter_registry.py tests/unit/run/test_suite_manifest.py tests/integration/run/test_catalog_scan.py tests/integration/run/test_case_discovery.py tests/integration/run/test_registry_scan.py -q`

## 执行备注

- 本阶段优先做可局部验证、低耦合的目录迁移。
- 对 `tests/platform` / `tests/gate`、`tests/registry` 这类与 loader/CMake 直接耦合的路径，下一批要带兼容层一起切，不做裸搬迁。
