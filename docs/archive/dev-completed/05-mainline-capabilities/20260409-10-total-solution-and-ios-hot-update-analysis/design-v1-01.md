# 完整 IL2CPP 总方案与 iOS 热更设计分析

## 1. 当前基线判断

从仓库现状看，当前主线还不是“完整 `C# -> C++` 总方案”，而是一个正在收口的 `performance-first core mainline`：

- 父 roadmap 明确把当前范围限制在 `Phase A`，并排除了 `.NET 10` 输入兼容、Android/iOS 完整 runtime subset、完整高阶 `BCL` 兼容等内容
- `engine binding` 仍然被放在尾阶段
- iOS 当前 gate 仍然只是 `compile/link/packaging`
- ~~`ManagedClosureRequest` 仍然以单个 `InputAssemblyPath` 为核心输入~~ **（已更新）**：Driver 已重构为子命令 CLI（`chaos-il2cpp convert/build/publish`），subject.manifest.json 的 `source` 字段已支持三种输入类型（`dotnet-project` / `managed-dlls` / `dotnet-project+dlls`），但 Pipeline 内部仍以单 assembly 为入口
- `LoaderStage` 仍然基于单个 PE/metadata 读取

因此，当前仓库更准确的定位是：

- 已经有 IL2CPP 风格的 managed pipeline 骨架
- 已经有 proof-oriented native runtime/bootstrap 骨架
- 已经开始围绕 generated native 的能力批次做性能导向收口
- **已经有子命令风格 CLI 入口和 manifest 驱动的输入层骨架**
- 但离”完整项目级产品化 IL2CPP”还差一整层系统建设

## 2. 还缺哪些工作

### 2.1 工程输入层还不够产品化

如果目标是“完整 C# projects 翻译成 C++”，现在至少还缺：

- 从单程序集输入升级到完整 project/solution graph 输入
- `ProjectReference`、`PackageReference`、多 target framework、RID、平台条件编译的解析与收口
- 与 `dotnet build` / MSBuild 的正式接线策略
- 编译上下文缓存、增量构建、变更检测、可重放构建输入
- 完整的入口发现、资源布局、配置文件、嵌入资源与卫星程序集策略

没有这层，系统只能稳定处理 proof subjects，不能稳定处理真实业务工程。

### 2.2 语义闭包还远未覆盖完整 CLR 子集

当前主线已经覆盖和正在补齐的一些能力是：

- `dispatch`
- `closed generics/layout`
- `arrays/boxing`
- `delegates/callbacks`
- `exceptions`
- 必要的 `reflection/interop` 补位

但离完整项目级兼容，还需要继续扩展：

- 更完整的 generic instantiation / generic sharing / rgctx 体系
- 接口调用、虚调用、约束调用、协变逆变
- `async/await`、iterator、状态机生成物语义
- 线程、同步原语、`ThreadStatic`、内存模型
- 更完整的 reflection surface
- 更完整的 `DllImport`、reverse P/Invoke、blittable/non-blittable marshaling
- unsafe、pointer、fixed、stackalloc、Span 相关 lowerings
- 属性、特性、序列化、资源、配置、表达式树等高层能力

### 2.3 runtime-core 还只是主线骨架，不是完整运行时

如果目标是长期替代 Unity/自研引擎里的 IL2CPP runtime，原生侧至少还要补齐：

- class init、metadata cache、type system 常驻结构
- 对象分配、数组、字符串、装箱拆箱、类型检查与转换的完整语义
- GC 集成策略与 safepoint / root reporting
- 线程 attach/detach、TLS、异常传播边界
- delegate、virtual/interface dispatch 的稳定 ABI
- icall / internal call 体系
- 反射查询、成员访问、泛型元数据查询
- 调试符号、堆栈恢复、崩溃诊断、profile hooks

当前 native 侧已经开始出现 delegate/exception/reflection 这些 proof 级 helper，但这还不是完整 runtime 产品面。

### 2.4 Linker 的 tree-shaking / whole-program analysis 缺失

当前 `Chaos.IL2CPP.Linker` 只是骨架。对于完整 project graph 输入，如果不做 dead code elimination，生成的 C++ 体积会不可控。至少需要：

- reachability analysis（从 entry point 出发的可达性分析）
- metadata stripping（未使用类型/方法不写入 registration）
- 与 `[Preserve]` / XML link descriptor 的兼容
- 与 reflection 使用分析的联动（保守保留）

### 2.5 调试与诊断支撑层需要尽早引入

调试能力不能留到产品化阶段才做，否则 runtime completeness 和 mobile runtime 阶段的问题定位效率会极低。至少需要从 AOT completeness 阶段就开始引入：

- managed stack trace recovery（从 native crash 恢复 managed 调用栈）
- native crash → managed source mapping（行号、方法名、文件名）
- generated C++ → IL 对应关系的 debug metadata
- 最小 logging / tracing hooks

### 2.6 “性能最优” 还需要正式工程化

要把“性能最好的 IL2CPP”从口号变成工程能力，至少还需要：

- 明确 codegen objective function
- 在 `throughput / allocation / startup / code size / compile time` 之间固定优先级
- 形成 lowering arbitration、specialization、devirtualization、layout-aware codegen 的统一裁决器
- 引入稳定的 native perf baseline、回归阈值与自动比对
- 增加 generated C++ inspection、IR diff、native asm diff、perf trace 证据链
- 评估 profile-guided 或场景驱动的 specialization 策略

否则“高性能”只会停留在局部案例。

### 2.7 引擎衔接需要单独作为产品面来做

你提到“下能衔接引擎层接口”，这不是给 runtime 补几个 API 就够了，而是要做稳定的 engine-facing contract：

- 生命周期：init、shutdown、reload、scene/world attach
- 对象桥接：managed object 与 engine object 的 identity、ownership、lifetime
- 回调桥：engine -> managed、managed -> engine
- 线程模型：主线程约束、job 线程、回调线程切换
- 序列化/反序列化、资源装载、反射型编辑器接口
- 调试与 profile 对接
- 生成绑定代码、版本兼容与 ABI 冻结策略

这块如果做晚了，后面容易把 runtime 和引擎代码缠死。

### 2.8 平台层当前还没有真正打穿移动端

当前文档已经明确：

- Windows/macOS 是 reference desktop
- Android 还偏 startup smoke
- iOS 还只是 `compile/link/packaging`

如果你要面向真实产品，还要继续补：

- iOS arm64 真正的 runtime 子集验证
- simulator/device 差异、静态链接、bundle、符号化
- Android/iOS 的最小宿主应用、启动路径、日志与 crash 收集
- 平台相关 PAL、文件系统、时间、线程、本地库装载、异常边界
- 真机 perf / memory / package size gates

### 2.9 兼容性与回归基础设施还不够

完整产品化一定需要：

- golden projects 矩阵
- 与 CoreCLR / Mono / Unity IL2CPP 的差分执行和差分产物
- unsupported feature report
- manifest diff / semantic diff / generated diff
- 大规模回归样本库

不做这层，功能扩展会越来越不可控。

## 3. 热更体系的现实边界

### 3.1 技术边界

iOS 热更不应该按“发布后 JIT 出新的本地代码”来设计。更现实的路线是：

- 主包仍然是 AOT native binary
- 热更代码以 IL/字节码/中间指令形式装载
- 运行时通过解释器或混合执行机制运行热更代码
- AOT 与热更之间靠预生成 bridge 和 metadata supplement 打通

HybridCLR 的公开文档本质上也是这个方向：把纯 AOT runtime 扩成 `AOT + Interpreter` 混合系统，并通过 supplemental metadata 与 bridge functions 解决泛型和双向调用问题。

### 3.2 发行与审核边界

苹果当前 App Store Review Guidelines 2.5.2 明确限制应用下载、安装或执行会改变应用功能的代码。

这意味着 iOS 热更要分成两个问题：

1. 技术上能不能运行
2. 发行上是否符合你的上架与运营策略

技术上，解释器式热更是可行方向。
发行上，如果是远程下发会改变功能的新代码，就存在审核风险，必须单独制定产品和发行策略。

## 4. 热更可选架构

### 方案 1：纯解释器热更平面

做法：

- 主包全部 AOT
- 热更程序集全部解释执行
- 通过 stable bridge 与主包交互

优点：

- iOS 技术风险最低
- 架构清晰

缺点：

- 热更热点性能差
- bridge、delegate、泛型调用成本高

### 方案 2：AOT 主线 + Interpreter 热更 + Metadata Supplement

做法：

- 主包继续用 IL2CPP AOT
- 热更程序集运行在解释器
- 对 AOT 泛型、反射、delegate、跨边界调用补充 metadata
- 预生成 `AOT <-> hot update` 的桥接函数和回调 wrapper
- 允许“未修改代码继续走 AOT，新增或变更代码走解释器”这类混合执行策略

优点：

- 最接近 HybridCLR 证明过的路线
- iOS 可行性最好
- 不会破坏主包 generated native 的性能上限

缺点：

- 需要专门建设 interpreter、bridge generation、metadata supplement
- ABI 与版本兼容设计要更严格

### 方案 3：原生二进制级 patch/hot swap

做法：

- 对生成后的 C++/binary 做差分或替换

缺点：

- iOS code signing 和分发风险最高
- 工程复杂度远高于收益
- 与“长期稳定热更”目标不匹配

## 5. 推荐架构

推荐采用方案 2，并明确分成两条共享 contract 的主线：

### 5.1 主线 A：IL2CPP AOT Mainline

职责：

- 面向性能最优
- 负责主包、主逻辑、引擎主接口
- 继续推进完整 project ingestion、runtime completeness、engine binding、mobile runtime

### 5.2 主线 B：Hot Update Runtime Mode

职责：

- 负责动态程序集装载
- 负责解释执行或混合执行
- 负责 supplemental metadata、bridge functions、wrapper stubs、hot-update package layout
- 不反向污染 AOT 热路径

### 5.3 两条主线共享的 contract

必须尽早冻结的共享面：

- type/method/field metadata identity
- object model 与 handle model
- delegate/callback ABI
- exception boundary
- reverse P/Invoke / native callback wrapper
- engine API ABI
- packaging manifest
- version compatibility policy

## 6. 建议的工程拆分

如果按产品路线推进，我建议至少拆成下面 6 个 roadmap 主题：

### Roadmap A：完整项目输入与构建图

- project/solution ingestion
- MSBuild graph
- package/asset/resource closure
- 增量构建与构建缓存

### Roadmap B：runtime completeness 与 codegen quality

- generic sharing / rgctx
- async/iterator
- threading / GC / EH / reflection / interop
- lowering arbitration、perf evidence、native regression gates

### Roadmap C：engine binding 正式产品化

- stable engine ABI
- lifecycle、ownership、callback、thread model
- binding generation 与版本治理

### Roadmap D：移动端 runtime 与工具链

- Android/iOS runtime subset 正式化
- simulator/device smoke 与语义子集
- packaging、crash、symbol、perf gates

### Roadmap E：hot update hybrid runtime

- dynamic metadata registration
- IL to interpreter IR
- interpreter
- supplemental metadata
- bridge generation
- hot update package loading/runtime mode switch

### Roadmap F：发行与合规策略

- iOS 审核边界
- 主包与热更包职责划分
- 远程热更的运营/风控/回滚机制
- 版本兼容矩阵

## 7. 我对优先级的建议

如果目标是“先把产品做成，再把热更做稳”，推荐顺序是：

1. 继续完成当前 `Phase A`，不要让主线停在 proof 状态
2. 尽快补 `完整项目输入层`，因为这是从 demo 走向真实工程的硬门槛
3. 提前冻结 `engine ABI` 和 `hot-update shared contract`
4. 再做 `mobile runtime + iOS host/package`
5. 在此基础上进入 `AOT + Interpreter` 热更主线

不推荐的顺序是：

- 在完整项目输入、runtime completeness、engine ABI 都没稳之前，就先冲热更实现

那样最后很可能返工三次：

- metadata 返工
- bridge 返工
- engine callback ABI 返工

## 8. 结论

这次分析的结论很明确：

- 你要的目标不是当前仓库再补几个 capability batch 就能达到的
- 它需要从“主线 proof 收口”升级成“产品化 IL2CPP + 热更双轨架构”
- iOS 热更必须按 `AOT + Interpreter` 方向设计，不能按 JIT 思路设计
- 远程热更在 iOS 上除了技术问题，还必须正视审核/发行边界

## 9. 参考依据

### 仓库内

- `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/design-v1-02.md`
- `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/roadmap-v1-02.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/platform-gates.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/host-verify-matrix.md`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`

### 外部

- Apple App Store Review Guidelines 2.5.2
  - https://developer.apple.com/app-store/review/guidelines/
- HybridCLR Introduction
  - https://www.hybridclr.cn/en/docs/intro
- HybridCLR Bridge Functions
  - https://www.hybridclr.cn/en/docs/basic/methodbridge
- HybridCLR AOT Generics / supplemental metadata
  - https://www.hybridclr.cn/en/docs/8.5.0/basic/aotgeneric
