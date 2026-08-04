# Phase 0 Coverage Ledger And Subject Verification Freeze 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 冻结新的母 roadmap 所需 capability owner 台账、完整验证 checklist、reference bundle 策略与 Phase 0 决策。

**架构：** 本阶段不修改运行时代码，先把后续所有批次都会依赖的词汇、owner 和证据规则冻结成文档产物。Phase 1 及之后的代码任务必须以这些 freeze 文档为准，不再反复讨论边界。

**技术栈：** Markdown、`docs/dev` 状态维护、PowerShell 自检、现有 architecture 文档复用。

**设计文档：** `docs/dev/in-progress/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`

**预期知识沉淀：** 先留在本子任务目录；若后续长期稳定，再在 Phase 5/6 同步到 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考`。

---

## 文件与职责

- `STATUS.md`
  - 记录本 child task 的状态、摘要、下一步和风险。
- `plan-v1-01.md`
  - 记录本阶段执行步骤与验证方法。
- `coverage-ledger-freeze-v1-01.md`
  - 冻结 capability item、owner subject、owner archetype、proof/benchmark/support-state 的台账。
- `subject-verification-checklist-v1-01.md`
  - 冻结“subject 工程有完整验证能力”的统一 checklist。
- `reference-bundle-policy-freeze-v1-01.md`
  - 冻结 reference/corelib/patch bundle 的来源、边界、刷新和验收规则。
- `phase-0-decisions-freeze-v1-01.md`
  - 汇总本阶段默认采用的工程判断。
- `notes/progress-*.md`
  - 记录本轮执行摘要。

## 执行步骤

- [x] Task 1: 复核父 roadmap、capability matrix、reference policy 与真实 retained subject 目录。
- [x] Task 2: 产出 `coverage-ledger-freeze-v1-01.md`。
- [x] Task 3: 产出 `subject-verification-checklist-v1-01.md`。
- [x] Task 4: 产出 `reference-bundle-policy-freeze-v1-01.md`。
- [x] Task 5: 产出 `phase-0-decisions-freeze-v1-01.md`。
- [x] Task 6: 自检文档之间的 naming、owner、archetype、bundle 策略与父 roadmap 一致。
- [x] Task 7: 更新 child `STATUS.md`、父 roadmap `STATUS.md` / `roadmap-v1-01.md`、`docs/dev/ACTIVE.md` 与索引。

## 验证

- 逐份阅读 4 份 freeze 文档，确认：
  - capability item 命名与父 roadmap / architecture matrix 一致；
  - owner subject / owner archetype 一致；
  - complete verification checklist 可直接作为后续 child task 验收标准；
  - reference bundle 策略与当前仓库真实路径一致；
  - decisions freeze 没有和既有架构文档冲突。
- 读取 `subjects/*/source` 和 `assets/reference-bundles` 相关事实，确认文档不是凭空假设。

## 状态维护

- 每完成一份 freeze 文档，更新 `STATUS.md` 的最近摘要和下一步。
- 完成后回写父 roadmap，使 `20260414-13` 标记为 `completed`，并把下一步切到 `20260414-14-phase-1-subject-verification-closure`。
