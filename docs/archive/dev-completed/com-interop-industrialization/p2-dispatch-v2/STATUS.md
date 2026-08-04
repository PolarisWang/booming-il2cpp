---
parent_task_id: com-interop-industrialization
source_task_id: p2-dispatch-v2
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
lifecycle_status: in-progress
phase: executing
---

# p2-dispatch-v2: IDispatch V2 + 完整 TypeLib

## Scope

完整 IDispatch::Invoke（含 DISPPARAMS 参数编组）+ ITypeLib/ITypeInfo 实现。
codegen 生成 ComTypeLibData/ComTypeInfoData 静态描述符，运行时 ComTypeInfoImpl 包装。

## Exit Criteria

1. com_abi.h 新增 IDispatchVtbl/DISPPARAMS/EXCEPINFO 定义
2. com_typelib.h/.cpp 创建 — ComTypeLibData/ComTypeInfoData 静态数据结构 + ComTypeInfoImpl 运行时包装
3. com_ccw.h — ComCcw 新增 typelib_data 指针
4. com_ccw.cpp — CcwGetTypeInfo/CcwGetTypeInfoCount 返回真实 ITypeInfo
5. Scriban 模板 — TypeLib 数据生成（方法名、DISPID、参数类型） + DISPPARAMS Invoke 参数编组
6. ModuleRegistration.cs — 传递参数信息到模板
7. CMakeLists.txt — 添加 com_typelib.cpp
