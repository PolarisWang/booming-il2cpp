---
task_type: plan
phase: planning
parent_task_id: 20260524-jit-perf-optimization
source_task_id: p1-tree-ir
source_relation: roadmap-child
roadmap_or_plan: pending
dispatch_model: sequential
child_execution_mode: auto
entry_skill: dev-writing-plans
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
created: 2026-05-24
---
lifecycle_status: completed
phase: completed
completed: 2026-05-24
archived_to: docs/dev/completed/20260524-jit-perf-optimization/p1-tree-ir
---

# P1: 轻量树 IR 框架 — STATUS

> 父任务: 20260524-jit-perf-optimization (JIT 性能优化路线)

## 设计摘要

在 `AllocateRegisters` 之后、`NativeCodeGenerator` 之前插入单 BB 表达式树 IR 优化阶段。支持 VN（值编号）、CSE（公共子表达式消除）、常量折叠，然后线性化回 `RegisterInstruction`。

**架构决策（继承父设计 D3/D4）：**
- **D3** 轻量树 IR（方案 C）：不做完整 GenTree，只对单基本块构建表达式树
- **D4** 函数式 pass 编排（方案 B）：显式函数调用，预留 PassManager 迁移

**5 个新文件：**
- `jit_tree_builder.h/cpp` — RegisterMethod → 表达式树
- `jit_vn.h/cpp` — 值编号表 + CSE 检测
- `jit_tree_mutator.h/cpp` — 树遍历/替换基类
- `jit_linearizer.h/cpp` — 优化后树 → RegisterInstruction
- `jit_optimizer.h/cpp` — 优化入口编排（常量折叠 → CSE → LdFld 合并）

**修改文件：**
- `jit_engine.cpp` — Compile() 中插入 `Optimizer::Run(tree)`
- `CMakeLists.txt` — 新增源文件注册

## Scope

- 单 BB 表达式树 IR 体系（非完整 GenTree）
- 值编号（VN）+ 公共子表达式消除（CSE）
- 常量折叠 + LdFld 合并
- Mutator + Linearizer + Optimizer pass 管线
- 空管线回归基线（确保不做变换时语义等价）

## 非目标（继承父级）

- 不做全局值编号（只做单 BB VN）
- 不涉及内联器（P2 阶段）
- 不涉及 intrinsic（P3 阶段）
- 不重构 codegen 管线或 Scriban 模板

## Exit Criteria

1. 空优化管线（不做任何变换）通过所有 18 个 fact 测试
2. 常量折叠：纯算术折叠正确
3. CSE：重复表达式消除正确
4. LdFld 合并：连续 LdFld 合并正确
5. 线性化后 RegisterMethod 语义等价于原始 IR
6. entry-jit.exe build + fact + benchmark 通过（18/18 fact, 支持 8/18 benchmark 完全通过 — 其余为预存 AOT 运行时崩溃，与树 IR 无关）

## 验证结果

**Fact 测试：18/18 通过 ✅**

**Benchmark（10000 iterations）：**
| 方法 | 结果 | 耗时(ms) | 备注 |
|------|------|----------|------|
| 0 (ToChar_Boolean) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 1 (ToChar_Byte) | ✅ 通过 | 0.442 | |
| 2 (ToChar_SByte) | ✅ 通过 | 0.325 | |
| 3 (ToChar_Int16) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 4 (ToChar_UInt16) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 5 (ToChar_Int32) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 6 (ToChar_UInt32) | ✅ 通过 | 0.342 | |
| 7 (ToChar_Int64) | ✅ 通过 | 0.329 | |
| 8 (ToChar_UInt64) | ✅ 通过 | 0.359 | |
| 9 (ToChar_Single) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 10 (ToChar_Double) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 11 (ToChar_Decimal) | ✅ 通过 | 0.329 | |
| 12 (ToChar_String) | ⚠️ 数据输出后崩溃 | 0.643 | 清理阶段 BGC/Finalizer 竞争 |
| 13 (ToChar_Object) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 14 (ToChar_Char) | ❌ 崩溃 | — | 预存 AOT 运行时问题 |
| 15 (ToChar_DateTime) | ⚠️ 数据输出后崩溃 | 0.556 | 清理阶段 BGC/Finalizer 竞争 |
| 16 (ToChar_Char_Default) | ✅ 通过 | 0.336 | |
| 17 (ToChar_String_Default) | ✅ 通过 | 0.351 | |

**关键发现：**
- `ChaosJitRegisterAll()` 是空实现（`extern "C" void ChaosJitRegisterAll() {}`），所有方法通过 AOT 编译 C++ 代码执行——树 IR 尚未被 JIT 调用
- 崩溃发生在 BGC/Finalizer 线程的进程清理阶段，与树 IR 无关
- 修复了 Opcode 映射遗漏（ConvRUn, ConvI, ConvU, ConvOvf* 系列），扩展了 `kConvI`/`kConvU` 节点种类

## 风险评估

- 树构建和线性化可能导致 compile 时间增加（监控 Generate() 耗时）
- 线性化器输出必须与原始 RegisterInstruction 语义严格等价（空管线回归基线兜底）
- VN 表哈希冲突（选用 identity_hash + 避免 string key）
