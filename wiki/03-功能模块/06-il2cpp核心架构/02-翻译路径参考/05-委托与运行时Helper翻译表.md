# 委托与运行时Helper翻译表

## 委托生命周期

| 阶段 | 操作 | 说明 |
|------|------|------|
| 创建 | `create_delegate(type, target, method_ptr)` | 通过桥接函数创建委托对象 |
| 调用 | `delegate_invoke(delegate, args...)` | 间接调用委托目标 |
| 函数指针 | `ldftn` → `create_delegate` | 静态方法委托 |
| 虚函数指针 | `ldvirtftn` → `create_delegate` | 虚方法委托 |

## 16 类 Runtime Helper

| 类别 | 触发条件 | 实现文件 |
|------|---------|---------|
| Assertion | Debug.Assert / Contract 调用 | `AssertionHelpers.gen.h` |
| TestFramework Assertion | Chaos.TestFramework.Assert::Equal 调用（int / string 比较） | `NativeAotLoweringPlanner.ExternalRuntimeHelpers.Assertions.cs` → Scriban 内联发射 |
| Async | `async`/`await` 状态机 | `AsyncHelpers.gen.h` |
| String | 字符串创建、连接、比较 | `StringHelpers.gen.h` |
| Exception | `throw` / 运行时检查失败 | `ExceptionHelpers.gen.h` |
| Formatting | `String.Format` / 复合格式化 | `FormattingHelpers.gen.h` |
| InterpolatedString | 内插字符串 `$"..."` | `InterpolatedStringHelpers.gen.h` |
| Platform | `Environment` / `RuntimeInformation` | `PlatformHelpers.gen.h` |
| DelegateInterop | 委托与原生函数指针互操作 | `DelegateHelpers.gen.h` |
| Marshaling | P/Invoke 数据封送 | `MarshalingHelpers.gen.h` |
| Interlocked | `Interlocked.*` 原子操作 | `InterlockedHelpers.gen.h` |
| Span | `Span<T>` / `ReadOnlySpan<T>` 操作 | `SpanHelpers.gen.h` |
| Collection | `List<T>` / `Dictionary<,>` 辅助 | `CollectionHelpers.gen.h` |
| Object-Thread | `Monitor` / `Thread` / 同步 | `ThreadHelpers.gen.h` |
| Reflection | `Type` / `MethodInfo` 查询 | `ReflectionHelpers.gen.h` |
| InvocationAbi | `MethodBase.Invoke` | `InvocationHelpers.gen.h` |
| CustomAttribute | 自定义属性读取 | `CustomAttributeHelpers.gen.h` |

## TestFramework Assertion 翻译路径

| 属性 | 值 |
|------|-----|
| **IL 特性** | `call Chaos.TestFramework.Assert::Equal(T, T, string)` |
| **翻译策略** | Inline intrinsic — 跳过 interop 桩，直接发射 C++ 比较代码 |
| **Planner 文件** | `NativeAotLoweringPlanner.ExternalRuntimeHelpers.Assertions.cs` |
| **Emitter** | Scriban 模板 `NativeAot.SimpleExternalRuntimeHelper.cpp.scriban` + `NativeAot.RuntimeHelperShapeDispatch.h.scriban` |
| **Runtime 支持** | `__chaos_assert_failures` 全局计数器（在 translation unit 中定义） |
| **支持的类型** | `System.Int32`（直接 == 比较）、`System.String`（length + strcmp）、以及其他 int/float 原生类型 |
| **行为** | 两个值相等 → return；不等 → 置位 ChaosAssertState.ExitCode → 递增 `__chaos_assert_failures` |
| **约束** | Assert.Equal 只支持值类型和 string；对象相等性（Object.Equals）由 `chaos_object_equals` 运行时函数处理 |
