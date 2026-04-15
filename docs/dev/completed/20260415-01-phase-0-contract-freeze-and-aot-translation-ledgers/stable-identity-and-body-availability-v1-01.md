# Stable Identity And Body Availability Freeze v1.01

Date: 2026-04-15 00:11:06 +08:00
Status: draft-freeze

## 1. 目标

冻结母 roadmap 的两个核心口径：

- `Q2`: public stable id 采用 `family enum + item enum` 复合 numeric identity。
- `body availability` 进入明确、可枚举、可序列化的实现契约，不再继续扩大 string 协议面。

## 2. Stable Capability Identity

### 2.1 外层稳定身份

能力的外层稳定身份由以下两部分组成：

- `ChaosCapabilityFamily : byte`
- `ChaosCapabilityItem : ushort`

组合规则冻结为：

```text
stable_capability_id = ((uint)family << 16) | item
```

这意味着：

- family 是一级分桶；
- item 是二级稳定编号；
- 对外报表、gate、dashboard、编译产物索引都可以使用同一套 numeric identity；
- 不需要把 capability identity 再编码成新的字符串协议。

### 2.2 `ChaosUnitTest` / `ChaosBenchmark` 的最小公开契约

proof / benchmark 的声明入口继续固定在：

- `src/reference/Chaos.TestFramework`

最小公开信息冻结为：

- `CapabilityFamily`
- `Capability`
- `Archetype`
- `Requires`
- `HotUpdateCapability`
- benchmark 额外拥有 `Modes` / `Metrics` / `WarmupCount` / `IterationCount` / `InvocationCount`

`Alias` 可以保留，但只用于：

- 人类可读展示
- 报表短标签

`Alias` 不是稳定路由主键，也不能替代 numeric identity。

## 3. Subject Dispatch Surface 收口

当前 `Chaos.TestFramework` 中仍然存在两类入口：

- 紧凑 numeric surface
  - `ChaosSubjectEntryKind`
  - `ChaosSubjectSlice`
- 声明发现后的 string surface
  - `--chaos-source-entry=...`

Phase 0 冻结结论：

- 可以保留现有 string surface 作为过渡兼容入口；
- 但禁止继续扩大它的语义承载面；
- 后续新增的调度、分类、reporting 需求，优先挂到枚举和 compact numeric identity，而不是继续发明新的 source-entry 字符串协议。

## 4. `BodyAvailability` 冻结草案

当前 contracts 中 `BodyAvailability` 仍以字符串出现在若干 artifact 模型里。Phase 0 冻结的下一阶段实现口径如下：

```csharp
public enum BodyAvailability : byte
{
    None = 0,
    NativeGenerated = 1,
    InterpreterReady = 2,
    BridgeDispatch = 3,
    MetadataOnly = 4,
    ExternalRuntime = 5,
    Unsupported = 6,
}
```

### 4.1 含义

- `None`
  - 尚未解析或不应出现于正式产物。
- `NativeGenerated`
  - AOT 路径负责生成本地可执行 body。
- `InterpreterReady`
  - body 由 interpreter / hot-update 运行时消费。
- `BridgeDispatch`
  - body 需要经 bridge/thunk 或 host-patch dispatch 进入另一条执行线。
- `MetadataOnly`
  - 当前只保留元数据闭包，不承诺可执行 body。
- `ExternalRuntime`
  - body 由 import、icall、宿主 runtime 或外部 ABI 提供。
- `Unsupported`
  - 当前明确不支持，或没有正确实现。

### 4.2 归类规则

| 场景 | `BodyAvailability` |
| --- | --- |
| 完成 AOT 闭包并生成 native body | `NativeGenerated` |
| mixed execution 中明确走 interpreter | `InterpreterReady` |
| host 调 patch / patch 调 host 的桥接路径 | `BridgeDispatch` |
| 仅为反射、metadata supplement、preserve 而保留 | `MetadataOnly` |
| P/Invoke、icall、引擎绑定等外部实现 | `ExternalRuntime` |
| 当前 roadmap 明确 deferred / unsupported | `Unsupported` |

## 5. Reason Code 的冻结要求

support state / unsupported state 的 reason code 继续复用：

- `platform_host_gap`
- `toolchain_gap`
- `runtime_gap`
- `metadata_gap`
- `compatibility_gap`
- `policy_blocked`
- `scope_not_targeted`

Phase 0 冻结的额外约束：

- 代码层应尽快转为紧凑枚举或等价紧凑类型；
- 对外序列化仍可输出稳定标识名；
- 不允许为了报表方便重新引入新的自由字符串 reason。

## 6. 对后续阶段的约束

- Phase 1 不再为 subject 调度新增 string 主键。
- Phase 2 必须产出 `BodyAvailabilityResolver`，并以本文件的枚举语义为目标。
- Phase 3 的 `HybridDispatchResolver`、bridge/thunk 与 supplemental metadata 只能建立在同一套 method identity + body availability 之上。
- Phase 4 的 `AotCoreIr` 和 Phase 6 的 dashboard 都应消费同一套 compact identity / body availability，而不是各自定义“看起来相似”的字符串字段。
