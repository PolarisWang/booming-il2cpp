# JSON / XML 工业化 — STATUS

```yaml
task_id: json-xml-industrialization
task_type: roadmap
phase: planning
lifecycle_status: in-progress
current_dir: docs/dev/in-progress/json-xml-industrialization
创建日期: 2026-09-20
entry_skill: dev-il2cpp → dev-brainstorm → dev-roadmap
parent_task_id: null

roadmap_or_plan: roadmap-v2-01.md
上游基线: docs/dev/review/json-xml-industrialization-gap-2026-09-20.md（本日实测）
前身: docs/dev/in-progress/json-xml-production-readiness/roadmap-v1-01.md

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
dispatch_model: hybrid
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
merge_granularity: 阶段边界
recommended_next_child: P0-01
```

## 边界拍板

**覆盖**：`System.Text.Json`(486) / `System.Private.Xml`(849) / `System.Xml.ReaderWriter`(442)
的 public method 面。

**非目标**：不追求 100% real；不重构 `ExternalRuntimeFallback`；不为凑覆盖率加无断言 smoke；
**不并行跑同一个 chunk**。

## authority 决策

| 决策 | 结论 | 来源 |
|:-----|:-----|:-----|
| 终端数 | **1 个 + Workflow** | 用户拍板 |
| 起点 | **先建度量**（Phase 0） | 用户拍板 |
| v1 roadmap | **演进为 v2**（保留 v1 作参考） | 用户拍板 |
| 合回粒度 | **阶段边界** | 用户拍板 |

## 当前结论

### 基线（本日实测，非历史快照）

5 个可跑 chunk 合计：`total=1478, realVerified=213 (14.4%), stubGap=756 (51.2%)`。

| chunk | total | realVerified | stubGap |
|:------|:-----:|:------------:|:-------:|
| Xml/xml | 746 | 136 (18.2%) | 185 |
| Xml/serialization | 138 | 63 (45.7%) | 68 |
| Xml/xsl | 251 | 4 (1.6%) | 246 |
| Json/text-json | 247 | 2 (0.8%) | 177 |
| Json/converters | 96 | 8 (8.3%) | 80 |

### 阶段划分（6 阶段，17-24 会话）

| Phase | 目标 | 估算 |
|:-----:|:-----|:----:|
| 0 | 度量基建（矩阵 unclassified <10%） | 1-2 |
| 1 | 假绿清零 + 三口径修复 | 1 |
| 2 | P0 核心 API（realVerified >60%） | 4-6 |
| 3 | stubGap 收敛（<10%） | 6-8 |
| 4 | failed 对账 + 未跑 chunk | 3-4 |
| 5 | 性能 + 语义 + 归档 | 2-3 |

## 风险评估摘要

| 风险 | 等级 | 缓解 |
|:-----|:----:|:-----|
| **artifacts 并发污染** | 🔴 高 | §5.2 红线：不并行跑同 chunk + 三条判据 |
| 估算误差（外推） | 🟡 中 | Phase 0 后用真标尺重估 |
| stubGap 藏真缺陷 | 🟡 中 | Phase 4 强制对账 |
| 单终端吞吐 | 🟢 低 | 用 Workflow 补并行调查 |
| `global-ns` 跑失败 | 🟡 中 | Phase 4 处理（已排除与本日修复相关） |

## 调度设计要点（本 roadmap 核心）

**单终端下 workflow + worktree 的语义与多终端不同**：

- **Workflow** = agent 级并行（**共享工作树**）→ 只能做只读调查/分析
- **worktree** = 隔离高风险实验，**不是**并行开发手段

**最佳 Workflow 落点**：Phase 0（并行扫 chunk 数据）、Phase 4（并行对账 failed）——
均为纯调查，无共享写入。

**必须串行**：Phase 2/3 的实现（改共享源文件）、任何 pipeline 运行。

## 未验证 / 诚实标注

- 估算是基于 5 个 chunk **外推**到 8 chunk + 3 assembly，**非精确度量**
- 未纳入 `System.Xml.ReaderWriter`(442) 实测 —— 可能显著改变总量
- Phase 0 完成后应**重估**后续阶段

## 下一步入口

**Phase 0**：`dev-executing-plans` + Workflow（并行扫 chunk 数据建矩阵）
