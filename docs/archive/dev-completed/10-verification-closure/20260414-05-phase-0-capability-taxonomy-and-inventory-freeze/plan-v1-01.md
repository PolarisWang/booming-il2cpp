# Phase 0 Capability Taxonomy And Inventory Freeze 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 冻结本轮 capability taxonomy、canonical archetype inventory、retained-subject ownership 与 OQ2/OQ3/OQ4/OQ5 的工程落地口径。

**架构：** 本阶段不改代码逻辑，先把 vocabulary 和边界固定成文档产物。后续 `Chaos.TestFramework`、planner、dashboard、retained-subject 迁移都必须以这些 freeze 文档为准，不再重新讨论同一组方向问题。

**技术栈：** Markdown、`docs/dev` 任务状态维护、PowerShell 文件核对。

**设计文档：** `docs/dev/in-progress/20260414-04-il2cpp-hotupdate-capability-expansion-roadmap/design-v1-01.md`

**预期知识沉淀：** 先保留在本子任务目录；Phase 5/6 再同步到 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考`

---

## 文件与职责

- `STATUS.md`
  - 记录当前阶段状态、结论、下一步
- `plan-v1-01.md`
  - 记录 Phase 0 的执行步骤与验证方式
- `capability-taxonomy-freeze-v1-01.md`
  - 冻结 capability family / level-2 item / support-state / benchmark policy
- `solution-archetype-freeze-v1-01.md`
  - 冻结 canonical archetype inventory 与 foundation bundle 口径
- `subject-target-structure-freeze-v1-01.md`
  - 冻结三个 retained subject 的目标结构与 ownership
- `phase-0-decisions-freeze-v1-01.md`
  - 汇总 OQ2/OQ3/OQ4/OQ5 的最终结论
- `notes/progress-*.md`
  - 记录本轮执行摘要

## 执行步骤

- [x] 把父 roadmap 已确认的 OQ2/OQ3/OQ4/OQ5 写回父文档
- [x] 盘点当前三个 retained subjects 的真实目录状态
- [x] 产出 capability taxonomy freeze 文档
- [x] 产出 solution archetype freeze 文档
- [x] 产出 subject target structure freeze 文档
- [x] 产出 phase-0 decisions freeze 文档
- [x] 自检文档之间的命名、ownership、archetype 与 capability 口径一致
- [x] 更新 `STATUS.md`、`docs/dev/ACTIVE.md`、索引与进展记录

## 验证

- 读取 4 份 freeze 文档，确认：
  - 同一 capability family 命名一致
  - archetype 名称一致
  - retained subject ownership 一致
  - OQ2/OQ3/OQ4/OQ5 的结论一致
- 读取 `subjects/*/source` 真实目录，确认目标结构不是凭空假设

## 状态维护

- 每完成一类 freeze 文档，更新 `STATUS.md` 的最近摘要与下一步
- 完成本阶段后，将 child task 回写父 roadmap，并切换到下一 child task
