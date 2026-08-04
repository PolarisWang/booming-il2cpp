# Type System / VTable 完全工业化 — Roadmap v1

> **task_id**: 20260523-typesystem-vtable-industrialization
> **创建日期**: 2026-05-23
> **基于设计**: brainstorm-v1-01.md (clearance confirmed)

## 目标

将 Type System / VTable 子系统从"核心能力可用"提升到"工业化标准"：补齐全部 11 项差距（P0×2, P1×2, P2×4, P3×3）和 2 项风险，覆盖对象头优化、vtable 双路径统一、测试基建、冷路径填充、泛型实例化、跨 DLL 分派。

## 范围边界

- **覆盖**：A4-Dual+V2 ObjectHeader 优化、vtable_registry + vtable_array 双路径合并、type_registry/vtable_registry 测试安全网、EEClass/MetadataRegistry 冷路径补齐、动态类型注册表扩容、TCVC epoch 风险、Gen-2 Handle 运行时泛型、跨 DLL vtable 分派
- **不覆盖**：iOS 合规签名体系、Unity 编辑器集成、完整 AppDomain 级别代码隔离
- **不覆盖**：解释器 Layer 4 调试器（归 interpreter-industrialization）
- **不覆盖**：HotUpdate C4 工具链/C10 静态变量/C11 COM Interop（归 hotupdate-industrialization）

## 非目标

- 不引入新的 vtable 分派架构（复用现有 `vtable_array[slot]`）
- 不做新 VM 设计（寄存器 VM 归 hotupdate C1）
- 不做通用调试器（归 interpreter Layer 4）

## 阶段列表

### Phase 0：测试安全网 + Epoch 修复（预估 3 周）

**Goal:** 为后续所有重构阶段建立测试安全网。补齐核心类型注册表 API 和 vtable 注册表的测试覆盖，建立性能基线，修复已知 epoch 风险。

**Exit Criteria:**
- type_registry API（chaos_register_type, chaos_find_type_by_stable_id, ChaosTypeAddInterface）有完整单元测试
- vtable_registry 有独立隔离测试目标（非集成测试），覆盖 TCVC/IOC/继承链/幂等性/epoch
- VTable 查找性能基准（3 项）可重复执行，输出基线
- 并发/压力测试（3 项）在 CHECK 配置下全部通过
- TCVC epoch 风险已分析，需要时已修复
- 代码覆盖率达到：vtable_registry.cpp ≥ 80%, type_registry.cpp ≥ 85%

**Deliverables:**
- `testing/src/native/runtime-core/type_registry_test.cpp`
- `testing/src/native/runtime-core/vtable_registry_test.cpp`
- `testing/src/native/runtime-core/vtable_lookup_benchmark_test.cpp`
- `testing/src/native/runtime-core/vtable_concurrency_test.cpp`
- `CMakeLists.txt` 新增对应测试目标
- Epoch 修复补丁（`vtable_registry.cpp`，如需）

**Dependencies:** 无

**Resolved Decisions:**
- 隔离测试使用 mock TypeVTable 和 mock VTableSlot，不依赖完整运行时启动
- 基准测试使用 RDTSC 测量（复用 profile.h），输出 ns/op
- epoch 修复优先考虑"延迟失效"而非"分代 epoch"（改动最小）

**Watch Items:**
- A4-Dual+V2 会改变 TypeInfo 布局，测试中使用的 mock 可能需要同步更新
- 并发测试需要稳定的多线程环境（Windows x64 专注）

### Phase 1：A4-Dual+V2 ObjectHeader 优化（预估 4 周）

**Goal:** 将对象头从 56B 降至 ~9.6B 加权平均，实现 PureType(8B)/ThinLockable(8B)/Fat(24B) 三种分型布局。遵循 wiki 已批准的 `19-A4-Dual+V2-ObjectHeader优化决策.md`。

**Exit Criteria:**
- 运行时基础：field_storage[32] 删除，ObjectHeader 56B→24B，TypeInfo 扩展，TypeInfoV2 定义
- Codegen：决策引擎按虚方法数+同步需求选择 HeaderKind 和 VTableVariant，分型发射通过
- 运行时适配：chaos_object_get_type_info() 统一访问器验证通过，sync_state 偷位移植完成
- 全量重新生成后所有 Phase 0 测试通过
- 基准测试性能不下降
- 对象头内存节省验证：1M 对象节省 ~46MB

**Deliverables:**
- `type_info.h` — TypeInfo 扩展 + TypeInfoV2 定义
- `generated_code_compat.h` — 三种 Header + chaos_object_get_type_info() 访问器
- `runtime_core.cpp` — ObjectNewThin/ObjectNewPure，sync_state 偷位
- `gc_layout.cpp` — 适配新 header
- `reflection_api.cpp`, `exception_api.cpp` — 统一访问器适配
- `NativeAotLoweringPlanner.cs` — 决策引擎
- `ObjectModelEmission.cs` — 分型发射
- `MethodEmission.cs` — 分型虚调用发射
- Scriban 模板适配

**Dependencies:**
- Phase 0 测试安全网（为重构提供 regression 保护）

**Resolved Decisions:**
- 严格遵循 19-A4-Dual+V2 决策文档的 3 子阶段方案（1a→1b→1c）
- FatHeader 字段交换（type_info 在 [0]），V2 自指 vtable_array
- 2-bit Magic 运行时判别
- 按类分配策略

**Watch Items:**
- 全量 re-gen 的回滚路径（如有严重问题可回退到旧 ObjectHeader）
- Scriban 模板修改影响面大（所有 codegen 输出）

### Phase 2：VTable 统一 + 冷路径补齐（预估 3 周）

**Goal:** 消除 vtable_registry 双路径技术债，补齐 EEClass 动态类型填充和 MetadataRegistry 冷路径，升级动态类型注册表。

**Exit Criteria:**
- ResolveVirtualMethodPointer 通过 vtable_array[slot] 完成查找（无双路径残留）
- RegisterTypeVTable/UpdateVTableSlotByMethodToken 中有一致性断言
- 动态类型的 GetMethods/GetFields/GetProperties/GetEvents 返回正确数据
- MetadataRegistry::GetMethodName() 非 AOT 路径返回正确结果
- MetadataRegistry::FindToken() 有完整 method token 索引
- chaos_find_type_by_stable_id 支持超过 256 个动态类型
- 所有 Phase 0 + Phase 1 测试通过

**Deliverables:**
- `vtable_registry.h/.cpp` — 双路径合并
- `eeclass.h/.cpp` — 动态类型填充补齐
- `metadata_interface.h/.cpp` — 冷路径回退补齐
- `type_registry.h/.cpp` — 哈希表升级

**Dependencies:**
- Phase 1（A4-Dual+V2 完成后，TypeInfo 布局已定）

**Resolved Decisions:**
- 动态类型注册表复用 `CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY`（key 为 stable_id uint64_t）
- EEClass 动态填充通过 stable_id 反向查找 ReflectionQueryTypeDescriptor
- vtable 一致性断言只在 CHECK 构建中启用

**Watch Items:**
- 动态类型注册表从平数组到哈希表的迁移需要线程安全
- EEClass 填充涉及反射路径，改动面较大

### Phase 3：Gen-2 Handle + 跨 DLL vtable（预估 4 周）

**Goal:** 补齐运行时 MakeGenericType 完整实现；跨程序集虚拟调用纳入 vtable_array 体系。

**Exit Criteria:**
- MakeGenericType 对 AOT 注册未命中的泛型类型返回正确实例化结果
- 跨程序集虚拟调用走 vtable_array[slot]（非运行时桩）
- hotupdate 测试套件全部通过
- 基准测试无回归

**Deliverables:**
- Gen-2 Handle 运行时泛型注册路径
- 跨 DLL vtable 分派统一路径
- 集成回归测试

**Dependencies:**
- hotupdate C1（寄存器 VM 完成）
- interpreter R-7~R-9（Layer 3 完成）
- hotupdate C9（AutoBridge 签名扩展）
- Phase 1 + Phase 2

**Resolved Decisions:**
- Phase 3 依赖外部 roadmap，进度可能受外部影响
- 如果外部依赖延迟，可降级为"设计 + 部分实现"

**Watch Items:**
- Gen-2 Handle 依赖较多外部组件（register VM、interpreter Layer 3），综合风险最高
- 跨 DLL vtable 需要跨模块 slot 协调机制

## 子任务映射

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|------------|-----------------|
| T0-1 | Phase 0 | planned | type_registry API 单元测试 | 无 | 3d |
| T0-2 | Phase 0 | planned | vtable_registry 隔离测试 | 无 | 5d |
| T0-3 | Phase 0 | planned | VTable 查找性能基准 | 无 | 3d |
| T0-4 | Phase 0 | planned | 并发/压力测试 | 无 | 5d |
| T0-5 | Phase 0 | planned | TCVC epoch 风险分析+修复 | 无 | 2d |
| T1-1 | Phase 1 | planned | 删除 field_storage[32], ObjectHeader 瘦身 | T0-1~T0-5 | 2d |
| T1-2 | Phase 1 | planned | TypeInfo 扩展 + flags/vtable_array/vtable_length | T0-1~T0-5 | 1d |
| T1-3 | Phase 1 | planned | TypeInfoV2 + 三种 Header 定义 | T0-1~T0-5 | 2d |
| T1-4 | Phase 1 | planned | Planner 决策引擎（HeaderKind + VTableVariant） | T1-1, T1-2, T1-3 | 3d |
| T1-5 | Phase 1 | planned | ObjectModelEmission 分型发射 | T1-4 | 3d |
| T1-6 | Phase 1 | planned | MethodEmission 虚调用分型适配 | T1-4 | 2d |
| T1-7 | Phase 1 | planned | Scriban 模板全线适配 | T1-5, T1-6 | 2d |
| T1-8 | Phase 1 | planned | chaos_object_get_type_info() + ObjectNewThin/Pure | T1-1, T1-2, T1-3 | 2d |
| T1-9 | Phase 1 | planned | sync_state 偷位移植 | T1-8 | 2d |
| T1-10 | Phase 1 | planned | 反射/异常/GC 适配统一访问器 | T1-8 | 1d |
| T2-1 | Phase 2 | planned | vtable_registry 双路径合并 | Phase 1 | 5d |
| T2-2 | Phase 2 | planned | vtable 一致性验证断言 | T2-1 | 2d |
| T2-3 | Phase 2 | planned | EEClass 动态类型填充补齐 | Phase 1 | 4d |
| T2-4 | Phase 2 | planned | MetadataRegistry 冷路径回退补齐 | Phase 1 | 3d |
| T2-5 | Phase 2 | planned | 动态类型注册表 256→哈希表 | Phase 1 | 3d |
| T3-1 | Phase 3 | planned | Gen-2 Handle 运行时 MakeGenericType | Phase 2 + hotupdate C1 + interpreter L3 | 10d |
| T3-2 | Phase 3 | planned | 跨 DLL vtable_array 统一 | Phase 2 + hotupdate C9 | 10d |
| T3-3 | Phase 3 | planned | 集成回归验证 | T3-1, T3-2 | 2d |

## 依赖

```
Phase 0 (独立)         Phase 1                  Phase 2              Phase 3
┌────────┐            ┌────────────────┐       ┌────────┐          ┌────────────┐
│ T0-1/2 │            │ T1-1 T1-2 T1-3 │←T0    │ T2-1   │←P1      │ T3-1 (C1)  │
│ T0-3/4 │            │    (Sub-1a)    │       │ T2-2   │         │ T3-2 (C9)  │
│ T0-5   │            └──────┬─────────┘       │ T2-3/4/5 │       │ T3-3       │
└────────┘                   │                  └────────┘          └────────────┘
                    ┌────────┴─────────┐                     ←── hotupdate C1/L3
                    │ T1-4~T1-7        │                     ←── hotupdate C9
                    │    (Sub-1b)      │
                    └────────┬─────────┘
                             │
                    ┌────────┴─────────┐
                    │ T1-8~T1-10       │
                    │    (Sub-1c)      │
                    └──────────────────┘
```

## 风险

| 风险 | 等级 | 触发条件 | 缓解措施 |
|------|------|---------|---------|
| Phase 0 测试在 A4-Dual+V2 后需调整 | 中 | A4-Dual+V2 改变对象头布局 | 测试使用抽象访问器而非硬编码偏移 |
| A4-Dual+V2 全量 re-gen 回归 | 高 | codegen 输出与运行时不匹配 | Phase 0 测试安全网 + Sub-1a→1b→1c 分步验证 |
| T1-4/T1-5 决策引擎复杂度 | 中 | HeaderKind + VTableVariant 组合爆炸 | 严格 YAGNI，只覆盖 3 种 Header × 3 种 Variant |
| Gen-2 Handle 依赖外部 roadmap | 中 | C1 寄存器 VM 延期或 interpreter L3 延期 | Phase 3 可降级为设计+部分实现；不阻塞 Phase 0-2 |
| 跨 DLL vtable 需要跨模块协调 | 中 | AutoBridge 未就绪 | Phase 3 预留降级路径：不改 ExternalRuntime，只做设计 |

## 备选路径

- **Phase 0 若测试覆盖不足**：A4-Dual+V2 延期，等测试覆盖达标后再启动 Phase 1
- **Phase 1 若全量 re-gen 回归严重**：回退到旧 ObjectHeader（通过 git revert），排查原因后再试
- **Phase 2 vtable 统一若范围过大**：T2-3/T2-4/T2-5（冷路径）可提前到 Phase 0 并行执行（无依赖）
- **Phase 3 若外部依赖延迟**：降级为"设计文档 + 接口定义 + 部分实现"，不阻塞 roadmap 完成

## 当前建议推进顺序

```
串行推进，按 Phase 0 → Phase 1 → Phase 2 → Phase 3 顺序。
Phase 0 内部: T0-1/T0-2 可并行启动 → T0-3/T0-4 → T0-5
Phase 1 内部: Sub-1a(串行) → Sub-1b(串行) → Sub-1c(串行)
Phase 2 内部: T2-1 → T2-2 → T2-3/T2-4/T2-5 (并行)
Phase 3 内部: T3-1/T3-2 (并行) → T3-3
```

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `recommended_next_child`: T0-1
