# Stage 2 HelloWorld Proof Spec 实现计划

> **面向执行 Agent：** 必须使用 `dev:subagent-driven-development`（如可用子 Agent）或 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 固定 object-based `HelloWorld` 的 canonical sample truth、artifact / helper 约束与 proof gate，使 Stage 3 / Stage 4 拥有稳定上游。

**架构：** 先把 Stage 1 已冻结的 artifact / native landing 收敛成一份长期规格文档，再对齐 `tests/proof/input/HelloWorldObject` 与 proof suite metadata，最后补最小验证，确保 source / artifact / 文档三者一致。Stage 2 到此为止，不进入 managed closure 或 native runtime 真正实现。

**技术栈：** .NET 8、现有 `contracts/artifacts/v0` schema/example、`contracts/native` proof-boundary、`tests/proof` suite manifest、Python `unittest`

**设计文档：** `docs/dev/in-progress/20260405-04-stage-2-helloworld-proof-spec/design-v1-01.md`

**预期知识沉淀：**

- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/hello-world-proof-spec-v0.md`

---

## 目标文件与职责

- 修改：`docs/dev/in-progress/20260405-04-stage-2-helloworld-proof-spec/STATUS.md`
  - 维护当前阶段、关键文档、最近摘要与下一步。
- 修改：`docs/dev/in-progress/20260405-04-stage-2-helloworld-proof-spec/notes/progress-v1-01.md`
  - 记录 spec 冻结过程中的完成项与验证结果。
- 创建或修改：`wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/hello-world-proof-spec-v0.md`
  - 形成长期有效的 canonical proof spec 文档。
- 修改：`tests/proof/input/HelloWorldObject/Program.cs`
  - 按 canonical truth 对齐 `Main` 签名、常量输入与输出语义。
- 修改：`tests/proof/hello-world-object/suite.manifest.json`
  - 明确 Stage 2 允许的 gate 与 metadata 描述，不误导为已具备 reference run。
- 创建或修改：`tests/unit/run/test_stage2_helloworld_proof_spec.py`
  - 覆盖 proof sample truth / manifest 关键信号，避免后续再次分叉。
- 视需要修改：`contracts/docs/v0/overview.md`
  - 增加对长期 proof spec 文档的入口链接或简要说明。

## 执行原则

- [ ] 所有改动都只服务于 Stage 2 的 proof spec freeze，不顺手实现 Stage 3 / Stage 4。
- [ ] 以 Stage 1 已冻结的 canonical artifact / native landing 为唯一真源，不再为当前 `Program.cs` 开第二套语义。
- [ ] 如果某个 gate 需要真实 generated code 执行或 reference runtime 运行，只在 Stage 2 写入 gate 定义和归属，不在本任务偷跑实现。
- [ ] 每完成一个 chunk，都同步更新 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md`。

## Chunk 1：写 canonical proof spec

- [ ] 创建 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/hello-world-proof-spec-v0.md`。
- [ ] 写明 canonical sample truth：
  - assembly：`HelloWorldObject`
  - stdout：`Hello, World!`
  - exit code：`0`
  - managed subjects：`Program`、`Greeter`、`_name`、3 个 methods
- [ ] 写明 artifact / native landing 的真源路径：
  - `contracts/examples/v0/artifacts/*.json`
  - `contracts/native/examples/v0/bridge/direct-call-landing.cpp`
- [ ] 写明 shared `subjectId`、direct-call landing 与 helper boundary 的关联。

## Chunk 2：对齐 proof input 与 suite metadata

- [ ] 将 `tests/proof/input/HelloWorldObject/Program.cs` 对齐到 canonical truth。
- [ ] 评估并更新 `tests/proof/hello-world-object/suite.manifest.json`，明确：
  - 当前 Stage 2 已覆盖哪些 gate
  - 哪些 gate 明确属于 Stage 3 / Stage 4
- [ ] 如需补摘要入口，在 `contracts/docs/v0/overview.md` 增加指向长期 spec 的链接。

## Chunk 3：补最小防回退验证

- [ ] 新增或修改 `tests/unit/run/test_stage2_helloworld_proof_spec.py`，覆盖：
  - `Program.Main` canonical truth
  - `HelloWorldObject` suite manifest 的阶段声明
  - spec 文档存在且引用 canonical artifact 路径
- [ ] 如 source 对齐导致 snapshot 或示例文档需要刷新，统一更新相关最小样例。

## Chunk 4：同步任务状态

- [ ] 更新 `STATUS.md` 的最近摘要、下一步与阶段。
- [ ] 更新 `docs/dev/ACTIVE.md` 的摘要与下一步。
- [ ] 在 `notes/progress-*.md` 记录完成项与验证结果。
- [ ] 如本任务完成，准备从 parent roadmap 切到 Stage 3 / Stage 4 派生决策。

## 关键验证命令

- [ ] `dotnet build tests/proof/input/HelloWorldObject/HelloWorldObject.csproj -c Release`
- [ ] `python -m unittest tests.unit.run.test_stage2_helloworld_proof_spec`
- [ ] `python -m unittest tests.unit.run.test_stage1b_layout`

## 完成标准

- [ ] `HelloWorld` proof 有唯一、可引用的 canonical spec 文档。
- [ ] `tests/proof/input/HelloWorldObject`、artifact 示例与 native direct-call landing 不再存在未解释的语义分叉。
- [ ] Stage 2 / Stage 3 / Stage 4 的 gate 归属明确。
- [ ] `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md` 与父 roadmap / 索引已同步。
