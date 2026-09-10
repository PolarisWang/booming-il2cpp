# R1.3 — Event API 完整实现

> **task_id**: r1.3-event-api
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

实现完整的 Event API 支持：新增 `ReflectionQueryEventDescriptor` 类型、扩展 `ReflectionQueryTypeDescriptor` 增加 events/event_count 字段、更新 Scriban 模板生成 event descriptor 数组、更新 managed codegen 从 metadata 读取 event 数据、实现 C++ 侧 `ChaosTypeGetEvents` 和 `ChaosTypeGetEvent`。

## Inputs

- 文件：`reflection_query_model.h` — 需添加 EventDescriptor + TypeDescriptor events 字段
- 文件：`type_stubs.cpp` — 当前 GetEvents 返回空数组，GetEvent 返回 0
- Scriban 模板：`NativeAot.ReflectionQueryImage.cpp.scriban` — 需添加 event 数组生成
- managed codegen：`NativeAotLoweringPlanner.cs` EmitReflectionQueryImage 方法 — 需添加 event 数据收集
- managed 运行时注册：`RuntimeHelperShapeRegistry.cs` — GetEvent 当前为 GenericShapeDescriptor 返回 0

## Expected Outputs

- `reflection_query_model.h` — 新增 ReflectionQueryEventDescriptor + kEventFlagIsStatic + TypeDescriptor 扩展 events/event_count
- `type_stubs.cpp` — ChaosTypeGetEvents/ChaosTypeGetEvent 真实实现
- `NativeAot.ReflectionQueryImage.cpp.scriban` — event 数组生成
- `NativeAotLoweringPlanner.cs` — event 数据收集（从 PE metadata 读取）
- 构建验证通过

## Exit Criteria

- ReflectionQueryEventDescriptor 定义完成
- TypeDescriptor 包含 events/event_count
- ChaosTypeGetEvents 返回真实 event 数据
- ChaosTypeGetEvent(name) 按名查找返回正确 event handle
- chaos_runtime_core 构建通过

## Terminal Notes

**Executed**: 2026-05-23, completed successfully.

### Changes Made

1. **`reflection_query_model.h`** — Added `ReflectionQueryEventDescriptor` struct (subject_id, name, member_type, flags), `kEventFlagIsStatic` constant, extended `ReflectionQueryTypeDescriptor` with `events`/`event_count` fields (inserted between property_count and methods), added `EncodeReflectionQueryEventHandle` helper, added `FindReflectionQueryEvent` helper.

2. **`type_stubs.cpp`** — Replaced stub `ChaosTypeGetEvents` (returning empty `[0]`) with real implementation that reads from `desc->events` array and encodes handles; replaced stub `ChaosTypeGetEvent` (returning 0) with name-matched lookup using `FindReflectionQueryEvent`.

3. **`NativeAot.ReflectionQueryImage.cpp.scriban`** — Added conditional `ReflectionQueryEventDescriptor[]` array generation for types with events; updated both TypeDescriptor initializer branches to include `events_ptr, event_count` before `methods_ptr, method_count`.

4. **`NativeAotLoweringPlanner.cs`** — Added `typeEventMap` dictionary, event data collection in the PE metadata scanning loop (resolves event handler type via `TryResolveTypeIdentity` with "System.EventHandler" fallback), added `events`/`event_count` to the typeGroups anonymous object.

5. **`reflection_metadata_impl.h`** — Updated all 8 pre-computed `ReflectionQueryTypeDescriptor` initializer lists (kTypeObject, kTypeValueType, kTypeEnum, kTypeByte, kTypeString, kTypeType, kTypeException, kTypeDayOfWeek) to include `nullptr, 0u,` for the new `events`/`event_count` fields.

### Build Verification

chaos_runtime_core builds with RelWithDebInfo configuration — 0 errors, only pre-existing LNK4006 warnings unrelated to this change.

### Exit Criteria

- [x] ReflectionQueryEventDescriptor defined
- [x] TypeDescriptor includes events/event_count
- [x] ChaosTypeGetEvents returns real event data
- [x] ChaosTypeGetEvent(name) finds events by name
- [x] chaos_runtime_core builds successfully
