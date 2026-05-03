# Skill System Architecture

## 目录结构

```
skills/
  INDEX.md                         # 顶层导航（本文件）
  library/skills/                  # 唯一可信源（20 个技能）
  discovery/                       # 自发现索引（自动生成）
    domain-catalog.json            # 领域定义
    skill-index.md                 # 所有技能索引
    registries/                    # 按领域分组的 registry
  runtime/                         # 跨 Agent 分发
    bootstrap-skills.ps1           # 初始化：生成索引 + 创建 junction
    sync-skills.ps1                # 增量同步（junction 失效时降级为复制）
  lifecycle/                       # 自学习
    telemetry/usage.jsonl          # 使用率数据
    learning/signals/              # 会话质量信号
    learning/candidates/           # 技能改进候选
    incubator/                     # 孵化中技能草稿
    governance/                    # 晋升/退役审计日志
  tooling/                         # 工具链
    catalog/generate_skill_catalog.py
    learning/skill_learn.py
  hooks/                           # 运行时钩子
    hook_track_usage.py            # PostToolUse: 记录 SKILL.md 读取
    hook_evaluate_session.py       # Stop: 评估会话质量
```

## 核心原则

1. **唯一可信源**: `skills/library/skills/` 是唯一编辑点，`.claude/skills/` 和 `.codex/skills/` 由 junction 自动同步
2. **自动发现**: 每个技能必须有 `skill.manifest.json`，目录由脚本自动生成
3. **自学习闭环**: 埋点 → 信号 → 聚类 → 晋升
4. **Git 提交**: 所有修改都在 `skills/` 下完成，无需分别提交到 `.claude/skills/` 和 `.codex/skills/`
