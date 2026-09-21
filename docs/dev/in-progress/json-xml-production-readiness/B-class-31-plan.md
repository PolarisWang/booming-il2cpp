# B 类 31 项 — Utf8JsonWriter 缺失重载 实施规划

> 日期：2026-09-21
> 前置：C 类 12 项已修（`941436a26`，factoryGap 62→31）
> 基线：`ce6db123e`（origin/main）
> 规划依据：`tools/plan_b_class.py` 从 subject 源码提取，**非估计**

## 1. 工作量精确清单

**31 项 → 31 个不同 shape 键**（当前无重复，一 subject 一键）。

按 native 侧现状分三组：

### 组 1：native 有近似符号但**参数类型不匹配**（12 键）

| method | 需要的 paramTypes | 现有 native 符号 | 差距 |
|:-------|:------------------|:-----------------|:-----|
| WriteNumber | (JsonEncodedText, decimal) | `WriteNumberStr`=(string,Int64) | arg0 类型 + arg1 宽度 |
| WriteNumber | (JsonEncodedText, double) | `WriteNumberStrDouble`=(string,double) | arg0 类型 |
| WriteNumber | (JsonEncodedText, float) | `WriteNumberStrFloat`=(string,float) | arg0 类型 |
| WriteNumber | (JsonEncodedText, int) | 无 | — |
| WriteNumber | (JsonEncodedText, long) | `WriteNumberStr`=(string,Int64) | arg0 类型 |
| WriteNumber | (JsonEncodedText, uint) | 无 | — |
| WriteNumber | (JsonEncodedText, ulong) | 无（`WriteNumberUInt64` 是 value-only） | — |
| WriteNumber | (string, decimal) | 无 | — |
| WriteNumber | (string, int) | 无（`WriteNumberStr` 是 Int64） | — |
| WriteNumber | (string, uint) | 无 | — |
| WriteNull | (JsonEncodedText) | `WriteNullStr`=(string) | arg0 类型 |
| WriteBoolean | (JsonEncodedText, bool) | `WriteBooleanStr`=(string,Int32) | arg0 类型 |

> ⚠️ **C2660 红线**：C++ 不允许同名不同参重载跨 TU 解析 —— 每个新形参列表
> **必须独立符号名**。命名建议 `ChaosUtf8JsonWriter<Method>Encoded<Width>` /
> `...Str<Width>`。

### 组 2：native **完全没有**该重载（17 键）

| 族 | 键数 | 明细 |
|:---|:----:|:-----|
| `WriteString(JsonEncodedText, X)` | 4 | X = DateTime / DateTimeOffset / Guid / JsonEncodedText |
| `WriteString(JsonEncodedText, string)` | 1 | |
| `WriteString(string, X)` | 3 | X = DateTime / DateTimeOffset / Guid |
| `WriteString(string, JsonEncodedText)` | 1 | |
| `WriteStartArray(string)` / `(JsonEncodedText)` | 2 | 现有只有 0 参版 |
| `WriteStartObject(string)` / `(JsonEncodedText)` | 2 | 同上 |
| `WriteRawValue(string, bool)` | 1 | 第二个 bool 未表示 |
| `WriteRawValue(ReadOnlySequence<byte>, bool)` | 1 | 无 |
| `WriteNumberValue(int)` / `(uint)` | 2 | 宽度缺失（long/ulong 已有） |

### 组 3：🔴 **已注册却仍落 catch-all**（2 键）—— 独立问题，非"缺注册"

| subject | shape 键 | 状态 |
|:--------|:---------|:-----|
| `WritePropertyName_34_JsonEncodedText_0` | `WritePropertyName(JsonEncodedText)` | 已注册（S21:65-68） |
| `WriteStringValue_57_JsonEncodedText_0` | `WriteStringValue(JsonEncodedText)` | 已注册（S21:188-190） |

**一手证据**：
- `aot-capability-manifest.json` **含**这两个 shape（`kind: exact`，
  `paramTypes: ["System.Text.Json.JsonEncodedText"]`）
- 但生成体的调用点仍是 **0 参 catch-all**
  `chaos_external_runtime_...WritePropertyName_System_Void_JsonEncodedText_();`

**即：注册存在、manifest 收录，匹配却没命中。** 这是**匹配环节**的问题
（形参类型解析 / 键规范化），与组 1/2 的"没有实现"性质不同。
**必须单独定位，不能靠"再注册一遍"解决。**

> 这与记忆中的 `shape 注册必须读产物验证`（编译通过 ≠ 匹配生效）
> 和 `WaitHandle shape 注册失败`（两条改动跨 commit 拆开）是同一类陷阱。

## 2. 建议实施顺序（按风险递增）

| 步 | 内容 | 理由 |
|:--:|:-----|:-----|
| **B0** | **先查组 3 的 2 键** | 成本最低；若是匹配缺陷，可能**顺带影响组 1/2 的注册方式**（先例：注册了也可能不生效） |
| **B1** | 组 2 的 17 键 | 纯新增，无既有行为可破坏 |
| **B2** | 组 1 的 12 键 | 需改既有符号语义，风险略高 |

**每步都要**：改 native → 重建 SDK → 重建 Generator/TPG（**TPG 捆绑陈旧
Generator.dll**）→ 跑 text-json build+fact → 对比 factoryGap。

## 3. 验收判据

- **主判据**：`factoryGap` 从 **31 → 0**
- **必要副判据**（防假绿）：
  1. 生成体调用点从 `chaos_external_runtime_*()`（0 参）
     变为 `ChaosUtf8JsonWriter*(...)`（带 receiver）
  2. `caught` 应由 `true` 变 `false`
  3. `stubGap` / `failed` / `realVerified` **不得回归**
- ⚠️ **不预期这些转成 `real`**：全是 void 方法，`value=42` 是结构性 sentinel
  → 预期落 `unassertable`。**这是准确分类，不是失败。**
  若要转 real，需 void 副作用断言基础设施（独立任务）。

## 4. 诚实标注 / 未决

- 组 3 的 2 键**根因未定位** —— 只确认了"注册存在但未命中"这一现象
- 「31 键」按 subject 源码逐一提取（`plan_b_class.py`），**非估计**；
  但**未逐一核对 native 侧是否另有等价符号未被脚本识别**（脚本只按名字前缀匹配）
- `WriteString(string, string)` 在组 1/2 均未出现 —— 它已由 C 类修复转绿，
  不在本清单内
- 组 2 中 `ReadOnlySequence<byte>` 涉及泛型/struct 传递，
  **ABI 形态未验证**，实施时需先确认载体类型（可能不是简单 INTPTR）
- 本规划**未估算会话数** —— 待 B0 完成、确认组 3 性质后再估（它可能显著改变总量）

---

## 5. 规划时的一手复核（2026-09-21）

**组 3 的「注册存在但未命中」已在当前产物上复现**：

```
生成体（.../native-aot.generated.cpp, 15:19）中
chaos_external_runtime_..._WritePropertyName_System_Void_JsonEncodedText_()  出现 2 次
chaos_external_runtime_..._WriteStringValue_System_Void_JsonEncodedText_()   出现 2 次
```

而 `aot-capability-manifest.json` 同时含：

```json
{"kind":"exact","typeDisplayName":"Utf8JsonWriter","methodName":"WritePropertyName",
 "paramTypes":["System.Text.Json.JsonEncodedText"],
 "nativeSymbol":"ChaosUtf8JsonWriterWritePropertyNameEncoded","shapeKind":"SimpleForward"}
{"kind":"exact","typeDisplayName":"Utf8JsonWriter","methodName":"WriteStringValue",
 "paramTypes":["System.Text.Json.JsonEncodedText"],
 "nativeSymbol":"ChaosUtf8JsonWriterWriteStringValueEncoded","shapeKind":"SimpleForward"}
```

**manifest 收录 + native 符号齐备 + 注册调用存在 → 调用点仍是 0 参 catch-all。**
组 3 是**匹配环节**的缺陷，与组 1/2 的「没有实现」性质不同，**必须独立定位**。

> ⚠️ 注意：该 manifest 与生成体属同一次构建，**非陈旧快照**；
> 已用「同目录同时间戳」核对。

**B0（查组 3）因此被提到第一步** —— 若根因是键规范化/参数解析，
它可能同样影响组 1/2 新增注册的生效性，先查可避免白做。
