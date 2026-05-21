# IL2CPP Core Structure And Scriban Governance Design v1.01

Date: 2026-04-17 13:20:00 +08:00
Status: draft-approved-for-roadmap

## 1. 问题重述

当前 `IL2CPP` 核心实现已经出现明显的结构失衡：

- `Chaos.IL2CPP.CodeGen` 过重，承担了过多异质职责。
- `NativeAotLoweringPlanner.cs` 把 orchestrator、runtime support 建模、helper 选择、C++ shape 组装、文本拼接混在一个文件里。
- `NativeReferenceLoweringPlanner.cs` 与 `NativeReferenceProofEmitter.cs` 存在明显职责重叠。
- `LoaderStage.cs`、`LinkerStage.cs`、`ManagedClosureContracts.cs` 已经超过合理的单文件复杂度。
- `Scriban` 虽已接入，但核心规则还未真正下沉，模板层不足以承接未来扩展。

如果不先治理结构，后续每扩一类 `AOT` 能力都会继续放大如下问题：

- 能力落点不清，难以判断该改 loader、IR、planner 还是 emitter。
- 同一份 codegen 规则在多个文件中重复维护。
- 模板与 planner 的边界越来越模糊。
- 测试框架和 `IL2CPP` 内核更容易互相渗透。

## 2. 设计目标

### 2.1 主目标

- 建立 `IL2CPP` 核心代码的长期结构规范。
- 把 `Scriban` 提升为核心代码层的一部分。
- 让 planner 只做 plan，emitter 只做 render，模板只做 structured output。
- 为后续 `AOT Core IR` 与 `NativeAOT` 功能扩展提供稳定骨架。

### 2.2 非目标

- 本轮不直接扩大量新的 `AOT` feature。
- 本轮不重构 `Chaos.TestFramework` 主线。
- 本轮不把 `CodeGen` 第一时间拆成多个新的 managed assembly。
- 本轮不把 wiki 写成执行日志。

## 3. 目标架构

### 3.1 `Chaos.IL2CPP.CodeGen` 内部目标目录

建议在 `src/managed/Chaos.IL2CPP.CodeGen/` 内部演进到如下结构：

```text
Chaos.IL2CPP.CodeGen/
  CoreIr/
    Models/
    Contracts/
    Lowering/
  Planning/
    EntryAbi/
    Dispatch/
    ExceptionHandling/
    Plans/
    Builders/
  RuntimeSupport/
    ObjectModel/
    Helpers/
    Metadata/
    Marshaling/
  Emission/
    NativeAot/
    NativeReference/
    Shared/
  ReferenceProof/
    Planning/
    Emission/
  Templating/
    ModelAdapters/
    TemplateFunctions/
    TemplateLoading/
    Rendering/
  Templates/
    NativeAot/
    NativeReference/
    Shared/
```

这个结构的关键点不是“目录看起来更多”，而是每层只允许承担单一职责：

- `CoreIr/`: 承担 typed-IL 到 core IR 的语义承接。
- `Planning/`: 负责 native shape、ABI、dispatch、EH、wrapper/thunk 的规划。
- `RuntimeSupport/`: 负责 object model、helper family、runtime bridge 的建模，不直接写最终文本。
- `Emission/`: 负责把 plan 和 runtime support model 渲染成最终产物。
- `Templating/`: 负责 `Scriban` 的模型适配、模板函数、模板加载、渲染约束。
- `Templates/`: 只放模板，不放业务规则推导。

### 3.2 其它高风险项目的目标拆分方向

- `Chaos.IL2CPP.Loader`
  - `EntryDiscovery/`
  - `MetadataDecoding/`
  - `MethodBodyLoading/`
  - `HandleResolution/`
  - `Diagnostics/`
- `Chaos.IL2CPP.Linker`
  - `Closure/`
  - `Reachability/`
  - `Preservation/`
  - `Contracts/`
- `Chaos.IL2CPP.Contracts`
  - `ManagedClosure/`
  - `Schemas/`
  - `Serialization/`
- `Chaos.IL2CPP.Driver`
  - `Commands/`
  - `Pipelines/`
  - `Workspace/`
  - `Diagnostics/`

这些项目的第一目标同样不是“拆项目”，而是先拆文件职责。

## 4. 结构规则

### 4.1 文件尺寸规则

- orchestrator 类文件目标保持在 `200-400` 行。
- 规则型 lowering/planning 文件目标保持在 `300-600` 行。
- 单文件超过 `800` 行视为异常，需要说明理由。
- 单文件超过 `1200` 行默认视为应拆分，除非是稳定 schema/enum 映射文件且已有明确注释说明。

### 4.2 命名与职责规则

- `*Planner` 只负责 plan 生成，不负责最终文本拼接。
- `*Emitter` 只负责把 plan 渲染为产物，不再二次推导业务决策。
- `*TemplateModel` 只承载模板渲染所需的只读数据，不携带行为分支。
- `*TemplateFunctions` 只提供通用格式化/语法糖函数，不承载业务级 lowering 逻辑。

### 4.3 依赖方向规则

- `CoreIr` 不依赖 `Emission`。
- `Planning` 可以依赖 `CoreIr` 与 `RuntimeSupport`，不能依赖模板实现细节。
- `Emission` 可以依赖 `Planning` 输出和 `Templating`，不能重新决定 ABI/dispatch/EH 规则。
- `Chaos.TestFramework.*` 不得反向进入 `IL2CPP` 核心层。

## 5. Scriban 的核心层定位

### 5.1 设计结论

`Scriban` 在本项目中应被视为 `IL2CPP` 核心 codegen 层的一部分，其职责不是“方便生成点字符串”，而是：

- 承担最终结构化文本产物的渲染。
- 承担可复用模板函数与格式化规则。
- 承担模板输入模型的稳定边界。
- 承担代码生成规范的长期治理面。

### 5.2 规则

以下产物默认必须走模板：

- `.cpp`
- `.h`
- `.g.cs`
- `.json`
- 其它具有明确结构的文本生成文件

以下内容应保留在 C#：

- IL 语义分析
- ABI 选择
- dispatch 决策
- EH shape 选择
- runtime support family 选择
- symbol 命名策略
- plan/graph 构造

### 5.3 当模板能力不够时的处理顺序

固定处理顺序如下：

1. 先评估是否只是模板模型缺字段。
2. 再评估是否需要新增模板函数、filters、model adapters。
3. 再评估是否需要扩展模板加载或渲染基础设施。
4. 只有在产物不是结构化文本时，才允许不经 `Scriban`。

明确禁止：

- 因为当前模板不方便，就把新规则直接塞回 `StringBuilder`。
- 在 emitter 中重新推导 planner 已经做过的业务决策。
- 在模板里写大段业务判断，把模板变成第二份 planner。

## 6. `NativeAOT` 目标职责切分

### 6.1 Planner 负责

- entry ABI 规划
- wrapper / thunk shape 规划
- dispatch family 规划
- EH lowering shape 规划
- helper family 与 runtime support 需求归集
- translation unit 级 plan 输出

### 6.2 Runtime support 负责

- object model 结构化描述
- metadata helper 描述
- marshaling/helper glue 描述
- literal tables、type tables、symbol bundle 等可复用支撑模型

### 6.3 Emitter 负责

- 组装模板模型
- 选择模板
- 调用渲染器
- 输出文件

### 6.4 Templates 负责

- 最终 `.cpp/.h` 结构化布局
- include 区块、helper 区块、method body 区块、translation unit 区块等模板结构
- 通用格式化，不承载 planner 决策

## 7. 首批重构对象

### 7.1 P0 对象

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`

### 7.2 P1 对象

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.EngineBinding/EngineBindingStubEmitter.cs`

### 7.3 P2 对象

- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
- 其它随着 `AOT` 扩展继续增长的单文件 stage 项目

## 8. 约束与边界

### 8.1 不混入测试框架逻辑

- `Chaos.TestFramework.Sdk/Runtime` 负责测试标注、运行、断言与宿主接线。
- `IL2CPP` core 只暴露 codegen/pipeline/runtime support 的正式接口。
- 不允许把测试发现、断言或 collection 解析逻辑塞入 `AotCoreIr`、planner、emitter。

### 8.2 不扩大 string 协议面

- 新的 capability/category/family/plan kind，如可枚举就不要用 string。
- 字段类型优先用枚举、小型 struct、显式 record，而不是自由字符串。
- 需要模板选择时，应由强类型 plan 映射到模板路径，而不是外部传字符串协议。

### 8.3 保持源码树干净

- `src/managed/**/bin`
- `src/managed/**/obj`
- `src/managed/**/artifacts`
- 临时目录 `.tmp-*`

这些路径不应干扰结构治理判断，后续执行阶段需要同步清理和规避。

## 9. 验收标准

### 9.1 结构验收

- 首批超大文件被拆成职责明确的目录与文件集合。
- planner/emitter/template 三者职责不再交叉。
- 新增或改动的生成规则不再直接落入大块字符串拼接。

### 9.2 工程验收

- `dotnet build` 在目标 managed 项目上通过。
- 关键单元测试与 contracts 测试通过。
- 至少一条 `NativeAOT` 真链路继续可执行，证明结构重构没有破坏主流程。

### 9.3 规范验收

- 文档规则已沉淀到 wiki。
- 后续新 feature 接入时可以直接引用规范页检查：
  - 文件是否过大。
  - 规则是否该进模板层。
  - 是否错误引入了新的 string 协议。
  - 是否混入了测试框架逻辑。

## 10. 推荐执行策略

推荐分两条线并行但不交叉写脏：

- 主线一：`Chaos.IL2CPP.CodeGen` 分层与 `NativeAot` planner/emitter/template 收口。
- 主线二：`Loader/Linker/Contracts/Driver` 的超大 stage 文件拆分。

但真正的先后顺序仍应是：

1. 先冻结规范与边界。
2. 先拆 `CodeGen`，因为这里是最大风险点。
3. 再扩 `Scriban` 基础设施并迁移 `NativeAot` 生成主面。
4. 再处理其它 stage 项目。
5. 最后补工程 gate 和 wiki 固化。
