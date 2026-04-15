# Phase 1 Subject Source Reshape And Test Metadata Cutover 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。代码改动前先补失败测试，再完成最小实现。
> **目标：** 把 retained subject 的 source / launcher / metadata / orchestration 收口到 Phase 0 冻结的稳定契约上，停止继续扩大字符串调度面，并为 Phase 2 的 typed-il / hybrid dispatch 共用契约清出边界。
> **架构：** 本阶段优先修改三类位置：

- subject launcher / host
- declaration discovery / metadata schema
- orchestration / registry / reporting 中仍参与真实路由的 string surface

优先原则：
1. 不再继续扩大 `ChaosSourceEntry` 字符串协议。
2. 优先使用 `ChaosUnitTest` / `ChaosBenchmark` 已声明的 compact metadata 做发现和路由。
3. 兼容路径可以短暂保留，但必须持续收缩，而不是继续扩展。

**技术栈：** .NET 8、`Chaos.TestFramework`、declaration discovery、现有 unit / integration tests

**设计文档：**

- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`

**预期知识沉淀：** 先留在 `docs/dev`；如后续 Phase 2-4 继续沿用本批 compact identity / display contract 边界，再统一沉淀到 architecture 文档。

---

## 预期改动面

- `subjects/SolutionCorePack/source/Launcher/Program.cs`
- `subjects/HotUpdateHostPack/source/Host/Program.cs`
- `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Program.cs`
- `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`
- `build/toolchains/run/commands/test.py`
- `build/toolchains/run/commands/benchmark.py`
- `build/toolchains/run/testing/subject_planner.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
- `build/toolchains/run/testing/subject_workers.py`
- `src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Program.cs`
- 对应 tests

## 执行步骤

- [x] Task 1: 盘点现有 tests，补齐 launcher / discovery 相关 RED 测试。
- [x] Task 2: 收口 `SolutionCorePack` launcher 中的 `ChaosSourceEntryArguments` 路由。
- [x] Task 3: 收口 `HotUpdateHostPack` / `MixedExecutionFeaturePack` 中的 source-entry 字符串字典。
- [x] Task 4: 对齐 declaration discovery 的 metadata schema 读取，避免继续扩大自由字符串协议。
- [x] Task 5: 跑通受影响 tests，并记录仍未关闭的兼容路径与后续 batch。
- [x] Task 6: 继续盘点 retained subject / registry / planner 剩余 string surface，完成 command / planner / registry / dashboard 第二批收口。
- [x] Task 7: 继续收口 registry / reporting 中仅剩的 display-only string contract，明确哪些字段只保留展示语义，哪些字段必须退出路由面。
- [x] Task 8: 继续盘点真正仍参与路由的字符串面，重点审查 declared matrix 选择、summary 匹配和 perf dashboard / harness 汇总路径；把可由 compact identity 替代的 fallback 改成 `entrySelection` / `entryIndex` + structured metadata。

## Task 8 完成结果

- `subject_workers.py` 新增 runtime benchmark declared catalog 解析，保留 `entryIndex`，并把 `declaredBenchmark` 结构写入 runtime manifest。
- perf harness 优先消费结构化参数：
  - `--assembly-name`
  - `--declaring-type`
  - `--method-name`
  - `--method-signature`
- `registry.py` 的 declared benchmark default matrix 选择不再依赖 `workloadEntry/sourceEntry` 字符串匹配，而是优先根据 declared `modes` 与 stage kind 对齐。
- `Benchmark.WorkloadEntry.PerfHarness` 新增结构化 benchmark metadata 解析路径，仅在无法解析时回落到 `--workload-entry`。

## 验收

- 至少有一组 launcher 行为测试走完 RED -> GREEN。
- 至少有一组 declaration discovery schema 测试走完 RED -> GREEN。
- orchestration / planner / dashboard / runtime perf harness 新增的 compact identity 路径必须有对应 RED -> GREEN 测试。
- retained subject 的默认 proof / benchmark 入口不能回归。
- 如仍保留兼容字段，必须在 `STATUS.md` 或 progress note 中明确记录，不得静默保留。
- 退出条件：
  - subject/test metadata 的真实路由面不再依赖新扩张的字符串协议；
  - display-only 字段与 routing identity 明确分离；
  - runtime benchmark harness 已具备结构化 declared metadata 执行能力。

## 验证命令

- `python -m pytest tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/performance/test_subject_workers_perf.py -k "declared_registry_matrix_selection or declared_benchmark_metadata or preserves_entry_index"`
- `dotnet build src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj -c Release -m:1`
- `python -m pytest tests/unit/execution/test_subject_workers.py tests/unit/performance/test_subject_workers_perf.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/integration/registry/test_registry_scan.py -k "declared_benchmark_by_entry_index_for_structured_harness_execution or runtime_perf or interpreter_runtime_perf or declared_registry_matrix_selection or projects_engineering_and_declared_catalog_object_families"`
- `python -m pytest tests/tooling/run/test_subject_command.py tests/unit/performance/test_benchmark_command.py tests/unit/planning/test_subject_planner.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/unit/execution/test_subject_workers.py tests/unit/performance/test_subject_workers_perf.py tests/unit/registry/test_declared_registry_matrix_selection.py`
