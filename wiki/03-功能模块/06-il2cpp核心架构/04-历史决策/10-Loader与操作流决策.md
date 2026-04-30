# Loader、SemanticWorld 与操作流决策

**来源**: `docs/discuss/il2cpp-mainline/20260408-10-*` ~ `20260408-14-*`

## Loader vs SemanticWorld 职责（20260408-10）

**决策**：

- `Loader` 解析编译后的结构化事实（assembly/metadata/IL decode）
- `SemanticWorld` 将这些事实归一化为当前支持切片内的规范语义含义
- Loader 回答"文件里有什么"
- SemanticWorld 回答"在当前 mainline 中它意味着什么"

## 语义操作流粒度（20260408-11）

**决策**：采用 semantic-action-level 操作流。

- 操作比原始 IL opcode 更具语义性
- 比 lowering family 更细粒度、更中性
- 纯栈 shuffle 细节和 lowering/runtime/backend 决策名称被排除在 canonical stream 外

## 语义操作词汇表（20260408-12）

**决策**：冻结覆盖当前 proof spine 的最小共享词汇表：

| 操作 | 说明 |
|------|------|
| `construct-object` | 构造对象 |
| `read-instance-field` | 读取实例字段 |
| `invoke-instance-method` | 调用实例方法 |
| `string-concat` | 字符串连接 |
| `emit-console-string` | 控制台字符串输出 |
| `query-closed-type-member` | 查询封闭类型成员 |

共 14 个操作。成员种类/目标/字面量差异进入 payload 而非操作名。

## 操作 Payload 合约（20260408-13/14）

**决策**：使用 `OperationKind` + typed payload variant。

- 所有变长数据通过 `SpanId`/side-table 引用
- 禁止 bag-shaped payload（untyped dictionary）
- Payload 格式：`{OperationId, Kind, ResultRef?, PayloadRef}` + kind-specific typed payload + shared spans
- 接收者/主体角色保持为显式字段

## 关键约束

- Loader 不做语义转换
- 操作流降低于 lowering family、高于 raw IL
- Payload 使用 typed variant 而非字典
