---
task_id: t4jit-C
task_type: inline-code-change
lifecycle_status: completed
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
parent_task_id: 20260523-t4jit-industrialization
source_relation: roadmap-child
---

# t4jit-C — SEH Linux 加固

## 完成证据

- 改动: `src/native/codegen/t4_seh_handler.cpp`
  - Linux stub 区域: `__builtin_trap()` → `CHAOS_IL2CPP_FAIL("Linux SEH not implemented")`
  - 添加 `#include <chaos/native_types.h>` 使 CHAOS_IL2CPP_FAIL 可用
  - 添加两个 `static_assert`: `sizeof(uint32_t)==4`, `sizeof(int32_t)==4`
- 验证: `cmake --build build/testing --target test_codegen_native` 编译通过
- 测试: `ctest -C Debug -R test_codegen_native` — 67/68 通过（仅预存 OsrPromote 超时）
- exit_criteria: Linux stub 已替换; static_assert 就位 ✓
