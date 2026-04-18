# Full Certified DLL 验收路径 v1.01

## 1. 目标

本文件定义 `dependency-layer` 阶段中 implementation DLL 的 `full certified` 验收路径，用于把 shared proof / benchmark / engineering validation 的批量执行证据，回填成逐 DLL 的 machine-readable 完成结论。

本路径服务于：

- `core-bcl-layer-*` 的逐 DLL closeout
- 后续复杂 BCL / external DLL lane 的单 DLL certified 判定
- layer completed 的统一判断口径

## 2. 适用范围

适用对象：

- 已经进入 `assembly-nativeization-plan/<assembly>.json` 的 implementation DLL
- 通过 `SolutionCorePack` owner subject 消费 shared canonical commands 的 foundation DLL

不适用对象：

- facade / shim / contract-only DLL
- 仅做 `contract-certified` 的程序集
- hotupdate consumer validation 阶段的 host / patch 消费者对象

## 3. 输入 authority

逐 DLL `full certified` 验收只允许消费以下 authority：

1. `assembly-nativeization-plan/<assembly>.json`
2. 当前 batch 的 scope 文件，例如 `remaining-ready-scope-v1-01.json`
3. shared execution evidence：
   - `native-proof-summary-v1-01.json`
   - `nativeization-throughput-benchmark-v1-01.json`
   - 对应 run 的 `summary.json` / `events.jsonl` / `console.log`
4. 父 roadmap 对当前 layer 的 exit criteria

不允许：

- 口头声称某 DLL “应该已覆盖”
- 手工挑 run 结果绕过 `assembly-nativeization-plan`
- 用某个代表 proof 的成功替代逐 DLL closeout

## 4. 单 DLL 完整 native 化判定

单 DLL 只有在以下四层都成立时，才允许进入 `full certified` 判定：

1. `surface complete`
   - official `ref/runtime` DLL 的公开 API 已全量入账。
   - 不存在漏 type、漏 member、漏 generic constraint 或漏 attribute semantics。
   - 这一层只证明“要翻译什么”是完整的，不等于“已经翻译完成”。
2. `semantic complete`
   - 该 DLL 的每个需要 native 化的 IL body 都已进入明确语义账本。
   - 每个 body 都必须明确归类到 `managed CIL`、`runtime helper`、`metadata supplement / reflection support`、`AOT closure / generic instantiation` 等可审计类别。
   - 不允许存在“未分类 body”或靠运行时偶然打通的公开成员。
3. `native executable evidence complete`
   - `collector -> registry -> workspace -> dependency-layer-proof -> benchmark` 对应证据都已真实执行并可追溯。
   - native proof、engineering validation、benchmark 结果都必须能回链到真实 run evidence。
   - 仅有 `build/link/smoke` 成功，不足以证明该 DLL 已完整 native 化。
4. `per-DLL certified complete`
   - 上述三层完成后，必须生成该 DLL 的 `assembly-certification-report/<assembly>.json`。
   - 只有在依赖闭合、required gates 完整、formal proofs 完整、benchmarks 可解析、blocked reasons 为空时，才允许 `finalStatus = ok`。

只要缺任意一层，都只能视为“部分证据成立”，不能宣称该 DLL 已完整翻译成 native 代码。

## 5. 验收阶段

### 阶段 0：冻结逐 DLL intake

每个 DLL 在进入 closeout 前，至少冻结以下字段：

- `assemblyName`
- `ownerSubjectId`
- `targetFrameworks`
- `requiredGates`
- `nativeProofs`
- `benchmarks`
- `conformanceGates`
- `engineeringWorkloads`
- `dependencyAssemblies`

冻结来源只能是 `assembly-nativeization-plan/<assembly>.json`。

### 阶段 1：静态前置校验

在执行 shared canonical commands 前，先校验：

- DLL 已出现在当前 batch scope
- `assembly-nativeization-plan/<assembly>.json` 可解析
- `requiredGates` 非空
- `nativeProofs` 非空
- 依赖 DLL 没有明显缺失 authority
- `targetFrameworks` 满足当前 layer 约定

若此阶段失败，DLL 状态直接记为 `blocked`，不得进入 `full certified` 判定。

### 阶段 2：shared execution evidence 采集

按 unique canonical commands 执行：

- native proofs
- engineering validations
- benchmarks

这一层只做去重执行，不按 DLL 重跑。

输出要求：

- 每条 canonical command 都有可解析的 run result
- 每条 result 都能追溯到 `summary.json` / `events.jsonl` / `console.log`

### 阶段 3：evidence projection 回填

把 shared execution evidence 回填到每个 DLL：

- `nativeProofs[*]` 按 `id` / `alias` 找到对应 run result
- `benchmarks[*]` 按 `id` / `alias` 找到对应 run result
- `conformanceGates[*]` / `engineeringWorkloads[*]` 找到对应 run result
- 生成 `assembly-certification-report/<assembly>.json`

这里的关键原则是：

- 执行去重
- 认证逐 DLL

### 阶段 4：逐 DLL 判定

单个 DLL 只有在以下条件全部满足时，才可标记为 `finalStatus = ok`：

1. 静态前置校验通过
2. `requiredGates` 对应证据全部存在且状态为通过
3. `nativeProofs` 对应证据全部存在且状态为通过
4. `benchmarks` 对应证据全部存在且可解析
5. `blockedReasons = []`
6. 所有证据都能追溯到实际 run evidence

否则：

- 缺失证据或依赖未满足：`finalStatus = blocked`
- 证据存在但执行失败：`finalStatus = failed`

### 阶段 5：整层判定

layer completed 的唯一口径：

- 当前 layer 的全部 selected DLL 都已经生成 `assembly-certification-report/<assembly>.json`
- 且每个 report 的 `finalStatus = ok`

只要有一个 DLL 仍是 `blocked` 或 `failed`，整层都不能标记为 completed。

## 6. `assembly-certification-report` 最小字段

每个 DLL 的 report 至少包含：

- `schemaVersion`
- `taskId`
- `sourceTaskId`
- `assemblyName`
- `ownerSubjectId`
- `targetFrameworks`
- `requiredGates`
- `dependencyAssemblies`
- `nativeProofResults`
- `benchmarkResults`
- `engineeringValidationResults`
- `engineeringWorkloadResults`
- `evidencePaths`
- `blockedReasons`
- `finalStatus`

建议附加字段：

- `benchmarkAcceptance`
- `dependencyStatus`
- `crossVersionStatus`
- `notes`

## 7. Benchmark 特殊规则

benchmark 的职责是吞吐与成本证据，不替代 correctness。

因此：

- benchmark 缺失：不能 `full certified`
- benchmark 执行失败：不能 `full certified`
- benchmark 成功但 `regressionStatus = no-baseline`：允许 DLL 进入 `finalStatus = ok`

此时 report 中必须明确：

- `benchmarkAcceptance = throughput-only`

也就是说：

- `full certified` 可以接受“无历史基线”
- release / regression gate 不能接受“无历史基线”

## 8. 依赖与跨版本规则

### 依赖规则

如果某 DLL 依赖的上游 foundation DLL 尚未 certified，则当前 DLL 只能标记为 `blocked`，不能跳过依赖直接宣告完成。

### 跨版本规则

当前 roadmap 要求 `.NET 8` 与 `.NET 10` 保持同构执行路径。

因此逐 DLL report 至少要记录：

- `targetFrameworks = ["net8.0", "net10.0"]`
- 当前 closeout 是否同时覆盖这两个版本的 authority

若后续 `.NET 10 delta` 发现该 DLL 存在新增或差异化 requirement，则：

- 先前 report 可保留
- 但 `crossVersionStatus` 需回落到待补全状态，直到 delta closure 完成

## 9. 状态机

建议逐 DLL 使用以下状态机：

1. `planned`
2. `scoped`
3. `executing`
4. `evidence-ready`
5. `ok | blocked | failed`

含义：

- `planned`: 只在 roadmap 中被识别，尚未进入当前 batch
- `scoped`: 已进入 batch，且 intake 冻结
- `executing`: shared canonical commands 正在运行
- `evidence-ready`: shared evidence 已齐，但还没完成逐 DLL projection
- `ok`: 满足 `full certified`
- `blocked`: 依赖、authority 或证据缺失
- `failed`: 证据存在，但至少一个 required object 失败

## 10. 当前 `20260418-11` 的落地要求

对当前 child `20260418-11`：

- 先跑 `11` 个 unique native proofs、`4` 个 engineering validations、`7` 个 benchmarks
- 再为剩余 `21` 个 DLL 逐个生成 `assembly-certification-report/<assembly>.json`
- 最后生成 `layer-certification-summary-v1-01.json`

只有这三层都完成，`core-bcl-layer-01` 才能进入整层 completed 评估。
