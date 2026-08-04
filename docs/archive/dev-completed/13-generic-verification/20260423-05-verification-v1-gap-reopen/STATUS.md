---
task_id: 20260423-05-verification-v1-gap-reopen
title: Verification V1 Gap Reopen
task_type: task
lifecycle_status: completed
phase: completed
created_at: 2026-04-23 15:48:20 +08:00
updated_at: 2026-04-23 17:59:31 +08:00
latest_stop_point: reopen 缺口已关闭；formal codegen-stubs 正式落地，公开 test-pipeline 主入口已退役，并已按 roadmap 要求重跑回归与 test inventory
current_dir: docs/dev/completed/20260423-05-verification-v1-gap-reopen
parent_task_id: 20260423-01-verification-v1-roadmap
source_task_id: 20260423-01-verification-v1-roadmap
source_relation: reopened-after-completion-review
roadmap_or_plan: wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/roadmap-v1-01.md
clearance_source: parent-roadmap
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child:
active: false
preflight_review: warn
preflight_summary: roadmap reopen 时发现的两项 blocker 都在已批准边界内；本轮只做 formal sink 接线、旧主入口退役与 completion review 重验
auto_execution_decision: continue
---

# 20260423-05 Verification V1 Gap Reopen

## 关键文档

- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/roadmap-v1-01.md`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
- `build/toolchains/run/testing/inventory_source.py`
- `build/toolchains/run/testing/verification_bundle.py`
- `build/toolchains/run/testing/verification_codegen_stubs.py`
- `build/toolchains/run/core/manifest.py`
- `build/toolchains/run/commands/test.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/tui.py`

## 当前结论

- `write_verification_bundle()` 已把 `codegenStubs` 正式写入 `subjects/<OwnerPack>/verification/codegen-stubs/<capability>/<route>[-<profile>]/stub-index.json`
- `collect_inventory_source()` 已从 `subjects/*/baselines/codegen/*` 与 owner declared methods/tests/benchmarks 合成 formal `codegenStubs`
- `test-pipeline` 已从 public manifest、动态 CLI 解析、TUI 菜单和 registry public list 中退役；内部 `pipeline/*` 对象仅保留给 registry expansion / regression tests
- `python build/toolchains/run/run.py test inventory --output docs/testing-inventory` 已重跑成功，并再次 materialize `docs/testing-inventory/verification/latest|master|reports`
- 本轮 inventory formal report 仍会显示部分 `blocked / missing` capability evidence；这是当前测试覆盖现状的正式表达，不是 verification-v1 rollout 的残留 blocker

## 问题清零

- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `verification-v1`
- capabilityItem: `gap-reopen-closeout`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/testing-inventory/verification/latest`
  - `docs/testing-inventory/verification/master`
  - `docs/testing-inventory/verification/reports`
  - `subjects/*/verification/codegen-stubs/*`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 架构审视

- arch_review_mode: `critical`
- 结论: `ok`
- duplicate_code:
  - public 测试主入口已统一到 `suite/subject/module/system/inventory`，没有继续保留并行的 public `pipeline` selector
  - codegen stub formal sink 复用 `verification_codegen_stubs.py`，没有引入第二套落盘路径
- structure:
  - `inventory_source.py` 只负责 source synthesis，`verification_bundle.py` 只负责 bundle/build/write，边界清晰
  - internal pipeline registry model 仍可服务 member expansion，但不再承担 public authority
- warn:
  - `_collect_codegen_stubs()` 当前按 `owner capability x baseline profile` 物化 stub index；如果 profile 继续扩张，后续可考虑抽 shared profile catalog 降低重复引用
  - `_codegen_route_from_profile()` 仍通过 profile 名称推断 route；当前 profile 已被回归测试锁住，但新增 profile 时更适合补显式 route metadata

## 权责图审核

- authority:
  - formal archive: `docs/testing-inventory/verification/*`
  - formal codegen stub sink: `subjects/<OwnerPack>/verification/codegen-stubs/*`
  - public test entrypoints: manifest / CLI / TUI / registry public list，不再暴露 `test-pipeline`
- upstream:
  - `build/toolchains/run/testing/inventory_source.py`
  - `build/toolchains/run/testing/verification_bundle.py`
  - `build/toolchains/run/testing/verification_codegen_stubs.py`
  - `build/toolchains/run/core/manifest.py`
  - `build/toolchains/run/commands/test.py`
  - `build/toolchains/run/testing/registry.py`
  - `build/toolchains/run/tui.py`
- downstream:
  - `docs/testing-inventory/*`
  - `subjects/SolutionCorePack/verification/codegen-stubs/*`
  - public `run test ...` / interactive menu / registry list flows
- allowed_deps:
  - `subjects/*/baselines/codegen/*` 只作为 migration 期 read-only source
  - `tests/fixtures/registry/pipelines/*` 只作为内部 registry expansion source
- shim_debt:
  - internal `pipeline/*` object model 仍保留在 registry 中服务组合展开，但已与 public UX 解耦，不构成 completed blocker
- proof:
  - 回归测试通过
  - `test inventory` 正式生成 latest/master/reports 并 materialize codegen stub indexes

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
  - summaryPath: `docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md`
  - eventsPath: `n/a`
  - consolePath: `n/a`
- test_result: passed
- wiki: n/a
- next: close parent roadmap

## 验证命令

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
- `python json.load(...)` 检查 `docs/testing-inventory/verification/latest/closure-snapshot.json`、`stage-snapshot.json`、`result-snapshot.json` 与 `master/closure-master.json`
  - 结果: 全部 `OK`
- `Get-ChildItem -Recurse 'subjects' -Filter 'stub-index.json' | Measure-Object`
  - 结果: `153` 个 formal stub indexes
