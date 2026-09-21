# 「还原 C# 真实表现」的方案分析

> 日期：2026-09-21
> 起因：用户质疑「C 方案不能还原 C# 真实表现」

## 1. 先定义「C# 真实表现」

C# 抛异常时建立的状态：

| 项 | C# 真实行为 |
|:---|:------------|
| 异常对象（带**运行时类型**） | ✅ 编译期解析类型 |
| **继承链**（`catch (Exception)` 捕获派生类） | ✅ 沿 type→base 查找 |
| `Message` | ✅ |
| `ObjectName` 等其它字段 | ✅ 由托管 `.ctor` 设置 |
| `StackTrace` / `InnerException` | ✅ |

## 2. 🔴 发现两个独立缺陷（不是方案问题）

### 缺陷 1：`ResolveTypeByName` 返回 0

反射镜像（`EmitReflectionQueryImage`）的类型**只来自「被 AOT lowering 方法的
声明类型」**。异常类型只被 native stub 抛出，从未被 lowering → 不在镜像。

→ `RaiseManagedException` 在 `type_handle == 0` 时 `chaos_raise_exception(0)`
→ **抛 null 异常对象**。

### 缺陷 2：异常类型的继承链是断的（**更根本**）

生成体实测：

```
chaos_mt_..._ObjectDisposedException = {nullptr, nullptr, 9595...}   ← parent=nullptr
chaos_mt_..._InvalidOperationException = {nullptr, nullptr, 55...}   ← parent=nullptr
chaos_mt_..._Exception = {nullptr, nullptr, 1097...}                 ← parent=nullptr
```

**全部异常类型的 parent 都是 nullptr，包括 `Exception` 自己。**

统计：57 个 MethodTable 中**只有 10 个有 parent**，47 个是 nullptr。
（有 parent 的例子：`AssertionException → Exception`、`MulticastDelegate → Delegate`）

**发射逻辑**（`ObjectModelEmission.cs:879-884`）：
```csharp
string parentExpr = "nullptr";
if (referenceTypeBaseSubjectIds.TryGetValue(item, out string? baseTypeId) && ...)
    parentExpr = "reinterpret_cast<...>(&" + GetNativeMethodTableSymbol(baseTypeId) + ")";
```

**异常类型不在 `_referenceTypeBaseSubjectIds` 里 → parent 恒为 nullptr。**

### 后果

`chaos_is_type_compatible` 走 parent 链：
```cpp
while (chaos_current != nullptr) {
    if (same stable_id) return true;
    chaos_current = chaos_current->parent;   // ← 恒 null，循环立即结束
}
```

**即使异常对象建对了，`catch (Exception)` 也只能匹配 `Exception` 本身**
—— 派生类捕获基类**全部失效**。

对比 C#：`catch (Exception)` 应能捕获 `ObjectDisposedException`。

## 3. 对方案的重新评估

| 方案 | 缺陷 1（类型查找） | 缺陷 2（继承链） | 能还原 C#？ |
|:-----|:------------------:|:----------------:|:-----------:|
| 现状 | ❌ | ❌ | ❌ |
| **A**（镜像收录异常类型） | ✅ | ❌ | ❌ 仍缺继承 |
| **C**（编译期符号） | ✅ | ❌ | ❌ 仍缺继承 |
| **必须**：C 或 A **+ 补 parent** | ✅ | ✅ | ✅ |

**用户质疑成立** —— **C 方案单独不够**。

## 4. 完整方案（两件事必须一起做）

### 4.1 让异常类型进入「被发射的类型集」

异常类型需**同时**满足：
1. 进反射镜像（缺陷 1）→ 否则运行时按名查不到
2. 进 `_referenceTypeBaseSubjectIds`（缺陷 2）→ 否则 parent 是 nullptr

**共同的前提：异常类型要被视为「被使用的引用类型」。**

**候选做法**：
- (a) 在 codegen 收集阶段，把「native stub 会抛出的异常类型」加入闭包
- (b) 更通用：把**所有 `chaos_mt_*` 中出现的类型的基类链**补全
  （即：只要一个类型被发射了 MethodTable，就应发射其完整 parent 链）

**(b) 更根本** —— 它不只修异常，而是修「任何一个被发射类型的继承链完整性」。

### 4.2 让 `RaiseManagedException` 用编译期符号（C 方案本体）

即使 4.1 修好，`ResolveTypeByName` 仍是**运行时字符串查表**，
比 C# 的编译期解析更易失配。

**4.1(b) 修好后**，异常类型的 `stable_id` 已知（编译期），
可在 native 侧直接引用 `chaos_mt_*` 符号。

## 5. 建议的分步

| 步 | 内容 | 价值 |
|:--:|:-----|:-----|
| **1** | 补全「被发射类型的 parent 链」（4.1b） | 🔴 修继承链 —— **影响所有 catch，不只异常** |
| **2** | 让异常类型进反射镜像（4.1a） | 修 `ResolveTypeByName` |
| **3** | `RaiseManagedException` 改用编译期符号（4.2） | 更接近 C# 的编译期解析 |

**步 1 价值最大**（不只影响异常，而是所有类型继承匹配），应优先。

## 6. 待确认

- 步 1 的 `_referenceTypeBaseSubjectIds` 为何不含异常类型 —— 是收集遗漏还是刻意
- 补全 parent 链是否会让产物体积显著增加（每个类型多一个指针 + 其 parent 的 MethodTable）
