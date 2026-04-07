# roadmap-0 测试合理性评估 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 结论

`roadmap-0` 作为历史阶段的“统一验证工作流”是合理的；  
但在当前已经明确采用 `subject + goal + environment-matrix + artifact-chain` 的新测试架构下，`roadmap-0` 已经不再适合作为正式的一等测试对象继续保留。

更准确地说：

- 它曾经是合理的阶段性集成壳层
- 但它现在不再是合理的长期架构中心
- 它应被降级为“历史迁移来源 / 能力拆解来源”
- 不应继续作为公开测试模型、正式 registry 主对象或 CLI 核心语义

## 2. 为什么说它曾经合理

在旧框架里，`roadmap-0` 解决的是“先把一条项目主线打通”的问题，而不是“把测试语义建模正确”的问题。

从现有文档和脚本看，它把这些东西一次性串了起来：

- schema / contract 基础校验
- managed smoke 项目构建与运行
- native ABI / bridge compile-only smoke
- Windows / macOS reference desktop trace compare
- Android / iOS / Linux 的 routing 或 packaging gate

对应证据：

- `roadmap-0 runtime` 明确把它定义为“项目级验证链路收敛为正式 system 对象”：
  [roadmap-0-runtime.md](/E:/agent/booming-il2cpp/wiki/06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/%E6%95%B4%E4%BD%93%E9%AA%8C%E8%AF%81%E5%9C%BA%E6%99%AF/roadmap-0-runtime.md#L8)
- Windows / macOS 的 `scenario.manifest.json` 也确实是一个聚合容器：
  [scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-windows/scenario.manifest.json#L8)
  [scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-macos/scenario.manifest.json#L8)
- `verify-roadmap-0.py` 把从 schema 到 smoke、再到 reference gate 和平台 gate 的动作都编排在一支脚本里：
  [verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L200)

如果项目当时的目标是“快速拥有一个能跑的主线验收入口”，这个设计是成立的。

## 3. 为什么它现在不再合理

### 3.1 主轴错了

你现在已经确认测试主轴应该是：

- 测试目标：`correctness.dev` / `correctness.platform` / `perf.dev` / `perf.release`
- 产物主线：`source -> host-input -> analysis -> generated -> build -> runtime -> report`

而 `roadmap-0` 的主轴却是：

- 按宿主平台分成 `roadmap-0-windows` / `roadmap-0-macos`
- 再把不同类型验证硬塞进一个 workflow / system 聚合对象

这意味着它回答的是“当前宿主的一条历史路线通没通”，而不是“某个 subject 在某个目标、某个 matrix 下是否达到了预期”。

这和新架构要解决的问题已经不是同一个问题。

### 3.2 它把多种验证语义混在一起

`verify-roadmap-0.py` 同时做了：

- JSON parse
- analysis contract validate
- trace schema validate
- native compile-only smoke
- managed output smoke
- reference preset build
- trace compare
- Android / iOS / Linux routing gate

可见：
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L204)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L216)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L222)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L233)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L246)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L280)

这些动作在新模型里至少会落到不同的：

- `goal`
- `validationMode`
- `adaptationLevel`
- `artifact bucket`

旧 `roadmap-0` 把它们捆成一个“通过 / 不通过”的大对象，会直接削弱失败定位能力。

### 3.3 它把“平台适配验证”与“完整平台正确性”混淆了

现有 gate 文档已经明确承认：

- Linux gate 验证的是“矩阵可调度、路由可构建、shell 可落盘”，不是完整运行时闭环
- reference desktop gate 验证的是“参考桌面基线仍可构建并且 trace contract 未漂移”，不是全量 workflow

见：
[roadmap-0-platform-gates.md](/E:/agent/booming-il2cpp/wiki/06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/%E6%95%B4%E4%BD%93%E9%AA%8C%E8%AF%81%E5%9C%BA%E6%99%AF/roadmap-0-platform-gates.md#L24)

这恰好说明 `roadmap-0` 里很多“平台验证”其实只是适配性 / buildability / routing capability 的 probe，不是平台正确性本身。

而你已经明确要求：

- 平台适配性应和平台正确性放在一起建模
- 需要区分不同成熟度与不同验证深度

因此旧 `roadmap-0` 的表达粒度已经不够。

### 3.4 它没有 subject 身份，只有工作流身份

Windows 聚合对象包含：

- `pipeline/completion-runtime-trace-windows`
- `system/roadmap-0-android-startup-gate`
- `system/roadmap-0-linux-packaging-gate`

见：
[scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-windows/scenario.manifest.json#L8)

macOS 聚合对象包含：

- `pipeline/completion-runtime-trace-macos`
- `system/roadmap-0-ios-packaging-gate`
- `system/roadmap-0-linux-packaging-gate`

见：
[scenario.manifest.json](/E:/agent/booming-il2cpp/tests/registry/system/roadmap-0-macos/scenario.manifest.json#L8)

这说明它关心的是“当前 host 能做哪些事”，而不是“哪个 subject 被验证了什么”。

但你现在希望顶层测试对象是可以持续扩展的大量调试项目，这就要求正式模型必须围绕 `subjects/` 收敛，而不是围绕历史工作流收敛。

### 3.5 它把具体样例和产物路径写死在底层脚本里

脚本直接硬编码了：

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`
- `HelloWorldObject`
- `artifacts/proof/managed-closure/...`
- `artifacts/proof/native-reference/...`

见：
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L85)
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L222)

这和你已经确认的方向冲突：

- `subjects/` 应作为未来大量扩展调试项目的统一入口
- subject 应可配置，而不是散落在脚本常量里
- `managed-closure` 命名本身也已经被判定为不合理

### 3.6 它的报告模型过粗

`roadmap-0` 更像一次宿主级“总工作流执行”，天然更适合输出单次脚本日志，而不是：

- `subject-report`
- `matrix-results[]`
- `stage-results[]`
- `artifact-results[]`

因此它不利于你要求的两类结果：

- 开发环境正确性验证
- 多平台 / 发布性能的分层报告与覆盖追踪

## 4. 应该如何定性 roadmap-0

建议给它一个明确的新定位：

- 不是正式测试对象
- 不是新 registry 的公开对象模型
- 不是新 CLI 的目标实体
- 不是长期文档入口

它应该被视为：

- 历史阶段的集成脚本集合
- 新框架迁移时的能力清单来源
- 用于拆解出 `subject` / `matrix` / `goal` / `stage-check` 的参考实现

## 5. roadmap-0 里哪些东西值得保留

不是整个 `roadmap-0` 保留，而是把其中有效能力拆出来保留。

### 5.1 值得保留的能力

- schema / contract 校验能力
- trace schema 与 snapshot compare 能力
- reference desktop trace gate 能力
- Android / iOS / Linux 的 routing / packaging probe 能力
- HostEmbeddingLite 相关 trace export 能力
- HelloWorldObject 的 native proof 输入与 reference 输出链路

### 5.2 不值得保留的壳层

- `workflow/roadmap-0-*` 这层公共语义
- `system/roadmap-0-*` 作为正式中心对象的地位
- “一个宿主对应一整条 roadmap 验证流程”的入口设计
- `verify-roadmap-0.*` 作为测试框架中心脚本的角色

## 6. 在新架构下怎么拆

建议按“subject + matrix + goal”拆，而不是按 “roadmap host workflow” 拆。

### 6.1 第一优先级 subject

- `subjects/HelloWorldObject`
  - 作为 canonical proof subject
  - 覆盖 `source -> host-input -> analysis -> generated -> build -> runtime -> report`
- `subjects/HostEmbeddingLite`
  - 作为 diagnostic subject
  - 覆盖 trace / host embedding / runtime 证据
- `subjects/ReflectionLite`
- `subjects/PInvokeLite`
- `subjects/GenericEcho`

### 6.2 把原 roadmap-0 gate 变成 matrix，而不是变成 system

例如 `HelloWorldObject` 可以声明：

- `windows-dev-output`
- `windows-reference-trace`
- `macos-reference-trace`
- `windows-android-buildable`
- `macos-ios-buildable`
- `windows-linux-buildable`
- `macos-linux-buildable`

然后通过：

- `validationMode`
- `adaptationLevel`
- `expectedOutcome`

来表达它到底是在验证：

- compile-only
- build
- startup
- output
- trace
- perf

而不是继续用 “android-startup-gate / linux-packaging-gate / reference-gate” 这类历史命名来承载主模型。

### 6.3 保留必要的低层实现，但只作为 stage worker

`verify-roadmap-0.py` 里不少动作仍然可以复用，但角色要变成：

- stage executor
- contract validator
- trace comparator
- preset probe helper

不能继续充当“整个测试体系的总 orchestrator”。

## 7. 最终判断

如果问题是：

“`roadmap-0` 这个测试在旧阶段是否有价值？”

答案是：

- 有，作为历史主线打通验证是合理的

如果问题是：

“`roadmap-0` 在当前新测试框架里是否还应继续作为正式测试对象存在？”

答案是：

- 不合理

更合适的处理方式是：

- 把 `roadmap-0` 降级为历史迁移来源
- 把里面可复用的验证能力拆到新 `subject` 架构
- 用 `subject-report -> matrix-results -> stage-results -> artifact-results` 重建报告面
- 不再让 `roadmap-0` 占据正式 CLI / registry / 文档入口

## 8. 对当前设计稿的影响

这条判断会强化以下设计结论：

- 第一波迁移只保留 `HelloWorldObject` 是合理的
- 旧 `roadmap-0` 不做对等迁移是合理的
- 未来若需要恢复其中某些验证，必须以新的 `subject` 形式重建
- 旧的 platform gate 应转译成 matrix 能力，而不是原样保留为主模型
