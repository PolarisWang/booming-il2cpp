# Skill 自进化闭环

本文定义 `skills/` 在方案 A 下的 sidecar 学习闭环：正式加载链保持稳定，学习、草拟、验证、观察和退役全部进入 `lifecycle/`。

## 正式加载链

正式加载链不变，只是目录按职责重组：

`library/skills/ -> discovery/skill-index.md -> discovery/registries/<domain>.md -> library/skills/<name>/SKILL.md`

## 生命周期目录职责

| 路径 | 职责 |
|------|------|
| `lifecycle/learning/signals/` | 原始学习信号，按月写入 jsonl |
| `lifecycle/learning/candidates/` | 聚类后的 candidate、证据与 review 结果 |
| `lifecycle/incubator/` | 草稿 skill；包含 candidate-id 草稿与 manual-* 手工 scaffold |
| `lifecycle/telemetry/` | skill 健康快照与使用率数据 |
| `lifecycle/governance/` | promote / retire 日志与备份 |

## 阶段一：收集与起草

```powershell
python skills/tooling/learning/skill_learn.py collect "信号描述" --domain-hint <domain> --action-hint <action>
python skills/tooling/learning/skill_learn.py cluster
python skills/tooling/learning/skill_learn.py draft <candidate-id>
powershell -File skills/tooling/scaffolding/create-skill-scaffold.ps1 -DirectoryName <Name> -Domain <Domain>
```

约束：

1. `collect` 只记录事实信号，不直接修改正式 skill。
2. `cluster` 负责形成 candidate 与 evidence。
3. `draft` 一律写到 `lifecycle/incubator/<candidate-id>/`。
4. scaffold 生成的是 `lifecycle/incubator/manual-<DirectoryName>/`。

## 阶段二：准入与合入

```powershell
python skills/tooling/learning/skill_learn.py benchmark <candidate-id>
python skills/tooling/learning/skill_learn.py review <candidate-id>
python skills/tooling/learning/skill_learn.py promote <candidate-id> --dry-run
python skills/tooling/learning/skill_learn.py promote <candidate-id>
python skills/tooling/learning/skill_learn.py promote-manual manual-<DirectoryName> --dry-run
python skills/tooling/learning/skill_learn.py promote-manual manual-<DirectoryName>
```

准入门：

1. `benchmark` 检查 domain、summary、description、trigger_signals、TODO 残留和重叠风险。
2. `review` 把 benchmark 问题整理成 review 结果。
3. `promote` 默认要求 benchmark 通过且 review 批准；合入后会刷新 catalog 并跑 verify-skill-pipeline.ps1。
4. `promote-manual` 不依赖 candidate review，但仍要求清理占位文本。

## 阶段三：观察与退役

```powershell
python skills/tooling/learning/skill_learn.py observe
python skills/tooling/learning/skill_learn.py retire <skill-name> --dry-run
python skills/tooling/learning/skill_learn.py retire <skill-name> --reason "merged into another skill"
```

治理动作：

1. `observe` 生成正式 skill 数量、domain 分布、budget 分布、candidate 状态和 stale candidate 快照。
2. `retire` 只把正式 skill 的 manifest 状态改为 `deprecated`，并写入治理日志与备份。
3. retire 不等于物理删除；先弃用，再决定是否合并或清理。

## 阶段四：自动学习闭环

在手动闭环之上增加自动层，实现信号采集 → 处理 → 评估 → 报告 → 清理的全自动管线。

### 架构概览

```
PostToolUse hook (Read 追踪)           Stop hook (会话评估)
        |                                      |
        v                                      v
lifecycle/telemetry/usage.jsonl     lifecycle/learning/signals/
        |                                      |
        +----------+---------------------------+
                   |
                   v
         auto-collect (usage.jsonl -> signals)
                   |
                   v
         auto-assess (multifactor analysis)
                   |
                   v
         auto-report (weekly report)
                   |
                   v
         auto-cleanup (retire stale skills)
```

### 自动闭环工作流

#### Hook 层（自动运行，无需干预）

- **PostToolUse hook**：`skills/hooks/hook_track_usage.py` 记录 Read 操作到 `lifecycle/telemetry/usage.jsonl`
- **Stop hook**：`skills/hooks/hook_evaluate_session.py` 评估会话质量，保留 quality >= 0.7 的信号到 `lifecycle/learning/signals/`

#### 自动化处理层（建议每周运行）

| 命令 | 功能 | 建议频率 |
|------|------|---------|
| `auto-collect` | 读取 usage.jsonl -> 生成 learning signals | 每周 |
| `auto-assess` | 多因子分析：usage heatmap、candidate 管线健康 | 每周 |
| `auto-report` | 生成综合报告（JSON + Markdown） | 每周 |
| `auto-cleanup --dry-run` | 预览哪些 skill 可退役 | 每月 |
| `auto-cleanup --no-dry-run` | 执行自动退役 | 每月 |

### 安全机制

- Stop hook 只产生 quality >= 0.7 的信号
- auto-collect 默认 `--min-uses 3`
- auto-cleanup 默认 `--dry-run`，需显式 `--no-dry-run`
- 所有退役操作写入 governance log + 备份
- 自动退役只影响 manifest status，不物理删除文件

## 红旗

1. 不要让 `lifecycle/incubator/` 直接参与 catalog 生成。
2. 不要跳过 `review` 就 `promote`。
3. 不要把一次性任务伪装成长期 skill。
4. 不要把 learning 闭环产物直接塞进发现层。
5. 不要在未审查 `auto-cleanup --dry-run` 结果的情况下直接用 `--no-dry-run`。
