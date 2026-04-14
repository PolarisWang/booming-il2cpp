# Subject Generated Test Workspace And Unified Test Entry Brainstorm

Date: 2026-04-14
Status: confirmed

## 用户需求

1. 每个 `subject` 工程都要生成完整的四类工程：
   - managed projects
   - managed test projects
   - native projects
   - native test projects
2. 检查当前测试入口是否已经统一从 native test project 走；如果不是，要统一入口，但不能把工程语义做歪。
3. 取消手工注册测试代码，改为基于 `ChaosUnitTest` / `ChaosBenchmark` attribute 自动发现，再生成对应的 unit test / benchmark 逻辑，并映射到 native test project。
4. 统一入口菜单也要继承这套生成功能。
5. 这次设计必须把旧逻辑清理纳入主目标，避免长期双轨和历史壳残留。

## 现状分析

### 已有能力

- `run generate project ...` 已有统一入口，入口在 `build/toolchains/run/commands/project.py`。
- subject workspace 已能生成 managed solution 和 native project，核心生成逻辑在 `build/toolchains/run/subject/project_workspace.py`。
- `Chaos.TestFramework` 已经定义 `ChaosUnitTest` / `ChaosBenchmark` / `Assert`，位于 `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`。
- `Chaos.IL2CPP.DeclarationDiscovery` 已能扫描程序集 metadata，发现 attribute 并产出 declared unit test / benchmark 元数据，位于 `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`。
- registry / compiled catalog / planner / executor 已经有“统一测试入口”的骨架，主要位于：
  - `build/toolchains/run/testing/compiled_catalog.py`
  - `build/toolchains/run/testing/registry.py`
  - `build/toolchains/run/commands/test.py`
  - `build/toolchains/run/testing/subject_planner.py`

### 当前差距

- 现在的 workspace manifest 还不是“四类工程”一等模型；它主要只有 `managedProjects` 和 `matrices[].nativeProjects`。
- 当前测试并不是统一从 native test project 执行；proof、benchmark、engineering validation/workload 仍然是混合执行模型。
- declared metadata 已经能被发现，但发现结果还没有直接驱动“生成 managed/native test host 工程”。
- 旧逻辑仍然残留在多个层次：
  - 手写 proof/native proof target
  - workspace manifest 中的临时字段
  - pipeline 侧和 registry 侧对历史入口的兼容路径
  - tests 中部分直接绑定历史 subject 布局或历史命名

## 方案对比

### 方案 A：把所有测试都物理统一成 native test project

- 优点：
  - 概念最简单，表面上只有一个执行出口。
- 缺点：
  - 会扭曲 managed baseline、engineering validation、interpreter / hot-update 等非 native-executable 场景。
  - 会把“统一入口”误做成“统一物理宿主”，后续反而更乱。

### 方案 B：统一入口和 workspace 模型，执行宿主双轨生成

- 做法：
  - 所有 proof / benchmark 都从同一份 declared catalog 生成 managed test host 和 native test host。
  - 工程验证对象继续保留 pipeline 语义，但统一收口到同一个 workspace manifest 和 test command。
- 优点：
  - 与当前系统真实语义一致。
  - 可以实现“新增 subject 只写 source + attribute”。
  - 旧逻辑有清晰收口路径。
- 缺点：
  - 需要同时重构 workspace、catalog、test entry、dashboard。

### 方案 C：只统一 CLI / 菜单，不重做工程生成模型

- 优点：
  - 改动最小。
- 缺点：
  - 不能解决手工注册、四类工程模型、历史逻辑清理这些核心问题。

## 结论

- 选用方案 B。
- 统一的是：
  - workspace contract
  - catalog contract
  - generated host contract
  - CLI / registry / dashboard 入口
- 不统一成“native-only”执行世界。

## 清理原则

1. 不保留长期 compatibility alias。
2. 不新增新的 string 协议面去描述 proof / benchmark 调度。
3. 历史入口如果必须存在，只允许作为阶段性 shim，并且 roadmap 中必须明确删除时点。
4. 新模型落地后，旧的手写注册、手写 proof host、旧 manifest 字段、旧 selector 兼容层都要进入删除清单。

## 分流判断

本任务必须进入 `roadmap`，原因如下：

- 涉及 workspace、catalog、registry、executor、dashboard、subject 清理六个相对独立但互相依赖的阶段。
- 需要先冻结 contract，再做 child task 逐步迁移，不适合直接写单份执行 plan。
- 任务会跨多个会话推进，并且必须显式管理旧逻辑删除节奏。
