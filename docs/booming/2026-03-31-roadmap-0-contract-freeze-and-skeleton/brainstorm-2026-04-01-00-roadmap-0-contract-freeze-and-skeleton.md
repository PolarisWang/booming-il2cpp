# Roadmap 0 brainstorm 讨论记录

**日期：** 2026-04-01 00:31:16 +08:00

**状态：** 进行中，正在把既有详细计划改回 brainstorm，并逐条确认实现方案与细节

## 用户原始请求

- 历史请求：用户希望基于现有 `plan-2026-03-31-23-il2cpp-like-cs-to-cpp-implementation-roadmap.md` 中的 `Roadmap 0`，单独做一份“全新的、详细的规划”，用于指导该阶段的实际落地。
- 当前请求：把 `plan-discuss-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md` 改为 brainstorm，然后一条条对实现方案和细节。

## 初始需求

- 不直接把现有详细计划视为已确认方案。
- 把现有详细计划降级为“候选方案草案”，作为讨论起点而不是执行起点。
- 逐条确认 `Roadmap 0` 的实现边界、交付物、目录布局、契约拆分、移动端验证层级和本地校验入口。
- 保留已有路线图、发现记录和详细计划中的有效上下文，避免重复分析。

## 已知上下文

- 当前仓库几乎没有实现代码，现有核心内容主要是 `docs/booming/...` 下的设计、路线图、规划和发现记录。
- 项目总路线图已将整体工作拆为 `Roadmap 0` 到 `Roadmap 6`，其中 `Roadmap 0` 的定位是“契约冻结与工程骨架”。
- `Roadmap 0` 当前被理解为优先冻结：
  - 仓库骨架
  - 契约产物格式
  - Runtime ABI v0
  - Codegen Bridge API v0
  - Preserve/descriptor 格式
  - Smoke 输入资产
  - Android/iOS 早期验证矩阵
- 现有 `plan-discuss` 已经形成一份偏执行导向的详细计划，并默认把很多实现取舍提前定了下来。
- 用户当前要求把流程退回 brainstorm，说明这些取舍仍需要逐条和用户对齐，而不是直接进入实现计划。

## 相关输入材料

- 总路线图：`docs/booming/2026-03-30-roadmap/plan-2026-03-31-23-il2cpp-like-cs-to-cpp-implementation-roadmap.md`
- 现有 Roadmap 0 详细计划：`docs/booming/2026-03-29-il2cpp-like-cs-to-cpp/plan-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md`
- 现有讨论记录：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/plan-discuss-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md`
- 现有发现记录：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/finding-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md`
- 最近提交：`f0fe780 talk about roadmap`、`5f19b05 add design docs`、`89d3e1a first commit`

## 探索范围

- 只讨论 `Roadmap 0`
- 不提前实现 `Roadmap 1`、`Roadmap 2`、`Roadmap 3`
- 不把“候选目录结构”“候选 schema 名称”“候选 smoke 集合”直接当成定案
- 重点对齐以下问题：
  - `Roadmap 0` 到底冻结哪些契约，哪些应明确延后
  - 目录骨架是一次性铺满，还是按最小闭环先落一部分
  - schema / ABI / bridge / preserve 的边界如何切
  - Android/iOS 在本阶段的目标到底止于 compile-only，还是需要 startup smoke
  - 本地验证入口需要多严格，哪些失败允许 `skip with reason`

## 当前未知项

1. `Roadmap 0` 是否应该一次性创建 `frontend/`、`analysis/`、`runtime/`、`codegen/`、`build/`、`tests/` 六个顶层目录，还是只创建当前阶段真正承载契约和验证的最小子集。
2. `metadata-graph` 与 `typed-il-ir` 是否属于 `Roadmap 0` 必须冻结的 contract pack，还是应该在 `Roadmap 1` 随实现一起收敛。
3. Runtime ABI v0 与 Codegen Bridge v0 应该冻结到什么粒度：文档级、头文件级，还是必须带 compile-only smoke。
4. Android/iOS 在 `Roadmap 0` 里是否需要要求“可配置并可启动 smoke 骨架”，还是只要求 toolchain 描述与 compile target。
5. smoke 输入项目是否需要一开始就放 `HelloWorld`、`GenericEcho`、`ReflectionLite`、`PInvokeLite`、`HostEmbeddingLite` 五个，还是先收缩成更小集合。
6. `verify-roadmap-0` 是否应该在一开始就覆盖 schema、ABI、bridge、smoke 输入、Android/iOS probe 全部主线，还是先只保 schema 与 desktop smoke。

## 可复用的候选结论

以下内容目前视为“可复用候选结论”，不是最终定案：

- `Roadmap 0` 的核心不是堆空目录，而是把后续 `Roadmap 1-4` 共用的边界正式化。
- 当前仓库几乎没有实现代码，因此本阶段更应该产出“契约 + 骨架 + 自动化校验入口”，而不是提前做真实运行时或真实翻译器。
- Unity IL2CPP 带来的主要启发更接近“注册 + 元数据缓存 + codegen helper”三件套，而不是单独某种 IL 到 C++ 的 lowering 算法。
- Android/iOS 应尽早纳入验证矩阵，但当前阶段未必需要完整语义验证。
- 现有详细计划里的 5 个 chunk 可以继续作为讨论顺序候选，但每个 chunk 的内容、粒度和优先级都需要重新确认。

## 当前已确认事项

1. `Roadmap 0` 的冻结范围采用 `A 大包版` 作为讨论基线：
   - 目录骨架
   - schema pack
   - runtime ABI / codegen bridge
   - 5 个 smoke 输入
   - Android/iOS startup smoke 骨架

说明：
- 这里的“已确认”表示后续讨论默认不再回到“做大包还是中包/小包”的范围层面。
- 这不等于大包版里的每个具体实现细节都已定稿，后续仍需逐条确认。

2. `schema pack` 里的 `metadata-graph` 与 `typed-il-ir` 采用 `A2 最小冻结`：
   - `Roadmap 0` 冻结根对象、版本、核心集合和扩展点
   - 不在 `Roadmap 0` 里把方法级 IR 细字段和完整 metadata 层级一次性锁死

说明：
- 这意味着 `schema pack` 仍属于 `Roadmap 0` 正式交付物。
- 但其目标是“建立稳定外轮廓”，而不是“替 `Roadmap 1` 提前完成详细建模”。

3. `runtime ABI / codegen bridge` 采用 `B2 头文件级冻结`：
   - 在 `Roadmap 0` 中落正式 header
   - 配套 compile-only smoke
   - 不要求在本阶段提供真实实现或完整 stub 调用链

说明：
- `ABI/bridge` 在本阶段不是纯文档占位。
- 但也不把 `Roadmap 0` 推进到 runtime/codegen 的半实现状态。

4. Android/iOS startup smoke 骨架采用 `C3 硬骨架`：
   - `Roadmap 0` 目标中包含真实启动 smoke，而不只是 toolchain/probe/README 级骨架
   - 移动端部分不再只满足“可配置”或“可编译”，而要纳入真实启动闭环

说明：
- 这会显著提高 `Roadmap 0` 对移动端宿主形态、入口设计、打包与启动命令清晰度的要求。
- 具体 gate 仍需继续确认，例如是否要求 Android/iOS 两个平台都成为硬性启动门槛。

5. 移动端 gate 采用 `D2 单平台先硬门槛`：
   - Android 和 iOS 中至少一个平台必须在 `Roadmap 0` 完成真实 startup smoke
   - 另一个平台至少完成 compile / link / packaging 闭环
   - 第二个平台的真实 startup smoke 不作为当前阶段唯一阻塞项

说明：
- 这保留了移动端“必须有真启动”的要求。
- 同时避免 `Roadmap 0` 被双平台环境、设备或宿主细节完全锁死。

6. 首个真实 startup smoke 平台采用 `E1 Android 优先`：
   - Android 作为 `Roadmap 0` 里优先打通真实 startup smoke 的移动端平台
   - iOS 在本阶段至少完成 compile / link / packaging 闭环

说明：
- 这会让移动端主线优先围绕 Android harness、toolchain、包体结构和启动命令展开。
- iOS 仍在 `Roadmap 0` 范围内，但不作为第一个必须打通真实启动的目标。

7. 五个 smoke 输入项目采用 `F1 五个都是真项目`：
   - `HelloWorld`
   - `GenericEcho`
   - `ReflectionLite`
   - `PInvokeLite`
   - `HostEmbeddingLite`
   都需要在 `Roadmap 0` 中具备真实 `.csproj` 和可执行 `dotnet build`

说明：
- 这意味着 `Roadmap 0` 对输入资产的要求不是占位样例，而是后续可直接复用的真实托管输入。
- `PInvokeLite` 与 `HostEmbeddingLite` 也需要在本阶段形成可构建的最小项目，而不是只写说明。

8. `verify-roadmap-0` 采用 `G1 全量硬校验`：
   - 脚本默认检查 schema、ABI smoke、bridge smoke、五个 smoke 输入、Android 真启动、iOS compile/link/packaging
   - 任何一项不过都算失败

说明：
- 这意味着 `verify-roadmap-0` 不是分层自检脚本，而是 `Roadmap 0` 的统一硬门槛。
- 后续仍需继续确认：在“当前机器缺少移动端环境”这种现实前提下，脚本失败是允许的、需要切换执行环境，还是需要额外定义远端/CI gate。

9. `verify-roadmap-0` 的运行位置采用 `H1 本地机硬门槛`：
   - 每台开发机都必须能够跑完整 `verify-roadmap-0`
   - 不接受把移动端校验仅下放给参考环境、远端环境或 CI

说明：
- 结合前面已确认的 `G1`、`D2`、`E1`，这意味着每台开发机至少都需要满足：
  - desktop contract 主线
  - Android 真实 startup smoke
  - iOS compile / link / packaging
- 这对开发机操作系统、工具链、设备/模拟器可达性和开发流程会带来很强约束，后续必须把这条约束进一步具体化。

10. 开发机操作系统基线采用 `I2 允许 Windows/Linux 开发机`：
   - 不要求所有核心开发机统一为 macOS
   - 允许 Windows/Linux 作为日常开发环境

说明：
- 这与 `H1 本地机硬门槛` 组合后，意味着“本地机完成完整 verify”不能简单理解为“所有步骤都在同一台机器本机原生完成”。
- 后续必须继续明确：iOS compile / link / packaging 在非 macOS 开发机上，是通过远程 Mac builder、共享构建节点，还是别的本地触发机制来达成。

11. iOS gate 实现方式当前选择 `J3 不使用远程 Mac，并把 iOS gate 降成文档/打包描述级`

说明：
- 这个选择与前面已确认的若干事项存在直接冲突：
  - `G1 全量硬校验`
  - `H1 本地机硬门槛`
  - `D2` 中对 iOS 至少要求 compile / link / packaging 闭环
- 因此 `J3` 当前只能作为“触发重对齐的选择”，不能直接与前述结论同时成立。
- 下一步必须明确：是保留 `J3` 并回退前面的硬门槛，还是撤回 `J3`。

12. verify host 矩阵采用“按开发机操作系统分平台子矩阵跑全量 verify”：
   - Windows 开发机：完成 `Windows / Android / Linux` 的完整 verify
   - macOS 开发机：完成 `iOS / macOS / Linux` 的全量 verify

说明：
- 这条新结论用于替代此前的 `H1 本地机硬门槛` 的朴素理解，即“每台开发机都跑完全相同的一套全量 verify”。
- 新语义变为：每台开发机都必须本地跑完一套“与其 host OS 相匹配的全量 verify 子矩阵”。
- 这也意味着：
  - iOS gate 不再要求 Windows/Linux 开发机本地承担
  - Android 真启动主线主要落在 Windows 开发机
  - Linux target 被同时放入 Windows/macOS 的 verify 子矩阵，后续仍需继续明确它是通过交叉编译、WSL/container、还是原生 runner 方式成立
- 因此，`J3` 造成的核心冲突已经被这条 host-specific verify 规则部分消解。

13. Linux verify 采用 `L2 cross-compile + packaging gate`：
   - Windows/macOS host 子矩阵中的 Linux 项只要求 cross-compile 和 packaging
   - `Roadmap 0` 不要求真实 Linux runtime smoke

说明：
- 这意味着 Linux 在 `Roadmap 0` 中更接近“目标平台产物与构建链路验证”，而不是“运行时语义或启动验证”。
- 结合前面的平台选择，当前各平台在 `Roadmap 0` 的验证强度大致为：
  - Windows：桌面主线 + 本地 verify
  - Android：真实 startup smoke
  - Linux：cross-compile + packaging
  - macOS：mac host 子矩阵主线
  - iOS：mac host 上的 compile/link/packaging

14. 仓库骨架采用 `M1 全骨架 upfront`：
   - 现在就创建 `frontend/`、`analysis/`、`runtime/`、`codegen/`、`build/`、`tests/` 等顶层目录
   - 每个顶层目录在 `Roadmap 0` 中都要有明确边界说明和落盘结构

说明：
- 这意味着 `Roadmap 0` 不只是在文档里约定模块边界，而是把顶层工程骨架真实落到仓库中。
- 后续仍需继续明确：这些目录下面哪些子目录现在就创建，哪些只停留在 README/ownership 级说明。

15. 需要预留单独的第三方依赖目录：
   - 后续会加入一些第三方库
   - 需要有独立目录承载第三方代码，而不是混放进自研模块目录

说明：
- 这个目录需要在 `Roadmap 0` 骨架阶段就被纳入顶层布局设计。
- 后续仍需继续确认目录命名、引入方式和治理策略，例如是 `third_party/`、`external/` 还是其它命名。

16. 第三方依赖目录命名采用 `N1 third_party/`：
   - 顶层使用 `third_party/` 作为第三方源码/依赖承载目录

说明：
- 这让第三方源码与自研目录的边界保持最直观的表达。
- 后续仍需继续确认：哪些内容允许进入 `third_party/`，以及工具链/下载产物是否单独放在别的目录。

17. `third_party/` 采用 `O1 只放 vendored 源码`：
   - `third_party/` 只承载第三方源码
   - 工具链、下载缓存、构建产物、临时文件不放入 `third_party/`

说明：
- 这会让 `third_party/` 的职责更清晰，便于后续做许可证、升级和源码扫描边界。
- 后续仍需继续确认：第三方源码是以手动 vendor、submodule、subtree 还是其它机制接入。

18. 第三方源码接入方式采用 `P1 手动 vendor snapshot`：
   - 以固定版本源码快照的形式放入 `third_party/`
   - 不使用 submodule 或 subtree 作为默认接入机制

说明：
- 这会降低日常拉取、初始化和 CI 的复杂度。
- 后续仍需继续确认：每个第三方依赖的来源、版本、补丁状态和升级流程以什么元数据形式记录。

19. 第三方依赖元数据记录采用 `Q3 双层记录`：
   - 每个依赖目录内保留本地说明文件
   - 仓库根或顶层目录额外保留一份总索引

说明：
- 这让“信息靠近源码”和“全局可扫描总览”同时成立。
- 后续仍需继续确认：目录内说明文件和根级总索引分别采用什么具体文件名与字段结构。

20. 第三方依赖双层记录的文件形态采用 `R1 文档型`：
   - 每个依赖目录放 `README.md`
   - 根级总索引放 `third_party/INDEX.md`

说明：
- 这意味着当前优先选择“人可读、易维护”的依赖记录方式，而不是先上机器可扫格式。
- 后续仍需继续确认：目录内 `README.md` 与根级 `INDEX.md` 的最小字段集合和更新规则。

21. 第三方依赖目录内 `README.md` 采用 `S3 扩展版字段集合`：
   - 来源
   - 版本 / commit
   - 许可证
   - 引入原因
   - 局部修改说明
   - 升级注意事项
   - 构建接入方式
   - 包含路径
   - 平台限制

说明：
- 这意味着第三方依赖 README 不只是许可证说明，还承担构建接入与平台约束的本地文档职责。
- 后续仍需继续确认：根级 `third_party/INDEX.md` 是保留精简汇总，还是也同步大部分字段。

22. 根级 `third_party/INDEX.md` 采用 `T1 精简总表`：
   - 名称
   - 版本 / commit
   - 许可证
   - 用途
   - 目录路径

说明：
- 这意味着根级索引承担“导航与总览”职责，而不是重复每个依赖目录 README 的详细内容。
- 详细构建接入、包含路径、平台限制、局部补丁等信息仍以各依赖目录 `README.md` 为准。

23. 工具链与下载缓存边界采用 `U1`：
   - 工具链描述保留在 `build/toolchains/`
   - 实际下载缓存和二进制依赖全部放仓库外

说明：
- 这意味着仓库内只保留“如何配置/定位工具链”的描述与脚本，不直接承载大体积下载物或二进制依赖缓存。
- 后续仍需继续确认：构建产物、临时输出和本地 verify 中间文件的目录策略。

24. 构建产物与 verify 中间输出采用 `V2 artifacts/`：
   - 仓库内统一使用 `artifacts/` 承载构建产物与 verify 中间输出

说明：
- 这意味着当前更偏向“发布/产物导向”的命名，而不是通用临时输出命名。
- 后续仍需继续确认：`artifacts/` 是否同时容纳 CMake configure/build 目录、日志、snapshot copy，还是需要进一步分层。

25. `artifacts/` 职责边界采用 `W1 全收口`：
   - CMake build 目录
   - 日志
   - 快照副本
   - 打包产物
   都统一放入 `artifacts/`

说明：
- 这意味着 `artifacts/` 被定义为仓库内统一的产物与运行输出收口目录。
- 后续仍需继续确认：版本化快照的“权威来源”是否也直接放在 `artifacts/`，还是仅把运行/生成副本放进去而保留源码树中的 canonical 文件。

26. 快照策略采用 `X2`：
   - 权威快照保留在源码树中
   - `artifacts/` 只放 verify 运行时生成的快照副本和 diff 输出

说明：
- 这意味着 `artifacts/` 虽然统一收口运行输出，但不接管版本管理中的 canonical snapshot。
- 后续仍需继续确认：schema/example/snapshot 在源码树中的正式目录布局是否沿用现有候选结构。

27. contract 资产目录布局采用 `Y1`：
   - `analysis/contracts/schemas/` 放 schema
   - `analysis/contracts/examples/` 放 example
   - `tests/contracts/schema/` 放权威 snapshot

说明：
- 这明确区分了 contract 资产与测试基线的职责边界。
- 后续仍需继续确认：registration schema、preserve schema、ABI smoke、bridge smoke 的目录布局是否也沿用同样的分层思路。

## 本轮策略

- 先把现有详细计划视为候选方案 A。
- 后续每轮只讨论一个关键问题。
- 每轮都记录：问题、用户回答、为什么改变或保留现有方案。
- 当关键问题收敛后，再整理成新的设计或实现计划。

## 讨论日志

- 2026-04-01 00:31:16 +08:00：记录用户当前请求，要求把现有 `plan-discuss` 改回 brainstorm，并逐条对齐实现方案和细节。
- 2026-04-01 00:31:16 +08:00：复查了总路线图、现有 Roadmap 0 详细计划、现有 `plan-discuss` 与 `finding`，确认已有上下文足够作为 brainstorm 起点。
- 2026-04-01 00:31:16 +08:00：确认当前关键变化不是“补更多计划细节”，而是把此前过早收敛的执行型计划退回到交互式设计确认流程。
- 2026-04-01 00:31:16 +08:00：决定保留原 `plan-discuss` 作为历史记录，不覆盖；新建 `brainstorm-2026-04-01-00-roadmap-0-contract-freeze-and-skeleton.md` 作为后续主记录。
- 2026-04-01 00:33:49 +08:00：提出第 1 个待确认问题，优先确认 `Roadmap 0` 的冻结范围应采用“大包版”“中包版”还是“小包版”，以决定后续目录、contract pack、移动端目标和验证入口的收敛边界。
- 2026-04-01 00:36:26 +08:00：用户追问“什么是 schema pack, ABI/bridge”，说明在继续讨论 `Roadmap 0` 冻结范围之前，需要先统一这些术语在本项目语境中的含义。
- 2026-04-01 00:40:21 +08:00：用户明确选择 `A 大包版`，即基本沿用现有详细计划，把目录骨架、schema pack、ABI/bridge、5 个 smoke 输入、Android/iOS startup smoke 骨架都放进 `Roadmap 0`。
- 2026-04-01 00:40:21 +08:00：据此确认后续讨论基线为“大包版范围内逐项定细节”，而不是继续讨论 `Roadmap 0` 做大还是做小。
- 2026-04-01 00:42:22 +08:00：针对 `schema pack` 的冻结粒度，用户选择 `A2 最小冻结`。
- 2026-04-01 00:42:22 +08:00：据此确认 `metadata-graph` 与 `typed-il-ir` 在 `Roadmap 0` 中只冻结最小稳定骨架，不提前锁死详细字段层级，详细建模留给 `Roadmap 1` 收敛。
- 2026-04-01 00:43:42 +08:00：针对 `ABI/bridge` 的冻结粒度，用户选择 `B2`。
- 2026-04-01 00:43:42 +08:00：据此确认 `Roadmap 0` 需要交付正式 header 和 compile-only smoke，以验证边界可编译；但不要求真实 runtime/codegen stub 实现。
- 2026-04-01 00:45:58 +08:00：针对 Android/iOS startup smoke 骨架强度，用户选择 `C3`。
- 2026-04-01 00:45:58 +08:00：据此确认 `Roadmap 0` 中的移动端目标包含真实启动 smoke，不再满足于 toolchain/probe/README 级的软骨架或中骨架。
- 2026-04-01 00:48:27 +08:00：针对移动端 gate 形态，用户选择 `D2`。
- 2026-04-01 00:48:27 +08:00：据此确认 `Roadmap 0` 采用“单平台先硬门槛”策略：Android/iOS 中至少一个平台完成真实 startup smoke，另一个平台至少完成 compile/link/packaging 闭环。
- 2026-04-01 00:50:05 +08:00：针对首个真实 startup smoke 平台，用户选择 `E1`。
- 2026-04-01 00:50:05 +08:00：据此确认 Android 作为 `Roadmap 0` 中优先打通真实启动的移动端平台，iOS 当前阶段至少完成 compile/link/packaging 闭环。
- 2026-04-01 00:51:53 +08:00：针对五个 smoke 输入项目的完成标准，用户选择 `F1`。
- 2026-04-01 00:51:53 +08:00：据此确认五个 smoke 输入都需要在 `Roadmap 0` 中成为真实可构建项目，而不是占位样例。
- 2026-04-01 00:53:28 +08:00：针对 `verify-roadmap-0` 的严格度，用户选择 `G1`。
- 2026-04-01 00:53:28 +08:00：据此确认 `verify-roadmap-0` 默认承担全量硬校验角色，而不是分层 profile 或宽松存在性检查。
- 2026-04-01 00:56:43 +08:00：针对 `verify-roadmap-0` 的运行位置，用户选择 `H1`。
- 2026-04-01 00:56:43 +08:00：据此确认完整 `verify-roadmap-0` 是每台开发机的本地硬门槛，而不是只在参考环境或 CI 上执行的全量 gate。
- 2026-04-01 00:58:46 +08:00：用户要求“继续”，进入下一条关键澄清：在 `H1 + G1 + D2 + E1` 已成立的前提下，需要明确开发机操作系统与工具链基线，否则本地硬门槛无法自洽。
- 2026-04-01 01:01:18 +08:00：针对开发机操作系统基线，用户选择 `I2`。
- 2026-04-01 01:01:18 +08:00：据此确认允许 Windows/Linux 作为日常开发环境；同时把“iOS gate 如何在非 macOS 开发机上成立”提升为下一条必须澄清的问题。
- 2026-04-01 01:04:22 +08:00：针对 iOS gate 实现方式，用户选择 `J3`，即不使用远程 Mac，并将 iOS gate 降为文档/打包描述级。
- 2026-04-01 01:04:22 +08:00：识别到 `J3` 与 `G1`、`H1`、`D2` 存在直接冲突，因此当前讨论进入约束重对齐阶段，需先决定保留哪一侧。
- 2026-04-01 01:07:42 +08:00：用户给出新的 host-specific verify 规则：Windows 开发机跑完 `Windows / Android / Linux` 的完整 verify，macOS 开发机跑完 `iOS / macOS / Linux` 的全量 verify。
- 2026-04-01 01:07:42 +08:00：据此确认 `verify-roadmap-0` 的 gate 规则应按 host OS 分平台子矩阵定义，而不是要求所有开发机运行同一套全量平台组合。
- 2026-04-01 01:10:28 +08:00：针对 Linux verify 形态，用户选择 `L2`。
- 2026-04-01 01:10:28 +08:00：据此确认 Linux 在 `Roadmap 0` 的 verify 中只要求 cross-compile 与 packaging，不要求真实 Linux runtime smoke。
- 2026-04-01 01:13:24 +08:00：针对仓库骨架落盘方式，用户选择 `M1`。
- 2026-04-01 01:13:24 +08:00：据此确认 `Roadmap 0` 采用全骨架 upfront 策略，顶层工程目录会在本阶段直接创建。
- 2026-04-01 01:13:24 +08:00：用户新增要求：后续会加入第三方库，需要预留独立目录放置。
- 2026-04-01 01:13:24 +08:00：据此确认第三方依赖目录属于 `Roadmap 0` 顶层骨架设计的一部分，后续需继续确认命名和治理策略。
- 2026-04-01 01:15:12 +08:00：用户要求“继续”，进入下一条关键澄清：确认第三方依赖目录的命名与治理边界，以便顶层骨架和后续依赖流程保持一致。
- 2026-04-01 01:17:01 +08:00：针对第三方依赖目录命名的提问使用的是 `N1 / N2 / N3` 选项集；用户回复 `H1`，与当前问题选项不匹配，因此本轮暂不记录为有效选择，需要一次简短澄清后再继续。
- 2026-04-01 01:18:04 +08:00：用户纠正上一轮输入，明确选择 `N1`。
- 2026-04-01 01:18:04 +08:00：据此确认第三方依赖目录采用 `third_party/` 命名。
- 2026-04-01 01:20:04 +08:00：针对 `third_party/` 的内容范围，用户选择 `O1`。
- 2026-04-01 01:20:04 +08:00：据此确认 `third_party/` 只放 vendored 源码，不放工具链、下载缓存和构建产物。
- 2026-04-01 01:21:47 +08:00：针对第三方源码接入方式，用户选择 `P1`。
- 2026-04-01 01:21:47 +08:00：据此确认第三方源码默认采用手动 vendor snapshot 方式接入 `third_party/`。
- 2026-04-01 01:23:40 +08:00：针对第三方依赖来源/版本记录方式，用户选择 `Q3`。
- 2026-04-01 01:23:40 +08:00：据此确认第三方依赖采用“双层记录”：依赖目录内保留本地说明，仓库根或顶层目录另有总索引。
- 2026-04-01 01:25:24 +08:00：针对第三方依赖双层记录的文件形态，用户选择 `R1`。
- 2026-04-01 01:25:24 +08:00：据此确认每个第三方依赖目录采用 `README.md` 记录本地信息，根级总索引采用 `third_party/INDEX.md`。
- 2026-04-01 01:27:20 +08:00：针对第三方依赖目录内 `README.md` 的字段集合，用户选择 `S3`。
- 2026-04-01 01:27:20 +08:00：据此确认第三方依赖 README 采用扩展版字段集合，除来源/版本/许可证外，还需记录构建接入方式、包含路径和平台限制等信息。
- 2026-04-01 01:29:18 +08:00：针对根级 `third_party/INDEX.md` 的字段粒度，用户选择 `T1`。
- 2026-04-01 01:29:18 +08:00：据此确认根级 `third_party/INDEX.md` 采用精简总表，只承担导航与总览职责。
- 2026-04-01 01:31:39 +08:00：针对工具链描述、下载缓存和二进制依赖的边界，用户选择 `U1`。
- 2026-04-01 01:31:39 +08:00：据此确认仓库内保留 `build/toolchains/` 等工具链描述，实际下载缓存和二进制依赖全部放仓库外。
- 2026-04-01 01:33:37 +08:00：针对构建产物与 verify 中间输出目录，用户选择 `V2`。
- 2026-04-01 01:33:37 +08:00：据此确认仓库内统一使用 `artifacts/` 承载构建产物与 verify 中间输出。
- 2026-04-01 01:36:19 +08:00：针对 `artifacts/` 的职责边界，用户选择 `W1`。
- 2026-04-01 01:36:19 +08:00：据此确认 `artifacts/` 统一收口 CMake build 目录、日志、快照副本和打包产物。
- 2026-04-01 01:39:52 +08:00：针对权威快照与运行时快照副本的分离策略，用户选择 `X2`。
- 2026-04-01 01:39:52 +08:00：据此确认权威快照继续保留在源码树中，`artifacts/` 只承载 verify 运行时生成的快照副本与 diff 输出。
- 2026-04-01 01:41:38 +08:00：针对源码树中的正式 contract 目录布局，用户选择 `Y1`。
- 2026-04-01 01:41:38 +08:00：据此确认 `schema`、`example` 和权威 `snapshot` 在源码树中采用分层目录布局，而不是全部收进 `docs/` 或 `tests/`。
- 2026-04-01 01:44:28 +08:00：在继续确认 `ABI/bridge` 目录布局之前，用户追问“ABI/bridge 会有哪些东西？给一些 sample”，说明需要先把这两类 contract 的具体内容具象化，再继续做目录与验证设计。
- 2026-04-01 01:46:28 +08:00：在收到 ABI/bridge 的 sample 说明后，用户选择 `Z2`，即把 native contract 统一收进公共目录，而不是继续保留在 `runtime/`、`codegen/` 的模块归属下。
- 2026-04-01 01:46:28 +08:00：据此确认后续需要围绕“公共 native contract 包”来设计 `ABI/bridge` 的头文件、文档和 smoke test 布局。
- 2026-04-01 01:49:04 +08:00：针对公共 native contract 包内部的分层方式，用户选择 `AA1`，即采用扁平目录布局。
- 2026-04-01 01:49:04 +08:00：据此确认 `ABI/bridge` 在公共目录下不再按 `runtime/` 与 `codegen/` 预先拆分子目录，后续需要通过文件命名和测试布局来维持边界可读性。
- 2026-04-01 01:55:50 +08:00：针对扁平目录下的文件命名，提问使用的是 `AB1 / AB2 / AB3` 选项集；用户回复 `A01`，与当前问题选项不匹配，因此本轮暂不记录为有效选择，需要一次简短澄清后再继续。
- 2026-04-01 01:56:55 +08:00：用户纠正上一轮输入，明确选择 `AB1`。
- 2026-04-01 01:56:55 +08:00：据此确认扁平 native contract 目录下采用前缀区分命名，例如 `boom_runtime_abi_v0.h`、`boom_codegen_bridge_v0.h`，以及对应的人读文档命名。
- 2026-04-01 01:58:48 +08:00：针对 `ABI/bridge` 的测试布局，用户选择 `AC2`。
- 2026-04-01 01:58:48 +08:00：据此确认虽然头文件目录保持扁平，但测试目录按 `abi` 与 `bridge` 分组，例如 `tests/contracts/native/abi/` 和 `tests/contracts/native/bridge/`。
- 2026-04-01 02:00:18 +08:00：针对公共 native contract 包的根路径，用户选择 `AD1`。
- 2026-04-01 02:00:18 +08:00：据此确认 `ABI/bridge` 的公共 contract 包根路径采用 `contracts/native/`，而不是挂在 `analysis/` 或 `docs/` 之下。
- 2026-04-01 02:02:46 +08:00：针对 native contract 的版本落盘方式，用户选择 `AE2`。
- 2026-04-01 02:02:46 +08:00：据此确认 native contract 采用“目录和文件名双重带版本”的布局，例如 `contracts/native/v0/boom_runtime_abi_v0.h` 和 `contracts/native/v0/boom_codegen_bridge_v0.h`。
- 2026-04-01 02:05:35 +08:00：用户对上一轮结论做出更正：保留 `AE2` 的目录级版本化，但文件名不带版本。
- 2026-04-01 02:05:35 +08:00：据此当前 native contract 布局更正为：`contracts/native/v0/boom_runtime_abi.h`、`contracts/native/v0/boom_codegen_bridge.h`，版本信息主要由 `v0/` 目录承载。
- 2026-04-01 02:05:35 +08:00：针对 native contract 文档布局，用户选择 `AF3`。
- 2026-04-01 02:05:35 +08:00：据此确认头文件放在 `contracts/native/v0/`，而人读文档放在 `contracts/native/docs/`，不与 header 同目录收口。
- 2026-04-01 02:07:38 +08:00：针对 native contract 文档的版本布局，用户选择 `AG1`。
- 2026-04-01 02:07:38 +08:00：据此确认 native contract 文档也采用目录级版本化，例如 `contracts/native/docs/v0/runtime-abi.md` 和 `contracts/native/docs/v0/codegen-bridge.md`。
- 2026-04-01 02:09:40 +08:00：针对 native contract 是否单独提供 example，用户选择 `AH2`。
- 2026-04-01 02:09:40 +08:00：据此确认 native contract 包除 header 和 docs 外，还提供独立的 example 目录，例如 `contracts/native/examples/v0/abi/` 和 `contracts/native/examples/v0/bridge/`。
- 2026-04-01 02:11:44 +08:00：针对 native contract example 的数量与粒度，用户选择 `AI2`。
- 2026-04-01 02:11:44 +08:00：据此确认 `ABI` 与 `bridge` 各自提供 2-3 个 example，用于覆盖多个典型动作，而不是只放单个最小样例。
- 2026-04-01 02:13:30 +08:00：针对 native contract example 的动作覆盖范围，用户选择 `AJ3`。
- 2026-04-01 02:13:30 +08:00：据此确认首批 example 也会覆盖较高阶动作，包括 virtual invoke、delegate 和 icall resolve，而不只停留在线程附着、分配、registration、boxing 等基础动作。
- 2026-04-01 02:15:21 +08:00：针对高阶动作与 `bridge v0` 正式 surface 的关系，用户选择 `AK3`。
- 2026-04-01 02:15:21 +08:00：据此确认 `virtual invoke`、`delegate`、`icall resolve` 不只是 example 覆盖项，也要进入 `bridge v0` 的正式 contract surface。
- 2026-04-01 02:16:21 +08:00：针对高阶 bridge helper 的冻结粒度，用户选择 `AL3`。
- 2026-04-01 02:16:21 +08:00：据此确认 `bridge v0` 中的高阶 helper 要冻结到全细节级，不仅包括名称和签名，也包括调用约定、参数语义和错误处理模式。
- 2026-04-01 02:18:16 +08:00：针对 `bridge v0` 的错误模型，用户选择 `AM2`。
- 2026-04-01 02:18:16 +08:00：据此确认 `bridge v0` 采用混合错误模型：查询/解析类 helper 返回状态码或空指针，真正的托管语义失败走受控异常出口。
- 2026-04-01 02:19:46 +08:00：在继续确认 `ABI` 的错误边界之前，用户追问“这里 ABI 具体指的是什么？给些实战用的用法 sample”，说明需要先把 `ABI` 在本项目中的角色、调用方和典型用法再具体化。
- 2026-04-01 02:21:58 +08:00：在收到 ABI 的角色说明与 sample 后，用户选择 `AN2`。
- 2026-04-01 02:21:58 +08:00：据此确认 `ABI` 维持更底层、更显式的错误边界：基础操作优先返回状态/空值，托管异常通过专门入口显式触发，而不是让 ABI 自身大量隐式走异常出口。
- 2026-04-01 02:23:29 +08:00：针对 `ABI v0` 基础动作集合，用户输入 `A03`，按当前题目选项集解释为 `AO3`。
- 2026-04-01 02:23:29 +08:00：据此确认 `ABI v0` 采用扩展集：除生命周期、线程、分配、类初始化、异常与最小 handle/类型查询外，还把字段访问、方法调用、反射查询直接纳入 `ABI v0` 讨论范围。
- 2026-04-01 02:24:47 +08:00：针对扩展能力进入 `ABI` 的风格，用户选择 `AP2`。
- 2026-04-01 02:24:47 +08:00：据此确认 `ABI v0` 即便覆盖字段访问、方法调用与反射查询，也优先采用句柄/描述符驱动方式，而不是铺设大量一一对应的直接 API。
- 2026-04-01 02:26:01 +08:00：针对 `ABI v0` 的最小句柄集合，用户选择 `AQ3`。
- 2026-04-01 02:26:01 +08:00：据此确认 `ABI v0` 的句柄集合采用扩展版，除 `TypeInfo`、`MethodInfo`、`FieldInfo`、`Assembly/Image`、`Exception` 外，还把 `PropertyInfo`、`EventInfo`、`ParameterInfo`、`GenericContext` 纳入讨论范围。
- 2026-04-01 02:27:52 +08:00：针对这些高阶句柄在 `ABI v0` 中的语义承诺强度，用户选择 `AR1`。
- 2026-04-01 02:27:52 +08:00：据此确认 `PropertyInfo`、`EventInfo`、`ParameterInfo`、`GenericContext` 等高阶句柄在 `ABI v0` 中按完整语义承诺处理，而不是只冻结句柄与最小查询面。
- 2026-04-01 02:29:56 +08:00：针对这些完整语义承诺在 `Roadmap 0` 中的落实强度，用户选择 `AS3`。
- 2026-04-01 02:29:56 +08:00：据此确认高阶反射/泛型语义不仅进入 `header + docs`，还要求在 `Roadmap 0` 的验证资产中被显式覆盖，而不只是纸面冻结。
- 2026-04-01 02:31:23 +08:00：针对这些高阶语义在 `Roadmap 0` 中的验证形态，用户选择 `AT3`。
- 2026-04-01 02:31:23 +08:00：据此确认高阶反射/泛型 contract 不只做静态或最小 harness 验证，而要进入真正可运行的 end-to-end 语义验证范围。
- 2026-04-01 02:32:54 +08:00：针对这些高阶 end-to-end 语义验证首先落在哪个平台，用户选择 `AU1`。
- 2026-04-01 02:32:54 +08:00：据此确认高阶 ABI/bridge 语义的真正 end-to-end 验证先只在 reference desktop 上落地，不同步要求 Android/iOS/macOS 子矩阵都达到同等深度。
- 2026-04-01 02:34:16 +08:00：针对 reference desktop 的选择，用户选择 `AV3`。
- 2026-04-01 02:34:16 +08:00：据此确认 `Roadmap 0` 采用双 reference desktop：`Windows x64` 与 `macOS` 都视为 reference desktop，而不是只保留单一桌面参考平台。
- 2026-04-01 02:35:22 +08:00：针对双 reference desktop 的高阶 end-to-end gate 强度，用户选择 `AW2`。
- 2026-04-01 02:35:22 +08:00：据此确认 `Windows` 与 `macOS` 都要承担高阶 end-to-end contract 验证，但 `Windows` 作为主线基准集，`macOS` 先通过同语义子集，而不是两边从第一天就完全等强。
- 2026-04-01 02:37:18 +08:00：针对 `macOS` 同语义子集的覆盖范围，用户选择 `AX3`。
- 2026-04-01 02:37:18 +08:00：据此确认 `macOS` 的高阶 contract 子集非常宽，基本覆盖与 `Windows` 主线相同的高阶 ABI/bridge contract，只对明显平台相关的 `icall` 差异留白。
- 2026-04-01 02:39:01 +08:00：针对 `macOS` 相对 `Windows` 允许留白的 `icall` 范围，用户选择 `AY1`。
- 2026-04-01 02:39:01 +08:00：据此确认只允许真正平台能力相关的 `icall` 差异留白，例如文件系统、时间/时区、动态库装载、socket/network；其它高阶 contract 仍尽量保持同一语义面。
- 2026-04-01 02:40:30 +08:00：针对平台能力相关 `icall` 在 `Roadmap 0` 中的验证强度，用户选择 `AZ2`。
- 2026-04-01 02:40:30 +08:00：据此确认文件系统、时间/时区、动态库装载、socket/network 这类平台能力在 `Roadmap 0` 中要求 query/basic-path 级验证，而不是完整端到端平台语义打满。
- 2026-04-01 02:42:13 +08:00：针对平台能力与高阶 contract 的 smoke 资产承载方式，用户选择 `BA2`。
- 2026-04-01 02:42:13 +08:00：据此确认仍保持 5 个 smoke 输入项目不变，但在现有项目内部增加明确的子场景/用例组，而不是继续新增独立 smoke 项目。
- 2026-04-01 02:44:15 +08:00：针对 5 个 smoke 项目与能力映射的倾向，用户选择 `BB3`。
- 2026-04-01 02:44:15 +08:00：据此确认平台能力相关内容更多压到 `HostEmbeddingLite`，而 `PInvokeLite` 收缩回原生互操作主线，不再承担过多平台能力验证职责。
- 2026-04-01 02:45:29 +08:00：针对 `HostEmbeddingLite` 与 `PInvokeLite` 的具体职责边界，用户选择 `BC2`。
- 2026-04-01 02:45:29 +08:00：据此确认 `HostEmbeddingLite` 负责 host lifecycle、thread attach/detach、managed entry、delegate/callback，以及文件系统/时间/动态库装载等平台能力；`PInvokeLite` 负责 `DllImport`、native export、符号解析与基础 marshalling。
- 2026-04-01 02:47:14 +08:00：针对 `HelloWorld`、`GenericEcho`、`ReflectionLite` 三者的职责边界，用户给出组合选择 `BD2 + BD3`。
- 2026-04-01 02:47:14 +08:00：据此确认：
  - `GenericContext` 的一部分从 `GenericEcho` 回收到 `ReflectionLite`
  - `GenericEcho` 更聚焦在最直观的泛型调用与实例形态
  - `registration` 从 `HelloWorld` 挪到 `HostEmbeddingLite`
  - `HelloWorld` 收缩为最小启动样例
- 2026-04-01 02:48:40 +08:00：针对 `HelloWorld` 与 `HostEmbeddingLite` 的边界强度，用户选择 `BE2`。
- 2026-04-01 02:48:40 +08:00：据此确认 `HelloWorld` 保留最小 entry path 和最薄的一层 registration 痕迹，但完整 registration 流、host lifecycle 与 managed entry 装配集中到 `HostEmbeddingLite`。
- 2026-04-01 02:49:55 +08:00：针对 `HostEmbeddingLite` 中 registration/metadata bootstrap 的验证粒度，用户选择 `BF3`。
- 2026-04-01 02:49:55 +08:00：据此确认 `HostEmbeddingLite` 不只验证 `code registration`、`metadata registration` 与 `runtime bootstrap sequence`，还要进一步拆开 metadata cache 初始化阶段与 late-init 阶段进行验证。
- 2026-04-01 02:51:09 +08:00：针对 metadata/bootstrap 状态机的拆分粒度，用户选择 `BG3`。
- 2026-04-01 02:51:09 +08:00：据此确认 `HostEmbeddingLite` 中相关状态机按四段式拆分：
  - registration complete
  - metadata cache initialize
  - GC-dependent late-init
  - class world / reflection cache warmup
- 2026-04-01 02:52:36 +08:00：针对 `class world / reflection cache warmup` 的边界，用户选择 `BH3`。
- 2026-04-01 02:52:36 +08:00：据此确认最后这段 warmup 不只包含类型世界与基础反射 cache，还将泛型上下文与 delegate 相关 warmup 一并纳入范围。
- 2026-04-01 02:54:42 +08:00：针对最后这段 warmup 的执行方式，用户选择 `BI3`。
- 2026-04-01 02:54:42 +08:00：据此确认 `class world / reflection cache / generic context / delegate` 相关 warmup 采用全 lazy 策略：startup 只建立最小壳，后续按访问路径触发初始化。
- 2026-04-01 02:56:20 +08:00：针对 lazy warmup 的验证方式，用户选择 `BJ3`。
- 2026-04-01 02:56:20 +08:00：据此确认不仅验证最终结果，还要求 runtime 额外暴露 warmup trace / event log，用于检查触发顺序与最终结果是否一致。
- 2026-04-01 02:57:29 +08:00：针对 warmup trace / event log 是否进入正式 contract 体系，用户选择 `BK2`。
- 2026-04-01 02:57:29 +08:00：据此确认 warmup trace / event log 进入 `Roadmap 0` 的验证 contract，但不提升为与核心 `ABI/bridge` 同等级的长期核心 contract。
- 2026-04-01 02:58:34 +08:00：针对 warmup trace / event log 的落盘格式，用户选择 `BL2`。
- 2026-04-01 02:58:34 +08:00：据此确认 warmup trace / event log 采用结构化 JSON 格式，以支持 snapshot、diff 与顺序校验。
- 2026-04-01 02:59:52 +08:00：针对 warmup trace / event log 在源码树中的正式位置，用户选择 `BM2`。
- 2026-04-01 02:59:52 +08:00：据此确认 warmup trace 作为验证 contract 单独成组，例如 schema 放 `tests/contracts/trace/schema/`，权威 snapshot 放 `tests/contracts/trace/snapshots/`。
- 2026-04-01 03:01:05 +08:00：针对 warmup trace schema 的字段粒度，用户选择 `BN2`。
- 2026-04-01 03:01:05 +08:00：据此确认 warmup trace schema 采用中等粒度，至少包含 `eventName`、`phase`、`subjectKind`、`subjectId`、`order`、`status`。
- 2026-04-01 03:02:29 +08:00：针对 `subjectKind` 枚举，用户输入 `B02`，按当前题目选项集解释为 `BO2`。
- 2026-04-01 03:02:29 +08:00：据此确认 `subjectKind` 采用中等枚举集：`type`、`method`、`field`、`property`、`event`、`parameter`、`genericContext`、`delegate`、`assembly`。
- 2026-04-01 03:03:34 +08:00：针对 trace 的 `phase` 枚举，用户选择 `BP2`。
- 2026-04-01 03:03:34 +08:00：据此确认 `phase` 采用四态枚举：`requested`、`started`、`completed`、`failed`。
- 2026-04-01 03:04:47 +08:00：用户对上一轮 trace schema 枚举做出更正：`phase` 从 `BP2` 调整为 `BP3`，同时当前轮选择 `BQ3` 作为 `status` 枚举。
- 2026-04-01 03:04:47 +08:00：据此当前 trace schema 更正为：
  - `phase` 采用扩展枚举：`requested`、`started`、`completed`、`failed`、`skipped`、`cached`、`replayed`
  - `status` 采用扩展枚举：`ok`、`error`、`skipped`、`cached`
- 2026-04-01 03:06:24 +08:00：针对 `eventName` 的命名风格，用户选择 `BR3`。
- 2026-04-01 03:06:24 +08:00：据此确认 `eventName` 采用命名空间格式，如 `<domain>.<action>` 或 `<domain>.<subject>.<action>`，而不是自由字符串或完全枚举死。
- 2026-04-01 03:07:28 +08:00：针对 `eventName` 的段数规则，用户选择 `BS2`。
- 2026-04-01 03:07:28 +08:00：据此确认 `eventName` 默认采用三段格式 `<domain>.<subject>.<action>`，必要时允许两段 `<domain>.<action>`。
- 2026-04-01 03:09:12 +08:00：针对 `eventName` 的 `domain` 枚举，用户选择以 `BT2` 为基础并额外加入 `interop`。
- 2026-04-01 03:09:12 +08:00：据此确认 `domain` 集合包含：`warmup`、`metadata`、`reflection`、`generic`、`delegate`、`bootstrap`、`interop`。
- 2026-04-01 03:10:59 +08:00：针对 trace 中 `subjectId` 的表示方式，用户选择 `BU2`。
- 2026-04-01 03:10:59 +08:00：据此确认 `subjectId` 采用规范化字符串 ID，以支持稳定 snapshot、diff 与跨平台比对。
- 2026-04-01 03:12:13 +08:00：针对规范化 `subjectId` 的风格，用户选择 `BV1`。
- 2026-04-01 03:12:13 +08:00：据此确认 `subjectId` 采用人读签名式表示，而不是 token/path 混合式或哈希式。
- 2026-04-01 03:13:34 +08:00：针对人读签名式 `subjectId` 的最小组成元素，用户选择 `BW2`。
- 2026-04-01 03:13:34 +08:00：据此确认 `subjectId` 至少包含“程序集 / 类型 / 成员签名”，在泛型场景下还需带上实例化形态。
- 2026-04-01 03:15:11 +08:00：针对 brainstorm 的收口方式，用户选择 `BX2`。
- 2026-04-01 03:15:11 +08:00：据此从“逐项提问确认”切换到“分段展示设计草案并逐段审阅”的阶段；优先先展示 `Roadmap 0` 的范围、交付物与验证总原则。
- 2026-04-01 03:16:50 +08:00：已向用户展示第 1 段设计草案（范围、交付物、验证总原则），用户回复“继续”，视为这一段暂时通过。
- 2026-04-01 03:16:50 +08:00：接下来展示第 2 段设计草案：仓库骨架、目录责任和产物落盘位置。
- 2026-04-01 03:18:02 +08:00：已向用户展示第 2 段设计草案（仓库骨架、目录责任和产物落盘位置），用户回复“继续”，视为这一段暂时通过。
- 2026-04-01 03:18:02 +08:00：接下来展示第 3 段设计草案：native contract pack（ABI/bridge）的结构、边界和冻结强度。
- 2026-04-01 03:19:07 +08:00：已向用户展示第 3 段设计草案（native contract pack 的结构、边界和冻结强度），用户回复“确定”，视为这一段通过。
- 2026-04-01 03:19:07 +08:00：接下来展示第 4 段设计草案：5 个 smoke 输入项目的职责映射，以及它们与平台验证矩阵的对应关系。
- 2026-04-01 03:20:06 +08:00：已向用户展示第 4 段设计草案（5 个 smoke 输入项目的职责映射，以及它们与平台验证矩阵的对应关系），用户回复“确定”，视为这一段通过。
- 2026-04-01 03:20:06 +08:00：接下来展示第 5 段设计草案：`verify-roadmap-0`、trace contract 和验证资产组织方式。
- 2026-04-01 03:21:27 +08:00：已向用户展示第 5 段设计草案（`verify-roadmap-0`、trace contract 和验证资产组织方式），用户回复“确定”，视为这一段通过。
- 2026-04-01 03:21:27 +08:00：接下来展示最后一段设计草案：`Roadmap 0` 的阶段切分、建议执行顺序，以及与后续 roadmap 的边界。
- 2026-04-01 03:23:47 +08:00：根据用户要求，不写单体 design，而是拆分为一个入口文件和每段一个独立文件。
- 2026-04-01 03:23:47 +08:00：已创建设计入口文件 `design-2026-04-01-03-roadmap-0-contract-freeze-and-skeleton.md`，并将 6 段设计分别写入独立文件。
- 2026-04-01 03:29:16 +08:00：用户要求把 design 继续转成 plan，并要求每一段都具备可落地 checklist 步骤。
- 2026-04-01 03:29:16 +08:00：已创建 plan 入口文件、5 个 chunk plan 文件，以及新的 `plan-discuss-2026-04-01-03-roadmap-0-contract-freeze-and-skeleton.md` 记录本轮 planning 上下文。
