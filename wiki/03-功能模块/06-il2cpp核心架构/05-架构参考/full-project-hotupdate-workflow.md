# Full-Project HotUpdate Workflow

## 目的

这份文档定义仓库中完整热更工程的 canonical 组织方式。目标不是给出某个临时 demo，而是固定一条长期可复用的 `host + shared contracts + patch modules` 工作流。

## Canonical 样例

当前 canonical archetype 位于：

- `subjects/HotUpdateHostPack/source/Archetypes/FullProjectHotUpdateSolution`

当前目录结构：

```text
FullProjectHotUpdateSolution/
  FullProjectHotUpdateSolution.sln
  HostApp/
    GoldenHotUpdateHost.App.csproj
    Program.cs
  SharedContracts/
    GoldenHotUpdate.SharedContracts.csproj
    Contracts.cs
  PatchModules/
    GoldenHotUpdate.PatchModule.csproj
    PatchFeature.cs
```

## 三类工程边界

### HostApp

职责：

- 负责加载 patch assembly。
- 提供 host 侧 runtime、生命周期、回滚与发布控制点。
- 调用 shared contract，而不是直接依赖 patch 细节。

当前样例中，`HostApp/Program.cs` 覆盖了以下行为：

- 从运行目录定位 `GoldenHotUpdate.PatchModule.dll`
- `Assembly.LoadFrom(...)` 加载 patch
- 通过 `IPatchFeature` 完成 shared contract binding
- 验证 host-to-patch callback、generic roundtrip、异常传播

### SharedContracts

职责：

- 定义 host 与 patch 共享的最小稳定接口。
- 只放长期稳定的 contract，不放 host 或 patch 的实现细节。

当前样例中的关键接口：

- `IHostMath`
- `IPatchFeature`

Shared contract 是热更工程最重要的稳定面之一。凡是可能频繁变化的实现细节，都不应直接暴露到这里。

### PatchModules

职责：

- 实现 shared contract。
- 承载真正需要热更的业务逻辑。
- 通过 package metadata、版本带宽与 rollback 策略进入发布链。

当前 `PatchFeature.cs` 展示了：

- 固定返回值替换
- 调用 host 提供的 callback
- generic `Echo<T>`
- delegate callback `Apply(...)`
- patch 内抛错并让 host 断言异常内容

## 作者使用规则

### 1. Proof / benchmark 仍然用 `Chaos.TestFramework`

完整热更工程的功能条目必须继续通过：

- `[ChaosUnitTest(...)]`
- `[ChaosBenchmark(...)]`
- `Assert`

进行声明与验证。

不要回到：

- `Console.WriteLine` + 外部脚本判定
- 独立 `scenario/validation` 目录
- 自定义 string 调度协议

### 2. Shared contract 优先于实现直连

Host 与 patch 的交互，应优先走 interface / delegate / DTO contract，而不是让 host 直接查找 patch 内部类并假定其实现细节。

允许的例外：

- 仅用于 assembly 加载与 bootstrap 的反射入口

### 3. Patch package 的公开 contract 要最小化

公开 contract 应只包含：

- package identity
- target AOT version / compatibility band
- metadata supplement 关联信息
- integrity / hash / rollback 所需信息

不要把 patch 内部实现状态暴露为长期稳定字段。

## 标准验证流

### 本地正确性

至少应覆盖：

1. package load
2. shared contract binding
3. host-to-patch callback
4. patch-to-host callback
5. generic / delegate / exception 跨边界
6. metadata supplement / package integrity
7. version rollback / compatibility band

这些能力点分别通过 `ChaosHotUpdateCapability` 表达，而不是靠路径命名或文档约定。

### 性能验证

benchmark 主要关注：

- load / activate
- dispatch / callback path
- roundtrip cost

managed baseline 是 dashboard 的基线，native / interpreter 作为对照面展示。

### 发布门禁

完整热更工程进入发布前，必须同时看：

- `version-compatibility-matrix.md`
- `release-checklist.md`
- `ios-distribution-policy.md`
- `unsupported-feature-policy.md`

## 版本与回滚

完整热更工程不能只证明“补丁能加载”，还必须证明：

- patch 声明自己面向哪个 AOT 主包版本带宽
- rollback 后 host 能回到 AOT-only 状态
- compatibility 校验与 release gate 使用同一套规则

当前统一规则见：

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/version-compatibility-matrix.md`

## 与其他 retained subjects 的关系

- `SolutionCorePack` 负责“完整 solution 如何进入 IL2CPP native 主线”。
- `HotUpdateHostPack` 负责“完整热更工程如何组织与发布”。
- `MixedExecutionFeaturePack` 负责“interpreter / native / managed 混合边界如何保持语义一致”。

完整热更工程如果涉及 mixed execution，只复用 mixed 语义，不把 owner 转移到别的 subject。

## 何时新增新热更 archetype

只有当出现以下情况才考虑新增：

1. solution graph 已与 `FullProjectHotUpdateSolution` 明显不同。
2. 现有 archetype 无法表达新的长期稳定工程组织方式。
3. 问题不是一个 capability slice，而是真正新的工程结构。

否则应优先：

- 在 `FullProjectHotUpdateSolution` 内补 capability 条目
- 或在 `HotUpdateHostPack` 下新增 proof / benchmark 条目
