# Stage 5B ReflectionLite Windows Reference 实现计划

> 面向执行 Agent：必须使用 `dev:subagent-driven-development`（如果可用子 Agent）或 `dev:executing-plans` 来执行本计划。步骤使用复选框语法跟踪。

**目标：** 在现有 Stage 4 `Windows x64` reference spine 与 Stage 5A generic foundation 上，用一个最小 query-only `ReflectionLite` proof 打通第一条 reflection / metadata 查询路径，并继续保持 Stage 4 / Stage 5A gates 不回退。

**架构：** 先冻结一个最小 reflection query proof 输入，再用 TDD 让当前 driver / native reference path 在真实 DLL 上失败，随后补齐 managed metadata materialization、runtime lookup 与 proof host 所需的最小 reflection glue，最后再判断是否需要继续扩到更高阶 reflection surface。

**技术栈：** .NET 8、`System.Reflection`、C++17、Python `unittest`

**设计文档：** `docs/dev/in-progress/20260406-05-stage-5b-reflection-lite-windows-reference/design-v1-01.md`

**预期知识沉淀：** 按任务决定；若本轮形成稳定长期边界，再补入 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/reflection-lite-proof-v0.md`

---

## 目标文件与职责

- 修改：`docs/dev/in-progress/20260406-05-stage-5b-reflection-lite-windows-reference/STATUS.md`
  - 维护阶段、摘要、下一步与风险。
- 修改：`docs/dev/ACTIVE.md`
  - 保持 active 指针反映当前 Stage 5B 执行状态。
- 修改：`docs/dev/INDEX.md`
  - 同步总览中的 Stage 5B 摘要与 active 状态。
- 修改：`docs/dev/in-progress/INDEX.md`
  - 同步进行中索引中的 Stage 5B 摘要与 active 状态。
- 新增或修改：`docs/dev/in-progress/20260406-05-stage-5b-reflection-lite-windows-reference/notes/progress-*.md`
  - 记录每轮 TDD、验证结果、剩余风险与下一步。
- 新增：`tests/proof/input/ReflectionLiteQueryMinimal/`
  - 承载 Stage 5B 的最小 reflection query proof 输入。
- 新增：`tests/unit/run/test_stage5b_reflection_lite_windows_reference.py`
  - 通过真实 DLL 驱动 Stage 5B 的 RED/GREEN。
- 按需修改：`src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - 补齐最小 reflection query 所需的 metadata / type / member retention。
- 按需修改：`src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 如果 reflection query 需要显式保留类型/成员 metadata，在这里最小补齐。
- 按需修改：`src/managed/Chaos.IL2CPP.MetadataWriter/MetadataWriterStage.cs`
  - materialize query-only reflection 所需的 metadata entry 与 lookup contract。
- 按需修改：`src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 若需要新增 reflection query artifact / manifest model，则在这里收口。
- 按需修改：`src/managed/Chaos.IL2CPP.CodeGen/`
  - 承接 query-only reflection 路径的 generated C++ emission。
- 按需修改：`src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
  - 暴露 Stage 5B 所需的 proof materialization 入口。
- 按需修改：`src/native/runtime-core/`
  - 落下最小 reflection handle / metadata lookup 实现。
- 按需修改：`src/native/bootstrap/`
  - 落下 query-only reflection lookup 所需的 bootstrap glue。
- 按需修改：`src/native/support/`
  - 若最小 reflection query path 需要额外 helper，在这里最小补齐。
- 新增或修改：`tests/proof/native-reference/ReflectionLiteQueryMinimal/`
  - 承接 Stage 5B proof host glue 与运行验证入口。
- 按需修改：`CMakeLists.txt`
  - 在保留 Stage 4 / Stage 5A target 的前提下接入 Stage 5B proof host。

## 执行原则

- [ ] 任意生产代码修改前，先写失败测试并看到正确 RED。
- [ ] 第一轮 sample 必须保持 query-only reflection，不把 `EventInfo`、`MakeGenericMethod`、`MethodInfo.Invoke` 带进来。
- [ ] `src/**` 继续 sample-agnostic，不允许出现测试工程名或样例名判断。
- [ ] Stage 4 `HelloWorldObject` 与 Stage 5A `GenericEcho` 基线必须继续保持可验证。
- [ ] 每完成一个 chunk，都同步更新 `STATUS.md`、`docs/dev/ACTIVE.md`、索引和 `notes/progress-*.md`。

## Chunk 1：冻结 Stage 5B 最小 reflection query proof 输入

- [ ] 新增 `tests/proof/input/ReflectionLiteQueryMinimal/ReflectionLiteQueryMinimal.csproj`
- [ ] 新增 `tests/proof/input/ReflectionLiteQueryMinimal/Program.cs`
- [ ] 保持 `Main` 只覆盖：
  - `typeof(ReflectionFixture<int>)`
  - `GetField`
  - `GetProperty`
  - `GetMethod`
  - `GetParameters`
  - `GetGenericTypeDefinition`
  - 稳定字符串输出
  - `return 0`

## Chunk 2：TDD 写出 Stage 5B RED

- [ ] 新增 `tests/unit/run/test_stage5b_reflection_lite_windows_reference.py`
- [ ] 先写失败测试，要求真实执行：
  - `dotnet build tests/proof/input/ReflectionLiteQueryMinimal/ReflectionLiteQueryMinimal.csproj -c Release`
  - `dotnet run --project src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -- <dll> <output-root>`
- [ ] 让测试明确断言 Stage 5B 期望的 metadata / registration / generated proof 形状，而不是只断言命令返回 `0`。
- [ ] 运行测试并确认当前 failure 来自 Stage 5B 预期的 reflection / metadata query 缺口。

## Chunk 3：补齐 managed metadata query 的最小支持

- [ ] 让 closed generic type 与被查询成员在 closure bundle、metadata registration 与 runtime lookup 中保持一致。
- [ ] 如当前 query path 需要显式保留 `Type` / `Field` / `Property` / `Method` / `Parameter` metadata，则按最小范围补齐 retention/materialization。
- [ ] 支持 `GetGenericTypeDefinition` 回到稳定的 generic type identity。
- [ ] 保持 Stage 4 / Stage 5A 已有样例不回退。

## Chunk 4：补齐 native query-only reflection glue

- [ ] 为 `Type`、`Field`、`Property`、`Method`、`Parameter` 查询补最小 runtime/bootstrap lookup path。
- [ ] 如 generated direct-call path 需要扩到 reflection query helper，保持扩展 sample-agnostic。
- [ ] 明确第一轮不实现：
  - `MethodInfo.Invoke`
  - `FieldInfo.GetValue` / `SetValue`
  - `PropertyInfo.GetValue` / `SetValue`
  - `EventInfo`
  - `MakeGenericMethod`

## Chunk 5：接入 Stage 5B proof host 并回归验证

- [ ] 新增 `tests/proof/native-reference/ReflectionLiteQueryMinimal/` proof host 目录与运行脚本。
- [ ] 让 Stage 5B 测试转绿。
- [ ] 复跑 Stage 4 / Stage 5A 回归。
- [ ] 若具备真实 proof run 条件，验证输出稳定包含：
  - `reflection-ok`
  - 预期的 type / field / property / method / parameter / generic-type-definition 名称

## 关键验证命令

- [ ] `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
- [ ] `python -m unittest tests.unit.run.test_stage5b_reflection_lite_windows_reference`
- [ ] `python -m unittest tests.unit.run.test_stage5a_generic_echo_windows_reference`
- [ ] `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- [ ] `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- [ ] `python -m unittest tests.unit.run.test_native_prefix_naming`

## 完成标准

- [ ] Stage 5B 有一个稳定的最小 reflection query proof 输入。
- [ ] 当前 managed pipeline 能稳定 materialize query-only reflection 所需 metadata。
- [ ] `Type` / `Field` / `Property` / `Method` / `Parameter` / `GetGenericTypeDefinition` 在 Windows reference host 上有独立 proof/gate。
- [ ] Stage 4 `HelloWorldObject` 与 Stage 5A `GenericEcho` 基线未回退。
- [ ] `src/**` 继续保持 sample-agnostic。
- [ ] `STATUS.md`、`ACTIVE.md`、Stage 5 roadmap 与索引始终与真实状态一致。
