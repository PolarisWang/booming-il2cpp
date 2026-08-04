# Harness Engineer 自动化设计

## 背景

当前仓库中的 `.codex/skills/` 提供了一套通用工作流技能，但还缺少面向本项目的两个关键能力：

1. 执行中的计划缺少稳定的“当前上下文”承载体，导致恢复执行时需要依赖会话记忆。
2. 执行过程中的经验与项目知识没有稳定分流，过程文档与长期知识容易混在一起。

本设计的目标是将“执行上下文管理”和“项目 wiki 沉淀”纳入现有 skill 主线，而不把 `.codex`、`docs`、`wiki` 混成一套。

## 目标

建立一套与项目绑定的 Harness Engineer 自动化机制，使 agent 在执行计划时能够：

- 在任何问答前检测是否存在活动计划。
- 使用 `docs/executions/CURRENT.md` 持久化当前执行上下文。
- 将历史执行归档到 `docs/executions/history/`，并带明确状态。
- 在每个任务后沉淀长期有效知识到项目 `wiki/`。
- 用项目化 wiki 解释 `.codex/skills/` 与项目流程、目录、知识体系的绑定关系。

## 非目标

本次设计不包含以下内容：

- 不实现通用数据库或索引服务。
- 不引入新的 GUI、Web 服务或外部存储。
- 不把执行过程文档写入 `wiki/`。
- 不要求为所有通用 skill 提供完整项目说明页；对通用 skill，registry 登记即可。

## 约束

- `.codex/skills/` 是 skill 实现真源。
- `wiki/` 是项目知识真源。
- `docs/executions/` 是执行过程真源。
- 三者必须通过链接绑定，但不能互相复制原文。
- 只要 `docs/executions/CURRENT.md` 存在，任何问答前都必须先要求用户处理当前计划。
- 用户只允许选择 `继续` 或 `放弃`。
- `completed` 只能由 agent 在满足完成条件时自动归档，不能由用户手动选择。

## 顶层架构

### 1. Skill 实现层

保留现有主线技能，并新增两个横切技能：

- 继续使用：`brainstorming`、`writing-plans`、`executing-plans`、`subagent-driven-development`、`finishing-a-development-branch`
- 新增：`active-execution-guard`
- 新增：`project-wiki-maintenance`

其中：

- `active-execution-guard` 负责任何回答前的活动计划检查。
- `project-wiki-maintenance` 负责项目 wiki 的结论化沉淀与索引维护。

### 2. 执行过程层

执行过程统一进入 `docs/executions/`：

```text
docs/
  executions/
    CURRENT.md
    history/
      execution-<YYYY-MM-DD-HH-mm>-<feature-name>-completed.md
      execution-<YYYY-MM-DD-HH-mm>-<feature-name>-abandoned.md
```

语义如下：

- `CURRENT.md`：唯一当前执行文档，可恢复执行。
- `history/`：归档后的 execution 文档，状态只允许 `completed` 或 `abandoned`。

### 3. 项目知识层

项目长期知识进入 `wiki/`：

```text
wiki/
  INDEX.md
  01-项目总览/
  02-Skill体系/
  03-功能模块/
  04-工具与集成/
  05-项目经验/
```

边界如下：

- `03-功能模块`：功能本体知识。
- `05-项目经验`：跨功能复用经验。
- `wiki/` 不写执行流水、对话过程、临时尝试。

## 核心流程

### 流程 A：任意问答前的守卫

1. 检查 `docs/executions/CURRENT.md` 是否存在。
2. 若不存在，正常进入后续技能或回答。
3. 若存在，先读取 `CURRENT.md`。
4. 告知用户存在活动计划，并提供两种选择：
   - `继续`
   - `放弃`
5. 未处理前，agent 不得继续任何新的回答、规划或决策。

### 流程 B：继续当前计划

1. 读取 `CURRENT.md` 作为恢复执行的唯一入口。
2. 基于其中的结构化上下文恢复执行：
   - 设计文档路径
   - 计划文档路径
   - 当前任务进度
   - 已确认的上下文与决策
   - 当前约束、风险、阻塞
   - 下一步动作
3. 执行当前任务或下一任务。
4. 每完成一个任务后更新 `CURRENT.md`。
5. 如产生长期有效知识，更新 `wiki/` 并同步索引。
6. 全部任务完成且验证通过、应写入的 wiki 已更新后，自动归档为 `completed`。

### 流程 C：放弃当前计划

1. 读取 `CURRENT.md`。
2. 将当前内容归档到 `docs/executions/history/`，状态设为 `abandoned`。
3. 删除或移走 `CURRENT.md`，使仓库回到“无活动计划”状态。

## 文档模型

### CURRENT.md 模板

`docs/executions/CURRENT.md` 固定包含：

- 基本信息
- 文档链接（设计、计划、历史）
- 执行进度（任务总数、已完成、进行中、未开始）
- 已确认的重要上下文
- 用户已确认决策
- 当前约束
- 风险与阻塞
- 最近执行摘要
- 下一步

它必须是结构化总结，不记录聊天流水。

### 历史 execution 模板

`docs/executions/history/` 下的 execution 归档固定包含：

- 归档信息
- 文档链接
- 执行概览
- 关键上下文摘要
- 任务执行记录
- 错误与纠正
- 经验沉淀
- Wiki 同步记录
- 收尾说明

## 计划与执行的绑定

为了让执行文档能稳定恢复上下文，相关 skill 需要遵守如下绑定：

- `writing-plans` 写出的计划要保持任务计数清晰，便于执行时确认任务总数。
- `executing-plans` 与 `subagent-driven-development` 在开始执行时创建或更新 `CURRENT.md`。
- `CURRENT.md` 必须记录设计文档路径和计划文档路径。
- 每完成一项任务，执行 skill 必须更新：
  - 当前进度
  - 最近执行摘要
  - 错误与纠正
  - 经验沉淀
  - 下一步

## Wiki 结构设计

### 1. Index 规则

每层目录必须存在固定模板的 `INDEX.md`。它负责：

- 解释本层职责。
- 列出本层子类别。
- 列出本级重要文档。
- 记录本层规则。
- 记录最近更新。

### 2. Skill 体系页面

`wiki/02-Skill体系/` 作为项目化解释层，结构如下：

```text
wiki/02-Skill体系/
  INDEX.md
  skill-registry.md
  01-主线工作流/
  02-执行守卫/
  03-项目知识维护/
  04-质量保障/
```

其中：

- `skill-registry.md` 收录 `.codex/skills/` 下全部 skill。
- 每个 skill 标注 `core / project / general`。
- 项目强绑定的 skill 具有项目说明页。
- 通用 skill 至少在 registry 中登记。

### 3. 功能模块页面

`wiki/03-功能模块/` 的二级目录固定：

```text
wiki/03-功能模块/
  INDEX.md
  01-执行系统/
  02-Skill工作流/
  03-文档体系/
  04-测试与验证/
  05-工具能力/
```

具体功能以三级目录形式进入上述固定大类。

## 任务完成后的沉淀规则

每完成一项任务后：

1. 先更新 `docs/executions/CURRENT.md`。
2. 判断是否产生长期有效的项目知识。
3. 如果有，必须写入 `wiki/`。
4. 更新 `wiki/` 时，必须同步维护所在目录和必要父级的 `INDEX.md`。

强约束：

- 只要产生了可复用规则、稳定结构、项目约定、功能认知或错误规避经验之一，就必须写入 `wiki/`。

## 需要修改的技能

### 新增技能

- `dev-active-execution-guard`
- `dev-project-wiki-maintenance`

### 增强技能

- `dev-writing-plans`
- `dev-executing-plans`
- `dev-subagent-driven-development`

## 测试策略

本次改动属于 skill 行为变更，优先采用 Claude Code skill 行为测试：

- 为新增技能添加直接问答测试：
  - `active-execution-guard`
  - `project-wiki-maintenance`
- 为现有技能补充新规则测试：
  - `executing-plans`
  - `subagent-driven-development`
  - 必要时补充 `writing-plans`

测试重点：

- 是否描述 `CURRENT.md` 语义与检查规则
- 是否只允许 `继续 / 放弃`
- 是否说明自动完成归档条件
- 是否说明 `docs` 与 `wiki` 的分离
- 是否说明任务后必须进行 wiki 沉淀与索引维护

## 风险与缓解

### 风险 1：规则散落，执行时漏掉

缓解：

- 新增 `active-execution-guard`
- 在执行 skill 中明确引用守卫与 wiki 维护 skill

### 风险 2：wiki 再次混入过程流水

缓解：

- 将边界写入 `project-wiki-maintenance`
- 在 wiki `INDEX.md` 与 skill 页面中重复强调“结论化知识”

### 风险 3：registry 与实现漂移

缓解：

- wiki 中的 skill 名必须和 `.codex/skills/` 实现名一致
- skill 改动时同步更新 registry 与对应说明页

## 实施结果标准

实现完成后应满足：

- 存在新的执行守卫 skill 与项目 wiki 维护 skill
- 现有执行 skill 明确维护 `CURRENT.md` 与 history archive
- 项目 wiki 已建立顶层结构、固定 `INDEX.md` 体系与 skill registry
- 测试能够覆盖新增和增强的 skill 规则
