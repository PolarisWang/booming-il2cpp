---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-B3
lifecycle_status: active
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-B3: ROI 预估门禁

## Scope
在 `pre_verification_audit.py` 中新增 ROI 预估门禁。进入 Phase 2 之前自动输出 `optimization-opportunity.md`，包含瓶颈分类和预估收益。

## 输入
- `testing/foundation-dll/verification/stages/pre_verification_audit.py`
- benchmark_scanner.py 的输出格式

## 具体修改

1. 在 `pre_verification_audit.py` 中新增 `--estimate-roi` 参数：
   - 输入：family slug + assembly
   - 分析 benchmark 数据，计算当前 ns/op
   - 根据瓶颈类型估算预期收益

2. 瓶颈分类逻辑：
   - dispatch-bound: 如果 chaos-aot 主要走 interpreter dispatch → 预期收益高
   - alloc-bound: 如果大量 GC alloc → 预期收益中
   - metadata-bound: 如果大量 metadata lookup → 预期收益低（架构限制）
   - 判断方法：检查 chaos-aot vs chaos-jit 差距 + 方法调用模式

3. 输出 `optimization-opportunity.md`：
```markdown
# 优化机会: <slug>

## 当前性能
- chaos-aot: Xns
- vs .NET 8: Yx slowdown
- 方法数: N

## 瓶颈分析
- 类型: [dispatch-bound | alloc-bound | metadata-bound]
- 分析依据: ...

## 预期收益
- 乐观: Z1 ns (Z1% improvement)
- 保守: Z2 ns (Z2% improvement)

## 建议方案
- 方案 1: ... (预估工时)
- 方案 2: ... (预估工时)
```

4. 门禁逻辑：
   - 预期收益 < 20% → 输出 WARN 但不阻塞
   - 预期收益 >= 20% → 输出 PASS，进入 Phase 2

## 预期输出
- `pre_verification_audit.py`（修改，新增 ROI 功能）
- 输出 `optimization-opportunity.md`

## 退出标准
- `--estimate-roi enum-parsing` 能正确输出瓶颈类型和预期收益
- 预期收益 < 20% 的 family 正确标记

## 终端备注
