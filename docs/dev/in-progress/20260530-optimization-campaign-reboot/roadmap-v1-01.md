# 优化战役重启 — 全数据驱动优化流水线 Roadmap

## 1. 目标

将当前的优化战役（dev-optimization-campaign）从"手选 family → 线性 14 步"升级为"全数据驱动 → 三阶段流水线 → 自动回归防护"，并优先解决 enum-parsing 的 10x+ 性能问题。

## 2. 范围边界

- 覆盖所有 `testing/foundation-dll/System.Private.CoreLib/` 下的 family
- 只修改 `verification/analysis/`、`verification/stages/`、`SKILL.md`、codegen/runtime 代码
- 不包括其他 assembly（如 mscorlib、System.Core 等）的 benchmark

## 3. 非目标

- 不重构整个 verification 框架
- 不添加新的 IL2CPP 功能
- 不修改现有的 family contract 格式
- 不改动 managed test framework

## 4. 阶段列表

### Phase 0: 数据基建（Week 1）

建立全量 benchmark 数据库，输出优化优先级队列。

阶段方案优先级评估：
- P1（性能最优）：scanner 只读不写，零开销
- P2（架构完美）：统一输出格式，支持 JSON + 表格
- P3（热更适配）：不影响

### Phase 1: 技能重构（Week 1-2）

将 skill 拆为 Phase 0→1→2 三阶段，修复冻结时序，增加 ROI 门禁和三明治模型。

阶段方案优先级评估：
- P1（性能最优）：skill 本身不产生运行时开销
- P2（架构完美）：三阶段职责清晰，出口条件明确
- P3（热更适配）：优化后的 skill 能更好保护 hotupdate 验证

### Phase 2: P0 优化 enum-parsing（Week 2-3）

目标：chaos-aot 从 602ns → < 60ns（10x improvement）

阶段方案优先级评估：
- P1（性能最优）：核心目标
- P2（架构完美）：优化方案需保持架构一致性
- P3（热更适配）：Enum 预计算表需支持 hotupdate 更新

### Phase 3: 批量优化基建（Week 3-4）

增量 benchmark、自动 regression detection、family 依赖图。

阶段方案优先级评估：
- P1（性能最优）：增量 benchmark 节省 80%+ 跑量时间
- P2（架构完美）：依赖图指导批量优化
- P3（热更适配）：regression detection 覆盖 hotupdate benchmark

### Phase 4: 长期基建（Week 4-6）

跨 session history、auto-bisect、pre-commit hook、dashboard。

阶段方案优先级评估：
- P1（性能最优）：pre-commit hook 避免性能退化合入
- P2（架构完美）：完整数据链
- P3（热更适配）：追踪 hotupdate 性能趋势

## 5. 每阶段完成定义

### Phase 0: 数据基建

| 字段 | 内容 |
|------|------|
| goal | 所有 family 有 benchmark 数据，输出优先级队列 |
| exit_criteria | benchmark_scanner.py 可输出完整排行榜；44 个无数据 family 至少 50% 跑通 |
| deliverables | benchmark_scanner.py, optimization-priority.json, batch-pipeline-run.log |
| dependencies | 无 |
| resolved_decisions | 输出格式为 JSON + 表格；失败 family 标原因不阻塞 |
| watch_items | Batch pipeline 失败率 > 50% 需评估基础设施健康度 |

### Phase 1: 技能重构

| 字段 | 内容 |
|------|------|
| goal | 新 skill 文档 + freeze 修复 + ROI 门禁 + 三明治模型 |
| exit_criteria | 新 SKILL.md 通过自审；freeze 测试通过；ROI gate 可输出预估报告 |
| deliverables | SKILL.md (重写), subject_correctness_audit.py (修改), pre_verification_audit.py (扩充) |
| dependencies | 无（与 Phase 0 并行） |
| resolved_decisions | 旧 skill 保留兼容；新 skill 在 Phase 0 完成后切换 |
| watch_items | 旧 skill 有 worktree 正在使用时暂不切换 |

### Phase 2: P0 优化 enum-parsing

| 字段 | 内容 |
|------|------|
| goal | enum-parsing chaos-aot 从 602ns → < 60ns |
| exit_criteria | 运行 unified pipeline 确认：所有 enum 方法 timing > 0，vs .NET 8 ≤ 20%（或 blocker.md 说明架构限制） |
| deliverables | codegen/runtime 修改, enum-parsing 性能报告 |
| dependencies | Phase 0 完成（确认 enum-parsing 是 P0）；Phase 1 完成（ROI gate 通过） |
| resolved_decisions | 优化方向由瓶颈分析决定 |
| watch_items | 10x+ 目标可能受 AOT 架构限制 |

### Phase 3: 批量优化基建

| 字段 | 内容 |
|------|------|
| goal | 增量 benchmark + auto regression + 依赖图 |
| exit_criteria | 增量 benchmark 可工作；regression detection 可阻止 > 5% degradation |
| deliverables | cli.py 修改, regression_check.py 扩充, dependency-graph.json |
| dependencies | Phase 0 完成 |
| resolved_decisions | regression 阈值 5%；batch 范围按 family 依赖图 |
| watch_items | 增量 benchmark 缓存失效策略 |

### Phase 4: 长期基建

| 字段 | 内容 |
|------|------|
| goal | 跨 session 趋势、auto-bisect、pre-commit hook、dashboard |
| exit_criteria | dashboard 可展示历史趋势；bisect 可定位 regression commit |
| deliverables | benchmark-history 中心存储, bisect script, git hook, dashboard HTML |
| dependencies | Phase 3 完成 |
| resolved_decisions | 存储格式 JSONL；dashboard 静态 HTML |
| watch_items | git hook 可能影响 commit 体验 |

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|-----------|----------|----------------|-----------------|
| T-A1 | Phase 0 | ready | terminal-1 | benchmark_scanner.py 实现 | — | batch-1 | verification/analysis/benchmark_scanner.py | 2天 |
| T-B1 | Phase 1 | ready | terminal-2 | 三阶段 Skill 架构设计 | — | batch-1 | .claude/skills/dev-optimization-campaign/SKILL.md | 2天 |
| T-A2 | Phase 0 | blocked | — | Batch Pipeline 执行 | T-A1 | batch-2 | testing/foundation-dll/*/unified-verification-report.json | 3天 |
| T-A3 | Phase 0 | blocked | — | Priority Queue 生成 | T-A2 | batch-3 | verification/analysis/optimization-priority.json | 1天 |
| T-B2 | Phase 1 | blocked | — | Freeze 时序修复 | T-B1 | batch-2 | verification/stages/subject_correctness_audit.py | 1天 |
| T-B3 | Phase 1 | blocked | — | ROI 预估门禁 | T-B1 | batch-2 | verification/stages/pre_verification_audit.py | 2天 |
| T-B4 | Phase 1 | blocked | — | 修复三明治模型 | T-B1 | batch-2 | verification/stages/ | 1天 |
| T-C1 | Phase 2 | blocked | — | enum-parsing 瓶颈深潜 | T-A3, T-B3 | batch-3 | testing/foundation-dll/System.Private.CoreLib/enum-parsing/ | 2天 |
| T-C2 | Phase 2 | blocked | — | enum-parsing 优化 Attempt 1-3 | T-C1 | batch-4 | src/managed/Chaos.IL2CPP.Generator/, src/native/ | 5天 |
| T-C3 | Phase 2 | blocked | — | enum-parsing 回归验证 + Doc + Merge | T-C2 | batch-5 | testing/foundation-dll/System.Private.CoreLib/enum-parsing/, docs/optimize/ | 2天 |
| T-D1 | Phase 3 | blocked | — | 增量 Benchmark | T-A3 | batch-3 | verification/entry_points/cli.py | 2天 |
| T-D2 | Phase 3 | blocked | — | 自动 Regression Detection | T-D1 | batch-4 | verification/analysis/perf_regression_check.py | 2天 |
| T-D3 | Phase 3 | blocked | — | Family 依赖图分析 | T-A3 | batch-4 | verification/analysis/dependency-graph.json | 2天 |
| T-E1 | Phase 4 | blocked | — | 跨 Session Benchmark 历史 | T-D2 | batch-5 | verification/analysis/ | 2天 |
| T-E2 | Phase 4 | blocked | — | Auto-Bisect | T-E1 | batch-5 | verification/analysis/bisect.py | 2天 |
| T-FINAL | 验收 | blocked | — | 全量自测 + 验收 | 所有 | batch-6 | — | 3天 |

## 7. 依赖

- T-A2 ← T-A1: scanner 必须先实现，才知道跑哪些 family
- T-A3 ← T-A2: 所有 family 有数据后才能生成优先级
- T-B2/3/4 ← T-B1: 必须先设计三阶段架构，才能修细节
- T-C1 ← T-A3 + T-B3: 需要确认 enum-parsing 是 P0 + ROI gate 通过
- T-C2 ← T-C1: 必须先知道瓶颈在哪
- T-D1 ← T-A3: 需要知道数据格式
- T-D2 ← T-D1: 需要增量 benchmark 基础设施
- T-FINAL ← 所有: 最后全量自测

## 8. 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| Batch pipeline > 50% 失败 | 高 | Phase 0 数据不全 | --continue-on-error，部分数据也够用 |
| enum-parsing 10x+ 慢是 AOT 架构问题 | 中 | Phase 2 不达标 | 接受 3x，文档说明架构限制 |
| 多 Agent 同时改同一个文件 | 低 | 合并冲突 | conflict_scope 已在子任务表标注 |
| 增量 benchmark 结果不稳定 | 中 | 假阳性 regression | 多次采样 + 统计显著性 |

## 9. 备选路径

- **如果 enum-parsing 优化不达标**：改为"显著改进"而非"10x+"目标，文档说明
- **如果 batch pipeline > 80% 失败**：先修基础设施再继续
- **如果 skill 重构发现不兼容**：旧 skill 保持可用，新 skill 独立部署

## 10. 当前建议推进顺序

```
batch-1 (并行): T-A1 + T-B1
    → wait_for: 两者完成
batch-2 (并行): T-A2 + T-B2 + T-B3 + T-B4
    → wait_for: batch-1 完成
batch-3 (混合): T-A3 + T-C1 + T-D1
    → wait_for: batch-2 完成
batch-4 (并行): T-C2 + T-D2 + T-D3
    → wait_for: batch-3 完成
batch-5 (并行): T-C3 + T-E1 + T-E2
    → wait_for: batch-4 完成
batch-6 (串行): T-FINAL
    → wait_for: batch-5 完成
```

## 11. 子任务执行策略

- child_execution_mode: auto
- auto_continue: true
- auto_stop_policy: blocking-only
- 每个子任务启动前做最小 preflight self-review
- 每个子任务完成后自动同步父 STATUS.md
