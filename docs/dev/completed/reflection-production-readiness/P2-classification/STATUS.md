# P2-1..P2-5 — 三档判定落地

> **task_id**: reflection-p2-three-tier-classification
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

逐 API 完成三档判定，消除所有 `unclassified`，使矩阵每一行都有明确归宿；**不允许任何 API 停留在「静默返错值」**。

## Inputs

- Phase 0 矩阵：501 个 `unclassified`（经分母修正后 425）
- Phase 1 证据：`tests/contracts/managed/reflection-semantic-contract/`
- 现有 native 面侦察：`reflection_api.h`（82 符号）、codegen 30 项黑名单

## Expected Outputs

- 判定完成的 `api-coverage-matrix.json`（无 unclassified）
- 扩展的 `Classifier.cs`（判定规则固化）

## Exit Criteria

- ✅ **矩阵无 `unclassified`**（Phase 2 核心 exit criterion）
- ✅ 每个 `not-supported` 有明确理由
- ✅ 判定规则固化在可复现的工具中，非一次性手工记录

## Terminal Notes

### 最终矩阵

| tier | 数量 | 占比 |
|---|---|---|
| **`real`**（有实现依据） | **398** | 71.3% |
| **`not-supported`**（结构不可行） | **160** | 28.7% |
| **分母** | **558** | 100% |
| `unclassified` | **0** | — |
| ~~`enum-surface`~~（排除） | 193 | — |
| ~~`data-carrier`~~（排除） | 76 | — |

### 判定规则（固化在 `Classifier.cs`）

按优先级顺序：

1. **`KnownNativeImpls`** — 82 个 `Chaos*` native 符号的显式映射 → `real`
2. **`NotSupportedMembers`** — 成员级结构不可行（如 `Assembly.Load`、`MethodBody.GetILAsByteArray`）→ `not-supported`
3. **`NotSupportedWholeTypes`** — 整类型不可行（`MethodBody`/`LocalVariableInfo`/委托类型等）→ `not-supported`
4. **`NotSupportedTypes`** — 类型级不可行（`TypeDelegator`/`Binder`/`IReflect` 等）→ `not-supported`
5. **`DescriptorBacked`** — Tier-2 descriptor 已持有数据、native 访问器为直接读取 → `real`（fact 档，Phase 3 落地）
6. 兜底 → `unclassified`

### 分母修正（执行中的重要发现）

初次扫描 849，经两轮口径修正收敛到 **558**：

| 修正 | 排除数 | 理由 |
|---|---|---|
| 枚举常量分离 | 193 | `BindingFlags`/`TypeAttributes` 等是**元数据值定义**，非反射操作；不分离会虚高分母 30% |
| 数据载体分离 | 76 | 异常类（`TargetException` 等）与特性类（`AssemblyVersionAttribute` 等）是**普通 BCL 类**，其成员走通用翻译路径，无反射操作语义 |
| not-applicable | 0 | `Reflection.Emit` 命名空间前缀已排除，但 ref pack 中无此程序集（见 Watch Items） |

**这是本次评审「验证真实性」原则的又一次应用**：若不修正口径，会得出"反射有 849 个 API 待实现"的错误结论，把 269 个非操作项误计入工作面。

### not-supported 分布（160 项，全部有理由）

| 类型 | 数量 | 理由 |
|---|---|---|
| `TypeDelegator` | 47 | 需要活的托管 Type 实现链 |
| `Assembly` | 25 | 运行时程序集加载（Load/CreateInstance/LoadModule 等） |
| `IReflect` | 12 | COM 时代后期绑定接口，AOT 无语义 |
| `AssemblyName` | 9 | 强名密钥材料不在 AOT 元数据中 |
| `ExceptionHandlingClause` | 8 | 需要 IL/方法体元数据 |
| `ConstructorInvoker`/`MethodInvoker` | 14 | 需 `Delegate.CreateDelegate` |
| `Binder` | 6 | 自定义模型绑定器 |
| `MethodBody` | 6 | AOT 无 IL 字节数组 |
| `LocalVariableInfo` | 4 | 需要 IL/方法体元数据 |
| 其他 | 29 | 委托类型/`IReflectableType`/`Pointer` 等 |

### real 档说明（398 项，fact 档）

398 项均为 **`realKind: "fact"`** —— 即「语义正确」档，**不含性能要求**（design §3.2）。

其中：
- **26 项**有真实 native 符号且已在 Phase 1 通过 52 个语义断言验证
- **372 项**为 descriptor-backed 访问器（数据已在 Tier-2 descriptor 中，native 侧为直接读取），标记为 Phase 3 落地目标

### 关于「显式抛异常」的执行说明

本阶段**未修改任何运行时行为**，仅完成判定。改抛异常属 Phase 3 的实施动作，且必须遵循 §11.2 的**测试先行**强制顺序：

```
造清单（本阶段完成）→ 为每个待改 API 先写断言其正确行为的测试
→ 跑到红 → 再改实现（抛 NotSupportedException）→ 跑到绿
```

**本阶段产出的 160 项 `not-supported` 清单，正是 Phase 3 测试先行的输入。**

## Watch Items（移交 Phase 3）

- ⚠️ **372 项 descriptor-backed `real` 是「判定」而非「实现」**：数据字段存在 ≠ 访问器已接线。Phase 3 必须逐项验证 native 访问器真实存在，不能假定。
- ⚠️ **`Reflection.Emit` 程序集未纳入枚举范围**：`System.Reflection.Emit*.dll` 不在 `Microsoft.NETCore.App.Ref/8.0.11/ref/net8.0` 中。需确认这些 API 应显式登记为 `not-applicable` 还是走独立追踪。
- ⚠️ **160 项 `not-supported` 需在 Phase 3 实际抛出 `NotSupportedException`**：当前 AOT 侧多为返回 null/0，与 managed 行为不一致（Phase 1 已归因 2 例：`TypeDelegator.GetConstructors/GetMethods`）。
- ⚠️ **`Assembly.get_FullName`/`get_Location` 等被归入 `real`**：它们当前是硬编码返回值（`"v4.0.30319"`/`"unknown"`），严格说属「静默返回非真实值」。Phase 3 需复核并决定改为真实实现还是归为 `not-supported`。
