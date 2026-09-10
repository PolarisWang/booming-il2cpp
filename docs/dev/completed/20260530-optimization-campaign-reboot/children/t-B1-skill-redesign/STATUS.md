---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-B1
source_relation: roadmap-child
clearance_source: parent-roadmap
lifecycle_status: completed
phase: completed
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-B1: 三阶段 Skill 架构设计

## Scope

重写 `.claude/skills/dev-optimization-campaign/SKILL.md`，从当前线性 14 步改为 Phase 0→1→2 三阶段结构。

## 设计要求

### Phase 0: 数据驱动选择
- Step 0.1: 全量 Benchmark 扫描 → `python -m verification.analysis.benchmark_scanner`
- Step 0.2: 优先级队列生成
- Step 0.3: 候选 bottleneck 分析
- 输出: optimization-priority.json, bottleneck-report.json
- 出口条件: 选定优化目标 + ROI 预估通过

### Phase 1: 预备阶段（不 claim，不 commit）
- Step 1.1: Subject 质量审计 + 自动修复 broken tests
- Step 1.2: 结构性审计（contract sync）
- Step 1.3: Freeze + Baseline 捕获（修复后冻结）
- 出口条件: 审计 PASS + freeze 完成

### Phase 2: 优化阶段（claim + commit）
- Step 2.1: Setup Worktree + Claim
- Step 2.2: Pre-flight Check
- Step 2.3: Pipeline Baseline
- Step 2.4: 瓶颈验证（profile scope）
- Step 2.5: 优化循环（三明治模型：假设→实现→验证→回滚）
- Step 2.6: 回归验证（semantic + benchmark）
- Step 2.7: .NET 8 对比
- Step 2.8: HotUpdate 检查
- Step 2.9: 文档 + Commit + Merge + CI
- 出口条件: pipeline passed + regression check passed + perf improved

## 输入

- 当前 `SKILL.md`（位于 `.claude/skills/dev-optimization-campaign/SKILL.md`）
- brainstorm 设计摘要（来自当前任务 STATUS.md 的 design summary）
- roadmap-v1-01.md 的阶段定义

## 预期输出

- `.claude/skills/dev-optimization-campaign/SKILL.md`（重写）
- 保留旧 skill 的兼容注释

## 退出标准

- SKILL.md 包含完整的 Phase 0→1→2 结构
- 每个 Phase 有清晰的 entry criteria 和 exit criteria
- 三明治模型修复循环描述完整
- 经自审无矛盾或模糊章节

## 终端备注

### 执行摘要

完成 `.claude/skills/dev-optimization-campaign/SKILL.md` 重写，从原来的线性 14 步改为 Phase 0→1→2 三阶段结构。

**核心变更：**

1. **Phase 0 新增（数据驱动选择）**：Step 0.1 全量 Benchmark 扫描 → Step 0.2 优先级队列生成（按 `slowdown × method_count × confidence` 排序）→ Step 0.3 瓶颈分析 + ROI 预估。出口条件为选定优化目标且预期收益 > 20%。

2. **Phase 1 重构（预备阶段）**：调整冻结时序——先修测试再冻结。Step 1.1 Subject 质量审计 + 自动修复 → Step 1.2 结构性审计（contract sync）→ Step 1.3 修复后再 Freeze + Baseline。不 claim、不 commit、不需要 worktree。

3. **Phase 2 重构（优化阶段）**：Step 2.1 Setup Worktree + Claim → Step 2.2 Pre-flight → Step 2.3 Pipeline（首次 Baseline）→ Step 2.4 瓶颈验证 → Step 2.5 **三明治优化循环**（假设→实现→验证→自动回滚）→ Step 2.6 回归验证 → Step 2.7 .NET 8 对比 → Step 2.8 HotUpdate 检查 → Step 2.9 文档+Commit+Merge+CI+Pull。

**关键设计决策：**
- 冻结时序修复：与旧版 SKILL.md 的核心区别，先修 subject 测试代码再执行 freeze
- 三明治模型：提出量化假设（"改变 X 使 Y 提升 Z%"）→ 实现 → 验证，Z' < Z/2 自动回滚
- attempt 配额：max_attempts=3，Infrastructure 失败不消耗配额
- 时间预算从 ~2.5 小时优化至 ~1.9 小时
