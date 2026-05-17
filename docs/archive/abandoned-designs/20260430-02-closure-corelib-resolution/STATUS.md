---
task_id: 20260430-02-closure-corelib-resolution
title: CoreLib Closure Resolution via manifest assembly include
task_type: plan
lifecycle_status: superseded
phase: superseded
created_at: 2026-04-30 00:00:00 +08:00
updated_at: 2026-05-01 14:30:00 +08:00
current_dir: docs/dev/abandoned/20260430-02-closure-corelib-resolution
parent_task_id:
source_task_id:
source_relation: standalone
clearance_source: direct user confirmation
design_doc: n/a
plan_doc: docs/dev/abandoned/20260430-02-closure-corelib-resolution/plan-v1-01.md
active: false
superseded_by: Reference Assembly architecture (implemented in LoaderStage + LinkerStage)

---

# 20260430-02 CoreLib Closure Resolution

## Outcome

**SUPERSEDED.** 原计划在 `batch_native_aot_runner.py` 中将 `System.Private.CoreLib.dll` 加入 manifest `assemblies` 列表，让 Linker BFS trace 跨程序集调用。

实际实现了更好的方案——**Reference Assembly** 架构：
- CoreLib 以 metadata-only 模式加载，不参与 codegen
- 保留了 callee 解析能力（`_methodsBySubjectId`），但 BFS 不展开 CoreLib 方法
- 避免了 IL body 解码膨胀、GenericInstantiationDemand 冲突、IsDefined 崩溃
- 32/32 foundation-dll families native AOT pipeline 全部通过

## 关系

- `batch_native_aot_runner.py` 当前 manifest 只含 `[dll_path]` — 这是正确的，CoreLib 通过 pipeline 内部 `referenceAssemblies` 机制加载
- 本计划的设计已完全被 Reference Assembly 替代
