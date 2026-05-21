# IL2CPP 主线 roadmap 骨架 v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 目标

把当前仓库已经建立的 `proof-first` 能力，继续收敛成一条面向长期推进的 IL2CPP 主线。

这条主线的最终目标不是只证明 `HelloWorldObject` 或单个 Windows proof，而是逐步形成：

- 面向真实程序集的 `C# -> IL/metadata -> retained world -> generated C++ -> native runtime` 稳定流水线
- 对 `.NET 8` 与 `.NET 10` 输入的明确兼容路径
- 对 `Windows / macOS / Android / iOS` 的分层适配路径
- 面向“嵌入自研引擎并向 C# 暴露稳定 API”的 contract 化接入路径
- 可持续扩张的 `subject + matrix + artifact-chain` 验证模型

## 2. 当前已知现状

从现有仓库资产看，当前起点不是“从零开始”，而是“已有 proof spine，需要扩成主线”。

已经成立的部分：

- `src/managed/Chaos.IL2CPP.*` 已建立 managed 侧分层骨架：
  - `Contracts`
  - `Driver`
  - `Loader`
  - `SemanticWorld`
  - `Pipeline`
  - `Linker`
  - `MetadataWriter`
  - `CodeGen`
- `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 已形成可执行的最小 managed pipeline。
- Stage 3 `Managed Minimal Closure` 已完成归档。
- Stage 4 `Native Bootstrap And Reference Proof` 已完成 Windows reference proof。
- Stage 5A / 5B / 5C 已完成 `GenericEcho -> ReflectionLite -> PInvokeLite` 的 Windows 主线 follow-on。
- Stage 5D 已冻结首轮 `macOS` parity handoff 计划。

仍然明显未完成的部分：

- managed 工具链当前仍只固定在 `net8.0`，尚未形成 `.NET 10` 输入兼容面。
- `macOS` parity 还停留在 handoff / plan，不是已完成的真实宿主闭环。
- Android / iOS 当前仍主要是 gate / packaging / routing 级，不是完整移动 runtime 语义支持。
- 当前 proof/subject 体系与历史 `roadmap-0` 迁移资产仍处在并存阶段，长期主线还没有完全统一。

## 3. 主线判断

当前最重要的判断不是“下一步先做 Android 还是 iOS”，而是先把主线拆成四条不同层次的问题：

- 核心语义层是否稳定
- 输入框架层是否兼容 `.NET 8` / `.NET 10`
- 参考桌面平台是否形成双 reference baseline
- 移动平台是否从 packaging/buildable 进化到 runtime-correctness

因此推荐的总顺序不是：

- `.NET 8/.NET 10 -> Android/iOS`

而是：

- `核心层恢复/扩张 -> 双 TFM 输入兼容 -> 双桌面 reference -> Android -> iOS -> 收口`

原因很直接：

- 如果把 `.NET 10`、`macOS`、Android、iOS 一起推进，失败点会同时来自 IL 形状、BCL 差异、toolchain、ABI、打包和签名，后续很难定位。
- 当前仓库最成熟的资产仍然是 Windows proof spine，因此应该先沿着它把“核心层”补强，再逐步把平台维度扩出去。

## 4. Roadmap 骨架

## 4.1 Phase A：核心层稳定化

- `goal`
  - 把当前 proof-first 流水线真正收敛成样例无关的核心管线。
- `核心工作`
  - 审视 `Loader / SemanticWorld / Linker / MetadataWriter / CodeGen` 的职责边界。
  - 清掉还残留在 `src/**` 内的 sample-driven 假设。
  - 固定 core contracts：输入程序集、typed IL IR、AOT manifest、metadata registration、code registration、lowering plan。
  - 明确 runtime-core / bootstrap / support / engine binding 与 managed pipeline 的分界。
- `exit_criteria`
  - `src/**` 内不依赖具体测试工程名。
  - 现有 `HelloWorldObject / GenericEcho / ReflectionLite / PInvokeLite` proof 仍然成立。
  - 关键 artifact contract 已固定为长期主线可复用的对象，不再只是 proof 临时产物。
- `deliverables`
  - 核心层职责审计结论
  - contract freeze 补充文档
  - proof spine 回归基线
- `open_questions`
  - 当前 `SemanticWorld` 还比较薄，什么时候开始真正承接语义归一化而不是简单转抄？
  - `Linker` 的 retained world 何时从 proof-required subset 扩到更通用子集？

## 4.2 Phase B：`.NET 8` 核心语义扩张

- `goal`
  - 在不引入新平台变量的前提下，先把核心 C# 语义子集打实。
- `核心工作`
  - 继续扩张 Windows reference subjects，而不是先扩平台。
  - 在现有 `HelloWorldObject / GenericEcho / ReflectionLite / PInvokeLite` 之外，补第一批核心语义 subject：
    - exceptions
    - delegates
    - interface dispatch
    - virtual dispatch
    - boxing / unboxing
    - arrays
    - generics 扩展子集
  - 为每类语义建立“loader 支持点 / world 语义 / metadata 需求 / codegen 需求 / runtime helper 需求”的映射。
- `exit_criteria`
  - 至少形成一批可持续回归的 `.NET 8` 核心语义 subject 集。
  - 每类失败能明确定位到 `source / host-input / analysis / generated / build / runtime` 某一层。
  - 不再把“新增一个语义样例”等同于“临时写一块样例特化逻辑”。
- `deliverables`
  - `.NET 8` 核心语义支持矩阵
  - 新增 subject 及其 matrix/gate
  - 对应 runtime/codegen/helper 缺口清单
- `open_questions`
  - 泛型支持优先做到 closed generic，还是尽快讨论 generic sharing？
  - exception / interface / delegate 三者谁应该先进入第一批？

## 4.3 Phase C：`.NET 8 / .NET 10` 双 TFM 输入兼容

- `goal`
  - 让同一条 IL2CPP 主线能稳定处理 `.NET 8` 与 `.NET 10` 输入，不复制两套 pipeline。
- `核心工作`
  - 明确“支持 `.NET 10`”的真实边界：
    - 是 managed 工具自己升到 `net10.0`
    - 还是优先支持消费 `net10.0` 编译出的程序集
  - 为同一批 subject 建立 `net8.0 + net10.0` 双输入矩阵。
  - 识别 `MetadataReader`、BCL surface、编译器生成模式、属性/metadata 差异带来的增量兼容点。
  - 把 `.NET 10` 差异写成输入兼容 contract，而不是散落在 bugfix 里。
- `exit_criteria`
  - 同一批核心 subject 至少有一组 `net8.0` 与 `net10.0` 的对照运行。
  - 已明确哪些差异属于输入兼容问题，哪些属于 runtime/BCL 语义问题。
  - `.NET 10` 没有被实现成一条单独分叉主线。
- `deliverables`
  - 双 TFM subject matrix
  - `.NET 10` 输入兼容差异清单
  - 对应 contract / loader / codegen / runtime 增量计划
- `open_questions`
  - `.NET 10` 兼容是否先只要求“输入可分析”，还是要求直接进入 native proof？
  - 若后续 managed 工具自身升级 TFM，何时做、为什么做？

## 4.4 Phase D：`macOS` Reference Parity

- `goal`
  - 把当前已经冻结的 `macOS` parity handoff 真正落成第二参考桌面平台。
- `核心工作`
  - 先按 Stage 5D 已冻结的边界推进首轮 parity 子集：
    - Stage 4 `HelloWorldObject`
    - Stage 5A `GenericEchoClosedMinimal`
    - Stage 5B `ReflectionLiteQueryMinimal`
  - 明确 `macOS` host build/run glue、preset、trace、gate。
  - 验证现有 sample-agnostic runtime/bootstrap/codegen 边界在 `macOS` 上是否仍成立。
- `exit_criteria`
  - `macOS` 形成真实 reference host 闭环，而不是停留在计划。
  - Stage 4 + 5A + 5B 具备首轮 parity 证据。
  - 当前 Windows-only 的 Stage 5C interop 仍明确隔离，不伪装为跨宿主 contract。
- `deliverables`
  - `macOS` reference proof/gate
  - parity 子集的 build/run/trace 证据
  - `macOS` 相对 Windows 的真实差异清单
- `open_questions`
  - `macOS` parity 是否只要求 output proof，还是一开始就要 trace parity？
  - `System.Console`、字符串、反射 query 等 helper 在 `macOS` 上需要哪些宿主胶水调整？

## 4.5 Phase E：Android Runtime Subset

- `goal`
  - 把 Android 从当前的 startup/buildable gate，提升到真实 runtime subset。
- `核心工作`
  - 先只复用已经在 Windows/macOS reference 上成立的语义子集。
  - 从 `startup smoke` 过渡到最小 runtime output proof。
  - 明确 Android 上的 toolchain、loader packaging、entrypoint、runtime helper、trace/日志收集路径。
  - 把 Android 的问题单独建模为 matrix 能力，而不是继续挂在历史 `roadmap-0` gate 名称下。
- `exit_criteria`
  - Android 至少对一批最小 subject 具备真实 runtime 证据，而不只是 preset/gate 通过。
  - 已区分“可构建”“可启动”“输出正确”“trace 正确”四种成熟度。
- `deliverables`
  - Android runtime subset matrix
  - Android host/target glue 与打包入口
  - Android 专属失败分层模型
- `open_questions`
  - Android 首轮是否要求真机/模拟器一致，还是先固定一种执行环境？
  - trace/diagnostic 在 Android 上的最小证据面要到什么程度？

## 4.6 Phase F：iOS Runtime Subset

- `goal`
  - 把 iOS 从 compile/link/packaging，提升到真实 runtime subset。
- `核心工作`
  - 继续只复用已经在 reference desktop 成立的语义子集。
  - 明确 iOS 的静态链接、签名、打包、运行拉起和产物采集边界。
  - 把 iOS AOT、裁剪、反射保留和 interop 限制写成正式 contract。
- `exit_criteria`
  - iOS 不再只是 packaging gate，而是至少有一批最小 runtime subject。
  - iOS 的限制项被写进支持矩阵，而不是以“偶发构建脚本特例”存在。
- `deliverables`
  - iOS runtime subset matrix
  - iOS packaging/run/observe glue
  - iOS AOT/trimming/restriction contract
- `open_questions`
  - iOS 首轮是否只要求 device，还是允许 simulator 作为第一步？
  - 反射、动态加载、interop 在 iOS 上的边界是否需要单列 capability matrix？

## 4.7 Phase G：主线收口与工程化

- `goal`
  - 把前面各阶段沉淀成长期可维护主线，而不是一组 proof/gate 的堆积。
- `核心工作`
  - 完成 `subject + matrix + artifact-chain` 的正式收口。
  - 把历史 `roadmap-0` 彻底降级为迁移来源，而不是长期一等对象。
  - 统一报告、watch、TUI、CI、perf baseline、stage reuse/invalidation。
  - 明确 release/perf 维度与 dev/correctness 维度的关系。
- `exit_criteria`
  - 新主线能够同时承载：
    - 核心语义扩张
    - 双 TFM 输入兼容
    - 双 reference desktop
    - 移动 runtime subset
  - 长期入口不再依赖历史聚合式 workflow 思维。
- `deliverables`
  - 正式 subject framework cutover
  - 统一 reporting / event / summary 契约
  - CI/perf/回归矩阵
- `open_questions`
  - perf 应该在哪个阶段切入，才不会过早干扰 correctness 主线？
  - 何时可以宣布历史 `roadmap-0` 只剩兼容壳层，不再承担正式职责？

## 5. 当前建议推进顺序

从当前仓库状态继续推进时，推荐顺序为：

1. `Phase A`：核心层稳定化
2. `Phase B`：`.NET 8` 核心语义扩张
3. `Phase C`：`.NET 8 / .NET 10` 双 TFM 输入兼容
4. `Phase D`：`macOS` reference parity
5. `Phase E`：Android runtime subset
6. `Phase F`：iOS runtime subset
7. `Phase G`：主线收口与工程化

这个顺序的核心原则是：

- 先稳定“语言/语义层”
- 再稳定“输入框架层”
- 再做“第二参考桌面平台”
- 最后才进入移动平台 runtime

## 6. 非目标

这条主线当前明确不追求：

- 一口气做完整 BCL 兼容
- 一轮内同时完成 `.NET 10`、`macOS`、Android、iOS 全量闭环
- 把 Android / iOS 的 buildable/packaging gate 直接包装成“已支持移动平台运行时”
- 为了追进度把样例特化逻辑写回 `src/**`
- 继续让历史 `roadmap-0` 作为正式主模型

## 7. 关键约束

后续无论讨论哪一阶段，都不应回退以下约束：

- `src/**` 必须保持 sample-agnostic。
- 新主线应围绕 `subject + matrix + artifact-chain` 建模，而不是围绕宿主工作流建模。
- proof/gate 需要继续保留，但它们应服务于阶段定位，不应重新演变成“总脚本入口”。
- 移动平台适配性验证与移动平台正确性验证必须明确分层。
- `.NET 10` 兼容应优先建模为“输入兼容 contract”，不是默认复制第二套 pipeline。
- 自研引擎对 C# 的暴露层必须与 `managed external dependency`、通用 `runtime helper`、`host/platform binding` 分层，而不是混成泛化 interop。

## 8. 当前最值得继续细化的议题

如果按“最能影响后续顺序”的优先级排序，下一轮最值得继续细化的是：

1. `Phase A / Phase B` 的边界
   - 哪些问题属于“核心层还没收口”
   - 哪些问题已经可以算“语义扩张”
2. `.NET 10` 的真实支持定义
   - 是先支持输入
   - 还是直接要求 proof parity
3. `macOS` parity 的第一轮执行策略
   - output-only
   - 还是 output + trace
4. Android 与 iOS 的“runtime subset”最小完成定义
   - 哪些能力算“已经进入 runtime”
   - 哪些还只是 packaging/buildable
5. 自研引擎嵌入与 API 暴露边界
   - 哪些能力属于 `engine binding`
   - 哪些仍属于通用 `runtime role` 或 `host binding`

## 9. 作为后续讨论基线的使用方式

本文件当前只承担“主线骨架讨论稿”的角色。

后续讨论建议按下面方式继续沉淀：

- 如果是对整条主线的阶段、顺序、优先级再讨论，继续增量修订本文件。
- 如果进入某个具体专题，例如：
  - `.NET 10` 输入兼容
  - `macOS` parity 执行边界
  - Android runtime subset 完成定义
  - iOS AOT/trimming 限制
  - engine binding lowering-plan contract
  - engine proof subject/matrix/artifact-chain
  - engine subject manifest/execution-pipeline draft
  - engine artifact evidence/report contract
  - engine summary consumer boundary
  则在 `docs/discuss/` 新开专题文档，再回链到本骨架。
- 当某个阶段的边界和方案已经稳定，应把结论迁移到 `docs/architecture/` 或 `docs/dev/` 的正式 roadmap / plan 文档。

## 10. 当前建议

从“继续讨论并不断补细节”的角度看，当前最合理的下一步不是马上开做移动平台，而是先把下面两件事讨论实：

- `Phase A` 到 `Phase C` 的主线边界
- `.NET 10` 与 `macOS` parity 谁先落真实执行、为什么

这两件事一旦讲清楚，后续 Android / iOS 的顺序和完成定义就会更稳，不容易反复回滚。
