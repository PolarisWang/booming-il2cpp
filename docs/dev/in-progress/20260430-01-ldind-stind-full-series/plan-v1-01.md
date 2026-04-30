# ldind/stind 全系列补齐实现计划

> **面向执行 Agent：** 使用 dev:subagent-driven-development 或 dev:executing-plans 执行。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 补齐 native-aot codegen 中 ldind.* 和 stind.* 所有 18 个变体的 IL 指令支持，用 C++ 模板 + 参数化 C# 发射器实现，消除当前仅支持 ldind.i4/stind.i4 的限制。

**架构：** C++ 侧定义 `chaos_load_indirect<T,Tag>` / `chaos_store_indirect<T,Tag>` 泛型模板，C# 侧用 `EmitLoadIndirect` / `EmitStoreIndirect` 两个参数化发射器 + 18 行 switch case 覆盖全部变体，删除原有的两个专用方法。

**技术栈：** C++ 模板（native_types.h）+ C# 字符串 builder（MethodEmission.cs）

**架构审核模式：** critical

**结构告警重点：** 消除重复代码——原有 ldind.i4/stind.i4 两个方法模式完全相同，18 个变体需要用模板参数化而非复制粘贴。

**权责图审核主题：** C++ 模板层负责零开销间接访问抽象，C# 发射器只负责根据 IL 操作码映射类型参数，职责划分清晰。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: ldind-stind
- capabilityItem: native-aot indirect load/store
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: []

**设计文档：** n/a（方案在 brainstorming 中已批准）

**问题清零来源：** direct user confirmation（用户确认方案 B）

**计划来源：** direct-plan

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

**全局优先级约束：**
- 优先级 1（性能最优）：C++ 模板编译内联后与手写代码完全等价，零抽象开销
- 优先级 2（方案完美性）：单一模板 + 参数化发射器，DRY 架构
- 优先级 3（HotUpdate 支持）：新增变体只需一行 switch case，模板自动支持
- 冲突时低优先级让位于高优先级

---

## 步骤

### Step 1: C++ 模板定义

**文件：** `src/native/common/chaos/native_types.h`

新增模板（文件末尾，保护宏 `CHAOS_IL2CPP_COMMON_NATIVE_TYPES_H_` 之前）：

```cpp
// ── Indirect load/store templates ────────────────────────────
#define CHAOS_IL2CPP_RAW_POINTER_TAG  CHAOS_IL2CPP_UINTPTR(1) << 63

template<typename T>
inline T chaos_load_indirect(CHAOS_IL2CPP_INTPTR address) {
    if (address == 0) { CHAOS_IL2CPP_ABORT(); }
    auto addr = static_cast<CHAOS_IL2CPP_UINTPTR>(address);
    if ((addr & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0) {
        return *reinterpret_cast<const T*>(addr & ~CHAOS_IL2CPP_RAW_POINTER_TAG);
    }
    return static_cast<T>(*chaos_resolve_native_int_slot(address));
}

template<typename T>
inline void chaos_store_indirect(CHAOS_IL2CPP_INTPTR address, T value) {
    if (address == 0) { CHAOS_IL2CPP_ABORT(); }
    auto addr = static_cast<CHAOS_IL2CPP_UINTPTR>(address);
    if ((addr & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0) {
        *reinterpret_cast<T*>(addr & ~CHAOS_IL2CPP_RAW_POINTER_TAG) = value;
        return;
    }
    *chaos_resolve_native_int_slot(address) = static_cast<CHAOS_IL2CPP_INTPTR>(value);
}
```

**变更摘要：**
- 新增 `CHAOS_IL2CPP_RAW_POINTER_TAG` 宏（取代 `chaos_raw_int32_pointer_tag`，bit 63）
- `chaos_load_indirect<T>`：模板参数 T 决定读取宽度
- `chaos_store_indirect<T>`：模板参数 T 决定写入宽度
- 生成的 C++ 代码由现有 `#include <chaos/native_types.h>` 自动引入

### Step 2: C# 参数化发射器 + switch cases

**文件：** `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`

**2a.** 删除原有方法（两处删除）：
- `EmitLoadIndirectInt32` — 第 2041-2063 行
- `EmitStoreIndirectInt32` — 第 2065-2086 行

**2b.** switch case 中原来的两行：
```csharp
case "ldind.i4":  EmitLoadIndirectInt32(builder, nextOffset, instruction.Op); break;
case "stind.i4":  EmitStoreIndirectInt32(builder, nextOffset, instruction.Op); break;
```

替换为 18 行参数化调用（见下表）。

**2c.** 新增 `EmitLoadIndirect` 方法：

```csharp
private static void EmitLoadIndirect(StringBuilder builder, string nativeType, string stackOp, int? nextOffset, string op)
{
    builder.AppendLine("    {");
    builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
    builder.AppendLine($"        const auto chaos_value = chaos_load_indirect<{nativeType}>(chaos_address);");
    builder.AppendLine($"        chaos_eval_stack[chaos_stack_top++] = {stackOp}(chaos_value);");
    builder.AppendLine("    }");
    AppendGotoNext(builder, nextOffset, op);
}
```

**2d.** 新增 `EmitStoreIndirect` 方法：

```csharp
private static void EmitStoreIndirect(StringBuilder builder, string nativeType, string valueCast, int? nextOffset, string op)
{
    builder.AppendLine("    {");
    builder.AppendLine($"        const auto chaos_value = {valueCast}(chaos_eval_stack[--chaos_stack_top]);");
    builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
    builder.AppendLine($"        chaos_store_indirect<{nativeType}>(chaos_address, chaos_value);");
    builder.AppendLine("    }");
    AppendGotoNext(builder, nextOffset, op);
}
```

**2e. 18 个 switch case（按 ldind 和 stind 分类插入当前位置）：**

Load（10 条）：
| switch case | nativeType | stackOp |
|---|---|---|
| `ldind.i4` | `CHAOS_IL2CPP_INT32` | `static_cast<CHAOS_IL2CPP_INTPTR>` |
| `ldind.u1` | `CHAOS_IL2CPP_UINT8` | `static_cast<CHAOS_IL2CPP_INTPTR>` |
| `ldind.i1` | `CHAOS_IL2CPP_INT8` | `static_cast<CHAOS_IL2CPP_INTPTR>` |
| `ldind.u2` | `CHAOS_IL2CPP_UINT16` | `static_cast<CHAOS_IL2CPP_INTPTR>` |
| `ldind.i2` | `CHAOS_IL2CPP_INT16` | `static_cast<CHAOS_IL2CPP_INTPTR>` |
| `ldind.u4` | `CHAOS_IL2CPP_UINT32` | `static_cast<CHAOS_IL2CPP_INTPTR>` |
| `ldind.i8` | `CHAOS_IL2CPP_INT64` | `static_cast<CHAOS_IL2CPP_INT64>` |
| `ldind.r4` | `CHAOS_IL2CPP_FLOAT32` | `chaos_store_float32` |
| `ldind.r8` | `CHAOS_IL2CPP_FLOAT64` | `chaos_store_float64` |
| `ldind.ref` | `CHAOS_IL2CPP_INTPTR` | `static_cast<CHAOS_IL2CPP_INTPTR>` |

Store（8 条）：
| switch case | nativeType | valueCast |
|---|---|---|
| `stind.i4` | `CHAOS_IL2CPP_INT32` | `static_cast<CHAOS_IL2CPP_INT32>` |
| `stind.i1` | `CHAOS_IL2CPP_INT8` | `static_cast<CHAOS_IL2CPP_INT8>` |
| `stind.i2` | `CHAOS_IL2CPP_INT16` | `static_cast<CHAOS_IL2CPP_INT16>` |
| `stind.i8` | `CHAOS_IL2CPP_INT64` | `static_cast<CHAOS_IL2CPP_INT64>` |
| `stind.r4` | `CHAOS_IL2CPP_FLOAT32` | `static_cast<CHAOS_IL2CPP_FLOAT32>` |
| `stind.r8` | `CHAOS_IL2CPP_FLOAT64` | `static_cast<CHAOS_IL2CPP_FLOAT64>` |
| `stind.ref` | `CHAOS_IL2CPP_INTPTR` | `static_cast<CHAOS_IL2CPP_INTPTR>` |

（注：stind.u1/stind.u2 在 IL 语义中不存在，无符号仅用于 ldind）

### Step 3: 构建验证

```bash
dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj
```

零 error（已有预存错误如 StringAndPlatform.cs 和 ThreadingSyncKernelFamily.cs 不属本次范围）。

### Step 4: Native 编译验证

```bash
# 验证 C++ native_types.h 语法正确
# 通过 cmake 构建或至少语法检查
cd build && cmake ... && make  # 或平台等价命令
```

### Step 5: 代码审查

- 确认 switch case 完整覆盖全部 18 个变体
- 确认原有的 `chaos_raw_int32_pointer_tag` 引用全部迁移到 `CHAOS_IL2CPP_RAW_POINTER_TAG`
- 确认 `EmitLoadIndirectInt32`/`EmitStoreIndirectInt32` 已被删除
- 确认 slot resolve 路径对非 i4 类型自动通过模板 `static_cast<T>` 兼容

---

## 关键文件

| 文件 | 操作 |
|------|------|
| `src/native/common/chaos/native_types.h` | 新增 `CHAOS_IL2CPP_RAW_POINTER_TAG`, `chaos_load_indirect<T>`, `chaos_store_indirect<T>` |
| `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` | switch 扩展 + `EmitLoadIndirect`/`EmitStoreIndirect` + 删除旧方法 |

## 验证方式

1. `dotnet build` 零 error（预存 error 除外）
2. 生成代码审查：生成的 C++ 使用 `chaos_load_indirect<CHAOS_IL2CPP_INT64>` 等模板调用而非 `goto`
3. 模板内联确认：C++ 模板在编译后与手写代码等价
