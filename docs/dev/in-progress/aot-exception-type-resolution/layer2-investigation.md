# 层 2 调查：镜像类型的三个前置条件

> 日期：2026-09-21

## 目标

让异常类型进入反射镜像（`kReflImage`），使 `ResolveTypeByName` 能查到。

## 调查发现：镜像类型需满足 3 个条件

`EmitReflectionQueryImage`（`DispatchAndRegistration.Reflection.cs:30`）的类型
来自 `typeMethodMap`（:58/:240），每个类型需要：

| # | 条件 | 异常类型的现状 |
|:-:|:-----|:---------------|
| 1 | 有 **lowering 方法**（`typeMethodMap` 的键） | ❌ 无方法 |
| 2 | 有 **metadata token**（`tokenLookup.TryGetTypeToken`） | ❌ 无注册 |
| 3 | 有 **MethodTable 定义**（`hasMethodTable`） | ✅ **层 1 已补** |

## 条件 2 是真正的跨域障碍

`MetadataTokenLookup` 的 token 来自
`MetadataRegistrationEntry`（`RegistrationKind == "type"`），
而该 entry 由 **`closedArtifact`**（Loader/Linker 域）生成 ——
**不是 codegen 侧能自行补的**。

**即：层 2 需要跨 codegen → Loader/Linker 两域。**

## 已尝试但无效的改动（需回退）

在 `ModuleRegistration.cs` 的 `allTypes` 里加了
`.Concat(_referenceTypeBaseSubjectIds.Keys)`：

**无效** —— 那段代码生成的是 **`ChaosRegisterReflectionMembers`**
（反射成员注册），**不是 `kReflImage`**。产物 `type_count` 仍为 16。

**应回退**（除非确认它有无害的正向价值）。

## 层 2 的可行路径

| 路径 | 做法 | 域 |
|:-----|:-----|:---|
| **2-甲** | Loader/Linker 把「被 catch 的异常类型及其基类链」注册为 `RegistrationKind="type"` | **Loader/Linker** |
| 2-乙 | codegen 侧自造 token（不从 metadata 来） | codegen，但违反 token 语义 |
| 2-丙 | 不走镜像，让 `RaiseManagedException` 用**编译期符号**（C 方案本体） | codegen + native |

**2-丙 其实绕开了整个镜像问题** —— 若 `RaiseManagedException` 能直接拿到
`chaos_mt_Xxx` 符号，就不需要运行时按名解析。

## 对原三步方案的修正

原方案：
```
1. 补 parent 链        ✅ 已完成
2. 让异常类型进镜像    ❌ 跨域，成本高于预估
3. RaiseManagedException 用编译期符号（C 本体）
```

**修订**：**跳过步 2，直接做步 3（2-丙）** —— 因为步 3 的目标（让类型匹配工作）
不依赖镜像；镜像只是 `ResolveTypeByName` 的**实现手段**，而步 3 用**更好的手段**
（编译期符号）替代它。

**即：原「三步」实为「两步」—— 步 1（parent）+ 步 3（符号），步 2 可省。**
