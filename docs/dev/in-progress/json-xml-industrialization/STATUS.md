# JSON / XML 工业化 — STATUS

```yaml
task_id: json-xml-industrialization
task_type: roadmap
phase: "Phase 2 — realDefect 35 条根因裁定与修复（P2-01 进行中）"
lifecycle_status: in-progress
current_dir: docs/dev/in-progress/json-xml-industrialization
创建日期: 2026-09-20
entry_skill: dev-il2cpp → dev-brainstorm → dev-roadmap
parent_task_id: null

roadmap_or_plan: roadmap-v5-01.md
前身: roadmap-v4-01.md
关键文档:
  - p2-survey/synthesis.md   # 159 条时代的跨族根因分析（P2-00 调查产物）
  - roadmap-v5-01.md         # 控制实验后的重划（realDefect 159→35）
  - p2-baseline/             # 一手基线：fact-results-59454792b.json + provenance

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
dispatch_model: hybrid
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
merge_granularity: 阶段边界
recommended_next_child: P2-02
```

## P1 阶段进展（2026-09-22）

**P1-01 ✅ 已完成**（提交 `99a16cfcf`）：ATG 消费 codegen shape registry，
取代手工白名单。效果：text-json 的 `AOT-STUB-GAP` 标记数 **211 → 146**，
守卫测试 470 checks + 两次负控验证。

**P1-02 ⏳ 已识别未解决**（详见 `p1-02-create-generic-canonical-body.md`）：
`SubjectInstanceFactory.Create<T>()` 被标记 `has-canonical-body` + `generated-direct-call`，
绑到 `InterpreterEntryDirect`，绕过 shape 决策链 → **factoryGap 22 条**（全在 `Utf8JsonWriterTests`）。
给 `Create` 注册泛型 shape **已实测无效**。AOT `real` 未提升由此导致，非 P1-01 缺陷。
**建议后续 Phase 3 处理**（该类型补真实 AOT body 时一并解决）。

## roadmap-v4（2026-09-22）

**两处前提修正**（推翻 v3）：

1. **worktree 可用于并行开发** —— v2/v3 假设「单终端」，实测已是多终端环境
   （3 个活跃 worktree + 20 个近期提交），且 `artifacts/` gitignored、
   **每个 worktree 独立副本** ⇒ v2 §5.1 担心的 artifacts 互覆盖在隔离下不成立。
   前提：`CHAOS_FOUNDATION_DLL=<自己的>/testing/foundation-dll`。

2. **「4 个未跑 chunk」澄清** —— 无一是废弃空壳。`ReaderWriter/xml` **能跑**
   （实测 136/185/107/746），且与 `PrivateXml/xml` **数值完全相同**（转发到同一实现）
   ⇒ **Q4 净增工作量 ≈ 0**。

**Phase 2 分族**（用户拍板 Q1b/Q9/Q10a）：

| worktree | 条数 | 独占 native 文件 |
|:---------|:----:|:-----------------|
| W1 wt-xml-writer | 96 | xml_writer_stubs.cpp |
| W2 wt-xml-convert | 23 | xml_convert_stubs.cpp |
| W3 wt-xml-document | 25 | xml_document_stubs.cpp |
| W4 wt-xml-reader | 15 | xml_reader_stubs.cpp |
| W5 wt-global-ns-fix | 0（解 build 阻断） | LoaderStage.CrossAssemblyInstantiation.cs |

批次：**3 路并行 × 2 批**（B1=W1+W2+W5，B2=W3+W4）。
Workflow 只用于**调查**（只读 fan-out），实现按 worktree 串行。

**下一步**：P2-00（建 worktree + Workflow 并行调查各族失败模式）。

## roadmap-v5 + P2 控制实验（2026-09-24）

### 🔴 控制实验：realDefect 159 → 35

p2-survey 的 P0-a 要求的控制实验**已执行**。产物：
`p2-baseline/fact-results-59454792b.json`（`provenance.gitCommit` 已比对 = main HEAD ✅）。

| resultKind | 修复前 `5fa746b86` | **当前 main `59454792b`** | 变化 |
|:-----------|:------------------:|:-------------------------:|:----:|
| **realDefect** | **159** | **35** | **−124 (−78%)** |
| real | 136 | **253** | +117 |
| stubGap | 185 | 174 | −11 |
| unassertable | 74 | 50 | −24 |
| smoke | 37 | 75 | +38 |

**根因**：`BuildExceptionTypeTable()` 去重键（subject id）≠ 消费键（display name）
→ 同名孪生占位版恒赢首匹配 → 抛 null 对象（= p2-survey 的 **RC-1**）。
修复 `87a01ecdd` 正是闸 1。

⇒ **p2-survey 规划的 W1+W2+W3+W4 = 96+23+25+15 = 159 条，与旧基线精确吻合；
那 124 条全部由 RC-1 造成，不是 native 类名写错。** v4 分族**大部分失效**。

### v4 的两处前提被推翻/修正

1. **分族基数失效** —— W1(96→3) / W2(23→0) / W3(25→14) / W4(15→18)。
   详见 `roadmap-v5-01.md` §2.1。
2. **§5.3 的 artifacts 隔离前提不完整** —— 隔离的是目录，不是时效性。
   实例：某 worktree `hotupdate.json` 记 135，同目录 `entry.exe` 重跑恒为 0；
   时间戳看似配套但实际不是同一次构建。
   **判据升级**：新增「`provenance.json` 的 `gitCommit` 必须等于当前 HEAD」。

### 第三处发现：`testing/` 下的 subject 源是陈旧副本

`testing/.../managed/combined/CombinedSubjects.cs` 是 **Sep-10** 的旧文件；
pipeline 每次构建重新生成到 `artifacts/.../managed/combined/CombinedSubjects.cs`。
**读 ATG 源码判断 subject 形态时必须读 `artifacts/` 那份**，否则判断会基于过时代码。

### 35 条的形态与未决问题

- **TYPED-CATCH 28 / AREQUAL 5 / BARE 2**（全部 `caught=true, assertFailed=false`）
- **两个方向相反的责任假设未裁定**（native 异常类名错 vs ATG probe 语境错），
  **裁定前不得改代码**。P2-01 的 workflow 正在做对抗性证伪。

**下一步**：P2-01 出裁定 → **P2-02** 按责任层修复。

## Phase 0 完成结论（2026-09-20）

**提交**：`0262d6873`

### 硬指标

**矩阵从 100% unclassified → 0% unclassified。**

| 档位（按方法重载数，分母 1603） | 数量 | 占比 |
|:--------------------------------|:----:|:----:|
| **real** | **238** | **14.8%** |
| not-covered | 652 | 40.7% |
| unassertable | 211 | 13.2% |
| stub | 202 | 12.6% |
| failed | 122 | 7.6% |
| smoke | 98 | 6.1% |
| factory-gap | 70 | 4.4% |
| **fake-green** | **10** | 0.6% |

**`real 14.8%` 与实测 `realVerified 14.4%` 同量级** —— 两个独立口径互相印证。

### 分母口径（用户拍板）

ref-pack 8.0.11 的 public API（System.Text.Json 312 + System.Xml.ReaderWriter 442）
+ shared 8.0.11 的 System.Private.Xml（849，ref-pack 无此程序集）= **1603**。

### Phase 0 挖出的关键问题（7 项发现）

1-2. manifest 不含 `JsonSerializer`；fact 与 manifest 无法直接 join（0%）
3. `passed` 包含 `stubGap` 的机制**已代码级确认**（`fact_chunk.py:139`）
4. **量化**：manifest 缺 1 个类型 = 漏 42% 测试对象
5-6. **根因**：现有 manifest 扫的是含 internal 的实现程序集；ref-pack 才是纯 public
7. 分布高度不均 → 禁用「类型覆盖率」

### 执行中的两处自我纠错

- **类型名规范化**：早期「去重复段」启发式产出 `System.Private.Xml.System.Xml.NameTable`
  （多一层前缀）→ 改用「按首个 `.` 切分丢弃 assembly 段」
- **聚合口径**：按方法名键统计会把 213 条 real 坍缩成 17 个键 → 改为双口径

### 已知局限（随产物引用）

- 映射为方法名级**不含参数** → 覆盖率为**下限估计**
- `not-covered 40.7%` **不等于未实现**（含从未生成测试的 API）
- 分母中四成 API 从未进入验证范围 → **可能低估工作量**，Phase 4 重估

## 下一步入口

**Phase 1（假绿清零）**：修复三个口径陷阱 + 消除 `passed` 虚高。
估算 1 会话。

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
