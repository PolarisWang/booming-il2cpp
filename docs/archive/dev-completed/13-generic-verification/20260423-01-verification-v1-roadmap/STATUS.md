---
task_id: 20260423-01-verification-v1-roadmap
title: Verification V1 Roadmap
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-23 12:25:58 +08:00
updated_at: 2026-04-23 17:59:31 +08:00
latest_stop_point: roadmap completion review 已重新通过；20260423-05 关闭了 formal codegen-stubs sink 与 legacy test-pipeline public entry 两项 blocker，verification-v1 回到 completed
current_dir: docs/dev/completed/20260423-01-verification-v1-roadmap
parent_task_id:
source_task_id:
source_relation: formalized-from-docs-architecture-verification-v1-roadmap
roadmap_or_plan: wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child:
active: false
preflight_review: warn
preflight_summary: completion gate reopen 时的两个 blocker 已在原批准边界内修复；本轮 closeout 只需要重做 verification、arch review 与索引归档
auto_execution_decision: continue
---

# 20260423-01 Verification V1 Roadmap

## 当前状态

- 状态: `completed`
- 完成阶段: `Roadmap CLOSEOUT`
- 当前目标: verification-v1 全部 phase 已收口并归档到 `docs/dev/completed/`

## 当前结论

- verification-v1 的 Phase 1 到 Phase 6 已全部闭环；此前 completion review 发现的两项 blocker 已由 `20260423-05-verification-v1-gap-reopen` 关闭
- formal source 现已固定到：
  - `docs/testing-inventory/verification/latest/*`
  - `docs/testing-inventory/verification/master/*`
  - `docs/testing-inventory/verification/reports/<closure-kind>/<scope-code>/*`
  - `subjects/<OwnerPack>/verification/codegen-stubs/*`
- public `test-pipeline` 已退出 manifest / CLI / TUI / registry public list；内部 pipeline object 只作为 registry expansion 与 regression support 保留
- `run test inventory` 已能稳定重建 formal archive；report 中出现的 `blocked / missing` 状态是 inventory 对当前覆盖现状的正式表达，不再是 rollout blocker
- roadmap closeout 不再存在未解决 completion blocker

## 本轮产物

- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/roadmap-v1-01.md`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/cutover-and-rollout.md`
- `docs/dev/completed/20260423-02-verification-v1-projection-cutover/STATUS.md`
- `docs/dev/completed/20260423-03-verification-v1-wiki-cutover/STATUS.md`
- `docs/dev/completed/20260423-04-verification-v1-legacy-retirement/STATUS.md`
- `docs/dev/completed/20260423-05-verification-v1-gap-reopen/STATUS.md`
- `docs/testing-inventory/verification/*`
- `subjects/SolutionCorePack/verification/codegen-stubs/*`

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `verification-v1`
- capabilityItem: `formal-inventory-cutover-roadmap`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/testing-inventory/verification/latest`
  - `docs/testing-inventory/verification/master`
  - `docs/testing-inventory/verification/reports`
  - `subjects/*/verification/codegen-stubs/*`
  - `wiki/06-测试验证/Verification-V1测试流程规范.md`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 子任务闭环

- `20260423-02-verification-v1-projection-cutover`: completed
- `20260423-03-verification-v1-wiki-cutover`: completed
- `20260423-04-verification-v1-legacy-retirement`: completed
- `20260423-05-verification-v1-gap-reopen`: completed，关闭最终 completion blocker

## 架构审视

- arch_review_mode: `critical`
- 结论: `ok`
- duplicate_code:
  - formal source 已统一到 verification bundle / projection 链路，不再并行维护旧 docs authority 与新 formal authority
  - public test entrypoint 已只保留一条主线，没有继续暴露旧 `pipeline` 公共别名
- structure:
  - `inventory_source -> verification_bundle -> verification_projection / verification_reporting` 的边界已稳定
  - codegen 存根已锚定到 owner subject，满足与 managed owner 代码对照的正式归档要求
- warn:
  - codegen stub 当前仍按 capability x profile 复制 baseline refs；若 profile 维度继续增长，可再考虑 shared catalog 去重
  - internal pipeline registry model 仍存在于内部 scan / expansion 逻辑中，但已与 public authority 解耦

## 权责图审核

- authority:
  - formal verification archive: `docs/testing-inventory/verification/*`
  - formal codegen archive: `subjects/<OwnerPack>/verification/codegen-stubs/*`
  - canonical wiki entry: `wiki/06-测试验证/Verification-V1测试流程规范.md`
  - public test UX: manifest / CLI / TUI / registry public list
- upstream:
  - `build/toolchains/run/testing/inventory_source.py`
  - `build/toolchains/run/testing/verification_bundle.py`
  - `build/toolchains/run/testing/verification_projection.py`
  - `build/toolchains/run/testing/verification_reporting.py`
  - `build/toolchains/run/testing/verification_codegen_stubs.py`
  - `build/toolchains/run/core/manifest.py`
  - `build/toolchains/run/commands/test.py`
  - `build/toolchains/run/testing/registry.py`
  - `build/toolchains/run/tui.py`
- downstream:
  - `docs/testing-inventory/*`
  - `docs/benchmark/*`
  - `wiki/06-测试验证/*`
  - `subjects/*/verification/codegen-stubs/*`
- allowed_deps:
  - `subjects/*/baselines/codegen/*` 只作为 migration/read-only source
  - internal pipeline manifests 只保留在 registry expansion 层
- shim_debt:
  - internal `pipeline/*` object model 仍是 registry 组合展开兼容层，但不再泄漏到 public 主入口
- proof:
  - child `20260423-05` 的 regression、CLI 与 inventory regeneration 已全部通过
  - child `20260423-02/03/04` 的 formal source / wiki / legacy retirement 产物继续有效

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand:
    - `python -m unittest tests.unit.verification.test_verification_bundle tests.unit.performance.test_testing_inventory_source tests.tooling.run.test_command_manifest_structure tests.tooling.run.test_command_manifest_cli tests.tooling.run.test_tui_unified_test_menu tests.tooling.run.test_tui_unified_routing_progress tests.tooling.run.test_registry_command_list tests.integration.run.test_command_manifest tests.integration.run.test_tui`
    - `python -m unittest tests.tooling.run.test_registry_command_module_pipeline tests.integration.registry.test_registry_scan_core tests.integration.registry.test_registry_scan_pipeline_plans tests.tooling.run.test_interactive_session`
    - `powershell -NonInteractive -File 'tests/tooling/run/test_run_cli.ps1'`
    - `powershell -NonInteractive -File 'tests/integration/run/test_run_cli.ps1'`
    - `python build/toolchains/run/run.py test inventory --output docs/testing-inventory`
  - formalObject:
    - `docs/testing-inventory/verification/latest`
    - `docs/testing-inventory/verification/master`
    - `docs/testing-inventory/verification/reports`
    - `subjects/SolutionCorePack/verification/codegen-stubs/*`
    - `wiki/06-测试验证/Verification-V1测试流程规范.md`
  - summaryPath: `docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md`
  - eventsPath: `n/a`
  - consolePath: `n/a`
- test_result: passed
- wiki: updated:`wiki/06-测试验证/Verification-V1测试流程规范.md`
- next: n/a

## Roadmap Completion Review

- formal `codegen-stubs` sink: `passed`
- `subjects/<OwnerPack>/verification/codegen-stubs/*` materialization: `passed`
- old `test-pipeline` public main entry retirement: `passed`
- manifest / CLI / TUI / registry public consistency: `passed`
- latest/master/reports regeneration: `passed`
- child closeout + archive consistency: `passed`

## 验证

- `python -m unittest tests.unit.verification.test_verification_bundle tests.unit.performance.test_testing_inventory_source tests.tooling.run.test_command_manifest_structure tests.tooling.run.test_command_manifest_cli tests.tooling.run.test_tui_unified_test_menu tests.tooling.run.test_tui_unified_routing_progress tests.tooling.run.test_registry_command_list tests.integration.run.test_command_manifest tests.integration.run.test_tui`
  - 结果: `45 tests, OK, skipped=6`
- `python -m unittest tests.tooling.run.test_registry_command_module_pipeline tests.integration.registry.test_registry_scan_core tests.integration.registry.test_registry_scan_pipeline_plans tests.tooling.run.test_interactive_session`
  - 结果: `16 tests, OK`
- `powershell -NonInteractive -File 'tests/tooling/run/test_run_cli.ps1'`
  - 结果: `run CLI tests passed`
- `powershell -NonInteractive -File 'tests/integration/run/test_run_cli.ps1'`
  - 结果: `run CLI tests passed`
- `python build/toolchains/run/run.py test inventory --output docs/testing-inventory`
  - 结果: `outputRoot: docs/testing-inventory`, `artifacts: 179`
- `python json.load(...)` 检查 `docs/testing-inventory/verification/latest/*.json` 与 `master/closure-master.json`
  - 结果: `OK`
- `Get-ChildItem -Recurse 'subjects' -Filter 'stub-index.json' | Measure-Object`
  - 结果: `153`

## 下一步

- `n/a`
