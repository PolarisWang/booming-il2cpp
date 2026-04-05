# HelloWorld Proof Spec v0

## 目标

本文件定义 reboot 主线中第一条 object-based `HelloWorld` proof 的唯一 canonical truth。

Stage 2 的职责不是打通 managed closure 或 native runtime，而是把 proof sample source、artifact snapshot、native direct-call landing 和 gate 归属收口到同一份规格上。

## Canonical Truth

### 程序标识

- assembly: `HelloWorldObject`
- entry method: `HelloWorldObject/Program::Main(System.String[])`

### Canonical Output

- stdout: `Hello, World!`
- exit code: `0`

### Managed Subjects

- type: `HelloWorldObject/Program`
- type: `HelloWorldObject/Greeter`
- field: `HelloWorldObject/Greeter::_name`
- method: `HelloWorldObject/Program::Main(System.String[])`
- method: `HelloWorldObject/Greeter::.ctor(System.String)`
- method: `HelloWorldObject/Greeter::BuildMessage()`

## Sample Shape

`HelloWorldObject` 的 canonical shape 固定为：

1. `Program.Main(System.String[])` 构造 `Greeter("World")`
2. `Greeter.BuildMessage()` 产出 `Hello, World!`
3. `Program.Main(System.String[])` 打印结果并 `return 0`

这里保留标准入口签名 `Main(string[] args)`，但 proof 语义不依赖运行时参数，也不再允许默认回退到 `"object-world"` 一类的可变输入。

## Canonical Artifact Truth Sources

Stage 2 采用以下文件作为 proof spec 的上游冻结真源：

- `contracts/examples/v0/artifacts/typed-il-ir.min.json`
- `contracts/examples/v0/artifacts/aot-manifest.min.json`
- `contracts/examples/v0/artifacts/code-registration.min.json`
- `contracts/examples/v0/artifacts/metadata-registration.min.json`
- `contracts/native/examples/v0/bridge/direct-call-landing.cpp`

其中：

- `contracts/examples/v0/artifacts/typed-il-ir.min.json` 固定了 3 个方法体的最小 IL 形状
- `contracts/examples/v0/artifacts/aot-manifest.min.json` 固定了 first-proof entry 与 retained world 最小集合
- `contracts/examples/v0/artifacts/code-registration.min.json` 固定了 generated method pointer 与 symbol
- `contracts/examples/v0/artifacts/metadata-registration.min.json` 固定了 assembly / type / field / method 注册集合
- `contracts/native/examples/v0/bridge/direct-call-landing.cpp` 固定了 generated direct-call landing，而不是 `method_invoke` fallback

## Shared Identity

proof 相关 artifact 必须共享同一套 canonical `subjectId` 规则。

关键 `subjectId` 为：

- `HelloWorldObject/Program::Main(System.String[])`
- `HelloWorldObject/Greeter::.ctor(System.String)`
- `HelloWorldObject/Greeter::BuildMessage()`
- `HelloWorldObject/Greeter::_name`

`typed-il-ir` 可以继续保留 artifact-local `methodId`，但跨 artifact 追溯以 `subjectId` 为唯一正式 identity。

## Direct-Call Landing Constraint

first-proof 的 managed 调用落点明确固定为 generated direct-call landing。

允许的最小路径：

1. `register_codegen`
2. `bootstrap_runtime`
3. `resolve_type_by_token`
4. `resolve_method_by_token`
5. `resolve_field_by_token`
6. `string_new_utf8`
7. `object_new`
8. direct call：
   - `HelloWorldObject_Greeter__ctor`
   - `HelloWorldObject_Greeter_BuildMessage`
   - `HelloWorldObject_Program_Main`

不允许的替代路径：

- `method_invoke` fallback
- 宿主手写直连替代 generated entry
- 解释执行替代 generated call landing

## Gate 归属

### Stage 2 冻结的 gate

- `proof-input-build`
  - `tests/proof/input/HelloWorldObject/HelloWorldObject.csproj` 可以稳定 build
- `proof-artifact-snapshots`
  - canonical artifact snapshot 稳定表达 3 个方法、1 个字段与 shared `subjectId`
- `proof-spec-consistency`
  - source / artifact / native landing / 文档不再各说各话

### 延后到 Stage 3 / Stage 4 的 gate

- `managed-minimal-closure`
  - managed pipeline 能从程序集走到 generated-code input
- `native-reference-proof`
  - reference platform 真正输出 `Hello, World!` 且 exit code 为 `0`

## Repository Entry Points

- sample source: `tests/proof/input/HelloWorldObject/Program.cs`
- suite manifest: `tests/proof/hello-world-object/suite.manifest.json`
- contract overview: `contracts/docs/v0/overview.md`

## 当前结论

如果 sample source 与 canonical artifact / native landing 再次出现冲突，应优先保持本文件与上游 contract truth 一致，再对齐 sample source，而不是重新开放 Stage 1 已冻结的 contract surface。
