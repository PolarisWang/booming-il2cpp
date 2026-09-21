# A 方向 brainstorm — 输入调研（page 0 载荷分页）

> 日期：2026-09-21
> 状态：brainstorm 输入，**未拍板**
> 上游：`c1002-heap-exhaustion-rootcause.md`

## 一、已确认的事实（实测，字节级）

### 1. 失败点

`native-aot.generated.cpp(571920): fatal error C1002: compiler is out of heap space in pass 2`
**空闲机器单跑同样失败** → 非并发争用。

### 2. page 0 = `native-aot.generated.cpp`：1,236,192 行 / **71.2 MB**

命名来源 `pageSuffix = i == 0 ? "" : $".page{i+1}"` —— page 0 恰好与「单体 TU」同名，
这解释了此前（含我）「分页失效退回单体」的误判。

### 3. page 0 的段构成（快照实测，字节级）

| 段 | MB | % | 行数 |
|---|---|---|---|
| **Virtual method table arrays** | 22.2 | 31.2% | 680,472 |
| **Module registration** | 22.0 | 31.0% | 254,269 |
| **GC Slot Map Section** | 8.7 | 12.2% | 153,926 |
| Hotpatch name index + dispatch table | 5.0 | 7.0% | 29,015 |
| Reflection Query Image Descriptor | 2.4 | 3.4% | 10,240 |
| Late type-id definitions | 2.1 | 3.0% | 12,281 |
| ABI manifest | 1.5 | 2.1% | 19,514 |
| Method table (kMethodTable[]) | 1.5 | 2.0% | 9,634 |
| Registration calls | 1.4 | 2.0% | 22,704 |
| CodeRegistrationV0 | 1.4 | 2.0% | 9,630 |
| 其余 | ~3 | ~3% | — |

**top-3 段 = 74.4%**。

### 4. 分页器只切方法，不切载荷

`NativeAotEmitter.Shared.cs:155-232`：

```csharp
int page0Overhead = objectModelLength + MethodDeclarations.Sum(..)
                  + ModuleRegistrationCode.Length + GenericRegistrationCode.Length + 500;
bool isFirstPage = (actualPageIdx == 0);
int accumulated = isFirstPage ? page0Overhead : perPageOverhead;   // ← 初值即超阈值

while (methodIdx < totalMethods) {
    int methodSize = (allMethods[methodIdx].MethodSource?.Length ?? 0) + perMethodOverhead;
    if (accumulated + methodSize > sizeThresholdChars && methodIdx > pageStart) break;
    accumulated += methodSize; methodIdx++;
}
```

`sizeThresholdChars = 350_000`，注释明写「避免 MSVC C1060（触发于 ~15MB / 448K 行）」。
**page 0 实际 71.2 MB = 设计上限的 ~200 倍**，该目标在 page 0 上从未达成。

### 5. ⚠️ 跨 TU 的真实障碍：234 个 `static` 表定义

- `kMethodTable` 是 **`static`**：`static void (*kMethodTable[9625])() = {`
- 有 **9,631** 处引用，且 `kDefaultArgThunks` 用 **lambda 捕获** 它
- 全文件共 **234 个顶层 `static` 表定义**

`static` = **内部链接**，**不能跨 TU 引用**。这是按段拆 TU 的硬约束 ——
不解决它，拆出去就是 C2065/LNK2019。

### 6. ✅ 有利条件：跨 TU 声明机制已存在且在跑

共享头 `native-aot.generated.header.h` 已声明：

| 符号族 | extern 声明数 |
|---|---|
| `chaos_mt_*` | 2,439 |
| `chaos_vtable_*` | 96 |
| `chaos_type_id_*` | 2,438 |

且 page 2..N **今天就在**跨 TU 引用它们（引用 `chaos_object->header.type_info = chaos_mt_X.AsTypeInfoHot()`）。
→ **按段拆 TU 复用既有机制，不是发明新机制。**

### 7. 段间依赖（切 TU 后会变成跨 TU 引用）

| 段 | 引用 `chaos_mt_` | 引用 `chaos_vtable_` | 引用 `chaos_type_id_` |
|---|---|---|---|
| Virtual method table arrays | 10,832 | 192 | 377 |
| Module registration | 25,843 | 0 | 0 |
| GC Slot Map Section | 1 | 96 | 0 |
| Late type-id definitions | 132 | 0 | 2,442（**定义处**）|

## 二、由此得出的设计约束

1. **必须同时改链接性**：把拆出去段内的 `static` 表改成 `extern`（或 `extern "C"`），
   并在共享头补声明。**这是 A 方向的主要工作量**，不是「挪代码」。
2. **段边界要尊重定义/引用配对**：如 `Late type-id`(定义 type_id) → `vtable arrays`(引用)，
   两者若不同 TU，type_id 必须在头里可见（现已可见：2,438 条）。
3. **page 0 必须保留注册入口**：`kMethodTable` / `kDefaultArgThunks` / CodeRegistration
   等被 runtime 以固定符号名查找者，入口位置不能随意挪。
4. **不改 ABI**：符号名不变，只改链接性与所在 TU。runtime 侧零改动。

## 三、待拍板

见 brainstorm 对话。
