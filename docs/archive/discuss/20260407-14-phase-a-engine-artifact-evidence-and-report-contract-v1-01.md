# Phase A Engine Artifact Evidence And Report Contract v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

前几轮我们已经连续定下了三件事：

- `engine binding` 是正式边界，不再混进泛化 `interop`
- `LoweringPlan` 里需要正式承载 `EngineBindings` 与 `HostBindings`
- 首批 engine proof 采用三个最小 `subject`，并复用现有 `subject + matrix + artifact-chain`

现在真正高影响、必须收口的问题是：

- **engine proof 的证据到底如何进入 `analysis.manifest.json`、`generated.manifest.json`、`runtime.manifest.json` 和 `report.json`，同时又不把 `LoweringPlan` 重复抄一遍，也不另外发明一套 engine 专属 reporting 系统。**

这个问题如果不先定下来，后面会很快出现三个坏结果：

1. `engineProofProfile` 只停留在 `subject.manifest.json` 的“意图层”，没有形成实际产物链路
2. `LoweringPlan`、`manifest`、`report` 会重复表达同一层语义，边界重新变脏
3. engine proof 最后会演变成一套平行 `engine-*` 产物体系，破坏我们刚刚收紧好的主线

## 2. 问题的具体描述

这一步不是在讨论“要不要多产几份 JSON”，而是在回答四个具体问题：

1. `analysis.manifest.json` 应该如何表达“analysis 已经解析出哪些 engine contract”
2. `generated.manifest.json` 应该如何表达“backend 实际生成了哪些 engine glue / bridge / proof artifacts”
3. `runtime.manifest.json` 应该如何索引“运行期到底观察到了哪些 engine-specific evidence”
4. `report.json` 应该如何让人一眼看出 engine proof 成功或失败在哪一类证据上，同时又不退化成大而全证据仓库

这里最容易混淆的，是四层对象各自回答什么：

- `subject.manifest.json`
  - 回答“这个 subject 计划验证什么”
- `LoweringPlan`
  - 回答“analysis 真正冻结了哪些 binding / role / capability”
- `*.manifest.json`
  - 回答“各 bucket 实际产出了什么、关键文件在哪里”
- `report.json`
  - 回答“这条 matrix 最终结果如何、排障时优先看什么”

因此，这一轮的关键不是“engine 信息放不放进去”，而是：

- **放进去的究竟是意图、真源、索引，还是结果摘要**

如果这四层边界不分清，后面很容易出现两类错误：

- 把 `manifest` 写成第二份 `LoweringPlan`
- 把 `report.json` 写成第二份 `runtime.manifest.json`

## 3. 有哪些可解决的路径

### 路径 A：保持 `manifest` 完全通用，engine 细节尽量只进局部 report 和 `report.json`

做法：

- `analysis/generated/runtime.manifest.json` 不新增 engine-specific section
- engine 相关信息主要放在局部 `*.report.json` 和最终 `report.json`
- `manifest` 继续只保留最基础的通用索引字段

优点：

- 改动最小
- 不需要扩展现有 manifest schema
- 最不容易引入“专门为 engine 开字段”的心理负担

缺点：

- `manifest` 读不出 engine proof 到底产出了哪些关键证据
- `report.json` 会被迫承载过多 engine 语义
- 运行时排障需要不断在 `report -> local report -> 原始证据` 之间来回跳
- 不能很好支撑后续 matrix report assembler 做清晰的 engine 结果摘要

判断：

- 不推荐作为主路径

### 路径 B：保持文件命名和主链路不变，但在现有 manifest / report 中增量加入轻量 engine evidence summary

做法：

- 继续沿用：
  - `analysis.manifest.json`
  - `generated.manifest.json`
  - `runtime.manifest.json`
  - `matrices/<matrix-id>/report.json`
- 只在这些既有文件里增加少量 engine-specific summary section
- `LoweringPlan` 仍然是 binding 细节真源，manifest/report 只保留高层摘要与证据索引
- 局部 report 只在真正需要的 stage 里增量补充

优点：

- 保住主线目录与命名体系
- manifest/report 都能读出 engine proof 的关键上下文
- 不会让 `Emitter`、`executor`、`reporting` 回到“重新解释语义”的状态
- 最符合当前 `B2` reporting 方向：`report.json` 足够可读，但不变成原始证据仓库

缺点：

- 需要新增少量 schema 字段
- 需要明确每一层只允许放“摘要”，不能把完整 `EngineBindings[]` 再抄进去

判断：

- 推荐

### 路径 C：单独创建 engine-evidence bundle / engine-report 系统

做法：

- 额外设计：
  - `engine-analysis.manifest.json`
  - `engine-runtime.manifest.json`
  - `engine-report.json`
  - 甚至单独 `engine-summary.json`
- 让 engine proof 在既有 subject framework 外再挂一条平行证据链

优点：

- engine 语义最显式
- 一眼能看出这是专门为 engine proof 服务的体系

缺点：

- 直接把 engine 从主线里“摘”出去
- 产物体系、reporting 体系、消费入口都会分叉
- 会与前几轮刚收敛出的 `subject + matrix + artifact-chain` 主线冲突
- 长期维护成本最高

判断：

- 不推荐

## 4. 你的方案和理由

我推荐：

- **路径 B：保持文件命名和主链路不变，但在现有 manifest / report 中增量加入轻量 engine evidence summary**

理由有六个：

1. 它最符合我们现在的主线收敛方向
   - engine proof 是主线增量，不是旁路系统
2. 它能把“意图 / 真源 / 索引 / 结果”四层重新拉开
   - `subject.manifest.json` 管意图
   - `LoweringPlan` 管真源
   - `*.manifest.json` 管索引
   - `report.json` 管结果
3. 它不会逼 `report.json` 去重复存放完整 runtime 细节
4. 它也不会逼 `manifest` 去重复存放完整 binding 细节
5. 它和既有 `B2` reporting 方案天然兼容
6. 它给下一轮 consumer boundary 讨论留下了干净接口

如果按这条路径推进，我建议先冻结六条规则。

### 规则 1：文件命名、目录层级、matrix report 入口全部不变

第一批明确不新增：

- `engine-analysis.manifest.json`
- `engine-generated.manifest.json`
- `engine-runtime.manifest.json`
- `engine-report.json`

继续沿用：

- `shared/analysis/analysis.manifest.json`
- `shared/generated/generated.manifest.json`
- `matrices/<matrix-id>/runtime/runtime.manifest.json`
- `matrices/<matrix-id>/report.json`

这条规则非常关键，因为它决定了：

- engine proof 是“主线的一种 subject”
- 不是“主线旁边再长一套 engine testing system”

### 规则 2：`analysis.manifest.json` 只加 `engineContractSummary`，不复制完整 `LoweringPlan`

推荐新增：

```json
{
  "engineContractSummary": {
    "proofKind": "engine-binding",
    "focusArea": "object-handle",
    "loweringPlanPath": "artifacts/subjects/EngineObjectHandleLite/shared/analysis/lowering-plan.json",
    "resolvedCapabilityIds": [
      "engine.object.handle.create",
      "engine.object.handle.resolve"
    ],
    "resolvedHostBindingKinds": [
      "main-thread-dispatch"
    ],
    "expectedEvidenceKinds": [
      "handle-roundtrip"
    ]
  }
}
```

这里故意不写：

- `EngineBindings[]`
- `HostBindings[]`
- lane / ownership / marshalling 的完整细节

原因很简单：

- 这些细节已经由 `LoweringPlan` 承担真源职责
- `analysis.manifest.json` 只需要告诉后续阶段“本轮 analysis 实际冻结出了什么范围的 engine contract”

同时，analysis 侧的局部校验继续沿用：

- `analysis/contract-validate.report.json`

如果后续需要 engine contract 校验明细，也应作为这个局部 report 的可选 section，而不是新开 `engine-contract-validate.report.json`。

### 规则 3：`generated.manifest.json` 只加 `engineEmissionSummary`，回答“发射了什么”，不回答“为什么这么发射”

推荐新增：

```json
{
  "engineEmissionSummary": {
    "proofKind": "engine-binding",
    "focusArea": "object-handle",
    "emittedCapabilityIds": [
      "engine.object.handle.create",
      "engine.object.handle.resolve"
    ],
    "bridgeArtifactPaths": [
      "artifacts/subjects/EngineObjectHandleLite/shared/generated/engine-handle-bridge.cpp"
    ],
    "registrationArtifactPaths": [
      "artifacts/subjects/EngineObjectHandleLite/shared/generated/engine-binding-registration.cpp"
    ]
  }
}
```

这一层只回答：

- backend 根据已经冻结的 plan 生成了哪些 engine glue / bridge / registration artifacts

这一层不回答：

- emitter 是如何推断 lowering family 的
- emitter 是如何“发现” engine 语义的

因为这些工作按前面的边界讨论，本来就不应该再由 emitter 负责。

### 规则 4：`runtime.manifest.json` 只加 `engineObservationSummary`，索引证据，不直接给最终结论

推荐新增：

```json
{
  "engineObservationSummary": {
    "evidenceItems": [
      {
        "kind": "handle-roundtrip",
        "path": "artifacts/subjects/EngineObjectHandleLite/matrices/windows-dev-output/runtime/handle-roundtrip.json",
        "format": "json",
        "relatedCapabilityIds": [
          "engine.object.handle.create",
          "engine.object.handle.resolve"
        ]
      }
    ],
    "localReportPaths": [
      "artifacts/subjects/EngineObjectHandleLite/matrices/windows-dev-output/runtime/engine-observe.report.json"
    ]
  }
}
```

这里推荐的原则是：

- `runtime.manifest.json` 管证据入口
- 局部 `*.report.json` 管当前 stage 的局部判断
- `report.json` 管整条 matrix 的最终摘要

对应第一批局部 report，我建议只引入一个新增文件并复用一个已有文件：

- `runtime/engine-observe.report.json`
  - 供 `engine-runtime-output` 路径使用
- `runtime/trace-compare.report.json`
  - 继续供 `engine-runtime-trace` 路径使用

也就是说：

- output 模式下新增 `engine-observe.report.json`
- trace 模式下继续复用 `trace-compare.report.json`，只在内容里补 engine compare summary，不再额外发明 `engine-trace-compare.report.json`

### 规则 5：`report.json` 只加 `engineProofSummary`，负责结果摘要，不负责搬运原始证据

推荐新增：

```json
{
  "engineProofSummary": {
    "proofKind": "engine-binding",
    "focusArea": "object-handle",
    "capabilityIds": [
      "engine.object.handle.create",
      "engine.object.handle.resolve"
    ],
    "evidenceResults": [
      {
        "kind": "handle-roundtrip",
        "status": "ok",
        "primaryPath": "artifacts/subjects/EngineObjectHandleLite/matrices/windows-dev-output/runtime/handle-roundtrip.json"
      }
    ],
    "localReportPaths": [
      "artifacts/subjects/EngineObjectHandleLite/matrices/windows-dev-output/runtime/engine-observe.report.json"
    ]
  }
}
```

这里要特别收住边界：

- `report.json` 可以告诉你“哪类 engine evidence 过了、没过、优先看哪个文件”
- 但它不应该把整份 observer payload、trace payload、binding details 原样塞进去

因此，`report.json` 里仍然继续保留既有主结构：

- `selection`
- `stageResults`
- `artifactResults`
- `errors`

而 `engineProofSummary` 只是一个面向 engine 场景的高层摘要补丁，不替代这些通用字段。

### 规则 6：第一批实施顺序必须从 analysis 到 report 单向推进，不反向让 reporting 重新解释语义

我建议具体推进顺序固定为：

1. 冻结四个可选 section 名称：
   - `engineContractSummary`
   - `engineEmissionSummary`
   - `engineObservationSummary`
   - `engineProofSummary`
2. 先定义 `analysis.manifest.json` 的 `engineContractSummary`
3. 再定义 `generated.manifest.json` 的 `engineEmissionSummary`
4. 再定义 `runtime.manifest.json` 的 `engineObservationSummary`
5. 再定义 `runtime/engine-observe.report.json` 与 `trace-compare.report.json` 的 engine 扩展内容
6. 最后由 matrix report assembler 汇总为 `report.json.engineProofSummary`

这个顺序的意义在于：

- engine 结果摘要只能从前面真实产物汇总出来
- 不能倒过来先在 `report.json` 里发明一套 engine 结果结构，再逼前面去迎合它

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

我认为这个问题本身非常合理，而且它就是 `20260407-12` 与 `20260407-13` 之后最自然、最有价值的下一问。

因为前两轮解决的是：

- subject 是谁
- matrix 是谁
- manifest 怎么声明
- pipeline 怎么跑

但还没有解决：

- **跑完之后，engine 证据到底以什么 contract 落到主线上**

如果这一步不补上，前面的讨论还是偏“结构定义”，没有真正闭合到“可诊断的产物链”。

不过，这个问题也需要被问得足够克制。更合理的问法不是：

- “完整 engine reporting 系统怎么设计？”

而应该是：

- “在不破坏现有 artifact/report 主线的前提下，engine proof 的关键证据如何进入既有 manifest / report contract？”

为了让这个方案在整体框架里更合理，我建议再补三条审查结论：

### A. 这一步只解决 evidence contract，不顺手扩写 consumer boundary

也就是说，这一轮先不把问题扩成：

- registry 要不要看 `engineProofSummary`
- planner 要不要消费 `engineContractSummary`
- executor 要不要理解 `engineObservationSummary`

这些是下一轮可以继续细化的 consumer boundary 议题，不应在这一步混进来。

### B. 这一步允许 engine-specific 可选 section，但暂时不做过早抽象

当前推荐的：

- `engineContractSummary`
- `engineEmissionSummary`
- `engineObservationSummary`
- `engineProofSummary`

看起来是 engine-specific 的，这是有意为之。

原因是：

- 现在只有 engine proof 明确需要这条增量链
- 如果现在就强行抽成“泛化 domain summary”，大概率会把问题抽象过头

更合理的做法是：

- 等未来出现第二类真正需要同构扩展的 proof domain，再把这四个 section 提炼成更通用的 envelope

### C. 这一步的成功标准不是字段多完整，而是主线边界没有退化

真正的成功标准应该是：

1. `LoweringPlan` 仍然是 binding 细节真源
2. `manifest` 仍然是产物索引
3. `report.json` 仍然是 matrix 诊断主入口
4. engine proof 没有长出第二套 artifact/report 体系

只要这四点守住了，这轮方案就在整体框架里是合理的。

所以，这一轮的最终结论是：

- **推荐保持现有文件命名与主链路不变**
- **推荐在 `analysis/generated/runtime.manifest.json` 与 `report.json` 中增量加入轻量 engine summary section**
- **推荐继续把完整 binding 细节留在 `LoweringPlan`，把原始证据留在 runtime artifacts，把局部判断留在局部 `*.report.json`**
- **推荐把下一轮高影响话题收敛到 consumer boundary，而不是继续扩写第二套 engine reporting 体系**
