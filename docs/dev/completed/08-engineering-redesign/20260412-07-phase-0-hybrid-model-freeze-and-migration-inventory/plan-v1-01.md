# Phase 0 Hybrid Model Freeze And Migration Inventory 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。步骤使用复选框语法跟踪。

**目标：** 冻结 `v1.03` hybrid model 的执行基线，并完成第一版 legacy subject / tests inventory、命名规则、边界矩阵与迁移映射。

**架构：** 本阶段不修改业务代码，只处理任务执行骨架、结构盘点与规范冻结文档。输出以文档为主，作为后续 Phase 1-3 的唯一输入基线。

**技术栈：** Markdown、YAML 状态文件、PowerShell、仓库结构盘点

**设计文档：** `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

**父 roadmap：** `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`

**预期知识沉淀：** 暂不写 wiki；先沉淀在当前子任务目录文档中，待 Phase 0 完成后再判断是否需要抽取长期知识。

---

## 计划任务

- [ ] Task 1: 建立 Phase 0 子任务执行骨架，并切换 `docs/dev/ACTIVE.md` 到本子任务。
  - 目标文件：
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/STATUS.md`
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/plan-v1-01.md`
    - `docs/dev/ACTIVE.md`
    - `docs/dev/in-progress/20260412-06-test-system-suite-redesign/STATUS.md`
  - 验收：
    - 子任务成为当前 ACTIVE 入口
    - 父任务明确记录当前 active child

- [ ] Task 2: 盘点现有 `subjects/` 与 `tests/`，输出 inventory。
  - 输出文件：
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/legacy-subject-inventory-v1-01.md`
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/tests-taxonomy-inventory-v1-01.md`
  - 内容要求：
    - subject 顶层 root 分类
    - `golden` / proof-like / benchmark-like / mainline / mobile / hot-update 分类
    - `tests/` 现有 bucket 与目标 bucket 的映射
  - 验收：
    - 所有现有顶层 subject 都被纳入某个分类
    - `tests/` 现有主目录都有目标归宿建议

- [ ] Task 3: 冻结命名规则、边界矩阵与迁移映射。
  - 输出文件：
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/naming-rules-v1-01.md`
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/engineering-vs-declared-boundary-v1-01.md`
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/legacy-to-solution-mapping-v1-01.md`
  - 内容要求：
    - `Solution*` archetype 命名规则
    - `*Proof` / `Bench*` 非默认新增规则
    - `ChaosUnitTest` / `ChaosBenchmark` 与 engineering workload 的边界
    - `assets/` 与 C# helper API 的边界
  - 验收：
    - 能回答每个 legacy root 的长期归宿
    - 能回答 `convert/codegen/native executable` 不属于 `ChaosBenchmark`

- [ ] Task 4: 更新状态、记录 progress，并判断 Phase 0 是否完成。
  - 目标文件：
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/STATUS.md`
    - `docs/dev/in-progress/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/notes/progress-v1-01.md`
    - `docs/dev/in-progress/20260412-06-test-system-suite-redesign/STATUS.md`
    - `docs/dev/ACTIVE.md`
  - 验收：
    - 子任务状态与父任务状态一致
    - 若 Phase 0 完成，子任务写明 exit criteria 已满足

## 验证

- 文档交叉检查：
  - `design-v1-03.md`
  - `roadmap-v1-03.md`
  - Phase 0 输出的 inventory / mapping / boundary 文档
- 状态检查：
  - `docs/dev/ACTIVE.md`
  - 父任务 `STATUS.md`
  - 子任务 `STATUS.md`

## 执行备注

- 本阶段默认不改实现代码。
- 如果在 inventory 过程中发现 `v1.03` 文档仍有明显歧义，可直接修正文档，但必须同步记入 progress。
