# Stage 4 Native Bootstrap And Reference Proof 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 消费 Stage 3 的 managed closure bundle，在 `Windows x64` reference host 上逐步打通 first generated C++ emission、native bootstrap 与 first proof，最终真实输出 `Hello, World!` 并返回 `0`。

**架构：** 先把 Stage 4 的长期 spec 和 proof artifacts 收口成稳定入口，再用 TDD 先落 first generated C++ emission，随后补 `runtime-core`、`bootstrap`、`support` 与 proof host 装配，最后把 `windows-x64-reference` 预设、验证入口和任务状态同步收口。Stage 4 必须始终以 Stage 3 bundle 为上游，不反向改写 managed closure contract。

**技术栈：** .NET 8、`System.Text.Json`、C++17、CMake Presets、Python `unittest`

**设计文档：** `docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof/design-v1-01.md`

**预期知识沉淀：**

- `docs/architecture/roadmap-0/native-reference-proof-v0.md`

---

## 目标文件与职责

- 修改：`docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof/STATUS.md`
  - 维护阶段、最近摘要、下一步、风险与阻塞。
- 修改：`docs/dev/ACTIVE.md`
  - 始终让 active 指针反映 Stage 4 的真实状态。
- 修改：`docs/dev/INDEX.md`
  - 同步总览中的 Stage 4 摘要与 active 状态。
- 修改：`docs/dev/in-progress/INDEX.md`
  - 同步进行中索引中的 Stage 4 摘要与 active 状态。
- 新增或修改：`docs/dev/completed/20260406-02-stage-4-native-bootstrap-and-reference-proof/notes/progress-*.md`
  - 记录每个 Chunk 的验证结果、剩余风险与后续步骤。
- 修改：`docs/architecture/roadmap-0/native-reference-proof-v0.md`
  - 写清 Stage 4 输入、输出、proof host 约束、generated emission 边界与 reference preset 接线原则。
- 修改：`src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 维护 Stage 4 native reference proof 所需的 request / manifest / artifact model。
- 修改：`src/managed/Chaos.IL2CPP.CodeGen/`
  - 用 Scriban 从 Stage 3 bundle materialize first generated C++ emission。
- 修改：`src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
  - 提供 Stage 4 emission orchestration 入口，同时保持 Stage 3 CLI 兼容。
- 修改：`tests/unit/run/test_stage4_native_reference_proof.py`
  - 通过 TDD 锁定 Stage 4 emission skeleton、proof manifest 与 direct-call landing 约束。
- 修改：`src/native/runtime-core/`
  - 落下 first proof 所需的最小 runtime ABI 实现。
- 修改：`src/native/bootstrap/`
  - 落下 registration / bootstrap / generated landing 与 proof host 装配路径。
- 修改：`src/native/support/`
  - 落下 narrow string / console glue 等 first proof helper。
- 修改：`tests/proof/native-reference/HelloWorldObject/`
  - 承接 Stage 4 CMake target、proof host glue 与运行验证入口。
- 修改：`CMakeLists.txt`
  - 在保留 reference contract anchor 的前提下，引入 Stage 4 proof target。
- 修改：`build/scripts/verify-roadmap-0.py` 与 `build/scripts/verify-roadmap-0.ps1`
  - 在 Windows reference gate 中接入 Stage 4 prepare / proof 验证。

## 执行原则

- [ ] Stage 4 必须消费 Stage 3 的 managed closure bundle，而不是重开 managed pipeline 语义。
- [ ] 任何生产代码改动前，先写失败测试并看到正确的 RED。
- [ ] 第一轮只锁 `Windows x64`，不并行展开 `macOS` / `Linux` proof host。
- [ ] 保留 `windows-x64-reference` 当前 compile-only contract smoke 锚点，不直接覆盖旧 target。
- [ ] `src/**` 保持 sample-agnostic；测试工程名与样例特化只允许留在 `tests/proof/**`、`artifacts/proof/**` 与文档中。
- [ ] 每完成一个 Chunk，都同步更新 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md` 与索引。

## Chunk 1：收口 Stage 4 长期 spec 与 emission contract

- [x] 创建 `docs/architecture/roadmap-0/native-reference-proof-v0.md`。
- [x] 写明 Stage 4 输入固定为 `artifacts/proof/managed-closure/HelloWorldObject/` 下的五件 bundle。
- [x] 写明 Stage 4 第一轮输出至少包含：
  - `artifacts/proof/native-reference/HelloWorldObject/generated/`
  - `artifacts/proof/native-reference/HelloWorldObject/native-proof.manifest.json`
  - `artifacts/proof/native-reference/HelloWorldObject/run/`
- [x] 写明 generated emission skeleton 与最终 native proof 的边界，避免一开始就把 runtime / bootstrap / support 做满。

## Chunk 2：TDD 打通 first generated C++ emission skeleton

- [x] 新增 `tests/unit/run/test_stage4_native_reference_proof.py`。
- [x] 先写失败测试，锁以下行为：
  - `dotnet run --project src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -- emit-native-reference <managed-closure-root> <output-root>` 能生成 Stage 4 proof skeleton。
  - 输出包含 `generated/native-reference.generated.cpp`。
  - 输出包含 `native-proof.manifest.json`。
  - generated C++ 明确使用 `register_codegen` 与 `bootstrap_runtime`，不出现 `method_invoke` fallback。
  - generated path 只从 Stage 3 bundle 推导 entry / ctor / instance method / field，不在 `src/**` 写死测试工程名。
- [x] 运行失败测试，确认因为功能缺失而 RED。
- [x] 在 `Chaos.IL2CPP.Contracts`、`Chaos.IL2CPP.CodeGen`、`Chaos.IL2CPP.Driver` 中写最小代码让测试转绿。
- [x] 运行 Stage 4 单测与相关 Stage 3 回归，确认没有破坏现有 closure bundle 入口。

## Chunk 3：TDD 落下 `runtime-core` 最小 ABI 实现

- [x] 先写文件级测试，锁 `runtime_init`、`thread_attach`、`object_new`、`string_new_utf8`、`class_init`、`field_get_value`、`field_set_value` 的最小实现形状。
- [x] 新增 `src/native/runtime-core/` 并只实现 first proof 所需最小集合。
- [x] 为 generated direct-call path 补上最小 inline field storage，承接对象字段读写。
- [ ] 等工具链恢复后补做真实 native compile / link 验证。

## Chunk 4：TDD 落下 `bootstrap`、`support` 与 proof host 骨架

- [x] 先写失败测试，锁 `register_codegen` / `bootstrap_runtime` 的最小 happy path，以及 `Console.WriteLine(string)` / narrow concat 的 proof helper glue。
- [x] 新增 `src/native/bootstrap/` 与 `src/native/support/`。
- [x] 新增 `tests/proof/native-reference/HelloWorldObject/` proof host glue。
- [x] 让 generated emission skeleton 与 proof host 在文件级装配上闭环。
- [x] 把 `bootstrap` 中的 token 解析改为通用 `token -> opaque handle`，不再写死样例 token switch。

## Chunk 5：接入 `windows-x64-reference` 预设与 first proof 验证

- [x] 保留现有 contract smoke target。
- [x] 在 `windows-x64-reference` 预设下新增 Stage 4 proof target。
- [x] 为 `build/scripts/verify-roadmap-0.py` 与 `.ps1` 补上 Stage 4 codegen prepare helper，确保 preset build 前先 materialize managed closure 与 native reference artifacts。
- [ ] 扩展 Windows reference gate：
  - 先构建 proof target。
  - 再运行 proof executable。
  - 校验 stdout 为 `Hello, World!`。
  - 校验 exit code 为 `0`。
- [ ] 明确 proof build / run artifact 的输出目录与收集方式。
- [ ] 在 generated C++ 未经真实链接验证前，proof target 保持 `EXCLUDE_FROM_ALL`。

## Chunk 6：统一测试对象、任务状态与知识沉淀

- [ ] 如 Stage 4 proof 已形成稳定验证入口，再评估是否接入 `tests/registry/**` 与 `wiki/06-测试验证/**`。
- [x] 更新 `STATUS.md`、`docs/dev/ACTIVE.md`、索引与 `notes/progress-*.md`。
- [x] 将生成文件名收口到 `generated/native-reference.generated.cpp`，并把 sample-agnostic 边界补入长期 spec。
- [ ] 若产生长期有效知识，再补入必要 wiki。
- [ ] 若本任务满足完成条件，再准备走 `verification-before-completion` 与归档流程。

## 关键验证命令

- [ ] `dotnet build tests/proof/input/HelloWorldObject/HelloWorldObject.csproj -c Release`
- [ ] `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
- [x] `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- [x] `python -m unittest tests.unit.run.test_stage4_runtime_core_skeleton`
- [x] `python -m unittest tests.unit.run.test_stage4_bootstrap_support_skeleton`
- [x] `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- [x] `python -m unittest tests.unit.run.test_native_prefix_naming`
- [ ] `cmake --preset windows-x64-reference`
- [ ] `cmake --build <windows-reference-binary-dir>`

## 完成标准

- [x] Stage 4 有稳定的长期 spec 文档与 proof artifact 入口。
- [x] Stage 3 bundle 可以被 materialize 成 first generated C++ emission。
- [x] `runtime-core -> bootstrap -> support` 形成 first proof 所需的最小 native 闭环。
- [ ] `HelloWorld` 在 `Windows x64` reference host 上真实输出 `Hello, World!` 且 exit code 为 `0`。
- [x] generated path 不依赖 `method_invoke` fallback 或宿主 per-method rescue path。
- [ ] `STATUS.md`、`ACTIVE.md`、父 roadmap 与索引持续与真实状态同步到任务完成。
