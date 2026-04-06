# Stage 4 Native Bootstrap And Reference Proof Brainstorm

Date: 2026-04-06 09:43:58 +08:00
Status: concluded

## 任务意图

Stage 4 负责把 Stage 3 已经冻结的 managed closure bundle 真正推进到 native proof。它不再讨论 managed pipeline 该输出什么，而是证明这些输出已经足以驱动第一条 reference-platform vertical slice。

当前仓库状态有几个关键信号：

- `artifacts/proof/managed-closure/HelloWorldObject/` 已经具备 Stage 3 的五件套 bundle。
- `contracts/native/v0/` 与 `tests/contracts/native/*` 已经冻结 ABI / bridge proof-boundary 与 compile-only smoke。
- `windows-x64-reference` 预设已经存在，但当前只绑定到 compile-only contract smoke。
- `src/native/` 还不存在，说明 Stage 4 不是“补一根线”，而是第一次把 native proof 骨架真正落地。

## Stage 4 必须守住的边界

- 只消费 Stage 3 bundle，不回头修改 managed closure contract。
- 继续遵守 Stage 2 的 canonical truth：
  - stdout: `Hello, World!`
  - exit code: `0`
  - generated direct-call landing
- 第一轮只锁定一个真正可运行的 host，不并行铺开多平台。
- 不把 compile-only contract smoke 当成“已经完成 native proof”。

## 备选方案

### 方案 A：继续停留在 contract/example 层，补一个假 proof host

- 做法：尽量复用 `contracts/native/examples/v0/*` 与 compile-only smoke，只新增极少量 proof 样例外壳。
- 优点：改动最少，短期看起来推进很快。
- 缺点：无法证明 Stage 3 bundle 真正能驱动 native runtime，也无法满足 roadmap 对 Stage 4 的完成定义。

### 方案 B：以 `Windows x64` 为首个 host，做一条最小 native vertical slice

- 做法：新增 `src/native/runtime-core`、`src/native/bootstrap`、`src/native/support`，让 Stage 3 bundle 先落成 first generated C++ emission，再由 proof host 真正输出 `Hello, World!`。
- 优点：直接命中 Stage 4 的真实目标；边界最清楚；与现有 `windows-x64-reference` 预设和 `reference-desktop-gates.md` 的“Windows 主基线”语义一致。
- 缺点：这是仓库第一次真正落地 native proof 代码，任务会明显重于前两个 contract-only 阶段。

### 方案 C：同时铺开 `Windows x64 + macOS` 的双 reference desktop proof

- 做法：按双 reference desktop 语义，一次性建立 Windows 与 macOS 两条 native proof 骨架。
- 优点：长期平台叙事更完整。
- 缺点：会把本轮的真实 blocker 从 runtime/bootstrap 转移成多平台构建壳层，明显超出 first proof 的收敛目标。

## 当前推荐

- 采用 **方案 B**。

## 补充判断

Stage 4 适合直接进入单个 `plan`，而不是再拆成新 `roadmap`：

- 上游输入已经稳定，Stage 2 / Stage 3 的边界都已冻结。
- 当前需要的是一个可执行的 chunked implementation plan，而不是再讨论阶段排序。
- `runtime-core`、`bootstrap`、`support`、generated emission 与 proof host 虽然是不同模块，但在 first proof 中强耦合，拆成多个子任务反而会增加状态管理成本。

因此，Stage 4 的下一步应是基于本结论写 `design-v1-01.md`，随后进入 `plan-v1-01.md`，而不是继续派生新的 roadmap-child。
