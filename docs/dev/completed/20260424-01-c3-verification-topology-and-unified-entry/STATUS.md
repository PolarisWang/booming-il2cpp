---
task_id: 20260424-01-c3-verification-topology-and-unified-entry
title: C3 Verification Topology And Unified Entry
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-24 00:00:00 +08:00
updated_at: 2026-04-24 06:00:00 +08:00
latest_stop_point: verification-v1 hard cut closed; authority and public entry moved to verification root; real commands re-verified.
current_dir: docs/dev/completed/20260424-01-c3-verification-topology-and-unified-entry
parent_task_id:
source_task_id: 20260423-01-verification-v1-roadmap
source_relation: follow-up architecture roadmap after verification-v1 completion
design_doc: docs/dev/completed/20260424-01-c3-verification-topology-and-unified-entry/design-v1-01.md
roadmap_or_plan: docs/dev/completed/20260424-01-c3-verification-topology-and-unified-entry/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child:
active: false
preflight_review: pass
preflight_summary: blocking questions cleared; roadmap scope aligned with verification-v1 authority; hard cut could proceed directly.
auto_execution_decision: continue
---

# 20260424-01 C3 Verification Topology And Unified Entry

## Outcome

- verification/ is now the only formal verification root.
- run.py remains the only real execution entry; .sln stays as IDE facade only.
- verification/archive, verification/evidence/owners, verification/projections, and verification/workspaces are all cut over.
- tests/verification is now the verification-framework test tree.
- wiki, skills, CLI/TUI, and architecture authority docs are on the new path vocabulary.
- legacy public entry under docs/testing-inventory, docs/benchmark, subjects/*/verification, and solutions is retired from mainline.

## Arch Review

- arch_review: ok
- authority_review: ok
- notes:
  - fixed relative-path breaks from verification/catalog/owners/*/support/host/*.sln to verification/catalog/scenarios/*
  - fixed generated managed/hotupdate host references to src/reference/* so verification/workspaces/subjects/* builds cleanly
  - added catalog support-host solution regression coverage for catalog/owners/scenarios and verification/src/reference path regressions

## Completion Evidence

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: python build/toolchains/run/run.py verify verification-v1 --json
  - formalObject: verification-v1
  - summaryPath: artifacts/logs/run/20260424-055625-windows-7200/summary.json
  - eventsPath: artifacts/logs/run/20260424-055625-windows-7200/events.jsonl
  - consolePath: artifacts/logs/run/20260424-055625-windows-7200/console.log
- verification_data:
  - refreshCommand: python build/toolchains/run/run.py verify verification-v1 --json
  - verificationOutputRoot: verification/
  - latestResultPath: verification/archive/latest/result-snapshot.json
  - masterResultPath: verification/archive/master/result-master.json
  - reportSummaryPath: verification/archive/reports/completed/testing-inventory/summary.md
  - codegenStubPath: verification/evidence/owners/SolutionCorePack/codegen-stubs/
- workspace_verification:
  - canonicalCommand: python build/toolchains/run/run.py generate project all --json
  - summaryPath: artifacts/logs/run/20260424-054801-windows-f748/summary.json
  - eventsPath: artifacts/logs/run/20260424-054801-windows-f748/events.jsonl
  - consolePath: artifacts/logs/run/20260424-054801-windows-f748/console.log
  - manifestPath: verification/workspaces/verification-all.manifest.json
  - solutionPath: verification/workspaces/verification-all.sln
- test_result: passed
- tests:
  - pytest tests/verification/unit/catalog/test_support_host_solution_contracts.py tests/verification/unit/archive tests/verification/unit/projections/testing_inventory tests/verification/unit/projections/benchmark tests/verification/tooling/run tests/unit/planning/test_generated_hotupdate_hosts.py tests/unit/planning/test_generated_managed_hosts.py tests/unit/planning/test_project_workspace_all.py tests/unit/planning/test_project_workspace_subject_generate_assets.py tests/unit/run/test_test_command_workspace.py -q
  - result: 56 passed, 3 subtests passed
- legacy_scan:
  - command: git grep -n -E "solutions/|docs/testing-inventory/verification|docs/benchmark/|subjects/.*/verification/codegen-stubs|catalog/owners/scenarios|verification/src/reference" -- . ":(exclude)docs/dev/**" ":(exclude).artifact/**"
  - result: clean
- wiki: updated wiki/06-测试验证/Verification-V1测试流程规范.md
- next: merge
