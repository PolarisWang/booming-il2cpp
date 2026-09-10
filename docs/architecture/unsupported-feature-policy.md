# Unsupported Feature Policy

## 目的

这份文档定义当 IL2CPP、热更或 mixed execution 不能完整支持某项能力时，仓库如何对外表达。目标是避免：

- “看起来像支持，但没有证据”
- “其实不支持，但没有统一说法”
- “partial 和 unsupported 混用，无法进入 gate / dashboard / 文档”

## 状态词汇

统一使用四个状态：

| 状态 | 含义 | 是否必须带 reason code |
| --- | --- | --- |
| `supported` | 在声明范围内有稳定证据，且无已知降级 | 否 |
| `partial` | 有证据，但只覆盖部分平台、模式或语义子集 | 是 |
| `unsupported` | 当前明确不支持 | 是 |
| `not-applicable` | 对该 subject / archetype / mode 不适用 | 否 |

## 适用范围

support state 只能出现在外层 contract 中：

- subject matrix
- compatibility matrix
- reporting payload
- dashboard summary
- architecture / release 文档

它不应写进 `Chaos.TestFramework` attribute。

## Reason Code 规则

### 1. 必须稳定

reason code 一旦公开，就必须保持稳定，便于：

- 报表聚合
- gate 过滤
- 发布说明引用

### 2. 推荐使用“代码内枚举，序列化为稳定标识”

代码层推荐：

- 用枚举或等价紧凑类型维护 reason code

对外序列化时：

- 使用稳定的枚举成员名或等价稳定标识

这条规则的目的，是避免在内部继续扩大 string 调度面，同时保留外层报告可读性。

### 3. 推荐 reason code 分类

| reason code | 含义 |
| --- | --- |
| `platform_host_gap` | 宿主平台不具备该能力的前置条件 |
| `toolchain_gap` | 当前 IL2CPP / codegen / backend 未实现该能力 |
| `runtime_gap` | 运行时行为未闭合，结果不正确或不稳定 |
| `metadata_gap` | metadata supplement、preserve、reflection 或 registration 不完整 |
| `compatibility_gap` | 版本带宽、rollback 或 patch contract 未满足 |
| `policy_blocked` | 平台/发行策略禁止，例如 iOS distribution 边界 |
| `scope_not_targeted` | 本轮 roadmap 明确不覆盖该子能力 |

如果现有分类足够表达，就不要再新增新的 reason code。

## 最小公开记录

当某项能力是 `partial` 或 `unsupported` 时，最少要公开：

| 字段 | 含义 |
| --- | --- |
| capability / archetype | 哪个能力点或工程形态 |
| state | `partial` 或 `unsupported` |
| reason code | 为什么 |
| affected modes | `managed` / `native` / `interpreter` 哪些受影响 |
| affected platforms | 哪些 host/target 受影响 |
| evidence | 失败证据、缺口定位或缺失原因 |
| next action | 是否已有明确 follow-up |

## 使用边界

### 何时标记为 `partial`

以下情况属于 `partial`：

- 只在 `managed` 成功，`native` 或 `interpreter` 缺证据
- 只覆盖单平台 host
- 语义主路径可用，但特定边界未闭合
- 有 benchmark 结果，但 correctness 闭环不完整

### 何时标记为 `unsupported`

以下情况属于 `unsupported`：

- 已知必然失败
- 当前没有正确实现
- 平台策略明确禁止
- roadmap 明确决定本轮不支持且无现成 fallback

### 何时标记为 `not-applicable`

以下情况属于 `not-applicable`：

- `SolutionCorePack` 不拥有热更 package rollback 能力
- `HotUpdateHostPack` 不负责 corelib reference 闭包
- `MixedExecutionFeaturePack` 不负责完整 package compatibility 发布带宽

`not-applicable` 不是失败，也不能拿来掩盖 `unsupported`。

## 与 Gate / Dashboard 的关系

- gate 只看统一 support state 与 reason code，不看临时口头说明。
- dashboard 应突出显示：
  - 哪些 mode 有数据
  - 哪些 mode 缺数据
  - 缺数据是 `missing evidence` 还是 `unsupported`
- 发布 checklist 应把 `unsupported feature report` 视为必查项。

## 维护规则

1. 新增 partial / unsupported 项时，必须同步更新外层报告或文档，而不是只在代码注释里说明。
2. `partial` 与 `unsupported` 必须带 reason code。
3. `not-applicable` 只能用于 ownership 边界，不得拿来掩盖实现缺口。
