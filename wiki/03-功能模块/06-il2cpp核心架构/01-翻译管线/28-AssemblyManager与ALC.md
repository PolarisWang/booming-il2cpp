# 28 — AssemblyManager 与 AssemblyLoadContext (ALC)

> 运行时程序集生命周期管理器：加载、卸载、静态字段隔离、多线程安全。

## 概述

`AssemblyManager` 是管理热更新程序集（`.patchdata`）生命周期的单例。每个加载的程序集对应一个 `AssemblyLoadContext` (ALC)，提供：
- 静态字段存储隔离（per-assembly domain heap）
- 线程安全的加载/卸载（`shared_mutex` 读写锁）
- ALC-level 类型身份（通过 `module_id` 和 `alc_id`）

### 核心文件

| 文件 | 职责 |
|------|------|
| `src/native/runtime-core/assembly_manager.h` | AssemblyLoadContext 结构体、AssemblyManager API |
| `src/native/runtime-core/assembly_manager.cpp` | 全部实现 |

### 关键类型

**AssemblyLoadContext** — 代表已加载程序集：
- `alc_id`: ALC 唯一 ID（0 = SharedContext/CoreLib，单调递增从 1 开始）
- `module_id`: module_registry 索引（AOT 侧）
- `static_field_ptr`: domain-allocated `InterpreterValue[]` 数组
- `domain_id`: per-assembly MemoryDomain ID
- `is_loaded` / `is_unloading`: 生命周期状态

**AssemblyManager** — 生命周期管理器：
- `assemblies_`: `vector<AssemblyLoadContext>`（初始 256 槽，自动倍增）
- `loaded_count_`: `atomic<uint32_t>`（无锁读取）
- `s_asm_mutex()`: `shared_mutex`（读写锁保护 `assemblies_`）

## 锁策略

```
AssemblyManager 使用 shared_mutex，模式：

  读取者 (shared_lock)          写入者 (unique_lock)
  ─────────────────────         ─────────────────────
  FindAssembly                  LoadAssembly
  FindByModuleId                UnloadAssembly
  GetStaticField (查找阶段)
```

锁设计要点：
- **`loaded_count_` 是 `std::atomic`** — `LoadedCount()` 完全无锁，适合监控/诊断
- **`s_asm_mutex()` 是函数局部静态** — 避免跨 TU 静态初始化顺序问题（与 module_registry 一致的模式）
- **UnloadAssembly** 在 `is_unloading` flag 设置后加锁 — 先发信号再等待锁，减少写者阻塞时间
- **GetStaticField** 查找阶段持有 shared_lock，找到 ALC 指针后释放锁 — 后续 grow/shrink 操作在 domain heap 上进行，不涉及 assemblies_ 表

## 加载流程 (LoadAssembly)

```
patch_data → ① CreateMemoryDomain
           → ② ApplyPatchFromMemory (PatchLoader)
           → ③ 验证 module_id (LookupModule != nullptr)
           → ④ AllocateStaticFieldStorage (64 个预分配槽)
           → ⑤ GcRegisterStaticRootRange (GC 静态根注册)
           → ⑥ unique_lock → 查找空闲槽/自动扩容
           → ⑦ 填充 descriptor → return &desc
```

关键决策：
- **module_id 验证** (Phase 3 加固)：验证 `ctx->methods[0].module_id` 对应的模块已在 module_registry 注册，防止加载针对不同 AOT 镜像构建的 patch
- **自动扩容** (Phase 2 加固)：当 `assemblies_` 256 槽满时，倍增容量（最少 +64），不再返回 nullptr

## 卸载流程 (UnloadAssembly)

```
① is_unloading.store(true)        — 提前发信号
② unique_lock                     — 保护 assemblies_ 表
③ MarkModuleTombstone(module_id)  — module_registry 墓碑化
④ ClearMarshalerCache()           — 清除 ICustomMarshaler 缓存
⑤ Unpatch(patch_context)          — 清除 dispatch 入口
⑥ UnregisterModuleGenerics        — 泛型实例清理
⑦ GcUnregisterDomainStaticRoots   — GC 静态根注销
⑧ static_var_store_unregister     — 字段级存储清理
⑨ UnloadDomain(domain_id)         — MemoryDomain 完全销毁
⑩ 清空 descriptor → --loaded_count_
```

## 静态字段管理

`GetStaticField(module_id, field_offset)` 返回 `InterpreterValue*` 指针：

- 初始分配: 64 个槽位 (`kDefaultStaticFieldCount`)
- 按需增长: 双倍扩容（最多 `kMaxStaticFieldCount = 1M`）
- 溢出保护: `field_offset` 超过 `UINT32_MAX - 1` 时拒绝（Phase 2 加固）
- 调用者约束: 返回的指针在 ALC 域销毁后失效；调用者必须保证 ALC 在访问期间存活

> **TOCTOU 注意**: GetStaticField 返回 ALC 指针后释放了 shared_lock。调用者访问 `static_field_ptr` 时可能已被并发 UnloadAssembly 销毁域。调用者必须通过外部机制（如 GC safepoint、引用计数）保证 ALC 存活。

## ALC 生命周期状态机

```
         [未分配]
            │
   LoadAssembly ▼
      ┌──────────────┐
      │  is_loaded    │
      │  = true       │
      └──────┬───────┘
             │
   UnloadAssembly ▼
      ┌──────────────┐
      │  is_unloading │
      │  = true       │
      └──────┬───────┘
             │
   descriptor 清空
      ┌──────────────┐
      │  is_loaded    │
      │  = false      │     ← 可被 LoadAssembly 重用
      └──────────────┘
```

## 与 ModuleRegistry 的集成

ALC 通过 `module_id` 绑定到 AOT 模块：
- `ctx->methods[0].module_id` 决定了 ALC 对应的 AOT module
- `MarkModuleTombstone(module_id)` 在卸载时将 module_registry 中对应槽标记为墓碑
- 墓碑化后，module_registry 空闲列表可回收该槽，分配给未来加载的模块
- ALC 的 `module_id` 复用逻辑在 module_registry 侧处理

## 与 MemoryDomain 的集成

每个 ALC 拥有独立的 MemoryDomain：
- 域 ID 存储在 `domain_id` 字段
- `static_field_ptr` 在域堆上分配
- `PatchContext` 元数据也在域堆上分配
- 域销毁时批量释放所有域内分配（无需逐元素释放）
- 域卸载前必须注销 GC 静态根，防止 GC 扫描到已释放内存

## 与 StaticVarStore 的集成

AOT codegen 生成的静态变量访问代码通过 `static_var_store_*` API：
- `LoadAssembly`: 调用 `static_var_store_register(hash, module_id, domain_id)`
- `UnloadAssembly`: 调用 `static_var_store_unregister_domain(domain_id)` + `static_var_store_unregister_assembly(hash)`
- 双路径清理确保 AOT 间接访问路径不会读到已卸载域的映射

## 与测试集成

存在两个测试文件（Loader 工业化 Phase 1）：
- `assembly_manager_stress_test.cpp` — 并发只读查询、NextAlcId 单调性、is_unloading 可见性、UnloadAssembly 空安全（5 测试）
- `assembly_manager_test.cpp` — 集成测试（编译 assembly_manager.cpp + domain_unloader.cpp 从源码）
