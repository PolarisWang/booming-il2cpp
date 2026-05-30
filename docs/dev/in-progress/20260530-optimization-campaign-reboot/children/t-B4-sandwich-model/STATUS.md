---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-B4
lifecycle_status: active
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-B4: 修复三明治模型

## Scope
实现"三明治模型"修复循环：每次优化尝试必须提出量化假设 → 实现修改 → benchmark 验证 → 不达预期自动回滚。

## 输入
- 当前 SKILL.md 的 Phase 2.5 描述
- `testing/foundation-dll/verification/analysis/perf_regression_check.py`（作为参考）

## 具体实现

1. 在 `verification/stages/` 下新建 `sandwich_fix_loop.py`（或扩充到现有文件）：
   - `record_hypothesis(slug, description, expected_improvement_pct)` — 记录优化假设
   - `verify_improvement(slug, baseline_report, current_report, min_threshold_pct)` — 验证 Z' >= Z/2
   - `rollback_if_needed(slug, hypothesis_id)` — 自动回滚（git revert）
   - `save_fix_attempt(hypothesis_id, result)` — 记录尝试结果到 fix-attempt-history.jsonl

2. 假设记录格式：
```json
{
  "hypothesis_id": "enum-001",
  "slug": "enum-parsing",
  "description": "预计算 Enum name→value 映射表为 AOT 数据段",
  "expected_improvement_pct": 80,
  "changed_files": ["src/managed/Chaos.IL2CPP.Generator/EnumEmitter.cs"],
  "committed_at": "2026-05-30T...",
  "baseline_ns": 602.51
}
```

3. 验证逻辑：
   - 读取 baseline multi-run-report.json
   - 读取 current multi-run-report.json
   - 计算每个方法的 improvement_pct
   - 如果 `geometric_mean_improvement < min_threshold`（默认 = expected/2）
   - → 执行 `git revert HEAD` 自动回滚
   - → 记录失败原因

## 预期输出
- `verification/stages/sandwich_fix_loop.py`（新文件）
- SKILL.md 中的 Phase 2.5 描述已引用此模块

## 退出标准
- `record_hypothesis` 可正确保存假设
- `verify_improvement` 可对比两个报告
- `rollback_if_needed` 在 improvement 不达标时回滚
- 测试通过（手动模拟一个失败场景）

## 终端备注
