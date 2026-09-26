# 剩余 26 条 realDefect — 分组与根因（第二批，2026-09-24）

> 承接 `ROOTCAUSE-35.md`。第一批已修 9 条（commit `77a443461`），realDefect 35 → 26。
> 本文件记录剩余 26 条的**分组与各自根因**，全部基于一手证据。

---

## 0. 剩余 26 条分组总览

| 组 | 根因层 | 条数 | 可修性 |
|:---|:-------|:----:|:-------|
| **H1** | reader handle 不是 `g_readers[]` 槽 → `Resolve()` 失败 → 静默 return 0 | 7 | 可修（native） |
| **H2** | DOM 构造器 `RaiseNotSupported()`（架构性未实现） | ~10 | **需实现 DOM**（Phase 3 范围） |
| **H3** | async / 其他 | ~5 | 待定 |
| **H4** | 已抛正确类却仍失败（需 H1/H2 解释） | 4 | 随 H1/H2 解决 |

---

## 1. H1 — reader handle 类型不匹配（7 条）🔴 本轮新发现的根因

### 机制（一手）

`xml_reader_stubs.cpp:84-88`：

```cpp
ReaderState* Resolve(CHAOS_IL2CPP_INTPTR h) {
    if (h <= 0) return nullptr;
    const auto idx = static_cast<size_t>(h - 1);
    return (idx < kReaderCap) ? g_readers[idx] : nullptr;   // 1-based 槽索引
}
```

`g_readers[]` **只由 `ChaosXmlTextReaderCreate` 填充**（创建时分配槽并把槽号当 handle 返回）。

但 `XmlValidatingReader` / `XmlNodeReader` 的 receiver 来自 **shape stub 的 ctor**，
它走 `CHAOS_IL2CPP_NEW_GC(...)` 分配一个 **GC 对象**，**不经过 `g_readers[]`**。

⇒ `Resolve(GC对象指针)` 把指针当 slot 索引 → `idx >= kReaderCap` → 返回 `nullptr`
⇒ stub 里 `if (!st) return 0;` **静默返回、不抛任何异常**
⇒ subject 随后执行自己的哨兵 `throw new Exception("AOT stub did not throw")`
⇒ typed `catch (NRE)` 不匹配 → 落到 `catch { throw "wrong exception type" }`
⇒ runner 记 `caught=true, value=0` → realDefect

### 证据

生成体（`native-aot.page-0017.cpp`，`XmlValidatingReaderTests::GetAttribute_0_string_0`）：

```cpp
{   // receiver 构造：GC 对象，非 reader 槽
    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_..._XmlValidatingReader, {});
    chaos_object->header.type_info = ...;
    CHAOS_IL2CPP_INTPTR chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
    _s1 = chaos_result;
}
_s2 = 0;
{
    const auto chaos_arg_0 = _s1;                 // GC 对象指针
    if (chaos_arg_0 == 0) { raise_null_reference_exception(); }   // 非 null，不触发
    chaos_result = ChaosXmlTextReaderGetAttributeStr(chaos_arg_0, chaos_arg_1);  // Resolve 失败 → return 0
}
_s1 = CHAOS_IL2CPP_STRING_ID("AOT stub did not throw");   // ← 哨兵抛出
```

**决定性对照**：同一个方法名、**同一个 native 符号**：

| 测试类 | receiver 构造 | Resolve | 结果 |
|:-------|:-------------|:--------|:-----|
| `XmlTextReaderTests` | `new XmlTextReader(new StringReader(...))` → **reader 槽** | 成功 | `caught=False, passed=True` ✅ |
| `XmlValidatingReaderTests` | `new XmlValidatingReader(XmlReader.Create(...))` → **GC 对象** | 失败 | `caught=True, value=0` ❌ |
| `XmlNodeReaderTests` | `new XmlNodeReader(new XmlDocument())` → **GC 对象** | 失败 | `caught=True, value=0` ❌ |

⇒ **不是异常类名问题**（第一批已修好类名，`XmlTextReaderTests` 因此转绿），
而是 **handle 类型不匹配**。

### 修法方向（待验证）

让 `XmlValidatingReader` / `XmlNodeReader` 的 ctor **也注册 `g_readers[]` 槽**
（或让 `Resolve` 能识别 GC 对象并取回其内部 reader）。
属 native 改动，但需先确认这两个 ctor 的 shape 注册点与内部状态来源。

---

## 2. H2 — DOM 构造器 `RaiseNotSupported()`（~10 条，架构性）

`xml_document_stubs.cpp` 共 **18 处** `RaiseNotSupported()`。
subject 的 receiver 表达式形如 `new XmlDocument().CreateElement("root")`，
在**构造 receiver 阶段**即抛 `NotSupportedException` → 被测方法永不执行。

**这不是「改个类名」能解决的** —— 它要求**实现 XmlDocument DOM 构造**，
属 roadmap **Phase 3**「XmlDocument DOM（19 处 NotSupported）」同一件事。

**受影响**：`SetAttribute_2`×2、`SetAttributeNode_5`、`WriteTo_17`、
`GetElementsByTagName_7`、`GetElementsByTagName_17`、`AppendChild_6`、`PrependChild_5`、
`CreateAttribute_15`、`CreateElement_16`。

---

## 3. H3 — async 与其他（~5 条）

| subject | 现象 |
|:--------|:-----|
| `WriteStringAsync_66_string_0` / `WriteRawAsync_70_string_0` | ATG 源码是**裸调用无断言**；native async stub 对 null 返回 0（非 Task）→ 后续 `GetAwaiter()` 在 0 上抛 NRE |
| `SplitText_1_int_0` | 待查 |
| `ResolveEntity_13/14` | 待查 |
| `WriteEndDocument_2__0` | native 已抛正确类，需 H1/H2 类解释 |
| `ReadContentAs*`×4 | 待查 |

---

## 4. 诚实标注

- §1 的机制是**一手**（读 `Resolve` 实现 + 生成体 + 三类对照），置信度高
- §2 的 18 处 `RaiseNotSupported` 已实测计数（workflow lane 报 17，我实测 18，以其名单为准）
- §3 未完成调查
- **本批修复工作量远大于第一批**：H2 是需要实现功能的架构级工作，
  不是缺陷修复。建议 H2 与 roadmap Phase 3 合并推进