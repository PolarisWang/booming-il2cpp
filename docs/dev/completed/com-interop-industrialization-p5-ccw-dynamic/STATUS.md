---
parent_task_id: com-interop-industrialization
source_task_id: p5-ccw-dynamic
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
lifecycle_status: completed
phase: completed
---

# p5-ccw-dynamic: CCW SmallVector 动态接口表

## Scope

ComCcw 接口表改为 SmallVector 模式（内嵌 4 槽 inline_interfaces + 超限动态 DomainAllocate 扩容 2 倍）。修改 ResolveCcw 去除范围检查（依赖 back-pointer），更新 RegisterCcwInterface/CreateCcw/CcwRelease/DestroyCcw 处理动态存储生命周期。

## Expected Outputs

- com_ccw.h — ComCcw 结构体修改（interfaces* + inline_interfaces[4] + interface_capacity）
- com_ccw.cpp — RegisterCcwInterface 动态扩容、CcwRelease/DestroyCcw 释放动态存储、ResolveCcw 简化
- build 通过，全部 COM 测试通过

## Exit Criteria

1. cmake --build build/vs2022 --target chaos_runtime_core 通过
2. 全部 runtime-core COM 测试通过（49 tests）
3. 父 STATUS.md 更新
