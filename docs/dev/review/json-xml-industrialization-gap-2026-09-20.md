# JSON / XML 翻译层 — 工业化品质差距综合分析报告

> 报告日期：2026-09-20
> 数据来源：**全部 chunk 于本日重跑**（非历史数据快照）
> 范围：System.Private.Xml（3 chunk）+ System.Text.Json（3 chunk，其中 1 个跑失败）
> 上一版报告：`json-xml-translation-production-readiness.md`（2026-09-13）

---

## 0. 一句话结论

**JSON/XML 翻译层当前处于「通道已打通、覆盖远未达标」阶段。**
以 `realVerified`（真正执行并通过断言）衡量，5 个可跑 chunk 合计 **213/1478 = 14.4%**；
**51.2% 的 subject 是 `stubGap`（native 未实现）**。
**距离工业化（可交付给业务方使用）仍有显著差距，预计需要 12–18 个会话量级的工作。**

---

## 1. 本报告的方法论与一个必须说明的前提

### 1.1 为什么必须重跑

上手时发现 `artifacts/` 下的数据新鲜度差异极大（9/10 到 9/20）。**抽查发现陈旧数据会给出完全错误的结论**：

| chunk | 陈旧数据（9/10） | 本日重跑 |
|:------|:----------------:|:--------:|
| `system-xml-xsl` | `passed=275`, **`stubGap=0`** | `total=251`, **`stubGap=246`** |
| `system-xml-serialization` | `passed=156`, `realTotal=66`, `stubGap=0` | `realVerified=63`, **`stubGap=68`** |

陈旧数据显示「stubGap=0，全部实现」，重跑后真相是 **98% 未实现**。
**本报告全部数字来自本日重跑。**

### 1.2 三个必须知道的口径陷阱

#### 陷阱 1：`passed` 不是品质指标

```
system-xml-xsl:  total=251  passed=251  stubGap=246  gatePassed=5/5
                 ↑ "251 全过"       ↑ 但 246 个是未实现的 stubGap
```

**`passed` 包含未实现的 stub**。`stubGap` 的 subject 返回 ATG 写死的 `42L`，从不调用 native，
却被计入 `passed`。**用 `passed` 汇报进度会严重虚高。**

真正有意义的是 **`realVerified`**（实际执行 + 断言通过）。

#### 陷阱 2：`gateTotal` 分母口径不一致

```
text-json:  gatePassed=185  gateTotal=8     ← 分子是分母的 23 倍
```

（9/17 数据；重跑后为 `32/35`，正常）。**该字段曾出现分子 > 分母的失效状态**，
说明门禁口径本身经历过修正。引用历史数据时需警惕。

#### 陷阱 3：AOT 与 JIT 记录分离

`fact-results.json` 有 `aot` / `jit` 两个独立数组。曾出现 **`aot=0` 而 `jit=275`**
的 chunk（`system-xml-xsl` 陈旧快照），而 `fact.total` 仍报 275。
**AOT 侧零执行却报通过** —— 引用数据时必须核对 `aot` 数组的实际长度。

---

## 2. 实测数据（全部本日重跑）

### 2.1 总览

| chunk | total | **realVerified** | stubGap | unassertable | failed | gate |
|:------|:-----:|:----------------:|:-------:|:------------:|:------:|:----:|
| Xml/xml | 746 | **136** (18.2%) | 185 | 173 | 96 | 405/526 |
| Xml/system-xml-serialization | 138 | **63** (45.7%) | 68 | 3 | 4 | 66/70 |
| Xml/system-xml-xsl | 251 | **4** (1.6%) | 246 | 1 | 0 | 5/5 |
| Json/text-json | 247 | **2** (0.8%) | 177 | 30 | 3 | 32/35 |
| Json/serialization-converters | 96 | **8** (8.3%) | 80 | 6 | 2 | 14/16 |
| Json/global-ns | — | — | — | — | — | **跑失败**（见 §5.1） |
| **合计（5 chunk）** | **1478** | **213 (14.4%)** | **756 (51.2%)** | 213 | 105 | 522/652 |

### 2.2 分母：API 总量

| Assembly | public method 总数 | chunk 数 |
|:---------|:------------------:|:--------:|
| System.Private.Xml | 849 | 4（另 1 个未跑） |
| System.Text.Json | 486 | 3 |
| System.Xml.ReaderWriter | 442 | 2（**本报告未覆盖**） |
| **合计** | **1777** | — |

⚠️ **chunk 的 subject 数（1478）与 API 数（1777）不是同一口径** ——
chunk 按「闭包内可达方法 × 泛型实例化」展开，一个 API 可能对应多个 subject。
**不能用 1478/1777 直接算覆盖率。**

### 2.3 既有 API 矩阵不可用

`docs/dev/in-progress/json-xml-production-readiness/api-coverage-matrix.json`（9/13 生成）：

```json
{"total": 1777, "real": 0, "notSupported": 0, "notApplicable": 0,
 "fakeGreen": 0, "unclassified": 1777}
```

**100% 条目为 `unclassified`，不提供任何有效信息。**
该报告自身也承认「real ~0（未复核，含 P0-B 假绿）」。

**结论：目前没有可信的 API 级覆盖数据，只有 subject 级 fact 数据。**

---

## 3. 分模块品质评估

### 3.1 System.Private.Xml / xml（主 chunk）—— **最有进展**

```
total=746  realVerified=136  stubGap=185  unassertable=173  failed=96
```

- **XML 同步写面**（XmlTextWriter）、**XmlConvert**、**NameTable** 有实质 native 实现
- 本日刚修复 **ctor 句柄 ABI**（`5184a0e59`），使 `XmlTextWriter`/`XmlTextReader`
  的 subject 真正执行，realVerified 提升
- **96 个 failed** + **173 个 unassertable** 是当前主要构成
  - `unassertable` 多为 `void` 返回方法，ATG 结构性无法断言（非缺陷）
  - `failed` 需逐个对账（**本报告未逐一对账**）

### 3.2 System.Xml / system-xml-serialization —— **部分可用**

```
total=138  realVerified=63 (45.7%)  stubGap=68  failed=4
```

**这是 5 个 chunk 中 realVerified 占比最高的**（45.7%）。
尚有 68 个 stubGap 待实现。

### 3.3 System.Xml / system-xml-xsl —— **几乎空白**

```
total=251  realVerified=4 (1.6%)  stubGap=246 (98%)
```

**XSLT 整体未实现。** 这是 XML 侧最大的空洞。

### 3.4 System.Text.Json / text-json —— **核心读取面未实现**

```
total=247  realVerified=2 (0.8%)  stubGap=177  unassertable=30  failed=3
```

**0.8% 的 realVerified 是全部 chunk 中最低的之一。**
`Utf8JsonReader` / `JsonDocument` 等核心 API 基本未实现（与 9/13 报告的判断一致）。

### 3.5 System.Text.Json / serialization-converters

```
total=96  realVerified=8 (8.3%)  stubGap=80
```

---

## 4. 距工业化的工作量评估

### 4.1 什么是「工业化」

参考本仓库其他已工业化子系统（如 `reflection-industrialization`、`threading-production-readiness`）
的验收口径，JSON/XML 达到工业化至少需要：

| 维度 | 当前状态 | 工业化要求 |
|:-----|:---------|:-----------|
| **核心 API 覆盖** | 14.4% realVerified | 核心路径 >80%，全量 >60% |
| **API 矩阵** | **100% unclassified** | 逐项分类（real / stub / N/A） |
| **语义正确性** | 部分 API 与 .NET 8 有差异 | 与 .NET 8 逐项对齐 |
| **性能** | 未测 | Benchmark 基线 + 加速比数据 |
| **异常语义** | 多处未对齐（本日刚修 3 处） | 异常类型/消息逐项对齐 |
| **HotUpdate** | 未评估 | 翻译路径热更友好 |

### 4.2 工作量估算

按「实现 + 验证 + 对账」三件套估：

| 工作项 | 规模 | 估算 |
|:-------|:-----|:-----|
| **P0 核心 API 补齐**（Utf8JsonReader/Writer、JsonDocument/Element、XmlReader 全族） | ~300 subject | **4-6 会话** |
| **stubGap 清零**（756 个） | 756 subject | **6-8 会话**（部分随 P0 消解） |
| **failed 逐项对账**（105 个） | 105 subject | **2-3 会话** |
| **API 矩阵重建**（1777 项分类） | 工具 + 人工复核 | **1-2 会话** |
| **语义对齐**（异常/边界） | 未知（需先建矩阵） | **2-4 会话** |
| **性能基线** | 未开始 | **1-2 会话** |
| **其余 chunk**（system-xml-schema / xsl-runtime / ReaderWriter×2） | 未跑过 | **2-3 会话** |
| **合计** | | **18–28 会话** |

**取中位数：约 22 个会话量级**（含返工余量）。

### 4.3 优先级建议

1. **先建 API 矩阵**（1-2 会话）—— 当前 100% unclassified，**没有它就无法度量进度**，
   所有后续估算都是盲猜
2. **P0 核心 API**（4-6 会话）—— 决定"能不能用"，收益最大
3. **failed 对账**（2-3 会话）—— 可能藏有真缺陷
4. 其余按矩阵指引推进

---

## 5. 本报告未能覆盖 / 存疑的部分（诚实标注）

### 5.1 global-ns chunk 跑失败（未归因）

```
[CODEGEN_GENERATE_FAILED] Code generation failed: An item with the same key has already been added.
Key: Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)
```

- 来源：`LoaderStage.CrossAssemblyInstantiation.cs:39-40` 的
  `assembly.Methods.ToDictionary(method => method.SubjectId, ...)` ——
  **同一 assembly 内出现重复 SubjectId**
- **已排除与本次 GC/ctor 修复相关**：未触碰 `Loader`，
  `AssertionException` 不在任何 shape 注册中
- ⚠️ **未做干净树对照验证**，因此这是**基于代码路径的排除**，非实测结论
- 疑似与并行 agent 的近期改动有关（该 chunk provenance 记录 commit `4c42dfdd`）

### 5.2 未覆盖的 chunk

- `System.Private.Xml/system-xml-schema`（配置存在，**从未跑过**）
- `System.Private.Xml/system-xml-xsl-runtime`（同上）
- `System.Xml.ReaderWriter` 的 2 个 chunk（**完全未纳入本报告**）

### 5.3 未做的分析

- **未逐一对账 105 个 `failed`** —— 其中可能有真缺陷
- **未评估性能** —— 无任何 benchmark 数据
- **未评估 HotUpdate 影响**
- **未验证 `unassertable` 是否真的"结构性无法断言"** —— 可能掩盖问题

### 5.4 与上一版报告的口径差异

上一版（9/13）结论为「JSON ~5-8%，XML ~0.5-2%」，基于当时的 chunk PASS/FAIL 口径。
本报告改用 `realVerified` 口径，**两者不可直接比较**。
但方向一致：**覆盖度远未达标**。

---

## 6. 附：复现命令

```bash
# 同步 SDK（必须先做，否则 pipeline 跑旧 lib）
python tests/e2e/translation/artifacts/build_presets.py --preset windows-x64-reference

# 跑单个 chunk
CHAOS_FOUNDATION_DLL=$(pwd)/testing/foundation-dll PYTHONPATH=$(pwd)/tests/e2e \
  python tests/e2e/verification/chunk_pipeline.py \
  --assembly <Assembly> --chunk <chunk> --stages build,fact --native-config check

# 关键指标（不要看 passed）
python -c "
import json,io
d=json.load(io.open('artifacts/foundation-dll/<A>/chunks/<c>/results/fact.json',encoding='utf-8'))
for k in ['total','realVerified','stubGap','unassertable','failed','gatePassed','gateTotal']:
    print(k,'=',d.get(k))
"
```
