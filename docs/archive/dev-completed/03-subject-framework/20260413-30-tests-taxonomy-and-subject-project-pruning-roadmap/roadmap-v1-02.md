# Tests Taxonomy And Subject Project Pruning Roadmap v1.02

Date: 2026-04-13
Status: completed

## 1. 目标回顾

本 roadmap 的完成目标是：

- 顶层 `subjects/` 只保留 3 个 retained subject。
- 一次性删除 compatibility alias。
- 删除或吸收 legacy 顶层 subject。
- 把 `tests/` 从旧 subject taxonomy 迁回 generic mechanism + retained smoke。
- 保留 retained solution-mode subject 的 proof / benchmark 真跑证据。

## 2. 最终结果

### Phase 0: Inventory Freeze
- `completed`
- 冻结 retained / migrate / delete 清单，并明确只保留：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`

### Phase 1: Solution-Mode Catalog Foundation
- `completed`
- solution-mode retained subject 的 declared catalog / registry / planner / selector 入口已经收口到 retained subject id。

### Phase 2: SolutionCorePack Source Absorption
- `completed`
- `SolutionCorePack` 不再作为空壳 subject 依赖顶层 legacy source。
- 已吸收的顶层 legacy subject 已删除。

### Phase 3: Public Entry And Gate Cutover
- `completed`
- public smoke / gate / run manifest / TUI 已切换到 retained subject 或 fixture。

### Phase 4: Tests Taxonomy Cleanup
- `completed`
- 修复并迁移了 planner / subject command / compatibility / performance 里残留的旧 taxonomy 假设。
- 旧的 `benchmark` category、`perf-release` profile、`csharp-perf-harness`、subject-owned xUnit 假设已从核心回归里清除。
- 需要保留的非 live 行为验证已转成 fixture subject。

### Phase 5: Verification And Archive
- `completed`
- retained subject proof / benchmark 真跑证据已保留。
- 相关 broader suite 已完整通过。
- 任务已归档到 `docs/dev/completed/`.

## 3. 本轮收尾修改

- 修复 `build/scripts/verify-runtime-baseline.py`
  - 从 `core.tooling` 导入，补齐 `resolve_subject_matrix_subject_id(...)` helper。
- 清理测试中的旧假设
  - `tests/unit/planning/test_subject_planner.py`
  - `tests/tooling/run/test_subject_command.py`
  - `tests/unit/performance/test_subject_perf_policy.py`
  - `tests/unit/compatibility/test_managed_closure_contract_bundle.py`
  - `tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py`
- 更新 managed closure 文档
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/managed-minimal-closure-v0.md`

## 4. 验收标准

以下标准均满足：

- retained subject 真实 proof / benchmark 有 runId 证据。
- 最小失败子集回归恢复为全绿。
- 与 subject taxonomy、registry、planner、command、compatibility 相关的 broader suite 全绿。
- `docs/dev` 状态文档已完成归档。

## 5. 后续建议

- 如果后续继续追求“更纯的 generic mechanism test”，下一条 follow-up 可以专门处理剩余 `select_subject_record(...)` 用法，把其中非 retained smoke 的部分继续下沉到 fixture 层。
