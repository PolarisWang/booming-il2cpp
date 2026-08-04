# 异常等效性验证：try/catch/finally 的 C++ 语义等价性

> 计划编号: P1-EXCEPTION-EQUIV
> 创建日期: 2026-05-17
> 状态: draft

## 问题描述

当前 codegen 的结构化 IR 通过 C++ 的 `if/while/switch/try-catch` 表达 IL 的异常处理语义。但:

1. **嵌套 finally 的线性 vs 树形差异**: IL 的 EH 表是线性的（`Clause` 列表，按 nesting 排序），而结构化 IR 生成嵌套的 C++ `try { try { } finally { } } finally { }`。两者的语义等价性在复杂嵌套场景下未经验证。
2. **leave → 跳转 finally**: IL 的 `leave` 指令语义是"退出 protected region，执行关联的 finally，然后跳转到目标"。C++ 的 `goto` 不能跳出 try 块作用域。当前通过 `IRExceptionRegion` 的结构化包装来处理，但覆盖有限。
3. **filter 异常**: `try-filter-catch` 仅有 `TryCreateFilterOnlyExceptionMethodShape` 一条路径，无实测验证。

## 目标

建立 IL 异常语义和 codegen 生成 C++ 代码之间的等效性验证，确保所有标准 EH 模式都能正确编译。

## 执行步骤

### Step 1: EH 模式枚举与覆盖率统计

**入口**: 新建 `docs/archive/reports/exception-coverage-audit.md`

枚举所有 EH 模式，标记当前 codegen 的覆盖状态：

| EH 模式 | StructuredIR | FlatGoto | 状态 |
|---------|-------------|----------|------|
| try-catch (单catch) | ✅ | ✅ | 已验证 |
| try-catch (多catch) | ⚠️ 未验证 | ✅ | 待验证 |
| try-finally | ✅ | ✅ | 已验证 |
| try-filter | ⚠️ 部分 | ✅ | 待验证 |
| try-catch-finally | ⚠️ 未验证 | ✅ | 待验证 |
| try-filter-finally | ❌ 未知 | ✅ | 待验证 |
| nesting try (2层) | ⚠️ 未验证 | ✅ | 待验证 |
| nesting try (3+层) | ❌ 未知 | ✅ | 待验证 |
| nesting finally in catch | ❌ 未知 | ✅ | 待验证 |
| leave in try | ✅ | ✅ | 已验证 |
| leave in catch | ⚠️ 未验证 | ✅ | 待验证 |
| leave in finally | ⚠️ 语义不符 | ✅ | 待验证 |

### Step 2: StructuredIR 的嵌套 EH 修复

**入口**: `NativeAotLoweringPlanner.StructuredIR.cs` 的 `BuildExceptionIRBody`

当前 `BuildExceptionIRBody` 对嵌套异常的处理是递归构建 `IRExceptionRegion` 树。但 `EmitIRExceptionRegion` 的 setjmp/longjmp 实现要求正确的 buffer 配对。

需要修复的关键点：
- `pop_exception_jmp_buf()` 必须在每个 handler 退出前被调用（当前 only in catch body）
- finally 内部的嵌套异常 → 需要新 setjmp 点
- filter 异常 → 验证 `FilterInstructions` 在 C++ 层面的正确性

### Step 3: 添加验证 subject

在 foundation-dll 中添加覆盖上述 EH 模式的验证 subject：

- `exception-nesting`: 3 层嵌套 try-catch-finally
- `exception-leave-in-catch`: catch 块中 leave
- `exception-multi-catch`: 多 catch 块（不同异常类型）
- `exception-filter`: try-filter-catch

### Step 4: 验证

- 新 subject 在 foundation-dll 管线中全部通过
- 对比 IL 预期行为（用 .NET Runtime 跑原始 IL 作为 baseline）和 codegen 产出的 C++ 行为

## Test Governance Intake

| 字段 | 值 |
|------|-----|
| capabilityFamily | codegen-exception |
| capabilityItem | exception-semantic-equivalence |
| ownerSubjectId | CodeGen/NativeAotLoweringPlanner |
| proofRequired | foundation-dll 异常 subjects 全部通过 |
| benchmarkRequired | 否 |
| hotupdateImpact | 无 — hotupdate 异常走 interpreter VM，不走 codegen EH 路径 |
| formalVerificationObjects | 每个 EH 模式对应一个 foundation-dll subject |
| requiredGates | collector → registry → workspace |

## 终态条件

1. 所有标准 EH 模式在覆盖矩阵中标记为已验证
2. foundation-dll 异常 subjects 全部通过
3. 嵌套 finally 的语义等价性通过双端对比（IL baseline == codegen output）
