# IL2CPP-Like C# to C++ 重启版设计（执行闭环优先）

Date: 2026-04-05 20:51:20 +08:00
Status: 草稿，待用户复核

## 设计目标

这份设计不试图直接继承旧父级 roadmap，而是要把旧路线已经沉淀出的有效资产，重新组织成更适合执行的主线入口。

目标有三点：

1. 旧路线保留为历史参考，而不是当前执行母文档。
2. 新主线以 proof-first 为第一原则，而不是 coverage-first。
3. 在 `HelloWorld` 真正打穿之前，不再过早拆出多个平行子任务主线。

## 设计原则

### 原则 1：先证明，再扩张

第一证明点不是“架构图更完整”，也不是“文档更同步”，而是：

- 一个 reference platform
- 一条真正 generated-code path
- 一次真正 runtime bootstrap
- 一个真正输出 `Hello, World!` 且返回 `0` 的 `HelloWorld`

### 原则 2：旧资产分层继承

旧路线的结论必须分成三类：

- 直接继承
- 暂时重开
- 明确废弃

如果不这样做，新路线仍会被旧路线的体量绑住。

### 原则 3：主线只能有一个

重启后的规划入口只能有一个主线：

- `HelloWorld proof spine`

在它被打穿之前，不应再把第一层工作拆回：

- 前端主线
- runtime 主线
- codegen 主线

这种看起来平行、其实互相等待的结构。

## 直接继承的资产

以下资产直接进入新路线：

- `IL-first`
- 闭世界 AOT
- `Metadata Graph / Typed IL IR / Runtime-Contract IR`
- `Linker / Metadata Writer / CodeGen / Runtime` 分层
- `System.Reflection.Metadata + PEReader` 作为读取层主实现
- `Mono.Cecil` 作为测试/对照基线
- `Scriban` 作为 codegen 模板方案
- `BDWGC behind GC facade` 作为 Phase 1 GC 路线
- `HelloWorld` object-based slice 及其五个检查点

## 暂时重开的部分

以下内容不直接废弃，但不再自动继承旧答案：

- 旧 7 条 roadmap 的主拆分是否仍然最佳
- `Roadmap 1 / Roadmap 3` 是否仍然应该作为首轮直接派生子任务
- 多平台验证何时从“显式约束”进入“真实实测”
- 第三方依赖讨论在新主线中的优先级

## 明确废弃的部分

以下内容在新路线中明确退出：

- 旧父级 roadmap 作为当前执行入口
- 旧 `Roadmap 1 / Roadmap 3` ready 子任务骨架的默认有效性
- 在 proof slice 之前继续扩展大体量外围依赖讨论

## 新主线结构

### Layer 1：Reset Package

职责：

- 整理旧方案全过程
- 输出保留/重开/废弃三分法
- 明确新路线的单一入口

### Layer 2：Heavy Contract Reset Package

职责：

- 重新定义“当前到底从哪里开工”
- 复用现有 contract、schema、smoke 和 runtime skeleton，但不再默认沿用旧父级 roadmap 的拆分方式
- 对 proof 所依赖的主 contract 做一次显式重开与重新冻结
- 明确现有 `HelloWorld` smoke 样例与目标 proof 样例之间的差异

约束：

- 这是一次面向 proof 的重量版 contract reset，但仍不是原样恢复旧 `Roadmap 0`
- 只重开与第一条 proof 直接相关的 contract 和执行骨架
- 不新增与第一条 proof 无关的大体量外围讨论

重开范围：

- `typed-il-ir`
- `runtime ABI`
- `codegen bridge`
- `AOT manifest`
- `code registration / metadata registration`
- smoke/proof 样例分层与 gate 边界

其中以下内容已得到用户确认，必须纳入 Stage 1：

- `AOT manifest`
- `code registration`
- `metadata registration`

明确不在本层展开：

- 第三方依赖再收敛
- 多平台 rollout
- `HelloWorld` 之外的新样例主线
- Phase 2/3 远期能力拆分

### Layer 3：HelloWorld Proof Package

职责：

- 以 `HelloWorld` 为唯一第一 proof slice
- 收拢最小 semantic world、manifest、metadata writer、runtime bootstrap 与 codegen landing path

### Layer 4：Reference Platform Proof

职责：

- 在单参考平台上完成：
  - 生成
  - 编译
  - 启动
  - 输出
  - 退出

### Layer 5：Post-Proof Roadmap Split

职责：

- 只有在 proof 成功后，才重启更细的 roadmap 拆分
- 那时再决定是否恢复类似：
  - `Loader/IR`
  - `Runtime Core`
  - `Linker/Manifest`
  - `CodeGen/Build`
  的平行分工

## 新计划的阶段定义

### Stage 0：复盘与冻结

退出条件：

- 旧方案全过程被梳理完成
- 新路线的保留/重开/废弃边界明确

### Stage 1：重量版执行入口与契约重整

退出条件：

- 明确哪些现有骨架直接继承，哪些需要重写或降级为历史底座
- 完成至少以下 contract 表面的重新冻结：
  - `typed-il-ir`
  - `runtime ABI`
  - `codegen bridge`
  - `AOT manifest`
  - `code registration / metadata registration`
- 明确当前 `tests/smoke/input/HelloWorld` 只代表 smoke，不代表第一 proof slice
- 新主线入口被压缩为单一路径，而不是回到旧父级 roadmap 或并行子任务入口
- 形成一份可直接约束 Stage 2 的 contract pack，而不是继续引用旧路线分散文档

建议的 Stage 1 最终交付物形态：

- 一份 proof-facing contract pack 总文档
- 一份 Stage 1 资产映射与替换关系表
- 一份 smoke/proof 分层规则
- 一份 `HelloWorld` proof 的契约输入清单

### Stage 1 执行节奏约束

- Stage 1 的设计边界保持重量版完整形态，不在 design 阶段预先裁成窄版
- 如果实际落地体量过大，允许在执行阶段再拆成多个批次推进
- 这种拆分只影响执行节奏，不改变 Stage 1 目标交付物的完整性
- 换句话说，先保留完整 contract / structure / tests landing 目标；若实现压力过大，再按顺序分期，而不是先把目标本身缩掉

### Stage 1 Contract Pack 建议结构

建议把重量版 Stage 1 收敛为 5 个 pack，而不是重新拉回旧 roadmap 的多阶段母文档：

#### Pack A：Proof Envelope

职责：

- 冻结 object-based `HelloWorld` 的目标形状
- 明确 smoke `HelloWorld` 与 proof `HelloWorld` 的角色差异
- 定义 Stage 1 只服务于哪一条 proof spine

建议的冻结内容：

- 目标 proof 样例不是当前仓库里的 registration-trace smoke
- 目标 proof 样例应恢复为旧路线已冻结过的 object-based `HelloWorld`
- 第一 proof spine 只围绕一个最小可执行 contract 展开，不并入 `GenericEcho`、`ReflectionLite`、`PInvokeLite` 等后续样例

建议的 proof 样例形状：

- 一个用户类型 `Greeter`
- 一个实例字段 `_name`
- 一个构造函数 `Greeter..ctor(string)`
- 一个实例方法 `Greeter.BuildMessage()`
- 一个 `Program.Main(string[] args)`，其中：
  - 分配 `Greeter("World")`
  - 调用 `BuildMessage()`
  - 调用 `Console.WriteLine(string)`
  - 返回 `0`

建议的外部可观察契约：

- stdout 为 `Hello, World!`
- exit code 为 `0`
- 无 per-method 手工 rescue path
- proof 结果必须经过 generated-code path，而不是解释执行或宿主直连替代路径

建议的调用落点约束：

- 第一 proof 的 managed 调用落点明确采用 generated direct-call landing
- `Program.Main(string[] args)` 作为 generated entry 落地
- `Greeter..ctor(string)` 与 `Greeter.BuildMessage()` 由 generated C++ 直接符号调用
- 不用 `method_invoke`、解释执行或宿主直连去替代这三段调用路径

建议的最小语义包络：

- managed entry
- object allocation
- constructor invocation
- instance field store/load
- instance method call
- string literal materialization
- 一条 narrow concat path
- 一条 narrow `Console.WriteLine(string)` path
- runtime bootstrap
- `AOT manifest`
- `code registration`
- `metadata registration`

当前 smoke 样例的重新定位：

- [`tests/smoke/input/HelloWorld/Program.cs`](e:/agent/booming-il2cpp/tests/smoke/input/HelloWorld/Program.cs) 继续保留为 managed smoke
- 它的角色是：
  - runner / host / registration-trace 冒烟
  - 现有 smoke harness 连通性验证
- 它的角色不是：
  - 第一 proof slice
  - object-model 验证样例
  - generated C++ 端到端证明样例

Pack A 明确不纳入的能力：

- 泛型语义扩张
- 反射语义扩张
- P/Invoke / marshaling
- 广义异常流
- 多平台行为一致性
- `HelloWorld` 之外的第二条 feature slice

Pack A 的退出条件：

- proof 样例形状被明确写死
- smoke 与 proof 的角色边界被写死
- Pack B、Pack C、Pack D 都以这条 proof spine 作为唯一上游输入

#### Pack B：Frontend Artifact Contract

职责：

- 重开并冻结 `typed-il-ir`
- 明确它对 `Greeter..ctor`、`Greeter.BuildMessage`、`Program.Main` 至少要表达什么
- 明确哪些 typed IL 信息仍然故意不在 `v0` 冻结

建议的冻结目标：

- `typed-il-ir` 在 Stage 1 不追求“通用而完整”，只追求“足够支撑第一条 proof spine”
- `v0` 的成功标准不是覆盖更多 IL，而是稳定表达 object-based `HelloWorld` 的三段方法体
- 现有 schema 若不足以稳定表达这三个方法的最小语义，应允许在 Stage 1 内调整

必须稳定表达的方法：

- `Greeter..ctor(string)`
- `Greeter.BuildMessage()`
- `Program.Main(string[] args)`

对三个方法的最小表达要求：

- `Greeter..ctor(string)`
  - base ctor call
  - `_name` field store
- `Greeter.BuildMessage()`
  - string literal load
  - `_name` field load
  - narrow concat helper call 或等价调用表达
- `Program.Main(string[] args)`
  - `newobj`
  - local store/load
  - instance method call
  - `Console.WriteLine(string)` call
  - `return 0`

建议冻结的 artifact 级约束：

- 顶层必须稳定包含：
  - `formatVersion`
  - `artifactKind`
  - `methods`
- 方法级必须稳定包含：
  - `methodId`
  - `subjectId`
  - `signature`
  - `parameters`
  - `blocks`
- block 级必须稳定包含：
  - `blockId`
  - `instructions`
- instruction 级至少要能稳定表达：
  - `op`
  - 必要的 `operand`
  - 必要的 `resultType`
  - 必要的 `callee`

建议新增的 shared identity 约束：

- `typed-il-ir` 必须显式输出 canonical `subjectId`，而不是只依赖 artifact 内部 `methodId`
- proof 样例涉及的 type / method / field 引用，必须能在 artifact 内稳定追溯到 canonical `subjectId`
- `callee`、field operand 与 retained subject 的对齐规则，必须能直接复用到 Pack D，而不是在 bootstrap 阶段重新补隐式映射

建议新增或明确的稳定性约束：

- 方法顺序必须可预测，便于 snapshot/diff
- block 顺序必须可预测，避免同义重排
- 对 proof 样例所需的 call/newobj/field/string/return 指令形状应形成明确样例快照
- schema 应允许未来扩展字段，但 Stage 1 要写清哪些字段是 proof 必需字段

Stage 1 明确不冻结的内容：

- SSA 形式
- 优化标记
- 异常区和 EH table
- 泛型展开细节
- debug info
- 平台特定 lowering 信息
- 面向 `HelloWorld` 之外样例的额外 instruction taxonomy

Pack B 的退出条件：

- 能为三个 proof 方法各自产出稳定、可 diff 的 typed IL artifact
- Pack D 可以直接消费或映射这些 artifact 所需的方法/类型/成员标识
- Pack C 可以从这里反推出 proof 所需的最小 helper 面，而无需再引入额外前端扩张

#### Pack C：Runtime Boundary Contract

职责：

- 重开并冻结 `runtime ABI`
- 重开并冻结 `codegen bridge`
- 明确哪些 helper 属于 ABI，哪些属于 bridge，哪些暂不进入 proof

建议的冻结原则：

- `runtime ABI` 只承载低层稳定运行时操作
- `codegen bridge` 只承载 generated-code 直接消费的高层 helper
- 如果某个 helper 不是第一条 proof spine 必需，就不进入 Stage 1 的规范性最小面
- 现有 header 可以暂时保留更宽表面，但 Stage 1 要明确 proof-required subset

建议归入 `runtime ABI` 的 proof-required subset：

- runtime lifecycle
  - `runtime_init`
  - `runtime_shutdown`
- thread lifecycle
  - `thread_attach`
  - `thread_detach`
- allocation / type init
  - `object_new`
  - `string_new_utf8`
  - `class_init`
- field access
  - `field_get_value`
  - `field_set_value`
- managed failure path
  - `raise_managed_exception`
- 最小查询面
  - `assembly_get_image`
  - `image_find_type`
  - `type_find_method`
  - `type_find_field`

建议暂不作为 proof-required subset 的 ABI 表面：

- `array_new`
- `gc_handle_new / gc_handle_free`
- `method_invoke`
- `type_find_property`
- `type_find_event`
- `method_get_parameter`
- `method_get_generic_context`

说明：

- 这些接口可以继续保留在当前 header 中
- 但在 Stage 1 中不应成为 proof 成败的前置冻结要求

建议归入 `codegen bridge` 的 proof-required subset：

- registration / bootstrap
  - `register_codegen`
  - `bootstrap_runtime`
- token resolve
  - `resolve_type_by_token`
  - `resolve_method_by_token`
  - `resolve_field_by_token`
- direct call landing
  - generated `Program.Main` entry landing
  - generated `Greeter..ctor(string)` direct symbol call
  - generated `Greeter.BuildMessage()` direct symbol call
- 一条最小实例调用支持路径
  - `callvirt`-as-null-check helper 或等价窄 helper

对现有 bridge 表面的建议处理：

- `resolve_virtual_method`
  - 对第一 proof 可降级为“只服务 callvirt null-check pattern 的窄 helper”，不要求完整虚派发语义
- `invoke_virtual`
  - 不要求完整通用虚调用；允许在 Stage 1 内收窄成 first-proof pattern
- `box_value / unbox_value`
  - 明确延后
- `create_delegate / delegate_invoke`
  - 明确延后
- `resolve_icall`
  - 明确延后

Pack C 的关键设计结论：

- ABI 负责“活着、分配、读写、查询、抛错”
- bridge 负责“注册、bootstrap、token 解析、generated-code 友好的最小调用辅助”
- 不允许把 generated-code helper 继续无界地堆进 ABI
- 也不允许把 runtime 生命周期和对象模型职责上推给 bridge

Pack C 的退出条件：

- 第一 proof 所需的 helper 已被分配到 ABI 或 bridge
- 明确列出延后 helper，避免 Stage 1 内隐性扩张
- Pack D 可以基于这套边界定义 registration/bootstrap 消费路径

#### Pack D：Retained World And Bootstrap Contract

职责：

- 重开并冻结 `AOT manifest`
- 重开并冻结 `code registration`
- 重开并冻结 `metadata registration`
- 明确 bootstrap 最小消费路径

建议的冻结目标：

- 这三个 artifact 不再只是“有 schema”，而要形成第一 proof 的可消费闭环
- `typed-il-ir` 必须显式产出 canonical `subjectId`
- `typed-il-ir`、`AOT manifest`、`code registration`、`metadata registration` 之间必须共享稳定 `subjectId`
- bootstrap 的输入必须能被明确追溯到这些 artifact，而不是依靠隐式命名或手工拼接

建议的 `AOT manifest` 最小保留集：

- `Program.Main`
- `Greeter..ctor(string)`
- `Greeter.BuildMessage()`
- `Greeter._name`
- `System.Object..ctor`
- `System.Console.WriteLine(string)`
- 所选 narrow concat path 对应目标
- proof 所需 runtime/bootstrap metadata roots

建议的 `code registration` 最小冻结面：

- 至少覆盖 generated method pointers：
  - `Program.Main`
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
- 模块级 registration 顺序必须稳定
- `slot` 与 `symbol` 的生成规则必须可 diff
- proof 阶段不要求为延后能力提前铺满 registration kinds

建议的 `metadata registration` 最小冻结面：

- `HelloWorld` proof assembly
- `Greeter` type
- 参与 proof 的 methods
- `Greeter._name` field
- proof 所需的字符串/字面量元数据落点

建议的 bootstrap 最小消费路径：

1. 读取或接收 `code registration`
2. 读取或接收 `metadata registration`
3. 调用 `register_codegen`
4. 调用 `bootstrap_runtime`
5. generated entry 根据 token / subjectId 找到所需 runtime subject

Pack D 需要额外冻结的一条规则：

- 所有 contract artifact 必须共享同一套 canonical `subjectId` 规则

因为：

- `typed-il-ir` 里的 `subjectId`
- `AOT manifest` 里的 retained subject
- `code registration` 的 subject
- `metadata registration` 的 subject

如果不能稳定对齐，后续只会在 bootstrap 阶段重新引入隐式映射。

Pack D 的退出条件：

- retained world、registration、bootstrap 路径可被说明成一条单向消费链
- proof 所需最小条目已经枚举完成
- `subjectId` 规则被明确写成 Stage 1 共享约束

#### Pack E：Verification And Sample Roles

职责：

- 冻结 smoke / proof / follow-on sample 的角色边界
- 明确 snapshot、diff、compile-only、run-proof 各自 gate
- 避免当前 managed smoke 样例继续冒充 proof 入口

建议的样例角色分层：

- `smoke`
  - 用于 runner、host、toolchain、registry、基础 harness 连通性
- `proof`
  - 用于第一条 object-based `HelloWorld` 端到端证明
- `follow-on`
  - 用于 proof 之后的功能递进样例，如 `GenericEcho`、`ReflectionLite`、`PInvokeLite`

建议的 gate 分层：

- Schema Gate
  - 校验 `typed-il-ir`、`AOT manifest`、`code registration`、`metadata registration` schema
- Contract Compile Gate
  - 校验 `runtime ABI`、`codegen bridge` compile-only smoke
- Artifact Diff Gate
  - 校验 proof 所需 artifact 稳定可 diff
- Native Build Gate
  - 校验 generated C++ 可以编译
- Run Proof Gate
  - 校验 object-based `HelloWorld` 输出 `Hello, World!` 且返回 `0`

对当前样例目录的建议处理：

- 继续保留：
  - `tests/smoke/hello-world`
  - `tests/smoke/input/HelloWorld`
- 明确保留 `tests/contracts/trace/` 作为 trace contract 的 canonical 路径
- `tests/contract/trace/` 只作为短期 compatibility 路径，不再新增长期内容
- 但新增独立 proof 目录，而不是复用 smoke 路径：
  - `tests/proof/hello-world-object`
  - `tests/proof/input/HelloWorldObject`

对 follow-on 样例的建议处理：

- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`

继续保留在 `tests/smoke` 或未来 `tests/follow-on` 体系中，但不进入第一 proof 的 gate。

Pack E 的退出条件：

- smoke、proof、follow-on 三类样例不再共享同一路径语义
- 第一条 proof 有独立目录、独立 gate、独立结果判断
- 现有 smoke 资产继续可用，但不会再对 Stage 1 造成命名误导

## 建议目录结构与技术路线

下面这版目录结构不是实现清单，而是为当前确认的技术路线服务的承接骨架。

## 结构方案对比（本轮新增）

### 1. Contract 分层方案

#### 方案 A：维持双根目录

结构：

- `analysis/contracts/`
  - artifact schema
  - example artifact
  - `subjectId` 规则说明
- `contracts/native/`
  - `runtime ABI`
  - `codegen bridge`
  - native docs/examples
- `src/managed/Chaos.IL2CPP.Contracts/`
  - managed 侧 DTO、adapter、contract model

优点：

- 迁移成本最低
- 不会打断现有 schema 与 native contract 路径
- 对当前仓库最保守

缺点：

- 随着 `typed-il-ir / manifest / registration / ABI / bridge` 一起成为 Stage 1 一等公民，`analysis/contracts` 这个名字会越来越误导
- 长期会形成“artifact contract”和“native contract”两个事实上的根目录
- 不利于统一版本化和统一 discoverability

适用场景：

- 只想短期继续 brainstorm，不准备很快进入结构性迁移

#### 方案 B：统一到单一 `contracts/` 根目录

结构：

- `contracts/artifacts/v0/`
  - schema
  - examples
  - `subjectId` / artifact contract docs
- `contracts/native/v0/`
  - `runtime_abi.h`
  - `codegen_bridge.h`
- `contracts/docs/v0/`
  - contract 文档总览
- `contracts/examples/v0/`
  - native/managed/registration/bootstrap example
- `src/managed/Chaos.IL2CPP.Contracts/`
  - managed code 侧 contract object model

优点：

- 所有 proof-facing contract 都在同一根下
- 版本化更直观
- 很适合现在“contract 越来越多”的状态
- 后续 `Pack A/B/C/D` 都能自然映射到 `contracts/`

缺点：

- 需要迁移现有 `analysis/contracts`
- 需要重新梳理 example / docs / schema 的落点

适用场景：

- 已确认 contract 是 Stage 1 主体，准备让它成为长期主骨架

#### 方案 C：按消费平面拆为 `frontend-contracts` / `runtime-contracts`

结构：

- `contracts/frontend/`
  - `typed-il-ir`
  - `aot-manifest`
  - `code-registration`
  - `metadata-registration`
- `contracts/runtime/`
  - `runtime ABI`
  - `codegen bridge`
- `contracts/shared/`
  - `subjectId`
  - versioning
  - common docs

优点：

- 能体现 producer/consumer 边界
- 对大型系统扩张后更清楚

缺点：

- 在当前阶段有点过度设计
- `manifest / registration` 究竟更偏 frontend 还是 bootstrap，很容易出现边界争议
- 会让当前 reboot 讨论更重

适用场景：

- proof 已打穿、准备向多 vertical slice 扩张时

已确认选择：

- 选择 **方案 B**

结论：

- contract 长期方向采用单一 `contracts/` 根目录
- `analysis/contracts` 后续应迁入 `contracts/artifacts/` 体系，而不是继续作为长期独立根目录
- `contracts/native/` 保留，但成为统一 `contracts/` 根下的 native contract 子树

### 2. `src/managed` 命名与分层方案

用户已明确要求使用 `Chaos.IL2CPP.XXXXXX` 前缀。

#### 方案 A：细粒度分层命名

- `Chaos.IL2CPP.Contracts`
- `Chaos.IL2CPP.Driver`
- `Chaos.IL2CPP.Loader`
- `Chaos.IL2CPP.SemanticWorld`
- `Chaos.IL2CPP.Pipeline`
- `Chaos.IL2CPP.Linker`
- `Chaos.IL2CPP.MetadataWriter`
- `Chaos.IL2CPP.CodeGen`

优点：

- 和当前架构讨论一一对应
- 很适合 proof-first 逐层落地
- 后续拆 plan 时自然

缺点：

- 早期项目数量较多
- 需要更强的工程约束来防止跨层偷依赖

#### 方案 B：合并成较粗的 4 层

- `Chaos.IL2CPP.Contracts`
- `Chaos.IL2CPP.Frontend`
- `Chaos.IL2CPP.AOT`
- `Chaos.IL2CPP.Backend`

优点：

- 项目数少
- 早期搭骨架快

缺点：

- 很快会变胖
- `Linker / MetadataWriter / CodeGen` 边界会重新糊掉

#### 方案 C：折中型分层

- `Chaos.IL2CPP.Contracts`
- `Chaos.IL2CPP.Loader`
- `Chaos.IL2CPP.World`
- `Chaos.IL2CPP.AOT`
- `Chaos.IL2CPP.CodeGen`

优点：

- 比方案 A 轻一点
- 比方案 B 清楚一点

缺点：

- `World / AOT / CodeGen` 的职责仍容易互相渗透
- 不如方案 A 那么稳定对应我们已经冻结的 pack

已确认选择：

- 选择 **方案 A**

结论：

- `src/managed` 采用细粒度 `Chaos.IL2CPP.*` 分层
- 后续 plan 与目录都应直接使用这组命名，而不是再回到粗粒度聚合项目

### 3. `tests/` 结构反推方案

当前观察：

- `tests/contract` 和 `tests/contracts` 同时存在，属于明显重复
- `tests/smoke` 里现在混有未来 proof 会关心的样例名称
- `tests/gate` 更像 CI gate 语义，不太像稳定目录语义
- `tests/registry` 更像 fixture / manifest data，而不是测试类型本身

#### 方案 A：最小修补

结构：

- 合并 `tests/contract` 与 `tests/contracts` 为 `tests/contracts`
- 保留现有 `tests/smoke / tests/integration / tests/platform / tests/gate / tests/registry`
- 新增 `tests/proof`

优点：

- 改动最小
- 最快落地

缺点：

- `gate`、`registry` 的语义仍然不够干净
- 长期仍会继续混

#### 方案 B：按测试角色重组

结构：

- `tests/contracts/`
  - schema
  - native
  - trace
- `tests/smoke/`
  - managed
  - host
- `tests/proof/`
  - `hello-world-object`
- `tests/integration/`
  - runner
  - pipeline
  - registry
- `tests/platform/`
  - android
  - ios
  - packaging
- `tests/unit/`
- `tests/fixtures/`
  - catalog
  - registry

优点：

- 和当前 reboot 后的真实需求最匹配
- smoke / proof / integration / platform 角色更清晰
- `gate` 从目录概念回到“CI 阶段概念”

缺点：

- 需要迁移现有 `registry`、`gate` 的一些路径
- run tooling 可能要同步更新

#### 方案 C：按执行阶段组织

结构：

- `tests/stage1-contracts`
- `tests/stage2-proof`
- `tests/stage3-pipeline`
- `tests/stage4-runtime`
- `tests/stage5-platform`

优点：

- 和当前 roadmap/plan 语言高度一致

缺点：

- 一旦 plan 变，目录语义就会漂
- 不适合长期测试资产沉淀

已确认选择：

- 选择 **方案 B**

结论：

- `tests/` 按长期角色重组，而不是按当前阶段命名
- `tests/contract` 与 `tests/contracts` 必须合并，统一保留 `tests/contracts`
- `tests/contracts/trace` 必须保留为 canonical trace contract 路径
- `tests/contract/trace` 只允许作为短期兼容层，后续 tooling 和 manifest 要逐步切换过去
- `tests/gate` 与 `tests/registry` 后续应回收到更稳定的角色目录中

### 顶层已确认方向

- `contracts/artifacts/v0/`
  - schema
  - example artifact
  - `subjectId` 规则说明
- `contracts/native/v0/`
  - proof-facing `runtime ABI` 与 `codegen bridge` 头文件
- `contracts/docs/v0/`
  - contract 总文档
- `contracts/examples/v0/`
  - native / artifact / bootstrap example
- `src/managed/`
  - 新增，承接 managed 侧翻译流水线
- `src/native/`
  - 新增，承接 runtime core 与 bootstrap 落地
- `tests/contracts/`
  - 放 schema / compile-only contract 测试
- `tests/smoke/`
  - 保留为非 proof 的基础冒烟
- `tests/proof/`
  - 新增，承接第一条以及后续 proof 级样例
- `tests/fixtures/`
  - 放 catalog / registry / manifest fixture
- `tests/integration/`
  - 放 runner / pipeline / registry 扫描等集成测试
- `tests/platform/`
  - 放平台和打包验证
- `artifacts/`
  - 区分 `smoke/` 与 `proof/` 产物

### Managed 侧已确认目录

- `src/managed/Chaos.IL2CPP.Contracts/`
  - 请求对象、subjectId、共享 DTO、artifact model
- `src/managed/Chaos.IL2CPP.Driver/`
  - 薄 orchestration，不承载语义状态
- `src/managed/Chaos.IL2CPP.Loader/`
  - SRM + PEReader 读取层
- `src/managed/Chaos.IL2CPP.SemanticWorld/`
  - metadata graph / world model
- `src/managed/Chaos.IL2CPP.Pipeline/`
  - phase orchestration、context、collector、results
- `src/managed/Chaos.IL2CPP.Linker/`
  - retained world / `AOT manifest`
- `src/managed/Chaos.IL2CPP.MetadataWriter/`
  - `code registration` / `metadata registration`
- `src/managed/Chaos.IL2CPP.CodeGen/`
  - `Runtime-Contract IR` 到 generated C++ 发射

### Native 侧已确认方向

- `src/native/runtime-core/`
  - runtime lifecycle、thread attach、object/string/class init、field access
- `src/native/bootstrap/`
  - registration attach、bootstrap path、generated entry landing
- `src/native/support/`
  - proof 所需的窄 helper 支撑，如 concat / console path glue

### Tests 侧已确认方向

- `tests/contracts/schema/`
  - schema 验证
- `tests/contracts/trace/`
  - trace schema、snapshot、compare script 的 canonical 落点
- `tests/contracts/native/abi/`
  - ABI compile-only
- `tests/contracts/native/bridge/`
  - bridge compile-only
- `tests/smoke/`
  - managed / host / runner 冒烟
- `tests/smoke/input/HelloWorld/`
  - 继续作为 smoke
- `tests/proof/hello-world-object/`
  - proof suite manifest 与 run gate
- `tests/proof/input/HelloWorldObject/`
  - 新的 object-based proof 输入
- `tests/integration/`
  - runner / pipeline / registry 扫描等集成验证
- `tests/platform/`
  - android / ios / packaging
- `tests/unit/`
  - 细粒度工具与适配器单测
- `tests/fixtures/`
  - catalog / registry / manifest fixture

### 对应的技术路线

- Stage 1
  - 先冻结统一 `contracts/` 根下的 proof-facing contract pack
- Stage 2
  - 在 `tests/proof/input/HelloWorldObject` 上固定 object-based `HelloWorld`
- Stage 3
  - 按 `src/managed` 的 `Chaos.IL2CPP.Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 打通最小闭环
- Stage 4
  - 按 `src/native` 的 `runtime-core -> bootstrap -> support` 跑通 reference platform proof
- Stage 5
  - proof 成功后，再决定是否把 `GenericEcho / ReflectionLite / PInvokeLite` 迁入 follow-on 路线，或拆出新的 roadmap

### Stage 2：`HelloWorld` 规格包

退出条件：

- `HelloWorld` 的输入、输出、IL 形状、metadata 要求、runtime helper 需求全部固定

### Stage 3：最小实现闭环

退出条件：

- 从 assembly load 到 generated C++ 再到 runtime bootstrap 的闭环路径可被真实执行

### Stage 4：参考平台证明

退出条件：

- `HelloWorld` 在 reference platform 上输出 `Hello, World!`
- exit code 为 `0`
- 没有 per-method 手工 rescue path

### Stage 5：证明后的 roadmap 重构

退出条件：

- 基于真实 proof 结果，而不是基于抽象预期，再拆后续 roadmap

## 当前建议

这轮重启后，不建议立即写新的大而全 roadmap。

当前已和用户对齐的执行顺序是：

1. 先做执行入口与工程骨架重整
2. 再进入 `HelloWorld proof-first`

更合理的顺序是：

1. 先完成复盘和新主线冻结
2. 再做一轮面向 proof 的重量版 contract reset
3. 把重整后的骨架压缩成单一执行入口
4. 然后围绕 object-based `HelloWorld` 形成更窄的执行计划
5. 等 proof 成功后，再决定新的 roadmap 拆分

## 预期下一步

如果用户认可这份重启版设计，下一步应进入：

- 先定义一个“重量版执行入口与契约重整”的计划
- 然后再进入 proof-first 的 `plan-v1-01`

该计划应直接围绕：

- 现有骨架资产复用边界
- `typed-il-ir / runtime ABI / codegen bridge / manifest / registration` 的 contract reset
- `HelloWorld` smoke/proof 分离
- `HelloWorld`
- reference platform
- minimal end-to-end proof

展开，而不是再次从大而全 roadmap 开始。
