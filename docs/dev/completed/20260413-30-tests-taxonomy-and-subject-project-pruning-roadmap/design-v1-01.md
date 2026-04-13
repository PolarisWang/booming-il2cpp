# Tests Taxonomy And Subject Project Pruning Design v1.01

Date: 2026-04-13 14:28:02 +08:00

## 1. 目标

本设计服务于两个直接结果：

1. `subjects/` 顶层最终只保留三个 retained subject：
   - `SolutionCorePack`
   - `HotUpdateHostPack`
   - `MixedExecutionFeaturePack`
2. `tests/` 最终按“通用机制测试 + retained subject 烟测”分层，不再把 legacy subject 目录本身当成长期结构契约。

## 2. 核心设计原则

### 2.1 顶层 subject 只承载 executable plan

顶层 subject 是否存在，依据不是“有没有一个测试点”，而是“是否存在独立 executable plan”。

- `SolutionCorePack`：承载 solution-mode generated-native / managed output / host-facing / archetype / benchmark 主线。
- `HotUpdateHostPack`：承载 host + patch + rollback + replacement。
- `MixedExecutionFeaturePack`：承载 mixed runtime / interpreter / AOT bridge。

不再允许因为新增 proof / benchmark / smoke 就新开顶层 subject。

### 2.2 subject 内部按 slice 组织源码，不再按目录数量表达测试颗粒度

`SolutionCorePack` 内部允许保留多个 C# project，但这些 project 是同一个 retained subject 的内部 slice，而不是新的顶层 subject。

推荐内部结构：

```text
subjects/SolutionCorePack/
  subject.manifest.json
  source/
    SolutionCorePack.sln
    Launcher/
    Mainline/
    Benchmarks/
    Smoke/
    Interop/
    Host/
    Engine/
    Bridge/
    Mobile/
    Archetypes/
```

说明：

- `Mainline/` 承载原 `MainlineFeaturePack` 及其 proof family。
- `Benchmarks/` 承载原 `PerformanceFeaturePack` 及必要 benchmark slice。
- `Smoke/` / `Interop/` / `Host/` / `Engine/` / `Bridge/` / `Mobile/` 只保留真正仍然需要独立 project 的源码样例。
- 旧 subject 如果只是“某个 entry 的壳”，迁移后不再保留独立 project。

### 2.3 subject-owned xUnit / perf harness 工程全部删除

以下内容不再允许作为长期 subject 内部结构：

- `validation/unit/*.Subject.UnitTests.csproj`
- `validation/perf/*.Subject.PerfHarness.csproj`
- `tests/unit/*.Subject.UnitTests.csproj`

替代方式：

- proof 使用 `ChaosUnitTest` 属性声明。
- benchmark 使用 `ChaosBenchmark` 属性声明。
- 系统侧验证逻辑进入 `tests/` 的 generic mechanism 测试，或进入 system/module fixture。
- perf harness 使用共享的 `src/validation/perf/Benchmark.WorkloadEntry.PerfHarness`，不再 subject 私有化。

### 2.4 solution-mode retained subject 的 declared catalog 必须支持多程序集

这是本轮重构的关键基础能力。

当前问题：

- `compiled_catalog.py` 只基于 `source.primaryProjectPath` 解析主程序集。
- 对 `SolutionCorePack` 这类 `.sln + 多项目` subject，主 launcher 程序集本身不包含所有 `ChaosUnitTest` / `ChaosBenchmark` 声明。

设计要求：

- 当 `source.path` 指向 `.sln` 时，declared discovery 必须能覆盖 solution 中需要暴露的程序集。
- 默认实现优先从 primary project 的 output 目录收集 solution 相关程序集。
- `subjects.py` 中已有 `resolve_source_solution_assembly_names()`，应作为 solution-mode assembly 收集的基础。
- registry / planner / compiled catalog 生成的 declared object id 与 engineering object id 应统一挂在 retained subject 下，而不是继续挂在已删除的 legacy subject id 下。

### 2.5 public smoke / gate 命令改成 retained slice 或 generic fixture

public suites 继续存在，但不能再要求顶层 legacy subject 存在。

改造方向：

- smoke suites：
  - 优先指向 `SolutionCorePack` 内部 slice 的 project path；
  - 或改成 `tests/fixtures` 提供的 generic smoke fixture。
- reference desktop gate / trace compare：
  - 改为使用 `SolutionCorePack` 的 retained matrix + entry slice；
  - 不再引用 `HostEmbeddingLite` 顶层 subject id。
- TUI、run manifest、help examples 的示例 subject id 同步切到 retained subject。

## 3. tests taxonomy 设计

### 3.1 通用机制测试

这类测试不应依赖真实 retained subject：

- `subjects.py` manifest 规范化
- planner / registry / selector / path resolver
- compiled catalog / declaration discovery
- project graph / solution ingestion
- reporting / command manifest / run manifest 解析

这类测试应优先使用：

- `tests/fixtures/subjects/*`
- 临时目录中构造的 fixture manifest / profile / assembly

### 3.2 retained subject 烟测

这类测试才允许绑定真实 subject，但目标是确认 retained contract，而不是确认历史目录仍存在：

- `SolutionCorePack` 是否暴露核心 declared unit / benchmark
- `HotUpdateHostPack` 是否暴露 patch / replacement / rollback
- `MixedExecutionFeaturePack` 是否暴露 mixed proof / benchmark / AOT bridge
- retained subject proof / benchmark 命令是否能真实跑通

### 3.3 兼容性旧测试的处理

现有很多 `tests/unit/compatibility/*.py` 实际是在锁定历史 subject 布局。

处理原则：

- 若测试验证的是长期能力边界，迁移为 retained subject smoke。
- 若测试验证的是历史目录名、旧 project 路径、旧 manifest 字段，则删除或改造成 fixture 级 generic mechanism 测试。

## 4. 需要的结构调整

### 4.1 `subjects/`

- 新增或整理 `subjects/SolutionCorePack/source/` 内部 slice 目录。
- 删除顶层 legacy subject 目录。
- 删除 subject-owned xUnit / perf harness 工程。

### 4.2 `build/toolchains/run/testing/`

- `compiled_catalog.py`
  - 支持 solution-mode multi-assembly discovery。
- `registry.py`
  - object id / default metadata 切到 retained subject。
- `subject_planner.py`
  - retained subject 的 entry / workload / entrySelection 路径稳定化。
- `public_specs.py`
  - public smoke 改指 retained slice 或 fixture。

### 4.3 `build/toolchains/run/commands/` 与 manifest

- `build.py` 的 reference preset 预热 subject 切换到 retained subject。
- `run_manifest.json` 和源 manifest 中涉及 legacy subject 的示例、command target、project path、subject id 全部更新。
- TUI 示例文本同步更新。

### 4.4 `tests/`

- 新增/扩展 `tests/fixtures/subjects/` 作为通用 fixture 主来源。
- 把 subject-heavy 的 planner / registry / manifest / command 测试改写为 retained subject 或 generic fixture。
- 删除仅验证旧目录存在的 compatibility 测试。

## 5. 验收标准

- `subjects/` 顶层只剩 `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack`。
- 仓库内不再存在 `*.Subject.UnitTests.csproj` 或 `*.Subject.PerfHarness.csproj`。
- `SolutionCorePack` 不再 `ProjectReference` 顶层 legacy subject 目录。
- declared catalog 能从 solution-mode retained subject 暴露多程序集 proof / benchmark 条目。
- public smoke / gate / run manifest / TUI 不再引用已删除的 legacy subject。
- `tests/` 不再把 legacy subject 顶层目录当作长期结构契约。
- retained solution-mode subjects 的 proof / benchmark 流程完成最新一轮真实验证。
