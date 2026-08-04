# Phase A Engine Proof Subject Matrix And Artifact-Chain v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

上一轮我们已经把两件事讲清楚了：

- `engine binding` 应该成为正式边界
- `LoweringPlan` 里应该增量引入 `EngineBindings` 与 `HostBindings`

那么接下来必须回答的问题就是：

- **首轮 engine proof 到底应该先定义哪些 `subject`，给它们配哪些 `matrix`，以及它们是不是继续走现有 `subject + matrix + artifact-chain` 主线。**

这个问题如果不先收口，会马上出现三个风险：

1. engine 集成会退化成几个临时 demo，而不是正式 `subject`
2. engine proof 会为了赶进度单独发明一条平行 artifact 链
3. 回调、对象句柄、引擎服务调用三类问题会混在一个巨型 smoke 里，失败无法定位

所以这一轮不是在讨论“再举几个例子”，而是在决定：

- engine 主线第一批 canonical subject 是什么
- 每个 subject 的第一批 matrix 到哪一层
- 失败是否仍然能稳定定位到 `source / host-input / analysis / generated / build / runtime`

## 2. 问题的具体描述

从前两轮结论看，首轮 engine proof 至少要覆盖三类不同能力：

1. `service call`
   - 例如 `engine.log.write`
2. `object handle / lifetime`
   - 例如 wrapper object、handle create/resolve、ownership
3. `callback / lifecycle / thread lane`
   - 例如 update callback、main-thread dispatch、lifecycle entry

这三类能力分别对应三个不同的 contract 风险：

- `service call`
  - 验证 facade method 到 engine service 的最小绑定闭环
- `object handle / lifetime`
  - 验证 managed wrapper 和 native object model 的桥接没有失真
- `callback / lifecycle / thread lane`
  - 验证 engine 反向进入 C# 的调度路径、线程约束和观察方式

因此这一轮要同时回答三个具体问题：

### 第一问：首轮 subject 是一个还是多个

可能的做法有：

- 一个大而全的 `EngineBindingSmoke`
- 拆成多个小 subject

这会直接影响：

- 失败是否可定位
- 后续 subject onboarding pattern 是否稳定
- 是否能明确知道哪一类 engine 语义已经进入正式支持

### 第二问：首轮 matrix 宽度到哪儿

可能的宽度有：

- 只做 `windows-dev-output`
- 做 `windows-dev-output + windows-reference-trace`
- 一开始就把 Android / iOS buildable 也带进来

这会直接影响：

- 我们当前是在验证“engine 语义闭环”
- 还是过早把平台/打包/toolchain 变量一起带进来

### 第三问：artifact-chain 要不要单开新路径

engine proof 很容易诱导出一条平行设计：

- `engine-source`
- `engine-analysis`
- `engine-generated`
- `engine-runtime`

看起来好像更“专门”，但它会破坏我们已经固定的长期主线：

- `subject + matrix + artifact-chain`
- `shared/`
- `matrices/<matrix-id>/`
- `*.manifest.json / report.json / summary.json / events.jsonl`

所以，当前最关键的不是“多快做出一个 engine sample”，而是：

- **engine proof 要不要成为现有主线里的正式一等对象。**

## 3. 有哪些可解决的路径

### 路径 A：先做一个总包型 `EngineBindingSmoke`，只配 `windows-dev-output`

做法：

- 只定义一个 subject，例如：
  - `EngineBindingSmoke`
- 在同一个工程里同时覆盖：
  - log write
  - object handle
  - lifecycle callback
- 只先跑：
  - `windows-dev-output`

优点：

- 启动成本最低
- 最快能看到“引擎接上了没有”
- 文档与 manifest 数量最少

缺点：

- 三类语义完全混在一起
- 一旦失败，很难判断是：
  - service-call contract 出错
  - handle/lifetime 出错
  - callback/thread lane 出错
- 不利于后续把不同能力逐步升格成正式支持矩阵
- 会鼓励继续写“大而全 smoke”，不利于长期 `subject` 建模

判断：

- 不推荐

### 路径 B：定义三个最小 engine subject，首轮只做 Windows 双 matrix，并继续复用现有 artifact-chain

做法：

- 首轮就把三类语义拆成三个正式 subject：
  - `EngineLogWriteLite`
  - `EngineObjectHandleLite`
  - `EngineLifecycleCallbackLite`
- 每个 subject 首轮只支持两个 matrix：
  - `windows-dev-output`
  - `windows-reference-trace`
- 继续复用现有 artifact-chain：
  - `shared/source`
  - `shared/host-input`
  - `shared/analysis`
  - `shared/generated`
  - `matrices/<matrix-id>/build`
  - `matrices/<matrix-id>/runtime`
  - `matrices/<matrix-id>/report.json`

优点：

- 三类 engine 语义边界清楚
- 既验证默认开发 correctness，也验证 reference trace 合同
- 不会过早把 Android / iOS / packaging 变量引进来
- 不需要为 engine proof 发明第二套 artifact 模型
- 最符合当前 roadmap 的节奏

缺点：

- 比路径 A 多三个 subject onboarding 成本
- 首轮就要想清楚每个 subject 的最小断言
- `windows-reference-trace` 的观察 contract 需要尽早补齐

判断：

- 推荐

### 路径 C：三个 subject 一次性铺开到桌面 + 移动 buildable matrix

做法：

- 同样定义三个最小 subject
- 但首轮 matrix 直接铺开为：
  - `windows-dev-output`
  - `windows-reference-trace`
  - `windows-android-buildable`
  - `macos-ios-buildable`
  - 甚至更多

优点：

- 平台覆盖面最大
- 从一开始就逼出 engine 接入的跨平台差异

缺点：

- 过早把 platform/toolchain/packaging 变量引进来
- 很容易掩盖真正的 engine contract 问题
- 会让我们在 `Phase A` 讨论里提前掉进 `Phase D/E/F`
- 失败面过宽，不利于稳定收敛

判断：

- 不推荐作为首轮主路径

## 4. 你的方案和理由

我推荐：

- **路径 B：定义三个最小 engine subject，首轮只做 Windows 双 matrix，并继续复用现有 artifact-chain**

理由有六个：

1. 它最符合当前 roadmap 节奏
   - `Phase A` 现在要收的是 core contract
   - 不是马上把移动平台适配一起卷进来

2. 它能精确覆盖三类 engine 风险
   - `EngineLogWriteLite`
   - `EngineObjectHandleLite`
   - `EngineLifecycleCallbackLite`
   分别对应三类不同的 semantic binding

3. 它最符合现有 `subject` 模型
   - 一个 subject 应该对应一个清晰的验证对象
   - 不应该再次退化成“大一统 smoke”

4. 它保留了 reference baseline
   - `windows-dev-output` 验证默认 correctness
   - `windows-reference-trace` 验证 reference/trace contract

5. 它不会破坏现有 artifact-chain
   - engine proof 仍然是现有主线的一部分
   - 不是 engine 专属平行系统

6. 它为后续平台扩张留出了稳定台阶
   - 等 Windows 双 matrix 稳定后
   - 再进入 `macOS` parity 与移动 buildable/runtime subset

如果按这个方案推进，我建议先冻结五条规则。

### 规则 1：首轮 engine 子集固定为三个 subject，不做总包 smoke

建议首轮正式 subject 固定为：

1. `EngineLogWriteLite`
   - 验证 facade -> engine service call
2. `EngineObjectHandleLite`
   - 验证 wrapper -> handle create/resolve -> identity roundtrip
3. `EngineLifecycleCallbackLite`
   - 验证 callback register -> lifecycle dispatch -> thread lane

不推荐首轮做：

- `EngineBindingSmoke`

因为它会把失败定位重新打散。

### 规则 2：首轮 matrix 固定为 Windows 双 matrix，不提前带入移动 buildable

每个 subject 首轮只定义：

- `windows-dev-output`
- `windows-reference-trace`

推荐默认值：

- `defaultGoal = correctness.dev`
- `defaultMatrix = windows-dev-output`

推荐支持的 goal：

- `correctness.dev`
- `correctness.platform`

推荐原因：

- `windows-dev-output`
  - 负责默认开发 correctness
- `windows-reference-trace`
  - 负责 reference baseline 与 trace 合同

而下面这些先明确后置：

- `windows-android-buildable`
- `macos-reference-trace`
- `macos-ios-buildable`

它们应属于后续平台阶段，而不是当前 engine contract 冻结阶段。

### 规则 3：engine proof 继续复用现有 artifact-chain，不新增平行 bucket 体系

首轮仍然沿用：

- `shared/source`
- `shared/host-input`
- `shared/analysis`
- `shared/generated`
- `matrices/<matrix-id>/build`
- `matrices/<matrix-id>/runtime`
- `matrices/<matrix-id>/report.json`

不要新增：

- `engine-source/`
- `engine-analysis/`
- `engine-runtime/`

这种平行路径。

这条规则很关键，因为它决定了：

- engine proof 是主线扩张
- 不是侧边实验系统

### 规则 4：engine-specific 证据写进既有 manifest/report 体系，而不是发明另一套结果对象

例如：

- `analysis.manifest.json`
  - 继续记录：
    - typed-il
    - metadata
    - closure manifest
    - lowering plan
  - 只是这次 lowering plan 中会包含：
    - `EngineBindings`
    - `HostBindings`
- `generated.manifest.json`
  - 继续记录 generated source 与 proof manifest
- `runtime.manifest.json`
  - 继续记录 runtime 观察证据
  - 但允许增加：
    - engine observer artifacts
    - callback order evidence
    - handle roundtrip evidence

换句话说：

- 文件名和 bucket 不变
- manifest 内部索引的 evidence 类型可以扩

### 规则 5：首轮 pipeline 只需要两条，不额外发明 engine 专属 graph 体系

推荐首轮 pipeline 只定义：

1. `engine-runtime-output`
   - 对应 `windows-dev-output`
2. `engine-runtime-trace`
   - 对应 `windows-reference-trace`

它们仍然遵循现有 graph 思想：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-engine-proof`
- `build-target`
- `runtime-observe`
- 可选 `runtime-trace-compare`
- `report-assemble`

重点不是 pipeline 名字，而是：

- 仍然通过 `pipelineId` 选择 graph
- 仍然用 `scope=shared` / `scope=matrix` 表达复用与终段

## 4.1 推荐的首轮 subject 定义

### `EngineLogWriteLite`

目标：

- 验证最小 engine service call 已经成立

最小语义：

- facade method 存在
- lowering plan 带出 `engine.log.write`
- backend 能连接到 engine log service
- runtime 能观察到预期消息

建议输出断言：

- 观察到一条确定的 log message
- exit code 正确

### `EngineObjectHandleLite`

目标：

- 验证 managed wrapper 与 engine object identity bridge 已成立

最小语义：

- wrapper type 存在
- handle create/resolve contract 成立
- ownership / lifetime metadata 被正确消费
- runtime 能完成 roundtrip 并读回稳定 identity

建议输出断言：

- handle roundtrip 成功
- identity/name/token 一致
- exit code 正确

### `EngineLifecycleCallbackLite`

目标：

- 验证 engine -> C# 回调与线程约束链路已成立

最小语义：

- callback register contract 成立
- lifecycle dispatch contract 成立
- main-thread / execution lane 要求被宿主满足

建议输出断言：

- callback 发生
- callback 次序符合预期
- thread lane 证据符合预期

## 4.2 推荐的首轮 matrix 定义

对三个 subject，首轮统一推荐：

### `windows-dev-output`

用途：

- 默认开发环境 correctness

目标：

- `correctness.dev`

terminal bucket：

- `runtime`

核心证据：

- subject source 可构建
- analysis 产物可生成
- lowering plan 含 engine/host binding
- generated proof 可构建
- runtime 观察结果符合预期

### `windows-reference-trace`

用途：

- Windows reference baseline

目标：

- `correctness.platform`

terminal bucket：

- `runtime`

核心证据：

- 主线 proof 可构建
- trace/observer 证据可导出
- trace/observer contract 与 snapshot 对齐

## 4.3 推荐的 artifact-chain 边界

首轮推荐的 bucket 仍然是：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

但对 engine subject，需要明确每一层多回答什么：

### `source`

回答：

- 当前 subject 的 canonical engine facade source 是什么

### `host-input`

回答：

- facade assembly 与必要 host 输入是否构建成功

### `analysis`

回答：

- 当前 subject 的 lowering plan 是否已经显式包含：
  - `EngineBindings`
  - `HostBindings`

### `generated`

回答：

- backend 是否已经基于上述 binding 生成 engine proof 所需 source/glue

### `build`

回答：

- 当前 Windows reference target 是否构建成功

### `runtime`

回答：

- engine-specific 观察证据是否成立
  - log message
  - handle roundtrip
  - callback order / lane

## 4.4 推荐的三条最小 pipeline

虽然首轮 matrix 只建议先落两条，但为了后续顺滑扩展，我建议文档上先明确三条语义 pipeline：

1. `engine-runtime-output`
   - 给 `windows-dev-output`
2. `engine-runtime-trace`
   - 给 `windows-reference-trace`
3. `engine-platform-buildable`
   - 预留给后续 Android / iOS / macOS 适配阶段

但其中只有前两条属于当前首轮范围。第三条只是预留命名，不是现在就要执行。

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

我认为这个问题本身非常合理，而且它是前两轮边界讨论之后的必然下一步。

因为前两轮已经回答了：

- engine binding 是正式边界
- 它在 lowering plan 里也应有正式落点

那如果再不回答：

- “第一批 canonical subject 到底是谁”
- “它们走哪些 matrix”
- “它们是不是继续复用同一条 artifact-chain”

前面的 contract 讨论就还是停留在抽象层。

不过，这个问题也要避免问得过大。

更精确的问法不应该是：

- “完整引擎接入测试体系怎么设计？”

而应该是：

- “在 `Phase A`，哪一批最小 engine subject 能最有效地验证 service call、object handle、callback/thread lane 三类 contract，并且还能继续服从现有 subject 主线模型？”

为了让这个方案在整体框架里更合理，我建议再补三条约束：

### A. 首轮 subject 必须是一语义一对象，不要混做大杂烩

如果某个 subject 同时覆盖：

- log
- handle
- callback

那么它大概率就不再是好 subject，而是临时 smoke。

### B. 首轮先锁 Windows 双 matrix，不要让平台适配把 engine contract 问题盖住

当前真正要回答的是：

- engine 语义闭环是否成立

而不是：

- Android packaging 是否已经跑通

这两件事在本阶段必须拆开。

### C. 首轮 artifact-chain 必须继续共用既有模型

如果 engine proof 重新开一套 bucket / manifest / report，对整体主线是伤害，不是加速。

所以，这一轮的最终结论是：

- **首轮 engine 主线应固定为三个最小 subject：`EngineLogWriteLite`、`EngineObjectHandleLite`、`EngineLifecycleCallbackLite`**
- **首轮 matrix 只建议先做 `windows-dev-output + windows-reference-trace`**
- **首轮 artifact-chain 必须继续复用现有 `subject + matrix + artifact-chain` 主线，不开 engine 专属平行体系**
