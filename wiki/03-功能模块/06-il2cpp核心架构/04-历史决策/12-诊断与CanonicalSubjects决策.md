# 诊断、CapabilityBundle、SemanticShapes 与 CanonicalSubjects 决策

**来源**: `docs/archive/discuss/il2cpp-mainline/20260408-20-*` ~ `20260408-25-*`

## SemanticWorld 诊断合约（20260408-20）

**决策**：`SemanticWorld.Diagnostics` 作为正式合约，覆盖 7 个类别：

| 类别 | 说明 |
|------|------|
| identity | 标识诊断 |
| input | 输入诊断 |
| operation | 操作诊断 |
| reference | 引用诊断 |
| def-use | 定义-使用诊断 |
| support | 支持诊断 |
| invariant | 不变式诊断 |

任何 `error` 级别的诊断阻止 `Linker` 消费该 `SemanticWorld`。canonical pipeline 不做部分消费。

## CapabilityBundle 合约（20260408-22）

**决策**：采用方法级 primary bundles + world-slice aggregate bundle。

Capability vocabulary 冻结为 6 项：

| Capability | 说明 |
|-----------|------|
| `uses-instance-field-state` | 使用实例字段状态 |
| `requires-string-concat` | 需要字符串连接 |
| `requires-console-string-output` | 需要控制台字符串输出 |
| `requires-closed-type-member-query` | 需要封闭类型成员查询 |
| `requires-generic-type-definition-query` | 需要泛型类型定义查询 |
| `requires-imported-call` | 需要 imported call |

Capability 是需求摘要，不是操作 dump 或 family classifier。

## SemanticShapes 合约（20260408-23）

**决策**：per-subject-kind typed shapes，3 个 family：

| Shape | 维度 |
|-------|------|
| `TypeShape` | 类型形状 |
| `FieldShape` | 字段形状 |
| `MethodShape` | Role（constructor/instance-method/static-method/imported-method）+ BodyAvailability（has-canonical-body/no-canonical-body）作为正交维度 |

`entry` 明确不是 shape——它属于 Linker 语义。

## CanonicalSubjects 合约（20260408-24）

**决策**：typed subject families + explicit declaring graph + lightweight member projection。

| 主 Subject | 说明 |
|-----------|------|
| `TypeId` | 类型标识 |
| `MethodId` | 方法标识 |
| `FieldId` | 字段标识 |

`MemberId` 是 canonical umbrella projection，不替代原子 SubjectId。`CanonicalSubjects` 只携带定义标识，不携带封闭实例化标识。

## StableMetadataLookup 合约（20260408-25）

**决策**：独立的受控定义级 stable metadata lookup companion boundary，不作为第 7 个 canonical artifact block。

- 查询 key 只能是 canonical IDs
- 返回：identity-display、declaration-signature、loader-owned definition annotations
- 禁止：instantiated generic context 查询
- 下游组件不得将 metadata lookup 用作语义事实源

## 关键约束

- error 级别诊断阻断管线消费
- Capability 是需求摘要，不是操作 dump
- entry 不是 shape
- 元数据查找不能用作语义事实源
