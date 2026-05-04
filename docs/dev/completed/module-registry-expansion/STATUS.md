---
task_id: module-registry-expansion
title: Module Registry 扩容 + Tombstone — 支持 80+ 模块规模，Hotunload 安全降级
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-04 20:00:00 +08:00
updated_at: 2026-05-04 20:00:00 +08:00

parent_task_id: 20260504-generic-runtime-instantiation-phases-3-5
source_task_id: 20260504-generic-runtime-instantiation-phases-3-5
source_relation: roadmap-child
clearance_source: parent-roadmap

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

## Scope

对 `ModuleRegistry` 进行扩容和 hotunload 安全增强，使其支持 80+ 模块规模并提供 tombstone 降级能力。

**核心职责**：
- `kMaxModules`: 64 → 256
- `ModuleDescriptor` 增加 `bool tombstone` 字段
- 新增 `FreeModuleId()` + 空闲列表分配器
- 修改 `RegisterModule` 优先从空闲列表分配
- 修改 `LookupModule` 对 tombstone 模块仍返回合法指针（type_count = 0）
- 新增 `MarkModuleTombstone(id)` / `IsModuleTombstone(id)` API
- 集成到 `UnregisterModuleGenerics` 调用 tombstone 标记

## Inputs

- 父 roadmap 设计上下文：`roadmap-v1-01.md` Task A2
- 现有文件：
  - `src/native/runtime-core/module_registry.h` — 当前 kMaxModules=64, ModuleDescriptor, RegisterModule, LookupModule
  - `src/native/runtime-core/module_registry.cpp` — 当前线性数组分配

## Expected Outputs

1. 修改 `src/native/runtime-core/module_registry.h` — kMaxModules 256, tombstone 字段, 新函数声明
2. 修改 `src/native/runtime-core/module_registry.cpp` — 空闲列表, tombstone 逻辑
3. 可能需要修改 `src/native/runtime-core/runtime_instantiation.cpp` — UnregisterModuleGenerics 调用 MarkModuleTombstone

## Exit Criteria

- Build 0 error
- 注册 80 个模块不溢出
- Hotunload 后 `LookupModule(id)` 返回 tombstone 标记的合法指针（type_count=0）
- 释放的 module_id 可被新模块重新使用
- 单元测试覆盖基本注册/查找/回收/查询 tombstone 状态

## Terminal Notes

## 执行摘要

### 完成情况

| 需求 | 状态 | 说明 |
|------|------|------|
| kMaxModules: 64 → 256 | ✅ | `module_registry.h` 常量更新 |
| ModuleDescriptor 增加 bool tombstone | ✅ | 新增字段 + 默认 false |
| 新增 MarkModuleTombstone / IsModuleTombstone API | ✅ | 头文件声明 + .cpp 实现 |
| 空闲列表分配器 | ✅ | g_free_list + RegisterModule 优先从空闲列表分配 |
| LookupModule 对 tombstone 返回合法指针 | ✅ | 返回 ModuleDescriptor* 但 type_count=0, image/type_flags=nullptr |
| LookupModuleByName 跳过 tombstone 模块 | ✅ | continue 跳过已标记模块 |
| CoreLib (module_id=0) 保护 | ✅ | MarkModuleTombstone 拒绝标记 module 0 |
| 集成 UnregisterModuleGenerics | ✅ | 调用 runtime_core::MarkModuleTombstone(module_id) |
| 测试 | ✅ | 19/19 测试通过 |

### 结构告警与架构审视

- **兼容性** ✅ 空闲列表优先于线性分配，不破坏现有调用方
- **安全性** ✅ Tombstone 模块的 image/type_flags/type_parent_tokens 被置 null 防止 UAF
- **Handle 安全** ✅ 旧 TypeInfoHandle 的 LookupModule 始终返回合法指针（非 nullptr）

### 验证

```bash
python -m pytest tests/unit/compatibility/test_module_registry.py -v  # 19 passed
python -m pytest tests/unit/compatibility/test_layout_engine.py -v    # 41 passed (no regression)
```
