---
task_id: 20260523-loader-industrialization
task_name: Loader 子系统工业化
task_type: plan
phase: execution
lifecycle_status: in-progress
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: brainstorming-2026-05-23
blocking_questions: []
---

## 设计摘要

Loader 工业化评估已完成，7 个组件中仅 MemoryDomain ⭐⭐⭐⭐，其余 6 个均存在不同程度缺口。

### 清零结论

| # | 问题 | 决策 |
|---|------|------|
| Q1 | 阶段切分策略 | 按工作类型切：测试→修复→加固→文档 |
| Q2 | 测试策略 | 集成测试（真实依赖） |
| Q3 | PatchLoader 死代码 | 补完，不删除 |
| Q4 | StaticVarStore/NativeLibrary | 重开，必须有测试通过才算完成 |
| Q5 | 终端容量 | 1（串行推进） |
| Q6 | MemoryDomain | 不纳入本轮 |
| Q7 | ReapplyInlining 内存泄漏 | 修复，涉及 hotupdate 契约变更则改 |

### 范围

| 组件 | 等级 | 本轮工作 |
|------|------|---------|
| DomainUnloader | ⭐⭐⭐ | 补压力测试 |
| AssemblyManager | ⭐⭐ | 测试 + 锁粒度修复 + 边界加固 |
| ModuleRegistry | ⭐⭐ | 测试 + 文档化 |
| PatchLoader | ⭐⭐ | 死代码补完 + 测试 + 内存泄漏修复 |
| NativeLibrary | ⭐⭐ | 测试（重开） |
| StaticVarStore | ⭐⭐ | 测试（重开） |

### 推进顺序

串行四阶段：Phase 1 测试 → Phase 2 修复 → Phase 3 加固 → Phase 4 文档

Phase 1 测试阶段串行覆盖 6 个组件：
  DomainUnloader → AssemblyManager → ModuleRegistry → PatchLoader → NativeLibrary → StaticVarStore

## 评估报告参考

| 组件 | 等级 | 主要问题 |
|------|------|---------|
| MemoryDomain | ⭐⭐⭐⭐ | 已完成，无需本轮处理 |
| DomainUnloader | ⭐⭐⭐ | 缺压力测试 |
| AssemblyManager | ⭐⭐ | 零测试，锁粒度不一致，边界不严谨 |
| ModuleRegistry | ⭐⭐ | 零测试，函数局部静态未文档化 |
| PatchLoader | ⭐⭐ | 零测试，死代码，内存泄漏，888 行最大组件 |
| NativeLibrary | ⭐⭐ | 零测试 |
| StaticVarStore | ⭐⭐ | 零测试 |

## 当前状态

Phase 1 测试阶段 — **全部 6 个组件完成**。Phase 2 修复阶段 — **全部完成**。

### Phase 2 修复摘要

| # | 项目 | 组件 | 变更 | 状态 |
|---|------|------|------|------|
| F1 | 锁粒度修复 | AssemblyManager | 函数局部 `std::mutex` → 文件级 `std::shared_mutex`；读路径加 `shared_lock`(FindAssembly/FindByModuleId/GetStaticField)，写路径加 `unique_lock`(LoadAssembly/UnloadAssembly)；`loaded_count_` → `std::atomic<uint32_t>` | ✅ |
| F2 | 内存泄漏修复 | ReapplyInlining | `reapply_inlining.cpp`(L63-67)：替换 call_cache 前释放旧指针；`inlining.cpp`(L317-321)：InlineLeafCallees 替换 call_cache 前释放旧指针（注释已更新） | ✅ |
| F3 | 溢出保护 | PatchLoader | `CreatePatchContext` 增加 `sizeof(PatchMethod) * method_count` 乘法溢出检查和 `ctx_size + cache_size + methods_size` 加法溢出检查 | ✅ |
| F4 | 溢出保护 + 容量上限 | AssemblyManager | `GetStaticField` 增加 `UINT32_MAX-1` 回绕保护、`kMaxStaticFieldCount=1M` 容量上限；`assemblies_` 表满时自动倍增扩容 | ✅ |
| F5 | 注释更新 | PatchLoader | `DestroyPatchContext` 注释反映 ReapplyInlining 已修复泄漏的现状 | ✅ |

### Phase 2 修改文件

| 文件 | 变更 |
|------|------|
| `src/native/runtime-core/assembly_manager.h` | `uint32_t loaded_count_` → `std::atomic<uint32_t>` |
| `src/native/runtime-core/assembly_manager.cpp` | 新增 `#include <shared_mutex>`、`s_asm_mutex()`、shared_lock/unique_lock 保护所有路径；GetStaticField 回绕保护 + 容量上限 + assemblies_ 自动扩容 |
| `src/native/runtime-core/patch_loader.cpp` | CreatePatchContext 溢出保护 |

### 已完成的 Phase 1 组件

| 组件 | 文件 | 测试数 | 状态 |
|------|------|--------|------|
| DomainUnloader | `domain_unloader_stress_test.cpp` | 8 | ✅ 已完成 |
| AssemblyManager | `assembly_manager_stress_test.cpp` | 5 | ✅ 已完成 |
| ModuleRegistry | `module_registry_test.cpp` | 16 | ✅ 已完成 |
| PatchLoader | `patch_loader_test.cpp` (+ 死代码补完) | 11 | ✅ 已完成 |
| NativeLibrary | `native_library_test.cpp` | 15 | ✅ 已完成 |
| StaticVarStore | `static_var_store_stress_test.cpp` | 10 | ✅ 已完成 |

### Phase 1 测试覆盖摘要

| # | 组件 | 关键测试覆盖 |
|---|------|-------------|
| 1 | DomainUnloader | register+unload 循环100次、多region、50 domain 并发、多线程并发 unload |
| 2 | AssemblyManager | 并发只读查询、NextAlcId 单调性、is_unloading 可见性、UnloadAssembly 空安全 |
| 3 | ModuleRegistry | 注册/查找/墓碑/重用、TypeInfoHandle 编解码、并发 tombstone+register |
| 4 | PatchLoader | 验证守卫(6种)、PatchMetadataCache 边界(8种)、PatchMethod 结构、死代码补完确认 |
| 5 | NativeLibrary | Load/Free/GetProcAddress 守卫、refcounting、并发加载、PInvokeResolver 回调 |
| 6 | StaticVarStore | register/unregister_domain 生命周期、并发 field-level 读写、assembly 隔离 |

### Phase 1 死代码补完

| 文件 | 变更 |
|------|------|
| `src/native/runtime-core/patch_loader.cpp` | 启用 `#if 0` 中 GetFieldDef/GetTypeDefByIndex 实现；修正 FieldCount() 返回 header_->field_def_count |

## 下一步

Phase 2 修复阶段已完成。Phase 3 加固阶段已完成（module_id 验证）。Phase 4 文档阶段已完成。

### 已完成 Phase 3 加固

| 组件 | 事项 | 状态 |
|------|------|------|
| AssemblyManager | module_id 来源验证（`LookupModule(module_id) == nullptr` 时拒绝加载） | ✅ |
| AssemblyManager | TOCTOU 窗口文档化（调用者需确保 ALC 存活，见 wiki 28 号文档） | ✅ 文档化 |
| ModuleRegistry | LookupModuleByName 墓碑扫描效率 | ⏸️ low，已知限制 |

### 已完成 Phase 4 文档

| wiki 文档 | 说明 |
|-----------|------|
| [`28-AssemblyManager与ALC.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/28-AssemblyManager与ALC.md) | 锁策略、加载/卸载流程、静态字段管理、ALC 状态机、与各组件集成 |
| [`29-ModuleRegistry运行时参考.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/29-ModuleRegistry运行时参考.md) | API 参考、生命周期、两级架构、并发安全、测试覆盖 |
| [`30-PatchLoader运行时参考.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/30-PatchLoader运行时参考.md) | 类型说明、API 参考、内存泄漏防护、溢出保护、组件集成图 |

INDEX.md 已更新。

### Loader 工业化最终状态

| 阶段 | 状态 | 关键交付 |
|------|------|---------|
| Phase 1 测试 | ✅ | 65 测试覆盖 6 组件 |
| Phase 2 修复 | ✅ | 锁粒度、内存泄漏、溢出保护 |
| Phase 3 加固 | ✅ | module_id 验证 |
| Phase 4 文档 | ✅ | 3 wiki 文档 + INDEX 更新 |
