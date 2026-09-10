# Reflection 工业化补齐 — Roadmap v1

> **面向执行 Agent:** 必须使用 dev-subagent-driven-development 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。所有子任务进入终态后，必须归档当前任务目录并同步父任务。

**目标：** 将 Reflection 子系统从 Phase 2 中期提升到完美级工业化标准：CoreCLR 反射能力 + 完整 proof 测试 + EventPipe 诊断 + 热更新兼容 + 性能基线。

**架构：** 基于现有 Module Registry + Two-Tier metadata 模式扩展，所有新增 API 遵循 descriptor-based const data 模式。核心改动在 codegen descriptor flags（新增字段）、runtime reflection API（消除 stub）、测试 proof 体系（新增套件）。

**技术栈：** native C++20 (`reflection_api.cpp` unity-build), managed C# (Generator Scriban templates), xUnit + CapabilityTest, GoogleTest (benchmark)

**架构审核模式：** critical（涉及 codegen descriptor + contract + test governance 主线）

**结构告警重点：** remaining_stubs.cpp 清除后文件职责；descriptor flags 向后兼容；Scriban 模板与 C++ 实现的职责分离

**权责图审核主题：** codegen 侧 NativeAotLoweringPlanner 与 Scriban 模板间的数据映射职责；Runtime 侧 ModuleRegistry 与 reflection_api 的职责边界

**AOT/IL2CPP/Test Governance Intake:**
- capabilityFamily: System.Private.CoreLib/reflection
- capabilityItem: bindingflags, events, customattributes, maketype, assemblymetadata, hotupdate-compat, edgecases
- ownerSubjectId: reflection
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Proof
- formalVerificationObjects: [ReflectionQueryTypeDescriptor, ReflectionQueryFieldDescriptor, ReflectionQueryMethodDescriptor, ReflectionQueryPropertyDescriptor, ReflectionQueryEventDescriptor, ModuleRegistry, remaining_stubs.cpp]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（设计摘要已写入 STATUS.md，无需独立设计文档）

**问题清零来源：** brainstorm-approved STATUS.md

**计划来源：** direct-brainstorm

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 1. 为什么需要 roadmap

Reflection 工业化涉及 17 个子任务，横跨 codegen（descriptor flags + Scriban 模板）、runtime-core（~15 个新 API 实现）、testing（7+ 新测试套件）、diagnostics（EventPipe 集成）、hot-update（ModuleRegistry 同步）五个子系统。子任务之间有强依赖链（Phase 1 descriptor 扩展 → Phase 2 API 实现 → Phase 3 热更新 → Phase 4 测试），必须跨多次会话持续推进。单一 plan 无法覆盖。

## 2. 范围边界

**覆盖：**
- BindingFlags 过滤：Native 侧实现（codegen descriptor flags + C++ filter）
- Member flags 补齐：IsInitOnly, IsLiteral, CanRead, CanWrite, IsVirtual
- Event API：新增 ReflectionQueryEventDescriptor, codegen + Scriban + C++ 实现
- GetElementType 完整实现
- CustomAttribute 多 scope 扩展：assembly/method/field/property/param 级别
- MakeArrayType/MakeByRefType/MakePointerType：走 instantiation_engine
- Assembly 元数据补齐：version, culture, forwarded types, manifest resources
- P2 edge cases 清零：GetBaseDefinition, GetCallingConvention, GetParamAttributes, PropertyGetIndexParameters, AssemblyGetEntryPoint
- 热更新反射兼容：ModuleRegistry 同步更新
- EventPipe 诊断事件
- 7+ 新测试 proof 套件 + 压力/soak + benchmark

**不覆盖：**
- 反射 emit（System.Reflection.Emit）— 与 AOT 架构冲突
- 完整 TypeBuilder/AssemblyBuilder — 需要 JIT
- iOS 合规签名 — 独立需求
- Mono.Cecil 集成 — 非本项目范围

## 3. 非目标

- 不追求 CoreCLR 的 JIT 级反射 emit 性能
- 不与 Unity IL2CPP 的 reflection 对标（不同元数据结构）
- 不做反射相关调试器扩展（属于 C2 调试器范畴）

## 4. 阶段列表

### Phase 1：API 核心矫正（1.5 周）

**Goal:** 消灭所有 P0 stub，补齐高频缺失 API，使 BindingFlags/Event/MemberFlags/GetElementType 正常工作。

**Exit Criteria:**
- BindingFlags 过滤在 Native 侧实现：`GetFieldsBindingFlags(name, flags)` 等 6 个函数不再忽略 flags 参数
- FieldDescriptor/MethodDescriptor/PropertyDescriptor 新增 flags 字段（codegen + Scriban），默认 0 向后兼容
- `ChaosReflectionFieldGetIsInitOnly`, `IsLiteral`, `GetIsVirtual`, `PropertyGetCanRead`, `PropertyGetCanWrite` 返回真实值
- `ReflectionQueryEventDescriptor` 定义完成，ImageDescriptor 扩展 events 数组
- `ChaosReflectionTypeGetEvents`, `ChaosReflectionTypeGetEvent` 返回正确数据
- `ChaosReflectionGetElementType` 支持 nested array 和 managed pointer
- 对应 proof 测试通过

**Deliverables:**
- `src/native/runtime-core/reflection_query_model.h` — 新增 flags, EventDescriptor
- `src/native/runtime-core/reflection/type_properties.cpp` — BindingFlags 实现
- `src/native/runtime-core/reflection/remaining_stubs.cpp` — flags 实现 (P0 部分)
- `src/native/runtime-core/reflection/type_stubs.cpp` — Event API
- `src/managed/Chaos.IL2CPP.Generator/` — codegen flags + EventDescriptor 输出
- Scriban 模板：ReflectionQueryImage 扩展 events
- `testing/foundation-dll/System.Private.CoreLib/reflection-binding/` + `reflection-event/`

**Dependencies:** 无（起始阶段）

**Resolved Decisions:**
- BindingFlags 方案 A：Native 侧过滤，codegen descriptor flags + C++ 实现
- Event API 方案 A：完整 ReflectionQueryEventDescriptor + ImageDescriptor 扩展
- 新 flags 字段默认 0，向后兼容

**Watch Items:**
- codegen descriptor flags 修改后需要 full rebuild，验证其他模块不退化

### Phase 2：API 表面全覆盖（1.5 周）

**Goal:** 补齐 P1/P2 stub，API surface 100%，remaining_stubs.cpp 清零。

**Exit Criteria:**
- CustomAttribute 支持 assembly/method/field/property/param 级别查询
- MakeArrayType/MakeByRefType/MakePointerType 通过 instantiation_engine 真实创建
- AssemblyGetVersion/AssemblyGetCultureInfo 返回真实数据
- AssemblyQualifiedName 包含正确的 version
- AssemblyGetForwardedTypes/AssemblyGetManifestResourceNames 不再返回 0
- GetBaseDefinition, GetCallingConvention, GetParamAttributes, PropertyGetIndexParameters, AssemblyGetEntryPoint 返回正确值
- remaining_stubs.cpp 中所有"返回 0"的 stub 被清除
- 对应 proof 测试通过

**Deliverables:**
- `src/native/runtime-core/reflection/attributes.cpp` — scope 扩展
- `src/native/runtime-core/reflection/remaining_stubs.cpp` — 全部清除
- `src/native/runtime-core/reflection/type_stubs.cpp` — MakeArrayType 等
- `src/native/runtime-core/reflection/misc.cpp` — Assembly metadata
- `src/native/runtime-core/reflection/invoke.cpp` — MakeGenericType 已有，扩展
- Scriban 模板调整
- `testing/foundation-dll/System.Private.CoreLib/reflection-customattr/` + `reflection-edge/`

**Dependencies:** Phase 1 完成（descriptor flags 和 model 扩展是前置）

**Resolved Decisions:**
- CustomAttribute 方案 A：blob scope 字段扩展
- MakeArrayType 方案 A：走 bridge->resolve_or_instantiate_type
- Assembly version 方案 A：ImageDescriptor 新增字段

**Watch Items:**
- CustomAttribute blob 格式扩展后需确认旧 blob 兼容性
- MakeArrayType 的性能（走 instantiation_engine 可能较慢）

### Phase 3：热更新兼容（1 周）

**Goal:** 热更新加载新类型后反射查询可见，GetTypeByName/GetTypeFromHandle 走热更新元数据路径。

**Exit Criteria:**
- 热更新模块注册后同步更新 ModuleRegistry type index
- GetTypeByName 能扫描到热更新模块
- GetTypeFromHandle 能解析热更新 token
- 热更新反射集成测试通过

**Deliverables:**
- `src/native/hot-update/hot_update.cpp` — ModuleRegistry 同步更新
- `src/native/runtime-core/reflection/type_resolve.cpp` — 热更新模块扫描
- `testing/foundation-dll/System.Private.CoreLib/reflection-hotupdate/`

**Dependencies:** Phase 1-2 完成（API surface 完整后才有意义）

**Resolved Decisions:**
- 方案 A：热更新注册时同步更新 ModuleRegistry

**Watch Items:**
- 热更新模块生命周期管理（卸载时清理）

### Phase 4：测试 + 工具链工业化（2 周）

**Goal:** 完整的 proof 体系 + EventPipe 诊断 + 性能基线。

**Exit Criteria:**
- 7 个新 proof 测试套件全部通过
- 反射压力/soak 测试 24h+ 通过
- 关键反射路径有 EventPipe 埋点
- Benchmark 基线建立，主要查询路径有数据

**Deliverables:**
- `testing/foundation-dll/System.Private.CoreLib/reflection-binding/` 等 7 套件
- `testing/src/native/runtime-core/reflection_benchmark.cpp`
- `src/native/diagnostics/eventpipe/ep_reflection_bridge.h/.cpp`
- `testing/stress/reflection_stress_test.cpp`

**Dependencies:** Phase 1-3 完成（所有 API 实现后方可测试）
Phase 4.7 (EventPipe) 可与 Phase 4.1-4.6 并行，但依赖 Phase 2 完成

**Resolved Decisions:**
- 每个子能力独立 proof 套件

**Watch Items:**
- Benchmark 基线需要确定可重复的环境

## 5. 子任务映射

| task_id | phase | status | purpose | depends_on | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|---------|------------|--------------|--------------|---------------|----------------|-----------------|
| R1.1 | Phase 1 | completed | BindingFlags 过滤 Native 实现 | — | codegen descriptor flags + C++ 6 个函数 | reflection_query_model.h, reflection_binding_flags.h, misc.cpp, type_properties.cpp, type_stubs.cpp | flags 不忽略，build 通过，review 通过 | src/native/runtime-core/reflection/ | 3 天 |
| R1.2 | Phase 1 | completed | Member flags 嵌入 codegen | R1.1 (共享 flags 字段) | IsInitOnly, IsLiteral, CanRead, CanWrite, IsVirtual 实现 | remaining_stubs.cpp | 5 flags 返回真实值 | src/native/runtime-core/reflection/ | 2 天 |
| R1.3 | Phase 1 | completed | Event API 完整实现 | — | ReflectionQueryEventDescriptor + codegen + C++ | reflection_query_model.h, type_stubs.cpp, Scriban, NativeAotLoweringPlanner.cs | GetEvents/GetEvent 正确，build 通过 | src/native/runtime-core/ + src/managed/ | 4 天 |
| R1.4 | Phase 1 | completed | GetElementType 完整 | — | nested array + managed pointer | type_stubs.cpp | 所有 array 类型可解析，build 通过 | src/native/runtime-core/reflection/ | 2 天 |
| R2.1 | Phase 2 | completed | CustomAttribute scope 扩展 | Phase 1 | blob scope 字段 + assembly/method/field/property/param | attributes.cpp, module_registry.h/.cpp, ModuleRegistration.cs | 5 个 scope 都可用，build 通过 | src/native/runtime-core/reflection/ | 5 天 |
| R2.2 | Phase 2 | completed | MakeArrayType 等 | — | instantiation_engine 路径 | type_stubs.cpp | 3 个 type 真实查找，build 通过 | src/native/runtime-core/reflection/ | 3 天 |
| R2.3 | Phase 2 | completed | Assembly metadata 补齐 | — | version/culture/forwarded/manifest | misc.cpp, remaining_stubs.cpp | 4 个 API 不返回 0，build 通过 | src/native/runtime-core/reflection/ | 3 天 |
| R2.4 | Phase 2 | completed | P2 edge cases | — | 5 个 stub 清除 | remaining_stubs.cpp | remaining_stubs.cpp 清零 | src/native/runtime-core/reflection/ | 2 天 |
| R3.1 | Phase 3 | completed | ModuleRegistry hot-update sync | Phase 1-2 | hot_update.cpp 注册后刷新 | hot_update.cpp, type_resolve.cpp | 热更新类型反射可见 | src/native/hot-update/ + src/native/runtime-core/ | 3 天 |
| R3.2 | Phase 3 | completed | 热更新反射集成测试 | R3.1 | 测试套件 | reflection-hotupdate/ | proof 创建完成 | testing/ | 2 天 |
| R4.1 | Phase 4 | completed | BindingFlags proof | R1.1 | 测试套件 | reflection-binding/ | proof 创建完成 | testing/ | 2 天 |
| R4.2 | Phase 4 | completed | Event proof | R1.3 | 测试套件 | reflection-event/ | proof 创建完成 | testing/ | 1 天 |
| R4.3 | Phase 4 | completed | CustomAttribute proof | R2.1 | 测试套件 | reflection-customattr/ | proof 创建完成 | testing/ | 1 天 |
| R4.4 | Phase 4 | completed | EdgeCase proof | R2.4 | 测试套件 | reflection-edge/ | proof 创建完成 | testing/ | 1 天 |
| R4.5 | Phase 4 | completed | 热更新反射 proof | R3.1-R3.2 | 测试套件 | reflection-hotupdate/ (同 R3.2) | proof 创建完成 | testing/ | 1 天 |
| R4.6 | Phase 4 | completed | 压力/soak 测试 | Phase 1-3 | 24h+ soak | reflection_stress_test.cpp | 文件创建 | testing/ | 2 天 |
| R4.7 | Phase 4 | completed | EventPipe 诊断埋点 | Phase 2 | 关键路径埋点 | ep_reflection_bridge.h/.cpp | 3 个事件可用 | src/native/diagnostics/ | 2 天 |
| R4.8 | Phase 4 | completed | Benchmark 基线 | Phase 1-3 | 主要查询路径数据 | reflection_benchmark.cpp | 基线可重复 | testing/ | 2 天 |

## 6. 依赖关系

```
Phase 1:
  R1.1 ──→ R1.2 (共享 flags 字段，建议串行)
  R1.3 (独立)
  R1.4 (独立)

Phase 2:
  全部依赖 Phase 1 完成（descriptor model 扩展是前置）
  R2.1 (独立)
  R2.2 (独立)
  R2.3 (独立)
  R2.4 (独立)

Phase 3:
  R3.1 依赖 Phase 1-2 完成
  R3.2 依赖 R3.1

Phase 4:
  R4.1 依赖 R1.1
  R4.2 依赖 R1.3
  R4.3 依赖 R2.1
  R4.4 依赖 R2.4
  R4.5 依赖 R3.1-R3.2
  R4.6 依赖 Phase 1-3
  R4.7 依赖 Phase 2
  R4.8 依赖 Phase 1-3
```

## 7. 风险

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| codegen descriptor flags 改动影响全量 project，build 时间 + 回归风险 | 中 | 新 flags = 0 向后兼容；先验证 single test 再全量 build |
| CustomAttribute blob scope 扩展可能与现有 blob 格式不兼容 | 中 | Phase 2 处理，Type 级已有；增加 scope_kind 参数而非改格式 |
| 热更新 + ModuleRegistry 同步可能引入并发问题 | 中 | Phase 3 单独隔离，有测试闸门 |
| 17 个子任务 × 3-4 天平均 = ~6 周，可能跨会话 | 低 | 每个 child 独立完成，STATUS.md 可恢复 |

## 8. 备选路径

- 如果时间不够：可先做 Phase 1 + Phase 2（API 核心矫正 + 表面全覆盖），Phase 3-4 放到下一轮
- 如果 BindingFlags 实现遇到 codegen 改造成本过高：回退到方案 B（Managed 侧过滤）作为临时方案
- 如果 Event API 需要大量 codegen 改动：可以分两步，先 native 侧返回空集合+实现 stub，codegen 扩展放到后一批

## 9. 当前建议推进顺序

严格按 Phase 1 → 2 → 3 → 4 顺序推进。Phase 1 内优先 R1.1+BindingFlags → R1.3+Event API（独立并行）；Phase 2 内各子任务可并行；Phase 4 的 4.1-4.6、4.7、4.8 可并行。

## 10. 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: R1.1
```

## 11. 阶段完成判定

每个阶段完成后必须验证：
1. 所有 deliverables 已产出
2. exit_criteria 全部满足（proof 通过、build 通过）
3. 无 blocker 残留
4. 子任务物理移入 `docs/dev/completed/reflection-industrialization/`

所有阶段完成后进入收尾闭环：结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交。
