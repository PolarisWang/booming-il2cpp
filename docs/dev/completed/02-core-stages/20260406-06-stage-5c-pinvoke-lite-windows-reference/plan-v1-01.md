# Stage 5C PInvokeLite Windows Reference 实现计划

> 面向执行 Agent：必须使用 `dev:subagent-driven-development`（如果可用子 Agent）或 `dev:executing-plans` 来执行本计划。步骤使用复选框语法跟踪。

**目标：** 在现有 Stage 4 `Windows x64` reference spine、Stage 5A generic foundation 与 Stage 5B reflection foundation 上，用一个最小 `DllImport` import-only `PInvokeLite` proof 打通第一条 interop / native boundary 路径，并继续保持 Stage 4 / Stage 5A / Stage 5B gates 不回退。

**架构：** 先冻结一个最小 `DllImport` proof 输入，再用 TDD 让当前 driver / native reference path 在真实 DLL 上失败，随后补齐 managed import descriptor、generated import glue 与 proof host 所需的最小 P/Invoke 支持，最后再判断是否需要继续扩到 native export、动态加载或基础 marshalling。

**技术栈：** .NET 8、`System.Runtime.InteropServices`、C++17、Python `unittest`

**设计文档：** `docs/dev/in-progress/20260406-06-stage-5c-pinvoke-lite-windows-reference/design-v1-01.md`

**预期知识沉淀：** 按任务决定；若本轮形成稳定长期边界，再补入 `docs/architecture/roadmap-0/pinvoke-lite-proof-v0.md`

---

## 目标文件与职责

- 修改：`docs/dev/in-progress/20260406-06-stage-5c-pinvoke-lite-windows-reference/STATUS.md`
  - 维护阶段、摘要、下一步与风险。
- 修改：`docs/dev/ACTIVE.md`
  - 保持 active 指针反映当前 Stage 5C 执行状态。
- 修改：`docs/dev/INDEX.md`
  - 同步总览中的 Stage 5C 摘要与 active 状态。
- 修改：`docs/dev/in-progress/INDEX.md`
  - 同步进行中索引中的 Stage 5C 摘要与 active 状态。
- 新增或修改：`docs/dev/in-progress/20260406-06-stage-5c-pinvoke-lite-windows-reference/notes/progress-*.md`
  - 记录每轮 TDD、验证结果、剩余风险与下一步。
- 新增：`tests/proof/input/PInvokeLiteDllImportMinimal/`
  - 承载 Stage 5C 的最小 `DllImport` import proof 输入。
- 新增：`tests/unit/run/test_stage5c_pinvoke_lite_windows_reference.py`
  - 通过真实 DLL 驱动 Stage 5C 的 RED/GREEN。
- 按需修改：`src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - 补齐最小 `DllImport` 所需的 method import / module / entry-point metadata 保留。
- 按需修改：`src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 若需要新增 P/Invoke descriptor / manifest model，则在这里收口。
- 按需修改：`src/managed/Chaos.IL2CPP.MetadataWriter/MetadataWriterStage.cs`
  - materialize import-only P/Invoke 所需的 descriptor entry 与 registration contract。
- 按需修改：`src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
  - 承接 import-only P/Invoke 路径的 generated C++ emission。
- 按需修改：`src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.cpp.scriban`
  - 以 Scriban 方式接入最小 import declaration / call glue。
- 按需修改：`src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
  - 暴露 Stage 5C 所需的 proof materialization 入口。
- 按需修改：`src/native/bootstrap/`
  - 仅在现有 direct-call landing 无法承接 import path 时，补最小 bootstrap glue。
- 按需修改：`src/native/runtime-core/`
  - 仅在 import descriptor 或 runtime helper 不可避免时，补最小支持；如 generated import path 已足够，避免扩 runtime。
- 新增或修改：`tests/proof/native-reference/PInvokeLiteDllImportMinimal/`
  - 承接 Stage 5C proof host glue 与运行验证入口。
- 按需修改：`CMakeLists.txt`
  - 在保留 Stage 4 / Stage 5A / Stage 5B targets 的前提下接入 Stage 5C proof host。

## 执行原则

- [ ] 任意生产代码修改前，先写失败测试并看到正确 RED。
- [ ] 第一轮 sample 必须保持 `DllImport` import-only，不把 `UnmanagedCallersOnly`、`Marshal`、`NativeLibrary` 与导出反射带进来。
- [ ] 第一轮优先使用稳定的整数参数与整数返回值，避免动态输出与环境相关字符串。
- [ ] `src/**` 继续 sample-agnostic，不允许出现测试工程名或样例名判断。
- [ ] Stage 4 `HelloWorldObject`、Stage 5A `GenericEcho` 与 Stage 5B `ReflectionLite` 基线必须继续保持可验证。
- [ ] 每完成一个 chunk，都同步更新 `STATUS.md`、`docs/dev/ACTIVE.md`、索引和 `notes/progress-*.md`。

## Chunk 1：冻结 Stage 5C 最小 `DllImport` proof 输入

- [ ] 新增 `tests/proof/input/PInvokeLiteDllImportMinimal/PInvokeLiteDllImportMinimal.csproj`
- [ ] 新增 `tests/proof/input/PInvokeLiteDllImportMinimal/Program.cs`
- [ ] 保持 `Main` 只覆盖：
  - `[DllImport("kernel32.dll", ExactSpelling = true)]`
  - `MulDiv(6, 7, 3)`
  - `Console.WriteLine("pinvoke-ok|14")`
  - `return 0`

## Chunk 2：TDD 写出 Stage 5C RED

- [ ] 新增 `tests/unit/run/test_stage5c_pinvoke_lite_windows_reference.py`
- [ ] 先写失败测试，要求真实执行：
  - `dotnet build tests/proof/input/PInvokeLiteDllImportMinimal/PInvokeLiteDllImportMinimal.csproj -c Release`
  - `dotnet run --project src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -- <dll> <output-root>`
- [ ] 让测试明确断言 Stage 5C 期望的 import descriptor / generated proof 形状，而不是只断言命令返回 `0`。
- [ ] 运行测试并确认当前 failure 来自 Stage 5C 预期的 `DllImport` descriptor 或 generated import glue 缺口。

## Chunk 3：补齐 managed import descriptor 的最小支持

- [ ] 让 `DllImport` 方法在 closure bundle、descriptor contract 与 downstream materialization 中保持一致。
- [ ] 保留模块名、入口点、调用约定与最小签名信息；如第一轮不需要显式扩调用约定，保持默认平台约定即可。
- [ ] 如当前 import path 需要显式的 method import registration，则按最小范围补齐。
- [ ] 保持 Stage 4 / Stage 5A / Stage 5B 已有样例不回退。

## Chunk 4：补齐 generated import glue 与 proof host

- [ ] 在 generated C++ / Scriban 模板中为最小 Windows import path 落下 sample-agnostic 的 declaration / call glue。
- [ ] 优先走静态系统库 import，不为了第一轮 proof 引入 `LoadLibrary` / `GetProcAddress`。
- [ ] 仅在现有 direct-call landing 无法承接时，最小扩 bootstrap / runtime-core。
- [ ] 明确第一轮不实现：
  - `UnmanagedCallersOnly`
  - native export verification
  - `NativeLibrary.TryLoad` / `TryGetExport`
  - `Marshal.StringToCoTaskMemUTF8` / `PtrToStringUTF8`
  - 非 blittable marshalling

## Chunk 5：接入 Stage 5C proof host 并回归验证

- [ ] 新增 `tests/proof/native-reference/PInvokeLiteDllImportMinimal/` proof host 目录与运行脚本。
- [ ] 让 Stage 5C 测试转绿。
- [ ] 复跑 Stage 5B / Stage 5A / Stage 4 回归。
- [ ] 若具备真实 proof run 条件，验证输出稳定包含：
  - `pinvoke-ok`
  - `14`

## 关键验证命令

- [ ] `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
- [ ] `python -m unittest tests.unit.run.test_stage5c_pinvoke_lite_windows_reference`
- [ ] `python -m unittest tests.unit.run.test_stage5b_reflection_lite_windows_reference`
- [ ] `python -m unittest tests.unit.run.test_stage5a_generic_echo_windows_reference`
- [ ] `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- [ ] `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- [ ] `python -m unittest tests.unit.run.test_native_prefix_naming`

## 完成标准

- [ ] Stage 5C 有一个稳定的最小 `DllImport` proof 输入。
- [ ] 当前 managed pipeline 能稳定 materialize import-only P/Invoke 所需 descriptor。
- [ ] Windows reference host 上存在独立的最小 `DllImport` import proof/gate。
- [ ] Stage 4 `HelloWorldObject`、Stage 5A `GenericEcho` 与 Stage 5B `ReflectionLite` 基线未回退。
- [ ] `src/**` 继续保持 sample-agnostic。
- [ ] `STATUS.md`、`ACTIVE.md`、Stage 5 roadmap 与索引始终与真实状态一致。
