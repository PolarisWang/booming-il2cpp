# system chunk C1002/C1060（MSVC 堆耗尽）— 根因已定位

> 日期：2026-09-21
> 状态：**根因已定，未修复**
> 症状：`native-aot.generated.cpp(571920): fatal error C1002: compiler is out of heap space in pass 2`

## 现象

system chunk 构建失败。**与分号缺陷无关**（错误从不落在 `kExcDesc` 行）。
**空闲机器单跑同样失败** → 不是并发争用。

## 根因：page 0 的「非方法」载荷不分页

`NativeAotEmitter.Shared.cs` 的分页器**只切分 `templateModel.Methods`**，
而 page 0 额外挂载的对象模型 / 注册表 / GC slot map **整体不分页**。

实测产物：

| 文件 | 行数 |
|---|---|
| `native-aot.generated.cpp` (=page 1) | **1,236,192** |
| `native-aot.generated.page2.cpp` | 15,110 |
| `native-aot.generated.page20.cpp` | 16,481 |

page 1 比其它页大 **82×**。文件命名 `pageSuffix = i == 0 ? "" : $".page{i+1}"`
使 page 1 恰好叫 `native-aot.generated.cpp`，与「单体 TU」同名 ——
此前（含我）误判为「分页失效退回单体」，实际是**分页生效但 page 0 的载荷没被切**。

### page 1 的构成（按段实测）

| 段 | 行数 | 占比 |
|---|---|---|
| Module registration | ~508,000 | 41% |
| GC Slot Map Section | ~308,000 | 25% |
| Virtual method table arrays | ~127,000 | 10% |
| 其余 15 段（ABI manifest / hotpatch index / 各类 subject 表 / 反射描述符…） | ~290,000 | 24% |
| `chaos_mt_*` MethodTable **定义** | **277** | ~0% |

> ⚠️ 修正：本文档初稿曾写「page 1 承载整个对象模型（36,808 处 chaos_mt_）」。
> 36,808 是 `chaos_mt_*` 的**引用**次数（含方法体内的 `.AsTypeInfoHot()`），
> 不是定义。真正的 MethodTable 定义只有 **277 行**。对象模型**不是**主因，
> 主因是 **Module registration + GC Slot Map 明细分页**。

## 机制（`Shared.cs:155-232`）

```csharp
int page0Overhead = objectModelLength + MethodDeclarations.Sum(..)
                  + ModuleRegistrationCode.Length + GenericRegistrationCode.Length + 500;
...
bool isFirstPage = (actualPageIdx == 0);
int accumulated = isFirstPage ? page0Overhead : perPageOverhead;

while (methodIdx < totalMethods) {
    int methodSize = (allMethods[methodIdx].MethodSource?.Length ?? 0) + perMethodOverhead;
    if (accumulated + methodSize > sizeThresholdChars && methodIdx > pageStart) break;
    accumulated += methodSize; methodIdx++;
}
```

`page0Overhead` 在 system chunk（49086 方法）上是**几十 MB 量级**，
单它一项就远超 `sizeThresholdChars = 350_000`。
循环只在 `methodIdx > pageStart` 时 break，于是 page 0 = 全部载荷 + 极少数方法。

**350 KB/页的设计目标在 page 0 上从未达成**（实际 1.2M 行 / 71 MB）。
该常量注释明写「避免 MSVC C1060（触发于 ~15MB / 448K 行）」。

## 影响面

| chunk | 产物行数 | 状态 |
|---|---|---|
| system | 1,236,192 | ❌ C1002 |
| reflection | 164,451 | ✅ 通过 |
| threading | 88,786 | ✅ 通过 |

## 修法（未实施）

原「方案 D（对象模型拆独立 TU）」**经实测范围有误** —— 对象模型只占 277 行。
真正该拆的是 **Module registration（41%）与 GC Slot Map（25%）**。

| # | 方案 | 说明 |
|---|---|---|
| **D'** | 把 page 0 的**大载荷段**（module registration / GC slot map / vtable arrays）拆成独立的**数据 TU**，page 0 只留注册入口 | 与 D 同构，但作用对象正确 |
| E | 让 `page0Overhead` **不参与** `accumulated` 初值，仅用于日志 | 最小改动，但 page 0 仍会因载荷本身过大而爆 —— 治标 |
| B | 给失败 TU 加 `/Zm`、限制 `/MP` 并发 | 环境侧缓解；`/MP` 模板里无上限 |
| A | 大载荷段内部再按条目切分 | 最彻底，改动最大 |

## ⚠️ 方法学

1. 前两次把失败归因于「并发争用」（`/MP` 无上限确有放大），**空闲单跑仍失败**才排除。
   判据：**先做空闲对照，再谈争用**。
2. 本文档初稿把「`chaos_mt_` 出现 36,808 次」当成「对象模型是主因」，
   未区分**引用**与**定义**。按段实测后才纠正。**判据：统计符号要分清
   定义处 / 引用处**（同 [[grep-scope-decides-caller-conclusion]]）。

相关：[[tpg-bundles-stale-generator-dll]]、[[generated-page-files-stale-orphans]]

### 🔴 实施阻断与解法（实测，2026-09-21）

**问题**：`kGeneric*` 族的引用形态是
```cpp
.generic_type_arg_count = sizeof(kGenericTypeArgTokens) / sizeof(kGenericTypeArgTokens[0]),
```
`sizeof` 需要**完整类型**。而跨 TU 声明若写成不完整数组：

```cpp
extern const U32 kTbl[];        // 不完整
sizeof(kTbl)                    // ❌ C2070: illegal sizeof operand
extern const U32 kTbl[2];       // 带维度
sizeof(kTbl)                    // ✅ 编译通过
```

**难点**：这些数组是**动态长度**的（每程序集的 token 数不同），
共享头**无法**预知维度。

**解法（已验证可编译）**：额外发射一个计数标量，消费方改用它：

```cpp
// 定义 TU
extern const U32 kGenericTypeArgTokens[] = { ... };
extern const U32 kGenericTypeArgTokensCount  = 2;

// 共享头（声明）
extern const U32 kGenericTypeArgTokens[];
extern const U32 kGenericTypeArgTokensCount;

// 消费方
.generic_type_arg_count = kGenericTypeArgTokensCount,   // 不再用 sizeof
```

实测 `extern const U32 kTbl[]; extern const U32 kTblCount;` + 引用标量
→ **编译干净**。

**影响**：
- `NativeAot.CodeRegistration.cpp.scriban:74-85` 共 **8 处** `sizeof(x)/sizeof(x[0])`
  需改引用计数标量
- `NativeAot.GenericRegistration.cpp.scriban` 需为每个数组补发计数标量
- **这是 `pps-2-extern` 的真实工作量**（不是「加个 extern 关键字」那么简单）

⚠️ 若不做此改造而直接 `extern const T k[];`，**在拆分 TU 之前就会 C2070**
（同类引用在同 TU 内也会失败）—— 因此该改造必须**先于**拆分落地，
且在**不拆分**时也保持可用（计数标量与 sizeof 等价的守卫测试）。

---

## pps-3 实测发现（2026-09-21）

### 段边界拆分不够：存在**超大单段**

启用段分页后，守卫测试报出：

```
a payload translation unit is 2,302,268 chars, over the 1,400,000 limit
  Sections in it: hotpatch(2,302,268)
```

`hotpatch` 段**单段 230 万字符**，超预算 6.5 倍。段边界分页对它无效 ——
它在自己的 TU 里仍然超限。

**这是 pps-3 未完成的真实原因**（不是测试写错）。后续需要：
- 对**超大单段**做**段内二次切分**（该段的子结构：`s_hotpatch_slots[]` /
  `s_hotpatch_types[]` / `s_hotpatch_methods[]` 等可按条目切）
- 或把该类段按**条目粒度**直接产出多个 section

### 全量对照（判无关的唯一判据）

| 配置 | 失败数 |
|---|---|
| 禁用段分页（≈baseline） | **17** |
| 启用段分页 | **2**（已知 flake + 上述预期红） |

段分页**大幅减少**失败（负载相关 flake 的放大效应），且未引入新失败。

### 阶段结论

`pps-3a`（分页器）已完成并验证；`pps-3b`（接入发射路径）已接线且无回归，
但**尚未达成「无 TU 超预算」的退出判据** —— 阻塞在超大单段的段内切分。
