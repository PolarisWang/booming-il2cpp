# T1-1: ObjectHeader 瘦身（已完成） + Phase 1 剩余清理

> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_task_id**: t1-1-objectheader-slim
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **task_type**: plan
> **phase**: execution
> **lifecycle_status**: completed
> **child_execution_mode**: auto
> **entry_skill**: dev-executing-plans

## Scope

A4-Dual+V2 ObjectHeader 优化：删除 field_storage[32], ObjectHeader 56B→PureType 8B / ThinLockable 16B，适配 FieldGetValue/SetValue。

## 执行发现

代码审查确认 **Phase 1 运行时部分（Sub-1a + Sub-1c）已在先期完成**：

| Task | 内容 | 状态 |
|------|------|------|
| T1-1 | field_storage[32] 删除, ObjectHeader 瘦身 | ✅ 已完成 |
| T1-2 | TypeInfo 扩展 (+flags +vtable_array +vtable_length) | ✅ 已完成 |
| T1-3 | TypeInfoV2 / PureTypeHeader / ThinLockableHeader | ✅ 已完成 |
| T1-4 | Planner 决策引擎 (GetHeaderKind + GetVTableVariant) | ✅ 已完成 |
| T1-5 | ObjectModelEmission 分型发射 | ⚠️ V0/V1 完整, V2 留给 Phase 2 |
| T1-6 | MethodEmission 虚调用适配 | ✅ 已完成 |
| T1-7 | Scriban 模板 | ✅ 无需改动 |
| T1-8 | chaos_object_get_type_info() + ObjectNew | ✅ 已完成 |
| T1-9 | sync_state 偷位移植 | ✅ 已完成 |
| T1-10 | 反射/异常/GC 适配 | ✅ 已完成 |

### 已验证的完成项

- `PureTypeHeader(8B)` / `ThinLockableHeader(16B)` 在 `generated_code_compat.h` 定义
- `HeaderSizeFromFlags()` 动态计算 header size
- `FieldGetValue()/FieldSetValue()` 使用动态 header_size
- `TypeInfoV2` 在 `type_info.h` 定义（112B: hot 32 + warm 32 + inline_slots[6] 48）
- `chaos_object_get_type_info()` 统一访问器
- `ObjectNew`/`ObjectNewDirect` 使用 `HeaderSizeFromFlags()`
- `GetSyncStatePtr()` 处理 PureType (nullptr) / ThinLockable (&sync_state)
- Codegen 虚调用使用 `chaos_object_get_type_info() → vtable_array → chaos_vtable_resolve()`
- Codegen ObjectModelEmission 设置基于 `GetHeaderKind()` 的 flags

### 剩余清理项

1. TypeInfoV2 inline_slots 发射 — 留给 Phase 2 T2-1（运行时 BuildRuntimeVTable 优化）
2. 过时测试断言修复（field_storage / ObjectHeader / 56B）

## Exit Criteria

- [ ] 修复 stale test assertions
- [ ] 全部测试通过
- [ ] Phase 1 标记完成，进入 Phase 2
