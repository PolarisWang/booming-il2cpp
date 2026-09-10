# Type System / VTable 完全工业化 — Roadmap v2（Metadata 合并版）

> **task_id**: 20260523-typesystem-vtable-industrialization
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **基于设计**: design-v1-01.md (clearance confirmed)

## 目标

将 Type System / VTable / Metadata 子系统从"核心能力可用"提升到"工业化标准"：补齐全部原始 11 项差距 + 评估报告识别的 14 项新增缺口，覆盖对象头优化、vtable 双路径统一、测试基建、冷路径填充、泛型实例化、跨 DLL 分派、反射补齐、可观测性、性能硬化。

## 范围边界

- **覆盖**：MetadataRegistry（GetMethodName/FindToken 数据层）、EEClass 动态类型填充、Reflection remaining_stubs 补齐、codegen descriptor flags 扩展、可观测性（日志/错误处理）、EnumerateMethods 真实枚举、ABI manifest fail-close、vtable 注册优化、动态类型生命周期
- **不覆盖**：A4-Dual+V2 ObjectHeader 完整优化（Phase 1 deferred）
- **不覆盖**：Gen-2 Handle 运行时泛型 / 跨 DLL vtable（Phase 3 blocked）
- **不覆盖**：iOS 合规签名体系、Unity 编辑器集成

## 非目标

- 不引入新的 vtable 分派架构（复用现有 `vtable_array[slot]`）
- 不做新 VM 设计（寄存器 VM 归 hotupdate C1）
- 不做通用调试器（归 interpreter Layer 4）

## 阶段列表

### Phase 0：测试安全网 ✅（已完成）

T0-1~T0-5 全部完成。无变动。

### Phase 1：A4-Dual+V2 ObjectHeader 优化 🔶（deferred）

T1-1 完成，T1-2~T1-10 等待排期。本 roadmap 不重新规划。

### Phase 2：冷路径数据补齐 🔶（新增）

**Goal:** 补齐 MetadataRegistry 和 EEClass 的数据层，使 GetMethodName、FindToken、Ensure\*Filled 从 stub 变为真实实现。全部使用 unordered_dense 哈希表保证 O(1) 查询。

**Exit Criteria:**
- MetadataRegistry::GetMethodName() 对 AOT 模块返回正确方法名（非 nullptr）
- MetadataRegistry::FindToken() 对 AOT 方法返回 (module_id<<32 | token) 复合键（非 0）
- EEClass AOT 路径 Ensure\*Filled 已验证
- EEClass 动态类型 methods/fields 从 PatchMetadataCache 填充（properties 已知限制）
- MetadataRegistry (ResolveType/Method/Field/GetMethodName/FindToken) 有单元测试覆盖
- ModuleAwareResolveMethod hotpatch 路径调用 PatchMetadataCache::ResolveToken
- LookupCallTarget hotpatch 路径通过 HotpatchNameRegistry 查询

**Deliverables:**
- `metadata_interface.cpp` — GetMethodName 索引、FindToken 索引
- `patch_loader.h/cpp` — PatchMetadataCache GetFieldDef/GetTypeDef accessor
- `eeclass.cpp` — Ensure\*Filled 数据填充（EEClassFromPatchData）
- `testing/src/native/runtime-core/metadata_registry_test.cpp`

**Dependencies:** 无外部依赖

**Resolved Decisions:**
- GetMethodName 索引：扫描 ImageDescriptor types→methods 数组构建 `unordered_dense_map(uint64_t token, const char* name)`
- FindToken 索引：同上源，构建 `unordered_dense_map(uint32_t token, uint64_t composite)` 
- EEClass 动态填充：扩展 PatchMetadataCache 暴露 FieldDef/TypeDef → domain-allocated LazyArray
- 动态类型 properties：patch_data.h 无 PropertyDef → 留空，记录已知限制

**Watch Items:**
- PatchMetadataCache extension 不能破坏现有 ABI

### Phase 3：Gen-2 Handle + 跨 DLL vtable 🔴（blocked，不变）

T3-1~T3-3 维持阻塞。依赖 hotupdate C1/C9、interpreter Layer 3。

### Phase 4：反射补齐 + Codegen 扩张 🔶（新增）

**Goal:** 解决所有 Reflection remaining_stubs，补齐 codegen 侧 descriptor flags 字段，建立可观测性。

**Exit Criteria:**
- 6 个 TODO 反射函数（GetIsVirtual、FieldGetIsInitOnly、FieldGetFieldHandle、FieldGetIsLiteral、PropertyGetCanRead、PropertyGetCanWrite）全部实现
- 6 个 Assembly stub（GetExportedTypes/ForwardedTypes/ManifestResourceNames/EntryPoint/OptionalCustomModifiers）返回正确空列表
- ReflectionQueryMethodDescriptor/FieldDescriptor/PropertyDescriptor 新增 flags 字段
- Codegen managed 侧发射新 flags 字段
- MetadataRegistry 关键路径有 LOG_ERROR 日志
- ModuleLifecycleManager 注册失败有 LOG_ERROR

**Deliverables:**
- `reflection/remaining_stubs.cpp` — TODO 函数实现
- `reflection_query_model.h` — 新增 flags 字段
- `NativeAotEmitter.cs` / Scriban 模板 — flags 发射
- `metadata_interface.cpp` — 日志埋点

**Dependencies:** Phase 2 数据层（T4-2/3）；T4-1a/b 独立

**Resolved Decisions:**
- flags 定义复用 ECMA 335 Flags 枚举的部分子集
- codegen 侧只在必须的 emit 路径加字段，不做全量 refactor

**Watch Items:**
- Codegen 修改影响面大，需要验证新字段不破坏旧 .patchdata 解析

### Phase 5：性能加固 + 硬化 🔶（新增）

**Goal:** 性能优化（枚举器、vtable 注册）、安全硬化（ABI fail-close）、生命周期验证、可观测性基线。

**Exit Criteria:**
- EnumerateMethods 返回真实方法 token+name（非占位符）
- ABI manifest 有 fail-close 编译开关，不匹配时返回明确错误码
- vtable RegisterTypeVTableRuntimeInterface 批量预分配（非逐条 malloc+memcpy+free）
- 动态类型生命周期文档化
- Metadata 查询有可重复性能基准（3 项）

**Deliverables:**
- `metadata_interface.cpp` — EnumerateMethods 真实索引
- `abi_manifest.cpp` — fail-close 开关
- `vtable_registry.cpp` — 批量预分配优化
- `testing/src/native/runtime-core/metadata_benchmark_test.cpp`

**Dependencies:** Phase 2（T5-1, T5-5）

**Resolved Decisions:**
- ABI fail-close 默认关闭，opt-in
- vtable 优化使用 `malloc` 一次分配 + placement new

**Watch Items:** 无

## 子任务映射

| task_id | phase | status | purpose | depends_on | requirements | deliverables | exit_criteria | estimated_effort |
|---------|-------|--------|---------|------------|-------------|-------------|---------------|-----------------|
| T0-1~T0-5 | 0 | completed | 测试安全网 | 无 | — | — | — | — |
| T1-1 | 1 | completed | ObjectHeader 瘦身 | 无 | — | — | — | — |
| T1-2~T1-10 | 1 | planned | A4-Dual+V2 | Phase 0 | deferred | — | — | 待排期 |
| **T2-6** | **2** | **completed** | GetMethodName 实现 | 无 | GetMethodName 返回正确方法名 | metadata_interface.cpp | metadata_registry_test 通过 | 3d |
| **T2-7** | **2** | **completed** | FindToken AOT 索引 | 无 | FindToken 返回正确复合键 | metadata_interface.cpp | metadata_registry_test 通过 | 3d |
| **T2-8a** | **2** | **completed** | EEClass AOT 验证测试 | T2-6 | Ensure*Filled AOT 路径正确 | eeclass_test.cpp | eeclass_test 通过 | 1d |
| **T2-8b** | **2** | **completed** | EEClass 动态填充 | T2-6 | PatchMetadataCache 扩展+转换层 | patch_loader.h/cpp + eeclass.cpp | 动态类型 method/field 填充 | 5d |
| **T2-9** | **2** | **completed** | MetadataRegistry 单元测试 | T2-6, T2-7 | 覆盖所有 Registry API | metadata_registry_test.cpp | 全部测试通过 | 2d |
| **T2-10** | **2** | **completed** | ModuleAwareResolveMethod hotpatch 路径 | T2-6 | (void)patch_cache → 真实调用 | metadata_interface.cpp | 集成测试 | 2d |
| **T2-11** | **2** | **completed** | LookupCallTarget hotpatch 路径 | T2-10 | 空路径 → HotpatchNameRegistry 查询 | metadata_interface.cpp | 集成测试 | 2d |
| T3-1~T3-3 | 3 | blocked | Gen-2 Handle + 跨 DLL | Phase 2 + hotupdate C1/C9 | 外部依赖 | — | — | — |
| **T4-1a** | **4** | **completed** | Reflection stubs runtime | 无 | TODO 函数+Assembly stub 实现 | remaining_stubs.cpp | 反射 API 全部可用 | 3d |
| **T4-1b** | **4** | **completed** | Reflection stubs codegen | T4-1a | Descriptor flags 字段+发射 | reflection_query_model.h + managed .cs | 编译通过 | 3d |
| **T4-2** | **4** | **completed** | 错误处理+可观测性 | Phase 2 | 关键路径 LOG_ERROR | metadata_interface.cpp | 日志确认 | 2d |
| **T4-3** | **4** | **completed** | 日志埋点 | Phase 2 | 全部 metadata 入口有日志 | metadata_interface.cpp | 日志确认 | 1d |
| **T5-1** | **5** | **completed** | EnumerateMethods 真实枚举 | T2-6, T2-7 | 真实 token+name | metadata_interface.cpp | debugger 枚举正确 | 3d |
| **T5-2** | **5** | **completed** | ABI fail-close | 无 | fail-close 编译开关 | abi_manifest.cpp | CI 通过 | 2d |
| **T5-3** | **5** | **completed** | vtable 注册优化 | 无 | 批量预分配 | vtable_registry.cpp | vtable_registry_test 通过 | 2d |
| **T5-4** | **5** | **completed** | 动态类型生命周期 | 无 | 文档+销毁验证 | 文档 + type_registry.h | 文档审查通过 | 1d |
| **T5-5** | **5** | **completed** | Metadata 性能基准 | T5-1 | 3 项性能基线 | metadata_benchmark_test.cpp | 基线可重复 | 2d |

## 依赖

```
Phase 2 内部:
  T2-6 ──→ T2-8a ──→
     │         T2-8b
     ├──→ T2-9
     └──→ T2-10 ──→ T2-11
  T2-7 ──→ T2-9

Phase 4:
  T4-1a ──→ T4-1b  (codegen)
  T4-2/3 ⟹⟹⟹ 依赖 Phase 2 完成

Phase 5:
  T5-1 ⟹⟹⟹ 依赖 T2-6, T2-7
  T5-5 ⟹⟹⟹ 依赖 T5-1
  T5-2/3/4 独立
```

由于串行执行，实际顺序按以下推进：
```
T2-6 → T2-7 → T2-8a → T2-8b → T2-9 → T2-10 → T2-11
  → T4-1a → T4-1b → T4-2 → T4-3
  → T5-2 → T5-3 → T5-4 → T5-1 → T5-5
```
（T5-2/3/4 与 Phase 2 无依赖但放 Phase 5 执行）

## 风险

| 风险 | 等级 | 触发条件 | 缓解措施 |
|------|------|---------|---------|
| T4-1b codegen 修改影响面大 | 中 | Scriban 模板修改触发全量 re-gen | 回滚路径；先在单元测试验证 |
| T2-8b PatchMetadataCache 扩展破坏向后兼容 | 中 | 新增 Accessor 改变现有 ABI | 只加不减，不改变现有方法签名 |
| T5-2 ABI fail-close 引发兼容性问题 | 低 | 现有 .patchdata 签名不匹配 | 默认 fail-open，新开关 opt-in |
| 总工作量 ~40 人天 | 中 | 单会话无法完成 | 按 Phase 切分，每个 Phase 可独立执行 |

## 备选路径

- **T2-8b 若 PatchMetadataCache 扩展范围过大**：降级为只填充 MethodDef（已有的），FieldDef 走 image descriptor fallback
- **T4-1b codegen 若回归严重**：只加 runtime 侧字段定义，codegen 发射推迟
- **T5-2 ABI fail-close 若破坏现有管线**：移除编译开关，只做文档记录

## 当前建议推进顺序

```
串行推进，推荐顺序：
T2-6 → T2-7 → T2-8a → T2-8b → T2-9 → T2-10 → T2-11
 → T4-1a → T4-1b → T4-2 → T4-3
 → T5-2 → T5-3 → T5-4 → T5-1 → T5-5
```

Phase 1/3 保持现有状态（deferred/blocked），不在本执行链中。

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `recommended_next_child`: T4-1a
