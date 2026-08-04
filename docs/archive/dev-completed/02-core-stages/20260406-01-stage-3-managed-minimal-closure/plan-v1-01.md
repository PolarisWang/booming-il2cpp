# Stage 3 Managed Minimal Closure 实现计划

> **面向执行 Agent：** 必须使用 `dev:subagent-driven-development`（如可用子 Agent）或 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 打通 `HelloWorldObject.dll` 到 managed closure bundle 的最小闭环，为 Stage 4 native proof 提供稳定上游。

**架构：** 先定义 Stage 3 的 closure bundle contract 和 proof-bound world model，再让 `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 形成真实数据流，最后由 `Driver` 提供可测试 orchestration 入口，并用 Python 单测锁住 closure 输出边界。Stage 3 到此为止，不越界进入 native bootstrap。

**技术栈：** .NET 8、`System.Reflection.Metadata + PEReader`、Stage 2 canonical proof spec、Python `unittest`

**设计文档：** `docs/dev/in-progress/20260406-01-stage-3-managed-minimal-closure/design-v1-01.md`

**预期知识沉淀：**

- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/managed-minimal-closure-v0.md`

---

## 目标文件与职责

- 修改：`docs/dev/in-progress/20260406-01-stage-3-managed-minimal-closure/STATUS.md`
  - 维护阶段、最近摘要与下一步。
- 修改：`docs/dev/in-progress/20260406-01-stage-3-managed-minimal-closure/notes/progress-v1-01.md`
  - 记录 closure 实现进展与验证结果。
- 创建或修改：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/managed-minimal-closure-v0.md`
  - 写明 Stage 3 closure bundle 边界、输入输出和 Stage 4 消费约束。
- 修改：`src/managed/Chaos.IL2CPP.Contracts/`
  - 增加 closure request / result / bundle model / proof-bound subject model。
- 修改：`src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - 从 `HelloWorldObject.dll` 读取 proof 所需 metadata。
- 修改：`src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
  - 把 loader 输出收束成 proof-bound semantic world。
- 修改：`src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 基于 entry subject 与 Stage 2 canonical dependency set 形成 retained world。
- 修改：`src/managed/Chaos.IL2CPP.MetadataWriter/MetadataWriterStage.cs`
  - 输出 `aot-manifest` 与 `metadata-registration`。
- 修改：`src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
  - 输出 `typed-il-ir`、`code-registration` 和 Stage 3 closure manifest。
- 修改：`src/managed/Chaos.IL2CPP.Pipeline/PipelinePlan.cs`
  - 把真实 stage object / data flow 挂到 pipeline 计划里。
- 修改：`src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
  - 提供可测试 orchestration 入口。
- 创建或修改：`tests/unit/run/test_stage3_managed_minimal_closure.py`
  - 锁 Stage 3 输出根目录、closure bundle 组成和 proof-bound 边界。

## 执行原则

- [ ] Stage 3 的完成边界固定在 managed closure bundle，不顺手越界实现 Stage 4。
- [ ] 必须从真实 proof input assembly 读取 metadata，不允许把 Stage 2 canonical JSON 当作“实现输出”直接回填。
- [ ] 所有输出都必须能追溯到 Stage 2 proof spec 和 canonical `subjectId`。
- [ ] 每完成一个 chunk，都同步更新 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md`。

## Chunk 1：定义 closure contract 与长期 spec

- [ ] 创建 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/managed-minimal-closure-v0.md`。
- [ ] 写明 Stage 3 输入：
  - `tests/proof/input/HelloWorldObject/bin/Release/net8.0/HelloWorldObject.dll`
- [ ] 写明 Stage 3 输出：
  - `artifacts/proof/managed-closure/HelloWorldObject/typed-il-ir.json`
  - `artifacts/proof/managed-closure/HelloWorldObject/aot-manifest.json`
  - `artifacts/proof/managed-closure/HelloWorldObject/metadata-registration.json`
  - `artifacts/proof/managed-closure/HelloWorldObject/code-registration.json`
  - `artifacts/proof/managed-closure/HelloWorldObject/closure.manifest.json`
- [ ] 在 `Chaos.IL2CPP.Contracts` 中定义 closure request / result / bundle model。

## Chunk 2：打通 Loader / SemanticWorld / Linker

- [ ] `LoaderStage` 读取 proof assembly 和 proof-bound metadata surface。
- [ ] `SemanticWorldStage` 构建 `Program / Greeter / _name / 3 methods` 的 world model。
- [ ] `LinkerStage` 依据 Stage 2 canonical dependency set 形成 retained world。

## Chunk 3：打通 MetadataWriter / CodeGen

- [ ] `MetadataWriterStage` 输出与 Stage 2 spec 对齐的 `aot-manifest` 和 `metadata-registration`。
- [ ] `CodeGenStage` 输出与 Stage 2 spec 对齐的 `typed-il-ir` 和 `code-registration`。
- [ ] 增加 `closure.manifest.json` 作为 Stage 3 聚合入口。

## Chunk 4：Orchestration 与测试

- [ ] `PipelinePlan` 与 `DriverEntry` 形成可执行 orchestration。
- [ ] 新增 `tests/unit/run/test_stage3_managed_minimal_closure.py`。
- [ ] 如需要，再补最小 integration-level 驱动验证，但只围绕 managed closure，不进入 native proof。

## Chunk 5：同步任务状态

- [ ] 更新 `STATUS.md` 的最近摘要、阶段和下一步。
- [ ] 更新 `docs/dev/ACTIVE.md`。
- [ ] 在 `notes/progress-*.md` 记录验证结果与剩余风险。
- [ ] 如本任务完成，准备把父 roadmap 的下一步切到 Stage 4 或 Stage 5 决策。

## 关键验证命令

- [ ] `dotnet build tests/proof/input/HelloWorldObject/HelloWorldObject.csproj -c Release`
- [ ] `dotnet build src/managed/Chaos.IL2CPP.Contracts/Chaos.IL2CPP.Contracts.csproj`
- [ ] `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj`
- [ ] `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure`
- [ ] `python -m unittest tests.unit.run.test_stage2_helloworld_proof_spec`

## 完成标准

- [ ] `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 形成真实 managed 数据流。
- [ ] Stage 3 输出 bundle 与 Stage 2 canonical proof spec 对齐。
- [ ] Stage 3 / Stage 4 的边界保持清晰，没有 native bootstrap 渗入。
- [ ] `STATUS.md`、`ACTIVE.md`、父 roadmap 与索引已同步。
