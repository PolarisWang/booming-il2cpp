# Stage 2 HelloWorld Proof Spec Brainstorm

Date: 2026-04-05 23:50:13 +08:00
Status: draft

## 任务意图

这个子任务负责把 object-based `HelloWorld` 冻结成唯一的第一条 proof 规格，而不是继续让 sample source、artifact snapshot 和 native direct-call example 各自代表不同版本的“HelloWorld”。

## 为什么单独拆出

- Stage 1A 已冻结 proof-facing contract surface。
- Stage 1B 已把 `tests/proof` 与 `tests/smoke` 分层落地。
- 但真正的 `HelloWorld` proof truth 仍未收口，继续进入 Stage 3 / Stage 4 会把分歧带进 managed pipeline 与 native bootstrap。

## 本任务聚焦

- 固定 object-based `HelloWorld` 的 canonical 输入、输出与退出语义
- 固定 managed 侧方法 / 字段最小集合
- 固定 `typed-il-ir`、`aot-manifest`、`code-registration`、`metadata-registration` 的追溯关系
- 固定 generated direct-call landing 的最小语义
- 固定 proof gate 的名称、边界和阶段归属

## 明确不做

- `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 的真实闭环实现
- `runtime-core -> bootstrap -> support` 的真实 native proof 打通
- `GenericEcho`、`ReflectionLite`、`PInvokeLite` 等 follow-on sample

## 当前差异

仓库已经有 3 份与 `HelloWorldObject` 相关的真源候选：

1. `tests/proof/input/HelloWorldObject/Program.cs`
2. `contracts/examples/v0/artifacts/*.json`
3. `contracts/native/examples/v0/bridge/direct-call-landing.cpp`

它们目前并不完全一致：

- `Program.cs` 仍保留 args-driven 的 `"object-world"` 默认值，并使用 `void Main`
- canonical artifact 与 native landing 已冻结为 `"World"`、`Program.Main(System.String[]) -> int32`、stdout `Hello, World!` 与 `return 0`

## 备选方案

### 方案 A：以当前 `Program.cs` 为真源

- 优点：最贴近仓库现存的 proof 输入项目
- 缺点：与 Stage 1 已冻结的 artifact / native direct-call example 冲突，会迫使上游 contract 重新开放

### 方案 B：以现有 contract / artifact / native landing 为真源

- 优点：与 parent design、Stage 1A 产物、`subjectId` / registration / direct-call landing 保持一致
- 缺点：后续需要把 `tests/proof/input/HelloWorldObject` 重新对齐到这份 canonical truth

### 方案 C：只冻结输出，不冻结 `Main` / helper 细节

- 优点：短期文档最少
- 缺点：会把真正的争议推迟到 Stage 3 / Stage 4，失去本任务存在的意义

## 当前推荐

- 采用 **方案 B**

原因：

- parent design 已明确第一条 proof 输出 `Hello, World!` 且返回 `0`
- Stage 1A 的 canonical artifact 与 direct-call landing 已经围绕这条 spine 冻结
- Stage 2 的职责就是把这些已存在的冻结结果收束成一份唯一规格，而不是再为当前 sample source 开第二套真源
