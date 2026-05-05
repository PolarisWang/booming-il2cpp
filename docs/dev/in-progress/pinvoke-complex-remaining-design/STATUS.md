# P/Invoke Complex Remaining — Architecture Design

**phase:** roadmap
**task_type:** roadmap
**created:** 2026-05-05
**roadmap_or_plan:** roadmap-v1-01.md

## 设计确认

- 方案选择: **C — 混合分层 (Hybrid Layered)**
- 三优先级权衡: P1(性能) = AOT 内联零开销路径覆盖 95%+; P2(架构) = 三层分层清晰; P3(热更) = descriptor 路径支持动态注册
- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true
