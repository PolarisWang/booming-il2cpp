# IL2CPP 核心能力与 Subject 完整验证设计 v1.01

Date: 2026-04-14 12:41:36 +08:00

## 1. 目标

本设计定义新的长期目标状态：

1. IL2CPP、热更与 mixed execution 的核心能力都能在统一的 capability 模型下被表达和跟踪。
2. retained subjects 不只是样例仓库，而是每个能力点的长期 owner 与验证载体。
3. “subject 工程有完整验证能力”有明确、可操作、可验收的定义。
4. 新能力点接入时，不需要新增顶层 subject，也不需要继续扩大 string 协议。

## 2. 核心设计原则

### 2.1 保持固定顶层 Subject

顶层只保留：

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

新增能力点必须优先落入现有 retained subject，而不是新增新的顶层 subject。

### 2.2 Subject Source 继续保持 Solution 模式

subject source 的基本组织仍然是：

- solution archetype 负责工程级依赖图与入口闭环；
- feature slice 负责语义级 proof 与 selective benchmark；
- 二者共存于同一个 retained subject 中。

这意味着：

- 完整 solution 不是“案例的一种”，而是 subject 的主形态。
- 历史语法点可以机械拆成单文件，但要尽量合并进共享 project。

### 2.3 `Chaos.TestFramework` 是唯一公共 Managed 测试 Authoring 面

公共入口仍位于：

- `src/reference/Chaos.TestFramework`

它只承载轻量的、稳定的 authoring metadata：

- `ChaosUnitTest`
- `ChaosBenchmark`
- capability / archetype / execution mode 等轻量枚举

它不承载：

- 支持状态矩阵
- 平台差异
- 发布门禁策略
- 大量 string key 约定

### 2.4 外层系统继续承载 Support-State 与 Productization 维度

以下信息仍然放在外层 matrix / registry / reporting / docs：

- `supported / partial / unsupported / not-applicable`
- reason code
- host / target / platform 差异
- compatibility band / rollback gate
- release 文档和产品约束

### 2.5 Proof 与 Benchmark 分工固定

- proof：
  - 使用 `Assert` 做正确性验证。
  - 优先覆盖全部能力点。
- benchmark：
  - 只覆盖高价值 runtime path。
  - 默认基线是 managed time。
  - identity 必须稳定绑定 `subject + archetype/capability + case`。

## 3. Retained Subject 职责模型

| Subject | 主职责 | 主要 owner archetype | 主要能力闭环 |
| --- | --- | --- | --- |
| `SolutionCorePack` | IL2CPP native 主线、solution ingest、reference/corelib 闭包 | `SimpleLibrarySolution`、`MultiProjectSolution`、`PackageReferenceSolution`、`ReferenceAssemblySolution`、`CoreLibReferenceSolution`、`MixedReferenceClosureSolution` | C# 运行时语义、AOT 闭包、metadata、低层 runtime、native benchmark |
| `HotUpdateHostPack` | host/patch/full-project hot-update 工程闭环 | `SkeletonPatchSolution`、`FullProjectHotUpdateSolution`、`VersionCompatibilitySolution`、`ReferenceAssemblyPatchSolution` | package load、shared contract、method replacement、metadata supplement、rollback、compatibility |
| `MixedExecutionFeaturePack` | managed/native/interpreter 混合执行与 bridge | `MixedBridgeSolution` | lowering、bridge、interpreter、mixed benchmark、边界语义一致性 |

## 4. 能力台账模型

后续所有能力扩展都采用同一台账模型：

| 字段 | 含义 |
| --- | --- |
| `capability_family` | 一级能力族，用于 dashboard 与文档归类 |
| `capability_item` | 二级能力项，用于 owner 与验收的最小单位 |
| `owner_subject` | 该能力点应归属哪个 retained subject |
| `owner_archetype` | 该能力点的工程级主验证载体 |
| `proof_entries` | 正确性验证条目 |
| `benchmark_entries` | 若该能力有高价值 runtime path，则列出 benchmark 条目 |
| `support_state_source` | 外层 support-state 与 reason-code 的真源位置 |
| `docs_anchor` | 文档中的稳定锚点 |

## 5. “完整验证能力”的定义

当且仅当以下条件成立时，某个 capability item 才算闭环：

1. 已确定唯一 `owner subject`。
2. 已确定唯一 `owner archetype` 或明确的 feature slice owner。
3. 有至少一个 assert-based proof，能在统一入口里被发现和执行。
4. 若属于高价值 runtime 路径，有稳定 identity 的 benchmark，且 dashboard 默认以 managed 为基线展示。
5. registry / reporting / dashboard 能显示该能力点与其证据。
6. support-state 与 reason code 能在外层系统中表达。
7. 若该能力影响完整 solution ingest、native closure、patch compatibility 或 mixed bridge，则至少有一个 solution 级验证与之绑定。
8. docs 中能回答“怎么验证、当前边界是什么、若失败会在哪里看到证据”。

## 6. 验证与产物链路

目标链路如下：

```text
subject source
  -> Chaos.TestFramework metadata
  -> declared discovery / compiled catalog
  -> registry / selector / runner
  -> proof / benchmark artifacts
  -> dashboard / reporting / support-state
  -> architecture / release docs
```

设计要求：

- 任何新增能力点都不能只落在 subject 代码里。
- 任何新增能力点也不能只出现在外层 matrix 里而没有实际 proof/archetype。

## 7. Reference Bundle 策略

仓库继续使用 repository-owned reference bundle，而不是依赖机器本地 SDK 路径。

范围包括：

- `ReferenceAssemblySolution` 所需 reference DLL
- `CoreLibReferenceSolution` 所需 foundation bundle
- `ReferenceAssemblyPatchSolution` 所需 patch reference bundle

约束：

- bundle 是工程基线的一部分；
- 刷新策略是显式的、脚本化的；
- archetype 只消费 bundle，不消费宿主机绝对路径。

## 8. Benchmark 设计约束

benchmark 不再按 proof 机械派生，而是围绕以下类型补齐：

- dispatch / interface / generic virtual
- allocation / GC-sensitive path
- async / scheduler / synchronization 热路径
- metadata lookup / reflection hot path
- interop / marshaling / callback path
- hot-update load / patch dispatch / bridge path
- mixed managed/native/interpreter bridge path

展示要求：

- 默认使用 managed time 作为对比基线；
- 同时展示 native、interpreter 或 patch 路径结果；
- 必须能 drill-down 到 `solution/archetype/case` 级别。

## 9. 新能力接入规则

后续新增能力点时，按如下顺序接入：

1. 先决定它属于哪个 `capability_item`。
2. 再决定 owner 在哪个 retained subject。
3. 再决定应落在哪个 archetype 或 feature slice。
4. 先做 proof，再决定是否需要 benchmark。
5. 最后补 support-state、dashboard、docs。

禁止顺序：

- 先加一个新 subject。
- 先发明一个新的 string 调度键。
- 先写 benchmark，再补正确性验证。
- 先在 docs 声称支持，再补 subject 证据。

## 10. 结论

理想中的完整测试体系不是“更多测试工程”，而是：

- 以 retained subjects 为固定 owner；
- 以 capability item 为最小验收单元；
- 以 archetype 表达工程级验证；
- 以 proof + selective benchmark + support-state + docs 构成完整证据链。

因此，后续推进必须以多阶段 roadmap 方式进行。
