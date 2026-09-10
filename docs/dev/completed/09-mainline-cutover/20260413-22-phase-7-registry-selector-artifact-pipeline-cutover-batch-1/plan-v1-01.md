# Phase 7 Registry Selector Artifact Pipeline Cutover Batch 1 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion` 执行本计划。
> 目标：让 compiled catalog 成为 registry / selector / CLI / report / artifact 的真实统一主入口。
> 设计文档：`docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

---

## 计划任务

- [ ] Task 1: 先写 RED tests，冻结 unified catalog / selector / CLI 合同
  - 范围：
    - 扩展 registry scan 测试，断言统一对象包含：
      - `engineering-validation`
      - `engineering-workload`
      - `declared-unit-test`
      - `declared-benchmark`
    - 扩展 command manifest / CLI 解析测试，断言支持：
      - `run test engineering-validation --id ...`
      - `run test engineering-workload --id ...`
      - `run test declared-unit-test --id ...`
      - `run test declared-benchmark --id ...`
    - 扩展 reporting / command 测试，断言 entry family 会进入 selected object、summary、family-specific report 路径
  - 验收：
    - 新断言在实现前失败
    - 失败原因直接指向 registry projection、selector 语义或 report/artifact 归属缺失

- [ ] Task 2: 实现 unified registry projection 与 selector/CLI 命令
  - 范围：
    - 把 compiled catalog 接入 registry scan
    - 为 engineering / declared family 建立稳定 object id、canonical command、selected object 元数据
    - 扩展 selector parser、run manifest 与 test command dispatch
  - 验收：
    - registry snapshot 可以列出新的 family 对象
    - CLI 可以解析并定位到对应 registry object

- [ ] Task 3: 实现 family-specific execution / reporting / artifact routing
  - 范围：
    - planner 支持 family-specific source/workload entry override
    - test command 能把 engineering / declared object 转成稳定 subject execution request
    - matrix report / subject summary / session summary 增加 family 归属字段
    - family-specific report 路径至少区分：
      - `engineering/validations`
      - `engineering/workloads`
      - `declared/unit`
      - `declared/benchmark`
  - 验收：
    - selected object 执行结果可追溯到具体 entry family
    - report 不再把 engineering workload 与 declared benchmark 混在同一层 summary/path

- [ ] Task 4: 完成验证并推进父 roadmap 到 Phase 8
  - 范围：
    - 运行定向 tests
    - 运行 `python -m pytest -q`
    - 更新 STATUS / ACTIVE / 索引 / 父 roadmap 状态
  - 验收：
    - 本批验证通过
    - 父 roadmap 明确进入 Phase 8 compatibility closure and legacy cleanup

## 关键改动文件

- `build/toolchains/run/testing/compiled_catalog.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/testing/selectors.py`
- `build/toolchains/run/testing/subject_planner.py`
- `build/toolchains/run/testing/subject_reporting.py`
- `build/toolchains/run/testing/reporting.py`
- `build/toolchains/run/commands/test.py`
- `build/toolchains/run/core/manifest.py`
- `build/toolchains/run/run_manifest.json`
- `tests/integration/registry/test_registry_scan.py`
- `tests/integration/run/test_command_manifest.py`
- `tests/integration/run/test_unified_test_command.py`
- `tests/unit/reporting/test_subject_reporting.py`

## 验证

- `python -m pytest tests/integration/registry/test_registry_scan.py -q`
- `python -m pytest tests/integration/run/test_command_manifest.py tests/integration/run/test_unified_test_command.py -q`
- `python -m pytest tests/unit/reporting/test_subject_reporting.py -q`
- `python -m pytest -q`
