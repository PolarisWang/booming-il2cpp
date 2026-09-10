# Phase 0 Contract Freeze And AOT Translation Ledgers 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 冻结 HybridCLR 对齐路线的 Phase 0 基线，把 reference bundle、两级 capability taxonomy、stable identity、`body availability` 和 AOT 双台账落成正式文档。

**架构：** 本阶段不改运行时代码；先把后续所有实现批次都会反复引用的 owner、id、surface、unsupported reason 变成单一事实源。Phase 1 之后的代码和 subject 迁移都必须以这批 freeze 文档为边界，不再继续扩展 string 协议面。

**技术栈：** Markdown、`docs/dev` 状态维护、PowerShell/本地代码事实复核、既有 `docs/architecture` 文档复用。

**设计文档：** `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/design-v1-01.md`

**预期知识沉淀：** 先留在本子任务目录；后续若 Phase 2-6 证明这些口径稳定，再合并到 `docs/architecture`。

---

## 文件与职责

- `STATUS.md`
  - 记录本 child task 的状态、摘要、下一步和风险。
- `plan-v1-01.md`
  - 记录本阶段执行步骤与验证方法。
- `reference-bundle-freeze-v1-01.md`
  - 冻结 canonical base bundle、controlled supplement、消费 archetype 与刷新条件。
- `capability-taxonomy-freeze-v1-01.md`
  - 冻结两级 capability taxonomy，以及 family / item 到 owner subject 的归属规则。
- `stable-identity-and-body-availability-v1-01.md`
  - 冻结 family enum + item enum 的 stable numeric identity 组合方式，明确 `body availability` 的下一阶段实现口径。
- `aot-translation-surface-ledger-v1-01.md`
  - 把目标 AOT translation surface 分成 language / compiler-lowered / runtime / metadata / ABI 五大类，并绑定 owner / evidence。
- `aot-unsupported-surface-ledger-v1-01.md`
  - 把当前明确 partial / unsupported / deferred 的条目记录为稳定台账，并绑定 reason code。
- `notes/progress-*.md`
  - 记录本轮执行摘要。

## 执行步骤

- [x] Task 1: 复核母 roadmap、architecture 文档、retained subjects、reference bundle 与 `Chaos.TestFramework` 当前事实。
- [x] Task 2: 产出 `reference-bundle-freeze-v1-01.md`。
- [x] Task 3: 产出 `capability-taxonomy-freeze-v1-01.md`。
- [x] Task 4: 产出 `stable-identity-and-body-availability-v1-01.md`。
- [x] Task 5: 产出 `aot-translation-surface-ledger-v1-01.md`。
- [x] Task 6: 产出 `aot-unsupported-surface-ledger-v1-01.md`。
- [x] Task 7: 自检 5 份文档的 naming、owner、reason code、bundle 边界与现有仓库事实一致。
- [x] Task 8: 更新 child `STATUS.md`、父 roadmap `STATUS.md`、`docs/dev/ACTIVE.md` 和索引。

## 验证

- 逐份核对新文档与以下事实是否一致：
  - `subjects/SolutionCorePack/source`
  - `subjects/HotUpdateHostPack/source`
  - `subjects/MixedExecutionFeaturePack/source`
  - `assets/reference-bundles/dotnet-foundation`
  - `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`
- 重点确认：
  - 所有 level-1 capability family 都能映射到明确 owner subject；
  - 所有 level-2 capability item 都能映射到 family，不再依赖新增 string 协议；
  - `AOT translation surface ledger` 覆盖 language、compiler-lowered、runtime service、metadata/closure、interop/ABI 五类；
  - `unsupported` / `partial` 条目带稳定 reason code，且 reason code 只来自 `docs/architecture/unsupported-feature-policy.md` 已冻结词汇；
  - `reference bundle` 的 base / supplement 边界能解释当前 `ReferenceAssemblySolution`、`CoreLibReferenceSolution`、`MixedReferenceClosureSolution` 和 patch 场景。

## 状态维护

- 每完成一份 freeze 文档，更新 `STATUS.md` 的最近摘要和下一步。
- 完成本阶段后，回写父 roadmap，使其下一步切到 `Phase 1: Subject Source Reshape And Test Metadata Cutover`。
