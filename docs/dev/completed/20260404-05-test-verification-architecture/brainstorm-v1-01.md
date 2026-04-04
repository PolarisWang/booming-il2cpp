# 测试与验证体系升级方案 Brainstorm

**日期：** 2026-04-04 20:00 +08:00

## 用户目标

用户希望把当前仓库的测试与验证体系从“统一测试命令 + 分散项目知识”升级为一套面向项目落地的完整体系，核心要求包括：

- `.codex/skills` 保持通用，只引用项目级测试知识，不直接承载项目细节。
- harness 测试流程需要显式区分 `代码测试 / 模块验证 / 整体验证`。
- 重要验证必须沉淀为正式可执行对象，不能长期停留在临时手工检查。
- 测试框架需要支持按模块接入、按系统场景接入、按测试管线编排，并指导 skill 如何引用这些对象。
- 测试注册、TUI、CLI、harness、skill 和错误定位要统一到同一套对象模型下。
- 最终只保留 `run test` 作为公开入口，一次性废除 `run verify`。

## 已对齐的核心判断

### 入口与知识分层

- 项目级测试知识唯一入口固定为 `wiki/06-测试验证/`。
- skill 优先读取机器入口，再通过 `docRefs` 跳转 `wiki/06-测试验证/` 及其子页。
- `wiki/06-测试验证/` 既服务人读，也保留稳定结构供机器回退消费。

### 验证层级与执行族

- `代码测试 / 模块验证 / 整体验证` 是新的“验证层级”维度，不替代现有 `family`。
- `code` 主体覆盖 `unit + integration + contract`。
- `module` 与 `system` 按验证语义定义，而不是按现有 family 强绑。
- suite 具有主层级，case 允许做例外覆盖。

### 正式对象层

- 新增正式对象：
  - 模块验证对象：`module/<module-id>/<profile-id>`
  - 系统场景对象：`system/<scenario-id>`
  - 测试管线对象：`pipeline/<pipeline-id>`
- 模块验证对象允许一模块多 profile。
- 测试管线是正式编排对象，不再使用“批次”命名。
- 测试管线允许混合 `suite / case / module verification / system scenario` 成员，并支持阶段分组、并行提示、前置准备和后置清理。

### 模块与实现归属

- 主模块采用 `primaryModuleId`。
- 关联模块采用 `moduleIds`。
- 实现侧子系统采用 `subsystemIds`。

### 重要验证沉淀

- 重要验证必须升格为正式对象，优先落为现有 suite 下的新 case，必要时才新建 suite。
- 允许临时例外，但必须登记，且只能以“事件触发”失效，不允许以时间失效。
- 重要验证升格规则需要成为可被 skill 和文档引用的正式规则集。

### 入口语义

- 公开入口统一为 `run test`，一次性废除 `run verify`。
- 命令实现采用“selector 内核 + 两种输入形式”：
  - 人工友好拆字段形式
  - harness 稳定对象 ID 形式
- `suite`、`module`、`system`、`pipeline` 都遵循这一原则。

### registry 与文档

- 运行注册与说明文档分层：
  - 运行注册位于 `tests/registry/...`
  - 说明文档位于 `wiki/06-测试验证/...`
- 统一 registry 同时扫描 `suite / module verification / system scenario / pipeline`。
- 静态索引快照需要稳定指针和历史快照两层。
- 自动刷新仅由运行注册文件变化触发；wiki 不参与自动刷新，但会在显式刷新和 CI 中做一致性校验。

### 结果与错误模型

- 结果必须输出完整归属链：
  - suite/case
  - module verification
  - system scenario
  - pipeline
- 错误定位最低要求为 `case + file + line/column`；拿不到时允许降级，但必须给出 `failureCode + parserHint + logPath`。
- `failureCode` 为机器字段，采用分层命名，粒度可细。
- `humanHint` 为人读排障主入口，要求输出“失败对象 + 原因 + 下一步建议”，且下一步建议必须落到正式对象、正式命令或明确日志路径。

## 方案收敛结论

本任务不再继续停留在“补几条文档规则”，而是收敛为一套完整的分层方案：

1. 保留现有 `family/suite/case` 执行层。
2. 新增 `module/system/pipeline` 正式对象层。
3. 新增统一 registry 与静态索引快照。
4. 将 skill、TUI、CLI、harness、wiki 统一绑定到同一对象模型。
5. 用结构化错误链和人类提示保证可定位、可恢复、可追踪。

下一步进入正式设计文档与实现计划。
