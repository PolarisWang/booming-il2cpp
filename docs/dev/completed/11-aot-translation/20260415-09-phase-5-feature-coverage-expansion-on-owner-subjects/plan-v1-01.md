# Phase 5 Feature Coverage Expansion On Owner Subjects 实现计划

> 面向执行 Agent：继续使用 `dev:executing-plans`；如出现 registry / discovery 假绿或测试失败，切到 `dev:systematic-debugging`。

## 目标

- 把 Phase 0 冻结的 capability ledger 正式映射回 retained owner subjects。
- 让 declared catalog / registry 能显式回答 owner subject / proof / support-state。
- 修复 declaration discovery 读取过期 assembly 的风险，保证 coverage 审计真实。

## 关键文档

- `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/capability-taxonomy-freeze-v1-01.md`
- `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-translation-surface-ledger-v1-01.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`

## 预计改动面

- `build/toolchains/run/testing/compiled_catalog.py`
- `build/toolchains/run/testing/declared_metadata_labels.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/testing/workspace_declared_catalog.py`
- `build/toolchains/run/testing/capability_coverage.py`
- `tests/unit/compatibility/test_compiled_subject_catalog.py`
- `tests/unit/compatibility/test_phase5_owner_subject_coverage.py`
- `tests/unit/registry/test_declared_registry_matrix_selection.py`
- `docs/dev/in-progress/20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects/STATUS.md`
- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`

## 执行步骤

- [ ] Task 1：补 Phase 5 RED 测试
  - registry declared item 需要显式暴露 capability family/item + owner/support-state
  - capability coverage 审计需要锁定三类 owner subject 的当前目标 item
  - declaration discovery 需要在 source 更新后避免复用过期 assembly
- [ ] Task 2：实现 capability coverage 聚合模块
  - 把 Phase 0 冻结的 owner/support-state 编成可复用模块
  - 输出 capability item -> owner subject / support-state / proof / benchmark 的聚合结果
- [ ] Task 3：扩充 declared catalog / registry metadata
  - registry declared item 写入 capability family/item 数值与 labels
  - registry declared item 写入 owner subject / support-state
- [ ] Task 4：修复 declaration discovery freshness
  - source 变更后重建 solution/project assembly
  - 避免 capability coverage 审计读到旧 dll
- [ ] Task 5：回归与文档回写
  - 跑 Phase 5 相关 pytest
  - 回写 child task 与 parent roadmap 状态

## 阶段目标

- owner-subject coverage 从“文档冻结”提升到“代码可审计”。
- registry declared item 成为 Phase 6 dashboard / unsupported report 的可直接消费输入。
- capability coverage 审计结果对真实源码变化敏感，不再受陈旧 build 输出影响。

## 验收标准

- `build/toolchains/run/testing` 中存在单一 capability coverage 事实源。
- registry 中每个 declared item 都带有 capability 数值与 owner/support-state。
- `SolutionCorePack` / `HotUpdateHostPack` / `MixedExecutionFeaturePack` 的当前目标 capability item 都能被聚合到 owner proof。
- source 更新后重新执行 declared discovery 能看到新增 capability item。
