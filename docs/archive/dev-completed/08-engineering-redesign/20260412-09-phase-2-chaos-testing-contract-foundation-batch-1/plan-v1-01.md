# Phase 2 Chaos Testing Contract Foundation Batch 1 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。

目标：完成 `Chaos.TestFramework` 的最小可执行合同冻结，并把 `testDeclarationMode` 与 declaration foundation 的第一层 typed 支撑落入仓库，使后续真正的 metadata discovery/cutover 有稳定边界。

## 计划任务

- [ ] Task 1: 激活 Phase 2 Batch 1 执行任务
  - 范围：
    - 新建 `docs/dev/in-progress/20260412-09-phase-2-chaos-testing-contract-foundation-batch-1/`
    - 更新 `docs/dev/ACTIVE.md`
    - 更新父 roadmap 状态与索引
  - 验收：
    - `ACTIVE.md` 指向当前 batch
    - `docs/dev/INDEX.md` 与 `docs/dev/in-progress/INDEX.md` 可见当前 batch

- [ ] Task 2: 通过失败测试冻结合同面
  - 范围：
    - 为 `src/reference/Chaos.TestFramework` 项目存在性与公开面补测试
    - 为 `testDeclarationMode` 默认值/显式值补测试
    - 为 declaration typed helper/stable id 规则补测试
  - 验收：
    - 新测试在实现前失败，并明确失败原因是能力缺失

- [ ] Task 3: 落地最小合同与 declaration foundation
  - 范围：
    - 新增 `src/reference/Chaos.TestFramework/`
    - 实现 `ChaosUnitTestAttribute` / `ChaosBenchmarkAttribute`
    - 实现紧凑枚举与枚举成员注释
    - 在 Python 侧实现 `testDeclarationMode` 规范化与 declaration helper
  - 验收：
    - `Chaos.TestFramework` 可单独 `dotnet build`
    - 公开面与设计一致，且不引入 orchestration 语义

- [ ] Task 4: 完成验证并归档本批次
  - 范围：
    - 运行定向 pytest
    - 运行 `dotnet build src/reference/Chaos.TestFramework/Chaos.TestFramework.csproj -c Release`
    - 运行全量 `python -m pytest -q`
    - 更新状态、进度记录与索引
  - 验收：
    - 本 batch 全部验证通过
    - 子任务可归档为 completed
    - 父 roadmap 下一步明确指向 Phase 2 Batch 2 discovery/cutover

## 验证

- `python -m pytest tests/unit/compatibility/test_repo_layout.py -q`
- `python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `dotnet build src/reference/Chaos.TestFramework/Chaos.TestFramework.csproj -c Release`
- `python -m pytest -q`

## 执行备注

- 本批只冻结最小合同与 typed discovery foundation，不直接切入 registry/catalog 主链。
- `src/reference/Chaos.TestFramework` 必须保持极薄，不承载 pipeline、device、budget、baseline 编排。
- discovery 第一批只解决 contract/identity/mode 规范，不在本批引入真正程序集 metadata 扫描实现。
