---
task_id: foundation-dll-industrialization
task_name: foundation-dll 工业化级验证
task_type: roadmap
phase: execution
lifecycle_status: in-progress
roadmap_or_plan: docs/dev/in-progress/foundation-dll-industrialization/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Foundation DLL 工业化级验证

## 最近摘要

Phase 0（基础设施搭建）已完成。Brotli/decoder 13/13 ALL PASSED（100%, 101.5s）。

Phase 1 Batch 1（7 families: array-indexing-copy, boxing-unboxing-casts, buffer-memory, ccw-basic, codegen-edge-cases, collections-generic-core, convert-char）已完成。6/7 全通过，buffer-memory 有审计 pipeline 误报 + 微基准已知问题。

## 当前状态

| Phase | Status |
|-------|--------|
| Phase 0 基础设施 | ✅ 完成 |
| Phase 1 CoreLib 攻坚 | 🔄 当前 — Batch 1 ✅，Batch 2 待启动 |
| Phase 2 小 assembly 热身 | ⏳ 待开始 |
| Phase 3 中等 assembly | ⏳ 待开始 |
| Phase 4 大 assembly 收尾 | ⏳ 待开始 |

### 子任务进度

| task_id | phase | status |
|---------|-------|--------|
| fd-p0-infra | 0 | ✅ completed |
| fd-p1-corelib-b1 | 1 | ✅ completed |
| fd-p1-corelib-b2 | 1 | ⏳ planned |
| fd-p1-corelib-b3 ~ fd-p4-asn1 | 1-4 | ⏳ planned |

## 下一步

启动 fd-p1-corelib-b2（CoreLib batch 2: 7 families）。

## 推荐下一子任务

fd-p1-corelib-b2

## 设计摘要

### 边界拍板

- 覆盖 14 个 assembly，所有具备 contract.json 的 family
- 推进粒度：family 级
- CoreLib 策略：优先攻坚
- Benchmark 基线：首次运行以 JIT 结果为 baseline
- HotUpdate 范围：全覆盖

### Phase 0 验证结论

- JIT codegen: ✅ 端到端工作
- HotUpdate: ✅ 全部 4 个 flag 通过
- AsmCompare: ✅ JIT vs AOT 对比正常
- Microbench: ✅ 正常
- Benchmark: ✅ 3-way 对比正常

### 关键文档

- [roadmap-v1-01.md](roadmap-v1-01.md)

### Authority 决策

- 推进粒度：family 级
- 调度方式：串行
- 终端容量：1
- 验收标准：每个 family 的 13-stage 全部通过（passed/skipped 合法）
