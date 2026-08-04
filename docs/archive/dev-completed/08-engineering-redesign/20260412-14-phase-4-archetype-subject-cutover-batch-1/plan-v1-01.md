# Phase 4 Archetype Subject Cutover Batch 1 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。
> 目标：把 `subjects/golden/*` 迁移为 `Solution*` archetype subjects，同时保留旧入口兼容层，打通第一批 solution-first 工程级 archetype subject 主链。

## 计划任务

- [ ] Task 1: 冻结 archetype cutover 范围并盘点引用
  - 范围：
    - 盘点 `subjects/golden/GoldenSimpleLib`
    - 盘点 `subjects/golden/GoldenMultiProject`
    - 盘点 `subjects/golden/GoldenWithPackage`
    - 盘点源码、测试、文档中对 `golden` 与 `convert-golden-multi-project` 的真实引用
  - 验收：
    - 明确三类 archetype 的目标目录与兼容策略
    - 明确需要保留 alias 的入口点

- [ ] Task 2: 通过失败测试冻结 `Solution*` archetype 行为与兼容要求
  - 范围：
    - 为 registry/path resolution/planner 增加 `Solution*` canonical subject 断言
    - 为 legacy `subjects/golden/*` alias 兼容增加断言
    - 为 `convert-golden-multi-project` 迁移方向增加断言或 TODO 防护
  - 验收：
    - 新测试在实现前失败
    - 失败原因来自 archetype cutover 尚未落地

- [ ] Task 3: 实现 archetype subject cutover 与 compatibility shell
  - 范围：
    - 新建或迁移 `subjects/SolutionSimpleLib`
    - 新建或迁移 `subjects/SolutionMultiProject`
    - 新建或迁移 `subjects/SolutionPackageReference`
    - 让 `subjects/golden/*` 退化为 compatibility alias/shell
    - 调整 registry/planner/path resolver/fixtures 适配新的 canonical 路径
  - 验收：
    - `Solution*` 成为 canonical subject
    - 旧 `golden` 入口仍可解析
    - archetype subject 的 `engineeringProfile` 与 source metadata 可稳定编译进 catalog

- [ ] Task 4: 完成验证并归档本批次
  - 范围：
    - 运行新增定向测试
    - 运行 `python -m pytest -q`
    - 更新状态、进度记录、索引与父 roadmap
  - 验收：
    - 本 batch 全部验证通过
    - 父 roadmap 下一步明确转入后续 Phase 4/5 主线

## 验证

- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/integration/registry/test_registry_scan.py -q`
- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py -q`
- `python -m pytest -q`

## 执行备注

- 本批先处理 archetype subject 的 canonical 归位与兼容入口，不一次性清理全部 legacy root。
- `convert-golden-multi-project` 的最终工程 workload 收口可以先以 catalog/selector 归属切换为主，物理目录迁移按验证成本分批推进。
- 仅搜索源码、测试、文档中的真实引用，避免被 `artifacts/.tmp-tests/` 一类临时产物污染。
