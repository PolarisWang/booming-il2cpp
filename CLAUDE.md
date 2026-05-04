# Chaos IL2CPP 开发规则

## 全局优先级约束（强制）

以下三条优先级在所有开发阶段必须遵守，从 brainstorm 到验证验收覆盖全流程：

1. **性能最优（第一优先级）** — 一切以 il2cpp 生成的 C++ 代码运行性能为第一考量。方案选择、翻译路径决策、代码生成策略，当其他因素与性能冲突时，性能优先。
2. **方案完美性（第二优先级）** — il2cpp 最终方案的架构完整性、正确性、可维护性为第二考量。在性能不受损的前提下，追求架构的干净、一致、可维护。
3. **HotUpdate 支持（第三优先级）** — 最大化支持热更新为第三考量。在性能和方案完整性都满足的前提下，优先选择对热更新友好的方案。

**裁决规则**：低优先级与高优先级冲突时，低优先级让位于高优先级。但高优先级方案不应故意破坏低优先级——在保证高优先级的前提下，尽量兼顾低优先级。

## 架构优先开发

在开发任何 il2cpp 新功能或修改现有翻译行为前，必须先执行 `dev:architecture-first-development` 技能完成架构查询，确认翻译路径与既有架构一致。禁止在未查表的情况下直接进入实现。

## 知识记录

- 新翻译路径必须记录到 `wiki/03-功能模块/il2cpp-核心架构/il2cpp-核心翻译路径参考.md`
- 设计讨论必须在 `docs/discuss/` 完成
- 每个功能的 STATUS.md 必须包含 `## 架构映射` 小节（记录格式见 `dev-architecture-first-development` 技能）

## 项目文档原则

- `wiki/` 记录长期知识，`docs/` 记录过程
- 仅当任务确实产生长期有效知识时才更新 wiki
- 历史决策只保留最终方案，不保留讨论过程

## 可用技能注册表

以下技能通过 `.claude/skills/` 注册到 Claude Code，全程可通过 Skill 工具按名称（`dev:<skill-name>`）调用，或在对话中输入 `/dev:<skill-name>` 触发。

### 核心开发流程
| 技能 | 说明 |
|------|------|
| `dev:brainstorm` | 新功能设计前澄清边界、拍板方案 |
| `dev:roadmap` | 复杂任务分多阶段/多子任务推进 |
| `dev:writing-plans` | 编写实现计划 |
| `dev:executing-plans` | 按计划执行任务 |
| `dev:subagent-driven-development` | 子 Agent 并行执行 |

### 开发前/中检查
| 技能 | 说明 |
|------|------|
| `dev:architecture-first-development` | il2cpp 翻译逻辑开发前必须先调用 |
| `dev:test-driven-development` | 测试优先开发 |
| `dev:systematic-debugging` | 复杂 bug 根因分析 |

### 验证与完成
| 技能 | 说明 |
|------|------|
| `dev:verification-before-completion` | 完成前运行验证 |
| `dev:finishing-a-development-branch` | 分支完成、提交和 PR |

### 领域特定
| 技能 | 说明 |
|------|------|
| `dev:foundation-dll-ai-test-generation` | foundation DLL 测试代码生成 |
| `dev:foundation-dll-family-verification` | foundation DLL 三维验证：Fact → Benchmark → HotUpdate |
| `dev:project-test-governance` | 测试治理 |

### 基础设施
| 技能 | 说明 |
|------|------|
| `dev:using-git-worktrees` | git worktree 隔离开发 |
| `dev:project-wiki-maintenance` | wiki 维护 |
| `dev:writing-skills` | 编写/修改技能 |
| `dev:dispatching-parallel-agents` | 并行 Agent 派发 |
| `dev:using-booming` | Booming 工具 |
| `dev:skill-evolution` | 技能自进化管线操作入口（健康检查、提案审查与晋升） |

### 技能自进化系统

技能自进化系统通过 telemetry → health → evolution → promote 管线自动优化技能质量。

**Telemetry 采集（HOOK 自动运行）：**
- `skills/hooks/hook_track_usage.py` — 记录 SKILL.md 读取
- `skills/hooks/hook_track_tool_outcomes.py` — 记录工具调用成败
- `skills/hooks/hook_evaluate_session.py` — 评估会话质量
- `skills/hooks/hook_track_session_outcomes.py` — 记录会话完成状态

**健康度量与进化触发（手动/定时运行）：**
- `python skills/tooling/learning/health_engine.py check --window 30` — **一键自检：** 计算健康指标 + 生成报告 + 预览进化提案（推荐日常使用）
- `python skills/tooling/learning/health_engine.py compute --all --window 30` — 计算 per-skill 健康指标（applied_rate、completion_rate、tool_success_rate 等）
- `python skills/tooling/learning/health_engine.py report --window 30` — 生成健康仪表盘
- `python skills/tooling/learning/evolve.py propose --dry-run` — 预览进化提案
- `python skills/tooling/learning/evolve.py auto-evolve` — 执行进化（FIX/DERIVED/CAPTURED）

**PIPELINE（提案审批）：**
- `python skills/tooling/learning/skill_learn.py evolve-benchmark <proposal-id>` — 准入检查
- `python skills/tooling/learning/skill_learn.py evolve-review <proposal-id>` — 审查
- `python skills/tooling/learning/skill_learn.py evolve-promote <proposal-id>` — 晋升到 library/skills/
- `python skills/tooling/learning/skill_learn.py evolve-history --skill <name>` — 查看版本谱系

**验证：**
- `python skills/tooling/verification/verify-skill-pipeline.ps1` — 完整管线一致性检查

技能定义见 `.codex/skills/<skill-name>/SKILL.md`。
