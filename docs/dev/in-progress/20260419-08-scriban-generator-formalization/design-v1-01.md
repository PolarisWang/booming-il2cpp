# Scriban Generator Formalization Design v1.01

Date: 2026-04-19 22:45:00 +08:00
Status: draft

## 1. 设计结论

把现有 `src/managed/Chaos.IL2CPP.CodeGen/` 原地重命名为 `src/managed/Chaos.IL2CPP.Generator/`，并保持 `third_party/scriban/src/Scriban/Scriban.csproj` 作为唯一第三方模板引擎入口。

这不是新增一层 wrapper，也不是再造一个 generator 子系统；本质上是对现有核心生成工程做 canonical naming cutover。

## 2. 命名决策

### 2.1 Canonical 名称

- 工程目录：`src/managed/Chaos.IL2CPP.Generator/`
- 项目文件：`Chaos.IL2CPP.Generator.csproj`
- namespace：`Chaos.IL2CPP.Generator`
- 上游引用：全部改为 `Chaos.IL2CPP.Generator`

### 2.2 不采纳 `CHaos.IL2CPP.Generator`

不建议采用 `CHaos.IL2CPP.Generator`，原因如下：

- 仓库现有命名统一为 `Chaos.*`。
- 单项目大小写漂移不会提升表达力，只会增加检索、路径、引用和文档不一致。
- 若要品牌改写，应作为仓库级别任务处理，而不是借这次 Scriban 正式接入做局部例外。

## 3. Scriban 的正式位置

`Scriban` 在 cutover 后的角色保持不变，但治理地位提升为显式 canonical 规则：

- 仍通过 `ProjectReference` 直接引用 vendored 工程：
  - `third_party/scriban/src/Scriban/Scriban.csproj`
- 仍由共享渲染入口统一加载与渲染：
  - `Templating/ScribanTemplateRenderer.cs`
- 仍作为文件级结构化文本生成的默认路径：
  - `.cpp`
  - `.h`
  - `.g.cs`
  - `.json`

明确不做：

- 不切到 NuGet 包引用。
- 不复制 `Scriban` 源码到 `src/managed/Chaos.IL2CPP.Generator/`。
- 不为 `Scriban` 再套一个仓库内私有 fork assembly。

## 4. 架构边界

本次变更只处理命名和 canonical 接入，不推翻 2026-04-17 已冻结的结构治理路线。

因此保留以下边界：

- 一个核心生成工程，继续承载：
  - `CoreIr`
  - `Planning`
  - `RuntimeSupport`
  - `Emission`
  - `ReferenceProof`
  - `Templating`
  - `Templates`
- 不新增并行 managed assembly。
- 不把 `Scriban` 变成“外围工具层”。
- 不重新放宽手写 `StringBuilder` 作为主生成面。

## 5. 数据流

cutover 前后，核心生成链不变：

`Loader/SemanticWorld/Linker/MetadataWriter -> GeneratorStage -> lowering plans -> NativeReference/NativeAot emitters -> Scriban templates -> generated artifacts`

变化只有 canonical 标识：

- `Chaos.IL2CPP.CodeGen` -> `Chaos.IL2CPP.Generator`
- `using Chaos.IL2CPP.CodeGen;` -> `using Chaos.IL2CPP.Generator;`
- 项目路径、测试锚点、authority 文档同步迁移

## 6. 影响面

### 6.1 必改源码

- `src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj`
- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
- `src/managed/Chaos.IL2CPP.Pipeline/Chaos.IL2CPP.Pipeline.csproj`
- `src/managed/Chaos.IL2CPP.Pipeline/PipelinePlan.cs`
- `src/managed/Chaos.IL2CPP.Generator/**` 下所有 namespace 与相对引用

### 6.2 必改测试

- `tests/unit/run/test_repo_layout.py`
- `tests/unit/planning/test_project_workspace.py`
- 直接锚定 `src/managed/Chaos.IL2CPP.CodeGen/**` 的 compatibility tests
- `tests/support.py`

### 6.3 必改 authority / wiki

- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `docs/architecture/runtime-baseline/repo-layout.md`
- `docs/architecture/runtime-baseline/ownership-map.md`
- `wiki/04-工具与集成/il2cpp-core-structure-and-scriban-governance.md`

### 6.4 不建议回写的历史文档

不建议大规模修改 `docs/dev/completed/**` 里的历史任务文档。

原因：

- 它们记录的是当时真实发生的任务与路径。
- 全量回写收益很低，且会模糊历史上下文。
- 只需要保证当前 authority 文档和活动代码树使用新名称。

## 7. 错误处理与风险

### 7.1 风险

- 目录与 `.csproj` 改名后，测试中的硬编码路径会大量失效。
- 某些 Python 规划逻辑把 `Chaos.IL2CPP.CodeGen.csproj` 作为 canonical 项目路径，若漏改会直接破坏 workspace 构造。
- 如果保留旧目录或 wrapper，会让 cutover 长期悬而未决。

### 7.2 应对

- 一次性 cutover，不保留双轨。
- 先改 source/test/authority doc，再做验证。
- 历史文档不追求全量回写，只更新“当前真相源”。

## 8. 验证建议

最少验证链：

1. `python -m pytest tests/unit/run/test_repo_layout.py -q`
2. `python -m pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance.py -q`
3. `python -m pytest tests/unit/compatibility/test_scriban_vendor_build.py -q`
4. `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

推荐补一条真实生成链：

5. 任选一个现有 native generation 入口，验证 `Driver -> Generator -> Scriban` 仍然能产出 generated artifacts。

## 9. 实施顺序

### Step 1

冻结 canonical 名称为 `Chaos.IL2CPP.Generator`。

### Step 2

原地重命名：

- 目录
- `.csproj`
- namespace
- 上游 `ProjectReference`

### Step 3

修正测试与 Python 规划层中的路径/工程名锚点。

### Step 4

更新 authority 文档和 wiki，把 `Generator` 设为当前真相源。

### Step 5

跑最小验证链并确认没有残留 `CodeGen` 双轨入口。

## 10. 结论

如果你的目标是“正式把 Scriban 纳入 IL2CPP 核心层，并把该层命名为 Generator”，那正确操作不是新建一个 `Generator` 工程，而是把现有 `Chaos.IL2CPP.CodeGen` 原地 cutover 成 `Chaos.IL2CPP.Generator`，同时保留 vendored `Scriban` 的直接引用方式。
