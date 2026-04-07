# 测试框架重构与升级 Brainstorm v1

Date: 2026-04-06
Status: discuss

## 这份文档记录什么

这不是最终 design。

这份文档只记录当前讨论阶段已经形成的判断、用户新增约束、候选重构方向，以及后续需要逐项确认的决策点。

## 当前背景

当前仓库已经有一套统一测试入口：

- `run test ...`
- `tests/registry/**`
- `module / system / pipeline`
- 统一日志、事件流、summary/report 契约

但它当前更接近“统一入口与调度外壳”，还不是“围绕 IL2CPP proof spine 的测试体系”。

现状里最明显的问题有：

- `proof` 目录已经出现，但还没有成为统一测试框架里的第一等对象。
- `managed-closure` 的命名和归类不准确。
- 真实 proof 执行还被包在 Python `unittest` 里，而不是测试框架的原生对象。
- 环境维度当前主要依赖 `supportedHosts`，不足以表达 IL2CPP 的 host/target/toolchain 差异。
- 当前高层对象更多是组合已有 suite，还没有形成按 IL2CPP 阶段切开的诊断能力。

## 当前已经确认的输入

### 1. 测试目标有四个重点维度

用户明确提出，后续测试框架应至少覆盖以下维度：

1. 开发环境正确性验证
2. 多平台正确性验证
3. 开发环境性能测试
4. 发布产物性能报告

其中第 4 点不是单点 perf smoke，而是要覆盖所有 case 和所有平台的产物级性能报告。

用户随后补充：

- 测试平台适配性也应纳入与多平台正确性同一组问题里考虑
- 也就是说，`correctness.platform` 不只是“结果对不对”，还包括“该平台是否可适配、以何种验证深度适配”

### 2. 测试项目应提升到顶层

用户明确提出，像 `HelloWorldObject` 这样的测试项目不应深埋在当前测试树内部，而应成为顶层测试输入对象。

用户给出的目标分层是：

1. 原项目
2. dotnet/mono 环境产物
3. IL2CPP 分析产物
4. 生成产物
5. build 产物

这样做的目的不是美化目录，而是为了让验证过程能沿着产物链回溯，从生成产物定位 IL2CPP 问题。

### 3. `managed-closure` 命名不合理

这个结论已确认。

后续需要讨论的不是“要不要改名”，而是：

- 它真正对应的是哪一层语义
- 它在测试体系里应该属于哪类对象
- 它与 Stage 3 / Stage 4 proof 的关系如何表达

### 4. 当前其它改进方向总体认可

用户认可本轮讨论中已经提出的这些大方向：

- 测试最终目标应重回 IL2CPP 主线，而不是围绕测试框架自身打转
- proof 应成为一等公民
- 测试环境维度需要显式建模
- 需要把高层对象真正变成诊断载体，而不是空壳组合

## 我当前对测试最终目标的判断

如果后续测试框架要长期服务这个仓库，最终目标不应定义成“所有测试入口统一到 `run test`”。

更合理的最终目标是：

1. 任意一个 proof / sample / case 都能沿着固定产物链被观察和诊断
2. 任意一次失败都能判断它落在哪一层：
   - 环境
   - 输入项目
   - frontend 分析
   - codegen 生成
   - native build
   - runtime 执行
3. 任意一个平台结论都能区分：
   - host 正确性
   - target 正确性
   - 性能
   - 发布报告
4. 发布前能自动汇总所有 case、所有平台的正确性和性能状态，而不是手工拼 checklist

换句话说，测试框架要服务“证据链”和“诊断链”，不是只服务“命令统一”。

## 建议的新测试视角

我建议把后续测试框架改成“两层视角同时存在”。

### 视角 A：测试目标维度

- correctness.dev
- correctness.platform
- perf.dev
- perf.release

这层回答的是“为什么要测”。

### 视角 B：产物与执行链阶段

- source project
- host runtime input
- frontend analysis artifact
- generated artifact
- build artifact
- runtime proof result

这层回答的是“测到哪里、坏在哪里”。

这两层组合后，测试才能同时表达：

- Windows 开发机上 frontend analysis 是否正确
- macOS target packaging gate 是否正确
- release binary 在 Android arm64 上的性能报告是否回退

## 初步重构方向

## 方向 1：把测试对象从“family 优先”切成“证据链优先”

当前 `unit / integration / contract / smoke / gate / workflow / perf` 更像执行类型。

后续更合理的中心应该是：

- 测试项目
- 产物阶段
- 环境矩阵
- 验证目标

`family` 仍可保留，但应退到执行分类，而不是主语义入口。

## 方向 2：把测试项目提升到顶层

像 `HelloWorldObject` 这种对象，后续更像“subject/sample”，不是普通 test fixture。

建议后续把这类项目从深层 `tests/proof/input/...` 结构中提升出来，成为顶层稳定输入源。

这样 Stage 3/Stage 4 和更后续的 proof/gate 都能消费同一份输入，而不是各自藏一份。

## 方向 3：把 Stage 级验证补齐

当前 managed 主线已经很清楚：

- Loader
- SemanticWorld
- Linker
- MetadataWriter
- CodeGen
- Driver

但测试层面还没有围绕这条链形成对应的 stage 级验证对象。

后续应该允许直接回答：

- 坏在 Loader 读 metadata
- 坏在 Linker retained world
- 坏在 MetadataRegistration
- 坏在 generated symbol emission
- 坏在 native bootstrap

而不是统一报成 “managed closure bundle failed”。

## 方向 4：把环境维度从 `supportedHosts` 升级为显式矩阵

当前只写 `supportedHosts` 不够。

后续至少需要显式区分：

- `hostPlatform`
- `targetPlatform`
- `toolchainProfile`
- `runtimeProfile`
- `validationMode`

其中 `validationMode` 我建议最少支持：

- `schema`
- `compile-only`
- `build`
- `startup`
- `output`
- `trace`
- `perf`
- `report`

## 方向 5：性能测试要拆成两条线

用户已经明确区分：

- 开发环境性能测试
- 发布产物性能报告

我认可这个拆分。

因为这两者的目标不同：

- 开发环境性能测试：用于迭代期发现 frontend/codegen/build 回退
- 发布产物性能报告：用于 release 阶段给出完整平台矩阵和 case 覆盖

两者不应继续混在一个 `perf` family 里。

## 需要逐项确认的决策点

下面这些点需要和用户一项项讨论。

## 决策点 1：测试框架的第一主轴是什么

### 方案 A：保留当前 family 为主轴，只在其上补环境矩阵

- 优点：改动最小，兼容现在的 `run test` 使用方式
- 缺点：测试框架仍会天然偏向“怎么执行”，而不是“为什么测、测到哪”

我的观点：

- 不推荐
- 适合短期修补，不适合作为长期架构

### 方案 B：改成“测试目标维度”为主轴，family 退到执行层

- 主轴：`correctness.dev / correctness.platform / perf.dev / perf.release`
- 执行层仍可保留 `family`
- 优点：更贴合用户刚刚明确的四个核心维度
- 缺点：需要重写现有 registry / menu / selector 的部分语义

我的观点：

- 这是当前最稳的长期方案
- 它能把“正确性”和“性能”直接提到顶层，不容易继续跑偏

### 方案 C：改成“产物链阶段”为主轴

- 主轴：source / analysis / generated / build / runtime
- 优点：诊断性极强，最贴近 IL2CPP 的问题定位
- 缺点：对非工程视角的使用者不够直观，也不直接表达 perf/release 目标

我的观点：

- 适合做第二主轴
- 不适合单独做唯一主轴

当前推荐：

- 采用 B + C 组合
- 即“测试目标维度”作为第一主轴，“产物链阶段”作为第二主轴

用户已确认：

- 决策点 1 采用 `B + C`
- 后续顶层测试框架以“测试目标维度”为第一主轴，以“产物链阶段”为第二主轴
- `correctness.platform` 中显式纳入“平台适配性”讨论，而不单独拆成无关维度

## 决策点 2：顶层测试项目应该如何落地

### 方案 A：维持 `tests/proof/input/*`，只在逻辑上把它视为顶层对象

- 优点：改目录最少
- 缺点：语义上仍然像 test fixture，不像稳定输入项目

我的观点：

- 不推荐
- 它会继续弱化 proof/sample 的地位

### 方案 B：新增顶层 `subjects/` 或 `samples/`，把 `HelloWorldObject` 等输入提升出去

- 优点：语义清晰，天然可被多个测试阶段复用
- 缺点：需要迁移引用路径和文档

我的观点：

- 推荐
- 这是最符合你刚才“提到顶层”要求的方案

### 方案 C：新增顶层 `cases/`，让每个 case 自带 source、expected、artifacts 约束

- 优点：测试驱动味道更强，便于打包每个 case 的完整上下文
- 缺点：`case` 这个词偏测试，不够像长期 proof/sample 输入

我的观点：

- 可选
- 但命名上我更偏向 `subjects/` 或 `samples/`

当前推荐：

- 方向上选 B
- 命名上优先讨论 `subjects/` vs `samples/`

用户已确认：

- 顶层测试项目采用 `subjects/`
- 对应阶段产物统一落在 `artifacts/subjects/<subject>/...`
- 该层不只承载少量 proof sample，未来还要承载大量调试项目与验证项目

因此这层的额外要求也已经明确：

- `subjects/` 下的项目必须容易配置
- 必须适合后续持续扩展，而不是每新增一个调试项目就要求改测试框架核心代码
- 必须允许同一个 `subject` 绑定多个验证目标、多个环境 profile 和多类期望

## 决策点 2A：`subjects/` 的配置模型怎么设计

`subjects/` 一旦成为顶层长期入口，它就不能只是“放几个 csproj 进去”。

它需要回答至少这些问题：

- 这个 subject 的源项目在哪里
- 默认入口和构建方式是什么
- 它支持哪些 host / target / toolchain profile
- 它有哪些期望产物和验证边界
- 它属于 smoke、proof、debug、regression 还是 benchmark

### 方案 A：单文件 manifest

建议形态：

```text
subjects/
  HelloWorldObject/
    subject.manifest.json
    source/
```

优点：

- 上手最简单
- 新增 subject 成本低
- 读取逻辑和 registry 一样直观

缺点：

- 当一个 subject 开始承载多个 profile、多个 target、多个验证目标时，单文件很容易膨胀

我的观点：

- 适合第一版
- 但长期不够稳

用户已确认：

- `subjects/` 第一版配置模型采用方案 A
- 即每个 subject 使用单个 `subject.manifest.json` 作为唯一显式配置入口

基于这个选择，需要补一个约束：

- 虽然当前不拆 `profiles/` 目录，但 `subject.manifest.json` 的字段设计必须预留未来平滑拆分的空间
- 换句话说，单文件是当前组织形式，不应把字段设计写死到无法演进

### 方案 B：基础 manifest + profiles 目录

建议形态：

```text
subjects/
  HelloWorldObject/
    subject.manifest.json
    profiles/
      dev-default.json
      windows-x64-reference.json
      android-arm64-smoke.json
    source/
    expectations/
```

优点：

- 基础身份和环境差异分离
- 适合后续扩展大量调试项目
- 更容易表达“同一个 subject，多种验证姿势”

缺点：

- 规则会比单文件复杂一点
- 需要定义 profile 继承或合并规则

我的观点：

- 这是目前最值得选的方向
- 既能顶层统一，又不会把复杂度全塞进一份 JSON

### 方案 C：尽量少配，更多依赖约定和自动推断

做法：

- 尽量从 `csproj`、目录命名和默认 convention 推断
- subject manifest 只写少量覆盖字段

优点：

- 新建项目最快
- 对简单 debug subject 友好

缺点：

- 约定一多就会隐式化
- 对复杂平台矩阵和发布报告不友好
- 后续排障时不容易看出真实配置来源

我的观点：

- 可以作为辅助策略
- 不适合做唯一主模型

当前推荐：

- 在我个人判断里，长期主模型仍更偏向 B
- 但既然当前已确认选 A，后续实现应采用“按 A 落地、向 B 兼容”的设计
- 即：
  - `subject.manifest.json` 写稳定身份、默认源项目、默认期望、标签和环境矩阵
  - host/target/toolchain/validationMode 先以内嵌结构存在
  - 简单字段允许按 convention 自动补全，但不让关键行为依赖隐式魔法
  - 未来若 subject 数量和 profile 复杂度继续上升，再无损拆到 `profiles/`

## 决策点 3：`managed-closure` 应改成什么语义

### 方案 A：改成 `frontend-bundle`

- 强调它是 managed frontend 生成出的上游 bundle
- 与 native proof 下游边界清晰

我的观点：

- 很强
- 但要确认是否会把 `MetadataWriter + CodeGen` 之后的内容过度压成“frontend”

### 方案 B：改成 `analysis-bundle`

- 强调它是分析产物集合
- 缺点是会弱化 code registration / generated symbol 这些偏 codegen 的语义

我的观点：

- 不够准

### 方案 C：改成 `proof-input-bundle`

- 强调它是 Stage 4 proof 的唯一上游输入
- 优点是很贴近当前主线
- 缺点是名字偏下游用途，不够表达它自身的生成责任

我的观点：

- 可以作为产物名
- 不太适合作为模块名

### 方案 D：拆成两个层次

- 模块语义：`frontend-pipeline`
- 关键产物：`proof-input-bundle`

我的观点：

- 最合理
- 它同时保留“这是前端流水线生成的”与“这是后续 proof 的唯一输入”两层语义

补充解释：

- `frontend-pipeline` 说的是“这一段系统在做什么”
  - 它表示从 subject 的 .NET/Mono 输入出发，经过 Loader、SemanticWorld、Linker、MetadataWriter、CodeGen、Driver 这条 managed 前端流水线，生成一组后续可消费的中间产物
  - 它是模块责任名，不是单个文件名
- `proof-input-bundle` 说的是“这一组产物在后续链路中扮演什么角色”
  - 它表示这组产物是后续 native bootstrap、reference runtime、平台 proof 的统一输入包
  - 它是产物角色名，不是流水线职责名

举例：

- 当我们说“`frontend-pipeline` 坏了”，意思更接近：
  - Loader 没把 `HelloWorldObject.dll` 正确读出来
  - Linker retained world 不对
  - CodeGen symbol emission 不对
  - 也就是问题发生在 managed 前端生成链内部
- 当我们说“`proof-input-bundle` 不满足 Stage 4”，意思更接近：
  - Stage 3 最终交付给 Stage 4 的那组产物不完整
  - 或者字段、registration、generated metadata 对后续 native proof 不够用
  - 也就是问题表现在“交给下游的输入包”这一层

对 `HelloWorldObject` 的具体例子：

- `frontend-pipeline` 的输入：
  - `subjects/HelloWorldObject/source/...`
  - 或其对应的 dotnet/mono build 输入产物
- `frontend-pipeline` 的输出：
  - `typed-il-ir.json`
  - `aot-manifest.json`
  - `metadata-registration.json`
  - `code-registration.json`
  - 以及同类 Stage 3 前端产物
- 这组输出从“下游视角”看，就是 `proof-input-bundle`
- Stage 4 以后不应该再关心它内部是 Loader 还是 Linker 产出的，而是把它整体当成 native proof 的唯一上游输入包

用户已确认：

- 接受“模块职责名”和“关键产物角色名”分层表达的思路
- 也接受下面这条语义区分：
  - `frontend-pipeline` 表达生成责任
  - `proof-input-bundle` 表达下游 proof 输入角色

当前暂定收口为：

- 模块层替换方向：`managed-closure` -> `frontend-pipeline`
- 产物层替换方向：Stage 3 最终交付物 -> `proof-input-bundle`

备注：

- 当前确认的是语义和方向
- 若后续在正式 design 阶段需要微调最终 token 命名，可以再做最后一轮收口

当前推荐：

- 选 D

## 决策点 4：性能测试与性能报告怎么组织

### 方案 A：继续统一在一个 `perf` family

- 优点：实现简单
- 缺点：开发 perf 和 release perf report 会混在一起

我的观点：

- 不推荐

### 方案 B：拆成 `perf-dev` 和 `perf-release`

- 优点：直观
- 缺点：如果未来还有 profiling、memory、cold-start，会继续横向膨胀

我的观点：

- 可用
- 但还不够结构化

### 方案 C：保留 `perf`，但引入明确 profile

- 例如：
  - `perf` + `profile=dev`
  - `perf` + `profile=release-report`
- 优点：框架层更统一
- 缺点：文档和选择器需要更严格，否则会继续混淆

我的观点：

- 长期更好
- 但短期需要更强的 manifest 约束

当前推荐：

- 短期对用户语义暴露成 B
- 底层实现逐步往 C 靠

## 我建议的讨论顺序

为了避免一次讨论太多，我建议按这个顺序确认：

1. 第一主轴：目标维度和产物链阶段怎么组合
2. 顶层测试项目的命名与目录
3. `managed-closure` 的替换语义
4. 环境矩阵字段模型
5. 性能测试与性能报告的组织方式
6. 最后再收敛成正式 design

## 当前第一推荐组合

如果现在先给出一版我主张的总体方向，我会选：

- 测试第一主轴：`correctness.dev / correctness.platform / perf.dev / perf.release`
- 测试第二主轴：`source -> host-input -> analysis -> generated -> build -> runtime`
- 顶层测试项目：提升到顶层 `subjects/`
- `subjects/` 配置模型：单文件 `subject.manifest.json`
- `managed-closure` 替换为：
  - 模块层：`frontend-pipeline`
  - 产物层：`proof-input-bundle`
- 环境模型升级为：
  - `hostPlatform`
  - `targetPlatform`
  - `toolchainProfile`
  - `runtimeProfile`
  - `validationMode`
- 性能体系拆成：
  - dev perf
  - release perf report

这套组合的核心优点是：

- 对准你刚刚定义的四个测试目标
- 能沿着产物链定位 IL2CPP 问题
- 能自然支撑多平台和发布报告
- 能把平台适配性与平台正确性放在同一条分析线上
- 不会继续把 proof 主线藏在 `unittest` 包装后面

## 下一步

决策点 1 已确认。

决策点 2 的目录方向已确认。
决策点 2A 的配置组织形式也已确认为单文件 manifest。
决策点 3 的双层语义方向也已接受。

下一轮讨论转入环境矩阵设计，即如何把 host、target、toolchain、validation depth、平台适配性与平台正确性放进统一测试模型。

## 决策点 4：环境矩阵如何建模

用户已经明确：

- 平台适配性不应孤立讨论
- 应与多平台正确性一起纳入同一组测试问题

因此这里真正要解决的是：

- 如何描述“在哪个 host 上，针对哪个 target，用什么 toolchain/runtime，以什么验证深度，证明该平台当前可适配且结果正确”

### 方案 A：直接平铺字段

做法：

- 每个测试对象或 subject 直接写：
  - `hostPlatform`
  - `targetPlatform`
  - `toolchainProfile`
  - `runtimeProfile`
  - `validationMode`
  - `adaptationStatus`

优点：

- 最直观
- 第一版实现最快

缺点：

- 字段重复会很多
- host/target 与验证意图混在一起
- 后续平台矩阵扩大后容易失控

我的观点：

- 适合快速起草
- 不适合长期主模型

### 方案 B：`executionContext` + `validationIntent`

做法：

- 把“环境事实”和“验证意图”拆开：
  - `executionContext`
    - `hostPlatform`
    - `targetPlatform`
    - `toolchainProfile`
    - `runtimeProfile`
  - `validationIntent`
    - `validationMode`
    - `adaptationLevel`
    - `expectedOutcome`

优点：

- host/target/toolchain 和验证深度分层清楚
- 非常适合把平台适配性和平台正确性放在一起建模
- 也适合后续扩展到 perf/report

缺点：

- 比平铺字段多一层结构
- 需要额外定义枚举值

我的观点：

- 当前最推荐
- 也是最符合用户当前目标维度的方案

### 方案 C：独立平台能力注册表

做法：

- 单独维护一套 platform capability registry
- 测试对象只引用 capability id

优点：

- 长期治理能力最强
- 适合大量平台和复杂 product matrix

缺点：

- 第一版会太重
- 很容易让当前讨论从测试目标转成框架工程

我的观点：

- 可以作为远期演进
- 当前不建议直接采用

当前推荐：

- 当前先采用 B
- 先把 subject/test object 内的环境矩阵稳定下来
- 等平台规模继续扩大后，再评估是否演进到 C

### 环境矩阵的下一层待确认项

如果选择 B，后面还要继续确认：

1. `validationMode` 的枚举范围
2. `adaptationLevel` 的枚举范围
3. 一个 subject 是否允许在同一 manifest 中声明多条 environment matrix
4. 平台失败时，结果是记为“不适配”、“适配中”还是“验证失败”

## 决策点 4.1：`validationMode` 的枚举范围

### 方案 A：粗粒度

- `build`
- `run`
- `perf`
- `report`

我的观点：

- 不推荐
- 诊断粒度太粗，不足以表达平台适配深度

### 方案 B：验证证据粒度

建议枚举：

- `schema`
- `compile-only`
- `build`
- `startup`
- `output`
- `trace`
- `perf`
- `report`

含义：

- `schema`：只验证 contract/schema/snapshot
- `compile-only`：只证明目标能编译
- `build`：生成完整构建产物，但不要求启动
- `startup`：验证程序能够启动
- `output`：验证可观察业务输出
- `trace`：验证 trace 导出与比对
- `perf`：采集性能数据
- `report`：产出发布级汇总报告

我的观点：

- 当前最推荐
- 最适合把平台适配性和平台正确性放到同一条验证线上

### 方案 C：按产物阶段命名

- `analysis`
- `generated`
- `build`
- `runtime`
- `perf`
- `report`

我的观点：

- 不适合作为 `validationMode`
- 它更接近第二主轴“产物链阶段”，不够表达验证深度

用户已确认：

- `validationMode` 采用方案 B
- 当前暂定枚举为：
  - `schema`
  - `compile-only`
  - `build`
  - `startup`
  - `output`
  - `trace`
  - `perf`
  - `report`

## 决策点 4.2：`adaptationLevel` 的枚举范围

这个字段不描述“本次跑了什么”，而描述“该平台当前被适配到了什么成熟度”。

### 方案 A：最简成熟度

- `unsupported`
- `partial`
- `supported`

我的观点：

- 不推荐
- 表达粒度太粗，不足以指导后续平台推进

### 方案 B：与验证深度对齐

建议枚举：

- `unsupported`
- `compile-only`
- `buildable`
- `startup`
- `observable-output`
- `traceable`
- `perf-reportable`

含义：

- `unsupported`：当前没有正式适配
- `compile-only`：只证明该 target 能编译
- `buildable`：可生成完整构建产物
- `startup`：可成功启动
- `observable-output`：可得到正确业务输出
- `traceable`：可稳定导出 trace 并校验
- `perf-reportable`：可纳入正式性能报告

我的观点：

- 当前最推荐
- 最适合把平台适配性和平台正确性放到统一成熟度线上

### 方案 C：拆成状态和深度两个字段

- `adaptationStatus`
- `adaptationDepth`

我的观点：

- 长期可扩展性强
- 但第一版复杂度偏高

用户已确认：

- `adaptationLevel` 采用方案 B
- 当前暂定枚举为：
  - `unsupported`
  - `compile-only`
  - `buildable`
  - `startup`
  - `observable-output`
  - `traceable`
  - `perf-reportable`

## 决策点 4.3：一个 subject 是否允许声明多条 environment matrix

### 方案 A：只允许一条环境配置

我的观点：

- 不推荐
- 同一个 subject 很快就会被迫复制成多个近似项目

### 方案 B：允许多条 environment matrix

做法：

- 单个 `subject.manifest.json` 内允许声明多条组合
- 每条组合至少包含：
  - `executionContext`
  - `validationIntent`

优点：

- 同一个 subject 可以覆盖多个 host/target/toolchain/validation 组合
- 非常适合后续扩展大量调试项目
- 也适合发布前做 all-case/all-platform 汇总

缺点：

- manifest 体积会变大
- 需要更清晰的 selector 和默认行为

我的观点：

- 当前最推荐
- 也是 subject 成为长期稳定输入对象后必须具备的能力

### 方案 C：subject 只保留默认环境，其他环境放到 registry

我的观点：

- 可作为后续治理层
- 当前不适合做第一版主模型

用户已确认：

- 单个 `subject.manifest.json` 允许声明多条 environment matrix

## 决策点 4.4：`subject.manifest.json` 的最小字段集合

### 方案 A：极简字段

建议只包含：

- `subjectId`
- `sourceType`
- `sourcePath`
- `environmentMatrices`

我的观点：

- 过轻
- 很快就会补字段

### 方案 B：平衡版

建议最小字段：

- `subjectId`
- `displayName`
- `category`
- `source`
  - `type`
  - `path`
  - `entry`
- `tags`
- `expectedArtifacts`
- `environmentMatrices`
  - `matrixId`
  - `executionContext`
  - `validationIntent`

我的观点：

- 当前最合适
- 既能表达 subject 身份，也能支撑多环境矩阵

### 方案 C：增强版

在 B 的基础上继续加入：

- `expectations`
- `artifactPolicies`
- `selectors`
- `debugProfiles`
- `reportPolicies`

我的观点：

- 长期可做
- 当前太重

用户已确认：

- `subject.manifest.json` 的第一版最小字段集合采用方案 B

当前暂定最小集合：

- `subjectId`
- `displayName`
- `category`
- `source`
- `tags`
- `expectedArtifacts`
- `environmentMatrices`

## 决策点 4.5：`category` 的取值范围

这里的 `category` 用来描述 “subject 本身是什么角色”，不直接替代测试目标维度。

### 方案 A：细粒度角色枚举

- `proof`
- `smoke`
- `debug`
- `regression`
- `benchmark`

我的观点：

- 可理解
- 但后续很容易和 `tags`、`validationIntent` 重叠

### 方案 B：粗粒度类别 + tags

建议类别：

- `canonical`
- `diagnostic`
- `benchmark`

然后通过 `tags` 补充：

- `proof`
- `smoke`
- `regression`
- `object`
- `interop`
- `trace`
- `debuggable`

我的观点：

- 当前最推荐
- 最适合未来持续扩展调试项目

### 方案 C：不设 `category`，只用 `tags`

我的观点：

- 不推荐
- 缺少稳定治理维度

用户已确认：

- `category` 采用方案 B

当前暂定 `category` 取值：

- `canonical`
- `diagnostic`
- `benchmark`

## 决策点 4.6：`source.type` 第一版支持哪些输入类型

### 方案 A：只支持 `dotnet-project`

我的观点：

- 适合作为短期过渡
- 不适合作为第一版正式边界

### 方案 B：支持少量核心类型

建议第一版支持：

- `dotnet-project`
- `managed-assembly`
- `native-project`
- `source-file`

含义：

- `dotnet-project`：标准托管项目，如 `.csproj`
- `managed-assembly`：已经构建好的 `.dll` / `.exe`
- `native-project`：CMake 或同类 native 工程
- `source-file`：单文件调试输入

我的观点：

- 当前最推荐
- 既能覆盖主线，也能支撑未来扩展大量调试项目

### 方案 C：更全的类型集合

例如继续加入：

- `mono-project`
- `script`
- `archive`
- `generated-input`

我的观点：

- 可作为后续扩展
- 当前过重

用户已确认：

- `source.type` 第一版采用方案 B

当前暂定第一版支持：

- `dotnet-project`
- `managed-assembly`
- `native-project`
- `source-file`

## 决策点 4.7：`expectedArtifacts` 第一版怎么定义

这里的目的不是穷举文件名，而是先把产物链稳定分桶。

### 方案 A：直接列具体技术产物名

例如：

- `typed-il-ir`
- `aot-manifest`
- `metadata-registration`
- `code-registration`
- `native-binary`

我的观点：

- 过于贴近当前实现
- 不适合作为统一框架层的稳定抽象

### 方案 B：定义稳定的产物阶段桶

建议第一版：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

与用户目标的大致对应：

- 原项目 -> `source`
- dotnet/mono 环境产物 -> `host-input`
- IL2CPP 分析产物 -> `analysis`
- 生成产物 -> `generated`
- build 产物 -> `build`
- runtime 结果 -> `runtime`
- 发布/性能汇总 -> `report`

我的观点：

- 当前最推荐
- 正好与第二主轴“产物链阶段”对齐

### 方案 C：阶段桶 + 具体文件清单

我的观点：

- 长期可以增强
- 当前不适合做第一版最小闭环

用户已确认：

- `expectedArtifacts` 第一版采用方案 B

当前暂定第一版产物阶段桶：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

## 决策点 5：`subjects` 与现有 `suite/module/system/pipeline` 如何衔接

### 方案 A：完全替换旧模型

做法：

- 让 `subjects` 成为唯一中心模型
- 现有 `suite/module/system/pipeline` 进入退役路径
- 后续正式设计、入口、registry、文档和执行链全部围绕 `subjects` 重构

优点：

- 长期体系最干净
- 不会长期维护两套语义
- 可以直接把测试框架重心切回 subject / artifact / environment 主线

缺点：

- 重构风险最高
- 需要同时调整现有 `run test`、registry、TUI、文档和测试接入规范
- 迁移窗口内如果边界没设计清楚，很容易造成短期失稳

我的观点：

- 这不是我原先最推荐的方案
- 但如果项目当前明确希望进行架构级重整，而不是温和演化，它是成立的
- 前提是：必须给出非常清楚的 cutover 设计，不允许半替换半悬空

### 方案 B：`subjects` 成为事实源，旧对象退化为视图

我的观点：

- 这是我原先更推荐的平滑方案
- 但用户当前没有选择此方案

### 方案 C：双轨并存

我的观点：

- 不推荐
- 长期一定漂移

用户已确认：

- 决策点 5 采用方案 A
- 后续设计按“`subjects` 完全替换现有 `suite/module/system/pipeline` 主模型”推进

基于这个选择，后续设计必须额外回答：

1. `run test` 的新主语法是否直接切到 `subject`
2. 旧 `suite/module/system/pipeline` 是否保留短期兼容别名
3. registry 是否改成以 `subjects` 为唯一正式注册源
4. 文档层是否同步废弃现有测试对象分层页面，还是保留历史兼容说明

## 决策点 5.1：`run test` 的新主语法是什么

### 方案 A：显式 `subject`

建议语法：

```text
run test subject <subject-id>
run test subject <subject-id> --matrix <matrix-id>
run test subject <subject-id> --goal <goal-id>
```

优点：

- 主语义最清楚
- 完成替换后不会和旧模型混淆
- TUI、文档和 selector 都更容易统一

缺点：

- 命令稍长

我的观点：

- 当前最推荐

### 方案 B：`subject` 作为默认对象

示例：

```text
run test <subject-id>
```

我的观点：

- 过于简写
- 迁移期解析冲突风险高

### 方案 C：保留旧命令字但内部映射到 subject

我的观点：

- 不适合“完全替换旧模型”的路线

用户已确认：

- `run test` 新主语法采用方案 A

当前暂定新主语法：

```text
run test subject <subject-id>
run test subject <subject-id> --matrix <matrix-id>
run test subject <subject-id> --goal <goal-id>
```

## 决策点 5.2：旧 `suite/module/system/pipeline` 命令是否保留兼容别名

### 方案 A：立即移除旧命令

做法：

- 旧 `suite/module/system/pipeline` 相关公开命令直接退出正式系统
- 不保留兼容别名
- 后续帮助、TUI、CLI、registry、文档和脚本统一切到 `subject`

优点：

- 语义最干净
- 不会拖延切换
- 不会长期维护兼容层

缺点：

- 切换窗口风险最高
- 需要同步处理现有命令解析、测试、自举脚本、文档和用户习惯

我的观点：

- 风险很高
- 但如果明确要做硬切换，这个方案是自洽的

### 方案 B：保留短期兼容别名

我的观点：

- 这是我原先更推荐的方案
- 但用户当前没有选择

### 方案 C：永久保留别名

我的观点：

- 不推荐
- 与“完全替换旧模型”的路线冲突

用户已确认：

- 决策点 5.2 采用方案 A
- 旧 `suite/module/system/pipeline` 命令不保留兼容别名

这意味着后续设计必须把下面这些一起切掉或一起替换：

- CLI 公开语法
- TUI 菜单项
- registry 对象类型
- 测试文档入口
- 现有基于旧对象名的测试用例和脚本

## 决策点 5.3：registry 是否改成 `subjects` 为唯一正式注册源

### 方案 A：`subjects` 成为唯一正式注册源

做法：

- registry 直接扫描 `subjects/**/subject.manifest.json`
- 不再保留现有 `tests/registry/modules`、`tests/registry/system`、`tests/registry/pipelines` 作为正式注册源
- 可执行对象统一从：
  - `subject`
  - `matrix`
  - `goal`
  组合推导

优点：

- 与“完全替换旧模型”完全一致
- 语义最统一
- 不会继续维护旧 registry 体系

缺点：

- 需要重写当前 registry 扫描、对象索引和 TUI 浏览逻辑

我的观点：

- 在当前已确认的路线下，这是最合理的选择

### 方案 B：保留独立 registry，但迁到新路径

我的观点：

- 可作为增强层
- 不适合当前硬切换路线

### 方案 C：完全取消显式 registry

我的观点：

- 过轻
- 不利于长期治理和缓存

用户已确认：

- 决策点 5.3 采用方案 A
- `subjects` 成为唯一正式注册源
