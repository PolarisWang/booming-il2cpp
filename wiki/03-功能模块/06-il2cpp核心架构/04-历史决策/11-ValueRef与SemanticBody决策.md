# ValueRef、LiteralRef、SpanId 与 SemanticBody 决策

**来源**: `docs/archive/discuss/il2cpp-mainline/20260408-15-*` ~ `20260408-19-*`

## ValueRef 模型（20260408-15）

**决策**：采用方法级统一 `ValueRef` 命名空间。

- 方法内所有可消费值（inputs、this、operation results）使用单一 `ValueRef` 语言
- `ValueRef` 是 SSA 风格的（单次定义），但不引入完整的 phi/merge
- 与 `OperationId` 和 IL 栈槽分离

## LiteralRef 池化模型（20260408-16）

**决策**：`SemanticWorld` 级 typed literal pool。

- `LiteralRef` 是 `SemanticWorld` 作用域的、按 `kind+value` 去重的常量条目引用
- 初始 literal kind：`string` 和 `int32`
- provenance 不参与 literal identity

## SpanId 合约（20260408-17）

**决策**：采用方法级 span table，`SpanId` 是稳定有序的 `ValueRef[]` 序列句柄。

- `SpanId` 只携带 `ValueRef[]`
- 顺序是语义上有意义的
- 空序列使用显式空 span
- receiver/subject roles 保持为 span 外的显式字段

## MethodSemanticBody 合约（20260408-18）

**决策**：显式的 `MethodSemanticBody` 容器，包含三个最小块：

```
MethodSemanticBody
 ├── Inputs     -- 输入定义
 ├── Spans      -- span 序列
 └── Operations -- 操作序列
```

World 级资源（LiteralRef、subject ids）被引用但不复制。派生摘要（`ReferencedLiteralRefs`、`Diagnostics`）排除在 canonical body 外。

## Well-Formedness 合约（20260408-19）

**决策**：`SemanticWorld` 负责结构性和局部数据流 well-formedness。

覆盖范围：body ownership、input shape、operation/result shape、reference resolution、temporal def-use。禁止 forward value references。lowering/runtime/backend readiness 不属于 body well-formedness 范围。

## 关键约束

- ValueRef 是 SSA 风格但不含 phi
- LiteralRef 按 kind+value 去重
- SpanId 顺序有语义含义
- SemanticBody 不包含派生摘要
