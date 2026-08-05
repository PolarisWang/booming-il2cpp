---
name: dev-skill-evolution
description: Use when running skill health checks, reviewing evolution proposals, promoting evolved skills, or managing the skill self-evolution pipeline.
---

# Skill 自进化管线

## 概述

技能自进化系统通过 telemetry → health → evolution → benchmark → review → promote 五层管线，自动优化技能质量。本技能封装了进化全流程的命令，是进化系统的操作入口。

## 使用时机

### 必须使用

- 需要查看当前技能健康状态时（运行 `check`）
- 有进化提案待审查或晋升时（`evolve.py propose` 输出中有提案）
- 需要晋升一个已审查通过的进化提案时（`evolve-promote`）
- 需要清理长期未使用的技能时（`auto-cleanup`）

### 不使用的情况

- 只是修改技能内容而非运行进化流程 → 用 `dev-writing-skills`
- 只是完成前的验证检查 → 用 `dev-verification-before-completion`
- 子 Agent 执行已明确的子任务（跳过本技能）

## 工作流

```
1. 健康检查 → python health_engine.py check --window 30
   ├── 计算指标 + 生成仪表盘 + 预览进化提案
   └── 如无异常 → 结束

2. 如有 FIX/DERIVED/CAPTURE 候选：
   python evolve.py auto-evolve → 生成 proposal 目录

3. 准入检查：
   python skill_learn.py evolve-benchmark <proposal-id>

4. 审查：
   python skill_learn.py evolve-review <proposal-id>

5. 晋升（benchmark + review 双通过后）：
   python skill_learn.py evolve-promote <proposal-id>

6. 验证：
   python verify_skill_pipeline.py
```

## 快速参考

| 命令 | 用途 |
|------|------|
| `python .ai/skills/tooling/learning/health_engine.py check --window 30` | 一键自检（推荐日常使用） |
| `python .ai/skills/tooling/learning/health_engine.py compute --all --window 30` | 仅计算健康快照 |
| `python .ai/skills/tooling/learning/health_engine.py report --window 30` | 仅生成健康报告 |
| `python .ai/skills/tooling/learning/evolve.py propose` | 预览进化提案 |
| `python .ai/skills/tooling/learning/evolve.py auto-evolve` | 执行进化（生成提案） |
| `python .ai/skills/tooling/learning/skill_learn.py evolve-benchmark <id>` | 提案准入检查 |
| `python .ai/skills/tooling/learning/skill_learn.py evolve-review <id>` | 提案审查 |
| `python .ai/skills/tooling/learning/skill_learn.py evolve-promote <id>` | 晋升提案到正式库 |
| `python .ai/skills/tooling/learning/skill_learn.py evolve-history --skill <name>` | 查看版本谱系 |
| `python .ai/skills/tooling/learning/skill_learn.py auto-cleanup --dry-run` | 预览自动退役 |
| `python .ai/skills/tooling/verification/verify_skill_pipeline.py` | 管线一致性验证 |

## 进化类型

| 类型 | 触发条件 | 动作 | 版本变化 |
|------|---------|------|---------|
| **FIX** | completion_rate < 0.6, sessions >= 3 | 原地修复 skill 内容 | 1.0.0 → 1.0.1 |
| **DERIVED** | context_switching_ratio > 0.5, sessions >= 10 | 派生新 skill | 新 skill 从 1.0.0 开始 |
| **CAPTURED** | quality >= 0.8, action_hint = new-skill | 从会话信号提取新 skill | 0.1.0 → 1.0.0 (promote) |

## 常见错误

- **跳过 evolve-review 直接 promote**: benchmark 通过后必须先 review。跳过会导致晋升失败
- **promote 后没有验证**: promote 自动运行 `verify_skill_pipeline.py`（PS1 → Python 降级），如果输出警告应先处理
- **auto-cleanup 默认 dry-run**: 默认不实际执行，需要 `--no-dry-run` 才真正退役
- **CAPTURED skill 内容不完整**: 自动生成的 skeleton 只包含基本结构，promote 前需要人工审查

## 红旗

1. 不要跳过 `evolve-review` 就 `evolve-promote`
2. `auto-cleanup` 始终先 `--dry-run` 审查
3. CAPTURED skill 在 promote 前必须人工审查内容
4. 不要在 `library/skills/` 中保留 draft 状态 manifest
5. 进化提案不会自动 deploy——必须经过 benchmark → review → promote 管线
6. 如果健康快照不存在（首次部署或刚清理后），静默跳过即可

## 关联技能

- 上游：无（由 `dev-il2cpp` 或用户直接触发）
- 协作：`dev-writing-skills`（创建/修改 skill 内容）
- 下游：`dev-verification-before-completion`（promote 后验证）
