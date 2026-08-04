# Roadmap: 泛型运行时实例化 Phase 3→5

> 基于方案 C：独立布局引擎 + Minimal Interpreter + Method Instantiation
> 上游设计文档：`brainstorm-v1-01.md`，`STATUS.md`

## 目标

完成 `RuntimeInstantiationBridgeV0` 的完整 API 链路：
- **Phase 3**：值类型布局从简化占位符升级为真实递归计算（LayoutEngine 子系统）
- **Phase 4**：解释器扩展（struct 值栈 + 外部调用派发）
- **Phase 5**：方法泛型实例化（metadata 注册 + IL 执行）

## 范围边界

- 组件：`runtime-core` (LayoutEngine, ModuleRegistry, InterpreterVM, GenericContext)
- 规模支持：50+ native DLL, 30+ hotupdate DLL
- 跨 DLL：频繁跨 DLL 泛型是常态，`TypeInfoHandle` (module_id, token) 编码负责跨模块身份

## 非目标

- 不做泛型共享（每个封闭实例化独立生成 C++ 代码，与现有设计一致）
- 不做 JIT/本地 thunk 缓存
- 不做 IL 完整 opcode 覆盖（增量 TDD，以实际测试用例驱动）
- 不做泛型虚方法派发（独立工作项，不在本 roadmap 范围）

## 全局优先级约束

每条子任务在方案选择时需遵循：
- **P1 性能最优** > **P2 方案完美性** > **P3 HotUpdate 支持**
- 本 roadmap 所有子任务均已在上游 brainstorm 中完成优先级裁决

---

## 阶段与子任务

### Phase A: 基础架构（Batch 1 — 可并行）

#### Task A1: LayoutEngine 子系统

| 字段 | 值 |
|------|-----|
| task_id | `layout-engine` |
| phase | A |
| status | ready |
| purpose | 值类型布局从简化占位符升级为真实递归计算 |
| dependencies | 无 |
| conflict_scope | `src/native/runtime-core/layout_engine.*`, `src/native/runtime-core/runtime_instantiation.cpp`, `src/native/runtime-core/CMakeLists.txt` |
| estimated_effort | 中 (~500-800 行 C++, ~200 行测试) |

**Requirements**:
1. 新建 `layout_engine.h` — `LayoutEngine` 类声明（`GetOrComputeLayout`, `PrecomputeModuleLayouts`, `InvalidateModuleCache`, `ResolveSizeAndAlignment`）
2. 新建 `layout_engine.cpp` — 实现递归类型大小解析 + 对齐规则 + 缓存 + 循环检测
3. 修改 `runtime_instantiation.cpp` — `ComputeValueTypeLayout` 调用 `LayoutEngine` 替代简化占位符
4. 修改 `CMakeLists.txt` — 添加 `layout_engine.cpp`
5. 修改 Codegen 端 — 在 `native-aot.module.h` 中发射 `LayoutExpectation[]` 常量数组
6. 修改 `runtime_core.cpp` — Bootstrap 中调用 `VerifyModuleLayouts`
7. 单元测试 — LayoutEngine 独立可执行测试（测试各种 struct 布局、嵌套、泛型参数替换）

**Deliverables**:
- `src/native/runtime-core/layout_engine.h` (new)
- `src/native/runtime-core/layout_engine.cpp` (new)
- 修改 `runtime_instantiation.cpp`
- 修改 `CMakeLists.txt`
- 修改 Codegen C# 端发射 LayoutExpectation
- 修改 `runtime_core.cpp` 注册验证
- `tests/unit/layout/test_layout_engine.cpp` (new)

**Exit Criteria**:
- Build 0 error (C# + native)
- LayoutEngine 单元测试通过（递归解析、对齐、缓存命中/未命中、循环检测）
- `ComputeValueTypeLayout` 不再使用简化占位符，全量通过 LayoutEngine
- 至少 1 个 AOT 模块的 LayoutExpectation bootstrap assert 通过

---

#### Task A2: Module Registry 扩容 + Tombstone

| 字段 | 值 |
|------|-----|
| task_id | `module-registry-expansion` |
| phase | A |
| status | ready |
| purpose | 支持 80+ 模块规模，Hotunload 安全降级 |
| dependencies | 无 |
| conflict_scope | `src/native/runtime-core/module_registry.h`, `src/native/runtime-core/module_registry.cpp` |
| estimated_effort | 小 (~150 行 C++) |

**Requirements**:
1. `kMaxModules`: 64 → 256
2. `ModuleDescriptor` 增加 `bool tombstone` 字段
3. 修改 `UnregisterModuleGenerics` 实现 tombstone 标记（不删除 module 条目）
4. 新增 `FreeModuleId(uint32_t id)` + 空闲列表分配器
5. 修改 `RegisterModule` 优先从空闲列表分配
6. 修改 `LookupModule` 对 tombstone 模块仍返回合法指针（但 type_count = 0）
7. 修改 `IsModuleTombstone(id)` 辅助函数

**Deliverables**:
- 修改 `module_registry.h` — 字段 + 函数声明
- 修改 `module_registry.cpp` — 实现

**Exit Criteria**:
- Build 0 error
- 注册 80 个模块不溢出
- Hotunload 后 `LookupModule(id)` 返回 tombstone 标记的合法指针
- 释放的 module_id 可被新模块重新使用

---

### Phase B: 执行引擎（Batch 2 — 串行，依赖 Phase A）

#### Task B1: MinimalInterpreter 扩展

| 字段 | 值 |
|------|-----|
| task_id | `minimal-interpreter` |
| phase | B |
| status | planned |
| purpose | 支持值类型栈 + 外部调用派发 |
| dependencies | Task A1 (LayoutEngine — 需要 struct 大小解析) |
| conflict_scope | `src/native/interpreter/interpreter_vm.h`, `src/native/interpreter/interpreter_vm.cpp`, `src/native/runtime-core/runtime_instantiation.cpp` |
| estimated_effort | 中 (~400-600 行) |

**Requirements**:
1. `ValueTag::Struct` 新增枚举值 + `InterpreterValue` 的 inline 小结构体存储
2. `ExecutionResult` 新增 `needs_external_dispatch` + `call_data`
3. Call/CallVirt/CallBridge → 设 `needs_external_dispatch = true`，返回
4. `RuntimeInstantiationBridge` 接收 `NEEDS_EXTERNAL_DISPATCH` → 通过 `RuntimeAbiV0.method_invoke` 路由
5. 结构体字段访问：`LdFld`/`StFld` 在 Struct tag 下的正确 offset 读写
6. 帧生命周期：`ExecutionFrame` 析构时释放 struct 数据的堆内存
7. 集成测试：解释器执行含 struct 局部变量和外部调用的 IR 方法

**Deliverables**:
- 修改 `interpreter_vm.h` — Struct tag, ExecutionResult 扩展
- 修改 `interpreter_vm.cpp` — 实现
- 修改 `runtime_instantiation.cpp` — NEEDS_EXTERNAL_DISPATCH 路由
- 集成测试

**Exit Criteria**:
- Build 0 error
- 值类型在栈上正确 push/pop (含 LdFld/StFld 读写)
- Call opcode → RuntimeAbiV0.method_invoke 调用成功
- 帧析构时 struct_data 内存正确释放
- 至少 1 个端到端测试（解释器方法含 struct + call）

---

#### Task B2: 方法泛型实例化 Metadata

| 字段 | 值 |
|------|-----|
| task_id | `method-instantiation-metadata` |
| phase | B |
| status | planned |
| purpose | resolve_or_instantiate_method 返回可查询的 MethodInfoHandle |
| dependencies | Task A2 (需要 module_id 回收) |
| conflict_scope | `src/native/runtime-core/generic_method_instantiation.*`, `src/native/runtime-core/generic_context.*`, `src/native/runtime-core/runtime_instantiation.cpp`, `CMakeLists.txt` |
| estimated_effort | 中 (~400 行) |

**Requirements**:
1. 新建 `generic_method_instantiation.h` — `RuntimeInstantiatedMethod` 结构声明
2. 新建 `generic_method_instantiation.cpp` — `CreateClosedMethodDescriptor`, `ResolveOrInstantiateMethod`
3. 修改 `generic_context.h/cpp` — 新增方法泛型的注册/查询 API（`RegisterGenericMethodInstantiation`, `TryResolveClosedMethod`）
4. 修改 `runtime_instantiation.cpp` — `resolve_or_instantiate_method` 从 nullptr stub 改为真实实现
5. MethodInfoHandle 编码/解码（与 TypeInfoHandle 相同的 tag-encoding 模式）
6. 调用已实例化的方法时 → 返回合理的错误"runtime method body execution not yet available"（因为 Phase B3 还未做）

**Deliverables**:
- `src/native/runtime-core/generic_method_instantiation.h` (new)
- `src/native/runtime-core/generic_method_instantiation.cpp` (new)
- 修改 `generic_context.h/cpp`
- 修改 `runtime_instantiation.cpp`
- 修改 `CMakeLists.txt`
- 测试：MakeGenericMethod → 返回 valid handle → metadata 查询正常

**Exit Criteria**:
- Build 0 error
- `resolve_or_instantiate_method` 返回 valid MethodInfoHandle（不再返回 nullptr）
- 方法泛型的 metadata 查询全部可用（Name, GetParameters, GetGenericArguments...）
- GenericContextRegistry 方法泛型条目正确注册和查询
- 模块卸载时方法泛型条目正确清理

---

#### Task B3: 方法泛型实例化 IL 执行

| 字段 | 值 |
|------|-----|
| task_id | `method-instantiation-il-execution` |
| phase | B |
| status | planned |
| purpose | 运行时泛型方法体可通过 MinimalInterpreter 执行 |
| dependencies | Task B1 (需要 MinimalInterpreter) + Task B2 (需要 metadata 框架) |
| conflict_scope | `src/native/runtime-core/generic_method_instantiation.cpp`, `src/native/interpreter/` |
| estimated_effort | 中 (~500 行, 含 TDD 驱动) |

**Requirements**:
1. IL→IR Lowering 核心：将 IL 字节码流解码为 `IRInstruction[]`
   - 初始支持 IL opcode 子集：`ldarg`, `ldloc`, `stloc`, `ldc.i4`, `ldc.i8`, `add`, `sub`, `call`, `ret`, `br`, `brtrue`, `brfalse`, `ceq`, `newobj`, `box` 等
   - 类型参数替换：IL token 中的泛型参数 → 替换为 TypeInfoHandle
2. 与 Task B2 集成：在首次调用方法时延迟执行 IL→IR Lowering
3. TDD 驱动新增 opcode：每次只新增一个 opcode，先写测试再实现
4. exception 处理：与 MinimalInterpreter 一致的 NEEDS_EXTERNAL_DISPATCH 模式
5. 端到端测试：MakeGenericMethod + 调用 + 验证结果正确

**Deliverables**:
- 修改 `generic_method_instantiation.cpp` — IL→IR lowerer
- 修改 `interpreter_vm.h/cpp` — 如有新的 opcode 支持
- 端到端测试

**Exit Criteria**:
- Build 0 error
- 至少 1 个泛型方法可通过 `MakeGenericMethod` → 调用 → 正确执行
- 覆盖 IL opcode: ~15-20 个核心 opcode
- 与 HybridCLR 或 .NET 运行时行为一致

---

## 依赖图

```
                    ┌──────────────────┐
                    │  Task A1         │ ← LayoutEngine
                    │  LayoutEngine    │
                    └────────┬─────────┘
                             │
                    ┌────────▼─────────┐
                    │  Task B1         │ ← MinimalInterpreter (依赖 A1 struct size)
                    │  MinimalInterp   │
                    └────────┬─────────┘
                             │
               ┌─────────────┼─────────────┐
               │             │             │
    ┌──────────▼──────┐     │    ┌─────────▼─────────┐
    │  Task A2        │     │    │  Task B2          │ ← Method Metadata (依赖 A2)
    │  Registry 扩容  │─────┼───→│  Method Metadata  │
    └─────────────────┘     │    └─────────┬─────────┘
                            │              │
                            │    ┌─────────▼─────────┐
                            │    │  Task B3          │ ← IL Execution (依赖 B1+B2)
                            │    │  Method IL Exec   │
                            └────┴───────────────────┘
```

---

## 子任务映射

| task_id | phase | status | purpose | depends_on | conflict_scope | est_effort |
|---------|-------|--------|---------|------------|----------------|------------|
| layout-engine | A | ready | LayoutEngine 子系统 | 无 | `src/native/runtime-core/layout_engine.*`, `runtime_instantiation.cpp`, `CMakeLists.txt` | 中 |
| module-registry-expansion | A | ready | Module Registry 扩容 + Tombstone | 无 | `src/native/runtime-core/module_registry.*` | 小 |
| minimal-interpreter | B | planned | MinimalInterpreter 扩展 | layout-engine | `src/native/interpreter/*`, `runtime_instantiation.cpp` | 中 |
| method-instantiation-metadata | B | planned | 方法泛型 Metadata | module-registry-expansion | `generic_method_instantiation.*`, `generic_context.*`, `runtime_instantiation.cpp` | 中 |
| method-instantiation-il-execution | B | planned | 方法泛型 IL 执行 | minimal-interpreter, method-instantiation-metadata | `generic_method_instantiation.cpp`, `interpreter/*` | 中 |

---

## 风险

| 风险 | 阶段 | 影响 | 缓解 |
|------|------|------|------|
| LayoutEngine 递归解析遇到预想不到的 IL 类型系统构造 | A | 布局偏差 → 运行时错位 | LayoutExpectation bootstrap assert 捕获 |
| ModuleRegistry tombstone 标记后旧 handle 的使用路径未全部处理 | A | 逻辑错误的 type lookup | 在 `GetTypeDescriptorFromHandle` 中统一处理 type_count=0 |
| IL→IR lowerer 对 .NET 异常处理子句 (SEH) 的支持 | B3 | 异常无法正确传播 | 初始版本直接向调用方抛 C++ 异常，逐步加 SEH 支持 |
| Phase B 依赖 Phase A 的集成点多 | B | 合并冲突 | 采用 Phase A merge 后再启动 B 的策略 |
| Hotunload 重新加载后 module_id 被回收，旧 handle 指向不同类型 | A | 静默类型错误 | Tombstone + is_registered 双重保护 |

---

## 当前建议推进顺序

```
Task A1 (LayoutEngine) ─→ Task B1 (MinimalInterpreter) ─→ Task B2/B3
  └─ Task A2 (Registry) ────→ (可并行与 Task A1)
```

串行模式：先 A1 → A2 → B1 → B2 → B3
（A1 和 A2 无依赖，但在此会话中顺序执行不损失正确性）

---

## 子任务执行策略

```
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```
