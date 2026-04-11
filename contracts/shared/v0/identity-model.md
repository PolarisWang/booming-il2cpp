# Shared Identity Model v0

## 目标

冻结 AOT 与热更共用的 SubjectId 规则，保证跨 artifact、跨 package、跨版本追踪都落在同一套 identity 上。

## 冻结格式

- 类型：`{Assembly}/{TypeIdentity}`
- 字段：`{TypeSubjectId}::{Field}`
- 属性：`{TypeSubjectId}::property:{Property}`
- 方法：`{TypeSubjectId}::{Method}({ParamTypeCsv})`
- 参数：`{MethodSubjectId}::parameter[{Index}]:{Name}`

## TypeIdentity 规则

- `TypeIdentity` 来自 `ManagedNaming.GetTypeIdentityPart(...)`。
- 如果 `namespace` 为空，或 `namespace == assembly`，类型部分只保留 `typeName`。
- 否则类型部分是 `{namespace}.{typeName}`。
- 嵌套类型保留 `+`，例如 `Outer+Inner`。

## 泛型规则

- 定义态 identity 保留 CLR generic arity，例如 `Namespace.List\`1`。
- 实例化 identity 先执行 `StripGenericArity(...)`，再拼 `<T1,T2>`。
- 泛型方法名使用 `Method<T1,T2>` 形式。
- 这与 `ManagedNaming.CreateInstantiatedTypeSubjectId(...)` / `CreateGenericMethodName(...)` 保持一致。

## Stable Identity 规则

- metadata token 只在“同一版本、同一 assembly”内部可视为稳定句柄。
- 跨版本引用必须使用 `SubjectId`，不能把 token 当作 stable identity。
- 热更包引用 AOT 类型/方法时，必须先用 `SubjectId` 查找，再在装载时解析到当前版本 token / runtime handle。

## 示例

- 类型：`HelloWorldObject/Greeter`
- 字段：`HelloWorldObject/Greeter::_name`
- 方法：`HelloWorld/Program::Main(System.String[])`
- 泛型实例：`GenericEcho/GenericHolder<System.Int32>`

## 事实来源

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
