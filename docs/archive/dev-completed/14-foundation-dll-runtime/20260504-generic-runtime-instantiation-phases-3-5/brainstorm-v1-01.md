# Brainstorm: 泛型运行时实例化 Phase 3→5 方案设计

## 背景

基于 Gen-2 Handle 方案，当前 `RuntimeInstantiationBridgeV0` 的 `resolve_or_instantiate_type` 已实现快/慢双路径，但值类型布局是简化占位符、解释器 call 派发缺失、方法泛型 `resolve_or_instantiate_method` 未实现。

目标：在"50+ native DLL, 30+ hotupdate DLL, 频繁跨 DLL 泛型"的规模下，完成 Phase 3→5 的完整链路。

## 三优先级

1. P1：性能最优 — 生成的 C++ 代码运行性能
2. P2：方案完美性 — 架构完整性、正确性、可维护性
3. P3：HotUpdate 支持 — 热更新适应性

## 澄清确认

用户确认（2026-05-04）：
- **跨 DLL 泛型**：频繁跨 DLL 是常态，跨模块类型身份是核心需求
- **解释器性能**：解释器方法不常用，不追求解释器性能优化
- **布局策略**：值类型布局需要"正确性 + 性能"两者都要
- **Phase 5 策略**：接受 metadata-first + 增量 IL，但两者都要完成
- **Tombstone 策略**：同意 Hotunload 时标记 tombstone 而非删除条目

## 方案对比

### 方案 A：渐进式精确布局 + 缓存优化

在现有架构上增量修改。
- **Layout**：`ComputeValueTypeLayout` 从简化占位符改为真实字段类型解析，加缓存
- **Interpreter**：Call opcode 通过 RuntimeAbiV0.method_invoke 桥接
- **方法泛型**：克隆 open method 描述符，IL→IR Lowering，注册到 registry
- **优**：风险最低，增量可验证，工期 3-4 周
- **劣**：Layout 和 interpreter 的边界不独立，维护时职责耦合

### 方案 B：Codegen 辅助布局 + 原生调用桥

利用 codegen 发射静态布局提示，加速 layout 查询。
- **Layout**：codegen 发射 per-type 布局 hint table，运行时查表
- **Interpreter**：原生函数 thunk 缓存
- **方法泛型**：同 A
- **优**：布局查询最快（查表）
- **劣**：两套路径（hint + fallback）同步维护成本高，跨 DLL 时 hint 生成复杂，工期 5-7 周

### 方案 C（选定）：独立布局引擎 + Minimal Interpreter

提取 LayoutEngine 为独立子系统，解释器不做 call dispatch。
- **Layout**：`LayoutEngine` 类，单一职责，可独立测试，per-module 缓存
- **Interpreter**：Minimal——Call opcode 返回 NEEDS_EXTERNAL_DISPATCH，由外层桥接
- **方法泛型**：Phase 5a metadata-first + Phase 5b 增量 IL
- **优**：架构最清晰，LayoutEngine 独立可测试，per-module 缓存原生支持 HotUpdate
- **劣**：多一个子系统，工期 4-6 周

### 评分

| 维度 | A | B | C |
|------|---|---|---|
| P1 性能 | 8 | 9 | 8 |
| P2 架构 | 7 | 5 | 9 |
| P3 热更 | 7 | 7 | 9 |
| 实现风险 | 8 | 5 | 7 |
| 维护性 | 7 | 5 | 9 |
| **总分** | 37 | 31 | **42** |

## 与 Unity IL2CPP 对比

| 维度 | Unity IL2CPP | Chaos IL2CPP 方案 C | 差异 |
|------|-------------|-------------------|------|
| 泛型共享 | 引用类型共享代码 | 不共享，每实例独立 | Unity 省代码量；Chaos 性能优 |
| 值类型布局 | il2cpp::vm::Class 巨类 | LayoutEngine 单一职责 | 算法等价，架构清晰度不同 |
| 类型身份 | Il2CppGenericInst* 指针 | (module_id\|token) uint64_t | **Chaos 优势**：不悬空 |
| MakeGenericType | vm::Class::FromIl2CppType | Registry + LayoutEngine | 功能等价 |
| 运行时方法泛型 | 不支持(AOT冻结) | Phase 5a metadata + 5b IL | **Chaos 独有** |
| HotUpdate | 不原生支持 | by_module 索引 + Tombstone | **Chaos 核心优势** |
| 模块隔离 | 单镜像全局表 | 256 模块槽 | **Chaos 优势** |

## 风险优化

| 风险 | 优化方案 |
|------|---------|
| R1 递归值类型 | visiting set + assert，不设硬性深度限制 |
| R2 IL→IR Lowering | Phase 5a metadata-only 先交付 + 5b TDD 增量扩展 |
| R3 Hotunload handle 悬空 | Tombstone 标记，保留 module 条目 |
| R4 布局偏差 | Codegen 发射 LayoutExpectation + bootstrap assert |
| R7 解释器 struct 栈性能 | Inline buffer (≤64B) + arena 分配器 |

## 风险评估摘要

见 STATUS.md。

## 架构影响

### 新增文件
- `src/native/runtime-core/layout_engine.h` — LayoutEngine 类声明
- `src/native/runtime-core/layout_engine.cpp` — LayoutEngine 实现
- `src/native/runtime-core/generic_method_instantiation.h` — 方法泛型实例化
- `src/native/runtime-core/generic_method_instantiation.cpp` — 方法泛型实例化实现

### 修改文件
- `runtime_instantiation.cpp` — 接入 LayoutEngine
- `module_registry.h/cpp` — kMaxModules 64→256，Tombstone，FreeModuleId
- `interpreter_vm.h/cpp` — ValueTag::Struct，NEEDS_EXTERNAL_DISPATCH
- `generic_context.h/cpp` — 方法泛型注册/查询
- `CMakeLists.txt` — 新源文件
- `NativeAotLoweringPlanner.cs` — LayoutExpectation 发射

## 分流

判定为 `roadmap`：4 个相对独立的子阶段 + 跨会话推进。
