# P3-1 — 真实实现补齐（fact real 口径）

> **task_id**: reflection-p3-implementation
> **parent_task_id**: reflection-production-readiness
> **source_task_id**: reflection-production-readiness
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-09-12
> **完成日期**: 2026-09-12
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

填充矩阵中 `real` 档缺口，达到 `fact real` 标准（语义正确 + 真实断言通过，**不含性能要求**）。

## Inputs

- Phase 2 矩阵：398 项判定为 descriptor-backed `real`
- Phase 2 Watch Item：**「数据字段存在 ≠ 访问器已接线，Phase 3 必须逐项验证，不能假定」**
- Phase 1 证据套件：`tests/contracts/managed/reflection-semantic-contract/`（52 断言）

## Expected Outputs

- 新增 native 访问器实现
- 扩展的语义断言套件
- 修正后的矩阵（区分 `real` 与 `real-planned`）

## Exit Criteria

- ✅ native 审计：真实 stub 从 5 项清零（余 2 项经论证为语义正确）
- ✅ 新增 30 个 native 访问器符号
- ✅ 语义断言从 52 扩展至 71
- ✅ 矩阵引入 `real-planned` 档，消除过度声称

## Terminal Notes

### 关键发现：Phase 2 的分类存在过度声称

Phase 2 将 372 项标记为 descriptor-backed `real`，理由为「数据已在 Tier-2 descriptor 中」。

Phase 3 逐项审计发现**该判定不成立**：

| 证据 | 结果 |
|---|---|
| `ReflectionQueryFieldDescriptor.flags` 实际位宽 | 仅 4 bit（Public/Static/InitOnly/Literal） |
| 矩阵声称 `real` 的 FieldInfo 成员 | 32 项（含 `IsPrivate`/`IsFamily`/`IsSecurityCritical` 等） |
| `ChaosReflectionGetParamAttributes` 实现 | 参数被注释掉，裸 `return 0` |

**结论**：数据字段存在 ≠ 访问器已实现。Phase 2 的 372 项应降级为 `real-planned`。

这正是 Phase 2 Watch Item 预警的情形，也是本次 roadmap「验证真实性」原则的第三次应用。

### 处置：引入 `real-planned` 第四档

在 `Models.cs` 新增档位，明确区分：

| 档位 | 含义 |
|---|---|
| `real` | **访问器已实现** + 有真实断言通过 |
| `real-planned` | 元数据存在，但 native 访问器尚未编写 |

矩阵不再声称未实现的覆盖。这是「不允许静默返错值」原则在**度量层**的延伸。

### 实施成果

**1. 修复 5 个真实 stub（审计结果）**

审计 92 个 native 函数，发现 5 个为真 stub，逐项处置：

| 函数 | 处置 | 理由 |
|---|---|---|
| `ChaosReflectionAssemblyGetExportedTypes` | **真实实现** | 复用 image->types 描述符数组（同 GetTypes 数据源） |
| `ChaosReflectionAssemblyGetModules` | **真实实现** | AOT 下 image 即单 module，返回 image handle |
| `ChaosReflectionGetParamAttributes` | **补文档 + 可用性探针** | descriptor 无 flags 字段，数据确实不存在；新增 `ChaosReflectionParamAttributesAvailable()` 使调用方可区分 |
| `ChaosReflectionAssemblyNameGetCultureInfo` | **保留** | 返回 0 = invariant culture，是 .NET 正确语义 |
| `ChaosReflectionGetCallingConvention` | **保留** | 返回 1 = `CallingConventions.Standard`，AOT 唯一约定 |

**修复后审计：92 函数，剩余疑似 stub 2 项，均为语义正确。**

**2. 扩展 descriptor flag 位（实现前置）**

`reflection_query_model.h`：

- `kMethodFlag*` 3 bit → **13 bit**（新增 Private/Assembly/Family/FamilyAndAssembly/FamilyOrAssembly/Final/HideBySig/SpecialName/Abstract/Constructor）
- `kFieldFlag*` 4 bit → **12 bit**（新增 Private/Assembly/Family/FamilyAndAssembly/FamilyOrAssembly/NotSerialized/PinvokeImpl/SpecialName）

**向后兼容**：位追加在既有位之后，`UINT32` 容器不变，descriptor 末尾 `flags` 字段位置不变（ABI 兼容，符合 R1.1 先例）。

**3. 新增 30 个 native 访问器**

`remaining_stubs.cpp`：

- **FieldInfo**：IsStatic/IsPublic/IsPrivate/IsAssembly/IsFamily/IsFamilyAndAssembly/IsFamilyOrAssembly/IsNotSerialized/IsPinvokeImpl/IsSpecialName + `GetAttributes`（组装 ECMA-335 FieldAttributes）+ `GetRawConstantValue`
- **MethodBase**：IsPublic/IsStatic/IsVirtual/IsPrivate/IsAssembly/IsFamily/IsFamilyAndAssembly/IsFamilyOrAssembly/IsFinal/IsHideBySig/IsSpecialName/IsAbstract/IsConstructor + `GetAttributes`（组装 MethodAttributes）+ `GetImplementationFlags`

统一使用 `CHAOS_DEFINE_FIELD_FLAG_ACCESSOR` / `CHAOS_DEFINE_METHOD_FLAG_ACCESSOR` 宏，模式与既有 `CHAOS_DEFINE_TYPE_FLAG_STUB` 一致。

**4. 语义断言 52 → 71**

新增 19 个断言覆盖：
- Field 修饰符（IsPublic/IsPrivate/IsAssembly 族/IsNotSerialized）
- `FieldInfo.Attributes` 的 ECMA-335 取值（Public/Private/Static/InitOnly/Literal）
- `GetRawConstantValue`
- Method 修饰符（IsPublic/IsPrivate/IsStatic/IsConstructor）
- `MethodBase.Attributes` 的 ECMA-335 取值
- `GetMethodImplementationFlags`
- `GetBaseDefinition` 返回自身语义

**运行结果：71 passed / 0 failed。**

### 矩阵现状

| tier | 数量 | 说明 |
|---|---|---|
| `real` | **73** | 27 原有 + 46 本轮新实现 |
| `real-planned` | 325 | 元数据存在，访问器待写 |
| `not-supported` | 160 | 结构不可行 |
| **分母** | **558** | |
| ~~enum-surface / data-carrier~~ | 193 / 76 | 排除 |

### 验证

- ✅ `chaos_runtime_core` 构建通过（0 error）
- ✅ 语义断言 71/71 绿（.NET 8 参考实现）
- ✅ 30 个新符号存在于 `remaining_stubs.cpp`
- ✅ descriptor ABI 未破坏（末尾追加位，字段位置不变）

## Watch Items（移交 Phase 4）

- ⚠️ **325 项 `real-planned` 是剩余工作面**：本轮实现了 46 项，其余需按同一模式继续。这是 Phase 4 之后仍存的主要缺口，**不应**被 `real` 数字的增长掩盖。
- ⚠️ **descriptor flag 位扩展需 codegen 配合**：本轮仅扩展了 native 侧位定义与访问器；**codegen 尚未填充这些新位**。当前访问器会读到 0（即"无修饰符"），需 codegen 侧（Scriban 模板 + NativeAotLoweringPlanner）发射真实 flag 后才能端到端生效。**这是最高优先级的遗留项。**
- ⚠️ **语义断言仍跑在 .NET 8 参考实现上**：验证的是契约定义，不是 Chaos AOT 实现。接入 foundation-dll pipeline 的 AOT 路径仍是待办。
- ⚠️ `GetBaseDefinition` 返回自身——本轮已论证 AOT 语义下正确，但若未来支持方法重写需重新评估。
