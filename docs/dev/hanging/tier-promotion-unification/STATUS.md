---
task_id: tier-promotion-unification
task_type: plan
phase: planned
lifecycle: in-progress
created: 2026-06-13
clearance_source: self-contained-design
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: false
---

# Tier 升级路径统一

## 状态
- **design**: ✅ completed — `design-v1-01.md`
- **plan**: ✅ completed — `plan-v1-01.md`
- **implementation**: 🔲 未开始

## 范围
消除 `TryTierUpgrade` (entry_direct) 和 `TryFastOsrPromotion` (fast_dispatch) 之间的 T1→T2/T2→T3 promotion 逻辑重复，引入共享 `EvaluateTierPromotion` 决策函数。

## 活着吗？
**是** — 设计完整、计划可用、不依赖外部任务。但需用户确认是否推进。
