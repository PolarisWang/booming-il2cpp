# 运行时Helper生态

## 16 类 Helper 总览

| 类别 | 说明 | 主要文件 |
|------|------|---------|
| Assertion | 断言辅助 | `AssertionHelpers` |
| Async | async/await 支持 | `AsyncHelpers` |
| String | 字符串操作 | `StringHelpers` |
| Exception | 异常抛出 | `ExceptionHelpers` |
| Formatting | 格式化 | `FormattingHelpers` |
| InterpolatedString | 内插字符串 | `InterpolatedStringHelpers` |
| Platform | 平台相关 | `PlatformHelpers` |
| DelegateInterop | 委托互操作 | `DelegateHelpers` |
| Marshaling | 数据封送 | `MarshalingHelpers` |
| Interlocked | 原子操作 | `InterlockedHelpers` |
| Span | Span 操作 | `SpanHelpers` |
| Collection | 集合辅助 | `CollectionHelpers` |
| Object-Thread | 线程/对象 | `ThreadHelpers` |
| Reflection | 反射 | `ReflectionHelpers` |
| InvocationAbi | 调用 ABI | `InvocationHelpers` |
| CustomAttribute | 自定义属性 | `CustomAttributeHelpers` |

## 调用模式

- Helper 通过 `codegen_bridge.h` 注册到运行时
- 编译期通过 `LoweringPlan` 标记需要哪些 helper
- Helper 实现在 C++ runtime 模块中，按模块分层组织
