# Skill 自进化闭环

本文定义 `skills/` 的完整自学习闭环：从 telemetry 采集 → 健康度量 → 进化触发（FIX/DERIVED/CAPTURED）→ benchmark → review → promote → 正式发布。

## 正式加载链

`library/skills/ -> discovery/skill-index.md -> discovery/registries/<domain>.md -> library/skills/<name>/SKILL.md`

## 生命周期目录职责

| 路径 | 职责 |
|------|------|
| `lifecycle/learning/signals/` | 原始学习信号，按月写入 jsonl |
| `lifecycle/learning/candidates/` | 聚类后的 candidate、证据与 review 结果 |
| `lifecycle/incubator/` | 草稿 skill；包含 candidate-id 草稿与 manual-* 手工 scaffold |
| `lifecycle/telemetry/` | skill 健康快照与使用率数据 |
| `lifecycle/telemetry/usage.jsonl` | PostToolUse hook 记录的 SKILL.md 读取 |
| `lifecycle/telemetry/tool_outcomes.jsonl` | PostToolUse hook 记录的工具调用成败 |
| `lifecycle/telemetry/session_outcomes.jsonl` | Stop hook 记录的会话完成状态 |
| `lifecycle/telemetry/health/` | 健康度量引擎计算的 per-skill 指标快照 |
| `lifecycle/evolution/proposals/` | 进化提案（FIX/DERIVED/CAPTURED） |
| `lifecycle/evolution/lineage/` | 版本谱系记录（每 skill 一个 jsonl） |
| `lifecycle/governance/` | promote / retire 日志与备份 |

## 数据流总览

```
HOOK 层（每次会话自动运行）
  PostToolUse → hook_track_usage.py           → usage.jsonl
  PostToolUse → hook_track_tool_outcomes.py    → tool_outcomes.jsonl
  Stop        → hook_evaluate_session.py       → signals/YYYY-MM.jsonl
  Stop        → hook_track_session_outcomes.py → session_outcomes.jsonl

HEALTH 层（每次会话后 + 每日）
  health_engine.py compute --all → health-snapshot-{date}.json
  health_engine.py report        → 健康仪表盘

EVOLUTION 层（每日 dry-run 提案，每周执行）
  evolve.py propose --dry-run              → pending-report.json
  evolve.py auto-evolve
    ├── FIX:     完成率低 → 原地修复，patch version bump
    ├── DERIVED: 切换率高 → 派生新 skill，version 1.0.0
    └── CAPTURED: 高质量信号 → 提取新 skill，v0.1.0 → v1.0.0

BENCHMARK/REVIEW/PROMOTE 层
  skill_learn.py evolve-benchmark <proposal-id>
  skill_learn.py evolve-review <proposal-id>
  skill_learn.py evolve-promote <proposal-id>
    ↓
  版本谱系记录 + promotion log + catalog refresh
```

## 阶段一：Telemetry 采集

三个 PostToolUse hook 和一个 Stop hook 自动运行：

| Hook | 触发 | 产出 |
|------|------|------|
| `hook_track_usage.py` | 每次 Read SKILL.md | `usage.jsonl` |
| `hook_track_tool_outcomes.py` | 每次工具调用 | `tool_outcomes.jsonl` |
| `hook_evaluate_session.py` | 会话结束 (quality >= 0.7) | `signals/YYYY-MM.jsonl` |
| `hook_track_session_outcomes.py` | 会话结束 | `session_outcomes.jsonl` |

## 阶段二：健康度量

```powershell
# 一键自检（推荐日常使用）
python skills/tooling/learning/health_engine.py check --window 30

# 分步执行
python skills/tooling/learning/health_engine.py compute --all --window 30
python skills/tooling/learning/health_engine.py report --window 30
python skills/tooling/learning/health_engine.py trend --all
```

> `check` 子命令依次执行：compute → report → evolve.py propose，一条命令完成日常健康检查。

### Per-Skill 指标

| 指标 | 含义 | 阈值 |
|------|------|------|
| `applied_rate` | reads / total sessions | < 0.4 = LOW-USE |
| `completion_rate` | 完成会话 / 总会话 | < 0.6 = FIX 候选 |
| `tool_success_rate` | 成功调用 / 总调用 | < 0.7 = LOW-TOOL |
| `fallback_rate` | 回退会话 / 总会话 | > 0.3 = HIGH-FALLBACK |
| `context_switching_ratio` | 多 skill 会话比例 | > 0.5 = DERIVE 候选 |

## 阶段三：进化触发

```powershell
# Dry-run 预览
python skills/tooling/learning/evolve.py propose

# 执行进化
python skills/tooling/learning/evolve.py auto-evolve

# 单 skill 操作
python skills/tooling/learning/evolve.py auto-evolve-fix --skill dev-executing-plans
python skills/tooling/learning/evolve.py auto-evolve-derive --skill dev-brainstorm
python skills/tooling/learning/evolve.py auto-evolve-capture
```

### FIX 模式

- **触发**: `completion_rate < 0.6` 且 sessions >= 3
- **动作**: 复制 skill → 版本 patch bump → 添加 evolution annotation
- **产出**: `lifecycle/evolution/proposals/fix-<skill>-v<next>/`

### DERIVED 模式

- **触发**: `context_switching_ratio > 0.5` 且sessions >= 10
- **动作**: 生成变体 skill（独立目录、独立版本），父 manifest 记录 `derived_to`
- **产出**: `lifecycle/evolution/proposals/derive-<skill>-<variant>-v1/`

### CAPTURED 模式

- **触发**: quality signal >= 0.8, action_hint 为 new-skill/feature, 无已有 skill 重叠
- **动作**: 从信号提取新 skill 骨架（v0.1.0），promote 后升级到 v1.0.0
- **产出**: `lifecycle/evolution/proposals/capture-auto-<id>-v1/`

## 阶段四：准入与合入

```powershell
python skills/tooling/learning/skill_learn.py evolve-benchmark <proposal-id>
python skills/tooling/learning/skill_learn.py evolve-review <proposal-id>
python skills/tooling/learning/skill_learn.py evolve-promote <proposal-id> --dry-run
python skills/tooling/learning/skill_learn.py evolve-promote <proposal-id>
```

Promote 后自动执行管线验证（首选 `verify-skill-pipeline.ps1`，不可用时降级为 Python 版 `verify_skill_pipeline.py`）：

### 进化准入检查

| 类型 | benchmark 内容 |
|------|---------------|
| **FIX** | 版本升级完整性、manifest 版本字段、SKILL.md frontmatter |
| **DERIVE** | 同上 + 与正式 skill 重叠检查（排除 parent） |
| **CAPTURE** | 同上 + 与正式 skill 重叠检查 |

### Promote 行为

| 类型 | 动作 |
|------|------|
| **FIX** | 覆盖原 skill 目录（含备份），原地升级版本 |
| **DERIVE** | 在 library/skills/ 创建新目录，更新 parent 的 `derived_to` |
| **CAPTURE** | 在 library/skills/ 创建新目录，版本 0.1.0 → 1.0.0 |

## 阶段五：观察与版本历史

```powershell
# 查看特定 skill 的进化历史
python skills/tooling/learning/skill_learn.py evolve-history --skill dev-brainstorm

# 查看所有有历史记录的 skill
python skills/tooling/learning/skill_learn.py evolve-history

# 系统级观察
python skills/tooling/learning/skill_learn.py observe
python skills/tooling/learning/skill_learn.py auto-assess
python skills/tooling/learning/skill_learn.py auto-report
```

## 版本策略

| 进化类型 | 版本变化 | 示例 |
|----------|---------|------|
| **FIX** | patch bump | 1.0.0 → 1.0.1 |
| **DERIVED** | 新 skill 从 1.0.0 开始 | (new) 1.0.0 |
| **CAPTURE** | 孵化 0.1.0，promote → 1.0.0 | 0.1.0 → 1.0.0 |

所有版本变更记录在 `lifecycle/evolution/lineage/<skill>.jsonl`，格式：
```jsonl
{"skill":"dev-brainstorm","version_from":"1.0.0","version_to":"1.0.1","type":"fix","timestamp":"...","reason":"completion_rate=0.45"}
```

## 安全机制

- 进化触发有最低 session 数量要求（FIX>=3, DERIVE>=10）
- benchmark 检查版本完整性、manifest 字段、TODO 残留
- promote 默认要求 benchmark + review 双通过
- FIX promote 前自动备份原 skill 到 `governance/backups/`
- 自动退役默认 dry-run；`auto-cleanup --health-engine` 会额外检查健康快照中的 LOW-USE 标记
- Promote 后自动运行管线验证（PS1 → Python 降级），确保入口桩/manifest/evolution 目录一致
- 所有操作写入 governance log

## 验证

```powershell
# Python 版管线验证（跨平台）
python skills/tooling/verification/verify_skill_pipeline.py

# 同时运行 bootstrap（可选）
python skills/tooling/verification/verify_skill_pipeline.py --bootstrap
```

## HOOK 验证

Hook 注册在 `.claude/settings.json`，每次 Claude Code 会话自动运行。可通过以下方式验证数据是否正常写入：

```powershell
# 检查 usage 数据
cat .ai/skills/lifecycle/telemetry/usage.jsonl

# 检查工具调用结果
cat .ai/skills/lifecycle/telemetry/tool_outcomes.jsonl

# 仅在有真实会话数据时显示内容；新部署时为空属正常状态
```

所有 4 个 hook 经过端到端测试确认：空 stdin、损坏 JSON、非 skill 路径等边缘情况均安全退出。

## 红旗

1. 不要跳过 `evolve-review` 就 `evolve-promote`
2. `auto-cleanup` 始终先 --dry-run 审查
3. CAPTURED skill 在 promote 前必须人工审查内容
4. 不要在 `library/skills/` 中保留 draft 状态 manifest
5. 进化提案不会自动 deploy——必须经过 benchmark → review → promote 管线
