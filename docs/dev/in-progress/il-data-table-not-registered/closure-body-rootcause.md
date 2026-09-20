# 闭包方法有槽无体 — 根因坐实（12/12）

> **日期**: 2026-09-18
> **上游**: `il-data-table-not-registered/STATUS.md` 第八节
> **结论**: 根因是 **`<>c::` 一刀切规则误伤 lambda** —— 非发射缺口、非链接缺口、
> 非 IL-data 缺口。

## 一、真正的根因

`NativeAotLoweringPlanner.MethodEmission.cs:169-183`：

```csharp
// Compiler-generated display class constructors (<>c::.cctor/<>c::.ctor)
// — their newobj instructions cannot be lowered properly ...
// Route them to the interpreter fallback instead of emitting CHAOS_IL2CPP_FAIL().
if (method.SubjectId is not null && method.SubjectId.Contains("<>c::", StringComparison.Ordinal))
{
    ... emit ChaosExternalRuntimeFallback(...) stub ...
}
```

**一条按 `<>c::` 前缀的规则，把 display-class 的 `.cctor/.ctor` 和
lambda（`b__N`）一刀切**。它本意是救 6 个 `.cctor/.ctor`
（newobj 无法结构化发射），却把 12 个**本可正常翻译的 lambda** 也带下去。

### 证据链（生成物判据）

实测 `threading` chunk 生成物：

| 角色 | 数量 |
|---|---:|
| `<>c::` 方法（methods.json） | 18 |
| ├ `.cctor/.ctor`（规则的正目标） | 6 |
| └ **lambda `b__N`（被误伤）** | **12** |
| lambda 中有真实 body | **0（100% 落 fallback stub）** |
| lambda 的 IL 指令数 | 全 ≤ 2 |

被误伤 lambda 的 IL 极其平凡：
```json
[ {"op":"ldc.i4","operand":0}, {"op":"ret"} ]
```
**`ldc.i4` / `ret` 比发射器日常处理的 IL 还简单**（对照：非 `<>c::` 的
`get_ExitCode` = `ldsfld + ret` 正常发射）。没有理由不能翻译。

### 运行时现象（先前已观察到）

闭包 stub 体内 `ChaosExternalRuntimeFallback(subject_id)` 走到 **Phase 3**
→ `NotImplementedException` → 外层被 `caught`。v:
```
Phase 3 catch-all: ...<>c::<EnsureInitialized_3...>b__14_0 — unresolvable subject raises NotImplementedException
```

## 二、为什么之前误判为「无 body」

先前测量的是**主体文件**（`native-aot.generated.cpp`）里
该符号 **只有声明**。实际上 body **存在、但在 page 文件里**，形态是
**fallback stub**（`page-0006.cpp`）：

```cpp
extern "C" CHAOS_IL2CPP_INT32 ...___c_EnsureInitialized_3_...b__14_0(CHAOS_IL2CPP_INTPTR a0)
{
    (void)ChaosExternalRuntimeFallback("...b__14_0:System.Int32()");
    return {};
}
```

所以「有槽无体」的准确说法是 **「有槽，有 stub，无真实 body」**。

## 三、修法范围

**只改 `MethodEmission.cs:169` 的判定** —— 从 `Contains("<>c::")` 收窄到
「`<>c::` 且不是 `b__`」：

```csharp
var afterCc = method.SubjectId.Split("<>c::", ...)[1];
if (method.SubjectId.Contains("<>c::") && !(afterCc.Contains("b__")))
```

即：`.cctor/.ctor` 继续走 fallback；`b__N` lambda **走正常发射路径**。

### 预期收益

- 本 chunk 12 个闭包中，凡是被除数事实卡住的测试
  （`EnsureInitialized_3`、`SpinWait`、`QueueUserWorkItem`）**下游立刻解封**
- 但**不保证全部转绿**：`EnsureInitialized_3` 的调用本身（非闭包）
  已由批次 2 修通；`QueueUserWorkItem` 的 3 参重载**本身仍无 native**
  （`managed_primitive_entries.h` 只导出 2 参）—— 那是独立缺口。

### 风险

- `b__` 前缀唯一标识 C# lambda；display-class 静态字段初始化
  也有 `<>c` 但无 `b__`，不受影响。
- IL 数据表中的闭包？**本 chunk 表中无闭包**（§8.2），无冲突。

## 四、实施记录（2026-09-18）：修复已写、**验证未完成**

### 4.1 改动

`NativeAotLoweringPlanner.MethodEmission.cs:169` 的判定收窄 +
新增 `IsLambdaBodySubjectId()` 辅助：

```csharp
var _isDisplayClassCtor = method.SubjectId is not null
    && method.SubjectId.Contains("<>c::", StringComparison.Ordinal)
    && !IsLambdaBodySubjectId(method.SubjectId);
```

Generator 与 TPG 均编译通过（0 error）。

### 4.2 ⚠️ 验证受阻：fact 阶段超时 + exe 挂死

首次跑 `threading` pipeline（build+fact）：

```
[build] Done (511673ms)          ← 平常约 200s，这次 8.5 分钟
<<< - [error] 0/0 passed (error); errors: aot: timed_out; jit: timed_out
```

随后**直接运行 `entry.exe --fact-json`**：跑到 `si=218` 后**输出停滞 16 分钟**
（文件 mtime 不再变化），但 **CPU 从 663s 持续涨到 969s** ——
**忙等，不是单纯卡住**。已 kill。

### 4.3 挂死原因**尚未归因**（两种假设并存）

**假设 A —— 我的 lambda 修复**：多发射 12 个真实 body → 编译变慢/运行路径变化。

**假设 B —— 并发会话的未提交改动**：
工作树里有**不属于本项目**的 `src/native/runtime-core/runtime_stubs/object_stubs.cpp`
修改（其在开发中，含 `[GUO]` 调试打印）。该文件正是
`ChaosRuntimeHelpersGetUninitializedObject` —— 而日志显示挂死前
连续输出 `[GUO] resolved type_name=(null)`。
其注释自述：「a live-but-pending handle would block the fact run forever (timed_out)」
—— **该改动本身已知会导致 timed_out**。

**我的那次运行同时包含了两者**，不能据此归因。

### 4.4 归因实验（已定论 2026-09-20）

| 实验 | 我的 lambda 修复 | object_stubs WIP | 结果 |
|---|---|---|---|
| 第 1 次 | ✅ 有 | ✅ 有 | 挂死 si=218 |
| 第 2 次（隔离） | ❌ 回退 | ✅ 仍有 | **同样挂死 si=218** |
| 第 3 次 | ✅ 恢复 | ✅ 有（=1c830ad24 WIP） | 必然同样挂死 |

**定论：挂死与我的 lambda 修复无关，是并发会话 `1c830ad24`（WIP）
引入的。** 该 commit 自承「未完成：stable_id 指纹匹配的实测值」。

**挂死路径**（si=218 `ReaderWriterLockSlim::TryEnterUpgradeableReadLock`）：
测试体 `SubjectInstanceFactory.Create<ReaderWriterLockSlim>()
.TryEnterUpgradeableReadLock(int)` → `GetUninitializedObject()`（GUO）。
日志显示挂死前连续 3 条 `[GUO] resolved type_name=(null)` ——
`LookupTypeNameByInfoPtr` 对 `typeof` 产物返回 null → 不进 Task 特判，
也不走 0-handle 快路 → 零初始化对象被当已绑定句柄死等。

**在对方 WIP 收口（或回退）之前，threading chunk 无法产生干净 fact 基线。
我的 lambda 修复恢复了但不可在本树提交 —— 等干净树验证。**

## 五、方法学教训

1. **「符号没有 body」要分两种**：真无 body vs **body 是 fallback stub**。
   grep 主体文件会漏 page 文件 → 误报「无 body」。
   判据要用跨文件正则（声明 vs `{` 后非 decl 的体）。
2. **`Contains("<>c::")` 这种前缀规则会静默误伤子类** ——
   检查规则时先问「正目标长什么样」，再问「还有谁匹配这个前缀」。
3. **⚠️ 并发工作树下跑 pipeline，不能把结果归因给自己的改动** ——
   第一次挂死被误认为可能与 lambda 修复相关，隔离实验
   （回退自改、保留他改）证明是对方 WIP。**归因前必须先隔离变量。**
4. **WIP 的 `LookupTypeNameByInfoPtr` 对 typeof 产物返 null** ——
   任何依赖「type_name 特判」的 GUO 路径在该 bug 修好前都不可用；
   以后若再看到 `[GUO] resolved type_name=(null)`，先怀疑这里。