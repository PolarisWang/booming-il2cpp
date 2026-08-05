# Skill 创建与引入规范

本文约束在项目中如何新建、拆分、引入和治理 skill。默认遵循 [skill-architecture.md](./skill-architecture.md) 的 A 方案分层结构。

## 总原则

1. 新建或引入 skill 前，先判断是否应该扩展已有 skill，而不是默认新增。
2. `SKILL.md` 只写工作流、判断规则和必要说明；重资料一律放进 `references/`、`scripts/`、`assets/`。
3. registry 只保留最小发现信息，不写长流程。
4. 任何会增加启动层常驻上下文的调整，都必须先评估 token 成本。

## 正式 skill 必需结构

```
library/skills/<name>/
  SKILL.md
  skill.manifest.json
```

可按需补充：`references/`、`scripts/`、`assets/`。

## Scaffold 命令

推荐使用脚手架创建新 skill：

```powershell
pwsh -File .ai/skills/tooling/scaffolding/create-skill-scaffold.ps1 -DirectoryName <Name> -Domain <Domain>
```

约束：

1. `DirectoryName` 只允许小写字母、数字和短横线。
2. `Domain` 必须来自 `discovery/domain-catalog.json`。
3. `Resources` 可选，仅允许 `scripts`、`references`、`assets`。
4. scaffold 写入 `lifecycle/incubator/manual-<DirectoryName>/`，不会直接落到 `library/skills/`。
5. 默认不刷新 discovery 索引，避免占位文本进入正式发现层。

## 新建 Skill 规则

### 1. 先做归类判断

新建 skill 前先回答三件事：

1. 它属于哪个 domain。
2. 是否可以并入已有 skill。
3. 它是不是一个稳定、可复用的问题域。

满足任一情况时，不应新建：

- 只是一次性任务。
- 只是某个已有 skill 的补充章节。
- 只是项目临时过程记录。
- 可以直接靠脚本、lint 或 CI 自动化约束的机械规则。

### 2. 命名规则

1. 目录名使用小写、数字和短横线。
2. `SKILL.md` frontmatter 的 `name` 必须唯一。
3. 不允许创建同义重复 skill。
4. 名称优先表达动作或能力，不表达临时项目名。

### 3. Manifest 规则

必须提供：

- `kind`（固定为 `project`）
- `domain`
- `summary`
- `trigger_signals`
- `owner`
- `status`
- `startup_visible`（必须为 `false`）
- `token_budget`

约束：

1. `trigger_signals` 建议保持在 2-4 条。
2. `summary` 只写一句话摘要，不写完整流程。

## 外部 Skill 引入规则

外部 skill 不允许原样拷贝进仓库，必须按本项目结构重组。

### 模式 A: Reference-only

只引入参考资料、脚本或模板，不引入对方整套 skill。

### 模式 B: Adapted import（默认）

保留核心工作流，但本地重写：SKILL.md、manifest.json、目录命名、summary、trigger_signals。

### 模式 C: Split import

当外部 skill 过大或职责过宽时，拆成多个本地 skill，再分别归域。

## 生命周期闭环约束

1. `collect` / `cluster` 只写 `lifecycle/learning/`。
2. `draft` 只写 `lifecycle/incubator/`。
3. `promote` / `promote-manual` 之前，不允许任何草稿直接进入 `library/skills/`。
4. 只有正式 promote 后，才允许刷新 discovery 索引。

## 禁止项

1. 把完整 skill catalog 放回启动层常驻上下文。
2. 在 `discovery/registries/*.md` 手工维护长表。
3. 不带 manifest 合入 skill。
4. 直接复制外部 skill 而不做本地适配。
5. 把大段 API 文档直接塞进 `SKILL.md`。
