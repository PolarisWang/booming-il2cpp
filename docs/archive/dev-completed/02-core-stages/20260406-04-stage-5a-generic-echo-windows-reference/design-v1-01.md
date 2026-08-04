# Stage 5A GenericEcho Windows Reference Design

Date: 2026-04-06 13:05:16 +08:00
Status: ready-for-plan

## 设计目标

在现有 Stage 4 `Windows x64` reference spine 上，先打通第一条最小闭合泛型路径：

- 闭合泛型静态方法调用
- 闭合泛型引用类型实例化
- 最小字符串输出

目标不是第一轮就覆盖完整 smoke `GenericEcho`，而是先让 Stage 5A 有一个可验证、可调试、可继续扩展的 generic foundation。

## 设计边界

### 纳入

- 新的最小闭合泛型 proof 输入
- `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 这条 managed closure 路径上的最小泛型支持
- 闭合泛型 type/method subject 形状与 registration 一致性
- 与 Stage 4 `HelloWorldObject` 基线并存的回归验证

### 不纳入

- 完整 smoke `GenericEcho` 的 tuple / 插值字符串 / 多值返回
- `ReflectionLite`
- `PInvokeLite`
- `macOS` parity
- 任何把样例逻辑特判写进 `src/**` 的做法

## 关键设计结论

1. Stage 5A 的第一个 proof 输入必须是最小闭合泛型 sample，而不是完整 smoke `GenericEcho`。

推荐形状：

```csharp
Console.WriteLine(new EchoBox<string>(GenericEcho.Echo("generic-ok")).Value);
return 0;
```

这条路径同时覆盖：

- `MethodSpecification`
- `TypeSpecification`
- `newobj`
- instance getter 调用
- `Console.WriteLine(string)`

但又不额外引入 tuple / 插值字符串。

2. 先修 managed closure，再判断 native reference proof 接线。

原因：

- 当前最早失败点已经在 loader 泛型引用解析，而不是 native host。
- 如果 managed closure bundle 本身还不闭环，继续改 proof emitter 只会放大下游噪音。
- 等 managed closure 路径稳定后，再判断当前 Stage 4 generated direct-call path 是否需要为 Stage 5A 做最小泛化。

3. 闭合泛型的 subject / registration 必须保持同一套 naming 约束。

Stage 5A 不接受“typed-il 里是闭合泛型 subject，code-registration / metadata-registration 里还是泛型定义 subject”的半闭环状态。
如果闭合泛型方法或类型被 entry path 直接引用，它们在产出的 closure bundle 里也必须能被一致地定位。

4. `src/**` 继续保持 sample-agnostic。

允许变化的是：

- 泛型引用解析
- 命名/registration 一致性
- 可能需要的最小 instantiation materialization

不允许的是：

- 判断 `GenericEcho`
- 判断某个测试工程名
- 为某个 proof sample 硬编码特殊路径

5. Stage 4 `HelloWorldObject` gate 继续是 Stage 5A 的回归基线。

Stage 5A 的任何变更都不能以破坏 Stage 4 真实 proof 为代价。

## 设计结果

Stage 5A 作为单个 `plan` 子任务继续推进。下一步进入 `plan-v1-01.md`，按 TDD 先钉 RED，再补最小泛型支持，并在 managed closure 绿灯后继续评估 native reference proof 的接线收口。
