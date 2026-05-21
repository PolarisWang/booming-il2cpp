# STATUS — A4-Dual+V2 ObjectHeader 优化实施

## 任务信息
- **task_id**: a4-dual-v2-objectheader
- **task_dir**: docs/dev/in-progress/a4-dual-v2-objectheader
- **lifecycle_status**: completed
- **phase**: 验证完成，准备归档
- **source_relation**: plan
- **clearance_source**: user-approved
- **question_clearance**: cleared
- **clearance_confirmed_by_user**: true

## 计划摘要

分 3 个 Phase 实施 A4-Dual+V2 ObjectHeader 优化架构：

### Phase 1：基础瘦身 ✅
1. **Phase 1.1** — ✅ field_storage[32] 删除，ObjectHeader 56B→24B，FieldGetValue/FieldSetValue 适配完成
2. **Phase 1.2** — ✅ TypeInfo 扩展（+flags +vtable_array +vtable_length），TypeInfoV2 定义完成

### Phase 2：Codegen 改造 ✅
3. **Phase 2.1** — ✅ generated_code_compat.h：3 header variants + chaos_object_get_type_info()
4. **Phase 2.2** — ✅ 决策引擎（HeaderKind + VTableVariant 选择）
5. **Phase 2.3** — ✅ ObjectModel 按 variant 发射，flags 写入 TypeInfo
6. **Phase 2.4** — ✅ EmitVirtualDispatchCall → vtable_array[slot]

### Phase 3：运行时适配 ✅
7. **Phase 3.1** — ✅ chaos_object_get_type_info()，ObjectNew 按 header kind 分配
8. **Phase 3.2** — ✅ GetSyncStatePtr 动态偏移，所有 sync 函数已适配
9. **Phase 3.3** — ✅ 反射/异常路径适配（sizeof(chaos_object_header) → sizeof(FatHeader)）

## 当前状态

| Phase | 任务 | 状态 | 备注 |
|-------|------|------|------|
| 1.1 | field_storage 删除 | ✅ 完成 | ObjectHeader 56B→24B |
| 1.2 | TypeInfo 扩展 | ✅ 完成 | +flags +vtable_array +vtable_length, TypeInfoV2 |
| 2.1 | Header 类型定义 | ✅ 完成 | Pure/ThinLockable/Fat + chaos_object_get_type_info() |
| 2.2 | Codegen 决策引擎 | ✅ 完成 | HeaderKind + VTableVariant |
| 2.3 | ObjectModel 发射 | ✅ 完成 | TypeInfo 按 variant 发射 |
| 2.4 | EmitVirtualDispatchCall | ✅ 完成 | vtable_array[slot] |
| 3.1 | 运行时分配函数 | ✅ 完成 | ObjectNew 按 header kind 分型分配 |
| 3.2 | ThinLockable 同步偷位 | ✅ 完成 | GetSyncStatePtr 动态偏移 |
| 3.3 | 反射/异常适配 | ✅ 完成 | exception_api/reflection_api 适配 |

## 完成证据

- arch_review: ok (重复代码无，抽象合理，codegen/Scriban 分层正确)
- authority_review: n/a
- verification:
  - canonicalCommand: dotnet build + dotnet test (snapshot) + cmake --build (native runtime-core)
  - formalObject: n/a (Phase 1-3 不涉及 formal verification gate)
  - summaryPath: docs/dev/in-progress/a4-dual-v2-objectheader/STATUS.md
- test_result: passed (58/58 snapshot, native build succeeded)
- wiki: pending
- next: wiki → branch completion → merge

## 结构告警

### 最终检查
- [x] 重复代码 — 无。HeaderSizeFromFlags + GetSyncStatePtr 封装了 header kind 差异
- [x] 抽象必要性 — 3 header variants 各自职责清晰，统一访问器 chaos_object_get_type_info() 是正确抽象
- [x] 文件职责拆分 — generated_code_compat.h 仅暴露给 codegen 的类型，runtime_core.cpp 的 ObjectHeaderFat/Thin 是内部实现
- [x] codegen/Scriban 分层 — 未改动 Scriban 模板逻辑，仅在模板中统一使用 chaos_object_get_type_info()
- [x] kInlineFieldStorageSize 已删除，sizeof(ObjectHeader) 不再使用
- [x] exception_api.cpp 的 chaos_object_header 引用已改为 FatHeader

### 已知风险
- PureType 对象不支持 sync，需后续确保托管代码不对 PureType 类型 lock()
- ThinLockableHeader 在 generated_code_compat.h 中定义含 sync_state，但生成的 struct 仍使用 FatHeader 布局（type_info + vtable），这是 codegen 统一输出设计的预期行为
