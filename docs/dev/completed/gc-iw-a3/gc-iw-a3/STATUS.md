---
task_id: gc-iw-a3
title: Codegen verification 管线打通 + 托管 API 完整验证（C3）
task_type: plan
lifecycle_status: completed
phase: executing-plans
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/in-progress/gc-iw-a3
---

# STATUS — gc-iw-a3：Codegen verification 管线打通 + 托管 API 完整验证（C3）

## 设计摘要

### 问题

GC 相关 managed API（GC.Collect, GC.GetGCMemoryInfo, LatencyMode 等）的 codegen verification pipeline 尚未集成到批量运行管线中。7 个已有 subject 使用非确定性断言（GetHashCode 比较），Fact 阶段未独立验证。

### 范围

- `verification/foundation-dll/System.Private.CoreLib/garbage-collection/` — 修复 subject 断言 + 扩充 API 覆盖
- `build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py` — 加入 garbage-collection family
- `build/toolchains/run/testing/foundation_dll/family_verification_orchestrator.py` — 如需修复

### preflight self-review

- 范围检查：在父 roadmap 已批准的 C3 边界内（verification/, codegen/）
- 依赖检查：gc-iw-a2（已完成），Phase A 基线稳定
- authority 检查：改 verification/ 和 pipeline 配置，不改 codegen 核心或 GC native 代码
- preflight_review: pass
- auto_execution_decision: continue

### 执行计划

1. Fix GC subject assertions (remove non-deterministic hash code compares)
2. Add garbage-collection to pipeline FAMILIES list
3. Add additional subjects for remaining GC managed APIs (LatencyMode, CollectionCount, etc.)
4. Run pipeline to verify end-to-end

### 改动文件

- `verification/foundation-dll/System.Private.CoreLib/garbage-collection/managed/subjects/GarbageCollectionSubjects.cs`
- `build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py`

## 下一步

1. 修复 subject 断言
2. 加入 FAMILIES 列表
3. 扩充 API 覆盖
4. 运行管线验证
