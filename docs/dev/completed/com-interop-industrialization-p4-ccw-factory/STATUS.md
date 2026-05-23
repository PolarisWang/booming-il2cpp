---
parent_task_id: com-interop-industrialization
source_task_id: p4-ccw-factory
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
lifecycle_status: completed
phase: completed
---

# p4-ccw-factory: CcwFactoryFn 运行时注册表

## Scope

创建 com_factory.h/.cpp，管理 CCW 工厂函数的注册/注销/查询。使用 CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY（stableId → CcwFactoryFn）。更新 codegen Scriban 模板在模块初始化中调用 RegisterFactory。

## Exit Criteria

1. com_factory.h/.cpp 创建完成，包含 RegisterCcwFactory/UnregisterCcwFactory/FindCcwFactory/CreateCcwForStableId
2. Scriban 模板添加静态 factory registration block
3. cmake --build build/vs2022 --target chaos_runtime_core 通过
4. test_com_rcw 通过
