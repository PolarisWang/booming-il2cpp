# Stage 5A GenericEcho Windows Reference 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 在现有 Stage 4 `Windows x64` reference spine 上，先用最小闭合泛型 proof 打通 Stage 5A 的第一条 generic foundation，优先关闭 `MethodSpecification` / `TypeSpecification` 的 managed pipeline 缺口，并在此基础上继续评估 native reference proof 接线。

**架构：** 先冻结一个最小闭合泛型 proof 输入，再用 TDD 让当前 driver 在真实 DLL 上失败，随后补齐 loader / closure bundle 所需的最小泛型引用解析与 registration 一致性，最后再判断当前 Stage 4 generated direct-call path 是否足以继续承接 Stage 5A。

**技术栈：** .NET 8、`System.Reflection.Metadata`、Python `unittest`

**设计文档：** `docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference/design-v1-01.md`

---

## 目标文件与职责

- 修改：`docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference/STATUS.md`
  - 维护阶段、摘要、下一步与风险。
- 修改：`docs/dev/ACTIVE.md`
  - 保持 active 指针在执行期间指向 Stage 5A，归档后回切父 roadmap。
- 修改：`docs/dev/INDEX.md`
  - 同步总览中的 Stage 5A 摘要与 active 状态。
- 修改：`docs/dev/in-progress/INDEX.md`
  - 同步进行中索引中的 Stage 5A 摘要与 active 状态。
- 修改：`docs/dev/completed/INDEX.md`
  - 在完成索引中登记 Stage 5A 归档结果。
- 新增或修改：`docs/dev/completed/20260406-04-stage-5a-generic-echo-windows-reference/notes/progress-*.md`
  - 记录每轮 TDD、验证结果、剩余风险与下一步。
- 新增：`tests/proof/input/GenericEchoClosedMinimal/`
  - 承载 Stage 5A 的最小闭合泛型 proof 输入。
- 新增：`tests/unit/run/test_stage5a_generic_echo_windows_reference.py`
  - 通过真实 DLL 驱动 Stage 5A 的 RED/GREEN。
- 修改：`src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - 补齐 `MethodSpecification` / `TypeSpecification` 与闭合泛型引用解析。
- 按需修改：`src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 若需要新增/统一 generic naming helper，则在这里收口。
- 按需修改：`src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 若闭合泛型 registration 需要额外一致性处理，则在此最小补齐。
- 按需修改：`src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
  - 若 closure bundle 需要 materialize 闭合泛型 method/type，保持与 typed-il / registration 一致。

## 执行原则

- [x] 任意生产代码修改前，先写失败测试并看到正确 RED。
- [x] 第一轮 sample 必须保持最小闭合泛型，不把 tuple / 插值字符串带进来。
- [x] `src/**` 继续 sample-agnostic，不允许出现测试工程名或样例名判断。
- [x] Stage 4 `HelloWorldObject` 基线必须继续保持可验证。
- [x] 每完成一个 chunk，都同步更新 `STATUS.md`、`docs/dev/ACTIVE.md`、索引和 `notes/progress-*.md`。

## Chunk 1：冻结 Stage 5A 最小闭合泛型 proof 输入

- [x] 新增 `tests/proof/input/GenericEchoClosedMinimal/GenericEchoClosedMinimal.csproj`
- [x] 新增 `tests/proof/input/GenericEchoClosedMinimal/Program.cs`
- [x] 保持 `Main` 只覆盖：
  - `GenericEcho.Echo<string>(...)`
  - `new EchoBox<string>(...)`
  - `Console.WriteLine(...Value)`
  - `return 0`

## Chunk 2：TDD 写出 Stage 5A RED

- [x] 新增 `tests/unit/run/test_stage5a_generic_echo_windows_reference.py`
- [x] 先写一个失败测试，要求真实执行：
  - `dotnet build tests/proof/input/GenericEchoClosedMinimal/GenericEchoClosedMinimal.csproj -c Release`
  - `dotnet run --project src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -- <dll> <output-root>`
- [x] 让测试明确断言 Stage 5A 期望的 closure bundle 形状，而不是只断言命令返回 0。
- [x] 运行测试并确认当前 failure 来自 Stage 5A 预期的泛型引用缺口。

## Chunk 3：补齐 managed pipeline 的最小泛型支持

- [x] 在 loader 中接入 `MethodSpecification`
- [x] 在 loader 中接入 `TypeSpecification`
- [x] 让闭合泛型 type / method 在 subjectId、signature、instruction callee 上保持一致
- [x] 如果当前 closure bundle 仍存在“指令引用了闭合泛型，但 registration 没有对应项”的半闭环，按最小范围补齐 materialization
- [x] 保持 Stage 3 / Stage 4 已有样例不回退

## Chunk 4：验证 managed closure 并评估 native reference proof 接线

- [x] 让 Stage 5A 测试转绿
- [x] 复跑相关 Stage 3 / Stage 4 回归
- [x] 检查当前 closure bundle 是否已经足以继续喂给现有 Stage 4 native reference path
- [x] 如果现有 generated direct-call path 还不足以承接 Stage 5A，明确记录剩余 blocker，不允许用样例特判“硬接通”

## 关键验证命令

- [x] `python -m unittest tests.unit.run.test_stage5a_generic_echo_windows_reference`
- [x] `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure`
- [x] `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- [x] `python -m unittest tests.unit.run.test_native_prefix_naming`

## 完成标准

- [x] Stage 5A 有一个稳定的最小闭合泛型 proof 输入
- [x] 当前 managed closure pipeline 能稳定消费这个真实 DLL
- [x] 闭合泛型 subject / registration 在 bundle 内保持一致
- [x] Stage 4 `HelloWorldObject` 基线未回退
- [x] `STATUS.md`、`ACTIVE.md`、Stage 5 roadmap 与索引始终与真实状态一致
