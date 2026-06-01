---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-B2
lifecycle_status: completed
phase: completed
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-B2: Freeze 时序修复

## Scope
修改 `subject_correctness_audit.py` 和 `pre_verification_audit.py`，将 test-fix 移到 freeze 之前。

## 终端备注

### 执行摘要

**已确认：** `subject_correctness_audit.py` 已正确实现 ISSUES_FOUND → exit code 1 的阻塞逻辑，`--force` 参数可跳过。

**改动：** 在 `pre_verification_audit.py` 中：
1. 新增 `--force-freeze` 参数（跳过审计检查强制冻结）
2. `freeze_subjects()` 在冻结前先调用 `run_full_correctness_audit()` 检查
3. 审计发现问题时拒绝冻结，输出提示信息
4. 验证：reflection-member-complete（已修复）冻结正常通过
