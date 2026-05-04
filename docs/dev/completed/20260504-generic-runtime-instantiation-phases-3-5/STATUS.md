---
task_id: 20260504-generic-runtime-instantiation-phases-3-5
title: 泛型运行时实例化 Phase 3→5 — LayoutEngine + MinimalInterpreter + MethodInstantiation
task_type: roadmap
lifecycle_status: roadmap
phase: roadmap
created_at: 2026-05-04 20:00:00 +08:00
updated_at: 2026-05-04 20:00:00 +08:00

child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: brainstorming-approval

roadmap_or_plan: roadmap-v1-01.md

key_documents:
  - brainstorm-v1-01.md (方案对比与权衡)
  - roadmap-v1-01.md (阶段拆分与子任务映射)
---

## 设计摘要

基于 `dev-brainstorm` 的三方案对比，选定**方案 C：独立布局引擎 + Minimal Interpreter**。

### 边界拍板

1. **范围**：Phase 3（值类型布局计算） + Phase 4（解释器扩展） + Phase 5（方法泛型实例化），覆盖 `RuntimeInstantiationBridgeV0` 完整 API
2. **非目标**：不做泛型共享（每个封闭实例化独立），不做 JIT 优化，不做 IL→IR 的完整 opcode 覆盖（增量 TDD）
3. **规模上限**：kMaxModules = 256，覆盖 50+ native DLL + 30+ hotupdate DLL 场景
4. **跨 DLL 泛型**：频繁跨 DLL 是常态，TypeInfoHandle 的 `(module_id, token)` 编码负责跨模块身份

### Authority 决策

| 决策项 | 决策 | 依据 |
|--------|------|------|
| Phase 5 策略 | metadata-first + 增量 IL | R2 风险控制的架构级让步；先交付 MethodInfoHandle 元数据能力，再逐步扩展可执行方法体 |
| Hotunload handle 安全 | Tombstone 标记 (不删除 module 条目) | 消除 R3 悬空 handle 风险，保留条目保证 LookupModule 始终返回合法指针 |
| 布局偏差检测 | Codegen 发射 LayoutExpectation + bootstrap assert | 消除 R4 布局偏差风险，每个模块加载时自动验证 |
| 递归检测 | visiting set 循环检测 + assert | 消除 R1 递归风险，无需硬性深度限制 |

### 当前结论

选定方案 C，进入 roadmap 分流。

### 风险评估摘要

| 风险 | 原方案残差 | 优化方案残差 | 优化手段 |
|------|:---------:|:-----------:|---------|
| R1 递归值类型无限循环 | 🟡 深度32后保守值 | 🟢 **风险消除** | visiting set + .NET类型系统保证 + assert |
| R2 IL→IR Lowering不完备 | 🔴 概率高影响高 | 🟡 **大幅降低** | Phase 5a(metadata)先交付 + 5b TDD增量 |
| R3 Hotunload跨模块handle悬空 | 🔴 不可用 | 🟢 **风险消除** | Tombstone标记保留module条目 |
| R4 布局与.NET运行时不一致 | 🟡 实现质量风险 | 🟢 **风险消除** | LayoutExpectation + bootstrap assert |
| R5 模块容量耗尽 | 🟢 256覆盖 | 🟢 不变 | — |
| R6 LayoutEngine并发竞争 | 🟢 标准锁 | 🟢 不变 | — |
| R7 解释器struct栈性能 | 🟡 堆分配 | 🟢 风险可忽略 | 解释器不常用，可跳过 |
| R8 跨模块注册顺序 | 🟢 可管理 | 🟢 不变 | — |
| R9 运行时token冲突 | 🟢 已解决 | 🟢 不变 | — |
| R10 空值类型 | 🟢 一次性修复 | 🟢 不变 | — |

### 三优先级权衡结论

| 优先级 | 方案 C 评价 |
|--------|-----------|
| P1 性能最优 | 🟢 LayoutEngine 缓存后 O(1)；解释器不常用不影响性能 |
| P2 方案完美性 | 🟢 LayoutEngine 独立职责、可独立测试、与 Module Registry 松耦合 |
| P3 HotUpdate 支持 | 🟢 by_module 缓存索引 + Tombstone 策略 + module_id 回收 |

### 关键文档

- `brainstorm-v1-01.md`：方案对比、权衡分析、与 Unity IL2CPP 对比
- `roadmap-v1-01.md`：阶段拆分、子任务映射、依赖关系

## 子任务映射

| task_id | phase | status | purpose | depends_on | est_effort |
|---------|-------|--------|---------|------------|------------|
| layout-engine | A | **completed** | LayoutEngine 子系统 | 无 | 中 |
| module-registry-expansion | A | **completed** | Module Registry 扩容 + Tombstone | 无 | 小 |
| minimal-interpreter | B | **completed** | MinimalInterpreter 扩展 | layout-engine | 中 |
| method-instantiation-metadata | B | **completed** | 方法泛型 Metadata | module-registry-expansion | 中 |
| method-instantiation-il-execution | B | **completed** | 方法泛型 IL 执行 | minimal-interpreter, method-instantiation-metadata | 中 |

## 下一步

**Roadmap 已完成**。所有 5 个子任务已全部完成并归档。
无待推进的子任务。进入新的独立工作项阶段。

## 最近摘要

2026-05-04：Roadmap 创建。Phase A (基础架构) 启动，从 LayoutEngine 子系统开始。
Phase A 完成后进入 Phase B (执行引擎)。

2026-05-04：**Task A1 (LayoutEngine) 完成**。41/41 测试通过，结构告警与架构审视通过。
- 新文件：`layout_engine.h` / `layout_engine.cpp`
- 修改文件：`runtime_instantiation.cpp/.h`、`CMakeLists.txt`
- Step 5 (Codegen LayoutExpectation) 按计划延迟
- 子任务目录已移入 `docs/dev/completed/layout-engine/`

2026-05-04：**Task A2 (Module Registry 扩容 + Tombstone) 完成**。19/19 测试通过。
- `kMaxModules`: 64 → 256
- 新增空闲列表分配器 + tombstone 标记 + MarkModuleTombstone API
- `UnregisterModuleGenerics` 集成 tombstone 标记
- 子任务目录已移入 `docs/dev/completed/module-registry-expansion/`

2026-05-04：**Task B1 (MinimalInterpreter 扩展) 完成**。22/22 测试通过，结构告警与架构审视通过。
- 新增 ValueTag::Struct、InterpreterValue 深拷贝三法则、ExecutionFrame 析构释放 struct 堆内存
- Call/CallVirt/CallBridge 改为外部派发模式（needs_external_dispatch=true）
- 子任务目录已移入 `docs/dev/completed/minimal-interpreter/`

2026-05-04：**Task B2 (方法泛型实例化 Metadata) 完成**。38/38 测试通过，结构告警与架构审视通过。
- 新建 `generic_method_instantiation.h/.cpp` — RuntimeInstantiatedMethod + CreateClosedMethodDescriptor
- 扩展 `generic_context.h/.cpp` — RegisterGenericMethodInstantiation + TryResolveClosedMethod
- `resolve_or_instantiate_method` 从 nullptr stub 改为完整实现（fast-path registry + slow-path build）
- 子任务目录已移入 `docs/dev/completed/method-instantiation-metadata/`

2026-05-04：**Task B3 (方法泛型实例化 IL 执行) 完成**。46/46 测试通过。
- 新文件：`il_to_ir_lowerer.h` / `il_to_ir_lowerer.cpp`
- 修改文件：`generic_method_instantiation.h/.cpp` (LowerMethodBody 延迟降低/缓存)、`CMakeLists.txt` (添加文件 + interpreter include 路径)
- 两遍 IL→IR 降低算法：第一遍解码为 RawInsn（保留 IL offset），第二遍解析分支目标 + token resolver 回调
- 支持 ~30 个核心 IL opcode（ldarg, ldloc, stloc, ldc.i4, call, ret, br, newobj, box 等）
- 子任务目录已移入 `docs/dev/completed/method-instantiation-il-execution/`

**Roadmap 全部完成。所有 5 个子任务均已进入终态。**

## 调度状态

dispatch_model: sequential
active_children: []
completed_children: [layout-engine, module-registry-expansion, minimal-interpreter, method-instantiation-metadata, method-instantiation-il-execution]
pending_children: []

## latest_stop_point

**Roadmap 已完成** — 所有 5 个子任务均已进入终态。

| task_id | phase | status |
|---------|-------|--------|
| layout-engine | A | completed |
| module-registry-expansion | A | completed |
| minimal-interpreter | B | completed |
| method-instantiation-metadata | B | completed |
| method-instantiation-il-execution | B | completed |

Phase A (基础架构) + Phase B (执行引擎) 全部完成。
`RuntimeInstantiationBridgeV0` 完整 API 链路已就绪。

下一阶段入口：解释器完整 opcode 覆盖、异常处理 (SEH)、泛型虚方法派发（独立工作项）。
