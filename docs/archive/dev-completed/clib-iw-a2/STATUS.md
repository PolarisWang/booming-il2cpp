---
task_id: clib-iw-a2
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

# clib-iw-a2: Core Type Tests — arithmetic + checked_conv

## 完成证据

- arch_review: ok（无重复代码、职责清晰；无生产代码改动）
- authority_review: n/a
- verification:
  - canonicalCommand: `cmake --build . --target test_common_arithmetic --config Debug && cmake --build . --target test_common_checked_conv_ext --config Debug && ./common/Debug/test_common_arithmetic.exe && ./common/Debug/test_common_checked_conv_ext.exe`
  - formalObject: `test_common_arithmetic` (20 tests), `test_common_checked_conv_ext` (12 tests)
  - summaryPath: n/a
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: n/a
  - verificationOutputRoot: n/a
- test_result: passed (32/32 tests)
- wiki: n/a
- next: merge / follow-up

## 产出文件

- `testing/src/native/common/common_arithmetic_test.cpp` — 20 个测试（wrap 运算、div/rem 边界、移位运算）
- `testing/src/native/common/common_checked_conv_test_ext.cpp` — 12 个测试（溢出死亡测试 + 边界值确认）
- `testing/src/native/common/CMakeLists.txt` — 新增两个测试目标
