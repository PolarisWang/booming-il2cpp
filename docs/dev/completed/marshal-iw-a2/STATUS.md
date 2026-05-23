---
task_id: a2-test-stubs
parent_task_id: marshal-industrialization
source_relation: roadmap-child
phase: A
lifecycle_status: completed
entry_skill: dev-executing-plans
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Phase A2 — BSTR/COM Mock 桩

## Scope

创建 BSTR mock 桩和测试辅助函数，使 marshal 测试能在非 Win32 平台编译链接。在 Win32 平台上，BSTR 函数通过 oleaut32.dll 运行时解析，无需 mock。

## 完成证据

- `testing/src/native/runtime-core/marshal_test_stubs.cpp` 已存在并通过 marshal_smoke 编译链接验证
- BSTR thunk 分析确认：
  - Win32: `ResolveOleAut32Proc` 在运行时解析 `oleaut32.dll`，无需桩
  - 非 Win32: thunk 在 `ResolveOleAut32Proc` 返回 nullptr 时返回 0/no-op
- 现有 stubs 提供：`chaos_il2cpp_aot_hotpatch_module`, `kChaosExternalRuntime*`, `ChaosJitRegisterAll`

## 关键决策

由于 BSTR 函数设计为防御性（返回 0/no-op on null），不需要添加额外的 mock 桩。现有 stubs 足够覆盖所有平台。
