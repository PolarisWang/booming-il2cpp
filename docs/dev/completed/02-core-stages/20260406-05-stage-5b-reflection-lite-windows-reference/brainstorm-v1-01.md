# Stage 5B ReflectionLite Windows Reference Brainstorm

Date: 2026-04-06 14:38:08 +08:00
Status: concluded

## 任务意图

Stage 5A 已经证明 `GenericEchoClosedMinimal` 能沿现有 `Windows x64` reference proof spine 跑通真实 build/run，并拿到 `generic-ok` / `exit code 0` 的证据。  
Stage 5B 的任务不是立刻把现有完整 smoke `ReflectionLite` 全量搬到这条 spine 上，而是先找出“reflection / metadata 查询第一次真正卡住的最短路径”，再用一个最小 proof 把它打穿。

## Stage 5B 必须守住的边界

- 继续以 Stage 4 `HelloWorldObject` 与 Stage 5A `GenericEcho` gates 作为固定基线，不允许回退。
- 生产代码继续 sample-agnostic；任何 `ReflectionLite` 特化都只能留在 `tests/**`、`artifacts/**` 和任务文档中。
- 第一轮只留在 `Windows x64` reference spine，不并行展开 `macOS` parity。
- 第一轮只覆盖“reflection / metadata 查询”这一最小子集，不把 `MethodInfo.Invoke`、`FieldInfo.GetValue`、`EventInfo`、`MakeGenericMethod` 一口气带进来。

## 备选方案

### 方案 A：直接拿现有 smoke `ReflectionLite`

- 做法：直接尝试让 `tests/smoke/input/ReflectionLite/Program.cs` 通过当前 Stage 4 / 5A spine。
- 优点：
  - 与现有 smoke 样例完全一致。
  - 一次性覆盖 `Type`、`Field`、`Property`、`Event`、`Method`、`Parameter` 和泛型方法相关查询。
- 缺点：
  - `GetEvent`、`MakeGenericMethod`、泛型方法返回类型检查会把首轮 blocker 混成 metadata 查询、generic context 与高阶反射三个面。
  - 很难判断失败到底来自 metadata materialization、runtime lookup，还是更高阶 reflection surface。

### 方案 B：最小 query-only ReflectionLite proof

- 做法：新建一个最小 proof 输入，只覆盖：
  - `typeof(ReflectionFixture<int>)`
  - `GetField`
  - `GetProperty`
  - `GetMethod`
  - `GetParameters`
  - `GetGenericTypeDefinition`
  - 稳定字符串输出
- 优点：
  - 能真实覆盖 “closed generic type + metadata query” 这条最短 reflection 路径。
  - 可以把首轮 blocker 收敛到 metadata registration / reflection lookup / query runtime glue。
  - 不会过早把 `EventInfo`、`MakeGenericMethod`、reflection invoke 混进来。
- 缺点：
  - 第一轮并不等于完整 smoke `ReflectionLite` parity。

### 方案 C：只做非泛型 reflection 查询

- 做法：只覆盖非泛型 `Type` / `Method` / `Field` 查询，不涉及 closed generic type。
- 优点：
  - 变更最小。
- 缺点：
  - 避开了 `ReflectionLite` 最容易与 Stage 5A generic foundation 交汇的部分。
  - 后续还得再开一轮补 generic type definition / closed generic metadata query。

## 当前推荐

- 采用 **方案 B**。

## 额外判断

Stage 5B 适合从 Stage 5 roadmap 进入单个 `plan` 子任务：

1. 当前真实 blocker 已经足够聚焦到“最小 reflection / metadata 查询子集”。
2. 可以先用 TDD 写出 query-only RED，再逐层补 managed metadata、native lookup 与 proof host。
3. `EventInfo`、`MakeGenericMethod`、`MethodInfo.Invoke` 等更高阶 surface 应留在 Stage 5B 后续扩展或 follow-up，而不是混入第一轮派生任务。
