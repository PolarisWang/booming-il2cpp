# B0 根因 — shape 匹配的**形参类型名形态不一致**（bare vs full）

> 日期：2026-09-21
> worktree：`b-class-writer-overloads`
> 定位方式：在 `TryCreateExternalRuntimeHelperDefinition` 与
> `BuildExternalRuntimeDispatchTable` 加**文件诊断**（TPG 吞 stderr）后实测

## 1. 结论（一句话）

`TryMatchShape` 用 **core-IR 的 subject-id** 解析形参类型（**裸名**，如 `JsonEncodedText`），
而 `Register(...)` 注册时用的是 **display name（全名**，如 `System.Text.Json.JsonEncodedText`）。
**两边字面量不同 → 规范键永不相等 → 静默落 catch-all。**

## 2. 一手证据

### 诊断输出（同一次构建，`b0_diag.txt`）

```
ENTRY callee=System.Text.Json/Utf8JsonWriter::WritePropertyName:System.Void(JsonEncodedText)
  TryMatchShape=False kind=- symbol=-                      ← ❌ 落 catch-all
ENTRY callee=System.Text.Json/Utf8JsonWriter::WritePropertyName:System.Void(System.String)
  TryMatchShape=True  kind=SimpleForward symbol=ChaosUtf8JsonWriterWritePropertyNameStr   ← ✅
ENTRY callee=System.Text.Json/Utf8JsonWriter::WriteStringValue:System.Void(JsonEncodedText)
  TryMatchShape=False kind=- symbol=-                      ← ❌
ENTRY callee=System.Text.Json/Utf8JsonWriter::WriteStringValue:System.Void(System.String)
  TryMatchShape=True  kind=SimpleForward symbol=ChaosUtf8JsonWriterWriteStringValueStr    ← ✅
```

**同一方法名、同一注册逻辑，唯一差别是形参是 `JsonEncodedText` 还是 `System.String`。**

### 两个产物的拼写不一致（决定性）

同一方法在**同一次构建**的两个产物里写法不同：

| 产物 | 该方法的拼写 |
|:-----|:------------|
| `aot-capability-manifest.json` | `WritePropertyName:(System.Text.Json.JsonEncodedText)` ← **全名** |
| `aot-core-ir.json` | `WritePropertyName:System.Void(JsonEncodedText)` ← **裸名** |

- **注册**用全名（`S21.cs:68` → `"System.Text.Json.JsonEncodedText"`）
- **匹配**读 core-IR 的裸名（`GetMethodParameterTypesFromSubjectIdImpl`
  只是 `SplitTopLevelTypeList(paramsPart)`，**原样切分，不做命名空间补全**）

⇒ 规范键 `Utf8JsonWriter::WritePropertyName(System.Text.Json.JsonEncodedText)`
≠ 实际计算的 `Utf8JsonWriter::WritePropertyName(JsonEncodedText)`。

## 3. 影响面**远超这 2 项**（重要）

扫描 `aot-core-ir.json` 的形参拼写分布：

| 拼写 | 次数 | 形态 |
|:-----|:----:|:-----|
| `System.String` | 1506 | FULL |
| `System.Int32` | 488 | FULL |
| **`JsonSerializerOptions`** | **232** | **BARE** |
| `System.RuntimeTypeHandle` | 162 | FULL |
| **`JsonEncodedText`** | **90** | **BARE** |
| **`JsonDocument`** | **86** | **BARE** |
| **`JsonElement`** | **74** | **BARE** |
| **`JsonDocumentOptions`** | **42** | **BARE** |

**凡形参类型来自 `System.Text.Json` 命名空间者，一律以裸名出现。**
本 chunk 只有 2 项**恰好**因此失败，是因为其余组合尚未注册或另有路径 ——
但**任何**未来为这些类型注册 shape 的尝试都会**静默失效**。

> 这与既有的「类型段短名 vs 全名」兜底（`TryMatchShape` 末尾那段）
> 是**同一类缺陷的第二个实例**：那次修的是**类型名**，这次是**形参类型名**。
> 该兜底只处理 `typeDisplayName`，**没有处理 paramTypes**。

## 4. 修法（待实施）

在 `TryMatchShape` 内，对解析出的 `paramTypes` 做**候选键尝试**：
若精确键未命中，则把裸名补全为全名再试一次。

**关键约束**：补全必须有**唯一依据**，不能猜。可行方案：
- **方案 A（推荐）**：从注册表自身建立「裸名 → 全名」映射
  （遍历 `_entriesByShapeId.Values` 的 `ParamTypeDisplayNames`，
  取最后一段作裸名；若某裸名对应**多个**全名则**不补全**——避免歧义）。
  这与既有 typeDisplayName 兜底的安全性论证同构（「零歧义才兜底」）。
- 方案 B：硬编码 System.Text.Json 命名空间前缀 —— 脆弱，不推荐。

## 5. 诚实标注

- 「`JsonEncodedText` 是该 chunk 唯一以裸名出现的已注册形参类型」经脚本核对
  （只统计了注册侧含 `JsonEncodedText` 的 3 处）；但**未穷举**所有注册的形参名，
  故「唯一」一词仅限本 chunk 实测范围
- §3 的分布表来自单个 chunk 的 core-IR，**未跨 assembly 统计**
- 修复前**必须先确认**补全映射无歧义，否则会把 A 的调用导到 B 的实现
