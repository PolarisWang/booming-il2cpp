# JSON / XML 工业化 — Roadmap v3

> 创建日期：2026-09-22
> 前身：`roadmap-v2-01.md`（2026-09-20，保留作历史参考）
> 上游基线：`roadmap-v2-01.md` + 本日（2026-09-22）实测复核
> 关键变更：**修正 v2 的三处事实性遗漏**（见 §1），纳入 XPath/XSLT，新增 ATG 自动化接线

---

## 0. 一句话结论

JSON/XML 翻译层处于「**通道已打通、接线大面积缺失**」阶段。

**最重要的发现**：native 实现远比预期完整（writer/reader/nametable 全是真实实现，无 `return 0` 桩），
但 **ATG 的 `Classifier.KnownNativeImpls` 白名单里 json/xml 条目数为 0** ——
`TestEmitter.cs:740 HasKnownNativeImpl()` 因此对每个 json/xml 方法都发 `return 42L` 桩，
**native 实现根本没被调用**。

→ **Phase 1 从「假绿清零」改为「ATG 接线」**，这是当前投入产出比最高的一步，
且它是后续所有工作的**前置闸门**（不接线，实现了也不生效）。

---

## 1. v2 的三处事实性遗漏（本日实测修正）

| # | v2 的表述 | 实测真相 | 影响 |
|:-:|:----------|:---------|:-----|
| 1 | 「stubGap 收敛」列为 Phase 3（实现问题） | stubGap 的闸门在 **ATG 白名单**，非 native | **阶段顺序错**：不先接线，Phase 2/3 的实现在 fact 上不可见 |
| 2 | 未提及 realDefect | `xml` chunk 有 **159 个 `realDefect`**（执行了但行为错） | 比 stubGap 严重，且 v2 完全未覆盖 |
| 3 | 估算外推自「5 个可跑 chunk」 | 实际配置 **8 个 chunk**，其中 **4 个从未产出 fact** | 分母与工作量均被低估 |

### 1.1 遗漏 1 的证据链

```
TestEmitter.cs:740  HasKnownNativeImpl(method)
  ├─ Classifier.WholeTypeRealViaDispatch.Contains(bareType)   → 仅 2 项
  └─ Classifier.KnownNativeImpls 匹配 (bareType, memberName)  → 904 条
        其中 json/xml 条目数 = 0     ← grep -icE "XmlWriter|ChaosXml|JsonWriter" = 0
```

后果实测：`xml_writer_stubs.cpp` 42KB / 36 export **全部真实实现**，
但 `xml` chunk 仍有 **185 stubGap**。

### 1.2 遗漏 2 的证据

`System.Private.Xml/chunks/xml`（9/21 数据）：

```
total=746   failed=314  =  107 failed + 159 realDefect + 48 factoryGap
realDefect 分布: XmlWriterTests 96 / XmlConvertTests 23 / XmlDocumentTests 17 / ...
```

### 1.3 遗漏 3 的证据

| assembly | chunk | 状态 |
|---|---|---|
| System.Text.Json | text-json | ✅ 9/22 |
| System.Text.Json | converters | ✅ 9/20 |
| System.Text.Json | global-ns | ❌ **provenance: build=error，无 fact** |
| System.Private.Xml | xml | ✅ 9/21 |
| System.Private.Xml | xsl | ✅ 9/20 |
| System.Private.Xml | serialization | ✅ 9/20 |
| System.Private.Xml | **schema / xsl-runtime** | ❌ 空壳，从未跑 |
| System.Xml.ReaderWriter | **全部 3 个** | ❌ 空壳目录（仅空 managed/native） |

---

## 2. 当前基线（2026-09-22 实测）

### 2.1 分 chunk

| chunk | total | realVerified | 覆盖率 | stubGap | failed | smoke 占比 |
|:------|:-----:|:------------:|:------:|:-------:|:------:|:----------:|
| Text.Json / text-json | 246 | **2** | 0.8% | 176 | 3 | **98%** |
| Text.Json / converters | 96 | 8 | 8.3% | 80 | 2 | 90% |
| P.Xml / xml | 746 | 136 | 18.2% | 185 | 107(+159 realDefect) | 40% |
| P.Xml / xsl | 251 | 4 | 1.6% | 246 | 0 | **98%** |
| P.Xml / serialization | 138 | 63 | 45.7% | 68 | 4 | 51% |

⚠️ **smoke 占比 98% 意味着 `passed` 高度虚高** —— text-json `passed=243`，但 `realVerified` 仅 2。

### 2.2 Phase 0 矩阵（9/20，分母 1603）

| 档位 | 数量 | 占比 |
|:-----|:----:|:----:|
| not-covered | 652 | 40.7% |
| real | 238 | **14.8%** |
| unassertable | 211 | 13.2% |
| stub | 202 | 12.6% |
| failed | 122 | 7.6% |
| smoke | 98 | 6.1% |
| factory-gap | 70 | 4.4% |
| fake-green | 10 | 0.6% |

**按域拆分（按 (type,method) key）**：

| 域 | keys | real | not-covered | stub |
|:---|:----:|:----:|:-----------:|:----:|
| System.Text.Json | 157 | **7 (4.5%)** | 62 (39.5%) | 57 (36.3%) |
| XML | 656 | 92 (14.0%) | **351 (53.5%)** | 47 (7.2%) |

### 2.3 not-covered 的大头（决定 Phase 2 范围）

| 类型族 | keys | 说明 |
|:-------|:----:|:-----|
| `System.Xml.Xsl.Runtime.*`（XmlQueryOutput/Runtime/XsltFunctions/…） | ~150 | **XSLT 引擎** |
| `System.Text.Json.Utf8JsonReader` | 38 | **读取链路整块缺失** |
| `System.Xml.Schema.XmlSchema*` | ~28 | Schema 验证 |
| `JsonElement(+Enumerator)` | ~12 | |

**Phase 2 的范围由 this 表决定，而非 v2 的「Utf8JsonReader + XmlReader 全族」。**

---

## 3. 范围边界

**覆盖**（沿用 v2 + 本日扩充）：

- `System.Text.Json`(486) / `System.Private.Xml`(849) / `System.Xml.ReaderWriter`(442)
- ✅ **纳入 XPath / XSLT**（用户 2026-09-22 拍板）：`System.Xml.XPath.*` + `System.Xml.Xsl.Runtime.*` + `System.Xml.Xsl.*`

**不覆盖**：`System.Text.Encodings.Web`、`System.Private.Xml.Linq`（沿用 v1/v2）

---

## 4. 阶段列表

### Phase 1 — ATG 接线（🔴 起点，最高优先级）

- **Goal**：让 ATG 知道 AOT 侧实现了什么，停止发错误的 `return 42L` 桩。
- **为什么是第一步**：当前 native 已实现的方法**在 fact 上完全不可见**。
  不接线，后续任何实现都白做。
- **方案（用户拍板 Q2=b：机制改造，非白名单堆砌）**：
  让 ATG 消费 codegen 产出的 **`aot-capability-manifest.json`**，替代手工白名单。

- **可行性（本日已实测验证）**：

  | 项 | 证据 |
  |:---|:-----|
  | manifest 存在 | `aot-capability-manifest.json`，schemaVersion 1 |
  | 规模 | text-json chunk: **2083 entries**（exact 1310 / generic 256 / inline 517） |
  | 字段对齐 | entry = `{typeDisplayName(Prefix), methodName, nativeSymbol}`，**与 `HasKnownNativeImpl` 的 (bareType, memberName) 匹配逻辑同构** |
  | json/xml 覆盖 | **308 entries**（Utf8JsonWriter 66 / XmlWriter 61 / XmlTextReader 30 / …） |
  | 产出点 | `NativeAotLoweringPlanner.Methods.cs:1765` `_shapeRegistry.ExportManifest()` |

- **🔴 关键时序约束（必须解决，否则方案不成立）**：

  ```
  build 阶段顺序（tests/e2e/verification/stages/build.py）:
    1. AutoTestGenerator          ← 发 42L 桩的决策点。此处 AOT 尚未运行
    2. 合并 .cs → subjects DLL
    3. 编译 subjects DLL
    4. TPG generate-dll → entry.exe ← codegen 在此，manifest 此刻才产出
  ```

  **ATG 在 codegen 之前运行** → 无法直接读本次的 manifest。
  三条候选路径（需在 Phase 1 子任务中择一，见 §6）：
  - (a) 用**上次构建**的 manifest（跨 chunk 累积一张能力表）
  - (b) 把 ATG 拆成两趟：先探明能力，再生成
  - (c) ATG 直接静态分析 shape 注册表源码（`RuntimeHelperShapeRegistry*.cs`）

- **Exit Criteria**（2026-09-22 修正）：
  - **`AOT-STUB-GAP` 标记数显著下降**（P1-01 直接产出；实测 text-json **211 → 146** ✅）
  - ~~AOT `real` 提升~~ ← **删除**：AOT `real` 受阻于独立的 `Create<T>` canonical-body 缺口
    （见 `p1-02-create-generic-canonical-body.md`），不是 P1-01 的产出责任
  - ⚠️ **接线后 `passed` 会先下降**（桩转真调用后，原本"passed"的变 red）——诚实化代价，**不得回退**

- **估算**：**3-5 会话**（Phase 1 主体已完成；P1-00 时序方案选型经实测走"ATG 直接引用 Generator"）

- **✅ P1-01 已完成**（`99a16cfcf`）：新增 `ShapeRegistryIndex`，ATG 从
  `RuntimeHelperShapeRegistry.BuildDefault()` 构建 (type, method) 索引，
  `HasKnownNativeImpl` 改为 registry ∪ 白名单。守卫测试 470 checks + 两次负控。
  **仓库首条 Tools→Codegen 引用**（经用户拍板）。

- **⚠️ P1-02 已识别未解决**（`p1-02-create-generic-canonical-body.md`）：
  `SubjectInstanceFactory.Create<T>` 被标记 `has-canonical-body` + `generated-direct-call`，
  绑到 `InterpreterEntryDirect`，**绕过 shape 决策链** → 22 条 factoryGap（AOT 侧方法没跑到）。
  给 `Create` 注册泛型 shape **已实测无效**。**建议并入 Phase 3** 处理。

### Phase 2 — realDefect 清零（🔴 新增，v2 遗漏）

- **Goal**：`xml` chunk 的 **159 个 realDefect** 逐项定位（执行了但行为错）。
- **特点**：这类是**真缺陷**，不是"未实现"。分布在 XmlWriter(96) / XmlConvert(23) / XmlDocument(17)。
- **依赖**：**Phase 1 之后**（否则大批 realDefect 会被 stubGap 掩盖）
- **估算**：**3-4 会话**

### Phase 3 — P0 核心 API 补齐

- **Goal**：`Utf8JsonReader` / `JsonDocument` / `JsonElement` / `XmlReader` 内容读取族 / `XmlDocument` DOM
- **范围依据**：§2.3 的 not-covered 表
- **已知缺口（agent 代码级核查）**：
  - `JsonReader` shape 注册 **0 项**，native **0 文件**（整块缺失）
  - `xml_document_stubs.cpp` **19 处 `RaiseNotSupported()`**（AppendChild/InsertBefore/ImportNode/LoadXml 等 DOM 树操作全缺）
  - `xml_reader_stubs.cpp:686-722` `ReadContentAs*`/base64/binhex 恒 `return 0`
  - `xml_convert_stubs.cpp:381/409` `ToDecimal`/`ToGuid` 显式未实现
- **估算**：**6-8 会话**

### Phase 4 — XPath / XSLT 族（🔴 新增）

- **Goal**：`System.Xml.XPath.*` + `System.Xml.Xsl.Runtime.*` 骨架
- **规模**：矩阵 not-covered 中约 150 keys（Xsl.Runtime）+ xsl chunk 的 **176 项 XPathNavigator** stubGap
- **风险**：这是**引擎级**代码（查询求值、排序、聚合），远超"接线"范畴；需先评估是否可用简化实现满足断言
- **估算**：**4-6 会话**（首个子任务应是可行性评估，而非直接实现）

### Phase 5 — stubGap 收敛 + 未跑 chunk 补齐

- **Goal**：残余 stubGap 逐项收敛；补齐 **4 个从未产出 fact 的 chunk**
- **未跑清单**：`Text.Json/global-ns`（build=error）、`P.Xml/schema`、`P.Xml/xsl-runtime`、`Xml.ReaderWriter` 全部 3 个
- **前置**：需先归因 `global-ns` 的 build error
- **估算**：**4-5 会话**

### Phase 6 — 收口

- **Goal**：性能基线 + 语义对齐 + 归档
- **估算**：**2-3 会话**

### 总计：**25–35 会话**

> **诚实标注**：此估算基于**本日实测的 5 个 chunk 数据** + 4 个未跑 chunk 的**结构性推断**。
> 未跑 chunk 的工作量是本次最主要的不确定源 —— Phase 5 完成后应重估。

---

## 5. 调度设计（沿用 v2 §5，本日补充）

### 5.1 并行红线（不变）

**可以并行**（只读）：多 agent 调查不同 chunk / API 族；分析 fact 数据；设计（不改文件）
**必须串行**（共享写入）：❌ 两个 agent 跑同一 chunk 的 pipeline；❌ 两个 agent 改同一批源文件

**判据（任一命中即作废本次数字）**：
1. 数字在修复提交后仍逐项不变
2. `entry.exe` mtime 早于同 chunk 的 codegen mtime
3. 编译错误自行消失

### 5.2 本日新增：flaky 判定纪律

本日实测中，`codegen` 测试套件在**同一代码**上出现 **15 / 42 / 0** 三种失败数。
**单次跑出红不能作为回归证据**。判定流程：
1. 重跑一次看失败数是否变化
2. 若变化 → flaky，需在**干净 main worktree** 对照确认基线
3. 三次一致才算稳定信号

### 5.3 每阶段执行模板（沿用 v2 §5.4）

```
Phase N 开始
  ├─ 1. 建阶段分支
  ├─ 2. Workflow 并行调查（只读）
  ├─ 3. 串行实现
  ├─ 4. 阶段自验（串行跑相关 chunk）
  └─ 5. 合回 main + 删 worktree
```

---

## 6. 子任务映射

| batch | phase | 子任务 | 模式 | 估算 |
|:-----:|:-----:|:-------|:-----|:----:|
| B1 | 1 | **P1-00 时序方案选型**（(a)/(b)/(c) 择一 + 原型验证） | 串行（架构决策） | 1-2 会话 |
| B1 | 1 | P1-01 ATG 消费 manifest | Workflow 调查 + 串行实现 | 2-3 会话 |
| B2 | 2 | P2-01..03 realDefect 分族对账（Writer/Convert/Document） | **Workflow 并行调查** | 3-4 会话 |
| B3 | 3 | P3-01 Utf8JsonReader 整链路 | 串行 | 2-3 会话 |
| B3 | 3 | P3-02 XmlDocument DOM 树（19 处 RaiseNotSupported） | 串行 | 2 会话 |
| B3 | 3 | P3-03 XmlReader 内容读取族 | 串行 | 2 会话 |
| B4 | 4 | P4-00 **XPath/XSLT 可行性评估**（🔴 先评估再实现） | 串行 | 1-2 会话 |
| B4 | 4 | P4-01..N XPath/XSLT 实现（按评估结论拆） | 串行 | 3-4 会话 |
| B5 | 5 | P5-01 global-ns build error 归因 | 串行 | 1 会话 |
| B5 | 5 | P5-02 4 个未跑 chunk 补齐 | 串行（避免 artifacts 冲突） | 2-3 会话 |
| B6 | 6 | P6-01 性能 + 语义 + 归档 | 串行 | 2-3 会话 |

**Workflow 最佳落点**：Phase 2（并行对账 realDefect）、Phase 5 的 chunk 调查。

---

## 7. 风险与诚实标注

| 风险 | 影响 | 缓解 |
|:-----|:-----|:-----|
| **Phase 1 时序方案不成立** | 后续全部阻塞 | B1 的 P1-00 先做原型验证，失败则回退手工白名单（降级路径） |
| **ATG 接线后 realVerified 先降** | 数字变"难看" | 明确这是诚实化代价；建立"接线前 vs 后"对照基线 |
| **XPath/XSLT 是引擎级工程** | 估算可能严重偏低 | P4-00 强制先评估，不直接实现 |
| **artifacts 并发污染** | 假数字 | §5.1 红线 + 三条判据 |
| **stubGap 里藏真缺陷** | 低估工作量 | Phase 2 的 realDefect 对账 + 每次修复后重跑 |
| **flaky 误导归因** | 误报回归 | §5.2 判定纪律 |
| 未跑 chunk 的工作量未知 | 总量失真 | §4 总计已标注为不确定；Phase 5 后重估 |

---

## 8. 与 v2 的差异总览

| | v2 | v3 |
|:--|:---|:---|
| 基线 | 9/20（5 chunk） | **9/22 实测复核** |
| Phase 1 | 假绿清零 | **ATG 接线**（根因优先） |
| realDefect | 未覆盖 | **Phase 2 独立阶段（159 项）** |
| XPath/XSLT | 未单列 | **Phase 4 独立阶段** |
| stubGap 收敛 | Phase 3（实现视角） | **Phase 5（接线后的残余）** |
| Phase 0 度量 | 作为起点（1-2 会话） | **已完成**（矩阵 v2 已产出，待刷新） |
| 估算 | 17-24 会话 | **25-35 会话** |

---

## 9. 下一步入口

**Phase 1 的 P1-00（时序方案选型）需要用户确认后启动。**

启动方式：`dev-executing-plans` + 单 agent（架构决策，不走并行）
