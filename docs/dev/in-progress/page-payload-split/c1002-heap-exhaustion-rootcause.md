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

---

## 补充实测（2026-09-21，Phase 2 输入）

### 跨段 static 引用：**仅 6 个**，非先前估计的 234

单遍扫描真实产物（1,236,193 行 / 71 MB）实测：

| 指标 | 实测值 |
|---|---|
| 唯一 `static` 表符号 | **7,716** |
| 其中**被其它段引用**（须改 `extern`） | **6** |

被引用的 6 个，全部是 `Registration calls` 定义、`VTable descriptors` 引用：

```
kGenericTypeArgTokens      kGenericTypeEntries
kGenericMethodArgTokens    kGenericMethodEntries
s_method_aot_entries       s_method_aot_entry_args
```

**两个独立方法互相印证**：
1. 本扫描（按产物段归属）
2. 守卫测试 `PayloadTables_AreNotStaticWhenReferencedAcrossSections`
   独立报出 `kGenericTypeArgTokens`

### ⚠️ 修正先前记载

先前写「**234 个顶层 static 表**」——该数字是**定义总数**（正则口径不同），
不是**需要改链接性的数量**。真正需要跨 TU 的只有 **6 个**。
→ `pps-2-extern` 工作量由「大规模改造」降为**小范围改造**。

### 链接性技术约束（实测）

| 写法 | 跨 TU 可用性 |
|---|---|
| `static constexpr T k[] = {...}` | ❌ 内部链接 |
| `extern constexpr T k[];`（仅声明） | ❌ **C2737: constexpr object must be initialized** |
| `extern const T k[];` + 定义处 `extern const T k[] = {...}` | ✅ **编译干净**，且反汇编与 static 版一致 |

→ **必须去掉 `constexpr`**，改为 `extern const`。
（`constexpr` 要求每个使用点可见初始化器，与跨 TU 声明互斥。）

### 引用形态（P1 证据）

`kGeneric*` 族的引用全部是 `sizeof(kGenericTypeArgTokens)/sizeof(...)`
与结构体字段赋值 —— **编译期常量折叠**，无运行期间接寻址。
→ 改 `extern const` 后，若 TPG/构建把泛型注册段与 vtable 段分开编译，
需确认 `sizeof` 在声明可见处仍可求值（数组维度必须在声明中给出）。

*注：`sizeof` 需要对完整类型求值。声明处若写成 `extern const T k[];`（不完整类型），
`sizeof` 将编译失败。**实施时须验证**：要么声明带维度，要么该表达式移到定义所在 TU。*
