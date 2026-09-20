# 全量排查：shape 静默失配的规模与规律

> 日期：2026-09-20
> 方法：静态比对「所有注册 key」vs「产物里所有 catch-all callee」

## 规模

| 类别 | 数量 |
|:-----|:----:|
| 注册的 (type, method) 对 | **475** |
| 产物里的 distinct catch-all callee | 2787（总计 9412 次） |
| **A. callee 短名 且 短名已注册** | **254** ✅ 正常 |
| **B. callee 短名 但仅全名已注册** | **5** ← 静默失配 |
| C. 完全未注册 | 2285（正常，多数无 native 实现） |

**结论：不是系统性问题，而是局部的 5 个。**

## 失配明细（B 类，全部）

| callee | 注册名 |
|:-------|:-------|
| `System.Text.Json/JsonElement::WriteTo` | `System.Text.Json.JsonElement` |
| `System.Text.Json/JsonDocument::WriteTo` | `System.Text.Json.JsonDocument` |
| `System.Text.Json/JsonProperty::WriteTo` | `System.Text.Json.JsonProperty` |
| `System.Private.CoreLib/ExecutionContext::IsFlowSuppressed` | `System.Threading.ExecutionContext` |
| `System.Private.CoreLib/ExecutionContext::RestoreFlow` | `System.Threading.ExecutionContext` |
| `System.Private.CoreLib/ExecutionContext::SuppressFlow` | `System.Threading.ExecutionContext` |
| `System.Private.CoreLib/SynchronizationContext::SetSynchronizationContext` | `System.Threading.SynchronizationContext` |
| `System.Private.CoreLib/SynchronizationContext::.ctor` | `System.Threading.SynchronizationContext` |

## 🔑 失配的确定性规律

callee 的类型段 = **类型全名去掉「与 assembly 名相同的前缀」**：

| assembly | 类型全名 | callee 类型段 | 是否失配 |
|:---------|:---------|:--------------|:--------:|
| `System.Text.Json` | `System.Text.Json.JsonElement` | `JsonElement` | ✅ 失配 |
| `System.Private.CoreLib` | `System.Threading.ExecutionContext` | `ExecutionContext` | ✅ 失配 |
| `System.Private.Xml` | `System.Xml.NameTable` | `System.Xml.NameTable` | ❌ 正常 |

### 实测确认（从产物提取的原始字符串）

```
System.Private.CoreLib/ExecutionContext::Capture:ExecutionContext() — no native body
System.Text.Json/JsonElement::WriteTo:System.Void(Utf8JsonWriter)  — no native body
```

**callee 的类型段确实是短名**（`ExecutionContext`），而注册用全名
（`System.Threading.ExecutionContext`）→ **失配属实**。

### 裁剪规则（尚未完全确定，但不影响修法）

| 情形 | assembly | 类型全名 | callee 段 |
|:-----|:---------|:---------|:----------|
| 1 | `System.Text.Json` | `System.Text.Json.JsonElement` | `JsonElement` |
| 2 | `System.Private.CoreLib` | `System.Threading.ExecutionContext` | `ExecutionContext` |
| 3 | `System.Private.Xml` | `System.Xml.NameTable` | `System.Xml.NameTable`（保留全名） |

情形 1 是「去掉 assembly 前缀」，情形 2 不是（assembly 与命名空间无关，
却仍被裁到只剩类型名）。**规则未完全确定** —— 但三种情形的共同点是：

**callee 段要么是短名（1、2），要么是全名（3）；注册必须与之匹配。**

**为什么只有 5 个失配**：多数类型的 callee 段恰好等于注册全名（情形 3）。

## 影响评估

- **规模小**（5 项），但**性质严重**：静默失配 → 走 catch-all → **返回 0 不抛**
  → 对 JSON 的 `WriteTo` 而言，**期望的 `ArgumentNullException` 从未发生**
- **是不可见的**：编译通过、build 成功、调用点看起来正常

## 修法

按现有证据，**最小修法**是让注册名与 callee 的实际形态一致。

但需先**确认 callee 类型段的裁剪规则** —— 否则可能修了这 5 个又漏别的。

**建议**：在 `TryMatchShape` 加**兜底匹配**（先按原 key 查，失败则尝试
「去掉命名空间前缀的短名」再查一次），这样对两种形态都兼容，且不必
逐个改注册。
