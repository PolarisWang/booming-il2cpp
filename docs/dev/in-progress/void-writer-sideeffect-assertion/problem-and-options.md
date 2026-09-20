# void writer 方法副作用断言 — 设计

> 日期：2026-09-20
> 上游：`json-xml-industrialization` Phase 2 阻塞（`phase2-blocker-void-assertability.md`）
> 用户拍板：范围 = XML + JSON；ATG 改动 = 限定 writer 类

---

## 1. 问题

JSON/XML 的**写面 API 绝大多数返回 void**（`WriteCData`/`WriteString`/
`WriteNumber*`…）。而 fact 的 `real` 判据是 **`value != 42`** ——
void 方法 `value` 恒为 sentinel 42 → **永远无法成为 `real`**。

### 量化

| 事实 | 数值 |
|:-----|:-----|
| `unassertable` 总数（JSON/XML 线） | **213** |
| 其中 `System.Void` | **166** |
| 其中 `Task`/`ValueTask` | 47 |
| `Utf8JsonWriter` 的 `unassertable` | 104 / 115 |
| `XmlTextWriter` 的 `unassertable` | 50 / 56 |

**这不是"未实现"** —— 证据（§2）。

### 对比：这是 JSON/XML 线特有

| chunk | unassertable 占比 |
|:------|:----------------:|
| **System.Net.Sockets** | 35.0% |
| **Xml/xml** | **23.2%** |
| CoreLib/text | 13.3% |
| threading | 12.6% |
| **reflection** | **0.7%** |
| **text-2** | **0.0%** |

---

## 2. 证据：它们是"实现了但无法验证"

### 2.1 生成代码确实调用了 native

```cpp
ChaosXmlWriterWriteCData(chaos_arg_0, chaos_arg_1);   // ✅ 真实调用
_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);           // ← 之后无条件返回 sentinel
return _i1;
```

`bodyAvailability: "NativeGenerated"`（173/173 的 unassertable 都是）。

### 2.2 native 侧有真实实现

`xml_writer_stubs.cpp` 的 `WriterState` **已维护完整输出缓冲**：
```cpp
struct WriterState {
    char*  buf;          // growable output buffer (NUL-terminated)
    size_t len;          // bytes used (excluding NUL)
```
**只是没有暴露读取接口。**

### 2.3 ATG 有显式的「void ⇒ 不生成断言」分支

| 事实 | 位置 |
|:-----|:-----|
| 确定性断言块被 `!result.IsVoid` 门控 | `TestEmitter.cs:546` |
| Probe 主动放弃确定性 | `ProbeEmitter.cs:673-678`（注释：*"no observable side effect to verify across runs"*） |
| 无副作用捕获 | `AppendAssert` 只读 out/ref 参数（`TestEmitter.cs:645-677`） |
| 断言机制 | `Chaos.TestFramework.Assert.*` |

---

## 3. 方案：调用后读回副作用并断言

### 3.1 核心思路

```
调用前：记录 writer 输出（基线）
调用：  被测 void 方法
调用后：再次读取输出 → 断言"新增内容符合预期"
```

### 3.2 XML 侧（基础设施已在，改动小）

1. **native**：新增 `ChaosXmlWriterGetOutput(handle) -> managed string`
   —— 读 `WriterState::buf`，返回托管字符串
2. **shape 注册**：把新接口注册为可调用的 native shape
3. **ATG**：为 `XmlTextWriter` 的 void 写入方法生成：
   ```csharp
   var w = new XmlTextWriter(new StringWriter());
   w.WriteCData("abc");
   Assert.AreEqual("<![CDATA[abc]]>", GetXmlWriterOutput(w));
   ```
4. 断言通过 → 该 subject 成为 `real`

### 3.3 🔴 JSON 侧：**不是"加断言"，是"从零实现"**（修正）

**初版判断错误**（曾写「JSON writer 大部分已实现，只缺 4 个容器方法」）——
那是基于 `grep '函数总数 37 vs RaiseManagedException 7'` 的**误判**：
很多函数内部调用的是 `RaiseDisposedOrInvalid()` 而非直接 `RaiseManagedException`。

**spike 实测结论**：

```cpp
void ChaosUtf8JsonWriterWriteStringStr(...) noexcept
{
    CheckThis(this_ptr);
    ...
    (void)value;
    RaiseDisposedOrInvalid();     // ← 直接抛异常，**没有写入任何东西**
}
```

| 指标 | JSON writer | XML writer |
|:-----|:-----------:|:----------:|
| 写入操作（`AppendStr`/`memcpy`） | **0** | 大量 |
| 抛异常调用 | **38** | 少数（仅错误路径） |
| 内部输出缓冲 | ❌ 无 | ✅ `WriterState::buf/len` |
| **性质** | **未实现** | **已实现，缺断言** |

**因此 JSON 侧与 XML 侧的工作性质根本不同**：

- **XML**：加读取接口 + ATG 生成断言 → 让已实现的方法可验证
- **JSON**：**先实现整个 writer**（含输出缓冲设计）→ 断言问题随之解决

⚠️ **这推翻了「XML + JSON 一起」的前提** —— 两者不是同一类工作，
放同一立项会让范围严重失衡。**需用户重新拍板。**

---

## 4. 边界与非目标

**In scope（XML）**：
- `XmlTextWriter` 的 void 写入方法（约 50 个）
- ATG：为 writer 类生成副作用断言

**待定（JSON）**：
- `Utf8JsonWriter` 需**先实现**（约 104 个），断言是后续问题
- 建议**独立立项**，不并入本任务

**非目标**：
- `Task`/`ValueTask` 返回的 async 方法（47 个）—— 属 async 主线
- 非 writer 类的 void 方法（如 `XmlDocument.AppendChild`）—— 副作用不明确
- 不改 fact 的 `real` 判据本身

---

## 5. 风险

| 风险 | 影响 | 缓解 |
|:-----|:-----|:-----|
| **输出格式细节差异** | 断言过严导致假失败 | 用 .NET 8 实测作基准（如本日的 WriteEndDocument 修复） |
| **JSON 输出去向不明** | 读不到输出 | 立项后先做 spike 确认 |
| **ATG 改动波及面** | 影响其他 chunk | 限定 writer 类；未匹配者走原路径 |
| **AOT/JIT 口径不一致**（356 条） | 数字难比较 | 本任务不处理，单独记录 |

---

## 6. 待确认（blocking）

| # | 问题 | 状态 |
|:-:|:-----|:-----|
| 1 | JSON writer 的输出从哪读（内部缓冲 vs IBufferWriter）？ | ❌ **未查** —— 需 spike |
| 2 | ATG 如何识别"这是 writer 类"？ | ❌ 未定 |
| 3 | 断言的期望值从哪来（probe 记录 vs .NET 8 实测）？ | ❌ 未定 |

**blocking_questions 非空 → 不得进入 writing-plans。**
