# IL2CPP 核心能力补齐与 Subject 完整验证 Brainstorm v1.01

Date: 2026-04-14 12:41:36 +08:00
Status: draft-approved

## 1. 触发背景

上一轮 `20260414-04-il2cpp-hotupdate-capability-expansion-roadmap` 已经完成了基础收口：

- retained subject 固定为：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- `src/reference/Chaos.TestFramework` 成为统一的 managed 测试声明面。
- capability taxonomy、solution archetype、dashboard、registry、reporting、docs 已经切到统一模型。
- 证明了“subject 结构整理”和“metadata 驱动发现”这条路线可行。

这意味着后续主线不再是“继续挪目录”，而是回答一个更大的问题：

> 如何把 IL2CPP 与热更的核心能力系统性补齐，并让 retained subjects 对这些能力点具备长期可扩展、工程级、可发布判断的验证能力？

## 2. 用户目标

结合本轮对话中已经确认过的要求，新的长期规划必须同时满足：

1. C# 的核心 feature 要能够被正确翻译到 IL2CPP 与热更路径，并在 subject 中有对应验证。
2. solution 级验证必须成为核心验证能力，而不只是单个 proof。
3. 要覆盖 `ProjectReference`、`Reference`、`PackageReference`、`corelib/reference bundle` 等多种真实依赖图。
4. subject source 继续保持 solution 模式，不回退到大量 `scenario/validation/workload` 外置目录。
5. 不继续扩大 string 协议作为 subject 内部调度面；优先使用轻量枚举和 attribute metadata。
6. benchmark 继续是“选择性高价值路径”，而不是“每个语法点都机械配一个 benchmark”。
7. support-state 继续留在外层 matrix / reporting / docs，不塞回 attribute。

## 3. 关键约束

- 顶层 subject 数量不能继续增长。
- `src/managed` 仍是 IL2CPP 核心 managed 实现，不放测试框架。
- `src/reference/Chaos.TestFramework` 仍是外部 C# 测试 authoring 的唯一公共入口。
- proof 继续使用 `Assert`，而不是 `Console.WriteLine` 再外部判定。
- benchmark dashboard 的默认基线继续使用 managed time。
- 语法点可以机械拆成单文件，但应优先合并进共享 project，而不是继续创造很多 project。

## 4. 为什么不能直接写单阶段执行计划

当前任务不是单次实现，而是新的长期母线。它同时跨越：

1. capability inventory 与 coverage ledger 的冻结。
2. retained subjects 的验证闭环补强。
3. runtime / interop / metadata / AOT / mixed / hot-update 多条技术主线。
4. benchmark、support-state、dashboard、docs、发布判断的统一外层表达。
5. 未来多个子任务的派生顺序与验收标准。

因此这里必须先产出 roadmap，而不是直接进入一个单阶段 plan。

## 5. 方案比较

### 方案 A：继续按历史语法点逐项补齐

- 做法：
  - 把历史语法点继续机械拆成更细颗粒度 proof。
  - 按缺哪个补哪个推进。
- 优点：
  - 进入实现快，局部任务边界清晰。
- 缺点：
  - 容易把 roadmap 退化成“语法点打表”。
  - 很难稳定回答“这个能力点应该落在哪个 subject / archetype / solution 验证里”。
  - 工程级 solution 验证、发布判断与 dashboard 维度会继续漂移。

### 方案 B：只按 product solution 场景推进

- 做法：
  - 以完整 solution archetype 为唯一驱动，把运行时语义差异都吸收到 solution 工程中。
- 优点：
  - 更贴近真实项目接入。
  - 能直接拉高工程级验证价值。
- 缺点：
  - 语义定位成本高。
  - 对 runtime / interop / metadata 等低层能力的缺口不够清晰。
  - 容易出现“大 solution 通过，但不知道哪类能力还缺”的问题。

### 方案 C：Capability 驱动的混合路线

- 做法：
  - 继续使用两级 capability taxonomy 作为能力真源。
  - 每个能力点都绑定 `owner subject`、`owner archetype`、`assert proof`、`selective benchmark`、`support-state` 证据。
  - solution archetype 负责工程级闭环，feature slice 负责语义覆盖，二者统一落到 retained subjects 内部。
- 优点：
  - 能同时保持语义清晰度与工程级价值。
  - 能稳定约束 subject 不再膨胀。
  - 与现有 capability/archetype/docs/dashboard 基线完全兼容。
- 缺点：
  - 前置设计与台账冻结工作较重。

## 6. 推荐结论

推荐采用方案 C，并进入新的多阶段 roadmap。核心判断：

- 现有仓库已经完成了 capability/archetype/subject 的基础收口，再回到方案 A 或 B 都会浪费这层基线。
- “subject 工程有完整验证能力”本质上不是多做几个 proof，而是让每个能力点都能从 source 声明一路映射到工程级验证、benchmark、reporting 与 docs。
- 这类工作天然跨多次会话和多阶段子任务，因此必须使用 roadmap。

## 7. 本次 Brainstorm 结论

新的 follow-up 应聚焦于：

1. 冻结 coverage ledger，明确每个 capability item 的 owner 与证据形式。
2. 先补齐 retained subjects 自己的验证闭环，再继续往更深的运行时能力推进。
3. 按三个技术批次推进 IL2CPP 核心能力：
   - runtime semantics
   - low-level/interop
   - metadata/AOT
4. 再把 full-solution、mixed execution、hot-update、release gate 拉到统一的工程级和产品级闭环。

结论：进入 `roadmap`，而不是 `writing-plans`。
