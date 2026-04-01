# Roadmap 0 详细规划发现记录

## 发现 1：当前仓库没有任何前端、运行时或后端实现代码，Roadmap 0 必须优先冻结边界而不是提前写模块

### 触发场景

为 `Roadmap 0` 制定详细计划时，需要确认当前仓库究竟是“已有半成品代码待整理”，还是“几乎纯文档仓库”。

### 证据

- 仓库顶层只有 `.booming`、`.codex`、`docs`、`.gitignore`、`README.md`
- `README.md` 当前只有仓库标题
- 现有核心内容都位于 `docs/booming/...` 的规划与分析文档中

### 影响范围

如果一开始就按“补模块实现”拆任务，会把不同契约面混在一起，导致后续 Roadmap 1-4 反复返工。

### 当前结论

`Roadmap 0` 应先交付：

- 仓库骨架
- 契约文档
- schema
- ABI/header
- smoke 输入
- 本地验证入口

而不应试图提前实现 Typed IL IR、AOT 分析或 Runtime Core 真实语义。

### 复用价值

后续任何“先做哪个阶段”的讨论，都可以把这条发现作为顺序基线。

## 发现 2：Roadmap 0 的最高优先级是为 `AOT manifest`、`Runtime-Contract IR`、内部 ABI 建立稳定边界

### 触发场景

制定详细任务时，需要决定哪些契约最该先冻结。

### 证据

- 既有 follow-up 讨论明确指出最适合优先细化的切入点是：
  - `AOT manifest`
  - `Runtime-Contract IR`
  - 内部 ABI

### 影响范围

这决定了 `Roadmap 0` 里 schema 与 header 的优先级：必须优先覆盖这些产物，而不是先做外围脚手架。

### 当前结论

`Roadmap 0` 计划中应把以下内容作为核心交付：

- `aot-manifest.schema.json`
- runtime ABI v0 header/doc
- codegen bridge v0 header/doc
- 示例 registration 产物与契约验证

### 复用价值

后续拆 Roadmap 1、Roadmap 2、Roadmap 3 的详细计划时，可以直接沿用这条优先级判断。

## 发现 3：Unity IL2CPP 的真正启发点是“注册 + 元数据缓存 + codegen helper”三件套，而不是某个单独的转换算法

### 触发场景

为 `Roadmap 0` 设计文件和任务结构时，需要从 Unity IL2CPP 分析中提炼最该复用的模式。

### 证据

- `Runtime::Init()` 先接入 `g_CodegenRegistration()`，再做 `MetadataCache::Initialize()`
- generated C++ 通过 `il2cpp_codegen_*` helper 调运行时
- 运行时元数据初始化明确区分可先初始化和依赖 GC 的后初始化

### 影响范围

说明 `Roadmap 0` 不能只写抽象 ABI 文档，还必须把：

- code registration
- metadata registration
- metadata cache 生命周期
- codegen helper surface

作为正式契约一并规划进去。

### 当前结论

Roadmap 0 的详细计划必须同时覆盖 schema、runtime ABI 和 codegen bridge，三者缺一不可。

### 复用价值

这条发现会直接影响后续 Runtime Core 和 C++ 发射器的接口设计。

## 发现 4：Android/iOS 在 Roadmap 0 中最合理的目标是“配置/编译/启动 smoke 骨架”，而不是完整语义对齐

### 触发场景

用户要求把 Android/iOS 纳入路线图验证，详细规划时需要明确它们在 `Roadmap 0` 的层级。

### 证据

- 现有路线图已经把 Android/iOS 纳入平台矩阵，但说明前期允许先做编译/打包/smoke 级验证
- 当前仓库尚无运行时和生成器实现，不具备在移动端做完整行为验证的前提

### 影响范围

`Roadmap 0` 的移动端任务应该落在：

- toolchain 描述
- host harness 形态定义
- compile/link smoke 入口

而不是提前要求 emulator/device 上的深度语义用例。

### 当前结论

详细计划中应为 Android/iOS 单独安排：

- toolchain/preset 文件
- smoke harness 目录
- 最小启动样例
- 平台验证矩阵文档

### 复用价值

可以避免后续把移动端集成成本错误地挤进 Roadmap 4 或 Roadmap 5 的末尾。

## 发现 5：现有 Roadmap 0 详细计划过早收敛到执行结构，需要退回 brainstorm 逐条确认关键取舍

### 触发场景

用户在查看 `plan-discuss-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md` 后，明确要求“把这份计划改为 brainstorm，然后一条条对实现方案和细节”。

### 证据

- 现有 `plan-discuss` 的关键判断里明确写了“这次请求目标明确，不需要再回到 brainstorm 阶段”。
- 现有 Roadmap 0 详细计划已经提前固化了大量执行级决策，例如：
  - 六个顶层目录一次性创建
  - `metadata-graph`、`typed-il-ir`、`aot-manifest` 全部纳入首版 schema pack
  - Android/iOS 直接纳入 startup smoke 骨架
  - 五个 smoke 输入项目一次性到位
- 上述决策在现有记录中仍缺少“用户逐条确认”的过程证据。

### 影响范围

如果直接按现有详细计划实施，后续很可能在以下方面发生返工：

- `Roadmap 0` 与 `Roadmap 1` 的边界
- 契约冻结粒度
- 移动端目标层级
- 本地验证入口范围

### 当前结论

应把现有详细计划降级为候选方案，而不是执行定案；重新以 brainstorm 方式逐条确认：

- 哪些契约必须在 `Roadmap 0` 冻结
- 哪些目录和文件需要现在就落地
- 哪些验证属于必须项，哪些可以延后

### 复用价值

后续任何“计划是否已经足够收敛到可执行”的讨论，都可以复用这条发现来提醒：没有经过逐条确认的执行细节，不应自动视为最终方案。

## 发现 6：在本项目语境里，schema pack、runtime ABI 和 codegen bridge 分别对应三种不同层级的契约

### 触发场景

在 brainstorm 讨论 `Roadmap 0` 的冻结范围时，用户追问“什么是 schema pack, ABI/bridge”，说明这些词虽然在现有计划里频繁出现，但当前还没有在共享语境里被解释清楚。

### 相关问答

问题：
什么是 `schema pack`、`ABI`、`bridge`？

回答要点：

- `schema pack`：一组“文件产物长什么样”的格式契约集合，主要约束 JSON 等静态工件的字段结构、命名和版本。
- `runtime ABI`：generated code 或其它模块与 runtime 交互时可依赖的二进制/调用边界，主要约束函数签名、结构体可见面、调用约定和可扩展方式。
- `codegen bridge`：比 ABI 更贴近生成代码使用方式的一层 helper surface，用来把“生成代码想做的事”翻译成对 runtime 的受控调用，而不是让生成代码直接理解 runtime 内部结构。

### 依据

- 现有总路线图和 Roadmap 0 详细计划都把 contract 拆成：
  - schema 类产物
  - runtime ABI v0
  - codegen bridge v0
- Unity IL2CPP 分析中最值得复用的也是：
  - registration / metadata 产物
  - runtime 访问边界
  - codegen helper surface

### 影响范围

这会直接影响后续所有边界讨论：

- 如果讨论“某个 JSON 文件现在要不要定”，本质是在讨论 `schema pack`
- 如果讨论“生成出来的 C++ 可以直接碰哪些 runtime 能力”，本质是在讨论 `ABI`
- 如果讨论“生成代码应该调用什么 helper，而不是怎么进入 runtime 内部”，本质是在讨论 `bridge`

### 当前结论

后续 brainstorm 可先按下面的简化理解继续推进：

- `schema pack` = 静态产物格式合同
- `ABI` = 二进制交互合同
- `bridge` = 面向生成代码的 helper 合同

### 复用价值

后续我们在逐条判断 `Roadmap 0` 该冻结哪些东西时，可以直接复用这组定义，避免把三类不同契约混在一起讨论。

## 发现 7：允许 Windows/Linux 开发机且拒绝远程 Mac 时，iOS gate 无法同时维持为本地全量硬门槛

### 触发场景

在 brainstorm 中，用户先后确认了：

- `G1`：`verify-roadmap-0` 为全量硬校验
- `H1`：每台开发机都必须本地跑完整 `verify-roadmap-0`
- `I2`：允许 Windows/Linux 作为日常开发环境

随后又选择：

- `J3`：不使用远程 Mac，并将 iOS gate 降成文档/打包描述级

### 依据

- iOS 的真实 compile / link / packaging 闭环通常依赖 macOS / Xcode 环境
- 在不要求所有开发机都是 macOS、又不允许远程 Mac 的前提下，非 macOS 开发机无法满足“本地完成 iOS compile / link / packaging 硬门槛”
- 因此前述约束无法同时成立

### 影响范围

必须至少回退以下其中一项：

- iOS gate 强度
- `verify-roadmap-0` 的全量硬校验范围
- `verify-roadmap-0` 的本地机硬门槛定义
- 开发机操作系统基线

### 当前结论

`J3` 不是一个可直接追加的细节选择，而是一个会触发上层 gate 重定义的分叉点。继续细化前，必须先完成一次约束重对齐。

### 复用价值

后续在任何跨平台 gate 讨论里，只要出现“非 macOS 主开发环境 + 不使用远程 Mac + iOS 本地硬门槛”，都可以直接引用这条发现，避免重复走入同一组互相冲突的前提。

## 发现 8：`verify-roadmap-0` 更适合作为按 host OS 划分的平台子矩阵 gate，而不是所有机器共用同一套全量平台组合

### 触发场景

在 brainstorm 中，用户先前选择了：

- `G1`：`verify-roadmap-0` 为全量硬校验
- `I2`：允许 Windows/Linux 作为开发机
- `J3`：不使用远程 Mac

随后用户给出新的重对齐方案：

- Windows 开发机跑完 `Windows / Android / Linux` 的完整 verify
- macOS 开发机跑完 `iOS / macOS / Linux` 的全量 verify

### 依据

- Android 真启动更容易与 Windows 开发机主线绑定
- iOS gate 更自然地绑定到 macOS 开发机
- 如果继续要求“所有开发机跑同一套全量平台组合”，会再次落回 iOS/macOS 环境冲突

### 影响范围

后续所有 verify 设计都应区分：

- 哪些 target 属于 Windows host 子矩阵
- 哪些 target 属于 macOS host 子矩阵
- 哪些 target 同时出现在两个子矩阵中，例如 Linux

### 当前结论

`verify-roadmap-0` 的“全量”不应再理解为“每台机器跑一样多的平台”，而应理解为“每种 host OS 都有一套定义清楚、必须跑完的全量子矩阵”。

### 复用价值

后续不管是写计划、写验证脚本，还是定义 Roadmap gate，都可以直接沿用这种 host-specific matrix 思路，避免在“跨平台全量”上再次出现表述歧义。

## 发现 9：在当前 Roadmap 0 约束下，Linux 更适合作为 cross-compile 与 packaging gate，而不是 runtime smoke gate

### 触发场景

在 host-specific verify 矩阵已经确定后，还需要决定 Linux target 在 Windows/macOS 开发机上的验证强度。

用户随后选择：

- `L2`：Linux 只做 cross-compile 和 packaging，不要求真实 Linux runtime smoke

### 依据

- 当前 `Roadmap 0` 已经把真实启动 smoke 的重点放在 Android
- iOS/macOS 侧还需要保留 host-specific gate
- 如果再要求 Linux runtime smoke，会显著膨胀开发机基线、脚本复杂度和平台环境依赖

### 影响范围

后续计划与脚本应把 Linux 归入：

- toolchain 配置
- 产物生成
- cross-compile
- packaging/布局验证

而不是：

- runtime startup smoke
- 语义子集验证

### 当前结论

`Roadmap 0` 的 Linux gate 重点是“能产出正确目标平台工件”，不是“已经具备真实运行态验证”。

### 复用价值

后续如果有人质疑为什么 Android 要真启动、Linux 却不真跑，可以直接引用这条发现：这是有意的验证分层，而不是遗漏。

## 发现 10：顶层仓库骨架需要为第三方依赖预留独立目录，避免与自研模块混放

### 触发场景

在确认仓库骨架采用 `M1 全骨架 upfront` 时，用户补充说明：后续还会加入一些第三方库，需要有目录放置。

### 依据

- `Roadmap 0` 已经决定在本阶段真实落盘顶层骨架
- 未来第三方库是明确存在的，而不是可有可无的可能项
- 如果不在顶层布局阶段预留独立目录，后续容易把 vendored 代码混入 `runtime/`、`codegen/` 或 `build/` 下，造成归属和升级策略混乱

### 影响范围

后续顶层布局、README、ownership map、验证脚本和依赖治理都应显式区分：

- 自研源码目录
- 第三方依赖目录
- 生成产物/缓存目录

### 当前结论

`Roadmap 0` 的仓库骨架应包含单独的第三方依赖目录，并采用 `third_party/` 命名；该目录只承载 vendored 源码，不承载工具链、下载缓存和构建产物；第三方源码默认采用手动 vendor snapshot 方式接入；来源/版本元数据采用双层记录：依赖目录内以 `README.md` 就地说明，仓库根或顶层目录以 `third_party/INDEX.md` 保留总索引；目录内 `README.md` 采用扩展版字段集合，除来源/版本/许可证外，还记录引入原因、局部修改说明、升级注意事项、构建接入方式、包含路径和平台限制；根级 `INDEX.md` 采用精简总表，只列名称、版本/commit、许可证、用途和目录路径；工具链描述保留在仓库内的 `build/toolchains/`，实际下载缓存和二进制依赖全部放仓库外；构建产物与 verify 中间输出统一放入仓库内的 `artifacts/`，并由 `artifacts/` 统一收口 CMake build 目录、日志、快照副本和打包产物；权威快照继续保留在源码树中，`artifacts/` 只承载运行时生成的快照副本与 diff 输出；contract 资产采用分层目录布局：`analysis/contracts/schemas/` 放 schema，`analysis/contracts/examples/` 放 example，`tests/contracts/schema/` 放权威 snapshot。

### 复用价值

后续在设计构建系统、许可证说明、依赖升级流程和源码扫描规则时，都可以直接复用这条发现，避免把第三方代码错误地纳入自研边界。

## 发现 11：在本项目语境里，ABI 与 bridge 最好通过“低层 runtime surface + 高层 codegen helper surface”的分层来理解

### 触发场景

在确认 `ABI/bridge` 的目录布局之前，用户追问“ABI/bridge 会有哪些东西？给一些 sample”。

### 回答要点

- `ABI` 更像 runtime 暴露出来的底层稳定边界：
  - 不透明类型
  - 生命周期与线程附着
  - 对象/数组/字符串分配
  - 类初始化
  - 异常抛出
  - GC/root/handle 基础操作
- `bridge` 更像生成代码直接调用的 helper 层：
  - code/metadata registration
  - metadata token 初始化/解析
  - boxing/unboxing helper
  - virtual/interface invoke helper
  - delegate helper
  - icall resolve helper
  - exception boundary helper

### 依据

- `ABI` 关注“runtime 至少要稳定暴露什么”
- `bridge` 关注“generated C++ 应该调什么，而不是直接碰 runtime 内部结构”

### 当前结论

后续设计时可以先按下面的简单分层推进：

- `ABI` = 底层 runtime surface
- `bridge` = 面向 codegen 的高层 helper surface

### 复用价值

这条分层能直接指导：

- 头文件内容怎么拆
- smoke test 怎么写
- 哪些能力应该先放 bridge，而不是直接塞进 ABI

## 发现 12：native contract 更适合作为独立公共包落在 `contracts/native/`，并通过目录级版本化管理

### 触发场景

在逐条确认 `ABI/bridge` 的目录布局时，用户先后确认了：

- `Z2`：native contract 统一收进公共目录
- `AA1`：公共目录内部采用扁平布局
- `AB1`：通过文件名前缀区分 `ABI` 与 `bridge`
- `AC2`：测试目录按 `abi` 与 `bridge` 分组
- `AD1`：根路径采用 `contracts/native/`
- 更正后的 `AE2`：版本放在目录层，文件名不带版本
- `AF3`：header 与人读文档分开放置
- `AG1`：文档也采用目录级版本化

### 当前结论

当前更稳定的 native contract 包结构是：

- `contracts/native/v0/boom_runtime_abi.h`
- `contracts/native/v0/boom_codegen_bridge.h`
- `contracts/native/docs/v0/runtime-abi.md`
- `contracts/native/docs/v0/codegen-bridge.md`
- `contracts/native/examples/v0/abi/`
- `contracts/native/examples/v0/bridge/`
- `tests/contracts/native/abi/`
- `tests/contracts/native/bridge/`

并且：

- `ABI` 与 `bridge` 的 example 不只放一个最小样例，而是各自提供 2-3 个典型示例
- 首批 example 的覆盖面会延伸到 virtual invoke、delegate、icall resolve 等较高阶动作
- 这些较高阶动作不只是 example 演示方向，也要进入 `bridge v0` 的正式 contract surface
- 并且这些高阶 bridge helper 在 `v0` 就冻结到全细节级，而不只是名称或最小签名级
- `bridge v0` 的错误模型采用混合方案：查询/解析类 helper 用返回状态或空指针，托管语义失败走受控异常出口

### 影响范围

后续所有关于 native contract 的实现计划、文档链接和 smoke test 路径，都应以这组布局为基线，而不是再回到：

- `runtime/abi/...`
- `codegen/bridge/...`
- 或者未分版本的扁平 docs 布局

### 复用价值

后续写设计文档、实施计划或真正落盘目录时，可以直接复用这组路径，减少再次讨论“ABI/bridge 到底挂哪”的反复成本。

## 发现 13：在本项目语境里，ABI 更适合理解为“runtime 对外暴露的低层基础操作面”，其直接调用者通常是 bridge、宿主或手写 smoke，而不是大量业务语义代码

### 触发场景

在确认 `ABI` 的错误边界时，用户追问“这里 ABI 具体指的是什么？给些实战用的用法 sample”。

### 回答要点

- `ABI` 不是“所有生成代码都直接调用的一切 API”
- `ABI` 更像 runtime 对外承诺的低层基础操作面，例如：
  - runtime init/shutdown
  - thread attach/detach
  - 对象/数组/字符串分配
  - class init
  - 托管异常抛出入口
  - GC handle / root 基础操作
- `bridge` 则是在 `ABI` 之上再封一层，给 generated code 提供更贴近代码生成习惯的 helper

### 典型调用关系

- generated C++：优先调 `bridge`
- `bridge`：按需再调 `ABI`
- host embedding 或手写 smoke：可以直接调 `ABI`

### 当前结论

讨论 `ABI` 时，应优先从“低层基础操作是否稳定、是否足够少、是否便于 bridge 和 host 依赖”来判断，而不是把所有高层语义都直接塞进 `ABI`。

并且：

- `ABI` 的错误边界采用更底层、更显式的模式
- 基础操作优先返回状态/空值
- 托管异常通过专门入口显式触发，而不是让 ABI 自身大量隐式走异常出口
- 当前讨论里，`ABI v0` 的覆盖范围也被扩大到字段访问、方法调用和反射查询，而不只停留在生命周期、线程与分配等最小基础操作
- 这些扩展能力进入 `ABI v0` 时，优先通过 `TypeInfo/FieldInfo/MethodInfo` 一类不透明句柄和通用操作函数承载，而不是铺开大量直接 API
- `ABI v0` 的句柄集合也采用扩展版，包含 `PropertyInfo`、`EventInfo`、`ParameterInfo`、`GenericContext` 等更高阶元数据句柄
- 并且这些高阶句柄在 `v0` 中按完整语义承诺处理，不只是占位句柄或最小查询入口
- 这些完整语义承诺不只停留在规范层，还要求进入 `Roadmap 0` 的验证资产显式覆盖
- 并且这种覆盖采用真正可运行的 end-to-end 语义验证，而不是只靠静态 smoke 或最小 contract harness

### 复用价值

这条说明可直接复用于：

- 后续 `ABI` 函数表裁剪
- `bridge` 与 `ABI` 的职责分界
- compile-only smoke 和 example 的设计

## 发现 14：即使将 ABI/bridge 的高阶语义提前冻结到 `v0`，真正的 end-to-end 语义验证也应先收敛到 reference desktop

### 触发场景

在讨论 `ABI v0` 的覆盖范围时，用户先后确认了：

- `AQ3`：句柄集合扩展到 `PropertyInfo`、`EventInfo`、`ParameterInfo`、`GenericContext`
- `AR1`：这些高阶句柄按完整语义承诺处理
- `AS3`：完整语义承诺要进入 `Roadmap 0` 的验证资产
- `AT3`：验证形态采用真正可运行的 end-to-end 语义验证

随后用户选择：

- `AU1`：这套 end-to-end 语义验证先只在 reference desktop 上落地
- `AV3`：reference desktop 采用 `Windows x64 + macOS` 双平台

### 依据

- 如果把这套高阶 contract 语义同时压到 Android/iOS/macOS 子矩阵，`Roadmap 0` 会快速跨入后续 runtime/codegen/platform roadmap 的主战区
- 在 reference desktop 上先打穿，可以保留 contract 冻结的严肃性，同时避免移动端和多平台矩阵过早绑死进度

### 当前结论

`Roadmap 0` 中对高阶 ABI/bridge contract 的“强验证”，应理解为：

- 规范强度很高
- reference desktop 上存在真正的 end-to-end 语义验证
- 当前 reference desktop 不是单平台，而是 `Windows x64 + macOS` 双平台
- 双 reference desktop 下，`Windows` 承担主线基准集，`macOS` 先通过同语义子集
- 当前 `macOS` 子集并不是窄子集，而是接近 `Windows` 主线覆盖范围，只对明显平台相关 `icall` 差异留白
- 允许留白的 `icall` 范围也被收敛到真正的平台能力相关项，如文件系统、时间/时区、动态库装载、socket/network
- 这些平台能力相关 `icall` 在 `Roadmap 0` 中只要求 query/basic-path 级验证，不要求完整平台语义打满
- 其它平台仍按各自既定矩阵强度推进，而不是同步抬到同一层

### 复用价值

后续写详细设计或实现计划时，可以直接引用这条发现来解释：

- 为什么 `ABI v0` 看起来很重
- 但移动端和其它平台 gate 仍然分层推进

## 发现 15：即使高阶 contract 和平台能力验证不断加码，Roadmap 0 仍维持 5 个 smoke 输入项目不变，通过项目内子场景扩展覆盖面

### 触发场景

在确认高阶 ABI/bridge contract、平台能力相关 `icall` 和双 reference desktop 验证后，还需要决定这些额外覆盖落在哪些 smoke 资产里。

用户随后选择：

- `BA2`：保持 5 个 smoke 输入项目不变，但在现有项目内部增加明确的子场景/用例组

### 当前结论

`Roadmap 0` 的 smoke 输入项目数量不继续扩张，仍然维持：

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`

但项目内部将承载更多子场景，用于吸纳：

- 高阶反射/泛型 contract
- 平台能力相关 `icall`
- host / bridge / interop 相关验证

当前映射倾向进一步收敛为：

- `HostEmbeddingLite` 承担更多平台能力相关内容
- `PInvokeLite` 更聚焦在原生互操作主线

进一步具体化后：

- `HostEmbeddingLite` 负责 host lifecycle、thread attach/detach、managed entry、delegate/callback，以及文件系统/时间/动态库装载等平台能力
- `PInvokeLite` 负责 `DllImport`、native export、符号解析与基础 marshalling
- `GenericEcho` 更聚焦在最直观的泛型调用与实例形态
- `ReflectionLite` 吸收一部分 `GenericContext` 与高阶反射/元数据相关验证
- `HelloWorld` 收缩为最小启动样例，`registration` 相关内容转移到 `HostEmbeddingLite`
- 更具体地说，`HelloWorld` 仍保留最小 entry path 和最薄的一层 registration 痕迹，而完整 registration 流、host lifecycle 与 managed entry 装配集中到 `HostEmbeddingLite`
- 并且 `HostEmbeddingLite` 中的 registration/metadata bootstrap 验证会继续细化到 metadata cache 初始化阶段与 late-init 阶段
- 当前这条 bootstrap 状态机进一步被细化为四段：`registration complete`、`metadata cache initialize`、`GC-dependent late-init`、`class world / reflection cache warmup`
- 最后一段 `class world / reflection cache warmup` 也被扩大到包含泛型上下文与 delegate 相关 warmup，而不只停留在基础反射 cache
- 这段 warmup 采用全 lazy 策略：startup 只建立最小壳，真正的 class world / reflection / generic / delegate 预热按访问路径触发
- 为了验证这套 lazy warmup，`Roadmap 0` 还要求 runtime 暴露 warmup trace / event log，验证不仅看最终结果，也检查触发顺序
- 这组 warmup trace / event log 进入 `Roadmap 0` 的验证 contract，但不提升为与核心 `ABI/bridge` 同等级的长期核心 contract
- 这组 trace / event log 采用结构化 JSON 格式，以支持 snapshot、diff 与顺序校验
- 这组验证 contract 在源码树中单独成组，例如 schema 放 `tests/contracts/trace/schema/`，权威 snapshot 放 `tests/contracts/trace/snapshots/`
- trace schema 本身采用中等粒度，至少包含 `eventName`、`phase`、`subjectKind`、`subjectId`、`order`、`status`
- `subjectKind` 采用中等枚举集：`type`、`method`、`field`、`property`、`event`、`parameter`、`genericContext`、`delegate`、`assembly`
- `phase` 采用扩展枚举：`requested`、`started`、`completed`、`failed`、`skipped`、`cached`、`replayed`
- `status` 采用扩展枚举：`ok`、`error`、`skipped`、`cached`
- `eventName` 采用命名空间风格，例如 `<domain>.<action>` 或 `<domain>.<subject>.<action>`
- `eventName` 默认三段 `<domain>.<subject>.<action>`，必要时允许两段 `<domain>.<action>`
- `domain` 集合包含：`warmup`、`metadata`、`reflection`、`generic`、`delegate`、`bootstrap`、`interop`
- `subjectId` 采用规范化字符串 ID，以支持稳定 snapshot、diff 与跨平台比对
- `subjectId` 的具体风格采用人读签名式，而不是 token/path 混合式或哈希式
- `subjectId` 至少包含“程序集 / 类型 / 成员签名”，泛型场景下还带实例化形态

### 复用价值

后续写详细设计和实施计划时，可以直接把新增验证项映射到现有 smoke 项目的子场景上，而不是再引入新的项目数量和命名复杂度。
