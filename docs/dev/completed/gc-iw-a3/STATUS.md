---
task_id: gc-iw-a3
title: Codegen verification 管线打通 + 托管 API 完整验证（C3）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-a3
---

# STATUS — gc-iw-a3：Codegen verification 管线打通 + 托管 API 完整验证（C3）

## 完成证据

- arch_review: ok
  - 10 个 subject 覆盖托管 GC API 完整范围：Collect（2 overloads）、WaitForPendingFinalizers、KeepAlive、GetGeneration、Array::CreateInstance、GetTotalMemory、GetGCMemoryInfo、CollectionCount、GetTotalPauseDuration
  - 非确定性情断言已全部移除（GetHashCode 比较 → 有意义的值域检查：非负整数/正内存值/数组长度验证）
  - 管线配置只追加 family 到 FAMILIES 列表，不修改 codegen 核心或 GC native 代码
- verification:
  - canonicalCommand: foundation-dll verification pipeline（`pipeline_native_aot_runner.py`）
  - formalObject: `garbage-collection` family — 10 subjects, 10/10 通过
  - asm-compare: 7/7 methods OK（initial 7 subjects）, overall IR expansion ratio 0.18, max 0.34
  - lowering: 所有 methods 有 lowering, ALIGNED
  - p3_patch_entry: 7 entries, all with interrupt_ptr [OK]
  - principle_alignment: ALIGNED（12 checks: 8 aligned, 1 concern, 3 not_applicable）
- test_result: passed（10/10 subjects pass — Subject_0 到 Subject_9 在 Fact 和 Fact JIT 阶段全部通过）
- compilation: 所有 codegen 输出 + CMake native build 通过
- pipeline: `garbage-collection` 已加入 FAMILIES 列表（共 35 families），与其他 22 non-report CoreLib families 同时运行
- wiki: n/a（无长期有效新知识，C3 是验证集成，不改算法或架构）
- next: gc-iw-b1（并发根扫描接入点集成）

## 改动文件

- `verification/foundation-dll/System.Private.CoreLib/garbage-collection/managed/subjects/GarbageCollectionSubjects.cs` — 修复断言（移除 GetHashCode 比较）+ 扩充 3 个新 subject（Subject_7/8/9）
- `build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py` — 将 `garbage-collection` 加入 FAMILIES 列表

## 设计摘要

### 问题

GC 相关 managed API（GC.Collect, GC.GetGCMemoryInfo, LatencyMode 等）的 codegen verification pipeline 尚未集成到批量运行管线中。7 个已有 subject 使用非确定性断言（GetHashCode 比较），Fact 阶段未独立验证。

### 范围

- `verification/foundation-dll/System.Private.CoreLib/garbage-collection/` — 修复 subject 断言 + 扩充 API 覆盖
- `build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py` — 加入 garbage-collection family

### preflight self-review

- 范围检查：在父 roadmap 已批准的 C3 边界内（verification/, codegen/）
- 依赖检查：gc-iw-a2（已完成），Phase A 基线稳定
- authority 检查：改 verification/ 和 pipeline 配置，不改 codegen 核心或 GC native 代码
- preflight_review: pass
- auto_execution_decision: continue

### 执行计划完成情况

1. ~~Fix GC subject assertions (remove non-deterministic hash code compares)~~ ✅
2. ~~Add garbage-collection to pipeline FAMILIES list~~ ✅
3. ~~Add additional subjects for remaining GC managed APIs (CollectionCount, GetTotalPauseDuration, Collect overload)~~ ✅
4. ~~Run pipeline to verify end-to-end~~ ✅ — 10/10 通过

