# JSON / XML 工业化 — Roadmap v4

> 创建日期：2026-09-22
> 前身：`roadmap-v3-01.md`（2026-09-22 入库）、`roadmap-v2-01.md`、`roadmap-v1-01.md`
> 关键变更：**多终端 worktree 并行**（推翻 v2/v3 的"单终端"前提）+ Phase 2 分族拆解
> 上游基线：本文件 §2（2026-09-22 实测）

---

## 0. 一句话结论

P1-01（ATG 接线）已交付并入库，`AOT-STUB-GAP` 标记 **211 → 146**。
剩余工作约 **1050 条**分布明确，其中 **Phase 2（realDefect 清零，159 条）是投入产出比最高的一步**。

v4 与 v3 的**根本差异**：v2/v3 假设「单终端 + Workflow」，因此判定 worktree
**不能用于并行开发**。实测该前提**不成立**——本仓库已是多终端环境（3 个活跃 worktree
+ 20 个近期提交），且 `artifacts/` 是 gitignored、**每个 worktree 有独立副本**。
因此 v4 改为 **worktree 分族并行**。

---

## 1. v3 的两处前提修正（本日实测）

| # | v3 的表述 | 实测真相 | 影响 |
|:-:|:----------|:---------|:-----|
| 1 | §5.1「单终端下 workflow + worktree 的语义」（worktree 不用于并行） | **已是多终端**；`artifacts/` gitignored，每 worktree 独立 | **worktree 可用于并行开发** |
| 2 | §4 Phase 5「4 个未跑 chunk」（工作量未知） | 实际是 **1 个 build 失败 + 4 个有内容但未配置**，**无一是废弃空壳** | Q4 范围澄清，净增工作量接近 0 |

### 1.1 修正 1 的证据

```
$ git worktree list
D:/agent/chaos-il2cpp                                    [main]
D:/agent/chaos-il2cpp/.claude/worktrees/ci-red-checks    [fix/bootstrap-lnk2019]
D:/agent/chaos-il2cpp/.claude/worktrees/reflection-eh-mode-fix  [...]
D:/agent/chaos-il2cpp/.claude/worktrees/text-baseline-c2c1      [...]

$ git check-ignore -v artifacts/
.gitignore:36:artifacts/	artifacts/
```

`artifacts/` 被忽略 ⇒ 每个 worktree 检出时**自然拥有独立的一份** ⇒
v2 §5.1 最担心的「两个 agent 跑同 chunk 互相覆盖 artifacts」**在 worktree 隔离下不成立**。

**前提**：每个 worktree 必须用 `CHAOS_FOUNDATION_DLL=<自己的>/testing/foundation-dll`，
**不得**指向主检出。

### 1.2 修正 2 的证据（含两次自我纠错）

初次判断「5 个空壳」→ **错误**（实际有 5-19 个文件、8742-18310 行内容）。
二次判断「废弃残留」→ **亦错误**（`_dll/namespace-partition.json` 正式登记了它们）。

**权威依据**：`testing/foundation-dll/System.Xml.ReaderWriter/_dll/namespace-partition.json`

```json
{ "assemblyName": "System.Xml.ReaderWriter", "totalChunks": 3,
  "chunks": [ { "slug": "xml", "namespaces": ["System.Xml"], "methodCount": 287, ... } ] }
```

**教训**：目录列表 + git 历史不足以判定废弃；必须查**权威配置**。

---

## 2. 当前基线（2026-09-22 实测）

| chunk | real | **realDefect** | stubGap | factoryGap | failed | total |
|:------|:----:|:--------------:|:-------:|:----------:|:------:|:-----:|
| Xml/xml | 136 | **159** | 185 | 48 | 107 | 746 |
| Xml/xsl | 4 | 0 | **246** | 0 | 0 | 251 |
| Xml/serialization | 63 | 0 | 68 | 0 | 4 | 138 |
| Json/text-json | 54 | 0 | 129 | 22 | 3 | 224 |
| Json/converters | 8 | 0 | 80 | ? | 2 | 96 |
| Json/global-ns | — | — | — | — | — | **build 阻断** |
| Xml/ReaderWriter/xml | 136 | 159 | 185 | 48 | 107 | 746 |
| Xml/{schema,xsl-runtime}, RW/{schema,xsl} | — | — | — | — | — | 未跑 |

### 2.1 🔴 关键发现：`ReaderWriter/xml` 与 `PrivateXml/xml` 数值完全相同

```
ReaderWriter/xml: real=136 stubGap=185 failed=107 total=746
PrivateXml/xml  : real=136 stubGap=185 failed=107 total=746
（subject 交集 = 0，证明是两批不同 subject、同一批实现）
```

**非假数字**（独立 provenance，`cacheProvenance: miss`）。
原因：`System.Xml.ReaderWriter` 的方法**转发到 System.Private.Xml 的实现**。

⇒ **修好 `System.Private.Xml`，ReaderWriter 自动同步好转。Q4 净增工作量 ≈ 0。**

### 2.2 realDefect 分布（Phase 2 的目标）

| 测试类 | 条数 | 归属 worktree |
|:-------|:----:|:-------------|
| `XmlWriterTests` | **96** | W1 |
| `XmlConvertTests` | **23** | W2 |
| `XmlDocumentTests` | 17 | W3 |
| `XmlTextReaderTests` | 9 | W4 |
| `XmlElementTests` | 5 | W3 |
| `XmlValidatingReaderTests` | 5 | W4 |
| `XmlAttributeTests` | 2 | W3 |
| `XmlNodeReaderTests` | 1 | W4 |
| `XmlTextTests` | 1 | W3 |

**合计 159**（W1=96 / W2=23 / W3=25 / W4=15）

---

## 3. 范围边界

**覆盖**：`System.Text.Json`(486) / `System.Private.Xml`(849) / `System.Xml.ReaderWriter`(442)
+ **XPath/XSLT**（用户 2026-09-22 拍板纳入）

**非目标**：`System.Text.Encodings.Web`、`System.Private.Xml.Linq`（沿用 v1/v2）

---

## 4. 阶段列表

### Phase 2 — realDefect 清零（🔴 当前起点）

- **Goal**：清零 `System.Private.Xml/xml` 的 **159 个 realDefect**
- **依赖**：Phase 1 已完成（`99a16cfcf`）
- **Q8（用户拍板）**：`global-ns` 的 build 阻断**纳入本阶段**
- **分族拆解**（Q1b：按 native 实现文件分，保证 worktree 间不改同一文件）：

| # | worktree | 条数 | 独占的 native 文件 |
|:-:|:---------|:----:|:-------------------|
| **W1** | `wt-xml-writer` | 96 | `xml_writer_stubs.cpp`(42KB) + `xml_writer_async_stubs.cpp`(19KB) |
| **W2** | `wt-xml-convert` | 23 | `xml_convert_stubs.cpp`(23KB) |
| **W3** | `wt-xml-document` | 25 | `xml_document_stubs.cpp`(28KB) |
| **W4** | `wt-xml-reader` | 15 | `xml_reader_stubs.cpp`(32KB) |
| **W5** | `wt-global-ns-fix` | 0（解阻断） | `LoaderStage.CrossAssemblyInstantiation.cs` |

**共享文件**（Q2c：接受冲突，合并时解决）：
`RuntimeHelperShapeRegistry.CoreStubs.Part1.S19/S20/S21/S22.cs`、`Classifier.cs`

**批次**（Q9：3 路并行 × 2 批，避免 CPU/磁盘争用）：
- 批次 1：**W1 + W2 + W5**
- 批次 2：**W3 + W4**

- **Exit Criteria**：`realDefect` == 0
- **估算**：**4-6 会话**

### Phase 3 — P0 核心 API 补齐

- **Goal**：`Utf8JsonReader` / `JsonDocument` / `JsonElement` / `XmlReader` 内容读取族 / `XmlDocument` DOM
- **已知缺口**（代码级核查）：
  - `JsonReader` shape 注册 **0 项**、native **0 文件**
  - `xml_document_stubs.cpp` **19 处 `RaiseNotSupported()`**
  - `xml_reader_stubs.cpp:686-722` `ReadContentAs*`/base64/binhex 恒 `return 0`
  - `xml_convert_stubs.cpp:381/409` `ToDecimal`/`ToGuid` 未实现
- **含 P1-02**：`SubjectInstanceFactory.Create<T>` 的 canonical-body 绕过
  （见 `p1-02-create-generic-canonical-body.md`）
- **估算**：**6-8 会话**

### Phase 4 — XPath / XSLT 族

- **规模**：矩阵 not-covered 约 150 keys + xsl chunk **176 项 XPathNavigator** stubGap
- **首个子任务必须是可行性评估**（引擎级代码，远超"接线"）
- **估算**：**4-6 会话**

### Phase 5 — stubGap 收敛 + 未跑 chunk

- **含**：`ReaderWriter/{schema,xsl}`、`Xml/{schema,xsl-runtime}` 的配置补齐与首跑
- **估算**：**3-4 会话**（较 v3 下调，因 ReaderWriter 与 PrivateXml 同实现）

### Phase 6 — 收口

- **估算**：**2-3 会话**

### 总计：**19–27 会话**

---

## 5. 调度设计（v4 核心变更）

### 5.1 worktree 并行（🔴 推翻 v2/v3）

**可以并行**：不同族的**实现 + pipeline 运行**（各自独立 artifacts）
**必须串行**：同一 worktree 内；同一 chunk 的多 agent

**约束**（每个 worktree 必须遵守）：
```bash
CHAOS_FOUNDATION_DLL=<worktree>/testing/foundation-dll   # 不指向主检出
```

**判据（任一命中即作废本次数字）**：
1. 数字在修复提交后仍逐项不变
2. `entry.exe` mtime 早于同 chunk 的 codegen mtime
3. 编译错误自行消失

### 5.2 Workflow 落点（Q10a：调查并行、实现串行）

```
Phase 2 执行模板:
  ├─ 1. 建 5 个 worktree（各自的 CHAOS_FOUNDATION_DLL）
  ├─ 2. Workflow fan-out：5 个 agent 并行**调查**各族 realDefect（只读）
  │      每族产出：失败模式分类 + 根因假设 + 修复点
  ├─ 3. Workflow synthesize：汇聚跨族共同根因
  ├─ 4. 按批次**串行实现**（worktree 隔离，单 agent）
  ├─ 5. 各族跑自己的 chunk 验证
  └─ 6. 合回 main（解决 shape 注册冲突）
```

**红线**：调查可并行（只读）；**实现阶段同 chunk 不并行**。

### 5.3 flaky 判定纪律

本日实测中，`codegen` 套件在**同一代码**上出现 **15 / 42 / 0** 三种失败数。
**单次跑出红不能作为回归证据**。判定流程：
1. 重跑一次看失败数是否变化
2. 若变化 → flaky，需在**干净 main worktree** 对照
3. 三次一致才算稳定信号

---

## 6. 子任务映射

| batch | phase | 子任务 | 模式 | 估算 |
|:-----:|:-----:|:-------|:-----|:----:|
| B1 | 2 | P2-00 建 5 worktree + Workflow 并行调查 | Workflow fan-out | 1 会话 |
| B1 | 2 | P2-W1 XmlWriter 96 条 | worktree 串行 | 2 会话 |
| B1 | 2 | P2-W2 XmlConvert 23 条 | worktree 串行 | 1 会话 |
| B1 | 2 | P2-W5 global-ns 重复键 | worktree 串行 | 1 会话 |
| B2 | 2 | P2-W3 XmlDocument 25 条 | worktree 串行 | 1 会话 |
| B2 | 2 | P2-W4 XmlReader 15 条 | worktree 串行 | 1 会话 |
| B3 | 3 | P3-01 Utf8JsonReader 整链路 | 串行 | 2-3 会话 |
| B3 | 3 | P3-02 XmlDocument DOM（19 处 NotSupported） | 串行 | 2 会话 |
| B3 | 3 | P3-03 XmlReader 内容读取族 | 串行 | 2 会话 |
| B3 | 3 | P3-04 P1-02 Create<T> canonical-body | 串行 | 1-2 会话 |
| B4 | 4 | P4-00 XPath/XSLT 可行性评估（🔴 先评估） | 串行 | 1-2 会话 |
| B4 | 4 | P4-01..N XPath/XSLT 实现 | 串行 | 3-4 会话 |
| B5 | 5 | P5-01..02 未跑 chunk 配置补齐 + 首跑 | 串行 | 2-3 会话 |
| B6 | 6 | P6-01 性能 + 语义 + 归档 | 串行 | 2-3 会话 |

**Workflow 最佳落点**：Phase 2 的调查（5 族 fan-out）、Phase 5 的 chunk 调查。

---

## 7. 风险与诚实标注

| 风险 | 影响 | 缓解 |
|:-----|:-----|:-----|
| **worktree 并行 CPU/磁盘争用** | 反而更慢 | 限 3 路；xml build 单次 190-260s |
| **shape 注册文件冲突** | 合并困难 | Q2c 已接受；建议按 `Part1.SNN` 分片 |
| **artifacts 指向错** | 假数字 | 强制 `CHAOS_FOUNDATION_DLL=<自己>` |
| **flaky 误导归因** | 误报回归 | §5.3 判定纪律 |
| **XPath/XSLT 是引擎级工程** | 估算可能严重偏低 | P4-00 强制先评估 |
| 未跑 chunk 工作量未知 | 总量失真 | 已确认净增≈0（ReaderWriter 同实现）；Phase 5 后重估 |

**诚实标注**：
- 本 roadmap 的 Phase 2 分族基于 **realDefect 的测试类分布**，各族**内部的失败模式尚未调查**
  —— P2-00 的调查会验证「按测试类分族 == 按根因分族」这一假设
- worktree 并行的实际加速比**未经实测**，§5.1 是结构性推断

---

## 8. 与 v3 的差异总览

| | v3 | v4 |
|:--|:---|:---|
| 终端假设 | 单终端 | **多终端** |
| worktree 用途 | 仅隔离高风险实验 | **分族并行开发** |
| Phase 2 | 整体 3-4 会话 | **5 worktree 分族，4-6 会话** |
| Phase 5 | 4-5 会话 | **3-4 会话**（ReaderWriter 同实现） |
| 总计 | 25-35 会话 | **19-27 会话** |

---

## 9. 下一步入口

**P2-00**：建 5 个 worktree + Workflow 并行调查各族 realDefect 失败模式。

启动方式：`dev-executing-plans`（worktree 隔离 + Workflow 调查）
