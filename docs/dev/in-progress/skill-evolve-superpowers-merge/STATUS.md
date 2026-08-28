# STATUS.md — skill-evolve-superpowers-merge

> 本任务是"superpowers 精华最小移植 + 三路径域路由集成"（方案1+2）的进化任务。

| 字段 | 值 |
|------|-----|
| task_id | skill-evolve-superpowers-merge |
| task_type | skill-evolution (manual transplant) |
| phase | executing |
| roadmap_or_plan | `plan-v1-01.md` |
| lifecycle_status | in-progress |
| blocking_questions | [] |
| question_clearance | cleared |
| clearance_confirmed_by_user | true |
| clearance_source | direct user confirmation (方案1+2 获准) |

## 背景

项目技能库与 superpowers 同源。用户选定方案1+2：最小移植 superpowers 分支上长出、而项目分支缺失的 6 项净增量精华（A三路径/B Interfaces/C ledger/D Rulings/E 修复上限+模型分级/F Batch），并把三路径映射进 expert 域路由。

## 进度表

| 阶段 | 内容 | 状态 |
|------|------|------|
| 0 | 任务目录 + 基线 | ✅ |
| 1 | dev-brainstorm 三路径+棘轮 | ✅ |
| 2 | dev-writing-plans Interfaces 块 | ✅ |
| 3 | dev-executing-plans ledger+Rulings | ✅ |
| 4 | dev-subagent E/F (修复上限+模型+Batch) | ✅ |
| 5 | dev-il2cpp + expert-registry 三路径路由 | ✅ |
| V | verify_skill_pipeline.py + health_engine.py | ✅ |

## 权威输入（不随本任务重复）

- 完整计划：`plan-v1-01.md`
- 上游来源：`C:\Users\haochuan.wang\.claude\plans\composed-zooming-squid.md`（已批准方案，含全部 6 项增补精确文本）

## 验证结论

- `verify_skill_pipeline.py`：All checks passed ✓
- `health_engine.py check`：正常完成（tracked=0 为无会话数据正常态）✓
- `expert-registry.json`：JSON parse 合法 ✓
- `git diff`：纯追加 114 行，无删改，排版干净 ✓

## 下一步

单次整理性提交，仅含本次 7 个技能文件 + 本任务目录；不卷入并行线改动（gc 原生代码等）。等待用户确认后提交。

## 边界声明

- 只改 `.ai/skills/library/skills/{dev-brainstorm,dev-writing-plans,dev-executing-plans,dev-subagent-driven-development,dev-il2cpp}/SKILL.md` + `.ai/skills/discovery/expert-registry.json` + `dev-il2cpp-core-agent/SKILL.md`。
- 不碰工作区里其他并行线改动（gc 原生代码、parse_convert、runtime_core 等）。
- 不改任何运行时代码，不跑 foundation-dll 管线。
