# Legacy Cleanup Inventory v1.01

Date: 2026-04-14 16:34:34 +08:00
Status: frozen-for-phase-1

## 1. 说明

本 inventory 记录“已经确认必须被新模型替代并最终删除”的 surface。
字段说明：

- `area`: 遗留面的类别
- `evidence`: 文件级锚点
- `problem`: 为什么它属于 legacy
- `remove_in`: 建议删除阶段

## 2. Workspace / Manifest Legacy

| area | evidence | problem | remove_in |
| --- | --- | --- | --- |
| 旧 native proof target 常量 | `build/toolchains/run/subject/project_workspace.py:42-43` | 仍把 `chaos_subject_generated_native` / `chaos_subject_reference_proof` 当成一等目标，阻碍四类工程模型落地 | Phase 2 / Phase 5 |
| 旧 proof native CMake target | `build/toolchains/run/subject/project_workspace.py:261-290` | 仍生成 `chaos_subject_reference_proof` 与专用 run target，属于旧 proof host 模型 | Phase 2 / Phase 5 |
| matrix 写入旧 manifest 字段 | `build/toolchains/run/subject/project_workspace.py:1438-1455` | `defaultOpenNativeProject`、`defaultBuildNativeProject`、`generatedNativeProjectPath`、`proofNativeProjectPath`、`nativeProjectPath` 都是 v1/v1.5 时代字段 | Phase 2 |
| 根级仍写旧 root contract | `build/toolchains/run/subject/project_workspace.py:1473-1486` | 只有 `managedProjects + matrices`，缺失 `managedTestProjects/nativeProjects/nativeTestProjects` 四类工程 | Phase 2 |
| 构建侧仍消费旧 matrix 字段 | `build/toolchains/run/subject/project_workspace.py:1792-1798` | build 仍默认从 `defaultBuildNativeProject` 和 `nativeProjects` 反推旧 target | Phase 3 |
| core workspace 仍沿用 `nativeTargets` | `build/toolchains/run/subject/project_workspace.py:1620-1624` | shared helper 仍保留旧 target 列表 contract，容易反向污染 subject workspace v2 | Phase 3 / follow-up |
| deploy 仍消费 `nativeTargets` | `build/toolchains/run/subject/deploy_layout.py:73-79` | 说明旧 workspace shape 仍在下游被依赖 | Phase 3 / follow-up |

## 3. Registry / Planner / Runner Legacy

| area | evidence | problem | remove_in |
| --- | --- | --- | --- |
| CLI 仍向 planner 透传 `sourceEntry` | `build/toolchains/run/commands/test.py:886-905` | `run test` 还靠 `sourceEntry/workloadEntry` 参与执行定位，而不是直接走 generated host | Phase 3 |
| planner 仍将 `source.entry` 写回 selection | `build/toolchains/run/testing/subject_planner.py:140-155` | plan fingerprint 与 artifact 路径仍绑定 `sourceEntry/workloadEntry` 文本 | Phase 3 |
| worker 仍导出 `--chaos-source-entry=` | `build/toolchains/run/testing/subject_workers.py:51-53`, `build/toolchains/run/testing/subject_workers.py:169-190` | runtime 调度仍依赖字符串 source entry 参数 | Phase 3 / Phase 5 |
| registry 仍把 declared entry 映射回 `Assembly/Type::Method` 字符串 | `build/toolchains/run/testing/registry.py:553-626` | registry 解析 declared 对象时仍把 string source entry 当默认 matrix 选择键 | Phase 3 |
| benchmark command 仍使用 workloadEntry 字符串 | `build/toolchains/run/commands/benchmark.py:226-233`, `build/toolchains/run/commands/benchmark.py:349-364` | benchmark pipeline 仍通过 workloadEntry 直接喂 planner | Phase 4 |
| dashboard 仍用 workloadEntry 做 case identity 兼容匹配 | `build/toolchains/run/testing/benchmark_dashboard_generator.py:83-167` | dashboard 还保留 workloadEntry 字符串映射链路 | Phase 4 |
| perf dashboard 仍有同类 legacy helper | `build/toolchains/run/testing/perf_dashboard.py:60`, `build/toolchains/run/testing/perf_dashboard.py:90` | 历史 perf 聚合仍依赖 workloadEntry 字段 | Phase 4 |

## 4. Framework-level Shim Surface

| area | evidence | problem | remove_in |
| --- | --- | --- | --- |
| retained subject enum 调度面 | `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs:95-180` | `ChaosSubjectEntryKind` / `ChaosSubjectSlice` 是 retained subject 迁移期桥接层，不应再扩张 | Freeze now, shrink in Phase 5 |
| string source-entry 参数解析 | `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs:972-1008` | `ChaosSourceEntryArguments` 继续放大 string protocol 面 | Phase 5 |
| subject entry 参数解析 | `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs:884-970` | `ChaosSubjectEntryArguments` 只应作为 retained subject bridge，不应继续承担新增功能 | Phase 5 |

## 5. Subject Hand-written Host Legacy

| area | evidence | problem | remove_in |
| --- | --- | --- | --- |
| SolutionCorePack 手写 launcher | `subjects/SolutionCorePack/source/Launcher/Program.cs:8-83` | 同时维护 `ChaosSourceEntryArguments`、`ChaosSubjectEntryArguments`、反射 `InvokeSourceEntry` 与 slice switch，属于典型旧 host | Phase 5 |
| HotUpdateHostPack 手写 proof host | `subjects/HotUpdateHostPack/source/Host/Program.cs:7-63` | 仍手写 proof entry 列表与 `Dictionary<string, Func<int>>` source-entry 映射 | Phase 5 |
| MixedExecutionFeaturePack 手写 proof host | `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Program.cs:7-66` | 同样手写默认 proof 列表、source-entry 字典与 slice switch | Phase 5 |
| SolutionCorePack trace-only program | `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/Program.cs:91-97` | `Program.Main(string[])` 只是 pipeline trace 壳，不应长期作为 subject test dispatch 入口 | Phase 5 |
| benchmark fallback main | `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/Program.cs:3-9` | 用 `Program.Main` 包一层 benchmark entry，属于旧 workload entry 契约痕迹 | Phase 5 |

## 6. Subject Manifest Legacy

| area | evidence | problem | remove_in |
| --- | --- | --- | --- |
| HotUpdate subject 默认 proof 入口 | `subjects/HotUpdateHostPack/subject.manifest.json:12` | 直接绑定 `HotUpdateHostPack/Program::Main()` | Phase 3 / Phase 5 |
| HotUpdate archetype managed-output 入口 | `subjects/HotUpdateHostPack/subject.manifest.json:237` | 仍使用 `Program::Main()` 作为 managed-output contract | Phase 5 |
| MixedExecution 默认 proof 入口 | `subjects/MixedExecutionFeaturePack/subject.manifest.json:12` | 仍绑定 `MixedExecutionProofEntry::Run()` / legacy source entry 路由 | Phase 3 / Phase 5 |
| MixedExecution archetype 入口 | `subjects/MixedExecutionFeaturePack/subject.manifest.json:290` | 仍绑定 `InterpreterArithmeticProof/Program::Main()` | Phase 5 |
| SolutionCorePack 默认 proof / interface proof / trace / benchmark 入口 | `subjects/SolutionCorePack/subject.manifest.json:12`, `subjects/SolutionCorePack/subject.manifest.json:551`, `subjects/SolutionCorePack/subject.manifest.json:589`, `subjects/SolutionCorePack/subject.manifest.json:620`, `subjects/SolutionCorePack/subject.manifest.json:650`, `subjects/SolutionCorePack/subject.manifest.json:684` | 同一 subject 内混合保留 proof entry、trace `Program::Main`、benchmark workload string，说明统一 host 尚未收口 | Phase 3 / Phase 5 |
| SolutionCorePack 各 archetype `Program::Main` 入口 | `subjects/SolutionCorePack/subject.manifest.json:341`, `subjects/SolutionCorePack/subject.manifest.json:376`, `subjects/SolutionCorePack/subject.manifest.json:411`, `subjects/SolutionCorePack/subject.manifest.json:446`, `subjects/SolutionCorePack/subject.manifest.json:481`, `subjects/SolutionCorePack/subject.manifest.json:516` | archetype 仍通过 app entrypoint 作为验证 contract，而不是 declared proof / generated host | Phase 5 |

## 7. Subject Source Validation Legacy

| area | evidence | problem | remove_in |
| --- | --- | --- | --- |
| Simple library app 输出校验 | `subjects/SolutionCorePack/source/Archetypes/SimpleLibrarySolution/App/Program.cs:7-10` | 仍以 `Console.WriteLine + exit code` 充当验证面 | Phase 5 |
| Multi-project app 输出校验 | `subjects/SolutionCorePack/source/Archetypes/MultiProjectSolution/App/Program.cs:7-10` | 同上 | Phase 5 |
| Package-reference app 输出校验 | `subjects/SolutionCorePack/source/Archetypes/PackageReferenceSolution/App/Program.cs:7-11` | 同上 | Phase 5 |
| Reference-assembly app 输出校验 | `subjects/SolutionCorePack/source/Archetypes/ReferenceAssemblySolution/App/Program.cs:7-11` | 同上 | Phase 5 |
| Corelib-reference app 输出校验 | `subjects/SolutionCorePack/source/Archetypes/CoreLibReferenceSolution/App/Program.cs:9-14` | 同上 | Phase 5 |
| Mixed-reference closure app 输出校验 | `subjects/SolutionCorePack/source/Archetypes/MixedReferenceClosureSolution/App/Program.cs:9-16` | 同上 | Phase 5 |

说明：

- 这些 archetype app 是真实 subject source，不一定都要删除
- 但它们不能继续作为 proof/benchmark 的长期验证协议；后续应转为 declared proof、golden assert 或 generated host 驱动

## 8. Tests / Fixtures Legacy

| area | evidence | problem | remove_in |
| --- | --- | --- | --- |
| fixture subject 克隆与 materialize 基础设施 | `tests/support.py:64-118` | 测试基础设施仍直接围绕 `subject.manifest.json + source.entry` 物化 | Phase 5 |
| run bucket shim 文件 | `tests/unit/run/*.py`, `tests/integration/run/*.py` 中所有 `Legacy shim;` 文件 | 这些 shim 仅服务目录过渡，已经不是长期结构 | Phase 5 |
| repo layout 测试显式要求 shim 存在 | `tests/unit/run/test_repo_layout.py:345-365` | 当前测试还把 shim 当预期结构的一部分 | Phase 5 |
| 真实 subject planner 断言旧 `source.entry` | `tests/unit/planning/test_solution_core_pack_planner.py:14-178` | 大量断言真实 subject 的 `Program::Main` / `source.entry` 细节，耦合旧路由协议 | Phase 5 |
| planner 测试直接锚定真实 subject 旧选择面 | `tests/unit/planning/test_subject_planner.py:727-1050` | 同时覆盖 declared case 与真实 subject `source.entry` 细节，后续需要拆成 contract test + retained-subject product test | Phase 5 |
| declared metadata discovery 直接绑定 retained subject 工程路径 | `tests/integration/registry/test_declared_metadata_discovery.py:16-23`, `tests/integration/registry/test_declared_metadata_discovery.py:224-864` | 真实 subject 集成测试有价值，但不应继续依赖手写 host / `Program::Main` 细节 | keep real-subject integration, refactor assertions in Phase 5 |
| benchmark subject source 测试直接断言 manifest `source.entry` / `Program::Main` | `tests/unit/performance/test_benchmark_subject_sources.py:33-448` | 真实 benchmark subject 需要保留，但断言目标应从旧 source-entry 迁到 generated host / declared benchmark contract | Phase 5 |
| registry scan 对 retained subject 默认 entry 元数据有旧耦合 | `tests/integration/registry/test_registry_scan.py:689-734` | 仍把默认 source entry / primary project 视为外显 contract | Phase 5 |

## 9. Inventory 结论

- 必删：
  - 旧 proof native target
  - 旧 manifest 字段
  - 手写 proof host / benchmark host
  - run bucket shim 文件
  - `--chaos-source-entry=` 新生产路径
- 保留但改断言：
  - 真实 subject 集成测试
  - archetype solution 真实 source 项目
- 冻结不扩张：
  - `ChaosSubjectEntryKind`
  - `ChaosSubjectSlice`
  - `ChaosSourceEntryArguments`
