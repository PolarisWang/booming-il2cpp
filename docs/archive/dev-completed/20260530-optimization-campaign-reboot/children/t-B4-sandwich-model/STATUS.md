---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-B4
lifecycle_status: completed
phase: completed
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

1. 在 `verification/stages/` 下新建 `sandwich_fix_loop.py`：
   - `record_hypothesis(slug, description, expected_improvement_pct, changed_files, baseline_ns)` — 记录优化假设
   - `verify_improvement(slug, hypothesis_id, min_threshold_pct)` — 验证 improvement >= threshold
   - `rollback_if_needed(slug, hypothesis_id)` — 自动回滚（git revert HEAD）
   - CLI 入口支持三个子命令

2. 假设记录到 `{family_dir}/fix-attempt-history.jsonl`，三条记录格式：
   - 初始假设记录 (recorded_at)
   - 验证结果记录 (verified_at)
   - 回滚记录 (rolled_back_at)

3. 验证逻辑：
   - 从 hypothesis 记录读取 baseline_ns
   - 从当前 multi-run-report.json 读取 per-method chaos-aot ns/op
   - 计算 geometric mean improvement %
   - 阈值默认 = expected_improvement_pct / 2
   - git revert 前的安全检查：工作树脏检查、HEAD subject 匹配警告

## 预期输出
- `verification/stages/sandwich_fix_loop.py`（新文件）

## 退出标准
- [x] `record_hypothesis` 可正确保存假设（自动编号、baseline_ns 自动提取）
- [x] `verify_improvement` 可对比 baseline 和 current multi-run-report
- [x] `rollback_if_needed` 在 improvement 不达标时执行 git revert
- [x] 测试通过（手动模拟 record/verify/rollback 三个命令）

## 终端备注

### 根因三段式

- `root_cause` — 三明治模型的 verify/rollback 循环缺乏代码实现，优化尝试无自动回滚机制
- `fix_strategy` — 新增 `sandwich_fix_loop.py` 模块，实现 record_hypothesis / verify_improvement / rollback_if_needed 三函数 + CLI 入口，历史记录到 fix-attempt-history.jsonl
- `regression_check` — 手动测试三条 CLI 命令确认可用；已与 perf_regression_check.py 联通的 benchmark-history.jsonl / multi-run-report.json 路径一致

### 文件创建
- `testing/foundation-dll/verification/stages/sandwich_fix_loop.py` — 三明治模型主模块
