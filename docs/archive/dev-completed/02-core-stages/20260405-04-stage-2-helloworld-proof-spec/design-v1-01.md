# Stage 2 HelloWorld Proof Spec Design

Date: 2026-04-05 23:50:13 +08:00
Status: ready-for-plan

## 设计目标

固定 object-based `HelloWorld` 的唯一 proof 规格，使后续 Stage 3 / Stage 4 能基于稳定上游推进，而不再重新争论 sample truth。

## 设计边界

### 纳入

- `tests/proof/input/HelloWorldObject`
- `tests/proof/hello-world-object`
- `contracts/examples/v0/artifacts/*.json`
- `contracts/native/examples/v0/bridge/direct-call-landing.cpp`
- `contracts/docs/v0/*` 中与 first-proof 相关的边界说明
- Stage 2 自身的 canonical spec 文档与 gate 定义

### 不纳入

- managed pipeline 的真实实现
- native runtime / bootstrap 的真实实现
- follow-on sample 的新增或迁移

## 关键设计结论

1. object-based `HelloWorld` 的 canonical truth 以 Stage 1 已冻结的 contract / artifact / native landing 为准，而不是以当前 `Program.cs` 的临时形状为准。
2. canonical proof 结果必须满足：
   - 程序装配名为 `HelloWorldObject`
   - stdout 为 `Hello, World!`
   - 进程 exit code 为 `0`
3. canonical managed subject 集固定为：
   - type: `HelloWorldObject/Program`
   - type: `HelloWorldObject/Greeter`
   - field: `HelloWorldObject/Greeter::_name`
   - method: `HelloWorldObject/Program::Main(System.String[])`
   - method: `HelloWorldObject/Greeter::.ctor(System.String)`
   - method: `HelloWorldObject/Greeter::BuildMessage()`
4. `Program.Main(System.String[])` 保留标准入口签名，但 proof 语义不依赖运行时参数；canonical artifact 当前固定为构造 `Greeter("World")`、打印结果并返回 `0`。
5. generated direct-call landing 是 first-proof 的唯一允许路径；不回退到 `method_invoke`、宿主直连或解释执行替代路径。
6. proof-required helper / metadata 面保持 Stage 1 已冻结的窄集合，不在 Stage 2 重新扩大：
   - `object_new`
   - `string_new_utf8`
   - `resolve_type_by_token`
   - `resolve_method_by_token`
   - `resolve_field_by_token`
   - `register_codegen`
   - `bootstrap_runtime`
   - `System.String::Concat(System.String,System.String)`
   - `System.Console::WriteLine(System.String)`

## Gate 划分

### Stage 2 要冻结的 gate 契约

- `proof-input-build`: `tests/proof/input/HelloWorldObject/HelloWorldObject.csproj` 可稳定 build
- `proof-artifact-snapshots`: canonical artifact snapshot 能稳定表达 3 个方法、1 个字段与 shared `subjectId`
- `proof-spec-consistency`: source / artifact / native landing / 文档不再各说各话

### 明确延后到 Stage 3 / Stage 4 的 gate

- `managed-minimal-closure`: managed pipeline 能从程序集走到 generated-code input
- `native-reference-proof`: reference platform 真正输出 `Hello, World!` 且 exit code 为 `0`

## 交付落点

- 任务文档：
  - `brainstorm-v1-01.md`
  - `design-v1-01.md`
  - `plan-v1-01.md`
- 长期规格文档：
  - `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/hello-world-proof-spec-v0.md`
- 对齐对象：
  - `tests/proof/input/HelloWorldObject/Program.cs`
  - `tests/proof/hello-world-object/suite.manifest.json`
  - 如有必要，补与 proof spec 对齐的测试或文档摘要

## 设计结果

Stage 2 不再把“哪一份 `HelloWorld` 才是 canonical truth”留给后续实现阶段处理，而是在本阶段直接选定 contract / artifact / native landing 这一侧为唯一真源，并据此生成后续执行 plan。
