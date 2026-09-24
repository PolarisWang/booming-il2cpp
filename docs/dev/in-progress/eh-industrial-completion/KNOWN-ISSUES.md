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

### 追加证据（2026-09-24 第四轮，二分到「9777 字符 → hotpatch section」之间）

**E10. 残片确定存在于 `moduleRegSb`，且 `hotpatch` 是首个能观察到它的 section**

在 `AddSection(...)` 序列的每个调用点后插入片段检测探针，实测输出：

```
[MRB] frag appeared at/after section: hotpatch    (len=43979)
[MRB] frag appeared at/after section: extruntime  (len=69181)
...
[MRB] FINAL hasFrag=True mrbLen=219969
```

`hotpatch` 是**第一个**被探测到的 section，说明残片在它**之前或之内**就已存在。

**E11. `BuildModuleRegistration()` 的返回值是干净的（关键分界点）**

在 `NativeAotLoweringPlanner.Methods.cs:1271` 入参处插桩：

```
[INIT] BMR len=9777  hasFrag=False
```

**`BuildModuleRegistration()` 返回 9777 字符、不含残片。**

于是范围被二分到：

```
BuildModuleRegistration()  → 9777, 干净
        ↓  ← 残片在此区间被引入
hotpatch section           → 43979, 已含残片
```

中间只有两个操作：
1. `var moduleRegSb = new StringBuilder(moduleRegistrationCode, 65536);`
2. `AddSection("modulereg", moduleRegistrationCode);`
3. `foreach (var deferred in _deferredPayloadSections) AddSection(...)`

**第 3 项是当前最重要的嫌疑** —— `_deferredPayloadSections` 由
`AddDeferredPayloadSection(name, content)` 填充，调用点在：

| 位置 | 产出的 section 名 |
|---|---|
| `ModuleRegistration.cs:1183` | `reflmembers_{typeId}` |
| `ModuleRegistration.cs:1277` | `{B3ResolveName}_{part.NameSuffix}` |
| `ModuleRegistration.cs:1335` | `{B3ParamsName}_{part.NameSuffix}` |
| `ReflectionObjectEmission.cs:1970` | `{FunctionName}_{part.NameSuffix}` |
| `ReflectionObjectEmission.cs:2071` | `{FunctionName}_{part.NameSuffix}` |

其中后四者走**函数拆分（拆壳+分包）**机制 —— 项目记忆
`split-function-shell-must-pass-internal-vars-and-declare-cross-tu` 记录过该机制的
两类坑。**下一轮应从这里入手**：打印每个 deferred section 的 name/length，
找出哪一个的内容以残片开头。

**E12. 两次「陈旧 DLL 假阴性」（方法论提醒）**

本轮出现两次探针未打印、看似「路径未执行」的情况，实际都是
**TPG 内嵌的 `Chaos.IL2CPP.Generator.dll` 未同步**（时间戳落后于
`src/managed/.../bin/Debug`）。判据：

```bash
stat -c '%y' src/managed/Chaos.IL2CPP.Generator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll \
             src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll
```

**两者时间戳必须相同**，否则探针结论不可信。这已第二次误导判断。

**E13. `BuildModuleRegistration` 有多个 `return sb.ToString()`**

`ModuleRegistration.cs` 的 714 / 1348 / 1380 行各有一个。插探针时会插错方法体
（714 行的 `return` 属于另一个方法）。定位 return 点必须连同所在方法一起核对。

---

### 建议的下一步（第四次修订）

范围已收敛到**约 34 KB 的单一区间**（9777 → 43979）与**两条语句**。下一步：

1. 在 `_deferredPayloadSections` 的 **drain 循环**里逐个打印
   `deferred.Name` 与 `deferred.Content.Length`，并检测残片 ——
   这能一次性锁定是哪个 deferred section 带进来的；
2. 若无 —— 则只剩 `new StringBuilder(...)` 与 `AddSection("modulereg", ...)`，
   需检查 `BuildModuleRegistration()` 内部**是否存在第二个 return 路径**
   （E13）实际被执行，那样 `INIT` 探针观测到的就不是真正返回的字符串。

> **方法论要求**：每轮插桩前先核对 E12 的时间戳，避免第三次假阴性。

---

### ⚠️ 追加证据（2026-09-24 第五轮）—— 本文件此前第四轮结论被推翻

**E14（最重要）此前所有基于 `IndexOf` 的「残片检测」都是假阳性**

残片字符串 `Chaos_..._AssertionException__ctor_System_S`（77 字符）**本身就是
完整符号 `..._ctor_System_String` 的前缀**。用 `IndexOf(前缀) >= 0` 检测，
在完整符号处同样命中。在真实产物上实测：

```
prefix occurrences (frag):                      38   ← 完整符号，不是残片
TRUE truncations (frag not followed by tring):   1   ← 唯一的真残片
```

**38 : 1。** 因此 E10/E11 中「`[MRB] hasFrag=True`」「`[INIT] hasFrag=...`」
**全部无意义**，第四轮据此得出的「残片在 moduleRegSb / 由 deferred section
引入」推理链**作废**。

> 讽刺的是，本文件的 **E9 已记录过这个陷阱**，随后仍再次踩入。任何后续
> 检测必须使用否定前瞻：`Index(prefix) 后检查 substring(77,5) != "tring"`。

**E15 用正确判据重测的可靠结论（这些才是可依赖的）**

在 `AddSection("modulereg", ...)` 之后、`_deferredPayloadSections` drain 之后、
`BuildModuleRegistration()` 调用处、`aotreg` append 处，各用**否定前瞻**检测：

```
[CHK1] after AddSection(modulereg): mrcLen=9777 mrcHasFrag=False sbLen=9777 sbHasFrag=False
[CHK3] after drain:                sbLen=9777 sbHasFrag=False
[BMR-CORRECT] BuildModuleRegistration(): len=9777  trueTrunc=0
[AOTREG] aotRegistrationCode:            len=42    truncCount=0
```

**结论（推翻第四轮）**：残片**不在** `modulereg`、**不在** deferred sections、
**不在** `BuildModuleRegistration()`、**不在** `aotreg`。

**E16 残片所在位置的最新定位**

全文件唯一的真截断在偏移 **146104**，其前 300 字节为：

```
extern const CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };
extern const CHAOS_IL2CPP_UINT32 kGenericTypeArgTokensCount = 0u;
extern const GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };
...
extern const GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
extern const CHAOS_IL2CPP_UINT32 s_method_aot_entriesCount = 0u;
extern const CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };
extern const CHAOS_IL2CPP_UINT32 s_method_aot_entry_argsCount = 0u;   ← 这 5 行后紧跟残片

extern "C" void Chaos_..._AssertionException__ctor_System_S          ← 残片
```

这一整块（`kGenericTypeArgTokens` … `s_method_aot_entry_argsCount`）由
`Templates/NativeAot.GenericRegistration.cpp.scriban` 产出，**不是** `aotreg`
（`aotreg` 是 `NativeAot.GenericAotRegistration.cpp.scriban`，本 chunk 只 42 字符
的注释）。因此残片出现在 **`crypto` 或 `abimanifest`** 这两个 append 之一之后 ——
这是排除法后仅剩的候选。

**E17 TPG 内嵌 DLL 同步失败（本轮出现 4 次）**

`dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/` 的增量拷贝
**不可靠**：多次出现 `src/tools/.../bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll`
时间戳落后于 `src/managed/.../bin/Debug/net8.0/` 的情况，表现为**探针不打印**，
极易被误读为「该代码路径未执行」（本轮因此产生两次错误推断）。

**可靠做法**（本轮最终采用）：

```bash
dotnet build src/managed/Chaos.IL2CPP.Generator/Chaos.IL2CPP.Generator.csproj -c Debug
cp src/managed/Chaos.IL2CPP.Generator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll \
   src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll
# 核对两者时间戳一致后再跑
```

---

### 收口说明（2026-09-24）

本缺陷历经 **5 轮追查（约 10 个 agent + 30 轮手动）**，仍未定位到 `file:line`。
本轮起停止手动迭代。移交时请以下列**已确证**事实为起点，并**务必使用
E14 的正确判据**：

| 已确证 | 内容 |
|---|---|
| 缺陷形态 | 单行、内容 93 字符、符号名在 77 字符处截断（缺 78 字节） |
| 位置 | 产物偏移 ~146104，紧跟在 `s_method_aot_entry_argsCount = 0u;` 之后 |
| 已排除 | `modulereg` / deferred sections / `BuildModuleRegistration()` / `aotreg`（均有正确判据的探针证据） |
| 剩余候选 | `crypto` 或 `abimanifest` 的 append（排除法，仅剩这两个） |
| 影响面 | `System.ObjectModel` / `System.Collections.NonGeneric`；`System.Linq` / `System.Text.Json` 不受影响 |
| 与 EH 的关系 | **无** —— 属 codegen 组装链缺陷，是 EH 工作期间撞上的 |

**建议的下一步（收敛到单次动作）**：在 `cryptoAotIrCode` 与 `abiManifestCode`
两处 append 之前/之后，用 E14 的否定前瞻判据各检测一次。两者之一必然命中。

---

### 第六轮（2026-09-24）—— 假设检验与守卫尝试，均未命中

**E18 已执行 E16 建议的检测，两个候选均被排除**

用 E14 的正确判据（否定前瞻）实测：

```
[CAND-ABI]    len=16420  trueTrunc=2     ← 但见 E19
[CAND-CRYPTO] len=0      trueTrunc=0     ← 空，直接排除
```

`cryptoAotIrCode` 为空。`abiManifestCode` 虽有匹配，但**产物中 ABI 段在偏移
148079，位于残片（146104）之后** —— 因此 `abiManifestCode` 的内容不可能
插入到残片所在位置。**E16 的两个候选全部排除。**

**E19 `abiManifest` 的"截断"实为探针误读**

`BuildAbiManifest` 把 `native_symbol` 输出为 **C++ 注释**：

```csharp
["native_symbol"] = method.NativeSymbol,   // ModuleRegistration.Dispatch.cs:162
```
```scriban
{ {{ method.return_carrier }}u, {{ method.param_count }}u },  // {{ method.native_symbol }}
```

因此 `abiManifestCode` 内的形态是 `{ 0u, 1u },  // Chaos_..._ctor_System_String`。
`trueTrunc=2` 是**另一个符号**（`Assert_Throws_System_Action` 后被换行截断的
探针显示）造成的计数，与该断言无关。**该候选的有效性已被 E18 的位置论证独立排除。**

**E20 并发假设已排除**

新增 `CHAOS_FORCE_SERIAL_EMIT=1` 环境开关强制串行发射（`emitMethods.Count=66`，
正常走 `Parallel.For`），**残片依然存在**。→ 不是 `EmitOneMethod` 的并发写入问题。

**E21 第二次守卫尝试失败，且失败原因有诊断价值**

在 `NativeAotEmitter` 的三处返回点加 `GuardAgainstTruncatedDeclarations`，
对 System.ObjectModel 实测**守卫一次都未触发**（错误仍是 MSVC 的 C2144/C2182）。
两种可能：

1. 该守卫所在的 `NativeAotEmitter` 函数**不是**产出该文件的那条路径
   （与本文件 E7/E11 的多条"探针未打印"证据一致）；
2. 判据仍有缺陷。

**第一版判据的缺陷已确认并值得记录**：首版只检查「`extern "C"` 行括号不平衡」，
而**真实残片根本不含括号**（参数列表整段丢失）→ `balance == 0` → 被当作合法跳过。
改为「既无 `;` 也无 `{` 也无 `(` 也无 `,`」后仍未触发，倾向于可能 1。

**守卫已回退**：未生效的守卫等于**恒真的假检查**，留在代码里比没有更糟
（会给后续读者"已有防护"的错觉）。

**E22 本轮净结论**

| 假设 | 状态 |
|---|---|
| `cryptoAotIrCode` | ❌ 排除（len=0） |
| `abiManifestCode` | ❌ 排除（段落在残片之后） |
| `EmitOneMethod` 并发 | ❌ 排除（强制串行后仍在） |
| `Methods.Remaining.cs:423` 分次 Append | ❌ 排除（产出的是 `chaos_ensure_type_initialized_*`，非该符号） |
| `modulereg` / deferred / `BuildModuleRegistration` / `aotreg` | ❌ 排除（E15） |

**排除法已经把候选耗尽** —— 这意味着问题可能不在"哪个 append 写坏了"，
而在更上游：**该残片所在的整段文本，可能根本不是这些 append 中的任何一个产出的**
（呼应 E7：ObjectModel 方法体区不经 `BuildMethodSection`）。

**E23 建议的下一步（与前六轮不同的方向）**

不再"逐个排除 append"，改为**从产物反向工程**：

1. 取产物中残片所在位置的**前后完整上下文**（已有：前是 `s_method_aot_entry_argsCount`、
   后是 `// Forward declaration for module.image`），确定它落在**哪个逻辑段**；
2. 用 `git log -S` 追溯「`// Forward declaration for module.image`」这行的引入与
   其拼接代码的历史，看是否曾有过该段的**第二版实现**；
3. 在**产物落盘的最后一步**（`NativeAotGeneratedSource.Contents` 被写入文件之前）
   加一次性 dump，与 `BuildGeneratedSources` 的返回值 md5 比对 ——
   确认损坏是发生在生成期还是**落盘期**。

> 六轮累计约 12 个 agent + 40 轮手动。**建议此缺陷转由熟悉 TuPacker / TP-Step1-3
> 拆分机制的作者处理** —— 远端同期正在推进的 `TuPacker` / 分段协议
> （`SplitReportBuilder.cs` / `TuPacker.cs`）与该文本组装路径高度相关，
> 可能是同一处代码的后续演进。

### 第七轮（2026-09-24）—— 借鉴 dotnet/runtime，**排除发射器本身**

**方法来源**：对照 `D:\OpenSource\dotnet\runtime` 的 ILCompiler（NativeAOT）发射架构后，
采用其 `OutputInfoBuilder`（产物自检表）思路做**逐条核对**，而非继续猜测执行路径。

**dotnet 的关键对照（`src/coreclr/tools/Common/Compiler/`）**

| 机制 | dotnet 的做法 | 我们的现状 |
|---|---|---|
| 名字表示 | `Utf8String` = `readonly struct` 包**不可变** `byte[]`，`Length => _value.Length` | `string` + `StringBuilder` 拼接 |
| 名字缓存 | `ObjectWriter.GetMangledName`：同符号**全产物只计算一次**并冻结 | 无；同名符号可被多次发射 |
| 截断 | **唯一**截断在 `NativeAotNameMangler.SanitizeNameWithHash`，且**截断必配 SHA256 哈希**、有注释 | 发射路径遍布 `[..^1]` / `Substring` / `Regex.Replace` 等**隐式**截断 |
| 发射结构 | **节点树**（`ObjectNode` → 不可变 `ObjectData`）先建好，再由 `ObjectWriter` 统一序列化 | `StringBuilder` 边拼边写 |
| 并发 | `EmitObject()` **单线程**；并行只在**编译阶段**，靠 `CompilerComparer` 全序保证确定性 | `EmitOneMethod` 走 `Parallel.For` |
| 诊断 | `SectionWriter` 是携带 section/offset 的 struct；`IObjectDumper` 可挂载 dump 节点 | 裸 `StringBuilder` |

> dotnet 注释（`Common/Compiler/NameMangler.cs:12`）明确写：
> *"The key invariant is that the mangled names are independent on the compilation order."*

**E24 核心实验：发射清单逐条核对（借鉴 `OutputInfoBuilder`）**

在 `FormatMethodDeclaration` 出口记录**每一次**发射的完整文本（**append-only**，
不用字典按键覆盖 —— 按键覆盖会漏掉「后写的好文本盖掉先写的坏文本」这一假绿），
再在生成后逐条断言该文本**完整出现**于产物中：

```
[EMIT-VERIFY] checked=3  missing=0
```

**三次发射全部完整，且全部存在于产物中。**

**E25 全符号出现点枚举（纯文本核对，不依赖任何路径假设）**

```
off=  6146  line ~112    FULL              ← extern "C" 声明（声明区）
off=144052  line ~6418   *** TRUNCATED *** ← 残片（模块注册段）
off=543017  line ~26506  FULL              ← extern "C" 声明（第二处）
...（另有 34 处引用/注册/调用点，全部完整）
```

**同一个符号共 3 处 `extern "C"` 声明：第一、第三处完整，中间那处被截断。**

**E26 本轮结论（重要，缩小假设空间）**

**发射器本身已被排除** —— 它产出的 3 条声明全部完整，且都能在产物中找到。
残片是**第三条声明**，而它**不是** `FormatMethodDeclaration` 产出的
（三次发射都在清单里且都完整）。

**因此**：残片是**在发射器之外被构造的** —— 某段代码**自己拼了一条
`extern "C" void <符号>` 文本**，而非调用 `FormatMethodDeclaration`。

> **这解释了此前所有的「探针不打印」**：那条路径根本不经过我插桩的那些函数。

**E27 下一步（已收敛为一次定向搜索）**

全局搜索**自己拼接 `extern "C"` 文本**、而非调用 `FormatMethodDeclaration` 的代码：

```bash
grep -rn 'extern \\"C\\" ' src/managed/Chaos.IL2CPP.Generator/ --include=*.cs
```

此前已列出 30 处候选。其中**同时满足**下列三点的即为答案：
① 自己拼 `extern "C" `；② 产出位置在模块注册段（`argsCount` 之后、
`namespacePreamble` 之前）；③ 可能只写出符号名的一部分。

**特别提示**：E21 记录的第一版守卫判据缺陷（只查括号不平衡，而真实残片
**不含括号**）值得后续所有检测借鉴 —— **截断的特征是「缺少」，不是「不平衡」**。




### 第七轮补充（2026-09-24）—— 远端 CR 修复使对照样本变干净；发射器排除得到双重印证

**E28 远端 `b0011cbdd` 修复了 CR 问题，但残片仍在（证明是独立缺陷）**

远端提交 `b0011cbdd fix(driver): FullAssemblyEmitter 写盘归一化 LF` 已在
`FullAssemblyEmitter` 的写盘点加 `text.Replace("\r", "")`。实测效果：

| | 修复前 | 修复后 |
|---|---|---|
| CR 计数 | 大量（`\r\r\r\n` 混合 2/3/4-CR）| **0** |
| 文件大小 | 640,834 | **607,454**（−33,380，即被移除的 CR）|
| 行尾 | 混合 | **纯 LF** |

**但残片依然存在**（offset 146104 → 139687），且形态变得**完全干净**：

```
...s_method_aot_entry_argsCount = 0u;\n\n
extern "C" void Chaos_..._AssertionException__ctor_System_S\n\n\n
// Forward declaration for module.image (defined in Step 3 below)\n
```

→ **KNOWN-ISSUE-1 与 CR/行尾问题无关**，是独立缺陷。**这是目前最干净的对照样本。**

**E29 在真实写盘点复核 `FullAssemblyEmitter` —— 探针仍未打印**

在该文件的写盘分支插入 `EHVerifyWritten`（比对内存文本与磁盘文本、并检测
已知符号截断），实测**无输出**。

结合 E24（发射器清单 `checked=3 missing=0`）与 E26，**双重印证**：

> **该 chunk 的 `native-aot.generated.cpp` 不经 `FullAssemblyEmitter` 落盘。**
> TPG 内部持有自己的 Driver/Generator 副本并直接生成 chunk（这也解释了
> 为什么此前多次 `cp` 因 `Device or resource busy` 失败）。

**E30 本轮确立的完整排除清单（七轮累计）**

| # | 候选 | 排除依据 |
|---|---|---|
| 1 | `modulereg` / deferred sections | E15：9777 字符，trueTrunc=0 |
| 2 | `BuildModuleRegistration()` | E15：9777 字符，trueTrunc=0 |
| 3 | `aotreg` | E15：42 字符（仅注释）|
| 4 | `cryptoAotIrCode` | E18：len=0 |
| 5 | `abiManifestCode` | E18：ABI 段在残片**之后** |
| 6 | 并发（`EmitOneMethod` 的 `Parallel.For`）| E20：强制串行后仍在 |
| 7 | `Methods.Remaining.cs:423` 分次 Append | E22：产出的是 `chaos_ensure_type_initialized_*` |
| 8 | `DeduplicateTypeIdMtSymbols` | E6：PRE-DEDUP 时已在 |
| 9 | Scriban 模板 CRLF | E3：强制 LF 后形态不变 |
| 10 | `PayloadSectionPartitioner` | 死代码，单 TU 不经 section |
| 11 | **`FormatMethodDeclaration`（发射器本身）** | **E24：checked=3 missing=0** |
| 12 | **`FullAssemblyEmitter`（Driver 侧落盘）** | **E29：探针不打印** |
| 13 | CR / 行尾 | **E28：远端修复后残片仍在** |

**E31 收敛后的最终判断**

残片是**第三条 `extern "C"` 声明**（同符号共 3 处：两处完整、中间一处被截断），
而它**不是 `FormatMethodDeclaration` 产出的**。因此：**某段代码自己拼了一条
`extern "C" void <符号>` 文本**，只写出了符号名的一部分。

**下一步（定向搜索，一次即可缩小到个位数候选）**：

```bash
# 自己拼接 extern "C" 文本（而非调用 FormatMethodDeclaration）的所有位置
grep -rn 'extern \\"C\\" ' src/managed/Chaos.IL2CPP.Generator/ --include=*.cs
```

取同时满足以下三点者：
① 自己拼 `extern "C" ` 前缀与符号名（分次 `Append` 或插值）；
② 产出位置落在**模块注册段**（`s_method_aot_entry_argsCount` 之后、
   `// Forward declaration for module.image` 之前）；
③ 其输入可能不完整（例如按索引/长度切片、或依赖某个可能为空的集合）。

**E32 给后续实施者的三条方法论（本轮血泪教训）**

1. **截断的特征是「缺少」，不是「不平衡」** —— 首版守卫只查括号不平衡，
   而真实残片**根本不含括号**（参数列表整段丢失）→ `balance == 0` → 漏检。
   任何检测都应检查「缺少了本该有的终止符」。
2. **检测串必须用否定前瞻** —— `..._ctor_System_S` 是完整符号
   `..._ctor_System_String` 的**前缀**，用 `IndexOf(前缀) >= 0` 会把 38 处
   完整符号误报为残片。用 `(?!tring)` 或检查后续 5 字符。
3. **核对两边时间戳再跑探针** —— `dotnet build` 对 TPG 内嵌
   `Chaos.IL2CPP.Generator.dll` 的增量拷贝**不可靠**（本轮遇到 4 次），
   表现为探针不打印、极易被误读为「该路径未执行」。判据：
   ```bash
   stat -c '%y' src/managed/Chaos.IL2CPP.Generator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll \
                src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Debug/net8.0/Chaos.IL2CPP.Generator.dll
   # 两者必须相同；不同则手动 cp 后再跑
   ```

**E33 转手建议**

七轮累计约 13 个 agent + 50 轮手动追查。本缺陷：
- **不属于 EH 域** —— 是 codegen 文本组装链问题，在 EH 工作期间撞上；
- 与远端正推进的 `TuPacker.cs` / `SplitReportBuilder.cs`（TP-Step1/2/3a
  分段协议）**高度相关**，很可能是同一处代码的后续演进；
- 证据已完整固化（含 13 项排除、3 条反面教材、可复现探针）。

**建议转由熟悉 TU 拆分/分段协议的作者处理**，以 E31 的定向搜索为起点。

---

## KNOWN-ISSUE-2（设计层面）: L3 异常翻译正确性尚未系统化

见 `roadmap-v1-01.md` P2。当前 P2-0（duplicate key）已修复并验证；
C# `throw` 的完整形态清单（`throw new` / `throw;` / `throw ex;` / inner exception /
filter 子句 / finally 中 throw）**尚未穷举与验证**，属于 P2 的待办主体。
