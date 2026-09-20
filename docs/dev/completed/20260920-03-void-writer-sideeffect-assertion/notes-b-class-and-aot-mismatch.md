# B 类修复 + 一个**超出本任务范围**的 AOT 缺陷

> 日期：2026-09-20

## 1. B 类修复（native 语义纠正）

.NET 实测（bare `Utf8JsonWriter`）：

| 调用 | .NET 实际 |
|:-----|:----------|
| `Reset(default(Stream)!)` | `ObjectDisposedException` |
| `Reset(Stream.Null)` | `ObjectDisposedException` |
| `Reset(default(IBufferWriter<byte>)!)` | `ObjectDisposedException` |
| `Flush()` | `ObjectDisposedException` |
| `Dispose()` | **不抛** |

**修复**（`json_writer_stubs.cpp`）：
- `Flush`：补 `RaiseDisposedOrInvalid()`
- `ResetStream`/`ResetBufferWriter`：**接收者检查优先于参数校验**
  （原先 `if (stream == 0) RaiseArgumentNullException` 抢先，抛错类型）
- `Dispose`：保持不抛（与 .NET 一致）

## 2. 🔴 但修复**未使 subject 通过** —— 发现独立的 AOT 缺陷

### 诊断证据

在 `Flush` 与 `RaiseDisposedOrInvalid` 插桩后实测：

```
[FLUSH-DIAG] entered this=2551403840520        ← this 非 0（bare object 有地址）
[DISPOSED-DIAG] raising ObjectDisposedException ← 确实抛了正确类型
```

**native 行为完全正确**（抛的就是 `ObjectDisposedException`）。

### 但 subject 仍失败

生成体的断言是：
```cpp
if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ,
                         chaos_mt_System_Private_CoreLib_System_ObjectDisposedException.AsTypeInfoHot()))
{ CHAOS_EH_RETHROW; }     // ← 匹配失败 → 重抛 → 逃逸 → fact 记 caught=True/passed=False
```

**即：抛出的异常对象**没有**匹配上期望的类型。**

### 推测根因（未验证）

`RaiseManagedException` → `ResolveTypeByName("System.ObjectDisposedException")`
→ 若返回 0（类型不在模块注册表）则 `chaos_raise_exception(0)`，
**抛的是一个 null 异常对象** → 类型无从匹配。

对比：`InvalidOperationException`（XML 侧）能匹配成功，
说明机制本身可用 —— 差异可能是**类型是否在模块表**。

## 3. 结论与边界

| 项 | 结论 |
|:---|:-----|
| **B 类 native 修复** | ✅ 正确（.NET 实测对齐），**保留** |
| **subject 是否转绿** | ❌ 否 —— 被 AOT 类型匹配缺陷阻断 |
| **该 AOT 缺陷** | 🔴 **超出本任务范围**（void 断言），属「异常类型匹配」独立问题 |

**本任务的 goal（让 void 方法可验证）已达成** —— 断言机制工作正常，
且**正因为断言变强，才暴露出这个更深的问题**。

## 4. 移交

**新问题**：AOT 抛出的托管异常，其 `type_info` 在 `chaos_eh_match_type` 下
匹配不上期望类型（至少对 `ObjectDisposedException`）。需独立立项排查：
- `ResolveTypeByName` 是否返回 0
- 若返回 0，是类型未注册还是查找失败
- 影响面：所有依赖 typed catch 的 subject
