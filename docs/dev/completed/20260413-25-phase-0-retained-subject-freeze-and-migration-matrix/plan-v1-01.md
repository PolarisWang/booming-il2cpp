# Phase 0 Retained Subject Freeze And Migration Matrix 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。步骤使用复选框语法跟踪。

**目标：** 冻结方案 A 下的 retained subject 清单、准入规则和 legacy-to-retained 迁移矩阵，并把父 roadmap 从设计状态推进到可执行状态。

**架构：** 本阶段不修改生产代码，只处理执行骨架、盘点文档、迁移矩阵和执行边界冻结。输出将作为 Phase 1 显式 `.sln` 升级和后续实际代码迁移的唯一前置输入。

**技术栈：** Markdown、YAML 状态文件、PowerShell、仓库目录盘点

**设计文档：** `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/design-v1-01.md`

**父 roadmap：** `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`

**预期知识沉淀：** 暂不写 wiki；先沉淀在本子任务目录中，等 Phase 0 完成后再判断是否需要抽取长期知识。

---

## 计划任务

- [x] Task 1: 建立 Phase 0 子任务执行骨架，并切换 `docs/dev/ACTIVE.md` 到本子任务。
  - 目标文件：
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/STATUS.md`
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/plan-v1-01.md`
    - `docs/dev/ACTIVE.md`
    - `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/STATUS.md`
  - 验收：
    - 子任务成为当前 `ACTIVE` 入口
    - 父任务明确记录当前 active child

- [x] Task 2: 盘点现有 `subjects/` 与 `tests/`，输出 inventory。
  - 输出文件：
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/legacy-subject-inventory-v1-01.md`
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/tests-taxonomy-inventory-v1-01.md`
  - 内容要求：
    - subject 顶层 root 全量清单与分类
    - retained subject 与 legacy subject 的初步区分
    - `tests/` 当前 bucket 与后续保留/迁移职责
  - 验收：
    - 所有现有顶层 subject 都被纳入某个分类
    - `tests/` 当前主目录都有目标归宿建议

- [x] Task 3: 冻结 retained subject 准入规则与 legacy-to-retained 迁移矩阵。
  - 输出文件：
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/retained-subject-rules-v1-01.md`
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/executable-plan-admission-rules-v1-01.md`
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/legacy-to-retained-mapping-v1-01.md`
  - 内容要求：
    - retained subject 只保留三类：
      - `SolutionCorePack`
      - `HotUpdateHostPack`
      - `MixedExecutionFeaturePack`
    - 明确 compatibility alias 最终必须一次性清零
    - 明确何时允许新增顶层 subject
    - 明确每个 legacy root 的长期归宿形式
  - 验收：
    - 能回答任意 legacy root 的长期归宿
    - 能回答为什么不允许新增新的 `*Proof` / `Bench*`

- [x] Task 4: 更新状态、记录 progress，并关闭 Phase 0。
  - 目标文件：
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/STATUS.md`
    - `docs/dev/in-progress/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/notes/progress-v1-01.md`
    - `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/STATUS.md`
    - `docs/dev/ACTIVE.md`
    - `docs/dev/INDEX.md`
    - `docs/dev/in-progress/INDEX.md`
  - 验收：
    - 子任务状态与父任务状态一致
    - 若 Phase 0 完成，子任务明确写明 exit criteria 已满足

## 验证

- 文档交叉检查：
  - `design-v1-01.md`
  - `roadmap-v1-01.md`
  - Phase 0 输出的 inventory / rules / mapping 文档
- 状态检查：
  - `docs/dev/ACTIVE.md`
  - 父任务 `STATUS.md`
  - 子任务 `STATUS.md`

## 执行备注

- 本阶段默认不改生产代码。
- 如发现父设计或 roadmap 仍有与 “alias 最终一次性清零” 冲突的措辞，应同步修正文档并记入 progress。
