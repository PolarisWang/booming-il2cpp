# smoke 输入项目

## 目标

`tests/smoke/input/` 固定只保留 5 个输入项目：

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`

固定为 5 个项目，是因为 `Roadmap 0` 需要先冻结最小但完整的 contract 面，而不是继续堆项目数量。项目一旦继续细分，后续验证矩阵会同时放大：

- build 成本会线性增加，平台 smoke 入口会被无意义放大
- 职责边界会变模糊，多个项目会争抢同一类 contract
- 后续 trace / host / codegen 回归时，失败定位会变差

因此这里采用“项目数固定，子场景扩展”的策略：主项目只负责一条清晰主线，新增覆盖时优先往对应项目内部增加子场景，而不是继续新增第 6、第 7 个输入项目。

## 五个项目的职责

- `HelloWorld`：只承载最小 entry path，并保留最薄的一层 registration 痕迹。
- `GenericEcho`：承载最直观的泛型实例化与泛型调用。
- `ReflectionLite`：承载高阶反射 contract 主体，集中覆盖类型、成员与部分 generic context。
- `PInvokeLite`：承载 interop 主线，集中覆盖 `DllImport`、native export、符号解析与基础 marshalling。
- `HostEmbeddingLite`：承载 host lifecycle、thread attach/detach、managed entry、delegate/callback、文件系统、时间/时区、动态库加载、registration 流与 metadata/bootstrap 状态机。

## 为什么用子场景扩展

继续增加项目数并不会提高 contract 清晰度，反而会制造重复入口。这里的扩展原则是：

- entry path 相关扩展，只能进入 `HelloWorld`
- 直观泛型调用扩展，只能进入 `GenericEcho`
- 高阶反射与 generic context 扩展，只能进入 `ReflectionLite`
- interop 扩展，只能进入 `PInvokeLite`
- host / bootstrap / registration / 环境能力扩展，只能进入 `HostEmbeddingLite`

这样可以保证“一个失败属于哪条主线”始终可判定。

## 输出目录约定

`Directory.Build.props` 统一以下设置：

- target framework 固定为 `net8.0`
- `Nullable` 启用
- `LangVersion` 固定为 `12.0`
- 输出目录统一写入 `artifacts/smoke/bin/<ProjectName>/...`
- 中间目录统一写入 `artifacts/smoke/obj/<ProjectName>/...`

这保证 smoke 项目既能单独 `dotnet build`，又不会把 `bin/`、`obj/` 反向污染源目录。
