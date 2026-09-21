# JSON / XML API 覆盖矩阵 v2 — Phase 0 交付

> 日期：2026-09-20
> 生成器：`tools/build_api_coverage_matrix.py`
> 产物：`api-coverage-matrix-v2.json`

---

## 1. 结论

**分母已建立（1603 个方法重载），矩阵从 100% unclassified 降到 0% unclassified。**

| 档位（按方法重载数） | 数量 | 占比 |
|:---------------------|:----:|:----:|
| **real**（真实实现且验证通过） | **238** | **14.8%** |
| not-covered（分母有、无任何测试） | 652 | 40.7% |
| unassertable（void/Task 返回，ATG 无法断言） | 211 | 13.2% |
| stub（ATG 写死 42L，未调 native） | 202 | 12.6% |
| failed | 122 | 7.6% |
| smoke（返回 42 但无断言） | 98 | 6.1% |
| factory-gap | 70 | 4.4% |
| **fake-green**（执行了但与期望不符） | **10** | 0.6% |
| **合计** | **1603** | 100% |

**`real` 占比 14.8%** —— 与本日实测的 `realVerified 213/1478 = 14.4%` **同量级**，
两个独立口径互相印证。

---

## 2. 分母口径（用户拍板）

| Assembly | 来源 | methods |
|:---------|:-----|:-------:|
| System.Text.Json | **ref-pack 8.0.11**（纯 public） | 312 |
| System.Xml.ReaderWriter | **ref-pack 8.0.11**（纯 public） | 442 |
| System.Private.Xml | **shared 8.0.11**（ref-pack 无此程序集） | 849 |
| **合计** | | **1603** |

### 为什么不用仓库既有的 manifest

| 来源 | methods | types | 含 `JsonSerializer` | 类型性质 |
|:-----|:-------:|:-----:|:-------------------:|:---------|
| 仓库 manifest | 486 | 99 | ❌ | 混大量 internal |
| **ref-pack 8.0.11** | **312** | **17** | ✅ | **纯 public** |

**量化证据**：用仓库 manifest 作分母时，text-json 有 **103/247 = 42%** 的测试对象
无法映射（都指向它缺失的 `JsonSerializer`）。

---

## 3. 已知局限（**必须随产物一起引用**）

### 3.1 映射是方法名级，覆盖率为**下限估计**

测试方法 `JsonDocumentTests::Parse_7_string_X_0` → API `(System.Text.Json.JsonDocument, Parse)`。
**不含参数** —— 「该类型的该方法有任一变体被覆盖」即算命中。

⚠️ **一个方法的多个重载可能只有部分被覆盖，但矩阵显示为整体覆盖。**

### 3.2 分布高度不均，禁止用「类型覆盖率」

`ref-pack` 的 19 个类型对应 312 个方法（平均 16 个/类型），
`Utf8JsonWriter` **单类型约 100 个重载**。

**覆盖 1 个类型可能 = 覆盖 30% 的方法** —— 报告必须用**方法级**覆盖率。

### 3.3 `System.Private.Xml` 不在 ref-pack 中

它是**实现程序集**，其公开面在 ref-pack 里以 facade（`System.Xml.ReaderWriter` 等）
形式暴露。本矩阵对其采用 **shared DLL** 作分母（已验证无 internal 混入）。

### 3.4 分子只覆盖已跑过 chunk 的范围

`not-covered 652`（40.7%）**不代表未实现** —— 其中大部分可能是：
- 从未生成过测试的 API（`system-xml-schema` / `xsl-runtime` / ReaderWriter 的 2 个 chunk 未跑）
- 或 ATG 未覆盖的 API

**不能把 `not-covered` 直接读作「未实现」。**

---

## 4. 生成器实现要点（供后续维护）

文件：`tools/build_api_coverage_matrix.py`

### 4.1 测试方法名规范化（踩过坑）

```
System_Private_Xml.System_Xml_NameTable  ->  System.Xml.NameTable
       ↑ 第一段是 assembly（下划线形式）      ↑ 其余下划线转点
```

**早期实现用「去重复段」启发式，会产出 `System.Private.Xml.System.Xml.NameTable`
（多一层前缀）** —— 已废弃，改用「按首个 `.` 切分丢弃 assembly 段」。

### 4.2 两套统计口径（都保留）

| 口径 | 含义 | 用途 |
|:-----|:-----|:-----|
| `verdictCounts` | 按**方法重载数** | 分母口径，算覆盖率 |
| `verdictCountsByFactRecord` | 按 **fact 记录数** | 与 `realVerified` 可比 |

**两者数字不同是正常的**（一个 API 方法对应多条 fact 记录时）。

---

## 5. 对 Roadmap 的影响

Phase 0 原计划「从 manifest 聚合建矩阵」**已修正为**「ref-pack + shared 混合分母」。

**后续阶段的估算维持不变**（17-24 会话）—— 因为 `real 14.8%` 与新基线一致，
没有推翻原判断。

**但 `not-covered 652`（40.7%）是一个新信息**：它意味着**分母中有四成 API
从未进入验证范围**。这可能低估了工作量，需在 Phase 4（补未跑 chunk）时重新评估。
