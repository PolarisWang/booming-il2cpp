# 类 IL2CPP C# 到 C++ 系统落地 Roadmap 与执行计划

> **For agentic workers:** REQUIRED: Use booming-code:subagent-driven-development (if subagents available) or booming-code:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**目标：** 基于现有高层设计和 Unity IL2CPP 仓库分析，把“Phase 1 / 2 / 3”的产品级规划细化成可以真正开工的工程路线图，补齐关键遗漏项，并为每条路线定义验证闭环与退出标准。

**架构：** 保留既有设计里的双平面结构（翻译流水线 + 运行时平台），但把当前过粗的阶段规划拆成 7 条工程 Roadmap。执行策略采用“架构上从第一天跨平台，落地上先打穿单参考平台垂直切片，再扩展平台矩阵和项目覆盖”。验证矩阵从一开始就应显式纳入 `Android arm64` 与 `iOS arm64`，只是前期允许先做编译/打包/smoke 级验证，后期再升级到模拟器/真机上的语义与性能验证。

**技术栈：** .NET 程序集与元数据加载、IL 解码与 IR、AOT 闭世界分析、基于 `Scriban` 的 C++ 代码生成、运行时对象模型、GC 后端抽象、线程/异常/反射/PInvoke、原生构建系统、Golden Project 与跨平台验证。

**骨架校准：** 参考 Unity IL2CPP 整体编译系统架构，正式采用 `Contracts / Driver / Loader / Semantic World / Pipeline / Analyzer / CodeGen / Metadata Writer / Runtime / Build / Linker` 这组骨架分层。它不改变既有 7 条 Roadmap 的顺序，但要求 `Roadmap 1` 与 `Roadmap 3` 在正式铺开实现前，先落共享的 request/context/results/collector/phase skeleton。

---

## 总体判断

现有 `design-2026-03-30-00-il2cpp-like-cs-to-cpp.md` 已经足够作为顶层架构蓝图，但还不够直接指导从 0 到 1 开发。尤其是当前仓库没有任何翻译器、运行时或构建实现代码，只有设计与流程文档，因此 Phase 1 不能按“并行实现所有大模块”理解，而应该先把最关键的几个契约固定下来，再用一条可运行的垂直切片不断带动补齐缺失语义。

结合 Unity IL2CPP 仓库分析，最值得借鉴的不是“前端如何把 IL 变成 C++”这一点，而是下列事实：

- 运行时启动链必须是一个正式协议：`init -> codegen registration -> metadata cache -> runtime world`
- generated C++ 与 runtime 之间必须有独立 helper/bridge 层，而不是直接互相理解内部结构
- 元数据、反射包装、类初始化、对象分配、线程附着、P/Invoke、icall、ClassLibrary PAL 都是运行时一级系统，不是边角料
- Linker/descriptor/profile、构建集成 DTO、runtime profile 分化，都会在“真实项目成功率”阶段成为刚需

因此，建议把当前产品级三阶段，重构为下面 7 条工程 Roadmap：

1. Roadmap 0：契约冻结与工程骨架
2. Roadmap 1：程序集/元数据加载与 Typed IL IR
3. Roadmap 2：AOT 闭包、保留规则与 Manifest
4. Roadmap 3：Runtime Core v1
5. Roadmap 4：C++ 发射器、Codegen Bridge 与单平台端到端打通
6. Roadmap 5：MVP 范围补齐与 Golden Projects
7. Roadmap 6：跨平台、打包、宿主嵌入与 Phase 2/3 演进

其中：

- 原设计中的“Phase 1”大致对应 Roadmap 0-5
- 原设计中的“Phase 2”大致对应 Roadmap 6 的前半段
- 原设计中的“Phase 3”大致对应 Roadmap 6 的后半段

## 当前设计里还缺少但应显式纳入的开发内容

### 缺口 A：运行时启动与注册协议

当前设计已经提到 `Generated C++ -> Runtime Core` 内部 ABI，但还没有把“启动顺序”和“注册产物”提升为正式交付物。落地时必须显式定义：

- code registration 表
- metadata registration 表
- runtime bootstrap 顺序
- 运行时初始化阶段划分
- generated 模块初始化协议

否则 Runtime Core 和 C++ 后端会相互等待，无法并行推进。

### 缺口 B：Codegen Helper / Bridge 层

设计里提到了内部 ABI，但还缺一层比 ABI 更贴近生成代码的 helper API。建议新增正式子系统：

- `codegen bridge`
- 字段访问 helper
- metadata usage helper
- boxing/unboxing helper
- 委托调用 helper
- 异常边界 helper

这层应当稳定早于 runtime 内部实现细节。

### 缺口 C：Metadata Binary 与运行时缓存策略

当前设计已经有 `Metadata Graph` 与运行时反射模型，但没有明确：

- 哪些元数据以静态二进制形式落盘
- 哪些在 runtime 启动时构造成缓存
- 哪些需要 GC-safe late init

Unity IL2CPP 在这方面做得非常明确，这部分必须进入 Phase 1 的正式工作项。

### 缺口 D：Linker / Descriptor / Preserve 体系

当前设计提到了反射保留和序列化保留，但还没有把“保留规则来源”单独建模。建议补充：

- descriptor 文件格式
- 用户手工 preserve 配置
- 框架内建保留 profile
- 诊断报告：某成员为何被保留/为何被裁剪

不把这层单独做出来，Roadmap 2 的 AOT 闭世界很容易停留在实验室级别。

### 缺口 E：ClassLibrary PAL 与 BCL 原生支撑面

当前设计提到平台抽象层，但还没有单独强调“BCL 依赖的 native support”。落地时必须明确：

- 文件/目录/路径
- 时间/时区
- 环境变量/进程
- socket/networking
- 编码与区域信息
- 动态库装载

否则 Phase 1 虽然能翻译简单程序，但跑不动真实 CLI、服务或宿主嵌入场景。

### 缺口 F：宿主嵌入生命周期

当前设计把嵌入列为 Phase 1 范围，但还未拆成正式接口面。需要提前建模：

- runtime init/shutdown
- thread attach/detach
- managed entry invoke
- exception boundary
- native callback -> managed callback

这是一个独立交付面，不应当等主线“差不多完成”后再补。

### 缺口 G：差分验证与兼容性诊断

当前设计已经有 golden project 与语义验证，但仍建议再显式加入：

- 与 CoreCLR/Mono 的差分测试 harness
- unsupported feature 报告
- compatibility report
- manifest diff / IR diff / generated C++ diff

没有这组工具，Phase 1 到 Phase 2 的迭代成本会迅速上升。

### 缺口 H：Runtime Profile 策略

Unity IL2CPP 与 `libil2cpptiny` 的分化说明“一个 runtime 覆盖所有场景”并不是长期最优路径。虽然这不该阻塞 Phase 1，但架构上应从现在就预留：

- full runtime profile
- minimal runtime profile
- profile capability flags

这样后期不会被完整 profile 的复杂度反向锁死。

### 缺口 I：Android / iOS 目标的验证与打包约束

当前路线图虽然强调跨平台，但还没有把移动端验证拆成正式工作项。对 Android 和 iOS，至少要显式规划：

- Android NDK / iOS clang-xcode 工具链接入
- `arm64` 目标 ABI、调用约定和异常边界验证
- Android 包体布局、native library 装载与日志/调试接入
- iOS 静态链接、bundle 组织、模拟器与真机差异
- 移动端宿主生命周期、线程接入与回调边界
- 模拟器/真机 smoke 基建与可自动化执行的子集测试

如果不把这层单独列出来，Roadmap 很容易在桌面平台通过后才发现移动端集成成本远高于预期。

## Android / iOS 验证分层策略

建议把移动端验证拆成 5 层，而不是只用“支持/不支持”二元判断：

1. **配置与产物层**
   - 能生成 Android/iOS 对应的构建描述、打包描述和 registration 产物
2. **交叉编译层**
   - generated C++、runtime 和桥接代码能在 Android/iOS 工具链下完成编译与链接
3. **启动 smoke 层**
   - 在 Android emulator/device、iOS simulator/device 上完成启动、初始化、最小入口调用和退出
4. **语义子集层**
   - 在移动端执行选定的对象、异常、反射、P/Invoke、线程附着等子集测试
5. **非功能层**
   - 度量启动时间、内存、包体体积、崩溃率和调试可观测性

Roadmap 中应遵循如下节奏：

- Roadmap 0：纳入验证矩阵与样例工程设计
- Roadmap 3：运行时与 PAL 先能以 self-test 形式在移动端编译和启动
- Roadmap 4：端到端流水线增加 Android/iOS 编译与打包 smoke
- Roadmap 5：Golden Project 增加移动端友好样例
- Roadmap 6：把 Android/iOS 提升为正式平台 Gate，而不再只是附加实验目标

## Roadmap 0：契约冻结与工程骨架

**目标：** 在任何核心模块实现之前，先把系统的“交付边界”固化为可读可测的协议和骨架。

**为什么先做：**

- 当前仓库没有代码，先写模块很容易各自发散
- 后续 Roadmap 1-4 可以并行，但前提是 contract 已稳定

**关键工作：**

- [ ] 定义仓库布局：`frontend/`、`analysis/`、`runtime/`、`codegen/`、`build/`、`tests/`
- [ ] 定义产物格式：`metadata-graph.json`、`typed-il-ir.json`、`aot-manifest.json`、`code-registration.json`、`metadata-registration.json`
- [ ] 定义内部 ABI v0：对象访问、分配、类型查询、类初始化、异常、线程、GC root、P/Invoke、icall
- [ ] 定义 codegen bridge API v0：生成代码允许调用哪些 helper
- [ ] 定义 descriptor / preserve 文件格式 v0
- [ ] 定义 reference platform：建议先以 `Windows x64 + MSVC/clang-cl` 或 `Linux x64 + clang` 二选一，优先打通单平台
- [ ] 定义验证矩阵：`reference desktop platform + Android arm64 + iOS arm64`
- [ ] 定义移动端 smoke harness 形态：`Android test app/native activity` 与 `iOS test host/simulator app`
- [ ] 建立 smoke 项目集：`HelloWorld`、`GenericEcho`、`ReflectionLite`、`PInvokeLite`、`HostEmbeddingLite`

**验证：**

- [ ] 所有 schema 有样例文件与 round-trip 测试
- [ ] ABI 和 bridge API 有最小头文件/IDL 以及生成样例
- [ ] smoke 项目可以从“源码 -> IL 程序集”稳定产出基线输入
- [ ] Android/iOS 的构建描述与打包描述至少有样例和快照测试
- [ ] 每个 schema 的 diff 结果可读，适合后续做 golden snapshot

**退出标准：**

- 所有后续模块都能以这里定义的 schema/ABI 为接口开始开发
- 不再需要通过口头约定解释“manifest 里有什么”“generated code 能调什么”

**执行路径：**

1. 先定义文件产物和命名，不急着决定全部字段细节
2. 再定义内部 ABI 和 bridge helper 的最小闭集
3. 最后建立 smoke 工程和 schema snapshot 测试

## Roadmap 1：程序集/元数据加载与 Typed IL IR

**目标：** 把 IL 输入稳定转换成可分析的世界模型和方法级显式 IR。

**关键工作：**

- [ ] 以 `System.Reflection.Metadata + PEReader` 作为主实现读取层，直接面向 PE / metadata tables / handles 建立自有世界模型
- [ ] 将 `Mono.Cecil` 固定为测试/对照基线，不作为前端主实现对象模型
- [ ] 程序集加载、依赖解析、符号与资源挂接
- [ ] 构建 `Metadata Graph`：程序集、模块、类型、方法、字段、属性、事件、泛型参数、约束、特性
- [ ] 方法体 IL 解码与异常区域解码
- [ ] 栈式 IL 规范化，产出显式局部变量/临时变量
- [ ] 建立 CFG、异常边界、调用点、装箱/拆箱、数组操作节点
- [ ] 为后续分析预留 token/source map/debug map

**验证：**

- [ ] 对固定程序集输出 metadata graph snapshot
- [ ] 对 tricky IL 样例输出 typed IL IR snapshot
- [ ] 覆盖分支、switch、异常、泛型方法、装箱拆箱、数组访问
- [ ] 与 `Mono.Cecil` 结果做结构对比测试，并把差异收敛为前端基线回归

**退出标准：**

- 对 smoke 项目中的所有方法都能稳定产出 typed IL IR
- IR 不再依赖“解释 IL 栈状态”才能理解后续语义

**执行路径：**

1. 从无泛型、无异常的简单方法切入
2. 再补异常区域和复杂控制流
3. 最后处理泛型签名、token 解析和 debug/source 信息

## Roadmap 2：AOT 闭包、保留规则与 Manifest

**目标：** 把“哪些东西必须生成、必须保留、必须出现在 runtime 元数据中”做成显式可持久化产物。

**关键工作：**

- [ ] root discovery：入口点、导出 API、反射保留、序列化保留、宿主嵌入回调
- [ ] call graph 构建
- [ ] virtual/interface closure
- [ ] generic instantiation discovery
- [ ] descriptor/profile 加载与合并
- [ ] 产出 `AOT manifest`
- [ ] 产出 unsupported-feature/uncertain-closure diagnostics

**验证：**

- [ ] manifest snapshot 测试
- [ ] 对 direct call / virtual / interface / delegate / generic 样例做闭包验证
- [ ] 对 reflection preserve 和 descriptor preserve 做保留测试
- [ ] 对无法静态闭包的情况给出稳定诊断，而不是静默漏生成

**退出标准：**

- 任何一个待编译程序集都能得到一份可读、可 diff、可复现的 manifest
- manifest 足以驱动后续 codegen 和 runtime metadata 装配

**执行路径：**

1. 先支持直接调用闭包和静态根
2. 再加虚调用、接口调用和泛型实例发现
3. 再补反射/序列化保留、descriptor 和诊断体系

## Roadmap 3：Runtime Core v1

**目标：** 实现一套最小但真实承载托管语义的 runtime，而不是“为了跑通样例而临时拼的 native helper 集”。

**关键工作：**

- [ ] runtime bootstrap：`init -> registration -> metadata cache -> class world`
- [ ] 对象头、字符串、数组、值类型装箱模型
- [ ] 类型系统：`TypeInfo`、`MethodInfo`、`FieldInfo`、`Assembly/Image` 运行时对象
- [ ] class init / cctor 协议
- [ ] 对象分配与 finalizer/GC descriptor 接口
- [ ] GC 抽象层：root、handle、write barrier、safepoint
- [ ] 异常模型与 managed/native 边界
- [ ] thread attach/detach、TLS、主线程与宿主线程接入
- [ ] 反射基础缓存层
- [ ] icall registry
- [ ] P/Invoke 解析与 native library loader
- [ ] ClassLibrary PAL v1：文件、时间、环境、路径、基础网络

**建议策略：**

- Phase 1 不要把“自研高阶精确 GC”作为阻塞项
- 应优先实现稳定的 GC 抽象与运行时访问协议，底层可先选更容易落地的 collector backend
- 只要 generated code 从第一天就遵守 root/write-barrier/safepoint 契约，未来替换 GC 后端仍然可行

**验证：**

- [ ] 运行时语义测试：对象、数组、字符串、装箱、异常、反射、泛型基础、P/Invoke、线程 attach/detach
- [ ] 嵌入测试：宿主初始化、附着线程、调用 managed 入口、关闭 runtime
- [ ] 并发测试：cctor、线程附着、异常传播、safepoint 基本行为
- [ ] 长时间 smoke：对象分配和线程创建销毁不出现明显资源泄漏
- [ ] runtime self-test 可以交叉编译到 Android/iOS，并在 emulator/simulator 或受控设备上完成最小启动

**退出标准：**

- 不依赖最终 C++ 发射器，也能以手写/generated stub 驱动 runtime 跑完语义测试
- Runtime Core 的 API 和 helper 契约开始趋于稳定

**执行路径：**

1. 先打通 bootstrap、类型世界、对象分配和 class init
2. 再补异常、线程、反射缓存
3. 最后补 icall、P/Invoke 与 ClassLibrary PAL

## Roadmap 4：C++ 发射器、Codegen Bridge 与单平台端到端打通

**目标：** 把 Runtime-Contract IR 稳定发射成可编译 C++，并在一个参考平台上端到端跑通真实样例。

**关键工作：**

- [ ] Runtime-Contract IR 到 C++ 的 lowering
- [ ] 建立基于 `Scriban` 的 codegen 模板层：模板目录、模板输入 DTO、渲染入口与模板版本规则
- [ ] 生成类型、方法、静态字段、字符串字面量、registration table
- [ ] 生成 metadata registration / code registration
- [ ] 对接 codegen bridge helper
- [ ] 生成原生构建描述（建议 CMake + Ninja 或等价构建层）
- [ ] 生成 exe / static lib / shared lib / embeddable bundle
- [ ] 产出 source map / symbol map / manifest map

**验证：**

- [ ] `HelloWorld` 可以完整经历：程序集 -> IR -> manifest -> generated C++ -> native build -> 运行
- [ ] `GenericEcho` 与 `ReflectionLite` 可端到端跑通
- [ ] `PInvokeLite` 和 `HostEmbeddingLite` 至少在参考平台可用
- [ ] `HelloWorld` 或 `HostEmbeddingLite` 至少能在 Android/iOS 完成交叉编译、链接和启动 smoke
- [ ] `Scriban` 模板与渲染结果具备 snapshot/diff 测试，能区分 lowering 变更与模板变更
- [ ] 生成 C++ 是稳定可 diff 的，而不是每次随机漂移

**退出标准：**

- 参考平台上存在一条可重复执行的一键端到端流水线
- 可以生成至少一种嵌入产物（库或 bundle）

**执行路径：**

1. 先建立 `Runtime-Contract IR -> DTO -> Scriban 模板 -> C++` 的最小闭环，只支持静态方法和简单对象
2. 再接入 registration、metadata usage 和 bridge helper
3. 最后把构建系统和嵌入产物统一接入

## Roadmap 5：MVP 范围补齐与 Golden Projects

**目标：** 把“能跑样例”升级为“能稳定跑一批真实项目”，也就是把原设计中的 Phase 1 真正做实。

**关键工作：**

- [ ] 维护 Golden Project Suite：类库、CLI、后台任务、服务端子集、宿主嵌入
- [ ] 增加移动端友好 Golden 子集：以 `library + host embedding/mobile host` 为主，而不是把桌面项目形态生搬到手机上
- [ ] 建立 BCL 支撑优先级矩阵：哪些 API Phase 1 必须可用，哪些给出明确 unsupported
- [ ] 基于 golden failures 反推缺失特性
- [ ] 补齐泛型基础能力、反射基础能力、常见序列化保留路径
- [ ] 输出 compatibility report
- [ ] 输出 unsupported feature report

**验证：**

- [ ] 每个 golden project 至少验证翻译、编译、运行、行为校验四层
- [ ] 至少一组服务类项目在 reference platform 上稳定通过
- [ ] 至少一个宿主嵌入场景通过
- [ ] Android 与 iOS 各至少有一个移动端友好样例通过“编译/打包/启动 smoke”，并对一个语义子集完成行为验证
- [ ] 回归测试中能追溯某次失败是 IR、manifest、runtime 还是 codegen 引起

**退出标准：**

- 真正满足原设计里的 Phase 1 退出标准
- 团队已拥有一套“新增兼容性 -> 加测试 -> 收敛回归”的工作方式

**执行路径：**

1. 先只挑 3-5 个代表项目，不要一开始铺太大
2. 用 failure-driven 的方式补语义，不要靠 feature checklist 膨胀
3. 一旦 golden suite 稳定，再扩到更广项目形态

## Roadmap 6：跨平台、打包、宿主嵌入与 Phase 2/3 演进

**目标：** 从“参考平台上的 MVP”推进到“真正可交付的平台级系统”。

**关键工作：**

- [ ] 扩展 ABI 适配层到 Windows/Linux/macOS/arm64 子矩阵
- [ ] 单独补齐 Android NDK 与 iOS toolchain 的构建、打包和符号化链路
- [ ] 增加 compiler-family 兼容
- [ ] 完善宿主嵌入 SDK、文档和稳定 API
- [ ] 做增量 manifest、缓存与增量构建
- [ ] 做更强的泛型共享/特化
- [ ] 做元数据和二进制体积优化
- [ ] 做调试/符号/诊断改进
- [ ] 规划 full/minimal runtime profile 分化
- [ ] 逐步补更深动态特性与更广框架适配

**验证：**

- [ ] 跨平台子矩阵 CI
- [ ] ABI 一致性测试
- [ ] 真实宿主嵌入样例在多平台通过
- [ ] Android emulator/device 与 iOS simulator/device 均具备自动 smoke 能力，且至少一组语义子集测试可稳定运行
- [ ] 启动时间、吞吐、二进制体积基准
- [ ] 缓存命中和增量编译收益可量化

**退出标准：**

- 满足原设计中 Phase 2/3 的目标，不再只是“能跑”，而是“可诊断、可维护、可扩展”

**执行路径：**

1. 先稳住内部 ABI，再扩平台适配
2. 再做缓存、优化和调试能力
3. 最后再扩大动态特性和框架覆盖面

## HelloWorld First Vertical Slice

`HelloWorld` 现已冻结为 Phase 1 第一条正式 vertical slice。它不是旧式“只打印字面量”的微样例，而是一个最小 object-based 样例。

### 固定样例形态

- 类型：`Greeter`
- 字段：`_name`
- 方法：
  - `Greeter..ctor(string)`
  - `Greeter.BuildMessage()`
  - `Program.Main()`
- 可观察结果：
  - stdout: `Hello, World!`
  - exit code: `0`

### 执行原则

- 先跑通完整闭环，再扩功能
- 不让 `ICU`、`libffi`、更重 GC 候选或更广 BCL 覆盖阻塞首条切片
- `CodeGen` 只在 runtime landing zone、manifest、registration 稳定后进入最小发射

### Checkpoint 1 - Loader / Typed IL IR

- [ ] `Greeter..ctor(string)` 可稳定归一化为：
  - base ctor call
  - `_name` field store
- [ ] `Greeter.BuildMessage()` 可稳定归一化为：
  - string literals
  - `_name` field load
  - narrow concat helper call
- [ ] `Program.Main()` 可稳定归一化为：
  - `newobj`
  - local store/load
  - instance method call
  - `Console.WriteLine(string)`
  - `return 0`
- [ ] `HelloWorld` 的 typed IR 可做 snapshot/diff

### Checkpoint 2 - Linker / AOT Manifest

- [ ] retain `Program.Main`
- [ ] retain `Greeter..ctor(string)`
- [ ] retain `Greeter.BuildMessage()`
- [ ] retain `Greeter._name`
- [ ] retain `System.Object..ctor`
- [ ] retain `System.Console.WriteLine(string)`
- [ ] retain chosen narrow string path
- [ ] emit a small, explicit, diffable `AOT manifest`
- [ ] `HelloWorld` happy path 无 uncertain diagnostics

### Checkpoint 3 - Metadata Writer

- [ ] emit `code registration` for:
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
- [ ] emit `metadata registration` for:
  - sample assembly
  - `Greeter`
  - retained methods and field
- [ ] emit minimum metadata blob/table required by bootstrap
- [ ] emit string/literal metadata for:
  - `"Hello, "`
  - `"World"`
  - `"!"`
- [ ] registration products are independently diffable and inspectable

### Checkpoint 4 - Runtime Bootstrap / Minimum Helpers

- [ ] runtime startup entry exists
- [ ] code registration attach exists
- [ ] metadata registration attach exists
- [ ] current-thread attach path exists
- [ ] object allocation path exists
- [ ] constructor execution path exists
- [ ] instance field store/load path exists
- [ ] narrow `callvirt` null-check path exists for the sample pattern
- [ ] string literal materialization exists
- [ ] one narrow concat path exists
- [ ] one narrow `Console.WriteLine(string)` path exists
- [ ] managed `int` exit code returns to native host

### Checkpoint 5 - CodeGen First Emission

- [ ] emit only:
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
- [ ] `Runtime-Contract IR -> emission DTO/model` 路径可用
- [ ] `Scriban` 模板可稳定生成可读 `.cpp`
- [ ] generated code 通过 `CodeGen bridge` / `Runtime ABI` helper 调用 runtime
- [ ] generated output is stable enough for snapshot/diff

### HelloWorld Exit Criteria

- [ ] `HelloWorld` 完整经历：
  - assembly load
  - typed IR
  - `AOT manifest`
  - metadata/code registration
  - generated C++
  - native build
  - runtime bootstrap
  - managed execution
- [ ] stdout 等于 `Hello, World!`
- [ ] exit code 等于 `0`
- [ ] no manual per-method rescue logic is required

### Follow-on Sample Order

- [ ] `GenericEcho`
- [ ] `ExceptionLite`
- [ ] `ReflectionLite`
- [ ] `PInvokeLite`

## 推荐的整体执行顺序

建议严格按下面顺序推进：

1. `Roadmap 0`
2. `Roadmap 1`
3. `Roadmap 2`
4. `Roadmap 3`
5. `Roadmap 4`
6. `Roadmap 5`
7. `Roadmap 6`

但在具体执行上，允许以下并行关系：

- Roadmap 1 与 Roadmap 3 可在 Roadmap 0 完成后并行启动
- Roadmap 2 应在 Roadmap 1 的基础上推进，但可和 Roadmap 3 部分并行
- Roadmap 4 必须建立在 Roadmap 2 + Roadmap 3 的最小闭环已经可用之上
- Roadmap 5 必须以 golden failures 驱动，不建议脱离端到端链路单独长时间推进

## 推荐的团队分轨

如果后续进入实施，建议按 5 条长期工作轨来组织，而不是按技术层随机认领：

1. `Frontend & IR`
2. `AOT Closure & Manifest`
3. `Runtime Core`
4. `Codegen & Build`
5. `Verification & Golden Projects`

每条工作轨都必须有自己的测试资产，但统一受同一套 golden suite 和 phase exit 标准约束。

## 每个阶段的强制验证门槛

### Gate 1：Schema 与 Contract Gate

- schema round-trip
- snapshot diff 可读
- ABI/header 生成与示例调用可通过

### Gate 2：Semantic Gate

- runtime 语义测试通过
- 差分测试结果可解释
- 对 unsupported feature 有稳定报错

### Gate 3：End-to-End Gate

- 至少一个真实项目完整通过
- 至少一个宿主嵌入样例通过
- Android/iOS 至少各完成一次“生成 -> 编译/链接 -> 启动 smoke”闭环
- 生成物、manifest、IR 可追溯

### Gate 4：Platform Gate

- 第二个平台接入后不破坏 reference platform
- Android 与 iOS 不再只是编译目标，而是进入正式平台子矩阵
- ABI、P/Invoke、线程、文件系统、时间行为过子矩阵验证

## 最后的建议

如果现在就准备开工，我建议把最初 6-8 周只投入到三件事：

1. Roadmap 0：把 schema、内部 ABI、bridge helper 和 smoke projects 定下来
2. Roadmap 1：把程序集加载与 typed IL IR 打到可稳定快照
3. Roadmap 3：把 runtime bootstrap、对象模型、class init、线程 attach 与异常基线先做出来

原因很简单：这三块一旦站住，Roadmap 2 和 Roadmap 4 的推进速度会快很多；反过来，如果一开始就急着做“生成 C++”或“支持更多框架”，后面返工概率会很高。
