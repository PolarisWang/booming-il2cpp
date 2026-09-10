---
parent_task_id: com-interop-industrialization
source_task_id: p1-domain-heap
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
lifecycle_status: completed
phase: completed
---

# p1-domain-heap: IDomainHeap 集成

## Scope

将 COM Interop 子系统（RCW、CCW、ConnectionPoint）中的 `std::malloc`/`std::free` 全部替换为 `DomainCurrentAllocateTagged`/`DomainFreeTagged`。COM 分配点使用 CurrentDomain 自动路由，无需为 COM 单独指定 domain。

## Inputs

- `src/native/runtime-core/memory_domain.h` — IDomainHeap, DomainCurrentAllocateTagged, DomainFreeTagged API
- `src/native/runtime-core/com_rcw.cpp` — FindOrCreateRcw (malloc), ReleaseRcw (free)
- `src/native/runtime-core/com_ccw.cpp` — CreateCcw (malloc), Release/Destroy (free)
- `src/native/runtime-core/com_connection_point.cpp` — ComAdvise (CHAOS_IL2CPP_MALLOC), ComUnadvise (CHAOS_IL2CPP_FREE)
- `src/native/runtime-core/core/marshal_api.cpp` — COM 相关分配点

## Expected Outputs

- com_rcw.cpp 修改（malloc→DomainCurrentAllocateTagged, free→DomainFreeTagged）
- com_ccw.cpp 修改（同上）
- com_connection_point.cpp 修改（CHAOS_IL2CPP_MALLOC/FREE → DomainCurrentAllocateTagged/DomainFreeTagged）
- marshal_api.cpp 修改（如有 COM 相关分配点）
- build 通过，现有测试全部通过

## Exit Criteria

1. 所有 COM 分配点替换完成
2. `cmake --build build/vs2022 --target chaos_runtime_core --config RelWithDebInfo` 通过
3. `cmake --build build/testing --config RelWithDebInfo` 通过
4. 现有 COM 测试全部通过：com_rcw_test, com_ccw_test, com_connection_point_test（runtime-core 和 hot-update 两套）
5. 父 roadmap STATUS.md 更新 latest_stop_point

## 设计说明

- `DomainCurrentAllocateTagged` 自动从 TLS domain stack 获取 CurrentDomain
- `DomainFreeTagged` 从分配 header 中提取 IDomainHeap* 路由到正确的 heap
- COM 对象不归属于特定 MemoryDomain，使用 CurrentDomain 作为默认域
- 分配错误（返回 nullptr）保持现有错误处理逻辑（CHAOS_IL2CPP_FAIL）
