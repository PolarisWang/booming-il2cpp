# Metadata System 工业化补齐 — 合并方案设计

> **基于**: typesystem-vtable-industrialization 综合评估报告
> **创建日期**: 2026-05-23

## 背景

Metadata System 综合评估报告识别出 14 项新增工业化缺口（P0×3, P1×5, P2×4, P3×2），加上现有 typesystem-vtable-industrialization roadmap 中已识别但未启动的 Phase 1（9 项子任务）和 Phase 3（3 项子任务），需要统一合并为完整的工业化路线图。

## 架构总览

```
Metadata System 完整视图：

Managed Pipeline (codegen 侧)
  LinkerStage → MetadataWriter → Generator → Scriban → PatchDataExtractor
                                                        ↓
Runtime (native 侧)
  PatchLoader/PatchMetadataCache  ←── .patchdata binary
       ↓
  ModuleRegistry (module_id → ModuleDescriptor[4096])
       ↓
  TypeRegistry (stable_id → MethodTable*, unordered_dense)
       ↓
  VTableRegistry (TCVC/IOC epoch-based cache)
       ↓
  MetadataInterface (ModuleAwareResolve* → 统一查询入口)
       ↓
  EEClass (cold_delta MethodTable → EEClass 反射数据)
       ↓
  ReflectionAPI (ChaosReflectionGet* → 外部反射入口)
```

## 阶段设计

### Phase 0 — 测试安全网（已完成 ✅ 5/5）

T0-1~T0-5：type_registry API 测试、vtable_registry 隔离测试、查找性能基准、并发/压力测试、TCVC epoch 修复。

### Phase 1 — A4-Dual+V2 ObjectHeader 优化（🔶 1/10 done, deferred）

维持现有状态不变。T1-1（ObjectHeader 瘦身）已完成。T1-2~T1-10（TypeInfo 扩展、三种 Header 定义、Planner 决策引擎、分型发射、虚调用分型适配、Scriban 模板适配、sync_state 偷位、反射/异常/GC 适配）等待排期。**本 roadmap 不重新规划 Phase 1**。

### Phase 2 — 冷路径数据补齐（新增 6 项子任务）

**目标**：补齐 MetadataRegistry 和 EEClass 的数据层，使 `GetMethodName`、`FindToken`、`Ensure*Filled` 从 stub→真实实现。

| 子任务 | 缺口编号 | 前置 | 工作量 | 交付物 |
|--------|---------|------|--------|--------|
| **T2-6** | P0 | 无 | 3d | `MetadataRegistry::GetMethodName` 实现。建立 `method_token → name` 哈希索引。利用 `ReflectionQueryMethodDescriptor.name_utf8` 扫描 + 缓存到 `unordered_dense_map`。冷路径但用哈希表保证 O(1) 查询 |
| **T2-7** | P0 | 无 | 3d | `MetadataRegistry::FindToken` AOT 索引。建立 `method_token → module_id` 倒排索引。扫描所有模块的 ImageDescriptor types→methods 数组，填充 `unordered_dense_map` |
| **T2-8a** | P2 | T2-6 | 1d | EEClass Ensure*Filled AOT 验证测试。确认 AOT 路径的 constexpr descriptor 引用已正常工作，加单元测试 |
| **T2-8b** | P2 | T2-6, 扩展 PatchMetadataCache | 5d | EEClass Ensure*Filled 动态类型填充。扩展 PatchMetadataCache 暴露 `GetFieldDef(uint32_t)` / `GetTypeDef(uint32_t)`；新增转换层 `EEClassFromPatchData()` 从 PatchData 分配 domain 内存 + 填充 LazyArray |
| **T2-9** | P2 | T2-6, T2-7 | 2d | MetadataRegistry unit tests。覆盖 ResolveType/Method/Field、GetMethodName（AOT+hotpatch）、FindToken、BuildCache、EnumerateMethods |
| **T2-10** | P1 | T2-6 | 2d | `ModuleAwareResolveMethod` hotpatch 路径。`(void)patch_cache` → 调用 `PatchMetadataCache::ResolveToken` + 返回 MethodInfoHandle |
| **T2-11** | P1 | T2-10 | 2d | `LookupCallTarget` hotpatch 路径。`(void)patch_cache` → 解析 subject_id 格式 → `HotpatchNameRegistry` 查询 |

**小计**: Phase 2 = 18 人天

### Phase 3 — Gen-2 Handle + 跨 DLL vtable（🔴 0/3, blocked, 不变）

T3-1~T3-3 维持阻塞状态，依赖 hotupdate C1 / C9 / interpreter Layer 3。

### Phase 4 — 反射补齐 + codegen 扩张（新增 5 项子任务）

**目标**：解决所有 Reflection remaining_stubs，补齐 codegen 侧 descriptor flags 字段，可观测性。

| 子任务 | 缺口编号 | 前置 | 工作量 | 交付物 |
|--------|---------|------|--------|--------|
| **T4-1a** | P1 | 无 | 3d | **Reflection remaining_stubs 功能区补齐**：`GetIsVirtual`（检查 MethodTable flags）、`FieldGetIsInitOnly`/`FieldGetIsLiteral`（从 FieldDescriptor flags 解析）、`PropertyGetCanRead/CanWrite`（从 PropertyDescriptor flags 解析）、`GetExportedTypes`（返回完整类型数组，非 count-only）、`AssemblyGetForwardedTypes/ManifestResourceNames/EntryPoint`（返回空列表而非 0） |
| **T4-1b** | P1 | T4-1a | 3d | **codegen managed 侧补齐**：在 `ReflectionQueryMethodDescriptor` 新增 `flags` 字段，`ReflectionQueryFieldDescriptor` 新增 `flags` 字段，`ReflectionQueryPropertyDescriptor` 新增 `flags` 字段。修改 Scriban 模板/NativeAotEmitter 发射新字段。Manager 侧（MetadataWriter/Generator）patch |
| **T4-2** | P1 | Phase 2 | 2d | **错误处理与可观测性**：MetadataRegistry 关键路径（GetMethodName/FindToken/ResolveType/Method/Field）的 nullptr 返回路径加 `LOG_WARN`/`LOG_ERROR`。`ModuleLifecycleManager::RegisterModule` 失败路径加 LOG_ERROR。定义 `MetadataError` 枚举，静默失败改为至少日志记录 |
| **T4-3** | P2 | Phase 2 | 1d | **日志埋点**：`GetHotpatchNameRegistry().RegisterModule` 失败、`ModuleAwareResolveMethod` 各路径命中/失败、`LookupCallTarget` 各阶段日志 |

**小计**: Phase 4 = 9 人天

### Phase 5 — 性能加固 + 硬化（新增 5 项子任务）

**目标**：性能优化（枚举器、vtable 注册）、安全硬化（ABI fail-close）、生命周期验证、可观测性基线。

| 子任务 | 缺口编号 | 前置 | 工作量 | 交付物 |
|--------|---------|------|--------|--------|
| **T5-1** | P2 | T2-7, T2-6 | 3d | **EnumerateMethods 真实枚举**：构建所有模块的 (method_token, module_id, method_name) 索引表。从 `ReflectionQueryImageDescriptor` 遍历 → `unordered_dense_map`。`EnumerateMethods()` 改为返回真实 token + name |
| **T5-2** | P2 | 无 | 2d | **ABI manifest fail-open → fail-close**：`ChaosAbiManifestCheckMethodSignature` 当前返回 `CHAOS_ABI_MANIFEST_OK` 即使校验失败。改为返回具体错误码。添加 `CHAOS_ABI_MANIFEST_FAIL_CLOSE` 编译开关 |
| **T5-3** | P3 | 无 | 2d | **vtable RegisterTypeVTableRuntimeInterface 优化**：批量预分配替代逐条 `malloc+memcpy+free`。在 `RegisterTypeVTable` 中计算所需总空间，一次 malloc + 分片填充 |
| **T5-4** | P3 | 无 | 1d | **动态类型生命周期文档 + 销毁路径验证**：`chaos_register_type` 注册的动态类型在当前无 GC 集成销毁路径。记录当前生命周期管理假设。添加 `chaos_unregister_type` 声明（实现可选）。验证 `TryGetTypeInfoForDynamicType` 返回指针的生命周期 |
| **T5-5** | P2 | T5-1 | 2d | **Metadata 查询性能基准**：EnumerateMethods/ResolveType/Method/Field 基线。`testing/src/native/runtime-core/metadata_benchmark_test.cpp` |

**小计**: Phase 5 = 10 人天

## 依赖关系

```
Phase 2 (18人天)
  T2-6  T2-7    (无前置, 可并行)
     \    /
    T2-8a T2-8b T2-9 T2-10 T2-11
      (T2-8b 需 PatchMetadataCache 扩展)
       \
Phase 4 (9人天)
  T4-1a → T4-1b   (串行: 先 runtime 后 codegen)
  T4-2   T4-3     (依赖 Phase 2)
  
Phase 5 (10人天)
  T5-1 (依赖 T2-6, T2-7)
  T5-2  T5-3  T5-4  (无前置, 可并行)
  T5-5 (依赖 T5-1)
```

### 跨子任务依赖

```
Phase 2 内部:
  T2-6, T2-7 可并行启动
  T2-8a 依赖 T2-6
  T2-8b 依赖 T2-6, PatchMetadataCache 扩展
  T2-9 依赖 T2-6, T2-7
  T2-10 依赖 T2-6
  T2-11 依赖 T2-10

Phase 4 依赖 Phase 2: T4-1a/b 独立, T4-2/3 依赖 Phase 2 数据层

Phase 5 依赖 Phase 2: T5-1 依赖 T2-6/7, T5-2/3/4 独立
```

## 已知缺口与约束

1. **`.patchdata` 格式不含 Property 定义**：`patch_data.h` 有 `PatchFieldDefEntry` 但无 `PatchPropertyDefEntry`。T2-8b 只能填充 methods 和 fields，properties 留空。除非：
   - 方案 A（推荐）：`patch_data.h` 新增 `PropertyDefEntry` + codegen `PatchDataExtractor.cs` 修改 → 本 roadmap 已包含 codegen 改动的授权
   - 方案 B（回退）：动态类型 properties 返回空列表，记录到已知限制

2. **Phase 1 (A4-Dual+V2) 不动**：维持 1/10 done 状态，等待独立排期

3. **Phase 3 (Gen-2 Handle) 不动**：维持 blocked，等待 hotupdate C1/C9

## 风险评估

| 风险 | 等级 | 触发条件 | 缓解 |
|------|------|---------|------|
| T4-1b codegen 修改影响面大 | 中 | Scriban 模板修改触发全量 re-gen | 预留回滚路径；先在单元测试验证新字段 |
| T2-8b PatchMetadataCache 扩展破坏向后兼容 | 中 | 新增 Accessor 改变现有 PatchMetadataCache ABI | 只加不减；新增 GetFieldDef/GetTypeDef 不改变现有方法签名 |
| T5-2 ABI fail-close 引发兼容性问题 | 低 | 现有 .patchdata 文件签名不匹配 | 默认 fail-open，新开关 opt-in |
| 总工作量超 40 人天，需要持续投入 | 中 | 单会话无法完成 | 按 Phase 切分，每个 Phase 可以作为独立子任务 |

## 三优先级权衡

| 优先级 | 本方案体现 |
|--------|-----------|
| P1 性能最优 | T2-6/7 用哈希表（unordered_dense）保证 O(1)，非 O(n) 扫描。T5-1/3 优化枚举器与注册器 |
| P2 方案完美 | 核心驱动力。从 75%→100% 工业化。全部 stub/placeholder 补齐 |
| P3 HotUpdate | T2-8b EEClass 动态填充、T2-10/11 hotpatch 路径直接服务热更新 |

**裁决**：性能基线通过哈希表保障，冷路径不加线性扫描。日志/观测通过 `CHAOS_IL2CPP_DEBUGGER` / `CHAOS_IL2CPP_LOG_LEVEL` 编译开关隔离。
