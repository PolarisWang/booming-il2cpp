---
task_id: 20260412-08-phase-1-generic-test-taxonomy-cleanup
title: Phase 1 Generic Test Taxonomy Cleanup
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-12 18:39:06 +08:00
updated_at: 2026-04-12 22:04:48 +08:00
current_dir: docs/dev/completed/20260412-08-phase-1-generic-test-taxonomy-cleanup
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260412-08-phase-1-generic-test-taxonomy-cleanup/plan-v1-01.md`

## 当前判断

- current_focus: Phase 1 已完成首轮 `tests/` taxonomy cleanup，canonical fixtures / buckets / platform-hosts / root pytest 入口已全部收口。
- why_done: 目标中的 registry manifests、platform host roots、generic unit buckets 和 root-level pytest 收集边界都已落到真实结构与自动化约束。
- done_definition: Phase 1 exit criteria 满足，可回到父 roadmap 规划下一阶段的 taxonomy / framework cutover。

## 最近摘要

- 2026-04-12 18:39:06 +08:00: 创建 Phase 1 子任务并接管 ACTIVE。
- 2026-04-12 18:39:06 +08:00: 已把 integration fixture canonical root 切到 `tests/fixtures/integration/catalog/repo`。
- 2026-04-12 18:39:06 +08:00: 已把 `test_adapter_registry.py`、`test_suite_manifest.py`、`test_catalog_scan.py`、`test_case_discovery.py`、`test_registry_scan.py` 迁到新的 registry bucket，并保留旧 `run/` shim。
- 2026-04-12 18:49:49 +08:00: repo layout 与旧入口 shim 回归通过，总计 33 passed。
- 2026-04-12 18:55:33 +08:00: 已把 `tests/gate` / `tests/platform` 的真实 host 工程提升到 `tests/integration/platform-hosts`，根 `CMakeLists.txt` 切到 canonical 路径。
- 2026-04-12 22:04:48 +08:00: 已把 registry manifests canonical root 收到 `tests/fixtures/registry/*`，并补 `pytest.ini` 固定根级收集只覆盖仓库 `tests/`。
- 2026-04-12 22:04:48 +08:00: 已对齐 runtime-baseline / platform-hosts / native-aot 相关陈旧断言；`python -m pytest -q` 全量通过，结果为 `683 passed, 40 skipped`。

## 下一步

- next_action: 返回 `20260412-06-test-system-suite-redesign` 父 roadmap，规划 Phase 2 对 `tests/tooling/run`、`tests/integration/run` 与 framework API 化的后续切片。
- owner: codex
- trigger: 父 roadmap 准备进入下一阶段 taxonomy / framework 结构调整时恢复。

## 风险 / 阻塞

### risks

- 兼容 shim 会在短期内让 canonical 路径与旧 `run/` 路径并存，后续需要明确退役时点。
- `build/toolchains/run/testing/registry.py` 与根 `CMakeLists.txt` 仍绑定旧路径，迁移 platform/gate 与 registry manifest 时要同步切兼容层。

### blockers

- 当前无外部 blocker。
