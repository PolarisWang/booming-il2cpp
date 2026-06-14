# AI Agent 入口说明

本页记录仓库对 AI coding agent 的长期有效入口约定。

## 当前加载链

1. 技能唯一可信源为 `skills/library/skills/`。`.claude/skills/dev-il2cpp/SKILL.md` 包含最小路由桩，加载后立即读取 library 版本。
2. 启动时 Agent 可通过 `skills/discovery/skill-index.md` 查阅完整技能目录。
3. 命中领域后读取 `skills/discovery/registries/<domain>.md`。
4. 最后按需读取 `skills/library/skills/<name>/SKILL.md`，以及 `references/` / `scripts/` / `assets/`。

## 目录职责

| 路径 | 职责 |
|------|------|
| `skills/discovery/skill-index.md` | 技能索引；列出所有领域与技能数量 |
| `skills/discovery/registries/<domain>.md` | 按领域拆分的二级 registry；从 manifest 自动生成 |
| `skills/discovery/domain-catalog.json` | 领域定义、摘要、读取提示与排序 |
| `skills/library/skills/` | 正式技能源目录（唯一可信源） |
| `skills/library/skills/<name>/skill.manifest.json` | 技能的结构化元数据 |
| `skills/lifecycle/incubator/` | 草稿区；不参与发现层 |
| `skills/lifecycle/learning/` | 学习信号与 candidate 聚类产物 |
| `skills/lifecycle/telemetry/` | 使用率数据 |
| `skills/lifecycle/governance/` | promotion / retire 日志与备份 |

## Agent 接入

| 工具 | 技能路径 |
|------|---------|
| Claude Code | `.claude/skills/dev-il2cpp/SKILL.md` (routing stub) → `.ai/skills/library/skills/dev-il2cpp/SKILL.md` |
| Codex CLI | `.codex/skills/` (if configured, similar routing approach) |

## 维护要求

1. 新增或引入 project skill 时，必须同时创建 `SKILL.md` 和 `skill.manifest.json`。
2. 新增领域前，先更新 `discovery/domain-catalog.json`。
3. 任何 `skill.manifest.json` 或 `discovery/domain-catalog.json` 变更后，都应重新运行 `generate_skill_catalog.py`。
4. 需要验证整条加载链时，运行 `verify-skill-pipeline.ps1`。
5. `library/skills/` 中不允许出现 `draft` manifest 或 scaffold 占位文本。
6. 不直接在 `discovery/registries/*.md` 手工维护长表。
