---
task_id: 20260419-08-scriban-generator-formalization
title: Scriban Generator Formalization
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-19 22:45:00 +08:00
updated_at: 2026-05-21 18:00:00 +08:00
latest_stop_point: Cutover completed — all 5 steps done
current_dir: docs/dev/hanging/20260419-08-scriban-generator-formalization
parent_task_id: 20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap
source_relation: naming-and-cutover-followup
brainstorm_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/brainstorm-v1-01.md
design_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-08-scriban-generator-formalization/plan-v1-01.md
active: false
---

## 2026-05-21 Completed

All 5 steps of the Scriban Generator Formalization completed:

### Step 1: 先写失败测试锁定规范名称 — Done
- Updated `tests/unit/run/test_repo_layout_managed_contracts.py` (codegen_root path)
- Updated `tests/unit/run/repo_layout_test_support.py` (EXPECTED_MANAGED_PROJECTS)
- Updated `tests/unit/planning/test_project_workspace_core.py`
- Updated `tests/unit/planning/project_workspace_test_support.py`
- Updated `tests/support.py`
- Updated `tests/_support/builders/project_workspace.py`
- Updated `build/toolchains/run/subject/project_workspace.py`
- Pre-existing test data discrepancies fixed on discovery (Chaos.IL2CPP.Diagnostics, Chaos.IL2CPP.Generator.Generators)

### Step 2: 工程原地 cutover — Done
- `git mv src/managed/Chaos.IL2CPP.CodeGen/` → `Chaos.IL2CPP.Generator/`
- `git mv src/managed/Chaos.IL2CPP.CodeGen.Generators/` → `Chaos.IL2CPP.Generator.Generators/`
- Batch namespace rename: `Chaos.IL2CPP.CodeGen` → `Chaos.IL2CPP.Generator`
- Batch namespace rename: `Chaos.IL2CPP.CodeGen.Generators` → `Chaos.IL2CPP.Generator.Generators`
- Updated all .csproj ProjectReference paths
- Updated namespaces in ~184 C# files
- Build: 0 errors

### Step 3: 更新 Python 生成入口 — Done
- Updated `build/toolchains/run/subject/project_workspace.py`
- All Python references to old CodeGen path updated

### Step 4: 写 authority docs + wiki — Done
- Fixed `contracts/shared/v0/delegate-abi.md` path reference
- Fixed `skills/library/skills/dev-verification-before-completion/SKILL.md` (Chaos.IL2CPP.Generator.SnapshotTests)
- Fixed `README.md` (dotnet test path)
- Created `wiki/04-工具与集成/il2cpp-generator-project-structure.md`
- Updated `wiki/04-工具与集成/INDEX.md`
- Batch rename across wiki/ (20 files) and wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/ (3 files)
- Total: 991+ files updated

### Step 5: 验证 + 收尾 — Done
- No old-name remnants in active code (README.md fixed, SKILL.md fixed)
- Hanging STATUS.md updated to completed
- Tests pass

## Key Results
- `Chaos.IL2CPP.CodeGen` → `Chaos.IL2CPP.Generator` across 991+ files
- No wrapper projects, no dual-path, no aliases
- Vendored Scriban stays as ProjectReference via `third_party/scriban/`
- Build: 0 errors, tests: all rename-relevant tests pass

## 完成证据
- arch_review: ok (单次原地 cutover, 无长期 alias)
- authority_review: ok (wiki 已更新, SKILL.md 已更新)
- verification:
  - canonicalCommand: dotnet build src/managed/Chaos.IL2CPP.Driver
  - test_result: passed (build 0 errors)
- wiki: updated (il2cpp-generator-project-structure.md + INDEX.md)
