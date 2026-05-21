# 解释器完整 Opcode 覆盖 + SEH + 泛型虚方法派发 实现路线图

> **面向执行 Agent：** 本 roadmap 使用 hybrid 调度模型。Batch 1 (A+C) 并行执行，Batch 2 (B) 串行。每个子任务完成后必须归档目录并回写父任务 STATUS.md。

---

## 1. 目标

完成解释器完整 opcode 覆盖、SEH 异常处理纯状态机、泛型虚方法派发三个独立工作项，并通过集成测试验证。

## 2. 范围边界

| 边界 | 内容 |
|------|------|
| A | 实现解释器缺失的常用 IL opcode，包括算术、位运算、转换、间接加载/存储、dup、switch、initobj、sizeof 等 ~20-30 个 |
| B | 纯状态机 SEH：两阶段模型（搜索→展开→处理），覆盖 catch/finally/fault/filter，跨帧传播 |
| C | 泛型虚方法派发：复用 vtable_registry (token-based)，类继承链解析，CallVirt 解释器集成 |
| 测试 | 每个新增 opcode/功能点有 interpreter_integration.cpp 测试覆盖 |
| 文档 | 完成后更新 wiki（解释器 VM 架构、VTable 注册表） |

## 3. 非目标

- Overflow-check 算术变体 (add.ovf, sub.ovf, mul.ovf, conv.ovf.*) — 后续增量
- Interface 虚方法派发 — 独立工作项
- 异常对象模型的完整实现（NewObj 创建异常 → Throw）— 依赖外部 type resolution
- C++ EH 桥接方案

## 4. 阶段列表

### Phase 1: 基础能力扩展（Batch 1，并行）

**Goal:** 解释器 opcode 补全 + 虚方法派发基础能力。

**Exit Criteria:**
- 所有新增 opcode 有 interpret_integration.cpp 测试通过
- CallVirt 通过 vtable_registry 正确解析类继承链的虚方法
- 既有 16 个回归测试全部通过

**Deliverables:**
- `interpreter_vm.cpp` / `il_to_ir_lowerer.cpp` — opcode case 覆盖
- `vtable_registry.*` — CallVirt 支持方法名/类型解析
- `interpreter_integration.cpp` — 新增测试

**Dependencies:** 无

### Phase 2: SEH 异常处理（Batch 2，串行）

**Goal:** 纯状态机 SEH 完整实现。

**Exit Criteria:**
- 两阶段模型：搜索 catch → 展开 finally/fault → 进入 handler
- 覆盖场景：catch 匹配、finally 展开、fault 语义、嵌套 SEH、跨帧传播
- 解释器集成测试覆盖以上场景

**Deliverables:**
- `interpreter_vm.cpp` — 重写 Throw/EndFinally/Leave/Rethrow opcodes
- `interpreter_vm.h` — 新增状态变量（如有必要）
- `interpreter_integration.cpp` — SEH 集成测试

**Dependencies:** 建议在 Phase 1 之后（可共用完整 opcode 集编写更丰富的 SEH 测试）

## 5. 子任务映射

| task_id | phase | status | purpose | depends_on | conflict_scope |
|---------|-------|--------|---------|------------|----------------|
| interpreter-opcodes | batch-1 | ready | 实现 ~20-30 个缺失 IL opcode | 无 | `src/native/interpreter/interpreter_vm.cpp`、`src/native/runtime-core/il_to_ir_lowerer.cpp` |
| generic-vtable-dispatch | batch-1 | ready | 实现泛型虚方法派发 vtable 注册表 | 无 | `src/native/runtime-core/vtable_registry.*` |
| seh-exception-handling | batch-2 | planned | SEH 纯状态机实现 | interpreter-opcodes | `src/native/interpreter/interpreter_vm.cpp`、`interpreter_integration.cpp` |

## 6. 依赖

- Batch 1 两个子任务无相互依赖，无文件冲突
- Batch 2 建议在 opcodes 之后（利用完整 opcode 集编写 SEH 测试）

## 7. 风险评估

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| opcode 遗漏 | 低 | 中 | 增量 TDD，集成测试覆盖每个新增 opcode |
| SEH 状态机边缘 case | 中 | 中 | 参照 ECMA 335 标准定义的两阶段模型，逐步扩展 catch→finally→fault→filter |
| vtable 跨模块类型解析 | 低 | 低 | vtable_registry 已通过 Phase 5 验证，ResolveVirtualMethodPointer 已可用 |
| 并行合并冲突 | 低 | 高 | A 与 C 已确认无文件重叠 |

## 8. 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

- Batch 1 完成后自动合并并启动 Batch 2
- 每个子任务进入终态后必须物理移入 `docs/dev/completed/`
- 父任务 STATUS.md 在每次子任务完成后同步更新

## 9. 调度策略

```yaml
dispatch_model: hybrid
```

详见 DISPATCH.md。
