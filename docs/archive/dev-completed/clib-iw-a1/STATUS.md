---
task_id: clib-iw-a1
lifecycle_status: completed
phase: completed
parent_task_id: commonlib-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: pass
auto_execution_decision: continue
entry_skill: dev-executing-plans
---

# clib-iw-a1: Core Type Tests — type_info + native_types

## 完成证据

- arch_review: ok（无重复代码、职责清晰；发现 native_types.h 宏缺少括号的生产 bug 并修复）
- authority_review: n/a
- verification:
  - canonicalCommand: `cmake --build . --target test_common_native_types --config Debug && cmake --build . --target test_common_type_info --config Debug && ./common/Debug/test_common_native_types.exe && ./common/Debug/test_common_type_info.exe`
  - formalObject: `test_common_native_types` (17 tests), `test_common_type_info` (25 tests)
  - summaryPath: n/a
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: n/a
  - verificationOutputRoot: n/a
  - latestResultPath: n/a
  - masterResultPath: n/a
  - reportSummaryPath: n/a
  - codegenStubPath: n/a
- test_result: passed (42/42 tests)
- wiki: n/a
- next: merge / follow-up

## 产出文件

- `testing/src/native/common/common_native_types_test.cpp` — 17 个测试
- `testing/src/native/common/common_type_info_test.cpp` — 25 个测试
- `testing/src/native/common/CMakeLists.txt` — 新增两个测试目标
- `src/native/common/chaos/native_types.h` — 修复 CHAOS_IL2CPP_RAW_POINTER_TAG 宏缺少括号的生产 bug
