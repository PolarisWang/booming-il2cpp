# 根因：WriteTo shape 匹配失败 = callee 类型名缺命名空间

> 日期：2026-09-20
> 方法：在 `TryMatchShape` 插诊断（写文件绕过 pipeline stderr 捕获）

## 根因

诊断输出（`w2_diag.txt`）：

```
[W2] callee=|System.Text.Json/JsonElement::WriteTo:System.Void(Utf8JsonWriter)|
  type=|JsonElement| method=|WriteTo|
  params=[Utf8JsonWriter]
  key=|JsonElement::WriteTo(Utf8JsonWriter)|
  hash=2580476868 found=False entries=1278
```

**`typeDisplayName` 解析出的是 `JsonElement`（短名，无命名空间）**

而注册用的是**全名**（`S21.cs:236-238`）：
```csharp
foreach (var (typeName, recvAbi) in new[]
{
    ("System.Text.Json.JsonDocument", jdocAbi),
    ("System.Text.Json.JsonElement",  jelemAbi),   // ← 全名
    ("System.Text.Json.JsonProperty", jpropAbi),
})
```

→ **canonicalKey 不匹配**：
```
实际查找 = JsonElement::WriteTo(Utf8JsonWriter)
注册的   = System.Text.Json.JsonElement::WriteTo(Utf8JsonWriter)
```

## 为什么会这样

`GetTypeDisplayNameFromSubjectId`（`Part1.cs:164-170`）：
```csharp
var separatorIndex = subjectId.IndexOf("::");
var typePart = subjectId[..separatorIndex];      // "System.Text.Json/JsonElement"
var slashIndex = typePart.IndexOf('/');
return slashIndex >= 0 ? typePart[(slashIndex + 1)..] : typePart;   // "JsonElement"
```

它取 `/` 之后的部分。而 callee 的格式是
**`<assembly>/<类型名>`** —— 这里的 `System.Text.Json` 是 **assembly 名**（非命名空间），
`JsonElement` 是**在 `System.Text.Json` 命名空间下的类型**，其命名空间被省略了。

**即：callee 里的类型名不含命名空间，而注册用全名 → 永不匹配。**

## 影响面

| 项 | 值 |
|:---|:---|
| 受影响 subject | `JsonDocument` / `JsonElement` / `JsonProperty` 的 `WriteTo` |
| 观测现象 | 全部落 catch-all（`no native body`），返回 0 不抛 |
| **静默性** | 编译通过、build 成功、**只是调用点仍是 catch-all**（记忆 `shape-registration-must-be-verified-by-generated-symbols` 描述的模式） |

## 这是既存缺陷

- 与本次 void 断言 spike **无关**（spike 只改 catch 类型）
- 由 spike 的**类型校验**暴露 —— 之前被 `catch { }` 弱断言掩盖

## 修法候选

| 方案 | 做法 | 评价 |
|:-----|:-----|:-----|
| **1** | 注册改用**短名**（`"JsonElement"` 而非 `"System.Text.Json.JsonElement"`） | 改动最小，但与同文件其他注册（如 `Reset` 用 `"Utf8JsonWriter"`）一致 —— **可能就是既有惯例** |
| **2** | 修 `GetTypeDisplayNameFromSubjectId` 使其返回全名 | 影响面大（所有 shape 都用它），风险高 |
| **3** | 让 callee 携带全名 | 需改上游 ID 生成，影响面最大 |

**初步看方案 1 最合理** —— 因为同文件 `Reset` 的注册就用短名
（`registry.Register("Utf8JsonWriter", "Reset", ...)`），且未被报告为问题。

## 待确认

- 短名注册是否对**同名跨命名空间类型**有歧义风险（如 `JsonDocument` 是否唯一）
- 其余用全名注册的 shape 是否也有同样问题（需全量排查）
