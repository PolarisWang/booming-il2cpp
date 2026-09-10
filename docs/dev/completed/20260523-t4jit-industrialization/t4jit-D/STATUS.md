---
task_id: t4jit-D
task_type: inline-code-change
lifecycle_status: completed
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
parent_task_id: 20260523-t4jit-industrialization
source_relation: roadmap-child
---

# t4jit-D — unwind_info 编译守卫

## 完成证据

- 改动: `src/native/codegen/unwind_info.cpp`
  - 添加 `#include <cstddef>`
  - 添加两个 `static_assert`: `sizeof(void*)==8` (x64 only), `offsetof(UnwindCode, code_offset)==0`
- 验证: `cmake --build build/testing --target test_codegen_native` 编译通过
- 测试: `ctest -C Debug -R test_codegen_native` — 67/68 通过（仅预存 OsrPromote 超时）
- exit_criteria: static_assert 就位; 编译通过 ✓
