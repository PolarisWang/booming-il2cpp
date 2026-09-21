# 完整根因链（三步实测）

> 日期：2026-09-21

## 根因链（4 层，逐层实测确认）

### 层 1：异常类型的 MethodTable parent = nullptr ✅ **已修**

`ObjectModelEmission.cs:879-884` 的 parent 来自 `_referenceTypeBaseSubjectIds`，
而该字典的收集（`CollectReferenceTypeBaseSubjectIds`）**只认 instruction 的
TargetReference** —— 异常类型从未作为 TargetReference 出现。

**已修**（本次）：两处
1. `GetSyntheticReferenceTypeBaseSubjectId` 补异常继承链（.NET 实测）
2. `Pass 1b` 把 catch 类型的**完整基类链**加入字典

**验证**：生成体里 `chaos_mt_..._ObjectDisposedException` 的 parent
**从 `nullptr` 变为真实符号** ✅

### 层 2：反射镜像只有 16 个类型，且全是 `Chaos.TestFramework.Sdk` ❌ **未修**

`kReflImage = { "CombinedSubjects", kReflTypePtrs, 16u, ... }`

16 个类型实测：
```
Chaos.TestFramework.Assert / AssertionException / FactAttribute / ...
```
**全是 Sdk 的，没有任何 System 类型。**

**根因**（`Methods.ModuleData.cs:47-49`）：
```csharp
if (!string.Equals(metadataAssemblyName, _assemblyName, StringComparison.Ordinal))
    return;    // ← 只扫当前 assembly
```

### 层 3：`ResolveTypeByName` 因此返回 0 ❌

`System.ObjectDisposedException` 在 `System.Private.CoreLib`，
**不在镜像里** → 按名找不到 → `handle=0`。

### 层 4：抛 null 异常对象 ❌

`RaiseManagedException`：`type_handle == 0` → `chaos_raise_exception(0)`
→ **抛 null** → `chaos_eh_match_type` 必然失败。

## 为什么层 1 修好但 fact 仍失败（62 项不变）

**层 2-4 未修** —— 异常对象是 null，parent 链再正确也无对象可匹配。

**层 1 的价值**：它为「一旦对象建对，`catch (Exception)` 就能工作」铺平了路。

## 层 2 的修复成本评估

**需要**：让镜像**跨 assembly 收录**异常类型。

**难点**：
- 现设计**刻意**只扫当前 assembly（注释：*"keeps the scan proportional to the
  closure rather than to the BCL"*）
- 跨 assembly 扫描会让产物膨胀（BCL 类型众多）

**可行做法（候选）**：
| 方案 | 做法 | 成本 |
|:-----|:-----|:-----|
| **2a** | 镜像**额外收录**「catch 类型及其基类链」——只增量，不扫全 BCL | 低 |
| 2b | 去掉 assembly 过滤，扫全部 resolved 程序集 | 高（产物膨胀） |

**2a 最合理** —— 复用层 1 已收集的 catch 类型集合，把它们从**各自所属 assembly**
的元数据里取出（而非只扫当前 assembly）。

## 当前状态

| 层 | 状态 |
|:--:|:-----|
| 1 | ✅ 已修（parent 链） |
| 2 | ❌ 待修（镜像跨 assembly） |
| 3-4 | 随层 2 自动解决 |

**已投入较多轮次，继续做层 2 前需确认方向。**
