# Phase 3 Engineering Core Compiled Catalog Batch 3 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。
> 目标：在不切动 selector/CLI 主入口的前提下，先建立 engineering-core compiled catalog foundation，把 `engineeringProfile` 投影成稳定的 `engineeringValidations[]` / `engineeringWorkloads[]` 结构，并与已存在的 declared catalog schema 对齐。

## 计划任务

- [ ] Task 1: 激活 Phase 3 Batch 3 执行任务
  - 范围：
    - 归档 Phase 3 Batch 2
    - 新建 `docs/dev/in-progress/20260412-13-phase-3-engineering-core-compiled-catalog-batch-3/`
    - 更新 `docs/dev/ACTIVE.md`、父 roadmap 状态与索引
  - 验收：
    - `ACTIVE.md` 指向 Batch 3
    - 父 roadmap 明确记录 Batch 2 已完成，Batch 3 已启动

- [ ] Task 2: 通过失败测试冻结 engineering-core compiled catalog 最小 schema
  - 范围：
    - 为 compiled catalog 新增 `engineeringValidations[]`
    - 为 compiled catalog 新增 `engineeringWorkloads[]`
    - 用 `managed-output` / `native-executable` / `hot-update-host` 三种 profile 冻结第一版投影
  - 验收：
    - 新测试在实现前失败
    - 失败原因来自缺失的 engineering-core compiled catalog 投影

- [ ] Task 3: 落地 engineering profile projection 与 compiled subject catalog merge
  - 范围：
    - 在 `build/toolchains/run/testing/compiled_catalog.py` 新增 engineering-core projection
    - 把 subject manifest、orchestration、工程级条目与 declared catalog 合并成统一 compiled subject catalog
    - 保持声明发现工具链不被破坏
  - 验收：
    - 同一个 subject 可以同时产出 engineering entries 与 declared entries
    - 工程级条目不依赖业务方法 attribute

- [ ] Task 4: 完成验证并归档本批次
  - 范围：
    - 运行新增定向测试
    - 运行 `python -m pytest -q`
    - 更新状态、进度记录与索引
  - 验收：
    - 本 batch 全部验证通过
    - 父 roadmap 下一步明确转入 Phase 4 archetype cutover 或继续 Phase 3 尾批

## 验证

- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/integration/registry/test_registry_scan.py -q`
- `python -m pytest -q`

## 执行备注

- 本批只做 compiled subject catalog foundation，不在本批切 selector/CLI/artifact 主入口。
- engineering profile 到 validation/workload 的第一版映射可以先偏保守，但必须稳定、可测试、可扩展。
- shared profile 已在 Batch 2 落地；本批默认直接消费归一化后的 manifest。
