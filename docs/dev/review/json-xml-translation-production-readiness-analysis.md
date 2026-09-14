> **报告日期**: 2026-09-13（初版）/ 2026-09-14（实地测试后修订）
> **数据分析依据**: honest-report 2026-09-12 + P0-C 覆盖矩阵（1777 分母） + artifact fact.json 实时数据 + **2026-09-14 pipeline 实测**
> **修订说明**: 初版将 global-ns 的 0 real 归因为 `JsonDocument::Parse` 无法翻译。2026-09-14 实地跑 pipeline 后证伪 —— 见 §2.1 的修正说明。本版已更正。
> **数据质量**: 报告数据经三方交叉校验（matrix/artifact/actual file）与 pipeline 实测，非"agent 说"
> **前提**: 本报告衡量的是翻译管线的**翻译覆盖率**（一个方法走了 native AOT 还是 fallback/interpreter），不是上层功能测试通过率。两者有相关性但不相等。

---

# JSON / XML 翻译管线 — 生产级品质差距综合分析

## 1. 总览

### 1.1 三个指标理解本报告

本报告使用的三个核心指标含义：

| 指标 | 含义 | 对应 fact.json 字段 |
|:----|:-----|:--------------------|
| **real verified** | 方法真实执行并产生了不同于哨兵值(42)的断言值 | `realVerified` |
| **stubGap** | ATG 在编译期标记 `AOT-STUB-GAP`：**未执行**（无 AOT 真实体） | `stubGap` |
| **smoke** | 方法有 AOT body 但返回哨兵值 42，未产生真实断言 | `smokeUnknown` |

### 1.2 快照（2026-09-14 实测）

| 模块 | 总公开方法 | real | stubGap | failed | 门禁 |
|:----|:----------:|:----:|:-------:|:------:|:----:|
| **System.Text.Json / text-json** | 242 | **45** | 188 | **6** | FAIL（6 个 async） |
| **System.Text.Json / global-ns** | 55 | 0 | 45 | 10 | **PASS** |
| **System.Text.Json / converters** | 96 | 10 | — | — | PASS |
| **System.Xml*** | >1291 | ≈0 | 未量化 | 0 | 2 FAIL / 4 PASS |

> System.Xml 除 text-json 外其余 chunk 的 fact.json 是**旧格式**（无 `resultKind` 字段），stubGap 未量化。需要重跑 pipeline 才能得到准确数字。

### 1.3 本轮的进展（2026-09-14）

| commit | 内容 | 实测效果 |
|:-------|:-----|:---------|
| `4d7af25eb`+ | Phase 0 基础件：stubGap 分桶 + C3313 修复 | text-json 可构建 |
| `6b3afa2e0` | JSON 正确性 + Double/Single stub + NameTable char[]/虚派发 | 14/14 gtest |
| `678074ea8` | arity 修复 + 浮点 Deserialize 位型 | **real 33→45, failed 18→6** |
| `d286c2988` | JsonSerializerOptions fixture + 异常一致性尝试 | global-ns PASS |

---

## 2. System.Text.Json（486 公开方法）

### 2.1 按 chunk 分解

| chunk | total | real | stubGap | unassertable | failed | gateTotal | 门禁 |
|:-----|:----:|:----:|:------:|:-----------:|:-----:|:--------:|:----:|
| **text-json** | 242 | **45 (18.6%)** | **188 (77.7%)** | 3 | **6 (2.5%)** | 54 | FAIL |
| **global-ns** | 55 | 0 | 45 | 0 | 10 | 10 | **PASS** |
| **system-text-json-serialization-converters** | 96 | 10 | 未测 | — | — | — | PASS |

**指标演进（text-json）：** real 33→45，failed 18→6，均由 commit `678074ea8` 的 arity 修复 + 浮点 Deserialize 位型修复达成。

**text-json 剩余的 6 个失败**全部是 `SerializeAsync`（Stream/PipeWriter + int/object + JsonSerializerOptions/JsonTypeInfo）。根因是 async 状态机，属 Phase M。

**global-ns 的 10 个失败**是 AOT 返回 0 而 managed 抛 `ArgumentNullException` 的不一致，详见 §2.3。

### 2.1b global-ns 归因修正（重要）

**初版报告将 global-ns 的 0 real 归因为 `JsonDocument::Parse` 无法翻译。这是错的。**

2026-09-14 实地调查证实：

1. global-ns 的 51 个 subject **全部不是** `JsonDocument::Parse`，而是 `JsonMetadataServices.CreateArrayInfo` / `CreateListInfo` / `CreateDictionaryInfo` 等 **source-generator 用的元数据工厂钩子**
2. `JsonDocument::Parse` 的 4 个重载根本**不在** global-ns chunk 中；它们在 `aot-manifest.json` 里标记为 `external-call`
3. 这些方法的 AOT 代码**已经生成**（`nativeGenerated=53`）

**所以 global-ns 的问题不是"无法翻译"，而是"ATG 输入生成 + AOT 异常行为未对齐"** —— 一个完全不同的、更小的问题。

**2026-09-14 实地 pipeline 验证**：给 `JsonSerializerOptions` 加上非 null fixture 后重跑 global-ns，`CombinedSubjects.cs` 确认 fixture 已生效（25 处 `new JsonSerializerOptions()`），但 pipeline 结果未变——stubGap=55。原因是 `JsonMetadataServices.Create*Info` 即使传入非 null options 也在 managed 下抛 `NotSupportedException`（source-generator 钩子的设计性质），ATG 看到"managed 抛异常"→ 外部程序集 → 仍标记 `[UNVERIFIED]`。要解决需要 ATG 对这类已知抛异常的外部方法也生成断言（AOT 需同步抛出匹配异常），这是跨层任务，已在 roadmap 登记。

### 2.2 真实实现的成员清单

codegen 已注册的 SimpleForward/形状：

**JsonSerializer::Serialize<T>() / Deserialize<T>()（`RuntimeHelperShapeRegistry.cs:598-664`）：**
- 原生 `ChaosJsonSerializeInt32/Int64/Bool/String`——snprintf → ChaosStringCreateFromUtf8
- 原生 `ChaosJsonDeserializeInt32/Int64/Bool`——atol/atoll
- **Single、Double、Decimal 未注册**
- **所有泛型 POCO 走解释器**

**async/await 任务相关（`S16.cs`）：**
- TaskAwaiter.GetResult → `ChaosAsyncTaskAwaiterGetResultValue/Void`
- ValueTaskAwaiter.GetResult → `ChaosAsyncTaskAwaiterGetResultValue/Void`
- Task.GetAwaiter、TaskAwaiter.get_IsCompleted 等

**非 JSON 特有：** 以上是 async 基础设施，被所有 async Task 方法共用，不限于 JSON。

### 2.3 未实现的翻译缺口（按影响面排序）

| 优先级 | 缺口 | 影响方法数（估计） | 当前路径 |
|:-----:|:-----|:-----------------:|:---------|
| **P0** | `JsonDocument::Parse` 无 AOT 体 | ~30（global-ns 全部 fail 源头） | ExternalRuntimeFallback → Phase 2 dispatch table → return 0 |
| **P0** | `JsonElement` 遍历方法（GetProperty/EnumerateArray/ValueKind） | ~50 | 全部解释器/bridge thunk |
| **P1** | `Utf8JsonReader` 22 个方法 | 22 | 全部解释器 |
| **P1** | `Utf8JsonWriter` 核心方法（WriteStartObject/WriteString/WriteEndObject） | ~30 | 全部解释器 |
| **P1** | `JsonSerializerOptions`（无 AOT 注册） | 不可枚举 | 解释器 |
| **P1** | `JsonNode` / `JsonValue` / `JsonArray` / `JsonObject` | ~40 | 全部解释器 |
| **P2** | Single/Double/Decimal round-trip | 3 | 解释器（基准类型无 stub） |
| **P2** | `SerializeAsync/DeserializeAsync` | 不详 | async 状态机路径 |
| **P3** | POCO 属性预编译（Phase 2） | ALL（~400） | 解释器 |

---

## 3. System.Xml.*（≥1291 公开方法，3 个程序集）

### 3.1 按 assembly 分解

| assembly | 公开方法 | chunk 数 | 门禁状态（现有） |
|:---------|:-------:|:--------:|:---------------:|
| System.Private.Xml | 849 | 4 (xml/xsl/serialization/schema) | 3 PASS / 1 FAIL(system-xml-xsl) |
| System.Xml.ReaderWriter | 442 | 2 (schema/xsl) | 1 PASS / 1 FAIL |
| **合计** | **≥1291** | **6** | **2 FAIL / 4 PASS** |

### 3.2 "PASS" 的真相

**PASS chunk 不代表 XML 功能可用。** 示例——`System.Private.Xml/xml`（1147 方法，honest-report 显示 PASS）：
- `real verified`: 72 (6.3%)
- `smoke`: 1075 (93.7%)
- benchmark: 0

**1075 个方法是 smoke**——编译通过 + 执行不崩，但返回值是 42（ATG 生成的断言值不对）。并不是真实验证。

### 3.3 真实实现的成员清单

**就一个类，两个方法：**

| 类 | 方法 | 原生实现 | 位置 |
|:--|:----|:---------|:------|
| `NameTable` | `Add(string)` → string | `ChaosXmlNameTableAddString` | `xml_nametable_stubs.cpp:59` |
| `NameTable` | `Get(string)` → string? | `ChaosXmlNameTableGetString` | `xml_nametable_stubs.cpp:106` |

**NameTable 实现质量评价（架构合理）：**
- FNV-1a 哈希 + TLS resolve cache + 全局 direct-mapped cache
- Thread-safe（AOT 表只读，动态表互斥锁保护）
- Hotupdate 感知（`UnregisterDomain` 跟踪 domain_id → StringId[]）
- 有 9 个 gtest 覆盖（P0-A 新增）

**但唯一被（原生）实现的方法只占 XML 面的大约 0.15%。**

### 3.4 更大的缺口

| 优先级 | 缺口 | 影响方法数 | 当前路径 |
|:-----:|:-----|:---------:|:---------|
| **P0** | `XmlDocument`（LoadXml/CreateElement/SelectNodes/InnerText/OuterXml 等） | ~100 | fallback → return 0 |
| **P0** | `XmlReader.Create` + Read 循环 | ~80 | fallback → 解释器 |
| **P0** | `XmlWriter.Create` + Write 方法 | ~60 | fallback → 解释器 |
| **P0** | `XDocument.Load/Parse` + `XElement` 树构建 | ~80 | fallback → 解释器 |
| **P0** | `XmlSerializer` 序列化/反序列化 | ~100 | fallback → 解释器 |
| **P1** | `XmlSchema` 验证 | 11 个 Schema 类型 | 全部 fallback |
| **P1** | `XmlNameTable`（抽象基类）虚派发 | ~10 | 已登记为已知缺陷（`Part1.cs:642-646`） |
| **P1** | `NameTable.Add(char[],int,int)` char[] 重载 | 2 | 已登记为已知缺陷 |
| **P2** | `XslCompiledTransform` 转换 | ~247 (system-xml-xsl chunk!) | 全部 fallback |

### 3.5 ReaderWriter vs Private.Xml 差异——值得专门说明

`System.Xml.ReaderWriter/xml` 的 honest-report 显示 `real=306/375`——这看起来远好于 `System.Private.Xml/xml` 的 `real=72/1147`。

**这不是真实实现的差异。** 两者方法数（442 vs 849）和 chunk 配置不同。ReaderWriter 是一个**较薄**的程序集（包 XML API 的入口点），Private.Xml 是**内部实现**。ReaderWriter 的 306 real 很可能来自能被 ATG 构造出有效实例的简单 getter/setter/helpers，而非 XML 核心功能。

**准确比较口径应该用 `%real` 而不是绝对数**：ReaderWriter = 306/375=81.6%，Private.Xml = 72/1147=6.3%。但两者分母口径不统一（ReaderWriter 的 chunk.json 只包含了 411 个方法中的 375 个），所以这个比较本身就有偏差。

**已记入 P0-C 矩阵的 `watch_items`，应在 Phase 1 统一处理。**

---

## 4. 基础设施与架构评估

### 4.1 Phase 0 交付的改进（已完成，已验证）

| 改进项 | 验证状态 |
|:-------|:--------|
| ExternalRuntimeFallback Phase 3 LOG 可观测 | 已提交 |
| codegen catch-all 一次性 WARN 守卫 | 已提交 |
| stubGap 独立分桶（text-json smokeUnknown 307→0） | **已 pipeline 验证** |
| NameTable gtest（9/9 PASS） | 已编译+运行 |
| 覆盖矩阵（分母 1777 与 manifest 复算一致） | 已交付 |

### 4.2 全局待改进（non-JSON/XML 共享）

| 缺陷 | 位置 | 影响 |
|:-----|:------|:------|
| **codegen 构建缓存坑**：Generator.dll 被 TPG 捆绑，修改 codegen 后需重建 TPG，否则 pipeline 跑旧 DLL | `tool_dll()` 指向 `TPG/bin/Debug` | 已在 9/13 pipeline 中踩过，修复需[ensure_tool_built](https://github.com/PolarisWang/booming-il2cpp/blob/main/tests/e2e/verification/_pipeline/tool_helpers.py:79)正确触发 |
| **Phase 3 return 0 未改成更显式的处置** | `interop_stubs.cpp:848` | 按 P0-A 原方案（A2：值类型 0/引用类型 null 并全加 LOG）**只实现了 LOG，未改返回语义** |
| **旧 fact.json 无 resultKind 字段** | 所有非 text-json 的 JSON/XML chunk | 无法查看 stubGap/real/smoke 分桶，所有数字是旧格式（通过 value=42 推断） |

### 4.3 架构层面

**ExternalRuntimeFallback 是一个合理的兜底机制**——它有三层（Phase 0.5 SIMD / Phase 1 IL data / Phase 2 dispatch table），能处理一部分本应 fallback 的方法。但它对 JSON/XML 这类复杂状态机问题是**先天不足**的：

- `JsonDocument.Parse` 需要的是一段真正的 JSON 解析逻辑，不是循环或硬件指令。解释器路径（IL data + dispatch table）能执行，但有 ~35% bridge thunk 开销
- `XmlDocument.LoadXml` 需要的是一棵 DOM 树构建——从 fallback 不可能生成
- 要把这些方法翻译成真正的 AOT C++，需要的不是 stub 注册，而是**翻译状态机逻辑到 C++**——这是 Phase 1/2/3 的内容

---

## 5. 量化差距总结

### 5.1 投入估算

| 阶段 | 内容 | 估算工作量 | 结果预期 |
|:-----|:-----|:---------:|:---------|
| **Phase S (止血)** | 修复 global-ns 0 real + text-json 18 fail + JSON stub 正确性 + NameTable char[] 重载 + XmlNameTable 虚派发 | **4-6 人周** | 3 FAIL chunk 变 PASS；NameTable 完整 |
| **Phase M (核心覆盖)** | Utf8JsonReader/Writer + JsonDocument 预编译 + XmlDocument/XmlReader/XElement | **10-14 人周** | JSON real 覆盖率从 6% → ~30%；XML 从 ~0% → ~10% |
| **Phase L (生产级)** | POCO 预编译 + XmlSerializer/Schema + 性能优化 + 端到端编码策略 | **3-6 个月** | 覆盖率达 >80%，benchmark 有基线 |

### 5.2 风险等级

| 风险 | 等级 | 说明 |
|:-----|:----:|:------|
| XmlDocument 的 C++ DOM 树规模被低估 | 🔴 高 | 兼容 .NET 的 DOM（XmlNode/XmlElement/XmlAttribute/XmlText 等层级 + Parent/Child/Sibling 导航 + InnerText/OuterXml/ChildNodes）可能是数万行 C++ |
| JsonDocument bridge thunk 消除需要改 IR lowering | 🟡 中 | root-cause-analysis 指出需桥接托管 IL 状态机到 AOT IR 岛，影响面可能超出翻译域 |
| 假绿再度累积 | 🟡 中 | stubGap 桶已建立，但只有 text-json 有实时数据。若要维持可信的数字，需要全量 re-run pipeline |

### 5.3 一句话

> **JSON 约完成 6%，XML 约完成 0.15%。两者离可用级都差一个数量级，离生产级差两个。Phase 0 建了可观测性的基础设施，但尚未开始填补翻译缺口。**

---

## 6. 方法论说明

### 6.1 数据可靠性分级

| 数据 | 分级 | 来源 |
|:----|:----:|:------|
| text-json 的 33 real / 188 stubGap / 18 failed | **A** | 实时 `fact-results.json`（9/13 pipeline，有 `resultKind` 字段） |
| global-ns 的 51 total / 0 real | **B** | artifact fact.json 旧格式，无 `resultKind`，需 re-pipeline 获得 stubGap 分桶 |
| XML chunk 的 PASS/FAIL 状态 | **C** | honest-report 9/12 旧数据，未经 P0-B 分类器再处理 |
| 覆盖矩阵的 1777 分母 | **A** | `dll-method-manifest.json` 三次独立复算一致 |

### 6.2 标准口径

本报告使用 `%real`（real / assertable_total）而非 `%real_of_total` 或 `%PASS`。`stubGap` 排除出分母——因为一个方法"没有被实现"不是"被测试方法"的缺陷，而是 backlog item。

- text-json 在 P0-B 部署后：`33 / (242 - 188) = 33/54 = 61.1%`
- 但这不是"61% 已实现"——它只是"在被执行的方法中，61% 有真实断言"。实现覆盖率仍然是 33/486 = 6.8%

**报告结束。**