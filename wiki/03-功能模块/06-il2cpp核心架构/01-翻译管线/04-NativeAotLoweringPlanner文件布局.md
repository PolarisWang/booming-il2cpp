# NativeAotLoweringPlanner 文件布局

`NativeAotLoweringPlanner` 是 CodeGen 层的中央编排器，以 C# partial class 分布在 20+ 文件中。

## Planning 区域

| 文件 | 职责 |
|------|------|
| `NativeAotLoweringPlanner.cs` | 主入口、plan 装配 |
| `NativeAotLoweringPlanner.Computation.cs` | 计算逻辑 lowering |
| `NativeAotLoweringPlanner.ControlFlow.cs` | 控制流 lowering |
| `NativeAotLoweringPlanner.MethodCall.cs` | 方法调用 lowering |
| `NativeAotLoweringPlanner.ArrayOperations.cs` | 数组操作 lowering |
| `NativeAotLoweringPlanner.TypeChecks.cs` | 类型检查（castclass/isinst）lowering |
| `NativeAotLoweringPlanner.Boxing.cs` | 装箱/拆箱 lowering |
| `NativeAotLoweringPlanner.Exceptions.cs` | EH shape lowering |
| `NativeAotLoweringPlanner.StringIds.cs` | StringId 操作 lowering |
| `NativeAotLoweringPlanner.StaticInit.cs` | 静态初始化 lowering |
| `NativeAotLoweringPlanner.Delegate.cs` | 委托操作 lowering |
| `NativeAotLoweringPlanner.GenericMethods.cs` | 泛型方法 lowering |

## Emission 区域

| 文件 | 职责 |
|------|------|
| `NativeAotLoweringPlanner.EmissionOrder.cs` | Emission 驱动管线 |
| `NativeAotLoweringPlanner.Emit_Computation.cs` | 计算操作 C++ 生成 |
| `NativeAotLoweringPlanner.Emit_MethodCall.cs` | 方法调用 C++ 生成 |
| `NativeAotLoweringPlanner.Emit_ArrayOperations.cs` | 数组操作 C++ 生成 |
| `NativeAotLoweringPlanner.Emit_TypeChecks.cs` | 类型检查 C++ 生成 |
| `NativeAotLoweringPlanner.Emit_Boxing.cs` | 装箱操作 C++ 生成 |
| `NativeAotLoweringPlanner.Emit_Exceptions.cs` | EH C++ 生成 |
| `NativeAotLoweringPlanner.Emit_Helpers.cs` | Helper 调用 C++ 生成 |

## RuntimeSupport 区域

| 文件 | 职责 |
|------|------|
| `NativeAotLoweringPlanner.RuntimeSupport.cs` | Runtime support 注册 |
| `NativeAotLoweringPlanner.RuntimeSupport.String.cs` | 字符串 runtime support |
| `NativeAotLoweringPlanner.RuntimeSupport.Exception.cs` | 异常 runtime support |
| `NativeAotLoweringPlanner.RuntimeSupport.Delegate.cs` | 委托 runtime support |
| `NativeAotLoweringPlanner.RuntimeSupport.Async.cs` | async/await support |
| `NativeAotLoweringPlanner.RuntimeSupport.Reflection.cs` | 反射 support |
