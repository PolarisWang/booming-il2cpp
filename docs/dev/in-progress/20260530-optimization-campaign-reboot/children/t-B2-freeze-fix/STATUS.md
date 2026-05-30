---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-B2
lifecycle_status: active
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-B2: Freeze 时序修复

## Scope
修改 `subject_correctness_audit.py`，将 test-fix 移到 freeze 之前。ISSUES_FOUND verdict 必须阻塞 pipeline，强制修好才能进入 freeze。

## 输入
- 当前 SKILL.md（Phase 1 定义）
- `testing/foundation-dll/verification/stages/subject_correctness_audit.py`

## 具体修改

1. 修改 `subject_correctness_audit.py` 的 `verdict` 逻辑：
   - 当前：ISSUES_FOUND 是 WARN，不阻塞
   - 改为：ISSUES_FOUND → exit code = 1（阻塞）
   - 添加 `--force` 参数可跳过阻塞（用于手动覆盖）

2. 修改 freeze 命令 `--freeze` 的行为：
   - 在计算 SHA256 之前先检查是否有 ISSUES_FOUND
   - 如有，输出错误并拒绝 freeze

3. 更新 audit 输出 JSON 的 verdict 字段：
   - 增加 `HAS_ISSUES_BLOCKING` 状态
   - 修改 exit code 映射

## 预期输出
- `verification/stages/subject_correctness_audit.py`（修改）
- 验证：对 reflection-member-complete 运行 audit，确认 ISSUES_FOUND 返回非零

## 退出标准
- ISSUES_FOUND 时 audit 返回 exit code != 0
- ISSUES_FOUND 时 freeze 拒绝执行
- PASS 时 freeze 正常执行
- `--force` 参数可跳过阻塞

## 终端备注
