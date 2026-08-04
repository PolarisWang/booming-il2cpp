---
task_id: clib-iw-f2
lifecycle_status: completed
phase: F
parent_task_id: commonlib-industrialization
source_relation: roadmap-child
---

# clib-iw-f2 — Profile Enabled-Mode Functional Tests

## 完成证据

- tests written: common_profile_enabled_test.cpp (5 tests)
- build: passed (uses CHAOS_IL2CPP_PROFILE_ENABLED=1 + profile_globals.cpp)
- test run: passed (5/5 — verified ProfileScope, Dump, Reset, multiple scopes)
- Profile dump output verifies RDTSC timing: "PROFILE|test_scope|avg=484|avg_ns=110"
- regression: all 33 common tests passed
