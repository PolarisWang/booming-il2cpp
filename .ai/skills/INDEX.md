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
    bootstrap-skills.ps1           # 初始化：生成索引 + 验证入口桩
    sync-skills.ps1                # 验证入口桩 + library 一致性
  lifecycle/                       # 自学习 + 自进化
    telemetry/usage.jsonl          # 使用率数据
    telemetry/tool_outcomes.jsonl  # 工具调用成败
    telemetry/session_outcomes.jsonl # 会话完成状态
    telemetry/health/              # 健康指标快照
    learning/signals/              # 会话质量信号
    learning/candidates/           # 技能改进候选
    incubator/                     # 孵化中技能草稿
    evolution/proposals/           # 进化提案 (FIX/DERIVED/CAPTURED)
    evolution/lineage/             # 版本谱系记录
    governance/                    # 晋升/退役审计日志
  tooling/                         # 工具链
    catalog/generate_skill_catalog.py
    learning/skill_learn.py
    learning/health_engine.py      # 健康度量引擎（含 check 一键自检）
    learning/evolve.py             # 进化触发引擎
    verification/verify-skill-pipeline.ps1   # PowerShell 管线验证
    verification/verify_skill_pipeline.py    # Python 管线验证（跨平台备选）
  hooks/                           # 运行时钩子
    hook_track_usage.py            # PostToolUse: 记录 SKILL.md 读取
    hook_track_tool_outcomes.py    # PostToolUse: 记录工具调用成败
    hook_evaluate_session.py       # Stop: 评估会话质量
    hook_track_session_outcomes.py # Stop: 记录会话完成状态
```

## 核心原则

1. **唯一可信源**: `skills/library/skills/` 是唯一编辑点，`.claude/skills/dev-il2cpp/SKILL.md` 包含最小路由桩指向 library。
2. **自动发现**: 每个技能必须有 `skill.manifest.json`，目录由脚本自动生成
3. **自学习闭环**: 埋点 → 信号 → 聚类 → 晋升 → 进化 → 版本谱系
4. **Git 提交**: 所有修改都在 `skills/` 下完成，无需分别提交到 `.claude/skills/` 和 `.codex/skills/`

## 详细文档

- [`docs/skill-evolution-loop.md`](docs/skill-evolution-loop.md) — 自进化闭环五阶段详解（telemetry → health → evolution → pipeline → observe）
- [`docs/skill-evolution-architecture.md`](docs/skill-evolution-architecture.md) — 进化系统架构总览
