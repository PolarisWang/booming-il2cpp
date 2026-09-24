# EH 工业化 — 已知未解决缺陷记录

> **task_id**: eh-industrial-completion
> **记录日期**: 2026-09-24
> **状态**: 未解决（已排除大量候选，根因未定位到单一 `file:line`）

---

## KNOWN-ISSUE-1: native-aot.generated.cpp 中一条声明被行中删除 78 字节

### 现象

产物 `artifacts/foundation-dll/System.ObjectModel/chunks/global-ns/native/codegen/generated/native-aot.generated.cpp`
中出现一条**被行中删除**的声明（字节偏移 146907，文件 md5 `160f1592debe0cd36fe09b174b58bb51`，640834 字节）：

```cpp
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_S
```

- 干净孪生体（偏移 6260 / 555205）长度 **170 字符**，带完整参数列表与 `;`
- 损坏体 **93 字符**
- **缺失 78 字节** = `tring(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);`
- 公共前缀恰为 93 字节 —— 是**删除**，不是截断（截断会保留行终止符形态）

### 编译症状（具误导性）

```
error C2182: 'Chaos_..._AssertionException__ctor_System_S': this use of 'void' is not valid
error C2144: syntax error: '...ReflectionQueryImageDescriptor' should be preceded by ';'
```

C2144 **报的是下一条声明**，与真正的损坏行无关 —— 这是它长期未被正确归因的原因。

### 最强证据：行尾编码分区

在 `[55318, 146908)` 这 **1165 行**区间内：

| 行尾编码 | 计数 |
|---|---|
| 4-CR (`\r\r\r\r\n`) | **1164** |
| 3-CR (`\r\r\r\n`) | **1** ← 损坏行本身 |

全文件直方图：`{CR-run 2: 43, CR-run 3: 8948, CR-run 4: 1737}`

**损坏行是 1165 行中唯一由不同写入路径产生的行。** 4-CR 来自 `DeduplicateTypeIdMtSymbols`（`AppendLine` 对已含 `\r` 的行再加一个 `\r`）；损坏行是 3-CR，即**未经该 pass** 的原始渲染形态。

### 已排除的候选（均有一手依据）

| 候选 | 排除依据 |
|---|---|
| `MethodDeclarations` 截断 | `MethodEmission.cs:40` 恒带 `(`；损坏行无 `(`；元素在 6260 处完整 |
| Scriban 模板过滤器 | 模板仅 `for` 循环，无 truncate/substring |
| `NormalizeIndentation` | `ScribanTemplateRenderer.cs:50-55` 只改行首 4 空格 |
| `RenderTemplate(...).TrimEnd()` | 仅裁尾 |
| `StringBuilder` 容量估算 | 只会重分配，不截断 |
| `PayloadSectionPartitioner` | **死代码**（全仓无调用点）；单 TU 输出 |
| `DeduplicateTypeIdMtSymbols` | 行原子（只有 `AppendLine(整行)`）；产出 4-CR，损坏行 3-CR |
| `abiManifestCode` / `aotRegistrationCode` | 探针验证头尾均干净 |
| `moduleRegSb` 自身构造 | 探针验证：`fragIdxInModuleRegSb` 命中的是注册表条目（完整），非残缺声明 |
| Scriban **渲染边界** | 探针验证：BEFORE render (222284) 与 AFTER render (618124) 两处该符号**均完整** |

### 插桩探针证据（可复现）

在 `NativeAotLoweringPlanner.Methods.cs` 与 `NativeAotEmitter.cs` 注入的探针输出：

```
[EH5] BEFORE-render mrc.len=222284 frag=8918
[EH5] mrc.seg=[Chaos_..._AssertionException__ctor_System_String\r\n        { 0u, 1u },  // ...]
[EH5] AFTER-render rendered.len=618124 frag=6025
[EH5] rendered.seg=[Chaos_..._AssertionException__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, ...)]
```

**渲染前后都完整** → 损坏必然发生在其后，但两条返回路径（Scriban 路径 `NativeAotEmitter.cs:332`、direct 路径 `:636`）的探针 `[EH6]` **均未打印**，说明该 chunk 走的是**第三条未定位的路径**。

### 尝试过的守卫方案（全部失败，勿重试）

目标是「把静默腐蚀变成响亮失败」。四次判据迭代，**全部误伤合法产物**：

| # | 判据 | 误伤对象 |
|---|---|---|
| 1 | 行尾 ∈ {`;` `{` `(` `,`} 白名单 | `extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_resolve_method_handle(...) noexcept` |
| 2 | 行尾非标识符字符 | 同上（`noexcept` 以字母结尾） |
| 3 | 行内括号不平衡 | `extern "C" void InterpreterEntryDirect(`（合法多行声明） |
| 4 | 括号不平衡 + 下一行非续行 | 实现受阻（转义/构建问题），未验证 |

**结论：行级启发式无法可靠区分「合法多行声明」与「行中删除」。** 若后续再尝试，必须先建立一个**同时包含合法多行声明与真实损坏行的对照样例集**，再设计判据。

### 影响面

| assembly | 状态 |
|---|---|
| `System.Linq` | ✅ 通过（386/386）—— 经 P2-0 修复后 |
| `System.Text.Json` | ✅ 通过（517/517）|
| `System.ObjectModel` | ❌ 阻断 |
| `System.Collections.NonGeneric` | ❌ 阻断 |

### 建议的下一步（如需继续）

1. 在 `BuildGeneratedSources`（`NativeAotEmitter.cs:93`）**入口与出口**各 dump 一次产物文本，用 md5 比对，确定损坏是否发生在该函数内。
2. 若在内部，逐个 `BuildGeneratedPage*` 重载加同样的 md5 探针，二分定位。
3. 若不在，检查 `FullAssemblyEmitter.Emit` 之后是否有第三方改写产物文本的 pass。

> **不要**再用行级启发式写守卫，除非先有对照样例集。

---

## KNOWN-ISSUE-2（设计层面）: L3 异常翻译正确性尚未系统化

见 `roadmap-v1-01.md` P2。当前 P2-0（duplicate key）已修复并验证；
C# `throw` 的完整形态清单（`throw new` / `throw;` / `throw ex;` / inner exception /
filter 子句 / finally 中 throw）**尚未穷举与验证**，属于 P2 的待办主体。
