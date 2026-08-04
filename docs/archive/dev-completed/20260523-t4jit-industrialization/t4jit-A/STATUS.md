---
task_id: t4jit-A
lifecycle_status: completed
phase: completed
parent_task_id: 20260523-t4jit-industrialization
source_task_id: 20260523-t4jit-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
preflight_review: pass
preflight_summary: 改动编译通过，63/65 测试通过，2 个失败为预存问题（DeoptEntry_Registration、Fuzz），与 Phase A 无关。
---

# t4jit-A — GC 精确槽映射（已完成）

## 完成证据

- **改动：**
  - `code_generator.cpp:3615` — `Generate()` 中设置 `nm->gc_slot_map = reinterpret_cast<GcSlotMapV0*>(nm->slot_map_data)`
  - `code_generator.cpp:3665-3668` — `~NativeMethod()` 中调用 `GcUnregisterSlotMap(code)` 防止 dangling pointer
  - `code_generator.cpp:8` — 添加 `#include <gc_root_scanner.h>`
  - `native_method.h:48` — 移除 `TODO(Phase 3d)`，更新为描述性注释
- **构建:** ✅ `test_codegen_native` 编译通过
- **测试:** ✅ 63/65 通过（2 个预存失败，与 Phase A 无关）
- **arch_review:** ok — 改动集中在 2 文件，无重复代码或结构问题
