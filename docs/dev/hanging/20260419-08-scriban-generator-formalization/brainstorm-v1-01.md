# Scriban Generator Formalization Brainstorm v1.01

Date: 2026-04-19 22:45:00 +08:00
Status: draft

## 1. 现状

仓库已经满足下面三件事：

- `third_party/scriban` 已 vendored 到仓库。
- `src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj` 已直接 `ProjectReference` 到 `third_party/scriban/src/Scriban/Scriban.csproj`。
- `ScribanTemplateRenderer`、`NativeAotTemplateCatalog`、`NativeReferenceProofCatalog` 已经是当前核心生成链的一部分。

所以这次任务不是“要不要接入 Scriban”，而是：

1. 是否把现有 `CodeGen` 命名正式收口为 `Generator`。
2. 是否把 `Scriban` 的地位从“已有依赖”提升为“核心层 canonical 组成部分”。
3. 如何做一次不留双轨的 cutover。

## 2. 约束

- 2026-04-17 已冻结的结构治理路线明确要求：
  - `Scriban` 属于 IL2CPP 核心 codegen 层。
  - 不建议新建并行 assembly 来复制当前边界。
  - 优先在现有核心工程内收口，而不是分裂出更多壳项目。
- 当前大量测试和 authority 文档把 `Chaos.IL2CPP.CodeGen` 当作 canonical 路径。
- 仓库的 cutover 规则不鼓励长期保留 alias、旧命名和双轨逻辑。

## 3. 可选方案

### 方案 A：原地重命名 `Chaos.IL2CPP.CodeGen` 为 `Chaos.IL2CPP.Generator`

做法：

- 重命名目录、`.csproj`、assembly 名、namespace。
- 保留现有内部目录分层和 `Scriban` vendor 引用方式。
- 同步更新 Driver、Pipeline、测试和 authority 文档。

优点：

- 最符合“正式收口”的目标。
- 没有双轨。
- 保留现有代码资产和已完成的分层治理。

代价：

- 影响面大，尤其是测试和文档路径。
- 需要一次性把 canonical 名称切干净。

### 方案 B：新增 `Chaos.IL2CPP.Generator`，内部转发到 `Chaos.IL2CPP.CodeGen`

做法：

- 新建一个壳工程。
- 暂时保留旧工程。
- 通过转发或引用方式逐步迁移。

优点：

- 短期代码变动看起来更小。

缺点：

- 明显违背仓库“cutover 后删除旧 alias/旧双轨”的治理方向。
- 两个名字并存，后续更难清理。
- 文档、测试、脚本最终还是都要改一遍。

结论：不推荐。

### 方案 C：继续保留 `Chaos.IL2CPP.CodeGen`，只在文档里声明 Scriban 已正式纳入核心层

做法：

- 不改工程名。
- 只补 authority 文档与 wiki。

优点：

- 风险最低。

缺点：

- 不能满足用户“命名为 `Generator`”的目标。
- 名称与治理意图继续错位。

结论：只适合用户放弃重命名时采用。

## 4. 推荐

推荐方案 A，但名字建议定为 `Chaos.IL2CPP.Generator`，不是 `CHaos.IL2CPP.Generator`。

原因：

- 仓库现有全量 managed 项目统一采用 `Chaos.*` 前缀。
- `CHaos` 会制造一个新的大小写品牌分支，技术收益为零，维护成本为正。
- 如果你真想把品牌整体改成 `CHaos`，那是另一条仓库级命名改造任务，不应该只在一个工程上单点发生。

## 5. 成功标准

- `Chaos.IL2CPP.Generator` 成为唯一 canonical 名称。
- `Scriban` 继续通过 vendored `ProjectReference` 进入生成层，不复制源码，不切回字符串拼接主线。
- `Driver`、`Pipeline`、测试、authority 文档与 wiki 统一指向新名称。
- 不保留长期 `CodeGen` alias、wrapper project 或双轨引用。
