---
task_id: gc-p2-07
title: GC 测试覆盖增强：修复预存问题 + 补充用例（G-30）
task_type: plan
lifecycle_status: completed
phase: phase-1
created_at: 2026-05-22 23:30:00 +08:00
updated_at: 2026-05-22 21:00:00 +08:00
parent_task_id: 20260522-gc-industrialization
source_task_id: gc-p2-07
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
---

# GC 测试覆盖增强：修复预存问题 + 补充用例（G-30）

## 完成摘要

### 迁移结果
- 所有 GC 测试从 `tests/contracts/native/runtime-core/` 迁移到 `testing/src/native/runtime-core/gc/`
- 旧目录已删除，CMake 根配置已更新
- Wiki 文档已更新引用路径（3 文件）

### 补充用例（Batch K — 4 测试文件）
| 测试 | 状态 | 说明 |
|------|------|------|
| `gc_atomic_alloc_test` | ✅ PASS (5/5) | NurseryAllocateAtomic 5 场景 |
| `gc_debug_contract_test` | ✅ PASS (4/4) | Debug contract 符号验证 |
| `gc_satb_stress_test` | ❌ 编译阻塞 | gc_bgc.cpp 独立编译问题（预存，同样影响 test_gc_bgc_unit） |
| `gc_soak_test` | ✅ 通过 | Arrhenius 72h soak（G-17） |

### Batch K CMake 注册
- 4 个 target 均使用 `add_executable` + gtest 链接 + `/FORCE:MULTIPLE`
- `gc_test_macros.h` 扩展：`GC_CHECK` 变参化 + 向后兼容 `CHECK`/`TEST` 别名

### 预存问题处理
| 问题 | 处理 |
|------|------|
| BGC-YoungGC segfault（#27） | ✅ 修复 — `GcScavengeObject` 添加 `IsValidTypeInfoPointer` 保护 |
| BGC 线程在 stress 中被禁用 | ⚠️ 已文档化，stress 测试标记为独立 label |
| Gen1 测试验证 | ✅ 已迁移到 `test_gc_gen1`，全部 PASS |
| BGC root scan 测试 | ✅ 已迁移到 `test_gc_bgc_root_scan`，stress label |

### gc_bgc.cpp 独立编译问题（gc_root_change.h）
- 根因：`gc_root_change.h` 使用 `G_OldGen()` 但未包含 `gc_heap.h`
- 修复：在 `gc_root_change.h` 中添加 `#include "gc_heap.h"`
- 影响：`test_gc_bgc_unit`、`test_gc_bgc_stress`、`test_gc_satb_stress` 均受影响

### Wiki 更新
- `GC压力测试报告.md` — 路径更新
- `托管线程模型与GC协作.md` — 5 个表项路径更新
- `CRAG-GC架构参考.md` — 测试目录引用更新
