# Phase 3 Thin Manifest Foundation Batch 1 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。
> 目标：在不批量改动现有 subject manifest 的前提下，为 Phase 3 建立 thin manifest typed foundation，并把 legacy heavy manifest 自动投影到新的薄字段上。

## 计划任务

- [ ] Task 1: 激活 Phase 3 Batch 1 执行任务
  - 范围：
    - 新建 `docs/dev/in-progress/20260412-11-phase-3-thin-manifest-foundation-batch-1/`
    - 更新 `docs/dev/ACTIVE.md`
    - 更新父 roadmap 状态与索引
  - 验收：
    - `ACTIVE.md` 指向当前 batch
    - `docs/dev/INDEX.md` 与 `docs/dev/in-progress/INDEX.md` 可见当前 batch

- [ ] Task 2: 通过失败测试冻结 thin manifest foundation
  - 范围：
    - 为 legacy heavy manifest 自动派生 `sourceModel` / `dependencyModel` / `executablePlan` / `engineeringProfile`
    - 为 `availability` / `compatibility` 默认值与显式覆盖补测试
    - 为 capability 暴露与 planner 兼容读取补测试
  - 验收：
    - 新测试在实现前失败，且失败原因来自缺失的 thin-manifest foundation

- [ ] Task 3: 落地 typed helper、manifest normalization 与兼容投影
  - 范围：
    - 在 `build/toolchains/run/testing/subjects.py` 新增 thin manifest enum/helper
    - 在 loader 层实现 legacy heavy manifest -> thin manifest 的默认投影
    - 让 `manifest_capabilities`、registry/planner 可读取新的薄字段，同时保留现有 heavy 字段主路径
  - 验收：
    - 现有 manifest 无需改写即可得到稳定的薄字段视图
    - 显式 thin 字段优先于自动派生
    - 现有 planner/registry 测试不回归

- [ ] Task 4: 完成验证并归档本批次
  - 范围：
    - 运行定向 pytest
    - 运行全量 `python -m pytest -q`
    - 更新状态、进度记录与索引
  - 验收：
    - 本 batch 全部验证通过
    - 父 roadmap 下一步明确指向 Phase 3 的后续 shared profile / engineering-core cutover

## 验证

- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/integration/registry/test_registry_scan.py -q`
- `python -m pytest -q`

## 执行备注

- 本批不批量编辑现有 47 个 direct subject manifest。
- 本批不引入 shared profile 外置目录，也不切断现有 `executionPipelines` / `environmentMatrices` 读取路径。
- `sourceModel` / `dependencyModel` / `executablePlan` / `engineeringProfile` 的默认派生要尽量保守，允许未来 manifest 显式覆盖。
