CLAUDE.md 必须遵守。本报告只读，不修改任何文件。

# JSON 与 XML 翻译管线 — 生产级品质差距综合分析报告

> 报告日期：2026-09-13  
> 范围：System.Text.Json + System.Xml.*（含 System.Private.Xml、System.Xml.ReaderWriter、System.Xml.Linq）  
> 分析维度：实现覆盖 / 测试验证 / 性能 / 内存安全 / 热更新与反射 / 架构合规  
> 数据来源：源码审计 + 代码级搜索 + 每日构建报告 + gap-analysis.md + 设计文档

---

## 目录

1. [概要 — 当前状态总览](#1-概要--当前状态总览)
2. [JSON 翻译管线详情](#2-json-翻译管线详情)
3. [XML 翻译管线详情](#3-xml-翻译管线详情)
4. [共享基础架构：ExternalRuntimeFallback 兜底链](#4-共享基础架构-externalruntimefallback-兜底链)
5. [测试与验证分析](#5-测试与验证分析)
6. [达到生产级品质所需的工作项](#6-达到生产级品质所需的工作项)
7. [优先级与路线图建议](#7-优先级与路线图建议)

---

## 1. 概要 — 当前状态总览

### 1.1 一句话总结

| 模块 | 真实实现的方法数 | 总暴露方法估计 | 覆盖率 | 通过 chunk | FAIL chunk |
|------|:---:|:---:|:---:|:---:|:---:|
| **System.Text.Json** | ~17（基本类型的 Serialize/Deserialize） | 500+ | ≈3% | 1/3 | **2/3** |
| **System.Xml.*** | 2（NameTable.Add/Get） | 1000+ | <0.2% | 3/5 | **2/5** |

两个模块都处于 **早期原型阶段**，远未达到生产级。JSON 略好（至少基本类型 stub 有了），XML 几乎完全未实现（只有 NameTable）。

两个模块的公共特征是：**绝大部分 API 通过 ChaosExternalRuntimeFallback 返回 0/null，在 AOT 模式下不产生功能性输出**。

### 1.2 每日构建验证快照（2026-09-12）

来源：`tests/e2e/nightly-build-report/honest-report-20260912_141257.txt`

```
                                tot  nom   real unassert smoke fail   %real  result
System.Text.Json/global-ns      34   34    0        0     34    0   0.0%   FAIL
System.Text.Json/text-json     312  311    4        0    307    1   1.3%   FAIL
System.Text.Json/...converters  96   96    10       0     86    0  10.4%   PASS
System.Private.Xml/xml        1147 1147    72       0   1075    0   6.3%   PASS
System.Private.Xml/...xml-xsl  250  250     2        0    248    0   0.8%   FAIL
System.Xml.ReaderWriter/xml    411  375   306       0     69   36  74.5%   PASS
System.Xml.ReaderWriter/...schema 51  51   49        0      2    0  96.1%   PASS
```

关键观察：
- **real verified = 真正的 AOT 实现验证**。JSON 三个 chunk 只有 14 个 real（共 442 个方法），XML 五个 chunk 有 429 个 real（但 306 个来自 `System.Xml.ReaderWriter/xml`，其中 36 个 fail）
- **绝大部分是 smoke（编译成功 + 执行不崩）**，不验证功能正确性
- "PASS" 不意味着功能完整，只意味着 smoke 级别的门禁通过

---

## 2. JSON 翻译管线详情

### 2.1 已实现的部分

**Codegen 层（C# `RuntimeHelperShapeRegistry.cs:598-664`）：**
- `RegisterJsonSerialize(typeArg, nativeFn)` / `RegisterJsonDeserialize(typeArg, nativeFn)` — 两个注册辅助方法
- 已注册的 10 个基本类型 Serialize（Int32/Int64/Int16/Byte/SByte/UInt16/UInt32/UInt64/Boolean/String）+ 3 个 Deserialize（Int32/Int64/Boolean）

**Native 层（`interop_stubs.cpp:119-177`）：**
- `ChaosJsonSerializeInt32/Int64/Bool/String` — snprintf 序列化为 JSON 字符串
- `ChaosJsonDeserializeInt32/Int64/Bool` — atol/atoll 反解析
- `ChaosStringCreateFromUtf8` — 辅助函数

**工具层（`src/native/common/chaos/json_reader.h` — 425 行单头文件）：**
- 迷你 JSON 解析器，用于 AOT Core IR 反序列化
- 支持：object/array/string/number/null/true/false
- **明确声明**：非通用 JSON 解析器，不支持 UTF-16 surrogates / Unicode 转义

### 2.2 未实现的关键缺口

| 类别 | 具体缺口 | 影响 | 严重度 |
|------|---------|------|:------:|
| **`JsonSerializer<T>` 非基本类型** | Single/Double/Decimal 未注册；POCO 无预编译 | 大量 JSON 序列化场景不工作 | P0 |
| **`JsonDocument`** | `Parse` 走 bridge thunk → 解释器，35% 桥接开销 | text-json chunk FAIL（root cause） | P0 |
| **`JsonElement`** | 所有方法（GetProperty/EnumerateArray/ValueKind 等）走 fallback | 无法遍历 JSON 树 | P1 |
| **`Utf8JsonReader`** | 22 个 method 被 coverage-audit 标记 MISSING | 低层 JSON 读取性能全损 | P1 |
| **`Utf8JsonWriter`** | 全部 fallback → 解释器 | JSON 输出性能全损 | P1 |
| **`JsonSerializerOptions`** | 无 AOT 路由 | 所有自定义选项不生效 | P2 |
| **`JsonConverter<T>`** | 无预编译 | 自定义转换器走解释器 | P2 |
| **`JsonTypeInfo<T>` / `JsonMetadataServices.Create*Info<T>`** | 需要完整对象图，ATG 无法生成 | global-ns chunk FAIL（51 smoke 无法执行） | P0 |
| **`SerializerAsync/DeserializeAsync`** | 保留解释器路径 | async JSON 场景无 AOT 加速 | P2 |

### 2.3 已知缺陷（白盒分类）

**P0（功能阻断）：**
1. `ChaosJsonSerializeString` 不做引号包裹 + 不做内部转义 (`interop_stubs.cpp:143-149`) — 返回无效 JSON
2. `ChaosJsonDeserializeInt32/Int64/Bool` 无 JSON 语法校验，接受 `"42abc"` 等非标准输入 (`interop_stubs.cpp:155-177`)
3. 无符号 32/64 位类型仅注册 Serialize，无 Deserialize 注册 (`RuntimeHelperShapeRegistry.cs`)
4. `text-json` chunk 的 `JsonDocument.Parse` 在 external-runtime stub 中无法复现 `JsonReaderException`（导致 307 smoke 只能判断编译通过，1 fail 是真正异常未匹配）

**P1（严重质量）：**
5. Bridge thunk 35% 额外开销 (`root-cause-analysis.md:33`)
6. L2 BridgeMethodResolver 设计存在但代码不存在 (`l2-implementation-plan.md`)
7. `json_reader.h` 无 UTF-16 surrogates / `\uXXXX` 支持；用 `CHAOS_IL2CPP_MALLOC` 分配需显式释放（内存管理易出错）

---

## 3. XML 翻译管线详情

### 3.1 已实现的部分

**就两个方法（`RuntimeHelperShapeRegistry.CoreStubs.Part1.cs:652-688`）：**

| 托管方法 | 原生函数 | 实现情况 |
|----------|---------|---------|
| `NameTable.Add(string)` | `ChaosXmlNameTableAddString` | 完整实现：`string_table::Intern` 去重 → 返回稳定 StringId |
| `NameTable.Get(string)` | `ChaosXmlNameTableGetString` | 完整实现：`string_table::Find` 查询 → 不存在返回 0 |

**NameTable 实现的架构评价（正面）：**
- 使用 `string_table` 的 FNV-1a 哈希 + TLS 缓存（无锁查询路径）+ 互斥锁保护动态映射
- 空字符串 `""` 手动匹配编译器的 `ldstr ""` 哈希值，保证引用相等性
- Hotupdate 域卸载支持：跟踪 `domain_id → StringId[]` 映射，卸载时释放 `malloc` 内存

### 3.2 未实现的关键缺口

| 类别 | 具体缺口 | 影响 | 严重度 |
|------|---------|------|:------:|
| **XmlDocument** | 所有方法（LoadXml/CreateElement/SelectNodes 等）fallback → 0 | XML DOM 完全不可用 | P0 |
| **XmlElement / XmlNode / XmlAttribute** | 所有成员 fallback → null | XML 树遍历完全不可用 | P0 |
| **XmlReader / XmlWriter** | 所有方法 fallback → 解释器 | XML 读写完全不可用 | P0 |
| **XmlSerializer** | 全部 fallback → return-0 | 序列化/反序列化完全不可用 | P0 |
| **XDocument / XElement (LINQ to XML)** | LTX 整个命名空间无 stub | LINQ XML 查询完全不可用 | P0 |
| **XmlSchema** | 11 个 Schema 类型全部 fallback | 无 XML Schema 验证 | P1 |
| **XslTransform** | XSLT 类型全部 fallback | 无 XSL 转换 | P2 |
| **抽象基类 XmlNameTable** | 虚派发不走注册的 NameTable 具体类型 | 通过抽象类型引用的调用走 fallback | P1 |

### 3.3 已知缺陷（白盒分类）

**P0（功能阻断）：**
1. 整个 System.Xml 命名空间（>1000 个方法）只有 2 个真实实现，其余全部返回 0/null
2. 抽象基类 `XmlNameTable.Add/Get` 的虚派发不走注册 (`Part1.cs:642-646` 注释承认)
3. `NameTable.Add(char[], int, int)` / `Get(char[], int, int)` char[] 重载未注册

**P1（严重质量）：**
4. 没有针对 `ChaosXmlNameTableAddString/GetString` 的单元功能测试（只有 probe smoke 测试）
5. `gap-analysis.md` 报告 XML chunk 无 subject（"no entry.exe found" / "no topics"）
6. 无 UTF-8/UTF-16 端到端编码策略文档

**P2（设计取舍）：**
7. NameTable interned 字符串 `CHAOS_IL2CPP_MALLOC` 分配，GC 永不回收（除非 hotupdate 域卸载）

---

## 4. 共享基础架构：ExternalRuntimeFallback 兜底链

两个模块共享同一个 catch-all 机制 (`NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs:656-698` + `interop_stubs.cpp:688-848`)。

```
未注册的方法调用
  ↓
codegen TryCreateExternalRuntimeHelperDefinition()
  ├── ShapeRegistry 匹配 → 直接生成 native wrapper ✓
  └── 不匹配 → Hybrid fallback:
        ├── Phase 0.5: SIMD 字符串匹配（仅 Vector 类）
        ├── Phase 1: IL data 解释器执行（kChaosExternalRuntimeIlData[]）
        ├── Phase 2: 外部运行时派发表（kChaosExternalRuntimeSubjects[]）
        └── Phase 3: return 0（静默吞掉）
```

**这个 catch-all 的设计缺陷：**
- Phase 3 的 `return 0` 对 JSON/XML 来说是 **静默数据损坏**（不是 crash，是错误结果）
- 它原本用于 SIMD 和 crypto 这样有循环/硬件指令的方法，JSON/XML 的复杂状态机逻辑直接返回 0 完全不正确
- `XmlDocument.LoadXml()` 返回 void（没有返回值），fallback 生成的是空函数体——既不做任何事情，也不报错

---

## 5. 测试与验证分析

### 5.1 Probe Tests 的局限性

ATG 生成的 probe 测试有一个根本性问题（`TestEmitter.cs:234-260`）：

```csharp
if (hasException && isExternalAssembly) {
    // 不执行调用，返回 42L 标记 [UNVERIFIED]
    return 42L;
}
```

这就是为什么：
- JSON chunk 有 307+34+86=427 个 smoke 但只有 14 个 real
- XML chunk 有 1075+248+69+2 = 1394 个 smoke 但有 429 个 real（其中 306 来自 ReaderWriter/xml）
- "smoke PASS" 只证明：编译通过 + 执行不崩 + 返回了某个值

**probe 测试对于 JSON/XML 的局限：** 大部分方法接收 `this=null`（静态调用约定问题），所以结果全是 `NullReferenceException`。这验证了 AOT 编译存在，但没有验证任何功能。

### 5.2 Chunk 配置的准确性问题

- `gap-analysis.md` 指出 XML 部分 chunk 生成时无 subject（"no entry.exe found"）
- `System.Xml.ReaderWriter/xml` 411 个方法中 36 个 fail——需要分析这些 fail 是骨架问题还是真正实现缺陷
- `System.Text.Json/text-json` 的 1 个 fail 已被定位为 `JsonReaderException` 不能从 external-runtime stub 抛出

---

## 6. 达到生产级品质所需的工作项

### 6.1 短期冲刺（4-6 周）— 解决 FAIL chunk + 堵漏洞

| 编号 | 工作项 | 模块 | 估计工作量 | 验收条件 |
|------|-------|:----:|:---------:|---------|
| **J1** | `ChaosJsonSerializeString` 修复——正确加引号 + 转义 | JSON | 1d | Serialize<string> 输出有效 JSON |
| **J2** | 补全基本类型 Deserialize（Int16/Byte/SByte/UInt16/UInt32/UInt64/Single/Double/Decimal/String） | JSON | 2d | 所有基本类型的 round-trip Serialize/Deserialize 可用 |
| **J3** | 修复 `ChaosJsonDeserializeInt32/Int64/Bool` 的 JSON 语法校验 | JSON | 1d | 非标准 JSON 输入返回错误而非静默解析 |
| **J4** | `UInt32`/`UInt64` 的 Serialize 类型正确性修复 | JSON | 0.5d | UInt64.MaxValue (18446744073709551615) 正确输出 |
| **J5** | text-json chunk 的 `JsonDocument.Parse` `JsonReaderException` 复现 | JSON | 3d | 307 smoke → 降低 fail 计数 |
| **J6** | global-ns chunk 的 `JsonMetadataServices.Create*Info<T>` 至少实现 smoke 级编译通过 | JSON | 5d | 34 smoke 全部 green |
| **J7** | 写 `NameTable` 的单元功能测试 | XML | 1d | 至少 3 个端到端用例（Add/Get/引用相等性） |
| **J8** | 写 NameTable char[] 重载的 native 实现 + 注册 | XML | 2d | char[] 重载工作 |
| **J9** | 修复 XML 抽象基类 `XmlNameTable` 虚派发路径 | XML | 2d | 通过 XmlNameTable 引用调用 NameTable 方法走原生路径 |
| **J10** | 补全 `System.Xml.ReaderWriter/xml` 的 36 个 fail | XML | 5d | 36 个 fail 修复或归入已知限制白名单 |

### 6.2 中期建设（8-12 周）— 核心 API 覆盖

| 编号 | 工作项 | 模块 | 估计工作量 | 说明 |
|------|-------|:----:|:---------:|------|
| **M1** | **Utf8JsonReader** 的形状注册 + 原生实现 | JSON | 3w | 22 个方法，需要翻译状态机逻辑到 C++ |
| **M2** | **JsonDocument** 预编译路径（消除 bridge thunk） | JSON | 4w | Phase 2 最核心项，需要 BridgeMethodResolver 实现 |
| **M3** | **Utf8JsonWriter** 原生实现 | JSON | 3w | Stage D 目标 |
| **M4** | **XmlDocument** 核心方法群（LoadXml/CreateElement/AppendChild/InnerText/OuterXml） | XML | 4w | 需要 DOM 树 C++ 实现 |
| **M5** | **XmlReader** 基本实现（Read/NodeType/Name/Value） | XML | 3w | `XmlReader.Create(string)` 路径 |
| **M6** | **XmlWriter** 基本实现（WriteStartElement/WriteString/WriteEndElement） | XML | 2w | `XmlWriter.Create(StringBuilder)` 路径 |
| **M7** | **XDocument / XElement** 核心 LINQ to XML 支持 | XML | 2w | 至少支持 XDocument.Load/Parse + XElement 树构建 |
| **M8** | **JsonElement** 核心方法群 | JSON | 2w | GetProperty/TryGetProperty/ValueKind/EnumerateObject/EnumerateArray/GetInt32 等 |

### 6.3 长期工程（3-6 个月）— 生产级品质

| 编号 | 工作项 | 模块 | 说明 |
|------|-------|:----:|------|
| **L1** | **JsonSerializer POCO 属性预编译** | JSON | Phase 2 的完整实现：属性枚举器 + 代码生成器 + 类型参数提取器 |
| **L2** | **BridgeMethodResolver (L2/L3)** | JSON | 消除所有 bridge thunk，由 ShapeRegistry + Interpreter IL data 完全取代 |
| **L3** | **XmlSerializer** 原生实现 | XML | 复杂（涉及反射和代码生成），需要外部 RuntimeHelper 形状 |
| **L4** | **XmlSchema** 验证 | XML | 最复杂的 XML 子模块，建议走 interpreter IL data |
| **L5** | **XmlNameTable** 内存策略可配置 | XML | 当前永久分配，长生命周期场景需要可回收策略 |
| **L6** | **UTF-8/UTF-16 编码策略文档 + 实现** | 共享 | 整个模块的编码基线选择 |
| **L7** | **Benchmark 基线建立** | 共享 | 为每个核心路径建立 .NET 8 baseline，量化 AOT 加速比 |

---

## 7. 优先级与路线图建议

### 7.1 推荐优先级原则

1. **先止血（堵 FAIL chunk + 修复明显 bug）** — 短期
2. **再覆盖核心 API（让开发者能完成最常用的任务）** — 中期   
3. **最后做性能优化（消除 bridge thunk + 预编译）** — 长期

### 7.2 具体路线图

```
Phase S（4 周）— 止血
├── 第 1 周：J1-J4（JSON stub 修复 + 补全）
├── 第 2-3 周：J5-J6（json chunk FAIL → PASS），J7（NameTable 测试）
└── 第 4 周：J8-J10（XML char[] + 虚派发 + 36 fail 修复）

Phase M（10 周）— 覆盖核心 API
├── 第 1-3 周：M1（Utf8JsonReader）
├── 第 4-6 周：M2（JsonDocument 预编译）
├── 第 7-8 周：M4 + M5（XmlDocument + XmlReader 核心）
├── 第 9 周：M3（Utf8JsonWriter）
└── 第 10 周：M6 + M7（XmlWriter + XDocument 核心）

Phase L（持续）— 生产级
├── L1 + L2 — JSON 性能优化
├── L3 + L4 — XML 序列化和 Schema
├── L5 + L6 — 内存和编码架构
└── L7 — Benchmark 基线
```

### 7.3 风险提示

1. **JsonDocument 的 bridge thunk 消除有重大架构风险**。root-cause-analysis 指出它需要「将托管 IL 状态机桥接到 AOT 编译的 IR 岛」，复杂度高且可能需要修改 core-agent 的 IR lowering 阶段
2. **XmlDocument 的 C++ DOM 树实现工作量被低估的风险**。一个兼容 .NET 的 DOM 树（XmlNode/XmlElement/XmlAttribute/XmlText/XmlComment/CDATA 等层级结构 + Parent/Child/Sibling 导航）是数万行 C++ 的规模
3. **测试假绿风险**。目前两个模块的真实验证覆盖率极低（<10%），Phase S 阶段必须增设功能测试，不能只依赖 probe smoke
4. **异步变体爆炸**。`SerializeAsync<T>` / `DeserializeAsync<T>` / `JsonDocument.ParseAsync` 需要 async 状态机支持，独立基础建设成本

---

## 报告结论

**System.Text.Json** 处于 **Phase 1 基本完成 + Phase 2 设计阶段**。有基础的 primitive type stub 和设计文档，但核心功能（JsonDocument/JsonElement/Utf8JsonReader/Writer）全部走解释器，基本的 POCO 序列化不可用。3 个 chunk 中 2 个 FAIL。

**System.Xml** 处于 **Minimal Viable Implementation 状态——仅 NameTable.Add/Get 2 个方法有真实实现**。XmlDocument/XmlReader/XmlWriter/XmlSerializer/LINQ to XML/XSLT/Schema 全部 fallback 返回 0/null。5 个 chunk 中 2 个 FAIL，但 PASS 的 chunk 也只是 smoke 级别覆盖，无功能验证。

**生产级品质的总体判断：两者均远未达到。JSON 约完成 5-8%，XML 约完成 0.5-2%。** 投入估计：JSON 需要约 12-16 人周达到基本可用（核心 API 覆盖），XML 需要约 20-30 人周。达到完整生产级（含性能优化）需要 3-6 个月。

**建议立即行动项**（4 周冲刺）：
1. 修复 `ChaosJsonSerializeString` 的 JSON 正确性
2. 补全所有基本类型的 Serialize/Deserialize
3. 写 NameTable 的端到端单元测试
4. 修复 NameTable char[] 重载和抽象基类虚派发
5. 解决 text-json chunk 的 `JsonReaderException` fail