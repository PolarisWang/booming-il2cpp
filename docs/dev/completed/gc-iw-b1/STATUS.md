---
task_id: gc-iw-b1
title: 并发根扫描接入点集成（C6）
task_type: plan
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-crag-gc-industrialization-sequel
source_relation: roadmap-child
clearance_source: parent-roadmap
created_at: 2026-05-23
updated_at: 2026-05-23
current_dir: docs/dev/completed/gc-iw-b1
---

# STATUS — gc-iw-b1：并发根扫描接入点集成（C6）

## 完成证据

- arch_review: ok
  - 新增 `BgcSatbPreWriteBarrier` + `BgcRecordRootChange` 在 codegen stsfld 发射路径
  - `BgcRecordRootChange` 用 `g_bgc_is_marking` 守卫，避免非 BGC 周期 fetch_add 开销
  - 与 interpreter Handle_StSFld 路径一致（interpreter 已有相同屏障）
- authority_review: ok
  - codegen emission 确保 AOT GC root write barrier 与 interpreter 对等
  - GCHandle 写入路径（GcSetHandleTarget、GcSetDependentHandleSecondary）审计通过：已有屏障
  - GCHandle 创建路径（CreateStrong/Weak/LongWeak/Pinned）：新句柄无旧值，无需 root change
- verification:
  - canonicalCommand: foundation-dll verification pipeline (garbage-collection family)
  - formalObject: codegen output containing stsfld barriers, entry.exe fact mode
  - consolePath: "Passed: 10/10"
- test_result: passed（10/10 subjects pass — fact mode）
- compilation: dotnet build Chaos.IL2CPP.Generator 0 errors
- pipeline: `garbage-collection` family building OK（build_subjects OK, convert_to_cpp OK, build_entry_exe OK）
- wiki: n/a
- next: gc-iw-b2（Parallel mark lock-free deque 改造）

## 改动文件

- `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs` — stsfld 发射增加 BgcSatbPreWriteBarrier + BgcRecordRootChange

## 设计摘要

### 问题

BGC concurrent mark 期间，AOT codegen 发射的 `stsfld`（静态字段写入）缺少 root change tracking。静态字段是 GC roots，改写 root slot 可能丢失对象的唯一引用。interpreter 的 `Handle_StSFld` 已有屏障，但 codegen 路径缺失。

### 修复

在 ExceptionEmission.cs 的 `case "stsfld"` 块中，在写入 `chaos_static_X` 前增加：
1. `BgcSatbPreWriteBarrier` — SATB 预写屏障（记录旧值，保护并发标记不丢失 old-gen 引用）
2. `g_bgc_is_marking` 守卫 + `BgcRecordRootChange` — root change 跟踪（仅在 BGC 标记期间记录）

### 审计结论

| 路径 | BgcSatbPreWriteBarrier | BgcRecordRootChange | 状态 |
|------|:---------------------:|:-------------------:|:----:|
| Interpreter Handle_StSFld | ✅ 有 | ✅ 有 | 正确 |
| Codegen stubs CodegenStSFld | ✅ 有 | ✅ 有 | 正确 |
| GcSetHandleTarget | ✅ 有 | ✅ 有 | 正确（已审计） |
| GcSetDependentHandleSecondary | ✅ 有 | ✅ 有 | 正确（已审计） |
| **Codegen stsfld (ExceptionEmission.cs)** | ✅ **已添加** | ✅ **已添加** | **修复** |
| BGC re-mark drain (gc_bgc.cpp) | n/a | ✅ 有 | 正确 |

### preflight self-review

- preflight_review: pass
- auto_execution_decision: continue

## 结构告警与架构审视

1. **重复代码**: 无。stsfld 屏障模式与 stfld 实例字段路径一致
2. **结构与抽象**: 正确。屏障在 codegen emission 层保持，不侵入 Scriban 模板
3. **文件职责**: 清晰。ExceptionEmission.cs 负责结构化 EH 线性化发射
4. **codegen/Scriban 分层**: 正确。策略决策（何时发射屏障）在 C# 宿主代码中，不在模板中
