# Scriban Generator Formalization 实现计划

> **面向执行 Agent：** 必须按 TDD 顺序执行：先补/改失败测试，再改实现，再做 wiki 与 authority 收口。

**目标：** 将现有 `Chaos.IL2CPP.CodeGen` 正式 cutover 为 `Chaos.IL2CPP.Generator`，同步收口 `Scriban` 的核心层定位、Python 生成入口、wiki 使用文档与 codegen 规范。

**架构：** 保持单一核心生成工程，不新建并行壳项目。`Scriban` 继续通过 vendored `ProjectReference` 进入生成层，所有结构化 codegen 规则统一经 `Chaos.IL2CPP.Generator` 和其 `Templating/Templates` 基础设施落地；当现有姿势不支持时，扩展 Generator，而不是绕开它写临时生成逻辑。

**技术栈：** C#/.NET 8、MSBuild、vendored `Scriban`、Python workspace/planning tooling、repo wiki、`pytest`

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a
- capabilityItem: scriban-generator-formalization
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: collector -> registry -> workspace

**设计文档：** `docs/dev/in-progress/20260419-08-scriban-generator-formalization/design-v1-01.md`

**预期知识沉淀：**
- `wiki/04-工具与集成/il2cpp-generator-usage-and-codegen-governance.md`
- `wiki/04-工具与集成/INDEX.md`
- 视情况同步更新 `wiki/04-工具与集成/il2cpp-core-structure-and-scriban-governance.md`

**收尾约束：** 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 文档/wiki 收口 -> 保持无双轨旧名”的固定链路。

---

## 目标文件与职责

- `src/managed/Chaos.IL2CPP.Generator/**`
  - 新 canonical 生成工程目录与源码。
- `src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj`
  - 改引用到 `Chaos.IL2CPP.Generator`。
- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
  - 改 `using` 与调用入口。
- `src/managed/Chaos.IL2CPP.Pipeline/Chaos.IL2CPP.Pipeline.csproj`
  - 改引用到 `Chaos.IL2CPP.Generator`。
- `src/managed/Chaos.IL2CPP.Pipeline/PipelinePlan.cs`
  - 改 `using` 与 stage 依赖。
- `build/toolchains/run/subject/project_workspace.py`
  - Python 核心工作区生成入口改为识别并引用 `Chaos.IL2CPP.Generator`。
- `tests/support.py`
  - shared helper 改到 `Chaos.IL2CPP.Generator` 路径。
- `tests/unit/run/test_repo_layout.py`
  - 仓库 canonical 项目布局与依赖关系切到 `Generator`。
- `tests/unit/planning/test_project_workspace.py`
  - Python 工作区生成用例改为引用 `Generator`。
- `tests/unit/compatibility/**`
  - 所有直接锚定 `Chaos.IL2CPP.CodeGen` 路径/项目名的用例切到 `Generator`。
- `docs/dev/ACTIVE.md`
  - 当前 active task 指向本任务。
- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
  - authority 文档明确 `Chaos.IL2CPP.Generator` 为 IL2CPP 核心 codegen 工程。
- `docs/architecture/runtime-baseline/repo-layout.md`
  - 当前 repo layout 真相源切到 `Generator`。
- `docs/architecture/runtime-baseline/ownership-map.md`
  - ownership map 切到 `Generator`。
- `wiki/04-工具与集成/il2cpp-core-structure-and-scriban-governance.md`
  - 同步 canonical 名称与“姿势不支持就扩展 Generator”的长期规则。
- `wiki/04-工具与集成/il2cpp-generator-usage-and-codegen-governance.md`
  - 新增 Generator 使用与 codegen 规范页。
- `wiki/04-工具与集成/INDEX.md`
  - 纳入新页面与入口。

## 执行步骤

- [ ] **步骤 1：先补/改失败测试，锁定新 canonical 名称**
  - 修改 `tests/unit/run/test_repo_layout.py`，把 managed project canonical 名称从 `Chaos.IL2CPP.CodeGen` 改为 `Chaos.IL2CPP.Generator`。
  - 修改 `tests/unit/planning/test_project_workspace.py`、`tests/support.py` 及最小必要 compatibility 用例，使它们锚定 `src/managed/Chaos.IL2CPP.Generator/**`。
  - 运行最小测试子集，确认在源码尚未重命名前确实失败。

- [ ] **步骤 2：实施单次工程 cutover**
  - 原地重命名目录 `src/managed/Chaos.IL2CPP.CodeGen` -> `src/managed/Chaos.IL2CPP.Generator`。
  - 重命名 `.csproj` 为 `Chaos.IL2CPP.Generator.csproj`。
  - 批量修改 namespace / `using` / `ProjectReference` / Python 硬编码路径。
  - 不保留长期 wrapper project、alias project 或双轨路径。

- [ ] **步骤 3：收口 Python 生成入口**
  - 更新 `build/toolchains/run/subject/project_workspace.py` 和相关测试夹具，使 Python 生成核心工作区时引用 `Chaos.IL2CPP.Generator`。
  - 明确 Python 侧 codegen 应消费/调用 Generator，而不是再单独维护旁路生成逻辑。

- [ ] **步骤 4：写入 authority 文档与 wiki**
  - 更新 architecture authority 文档中的 canonical 工程名与边界。
  - 新增 wiki 页面，明确：
    - `Chaos.IL2CPP.Generator` 的职责
    - `Scriban` 的正式位置
    - 文件级 codegen 默认走 Generator
    - 当当前姿势不支持时，应扩展 Generator，而不是写临时字符串拼接/旁路 Python 生成器
    - Python 如何使用 Generator 生成代码
  - 更新 `wiki/04-工具与集成/INDEX.md`。

- [ ] **步骤 5：验证与收口**
  - 跑 repo layout / planning / compatibility / build 验证。
  - 确认不存在活动代码树中的 `Chaos.IL2CPP.CodeGen` canonical 入口残留。
  - 如验证通过，更新任务 `STATUS.md` 并准备归档。

## 关键测试与验证命令

- `python -m pytest tests/unit/run/test_repo_layout.py -q`
- `python -m pytest tests/unit/planning/test_project_workspace.py -q`
- `python -m pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance.py -q`
- `python -m pytest tests/unit/compatibility/test_scriban_vendor_build.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

建议补充：

- `python -m pytest tests/unit/planning/test_canonical_solution_project_paths.py -q`
- 任选一条真实 `Generator` 生成链命令，验证 Python/workspace -> Driver -> Generator -> Scriban 仍能产出 generated artifacts
