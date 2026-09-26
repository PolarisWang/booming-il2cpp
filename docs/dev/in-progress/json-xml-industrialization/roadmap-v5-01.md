# JSON / XML 工业化 — Roadmap v5

> 创建日期：2026-09-24
> 前身：`roadmap-v4-01.md`（2026-09-22）
> 关键变更：**P2 控制实验已执行，realDefect 159 → 35**；v4 的 W1-W5 分族**大部分失效**
> 上游基线：`p2-baseline/fact-results-59454792b.json`（本文件 §2，一手实测）

---

## 0. 一句话结论

**v4 的 Phase 2 分族前提已被实测推翻。** 控制实验证明：
Phase 2 原定的 159 条里 **124 条（78%）由单一跨族根因造成，且该根因已修复**。
剩余 **35 条**，分布与 v4 假设完全不同 —— **不应再按 v4 建 5 个 worktree**。

| | v4 假设 | v5 实测 |
|:--|:--|:--|
| realDefect 总数 | 159 | **35** |
| W1 (XmlWriter) | 96 | **3** |
| W2 (XmlConvert) | 23 | **0** |
| W3 (XmlDocument/Element/Attribute/Text) | 25 | **14** |
| W4 (XmlReader 三族) | 15 | **18** |
| Phase 2 估算 | 4-6 会话 | **1-2 会话**（重新分族后） |

---

## 1. v4 的失效点（本日实测）

### 1.1 修正 1：分族基数失效

v4 §2.2 的分族基于 `realDefect=159` 的**测试类分布**。该 159 已不复存在。

**根因（已修复）**：`BuildExceptionTypeTable()` 对 subject id 去重，但发射到
`kChaosExceptionTypes[]` 的是 **display name**。两个同名不同类型
（`Chaos.TestFramework.Sdk/System.InvalidOperationException` 的 parent=nullptr 占位版
vs `System.Private.CoreLib/...` 的真继承链版）都进表，`ResolveTypeByName` 线性首匹配
+ 字典序（`'C' < 'S'`）→ **占位版恒赢** → 抛 null 对象 → `chaos_eh_match_type` 必然失败。

修复 commit：`87a01ecdd`（2026-09-24）。这与 p2-survey 的 **RC-1** 是同一根因。

### 1.2 修正 2：v4 §5.3 的 worktree artifacts 前提**不完整**

v4 称「`artifacts/` gitignored ⇒ 每 worktree 独立副本 ⇒ 互覆盖不成立」。
**实测该推论不成立**：

- `artifacts/` 下是**可变未跟踪构建产物**，`entry.exe` / `patch.patchdata` 会被任何一次
  pipeline 覆盖 —— 隔离的是**目录**，不是**时效性**
- 实例（2026-09-24）：某 worktree 的 `results/hotupdate.json` 记 `failedMethods:135`，
  但同目录 `entry.exe` 重跑恒为 `0`；两产物**时间戳看似配套**
  （patchdata 13:32:18 → exe 13:32:23 → json 13:33:00），**实际不是同一次构建**

⇒ **判据必须从「时间戳」升级为「产物内插桩标记 + provenance gitCommit 比对」**。
v4 §5.1 的三条判据保留，但**新增第 4 条**：
4. `provenance.json` 的 `gitCommit` 必须等于当前 HEAD，否则该次数字作废

### 1.3 修正 3：`testing/` 下的 subject 源可能是陈旧副本

本日实测：`testing/foundation-dll/System.Private.Xml/chunks/xml/managed/combined/CombinedSubjects.cs`
是 **Sep-10** 的旧文件，而 pipeline 每次构建会重新生成到
`artifacts/foundation-dll/System.Private.Xml/chunks/xml/managed/combined/CombinedSubjects.cs`。

**读 ATG 源码分析 subject 形态时，必须读 `artifacts/` 下那份**，否则会基于过时代码做判断。
（本日因此一度误判 35 条为 "SMOKE-BARE"，实际是新形态 "TYPED-CATCH"。）

---

## 2. 当前基线（2026-09-24 实测，一手）

**产物**：`p2-baseline/fact-results-59454792b.json`
**provenance**：`gitCommit=59454792b`，与 main HEAD 一致 ✅

| chunk | real | **realDefect** | stubGap | factoryGap | failed | smoke | unassertable | unimplemented | total |
|:------|:----:|:--------------:|:-------:|:----------:|:------:|:-----:|:------------:|:-------------:|:-----:|
| Xml/xml | 253 | **35** | 174 | 48 | 101 | 75 | 50 | 6 | 742 |

**对照修复前**（`5fa746b86`，2026-09-23）：realDefect **159** / real 136 / stubGap 185。

### 2.1 35 条 realDefect 的分布（v5 的新分族基数）

| 测试类 | 条数 | v4 归属 | 变化 |
|:-------|:----:|:--------|:-----|
| `XmlTextReaderTests` | 10 | W4 | +1 |
| `XmlDocumentTests` | 6 | W3 | −11 |
| `XmlElementTests` | 5 | W3 | 0 |
| `XmlValidatingReaderTests` | 5 | W4 | 0 |
| `XmlWriterTests` | 3 | W1 | **−93** |
| `XmlAttributeTests` | 2 | W3 | 0 |
| `XmlReaderTests` | 2 | W4 | +2 |
| `XmlNodeReaderTests` | 1 | W4 | 0 |
| `XmlTextTests` | 1 | W3 | 0 |
| `XmlConvertTests` | **0** | W2 | **−23** |

**按方法名聚类**：`GetAttribute`(6) / `MoveToAttribute`(6) / `CreateNode`(3) /
`GetElementsByTagName`(2) / `SetAttribute`(2) / `ResolveEntity`(2) / 其余 14 个方法各 1 条。

### 2.2 35 条的 ATG 源码形态（读 `artifacts/` 下新生成体）

| 形态 | 条数 | 特征 |
|:-----|:----:|:-----|
| **TYPED-CATCH** | 28 | `// AOT-THROWS-ASSERT` + `try { CALL; throw 哨兵 } catch (<期望类型>) {} catch { throw "wrong exception type" }` |
| **AREQUAL** | 5 | `var r = CALL; Assert.AreEqual(<期望>, r)` |
| **BARE** | 2 | `CALL; return 42L;`（无断言） |

**共同记录形态**：全部 `passed=false, value=0, caught=true, assertFailed=false`。

---

## 3. Phase 2 重划（v5）

### 3.1 不再按 worktree 分族

理由：35 条的**失败模式尚未定根因**（正在调查中），按测试类分族在 v4 已证明会
产生重复修复与语义冲突（v4 §2.2 自查）。且总量降到 1-2 会话规模，
**worktree 并行的调度开销大于收益**。

### 3.2 新的执行顺序

| # | 子任务 | 模式 | 依赖 |
|:-:|:-------|:-----|:-----|
| **P2-01** | 35 条根因裁定（workflow：native 语义 / ATG 载体 / 探针复现 / 历史对照 四线并行） | Workflow | ✅ 进行中 |
| **P2-02** | 按**责任层**分组修复（而非按测试类） | 串行 | P2-01 |
| **P2-03** | 重跑 xml chunk，验证 realDefect 收敛 | 串行 | P2-02 |
| **P2-04** | 若收敛，同步验证 `ReaderWriter/xml`（同实现，应自动同步） | 串行 | P2-03 |

**Exit Criteria**：`realDefect == 0`（或每条剩余项有书面豁免理由）

- **估算**：**2-3 会话**

### 3.3 关键未决问题（P2-01 必须在修之前裁定）

这 35 条的**责任层存在两个方向相反的假设**，裁定前不得动手改代码：

- **假设 N（native 错）**：native 抛 `ArgumentOutOfRangeException`，
  real .NET 抛 `NullReferenceException` → native 异常类名写错
- **假设 A（ATG 载体错）**：ATG probe 在**裸对象/null-this** 语境记录期望异常，
  而生成体用**真实构造实例** → 两边语境不同，期望不可比

**已有一手证据**（real .NET 8 实测，`tmp_probe/`）：

```
XmlTextReader.GetAttribute(null)                        -> NullReferenceException
XmlTextReader.MoveToAttribute(null)                     -> NullReferenceException
XmlDocument.CreateAttribute(null,null)                  -> NullReferenceException
XmlDocument.CreateElement("root").GetElementsByTagName(null) -> ArgumentNullException
```

⇒ **ATG 探针的期望与 real .NET 一致**，即 ATG 的期望本身是对的。
但这**还不能**推出"native 错"——因为还需确认 probe 记录期望时的**语境**
是否与生成体的语境相同。

---

## 4. 后续阶段（沿用 v4，未受影响）

- **Phase 3** — P0 核心 API 补齐（`Utf8JsonReader` / `JsonDocument` / `XmlReader` 内容读取族 / DOM）
- **Phase 4** — XPath / XSLT（🔴 首个子任务必须是可行性评估）
- **Phase 5** — stubGap 收敛 + 未跑 chunk
- **Phase 6** — 收口

Phase 3 的已知缺口核查清单见 v4 §4，**仍然有效**：
`json_reader` shape 注册 0 项 / `xml_document_stubs.cpp` 19 处 `RaiseNotSupported()`
/ `xml_reader_stubs.cpp:686-722` `ReadContentAs*` 恒 `return 0`。

---

## 5. 风险与诚实标注

| 风险 | 影响 | 缓解 |
|:-----|:-----|:-----|
| **产物陈旧伪装成缺陷** | 为不存在的缺陷开工作流 | §1.2 第 4 条判据（provenance gitCommit 比对） |
| **读错 ATG 源** | 基于过时代码判断形态 | §1.3：只读 `artifacts/` 下新生成体 |
| **责任层误判** | 改错层，白做且引入新失败 | §3.3：裁定前不改代码；对抗性证伪 |
| **按测试类分族** | 重复修复 + 语义冲突 | v4 已证明失效；v5 改按**责任层**分 |

**诚实标注**：
- §2 的 35 条分布是**一手实测**（provenance 已比对 gitCommit）
- §2.2 的形态分类基于 `artifacts/` 下新生成体，**非** `testing/` 旧副本
- §3.3 的两个假设**均未裁定**，P2-01 的 workflow 正在做对抗性证伪；
  在它出结论前，本 roadmap **不建议任何代码改动**

---

## 6. 下一步入口

**P2-01**（进行中）→ 裁定 35 条责任层 → **P2-02** 按责任层修复。

启动方式：`dev-executing-plans`
