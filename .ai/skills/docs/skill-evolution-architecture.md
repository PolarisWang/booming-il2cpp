# Skill 自进化体系架构

## 架构总览

自进化体系由五层组成，从数据采集到正式发布形成完整闭环：

```
┌─────────────────────────────────────────────────────────┐
│                     HOOK 层（实时）                       │
│  PostToolUse: usage.jsonl + tool_outcomes.jsonl          │
│  Stop:        signals/YYYY-MM.jsonl + session_outcomes   │
└──────────────────────┬──────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────┐
│                   HEALTH 层（每次会话后）                  │
│  health_engine.py compute/report/trend                   │
│  产出: health-snapshot-{date}.json + dashboard           │
└──────────────────────┬──────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────┐
│                 EVOLUTION 层（每日/每周）                  │
│  evolve.py propose/auto-evolve/status                    │
│  触发: FIX | DERIVED | CAPTURED                          │
│  产出: evolution/proposals/<type>-<id>/                   │
└──────────────────────┬──────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────┐
│              BENCHMARK/REVIEW/PROMOTE 层                  │
│  skill_learn.py evolve-benchmark/review/promote          │
│  准入: 版本完整性 + manifest + overlap 检查               │
│  产出: 更新 library/skills/ + lineage + catalog          │
└──────────────────────┬──────────────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────────────┐
│                  OBSERVE 层（持续）                        │
│  evolve-history / observe / auto-assess / auto-report    │
│  产出: 版本谱系 + 系统健康报告                             │
└─────────────────────────────────────────────────────────┘
```

## 新文件索引

| 文件 | 用途 |
|------|------|
| `skills/hooks/hook_track_tool_outcomes.py` | PostToolUse: 记录工具调用成败 |
| `skills/hooks/hook_track_session_outcomes.py` | Stop: 记录会话完成状态 |
| `skills/tooling/learning/health_engine.py` | 计算 per-skill 健康指标 |
| `skills/tooling/learning/evolve.py` | 进化触发引擎 (FIX/DERIVED/CAPTURED) |

## 修改文件索引

| 文件 | 变更 |
|------|------|
| `skills/tooling/learning/skill_learn.py` | 新增 4 子命令: evolve-benchmark, evolve-review, evolve-promote, evolve-history |
| `skills/tooling/catalog/generate_skill_catalog.py` | registry 增加 version 列 |
| `.claude/settings.json` | 新增 2 个 hooks |
| `skills/library/skills/*/skill.manifest.json` | 增加 version_history + evolution_lineage 字段 |

## 新目录索引

| 目录 | 用途 |
|------|------|
| `.ai/skills/lifecycle/telemetry/health/` | 健康指标快照 |
| `.ai/skills/lifecycle/evolution/proposals/` | 进化提案 |
| `.ai/skills/lifecycle/evolution/lineage/` | 版本谱系记录 |

## 相关技能

- `dev-using-booming` — 入口路由器，不直接关联但进化系统依赖其选择逻辑
- `dev-writing-skills` — 新建 skill 的规范与进化系统互补
- `dev-verification-before-completion` — 进化后验证 gate
