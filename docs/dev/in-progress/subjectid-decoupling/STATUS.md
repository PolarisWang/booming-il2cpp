# subjectid-decoupling — 解除 SubjectId 中 assembly 前缀与类型名的耦合

> task_id: `subjectid-decoupling`
> 类型：brainstorm（域：Loader/Metadata + CodeGen）
> 创建：2026-09-20
> 上游：`lock-handle-binding/notes/interlocked-regression-rootcause.md`（根因分析）
> phase: brainstorming

## 一、问题陈述

`ManagedNaming` 把 **assembly 前缀**与**类型名**编码进同一个字符串
`"{assembly}/{namespace.type}"`，并额外加了一条隐式规则：

```csharp
// ManagedNaming.cs:412
private static string GetTypeIdentityPart(assemblyName, namespaceName, typeName)
{
    return string.Equals(namespaceName, assemblyName) || IsNullOrEmpty(namespaceName)
        ? typeName                        // ← namespace == assembly 时丢掉 namespace
        : $"{namespaceName}.{typeName}";
}
```

**这条规则让"剥掉 assembly 后剩下的东西"依赖于 assembly 是什么**，从而与
shape 注册表的键（按类型名拼写）形成隐式契约。

实测后果（见根因文档）：`System.Threading/Interlocked` 剥出短名 `Interlocked`
（**恰好**命中注册键 `"Interlocked"`）；而归一为 `System.Private.CoreLib` 后
剥出 `System.Threading.Interlocked`，**失配** → 落 catch-all → 返回 0。

## 二、影响面

- 短名注册 8 个类型：`Interlocked` / `Monitor` / `Volatile` / `Marshal` /
  `RandomNumberGenerator` / `RuntimeEnvironment` / `Utf8JsonWriter` / `AsnWriter+Scope`
- **一半 Interlocked subject 的期望值恰为 0**，与 catch-all 返回值不可区分 →
  影响面被掩盖（判据必须盯非零期望值）

## 三、待决问题

见 §五 blocking_questions。
