# Tests Taxonomy And Subject Project Pruning Brainstorm v1.01

Date: 2026-04-13 14:28:02 +08:00
Status: draft-approved

## 1. 触发背景

上一轮 retained subject cutover 已经把主线收敛到：

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

但仓库当前还停留在“入口和 manifest 先切过去了，很多源码和测试还没彻底收口”的中间态：

- `subjects/` 顶层仍残留大量 legacy root。
- `SolutionCorePack` 仍直接引用顶层 `MainlineFeaturePack` / `PerformanceFeaturePack`。
- `tests/` 仍有大量直接耦合旧 subject 名称和旧目录结构的断言。
- 仍存在 subject 自带的 xUnit / perf harness 工程。

## 2. 用户确认的原则

- 顶层 `subjects/` 只保留三类 retained subject。
- 一次性删除 compatibility alias，不保留长期兼容壳。
- `tests/` 里的 subject 逻辑要抽成通用机制；只保留必要的 retained subject 烟测。
- subject 源码已经采用 solution 模式后，proof / benchmark 主要通过 `ChaosUnitTest` / `ChaosBenchmark` 声明，不再鼓励 subject 目录里继续堆 `validation/`、`scenario/`、`workload/` 的专有工程。
- `src/managed` 是 IL2CPP 核心代码；测试声明框架位于 `src/reference/Chaos.TestFramework`。
- 尽量少用 string，尽量用紧凑枚举，公开枚举都要带注释。

## 3. 为什么不能直接写单阶段实现计划

这轮工作不是“删几个目录”，而是四类耦合同时变化：

1. subject 物理布局会变化。
2. compiled catalog / registry / planner 的 subject 模型会变化。
3. public smoke / gate / run manifest 的目标路径会变化。
4. tests 需要从“验证某个旧 subject 目录存在”切换到“验证 retained subject 能力仍然成立”。

如果不先冻结迁移顺序，很容易出现：

- 目录删了，但 solution 主 subject 还在引用旧路径；
- registry 已切到 retained subject，但 tests 仍在查旧 object id；
- public suite 仍在用旧 `project_path` 或旧 `subject_id`；
- declared catalog 仍只扫描 primary project，导致 solution 内其他程序集的声明测试消失。

## 4. 当前最关键的结构事实

- `SolutionCorePack` 已存在 `.sln`，但 launcher 仍 `ProjectReference` 顶层 `MainlineFeaturePack` / `PerformanceFeaturePack`。
- `compiled_catalog.py` 当前 declared catalog 只按 `source.primaryProjectPath` 解析主程序集，不会把 solution 内的多程序集全部纳入扫描。
- `public_specs.py` 仍暴露 `HelloWorld`、`GenericEcho`、`ReflectionLite`、`PInvokeLite`、`HostEmbeddingLite` 等 smoke suite。
- `build.py` 的 reference preset 预热仍写死 `HelloWorldObject`。
- `test_registry_scan.py`、`test_declared_metadata_discovery.py`、`test_subject_planner.py` 等仍直接绑定旧 subject id。

## 5. 结论

需要新建一条 follow-up roadmap，先做 inventory 和结构冻结，再按下面顺序推进：

1. 打通 solution-mode retained subject 的多程序集 catalog / planner / registry 模型。
2. 把 `SolutionCorePack` 对顶层 legacy source 的源码依赖吸回 subject 内部。
3. 迁移 public smoke / gate / run manifest 到 retained subject 内部 slice 或 generic fixture。
4. 删除顶层 legacy subject 与 subject-owned 测试工程。
5. 重写 `tests/` 的 subject-heavy 测试，按通用机制和 retained smoke 重新分层。
