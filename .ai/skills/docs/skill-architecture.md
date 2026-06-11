# Skill 架构

本文记录 `skills/` 当前采用的方案 A：按职责分层。

## 加载链

```
skills/library/skills/<entry>/SKILL.md
  -> skills/discovery/skill-index.md
     -> skills/discovery/registries/<domain>.md
        -> skills/library/skills/<name>/SKILL.md
           -> references/ / scripts/ / assets/ on demand
```

## 目录结构

```
skills/
  INDEX.md                         # 顶层导航
  docs/
    skill-architecture.md          # 本文件
    skill-authoring-and-import-policy.md
    skill-evolution-loop.md
    agent-guide.md                 # Agent 入口说明
  discovery/
    domain-catalog.json            # 领域定义
    skill-index.md                 # 自动生成：所有技能索引
    registries/
      INDEX.md                     # 领域注册表索引
      <domain>.md                  # 按领域分组的 registry
  runtime/
    bootstrap-skills.ps1           # 初始化：生成索引 + 创建 junction
    sync-skills.ps1                # 增量同步（junction 失效时降级为复制）
  tooling/
    catalog/
      generate_skill_catalog.py    # 索引生成器
    learning/
      skill_learn.py               # 自学习管道
    scaffolding/
      create-skill-scaffold.ps1    # 脚手架生成器
    verification/
      verify-skill-pipeline.ps1    # 全链路验证
  hooks/
    hook_track_usage.py            # PostToolUse: 记录 SKILL.md 读取
    hook_evaluate_session.py       # Stop: 评估会话质量
  library/skills/                  # 正式技能库（唯一可信源）
    <project-skill>/
      SKILL.md
      skill.manifest.json
      references/
      scripts/
      assets/
  lifecycle/
    learning/
      signals/                     # 原始学习信号
      candidates/                  # 聚类后的 candidate
    incubator/                     # 草稿区
    telemetry/                     # 使用率数据
    governance/                    # 晋升/退役日志
```

## 分层职责

| 层 | 路径 | 职责 |
|----|------|------|
| 文档层 | `docs/` | 说明结构、边界、导入规则和演化流程 |
| 发现层 | `discovery/` | 自动生成的技能索引与 registry |
| 运行层 | `runtime/` | 跨 Agent 分发脚本（bootstrap/sync） |
| 工具层 | `tooling/` | 生成、校验、脚手架和 learning pipeline 脚本 |
| 正式库层 | `library/skills/` | 正式 project skill 正文 |
| 生命周期层 | `lifecycle/` | learning、draft、telemetry、governance 产物 |

## 关键约束

1. `library/skills/` 是唯一编辑点。`.claude/skills/` 和 `.codex/skills/` 由 junction 自动同步。
2. `discovery/skill-index.md` 与 `discovery/registries/*.md` 是生成产物，不手工维护长表。
3. `library/skills/` 只接收正式内容；draft 必须停留在 `lifecycle/incubator/`。
4. `lifecycle/` 不直接参与发现层。
5. `runtime/bootstrap-skills.ps1` 必须先刷新 catalog，再更新 junction。
6. `tooling/verify-skill-pipeline.ps1` 负责校验 bootstrap、catalog、junction 和 formal library 准入规则。

## 变更规则

1. 新增领域：先改 `discovery/domain-catalog.json`，再刷新 catalog。
2. 调整目录职责或加载链：必须同步更新 INDEX.md、agent-guide.md、skill-authoring-and-import-policy.md。
3. 引入或拆分 skill：优先经过 `lifecycle/incubator/`，再进入 `library/skills/`。
