# Stage 5B ReflectionLite Windows Reference Design

Date: 2026-04-06 14:38:08 +08:00
Status: ready-for-plan

## 设计目标

在现有 Stage 4 `Windows x64` reference spine 与 Stage 5A generic foundation 之上，先打通第一条最小 reflection / metadata 查询路径：

- closed generic `Type`
- `Field`
- `Property`
- `Method`
- `Parameter`
- `GetGenericTypeDefinition`
- 稳定字符串输出

目标不是第一轮就覆盖完整 smoke `ReflectionLite`，而是先让 Stage 5B 有一个可验证、可调试、可继续扩展的 reflection foundation。

## 设计边界

### 纳入

- 一个新的最小 reflection query proof 输入
- `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 路径上的最小 reflection metadata 支持
- native runtime / bootstrap / support 中承接最小 query-only reflection 路径的 glue
- 与 Stage 4 / Stage 5A 并存的回归验证

### 不纳入

- 完整 smoke `ReflectionLite` 的全部 surface
- `EventInfo`
- `MakeGenericMethod`
- `MethodInfo.Invoke`
- `FieldInfo.GetValue` / `SetValue`
- `PropertyInfo.GetValue` / `SetValue`
- `PInvokeLite`
- `macOS` parity
- 任何把样例判断写进 `src/**` 的做法

## 关键设计结论

1. Stage 5B 第一轮 proof 输入必须是 query-only reflection sample，而不是完整 smoke `ReflectionLite`。

推荐形状：

```csharp
Type closedType = typeof(ReflectionFixture<int>);
FieldInfo field = closedType.GetField(nameof(ReflectionFixture<int>.BackingField))!;
PropertyInfo property = closedType.GetProperty(nameof(ReflectionFixture<int>.Value))!;
MethodInfo method = closedType.GetMethod(nameof(ReflectionFixture<int>.Echo))!;
ParameterInfo parameter = method.GetParameters()[0];

Console.WriteLine(
    $"reflection-ok|{closedType.Name}|{field.Name}|{property.Name}|{method.Name}|{parameter.Name}|{closedType.GetGenericTypeDefinition().Name}");
return 0;
```

这条路径同时覆盖：

- closed generic type metadata
- member lookup by name
- parameter metadata query
- generic type definition query

但又不会额外引入 event、invoke、generic method instantiation。

2. 先补 metadata query，再决定是否继续扩 reflection invoke。

原因：

- 当前最早的真实 blocker大概率在 metadata materialization 与 runtime lookup，而不是 reflection invoke。
- 如果第一轮就把 invoke 带进来，会把 binder / call adapter / boxing 等后续语义一起混入。
- 只有 query path 先稳定，后续才有资格讨论 `MethodInfo.Invoke` 或 field/property value access。

3. Stage 5B 必须显式检查 closed generic type 与成员的 metadata identity。

不能接受的状态是：

- managed closure 能保留类型和成员
- 但 metadata registration / runtime lookup 里找不到对应 entry
- 或 `GetGenericTypeDefinition` 回不到稳定的 generic type identity

如果 query path 触达了某个 closed generic type 或其成员，它们在 bundle 与 metadata/runtime lookup 中也必须能被一致定位。

4. `src/**` 继续保持 sample-agnostic。

允许变化的是：

- metadata registration / lookup 的最小扩展
- reflection query glue
- generic type definition query support

不允许的是：

- 判断 `ReflectionLite`
- 判断测试工程名
- 针对某个 proof sample 写死成员 token 或字符串分支

5. Stage 4 `HelloWorldObject` 与 Stage 5A `GenericEcho` gates 继续作为 Stage 5B 回归基线。

任何 Stage 5B 改动都不能以破坏现有真实 proof 为代价。

## 设计结果

Stage 5B 作为单个 `plan` 子任务继续推进。下一步进入 `plan-v1-01.md`，按 TDD 先钉最小 reflection query RED，再逐层补 metadata query、runtime glue 与 proof host。
