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

### 追加证据（2026-09-24 第二轮，含一手实测）

**E1. 缺陷位置与内容在多次重跑间完全稳定**

用正则 `extern "C" void Chaos_..._AssertionException__ctor_System_S(?!tring)` 全文件扫描，
每次运行都恰好在**同一位置、同一符号、同一前后文**命中：

```
[前] extern const CHAOS_IL2CPP_UINT32 s_method_aot_entry_argsCount = 0u;
[残] extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_S
[后] // Forward declaration for module.image (defined in Step 3 below)
```

不是随机腐蚀 —— 是一个**稳定产生**的错误输出，可复现、可二分。

**E2. 与成功 assembly 的对照揭示结构差异（本轮最重要的新证据）**

对比 `s_method_aot_entry_argsCount = 0u;` **之后**的内容：

| assembly | 之后的内容 |
|---|---|
| `System.Linq` ✅ | `// Managed method: ...::AreEqual(...)` + 完整 extern 声明 |
| `System.Text.Json` ✅ | 同上，**逐字节相同** |
| `System.ObjectModel` ❌ | **直接是残缺声明**，无 `// Managed method:` 注释 |

**结论**：ObjectModel 的**方法体区整体缺失**，残片占据了它的位置。
换言之残片不是「某行被删了 78 字节」，而是**本该输出的一整块（注释 + 完整声明）被一段 93 字符的文本替代**。

**E3. 排除了「模板 CRLF」这一候选**

`d6f3e4a18`（Scriban 模板强制 LF）与本缺陷**无关**，已实测排除：

- 该 commit 已给 `.gitattributes` 加 `*.scriban text eol=lf`；
- 但工作区模板**仍是 CRLF**（规则只影响后续签出，已存在的文件不自动转换）；
- 手工重新签出全部模板为 LF 后重建、重跑，**缺陷形态不变**（仅 CR-run 从 3 降到 2，与周围行同步下降）。

**E4. 残片形态与所有已知发射函数不符（关键约束）**

逐个核对了可能产出该文本的代码，**全部恒带 `(`**，而残片**没有 `(`**：

| 函数 | 产出形态 | 是否匹配 |
|---|---|---|
| `FormatMethodDeclaration` (`MethodEmission.cs:27-41`) | `extern "C" {ret} {sym}({params});` | ❌ 恒有 `(` |
| `FormatMethodDeclaration` (static, `:40`) | 同上 | ❌ |
| `BuildMethodDeclarations` (`:53`) | 调上面那个 | ❌ |
| `EmitManagedMethod` 头部 (`:305-308`) | `// Managed method:` 注释 + 声明 + `{` | ❌ 且注释缺失 |

**插桩实测**（`EmitManagedMethod` 入口）：
```
[EHF] EmitManagedMethod subject=...AssertionException::.ctor:System.Void(System.String)
      retAbi=Void  paramCount=2
```
函数**被调用了**且**参数存在**（`paramCount=2`）—— 但产物里既没有 `// Managed method:` 注释，
也没有参数列表。说明产出残片的**不是** `EmitManagedMethod` 的正常路径。

**E5. 数值指纹（供后续比对）**

| 量 | 值 |
|---|---|
| 残片全长 | 93 字符 |
| 其中 `extern "C" void ` 前缀 | 16 |
| 保留的符号名长度 | **77** |
| 完整符号名长度 | 82 |
| 完整行长度 | 171 |
| 缺失 | 78 字节 = `tring(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);` |

全 artifacts 扫描显示「77 字符的截断符号」共 3 处，**全部是这同一个符号**。

---

### 建议的下一步（修订版，基于 E2/E4）

E2 把问题从「某行被删字节」重新定义为「**一整块输出被替换**」。因此下一轮的入手点应是：

1. **在方法体区的组装处**打点，而非在单行发射处。具体：`NativeAotEmitter` 里
   `methodSections` 的构造与 append（`NativeAotEmitter.cs:518-522` 的
   `foreach (var section in methodSections)`），比对该循环在 ObjectModel 与
   System.Linq 下的输入差异。
2. 检查 `FullAssemblyEmitter.Emit` 里 `subjectMethods` 过滤是否影响方法体区的生成 ——
   ObjectModel 的 subject 数是 10，System.Linq 是 386，**过滤路径可能不同**。
3. 若仍无进展，用「同一 assembly 分别以 full / 非 full closure 跑」制造对照，
   观察残片是否只出现在某一条路径。

> **不要**再用行级启发式写守卫，除非先有对照样例集（四种判据均已实测误伤，见上表）。

---

### 追加证据（2026-09-24 第三轮，定位到「哪条路径」）

**E6. 损坏在 `DeduplicateTypeIdMtSymbols` 之前就已存在**

在 `BuildGeneratedSources` 的返回值处（`NativeAotEmitter.cs` 的
`_pageText` / `_finalText`）双向探针，实测：

```
[FINAL-PRE-DEDUP ] afterArgs = ...argsCount = 0u;\r\r\n\r\r\n extern "C" void ..._System_S\n\n\r\n // Forward declaration ...
[FINAL-POST-DEDUP] afterArgs = ...argsCount = 0u;\r\r\r\n\r\r\r\n extern "C" void ..._System_S\r\n\r\n\r\n // Forward declaration ...
```

PRE-DEDUP 时残片**已经存在** → `DeduplicateTypeIdMtSymbols` **不是**成因（此前
Workflow 调查也得出同一结论，此处独立复现）。

**E7. `BuildMethodSection` 在 ObjectModel 上未被调用 —— 本轮最有价值的线索**

在 `NativeAotEmitter.Shared.cs:871` 的 `BuildMethodSection` 入口插桩（条件：
`SubjectId` 含 `AssertionException`），**探针一次都没有打印**。

结合 E2（ObjectModel 在 `argsCount` 之后**缺失 `// Managed method:` 注释**），
结论一致且更强：

> **ObjectModel 的方法体区根本没有经由 `BuildMethodSection` 这条正常路径生成。**

因此这条残片并非「某处把一行写坏了」，而是**另一条（未走 `BuildMethodSection`
的）路径产出的文本**，它恰好占据了本该属于方法体区的位置。

**E8. `chaos_generated_module.cpp` 中同一符号是完整的**

同一 chunk 的 `chaos_generated_module.cpp:143` 里：

```cpp
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(
            CHAOS_IL2CPP_INTPTR,
            CHAOS_IL2CPP_INTPTR
);
```

**完整、格式正确**。两个文件用的是同一份用例数据，因此**数据源是好的** ——
损坏发生在 `native-aot.generated.cpp` 独有的某条生成路径上。

**E9. 数值指纹复查（`IndexOf` 语义提醒）**

`fragShort`（`..._ctor_System_S`）与 `fragFull`（`..._ctor_System_String`）的
`IndexOf` 结果**相同（都是 6205）** —— 因为前者本就是后者的前缀。
**判断残片时必须用 `(?!tring)` 之类的否定前瞻**，否则会把完整声明误当残片，
这一点此前让我多绕了两轮。

---

### 建议的下一步（第三次修订）

E7/E8 把搜索范围**缩小到「ObjectModel 独有的、不走 `BuildMethodSection` 的生成路径」**。
下一轮应：

1. 枚举 `BuildMethodSection` 的**所有调用点**，找出在什么条件下会被绕过；
2. 对照 `System.Linq`（走正常路径）与 `System.ObjectModel`（绕过）在
   `pageMethods` / `TemplateModel` 构造上的差异 —— 特别关注 E2 提到的
   subject 数差异（10 vs 386）；
3. 检查 `BuildGeneratedPageDirect` 的触发阈值
   （`NativeAotEmitter.cs:291`：`objectModelLength > 200_000`）——
   System.Linq 的产物 809 KB、ObjectModel 638 KB，**两者都可能触发**，
   需实测确认各自走哪条分支。

> 前三轮共投入约 10 个 agent + 20 余轮手动追查仍未定位到 `file:line`。
> 若下一轮仍无收敛，建议改由「用最小可复现 assembly 二分缩小输入」的方式
> 替代「读代码找路径」。

---

## KNOWN-ISSUE-2（设计层面）: L3 异常翻译正确性尚未系统化

见 `roadmap-v1-01.md` P2。当前 P2-0（duplicate key）已修复并验证；
C# `throw` 的完整形态清单（`throw new` / `throw;` / `throw ex;` / inner exception /
filter 子句 / finally 中 throw）**尚未穷举与验证**，属于 P2 的待办主体。
