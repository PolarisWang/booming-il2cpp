---
parent_task_id: com-interop-industrialization
source_task_id: p3-platform-layer
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
lifecycle_status: completed
phase: completed
---

# p3-platform-layer: COM 平台抽象层

## Scope

创建 `com_platform.h/.cpp`，将现有 `#if defined(_WIN32)` 中的真实 COM API 调用（IUnknown QI/AddRef/Release、CoCreateInstance、BSTR、VARIANT）封装到平台抽象层。非 Win32 返回 E_NOTIMPL 或 stub 值。

## Inputs

- com_rcw.cpp — IUnknown::QueryInterface/AddRef/Release 调用（Win32 block）
- com_ccw.cpp — aggregation outer QI/AddRef/Release 调用
- com_connection_point.cpp — sink AddRef/Release 调用
- marshal_api.cpp — CoCreateInstance、SysAllocString、SysFreeString、VariantInit、VariantClear

## Expected Outputs

- com_platform.h（新建）— 平台抽象函数声明
- com_platform.cpp（新建）— Win32/非 Win32 实现
- com_rcw.cpp — 迁移到平台层
- com_ccw.cpp — 迁移到平台层
- com_connection_point.cpp — 迁移到平台层
- CMakeLists.txt — 新文件注册
- build 通过，测试通过

## Exit Criteria

1. 所有 `#if defined(_WIN32)` 中的 COM API 调用迁移到平台层
2. cmake --build build/vs2022 --target chaos_runtime_core 通过
3. 全部 runtime-core COM 测试通过（49 tests）
4. 父 STATUS.md 更新
