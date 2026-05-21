# 第 4 段：Smoke 输入项目与平台矩阵映射

## 固定的 5 个 smoke 输入项目

`Roadmap 0` 保持 5 个 smoke 输入项目不变，不再扩项目数量：

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`

这 5 个项目都必须是真项目、真 `.csproj`、真 `dotnet build`。

## 职责映射

### HelloWorld

`HelloWorld` 收缩为最小启动样例。
它保留：

- 最小 entry path
- 最薄的一层 registration 痕迹

它不再承担完整 registration 流。

### GenericEcho

`GenericEcho` 更聚焦于：

- 最直观的泛型调用
- 泛型实例形态

它不承担太多高阶反射/元数据解释任务。

### ReflectionLite

`ReflectionLite` 是高阶反射 contract 主体。
它负责：

- `Type`
- `Field`
- `Method`
- `Property`
- `Event`
- `Parameter`

以及一部分：

- `GenericContext`
- 高阶元数据查询路径

### PInvokeLite

`PInvokeLite` 聚焦原生互操作主线：

- `DllImport`
- native export
- 符号解析
- 基础 marshalling

不再承担过多平台能力验证职责。

### HostEmbeddingLite

`HostEmbeddingLite` 是最重的 smoke 项目，负责：

- host lifecycle
- thread attach / detach
- managed entry
- delegate / callback
- 文件系统
- 时间 / 时区
- 动态库装载
- registration 流
- metadata bootstrap 状态机

## Platform Matrix 对应关系

### Windows host

Windows 开发机承担：

- Windows desktop 主线
- Android 真启动 smoke
- Linux cross-compile / packaging

### macOS host

macOS 开发机承担：

- macOS desktop 主线
- iOS compile / link / packaging
- Linux cross-compile / packaging

### 双 reference desktop

`Windows x64 + macOS` 共同组成双 reference desktop。

- Windows：高阶 ABI/bridge contract 主线基准集
- macOS：接近同范围的高阶 contract 子集

其中：

- 仅对平台能力相关 `icall` 留白
- 这些留白能力在 `Roadmap 0` 中仍需做到 query/basic-path 级验证

## 结论

这 5 个项目不靠数量扩张来吸纳更多验证，而是靠“项目内部子场景扩容”来承载更重的 contract 范围。
