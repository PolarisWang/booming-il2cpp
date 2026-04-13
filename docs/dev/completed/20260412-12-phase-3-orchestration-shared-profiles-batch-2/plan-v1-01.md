# Phase 3 Orchestration Shared Profiles Batch 2 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。
> 目标：把 `subject.manifest.json` 中的 `orchestration` 引用正式接入 loader/planner/registry，先落 `matrixProfile` / `pipelineProfile` 的 shared profile 解析与兼容覆盖链路，并为后续 engineering-core schema cutover 建立稳定入口。

## 计划任务

- [ ] Task 1: 激活 Phase 3 Batch 2 执行任务
  - 范围：
    - 新建 `docs/dev/in-progress/20260412-12-phase-3-orchestration-shared-profiles-batch-2/`
    - 更新 `docs/dev/ACTIVE.md`
    - 更新父 roadmap 状态与索引
  - 验收：
    - `ACTIVE.md` 指向当前 batch
    - `docs/dev/INDEX.md` 与 `docs/dev/in-progress/INDEX.md` 可见当前 batch

- [ ] Task 2: 通过失败测试冻结 shared profile 解析行为
  - 范围：
    - 为 loader 增加 `orchestration.matrixProfile` / `orchestration.pipelineProfile` 的 RED 测试
    - 为 planner 增加共享 profile 展开后的选择与阶段规划 RED 测试
    - 为 registry 增加共享 profile 展开后的 subject 扫描 RED 测试
  - 验收：
    - 新测试在实现前失败
    - 失败原因来自缺失的 orchestration/shared profile 能力，而不是测试拼写错误

- [ ] Task 3: 落地 shared profile loader、path resolver 与兼容覆盖逻辑
  - 范围：
    - 在 `build/toolchains/run/testing/path_resolver.py` 增加 shared profile 路径解析
    - 在 `build/toolchains/run/testing/subjects.py` 增加：
      - `orchestration` 归一化
      - `matrixProfile` / `pipelineProfile` 解析
      - profile 内容到 `environmentMatrices` / `executionPipelines` 的展开
      - inline heavy fields 对 shared profile 的补充或覆盖
      - `budgetProfile` / `baselineProfile` 的透传归一化
    - 在 `subject_planner.py` / `registry.py` 暴露解析后的 orchestration 视图
  - 验收：
    - 仅声明 shared profile 的 subject manifest 可被 planner/registry 正确消费
    - 仍保留 inline heavy fields 的兼容路径
    - inline 项与 shared profile 同名时，以 subject 内声明为准

- [ ] Task 4: 完成验证并归档本批次
  - 范围：
    - 运行定向 pytest
    - 运行全量 `python -m pytest -q`
    - 更新状态、进度记录与索引
  - 验收：
    - 本 batch 全部验证通过
    - 父 roadmap 下一步明确指向 Phase 3 engineering-core schema 后续批次

## 验证

- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/integration/registry/test_registry_scan.py -q`
- `python -m pytest -q`

## 执行备注

- 本批优先建立 shared profile config root，不在本批批量迁移真实 subject。
- shared profile 先支持 `matrixProfile` / `pipelineProfile` 的强解析；`budgetProfile` / `baselineProfile` 先作为透明 orchestration 引用保留。
- 为避免最小 fixture 回归，本批必须继续允许“无 matrix/pipeline 的最小 manifest”在 loader 层被保守归一化。
