# DotNet Foundation Pack Nativeization Brainstorm v1.01

Date: 2026-04-18 01:19:47 +08:00
Status: approved-for-roadmap

## 1. 背景

当前仓库的 AOT 主线本质上是 `entry-driven`：

- 输入来自 `dotnet-project` 或 `managed-dlls`
- 由 entry project / entry method 驱动 closure
- `Loader -> SemanticWorld -> Linker -> CodeGen -> NativeAotEmitter`
- 最终围绕 reachable methods 生成 native 产物

这条主线适合项目级 proof，但不适合“先把 `.NET 8 / .NET 10` 的 `Microsoft.NETCore.App` 基础框架程序集完整 native 化，再逐层向上认证复杂 DLL”这一目标。

用户已明确要求：

- 主线起点是官方 `Microsoft.NETCore.App`
- 第一阶段只覆盖 `System.Private.CoreLib / mscorlib / netstandard / System.*`
- 不把 `Microsoft.AspNetCore.App` / `Microsoft.WindowsDesktop.App` 纳入第一阶段
- 不把仓库业务 DLL 作为主线起点
- 完成标准不是“关键 proof 通过”，而是“单个 DLL 的全部公开 API / IL 语义都必须可 native 化”

## 2. 现状证据

### 2.1 当前仓库仍是项目/入口驱动

来自现有实现的事实：

- `ProjectGraphLoader` 以 entry project 为起点，扫描 `ProjectReference` 与 package assets。
- `LoaderStage.LoadMultiple` 以 `InputAssemblyPath + AdditionalAssemblyPaths` 为装载集合。
- 现有 subject.manifest 也仍然以 `primaryProjectPath + entry` 建模。

这说明当前主线天然偏向“项目闭包”，而不是“官方框架程序集分层认证”。

### 2.2 本机只具备 `.NET 8` / `.NET 9`

本机环境已确认：

- `dotnet --list-sdks` 只有 `8.0.402` 与 `9.0.101`
- `Microsoft.NETCore.App.Ref` 本机可见版本为 `8.0.8`、`8.0.11`、`9.0.0`
- `Microsoft.NETCore.App` 本机可见版本为 `8.0.8`、`8.0.11`、`9.0.0`
- 本机没有 `.NET 10` ref/runtime packs

因此 `.NET 10` 只能通过后续显式引入官方快照来支持，不能假设执行机天然自带。

### 2.3 `Microsoft.NETCore.App` 内部存在多类程序集

从本机 `Microsoft.NETCore.App.Ref` / `Microsoft.NETCore.App` 可见：

- `.NET 8` ref pack DLL 数量为 `163`
- `.NET 8` runtime pack DLL 数量为 `180`
- `.NET 9` ref pack DLL 数量为 `164`
- `.NET 9` runtime pack DLL 数量为 `182`

这些 DLL 不能当成一类对象处理，至少要区分：

- contract/facade
- compatibility shim
- implementation
- runtime-backed

否则“单 DLL 完整 native 化”的完成标准会漂移。

## 3. 方案比较

### 方案 A：沿用现有 entry-driven 主线，只补更多基础库 proof

- 做法：继续从 `SolutionCorePack` 或其他 subject 的入口出发，逐步暴露 `System.*` 缺口并修补
- 优点：复用现有主线快
- 问题：
  - 仍然是“入口带动能力发现”，不是“DLL 分层认证”
  - 无法证明“单 DLL 全公开 surface 已完成”
  - 复杂 DLL 容易因为 proof 覆盖偏差出现伪完成

### 方案 B：先翻仓库业务 DLL，再逐步替换为官方基础 DLL

- 做法：以项目 DLL 练通整条 native 主线，再逆向逼近 `Microsoft.NETCore.App`
- 优点：短期可快速看到 native 输出
- 问题：
  - 与用户目标相反
  - 会把项目业务语义和基础框架语义混杂
  - 无法建立官方框架程序集的稳定认证边界

### 方案 C：新建 `framework-pack-driven` 主线

- 做法：
  - 直接以官方 `Microsoft.NETCore.App.Ref + Microsoft.NETCore.App` 为输入
  - 先建立 `framework catalog / assembly classification / surface ledger / layer plan`
  - 再按单 DLL 完整 native 化推进
  - 最后由项目、mixed execution、hotupdate 作为消费者验证层
- 优点：
  - 与目标一致
  - 可按 DLL 建立完整 completion gate
  - 可天然支持 `.NET 8` 与 `.NET 10` 双版本 diff
  - 可把 hotupdate 从“主驱动”降为“消费者验证”
- 问题：
  - 需要新增 source model、owner subject、认证账本与 per-assembly 产物
  - 前期基础设施投入较大

## 4. 结论

- 采用方案 C。
- 当前任务应进入新的 roadmap，而不是继续沿用 `20260416-01` 的 entry-driven AOT 拓展路线。
- 新路线的核心不是“再加更多 proof”，而是新增一条正式的：

`framework-pack -> assembly classification -> per-assembly certification -> consumer validation`

主线。

## 5. 推荐的高层阶段

- Phase 0: `Microsoft.NETCore.App` 双版本资产目录、快照策略与程序集分类
- Phase 1: contract/facade/shim 层认证
- Phase 2: `System.Private.CoreLib` 完整 native 化底座
- Phase 3: `System.Private.CoreLib` 语义族批次收口
- Phase 4: 依赖驱动的核心 `System.*` 分层 native 化
- Phase 5: 复杂 BCL 与常用外部 DLL 验证
- Phase 6: hotupdate 消费者验证
- Phase 7: `.NET 10` delta closure 与 release gate

## 6. 待显式固定的执行假设

- 默认建议：`.NET 10` 官方 ref/runtime packs 作为仓库内冻结快照管理
- 如果用户后续改为“执行机外部预装提供”，只影响 Phase 0 资产策略，不改变主线分层与认证模型

## 7. 分流判断

本任务满足以下条件：

- 需要多阶段推进
- 需要多个独立 child task
- 会跨多次会话持续推进
- 现阶段还不能直接写成稳定单次 implementation plan

因此必须进入 `roadmap`，不适合直接进入单次 `plan`。
