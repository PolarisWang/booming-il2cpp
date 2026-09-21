# AOT 异常类型解析失败 — 根因已定位

> 日期：2026-09-21
> 来源：`void-writer-sideeffect-assertion` 收尾时移交的缺陷

## 1. 现象

native 抛出了**类型正确**的异常（插桩证实），但生成体的
`chaos_eh_match_type` **匹配失败** → `CHAOS_EH_RETHROW` → 逃逸
→ fact 记 `caught=True / passed=False`。

## 2. 🔴 根因（插桩实证）

在 `ResolveTypeByName` 与 `RaiseManagedException` 插桩后实测
（写文件绕过 pipeline 的 stderr 捕获）：

```
[EXC] name=System.ObjectDisposedException   handle=0  modules=2   x26
[EXC] name=System.ArgumentNullException     handle=0  modules=2   x26
[EXC] name=System.InvalidOperationException handle=0  modules=2   x8
[EXC] name=System.ArgumentException         handle=0  modules=2   x2

[MOD] module=1  type_count=16  type_name=ObjectDisposedException  in_types=0
```

**`ResolveTypeByName` 对所有异常类型都返回 0** —— 模块注册了（`modules=2`），
但 **module 1 的类型表只有 16 个类型，不含任何异常类型**。

### 完整故障链

```
native RaiseManagedException("System.ObjectDisposedException")
  → ResolveTypeByName → 模块类型表找不到 → type_handle = 0
  → chaos_raise_exception(0)          ← **抛 null 异常对象**
  → 生成体 chaos_eh_match_type(null, mt_ObjectDisposedException) → 必然失败
  → CHAOS_EH_RETHROW → 逃逸 → caught=True / passed=False
```

## 3. 为什么模块类型表不含异常类型

`EmitReflectionQueryImage`（`NativeAotLoweringPlanner.DispatchAndRegistration.Reflection.cs:30`）：

```csharp
foreach (var method in methods)              // methods = 被 AOT lowering 的方法
{
    string declaringType = GetMethodDeclaringTypeSubjectId(method.SubjectId);
    typeMethodMap[declaringType].Add(...);   // ← 类型来自「被 lowering 方法的声明类型」
}
```

**反射镜像只包含「被 AOT lowering 的方法的声明类型」。**

异常类型（`ObjectDisposedException` 等）**不是任何被 lowering 方法的声明类型**
—— 它们只是被 native stub **抛出**，从未作为方法挂在某个类型上被 lowering
→ **不在镜像里** → `ResolveTypeByName` 找不到。

## 4. 影响面

**所有通过 `RaiseManagedException` 抛出的异常都可能受影响** ——
不只是 `ObjectDisposedException`，而是**上表列出的全部 4 类**
（`ArgumentNullException` / `InvalidOperationException` / `ArgumentException`
/ `ObjectDisposedException`）。

**但注意**：这些异常**部分场景仍能工作** —— 若异常类型恰好作为某方法的声明类型
进入了镜像，或调用方不依赖类型匹配（如 ATG 的弱断言 `catch { }`）。

**这正是为什么该缺陷长期未被发现**：弱断言不验类型，null 异常对象也能"通过"。

## 5. 修复方向（候选，未评估）

| 方案 | 做法 | 评价 |
|:-----|:-----|:-----|
| **A** | 反射镜像**额外收录「代码中抛出的异常类型」** | 需在 codegen 侧收集 `RaiseManagedException` 的类型名，加入镜像 |
| **B** | `ResolveTypeByName` 失败时**回退**到 MethodTable 符号 | 但 `chaos_mt_*` 是编译期符号，运行时按名字查需另一张表 |
| **C** | 让 `RaiseManagedException` 不再依赖名字解析 | 需为每个异常类型生成直接引用（如 `chaos_mt_Xxx.AsTypeInfoHot()`） |

**方案 C 最直接**（编译期已知类型，无需运行时查找），但需改 codegen 的
exception helper 生成方式。

## 6. 边界

**这是独立缺陷**，与 `void-writer-sideeffect-assertion`（void 断言）无关 ——
后者只是**暴露**了它（typed catch 需要类型匹配，弱断言不需要）。

**待用户确认是否立项修复。**
