# managed test host 不得出现 per-subject 宿主项目

**日期：** 2026-04-20  
**状态：** 已决策，shared runtime host cutover 已落地第一阶段

---

## 背景

历史实现里，部分 subject 通过 `Program.cs`、`ProofEntry`、`BenchmarkEntry` 或预留的 `DeclaredProofHost` / `DeclaredBenchmarkHost` 语义承接 managed proof / benchmark 执行。

这条路径的问题很明确：

- per-subject managed 宿主会造成重复模板与重复项目
- managed 执行入口分散在 subject 侧，难以统一治理
- 生成链路与运行链路都需要兼容旧 `Declared*Host` 产物，workspace 复杂度持续升高

与此同时，`Chaos.TestFramework.Runtime` 已经具备 shared runtime host 所需的核心能力：collection loader、host arguments、proof runner、benchmark runner 与统一入口项目。

---

## 讨论过的方案

### 方案 A：继续保留 per-subject 薄壳宿主（否决）

在每个 subject 下继续生成 `DeclaredProofHost`、`DeclaredBenchmarkHost`，只把真正执行逻辑委托给 `Chaos.TestFramework.Runtime`。

否决原因：

- 仍然保留重复 csproj / `Program.cs` 模板
- workspace 与 solution 仍需感知一组历史遗留 managed 宿主
- 没有解决 managed proof / benchmark 入口分散的问题

### 方案 B：managed proof / benchmark 统一切到 shared runtime host（采纳）

把 managed proof / benchmark 的正式宿主统一收敛到：

- `src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj`
- `ChaosProofRunner.Run(string[] args)`
- `ChaosBenchmarkRunner.Run(string[] args)`

上游通过 collection file 与 entry index 直接调用 shared runtime host，不再经过任何 per-subject `Main()` 包装。

### 方案 C：立即删除所有 `Program.cs` / `ProofEntry` / `BenchmarkEntry`（本轮不采纳）

这一步当前风险过高，不作为第一阶段 cutover 的范围。

原因：

- `source.entry` 仍是现有 manifest、兼容性用例与 native owner contract 的一部分
- `CoreRuntimeFeatures/ProofEntry::Run()` 仍被多组 compatibility / manifest contract 测试视为稳定契约
- managed 执行已经与它解耦，但 native / trace / compatibility 侧还未完成对应迁移

---

## 决策

本次正式决策如下：

1. managed proof / benchmark 的正式宿主统一为 `Chaos.TestFramework.Runtime`
2. `proof-host` / `benchmark-host` 必须解析到共享 runtime host，而不是 subject 下的独立 managed 宿主项目
3. subject 目录下禁止继续生成 `DeclaredProofHost`、`DeclaredBenchmarkHost` 以及其他仅服务 managed proof / benchmark 的 per-subject 宿主
4. workspace 生成阶段必须清理历史 `Declared*Host.csproj` 与 `ChaosGeneratedDeclaredTests.g.cs` / `ChaosGeneratedDeclaredBenchmarks.g.cs`
5. 若 subject 中仍保留 `Program.cs`、`ProofEntry`、`BenchmarkEntry`，它们只能作为 native owner contract、trace/export 或兼容过渡入口存在，不得参与 managed proof / benchmark 执行

---

## 已落地内容

第一阶段 cutover 已经落地：

- `project_workspace` 生成 `proof-host` / `benchmark-host` 时，已统一指向 `src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj`
- shared runtime host 的 `assemblyName` 固定为 `Chaos.TestFramework.Runtime`
- shared runtime host 的 `executionModel` 固定为 `shared-runtime-host`
- workspace 生成阶段会自动删除历史 per-subject managed 宿主产物与对应 generated source
- solution 写入阶段已做项目去重，避免 shared runtime host 被重复加入
- 实际 subject `.csproj` 中，旧 per-subject managed host 的 friend access 已按本轮边界做了首批清理

---

## 当前过渡边界

当前真实状态不是“所有 `Program.cs` / `ProofEntry` 都已删除”，而是：

- managed proof / benchmark 已经切离 per-subject host
- `Program.cs` / `ProofEntry` / `BenchmarkEntry` 仍可能在个别 subject 中保留
- 这些残留项当前只允许承担 native owner contract、manifest 兼容、trace/export 等过渡职责
- `benchmark-host-native` 等 native 侧链路仍属于过渡范围，不在本轮 cutover 内一并移除

换言之，第一阶段已经完成“managed host 去 per-subject 化”，但尚未完成“native owner contract 去 `source.entry` 化”。

---

## 后续工作

后续如果要继续清理，顺序必须是：

1. 先迁移 native owner contract / manifest / compatibility 对 `source.entry` 与 `ProofEntry` 的依赖
2. 再删除遗留 `Program.cs`、`ProofEntry`、`BenchmarkEntry`
3. 最后移除剩余仅为 native / transitional generation 保留的 `<InternalsVisibleTo>`

在这之前，任何 authority、wiki 或规范文档都不得把“禁止 per-subject managed host”错误表述成“所有 subject 入口残留都已经完成删除”。
