# JIT 分配质量 + 性能线 — 完整工作计划（重新梳理后 v3）

> **日期**：2026-08-12（基于真实运行实证修正后）
> **状态**：权威续跑计划。取代 `cross-platform-unify-plan.md` 中失焦的 §1-7 旧结构；§9 性能线并入本计划。
> **核心修正**：经预编译 AOT entry.exe **实际跑码**，推翻「中间层普遍 fake-return / 落解释器」假设。

---

## 0. 定位：这套工作在整个 il2cpp 里处理什么

**在「GC 必须扫到存活引用」+「方法必须有真实 native 执行」这两个正确性前提下，让 JIT 生成的机器码质量更高、且 AOT 跨 assembly 调用不落回解释器/假返回。** 纯性能 + 正确性收敛，不改 IL/翻译/HotUpdate 语义。

---

## 1. 已实证的现状（2026-08-12 真实运行数据）

### 1.1 AOT 中间层：健康，仅 1/242 失败（推翻旧假设）

用预编译 AOT product 跑 fact（`RunFactAll`，遍历 subject，`ChaosDispatchMethodGetValue` 比对实际返回值）：

| Assembly | fact 结果 |
|---|---|
| System.Linq | **157/157** |
| System.Private.CoreLib/buffers | **23/23** |
| System.ObjectModel | **10/10** |
| System.Collections | **51/52** |
| **合计** | **241/242（99.6%）** |

**结论 A**：`kChaosExternalRuntimeFnTable` / dispatch / interpreter 路径对 **subject 方法** 健康。我此前「普遍 fallback return-0 / 落解释器」**错误，已推翻**。

**结论 B（1 个真实 failure）**：`System.Collections` index96 = `...CollectionsUtilTests::CreateCaseInsensitiveHashtable_1_int_0` **返回 0、checksum 失败**。它在全源码树**无 C++ 函数体定义**、`Hashtable`/`CollectionsUtil` **无 native stub**、但注册了 `kHotpatchKeepNative`。→ fallback `return 0` 路径**真实存在**，但**罕见**（1/242），是定点正确性缺口，非全局面板。

**结论 C（checksum 语义）**：subject 返回的是**哨兵 42/0**，`42` = 方法完整执行到返回标志，`0` = 返回 0（可能是合法 void/零值，也可能是 fallback return-0）。fact 能抓"崩溃/未执行到末尾"，但**不能保证复杂跨 assembly callee 的字节级正确**（42 哨兵会掩盖 `Buffer.Memmove` 是否真拷贝正确）。

### 1.2 寄存器分配：已达 x64 硬件极限

`CHAOS_IL2CPP_CODEGEN_STATS` 实测 1091 方法：GPR load 命中 **97.5%**、spill 623、write-through 7268（arg-reg 基线）。x64 持久池 = R12-R15（+Win64 RDI），RAX/RCX/RDX 被 Div/Mul/Shift 隐式 clobber、RBX/RSI/RBP 架构保留 → **扩持久预算无余量且非主导**。

### 1.3 T4.1 基准可信度：已由既有 harness 覆盖

adaptive + outlier + FP-12 零时长守卫 + `_MIN_ELAPSED_FLOOR` + perf-store overwrite 修复均已就绪。

---

## 2. 已完成（本 session 交付）

| Commit | 内容 | 验证 |
|---|---|---|
| `7e69de781` | 方案3 捕获原语下沉 `PalCaptureReliable()` + JIT `kReliableRegisterCapture`；方案4 deopt 精确 spill `RecordDeoptValues`（union live@deopt+osr）统一 5 deopt 点 | test_jit_native 70/70 + osr/gc/abi/pgo/seh/simd/inline/mode/unwind/il_smoke 全绿 |
| `27e7cf718` | 性能线记录：x64 持久寄存器扩无余量、T4.1 已覆盖 | — |
| `6d64ef994` | 消除中间层**调研**（真因初判 = 跨 assembly callee 落解释器） | 文档 |
| （本次） | **实证修正**：241/242 通过，推翻初判 | fact 实测 |

---

## 3. 方向 A 的重定位（核心修正）

### 3.1 原假设（已推翻）
> "跨 assembly InternalCall 普遍落 `ChaosExternalRuntimeFallback` → 解释器 → 2.5-30× 普遍。"

### 3.2 真实现状（实证）
- subject 方法走 native direct_ptr，中间层健康（241/242）。
- fallback return-0 只落在 **「无 C++ body 定义 + 无 native stub」** 的少数方法（已抓到 1 个：`CreateCaseInsensitiveHashtable`）。

### 3.3 方向 A 的价值修正
从「普遍性能面板」→ **「定点正确性 + 性能修复」**：

> **让每个「有真实调用需求、但 codegen 没产出 body / 没有 native stub / 落到 fallback return-0」的方法拥有真实 native 执行体。** 对已正常的方法，方向 A **无改动、无回归**；对有缺口的方法，消除错返回 + 获得真实执行/性能。

### 3.4 需要先回答的关键问题（决定优先级）
**这类失败方法在真实 workload 里有多少？**
- 现在只有 System.Collections 抓到一个。需要 **fd-verification fact 覆盖率报告** 跨全部 20 个 entry 盘点「返回 0 但预期非 0」的方法= 方向 A 的**定点清单**。
- 若清单小（如 < 十几），方向 A 是低量定点修复；若大，才值得做机制层（如扩 ShapeRegistry / 编译闭包）。

---

## 4. 新工作计划（重新打分优先级）

> ⚠️ 优先级调整：原「消除中间层」不是宽泛优化，而是**定点正确性修复**；其价值取决于 §3.4 的清单规模。

### 优先级 P0 — 定盘点法（最高，先做，数据驱动一切）
**任务 A1**：跨全部预编译 entry（20 个）跑 `--fact-json`，盘点「返回 0 但预期非 0」的方法清单。
- 产出：`docs/dev/in-progress/jit-regalloc-a2/external-fallback-gap-report.md`（方法 + 所属 assembly + 是否无 body + 是否无 native stub）。
- 验收：给出方向 A 的**完整目标清单 + 规模**，据此决定走定点还是机制。

### 优先级 P1 — 定点方向 A（依 A1 清单，分批）
**任务 A2**（浅层，便宜）：对清单中**有 native 需求但缺注册**的方法，在 ShapeRegistry 补 SimpleForward（类似 `RuntimeHelpers::InitializeArray`）。如 `Buffer::Memmove` 已证有 `ChaosBufferMemmove` stub、ABI 可核对。
**任务 A3**（深层）：对清单中**无 C++ body 且无 stub** 的方法（如 `CreateCaseInsensitiveHashtable` 需要 `Hashtable` 语义），判断是「补 codegen 让方法进编译闭包」还是「补 native stub」。这决定是否值得做机制层。
- 验证：这些方法 fact 从失败转通过；相关 benchmark `dispatch_overhead` 减少。

### 优先级 P2 — 机制层（仅当 A1 清单足够大）
- 扩 ShapeRegistry shape 集 / 让跨 assembly callee 进 `_methodsBySubjectId` 编译闭包。机制已有（SimpleForward → direct native call），只是覆盖面和 ABI 核对工作量大。
- 依赖 A1 数据显示缺口的量与形态。

### 优先级 P3 — 寄存器线（已近硬件极限，仅保留）
- 无新 x64 动作。可选 ARM64 持久池（12 vs allocator 10），但测试盲区，非优先。

---

## 5. 待确认项 / 风险

1. **A1 清单规模**是方向 A 走「定点」还是「机制」的分水岭——**必须先量**。
2. **checksum 局限**：42 哨兵掩盖深层字节正确性。若要验证 `Buffer::Memmove` 真拷贝正确，需专门构造带非平凡 payload 的测试（fact 当前抓不到）。
3. 方向 A 修复需完整 AOT 重构建 + fact 回归（本环境只有预编译二进制，改 codegen 后需重建才能验证）。
4. 不回归保证：方向 A 只动「当前落 fallback return-0」的方法，不影响已正常路径。

---

## 6. 立即下一步（建议）

**执行任务 A1（定盘点法）**——用现有 20 个预编译 entry 跑 `--fact-json`，盘点所有「返回 0 但预期非 0」的方法。这一步不需要改任何代码、立即能做、直接决定方向 A 的工作量。跑完后给你一份带真实规模的缺口报告，再定 A2/A3 怎么排。

要我现在执行 A1 吗？

---

## 8. A2 验证能力打通（2026-08-12，关键 enablement）

**重建-验证环已验证可用**：设 `CHAOS_FOUNDATION_DLL=D:\agent\chaos-il2cpp\tests\e2e\translation` 重定向，`chunk_pipeline --chunk <c> --stages build,fact` 能：
1. 从源码重建 chunk 的 entry.exe（numerics 实测 ~29s，Convert→TPG→CMake→entry）
2. 对重建产物跑 fact 验证（numerics 重建后 16 失败，mostly Vector _All）

**已落地**：`fix(codegen) Unsafe.As passthrough`（commit `d0d101236`）—— 把 Unsafe.As 从 FAIL stub 改为 `return chaos_arg_0` 透传地址。Generator Release 编译 0 error（未重建验证，因 Unsafe 不在 numerics chunk 内）。

**A2-1 剩余（Vector SIMD，现可验证）**：numerics 重建后失败集中在 `Vector2/3/4::GreaterThanAll/LessThanAll/...`（`_All` 算子，value=0 或 32/64 部分match）。实现正确 `_All` 语义需先确认 Vector2/3/4 在该 codegen 的内存布局（X/Y/Z 分量存储 + bool 返回编码）。验证：重建 numerics + fact passed=false→true。

**后续 A2/A3 均可用此环验证**：Interop GCHandle/SafeBuffer、Xml importers、PipeReader、NameTable。

### 9. A2-1 Vector SIMD 实施进度（2026-08-12）

**已落地（commit 23e355b5f）**：runtime 新增 Vector2/3/4 的 `GreaterThanAll/GreaterThanOrEqualAll/LessThanAll/LessThanOrEqualAll` 12 个 native 算子（numerics_vectors.cpp + runtime_core.h 声明），逐 lane 比较 + AND 归约，bool→CHAOS_IL2CPP_INT32 1/0 匹配 fact 契约。chaos_runtime_core 编译 0 error。

**精确定位的 codegen 缺口（下一步）**：`RuntimeSkeletonVectorKernelCore.FixedPlans.cs:111 TryCreateFixedComparisonPlan` 的 `_All` 分支（line 210/250）只匹配**泛型 `Vector<T>`** 签名（`managedVectorTypeName<scalarManagedType>`，carrier = RuntimeIntrinsicVector256Carrier）。而 `Vector2/3/4::GreaterThanAll(Vector2,Vector2)` 是**非泛型具名**签名，不匹配 → `TryExtractClosedIntrinsicScalarManagedType` 失败 → return false → 落外部 fallback return-0（A1 的 12 个 numerics 缺口）。

**修复方向**：在 comparison planner 加 `System.Numerics.Vector2/3/4` (non-generic) 具名分支，carrier = `RuntimeNumericsVector2/3/4Carrier`，emit 调 `Vector2/3/4{GreaterThanAll,LessThanAll,...}(request->arg0, request->arg1)` → 我的新 native 符号。之后重建 numerics + fact 验证 passed=false→true。

### 10. A2-1 真实修复方向确认（2026-08-12，修正 kernel 误径）

**实证否决 kernel 路径**：把 `_All` 映射进 `TryCreateFixedComparisonPlan`（补 semanticId + Vector2/3/4 分支）后重建 numerics，fact 仍 16 失败——生成的 subject body 里**没有引用新 native 符号** (`Vector2GreaterThanAll`)。原因是：`Vector2/3/4::GreaterThanAll` 是 **managed CoreLib 方法**，经 subject→managed-method **外部 dispatch**（`ChaosExternalRuntimeFallback` 的硬编码 SIMD 常量 `interop_stubs.cpp:753`）落它，**不经 vector kernel**。kernel 只处理 `Vector<T>`/intrinsics。

**已 revert**：补 `MapOperationKindToSemanticId` mapping + FixedPlans Vector2/3/4 分支（编译通过但 dead code，已 `git checkout HEAD` 清掉，避免误导）。

**真实修复方向（下一 session）**：在 **ShapeRegistry / `TryCreateExternalRuntimeHelperDefinition`** 补 `System.Numerics.Vector2/3/4::GreaterThanAll/GreaterThanOrEqualAll/LessThanAll/LessThanOrEqualAll`(Vector,Vector→bool) 的 **SimpleForward** 条目 → 映射到已 land 的 native `Vector2/3/4{GreaterThan,LessThan,...}All`（commit 23e355b5f）。照 `RuntimeHelpers::InitializeArray`（S10.cs:156）的 Register pattern：`registry.Register("System.Numerics.Vector2","GreaterThanAll",["System.Numerics.Vector2","System.Numerics.Vector2"], SimpleForward, "Vector2GreaterThanAll", [Vector2-carrier ABI, Vector2-carrier ABI], bool ABI)`. carrier-by-value 需核对 ABI slot(2 floats)。之后重建 numerics + fact 验证。

**注意**：该 fallback 对 `EqualsAll/LessThanOrEqualAll` 等返回硬编码 1（interop_stubs.cpp:756-762），是"零输入巧合正确"的伪通过。真修复让全部 `_All` 走真实 native 计算，既修 GreaterThanAll/LessThanAll 的错返回，也消除伪通过。

### 11. A2-1 ABI 卡点裁决（2026-08-12）

核查 `AotCoreIrAbiCarrierKind`：存在 `ValueTypeByValue(3)` + `ByRefToValueType(14)` kinds，说明 dispatch 模型**概念上支持** by-value 结构传参。但 ShapeRegistry 的 `CollectBridgeImportThunks`/`Dispatch.Interface` 对 param 统一用 `CreateNativeIntAbiSlot()`（native int，8 字节），**无法承载 16 字节 `RuntimeNumericsVector2Carrier`**（2×float）。

∴ **SimpleForward 路线不可行**（carrier 无法经 INTPTR slot 传递）。正确路线回到 **vector-kernel numerics 路径**（`TryResolveNumericsFromArtifact` 已建 carrier=`RuntimeNumericsVector2Carrier`），但需确认为何：
1. kernel 改动重建后未生效（疑：AOT entry.exe 未用新 codegen 重建，只重建了 JIT entry-jit.exe）。
2. `Vector2::GreaterThanAll` 是否经 Phase-1 numerics 还是外部表派发。

**下一 session 第一件事**：核实重建命令是否对 AOT entry.exe 也用新 codegen（build stage 的 aot vs jit 产物差异），否则 kernel 修复永远不会在 fact 里可见。确认后再回到 `MapOperationKindToSemanticId`+Vector2/3/4 分支（该 ABI 路线正确，只是上轮验证方式错了）。

### 12. A2-1 验证断联根因 + kernel 仍未路由（2026-08-12 确认）

**验证断联根因（关键）**：pipeline 的 `tool_helpers.tool_dll()` 用 **Debug** 构建
（`src/tools/.../bin/Debug/net8.0/`），而我此前 `dotnet build -c Release` 只更新了
Release Generator DLL → pipeline codegen 从未加载我的改动（Unsafe.As / kernel 全没生效）。
修复：`dotnet build TestProjectGenerator.csproj`(Debug) 让 Debug Generator 依赖吸收
source 改动 → pipeline 才用上新 codegen。

**但 kernel 修复后重建 numerics 仍 164/180**：即使 Debug codegen 含改动，`Vector2::GreaterThanAll`
仍落 fallback。结论：`Vector2::GreaterThanAll` **不经 numerics kernel 派发**（可能走外部表
subject→managed-method 路径，或 `TryResolveNumericsFromArtifact` 对 这些 subject 压根未触发）。
需在 kernel `TryResolveNumericsFromArtifact` 加 debug 日志，确认 `Vector2Tests::GreaterThanAll_9`
(外部 subject) 内部调 `Vector2::GreaterThanAll` 时是否到达 numerics 解析——下一 session 第一事。

**教训**：代码库有 4 份 Generator DLL，pipeline 只用 Debug 那份；改 Generator source 必须
rebuild Debug 的 TPG（依赖传播），不然一切验证都是 stale。

### 13. 解析器问题实测量化（2026-08-12，展开用户质疑）

**真实数据（numerics chunk, --benchmark-all 100 iters）**：
- 180 benchmarkable 方法：**16 throw**(elapsed=-1.0 managed exception, 全为 Vector _All 归约) + 30 计为 0.000ms(可疑即时/伪执行) + 134 非零。
- 非零耗时：中位 0.003ms，max 0.145ms，p95 0.095ms → **max/中位 ≈ 30-45×**。

**修正"解析器问题"的定性**：
1. 不是"解析器均匀比 native 慢 10×"这种干净描述。而是**三类真实后果**：
   - **throw**（16/180=8.9%，Vector _All 等落 fallback→InterpreterEntryDirect→托管异常）：最严重，真实 workload 会异常。
   - **伪执行**（30 计 0.000ms，fallback 硬编码常数 return 0/1 或短路）：基准不可信、掩盖真实缺失。
   - **解释器慢尾**（慢至 0.145ms = 中位 45×）：真走解释器的方法确实慢一个量级+。
2. 与 roadmap 的 2.5-30× 吻合且更糟（尾部 45×）。
3. fallback 既非纯 slow 也非纯 correct——是 **correctness/robustness/perf 三合一热点**，集中在"无 C++ body 或无 native stub 被真实调用"的方法。

**结论**：方向 A（定点让这些方法有真 native 体）同时修三类后果（throw→正确执行、伪执行→真计算、慢尾→native 速度）。这是把"解析器问题"展开后的精确画像。

### 14. fallback 实际拦截规模：124 个 Numerics 方法（TEMP-DIAG 实测，2026-08-12）

在 `ChaosExternalRuntimeFallback` 加临时 `[fallback-diag]` 打印 subject_id，rebuild numerics
后跑 `--benchmark-all 100`：
- **124 个唯一 System.Numerics subject** 落入 fallback（去重）——远超 fact 的 16 失败。
- 巨量调用的方法：`StoreUnsafe`(1600) `Widen`(1400) `ToScalar`(800) `Store`/`StoreAligned`/
  `GetElement`/`ExtractMostSignificantBits` 各 800/600，加 `Vector2/3/4::All/EqualsAll/...` 各 200。
- 这些**多数 fact passed**——因 fallback 对 `Store→0`/`GetElement→0`/`EqualsAll→1` 等返回
  硬编码常量，恰好匹配 subject checksum（**伪执行：没真跑，但 checksum 对**）。
- `GreaterThanAll/LessThanAll` 也入 fallback，但**抛托管异常**（elapsed -1.0）。

**对"解析器问题"的最终定性（大幅扩展）**：
- fallback 拦截 **≥124 个 Numerics 方法**，不是 16。16 只是"checksum 抓到的显性失败"；
  其余 ~108 个是"checksum 掩盖的伪执行"（返回硬编码常量真值恰好对）。
- 真实代价分布：throw（16）+ 伪执行（~108）+ 真解释器慢尾。
- 这证实方向 A 的价值远超最初的 111 缺口估算——凡是落 fallback 的方法都可能未真执行。

**TEMP 诊断已测完，将 revert interop_stubs.cpp 的 fprintf**（不留 debug 残留）。

### 15. 派发链最终定位（2026-08-12，决定性）

`Vector2::GreaterThanAll` 等 124 个 Vector 方法**在 `kChaosExternalRuntimeSubjects` 外部表内**
(native-aot.generated.cpp:8681 的字符串数组)。∴ codegen 把它们当**外部 runtime 方法**处理，
生成的 call site → `kChaosExternalRuntimeFnTable` → fallback → 解释器/异常。

**∴ 真修复位置 = `TryCreateExternalRuntimeHelperDefinition`**（外部 helper 生成），**不是 vector kernel**
（这些方法不经过它）。kernel 路径(VectorFixed* semanticId + FixedPlans 分支)是编译通过但派发不达, 非正解。

**为何 SimpleForward 难**：表条目以 `Chaos*` 函数指针被调，ABI 是函数自身签名；而生成端 call site
以 `CHAOS_IL2CPP_INTPTR` 传参。Vector2 carrier(2×float, 16B) 无法经 INTPTR slot → 需 helper 内
"从 INTPTR 解出 carrier" 或用 kernel 的 by-value carrier emit 路径。

**下一 session 精确入口**：在 `TryCreateExternalRuntimeHelperDefinition` 对
`System.Numerics.Vector2/3/4::GreaterThanAll/...(Vector,Vector→bool)` 生成 native helper——
body 里把 2 个 INTPTR param 各自 reinterpret 成 `RuntimeNumericsVector2Carrier*` 解出 x/y,
调已 land 的 `Vector2GreaterThanAll` → 返回 1/0。这绕开 by-value 表 ABI 限制(用 pointer→carrier 解包)。

### 16. A2 验证基础设施屏障（2026-08-12，决定性）

**实现已完成**：`TryCreateVectorAllComparerHelper`（ExternalRuntimeHelpers.cs）为 Vector2/3/4
`_All` 生成 helper——2 个 INTPTR param reinterpret 成 carrier*，调已 land native，返回 1/0。
Generator Release 编译 0 error；regex 对实际 subjectId（`System.Numerics.Vectors/System.Numerics.Vector2::GreaterThanAll:Boolean(...)`）匹配验证通过。

**但验证被基础设施阻塞（关键）**：
1. **hephaestus cache 忽略源码变化**：`compute_key(subjects_dll, metadata, chunk)` 不含
   Generator/native 源码帧指纹 → 改 codegen/stub 后 `compute_context_fingerprint` 未含这些
   路径 → 永远 stale-restore AOT entry（5835264B 旧体），native 重建被 cache 覆盖。
   `is_context_fresh` 存在但 build stage 未用。
2. **JIT convert 可能用不同的 Generator DLL**（release vs debug；代码库 4 份 Generator.dll）——
   我 rebuild Debug TPG，但 convert-to-cpp 或许用 Release Driver/Pipeline 的副本。
   证据：Fresh JIT entry-jit.exe 也 16 失败、生成代码无 Vector2GreaterThanAll → 新 codegen
   未达任何生成路径。

**下步（新任务）**：修 A2 验证基础设施：
- 把 Generator/native stub 源码 mtime 纳入 `compute_context_fingerprint`（build.py extra_source_paths 传
  Generator + interop_stubs），cache key 变化 → 自动重建。
- 或加 `--no-hephaestus-cache` 强制真 convert+CMake。
- 确认 convert-to-cpp 用哪个 Generator DLL（确保 rebuild 正确的那个）。

### 17. A2-1 验证成功（2026-08-12，12/16 Vector _All 修复）

**A5 验证基础设施打通**：根因 = chunk 链接**本地 codegen/lib/chaos_runtime_core.lib(Jun 19 stale)**，
非共享 SDK。修复：(1) `build_presets.py --preset windows-x64-reference` 重建 SDK(11 libs,
含我的 Vector funcs)；(2) `cp` 刷新 chunk codegen/lib from SDK。此后 numerics fact **164→176/180**，
**12 个 Vector2/3/4::GreaterThanAll/LessThanAll/OrEqualAll 全部修复**(此前抛托管异常)。

**关键修复**：`TryCreateVectorAllComparerHelper` 移到 `TryMatchGenericShape` **之前**——原因为在
RegisterVectorReduction 的 generic shape TypeDisplayNamePrefix="System.Numerics.Vector" 前缀匹配
Vector2/3/4 → 路由到 Vector<T> 256-bit helper(错 ABI)→抛异常。移前+精确 Vector[234]:: 匹配后,
Vector2/3/4 走 carrier 专属 helper→真 native 归约。+ runtime_core.h 补 Vector3/4 _All 声明
(此前漏, LNK2019)。

**剩 4 失败 = Vector<T> 泛型**(VectorTests::GreaterThanAll_9_Vectorint, idx 535/537/545/547)—
独立 carrier 语义(Vector256 element-path), 非 Vector2/3/4 问题, 留 kernel/Vector<T> 路径。

**A5 待修**：build stage 应在源码变化时自动刷新 chunk codegen/lib + SDK(而非手工 cp); 建议把
runtime/numerics 源码 mtime 纳入 cache fingerprint 或强制 --no-cache。

### 18. Vector<T> 泛型 _All 失败诊断（2026-08-12，idx 535/537/545/547）

**现象**：`Vector<T>::GreaterThanAll/LessThanAll` 泛型（`_Vectorint`, `Vector<int>`）fact 返回 0
失败，但 Vector2/3/4（12个）已修。

**发现**：
- `chaos_vector_greater_than_all_i32` 等 native stub **存在**（vector_stubs.cpp 宏定义 + suffix 实例化
  i32/u32/i64/f/d/i16/u16/u8/i8 + 无后缀别名 aliases）。
- 但**生成代码里没有任何对该 native 符号的引用**——subject 调 `Vector<int>::GreaterThanAll` 既
  不 emit stub 调用，也无 `chaos_external_runtime_*` wrapper。

**结论**：`Vector<T>` 泛型 `_All` 走**不同于 Vector2/3/4 的更低层路径**（疑为 StructuredIR 内联
向量运算 或 `_TryExecuteViaIlData` 解释 IL-data），且当前产出错误结果(0)。非 Vector2/3/4 的
carrier−helper 问题。需查 StructuredIR/lowering 对 `Vector<T>::GreaterThanAll` 的 emit——
独立 follow-up, 与 Vector2/3/4 修复无关。

**A2-1 现状**：Vector2/3/4 12/12 修复(fact 164→176)。Vector<T> 泛型 4 个待 StructuredIR 路径。

### 18b. Vector<T> 泛型 _All 真正根因（2026-08-12，补充）

`_TryExecuteViaSimdStub`（interop_stubs.cpp:713-719）对 `Vector::LessThanAll/Any`、
`GreaterThanAll/Any` **硬编码 `out_value=0`**（OrEqual→1）。注释自认 "The Interpreter cannot
execute these methods (they are hardware SIMD)"，且仅对 **default(zero) 输入** 成立。但 idx535
等测试用**非零向量** → 0/1 是错值 → fact 失败。

**修复方向**：`Vector<T>::GreaterThanAll` 应调真 native `chaos_vector_greater_than_all_i32`
（存在，vector_stubs.cpp 宏生成），而非 fake 常量。但 codegen 的 `RegisterVectorReduction`
generic shape 未产出该 native 调用（生成代码无引用，§18）。= 与 Vector2/3/4 同类的"generic
shape 未正确 emit 真实 native"问题。需查为何 RegisterVectorReduction 的 GenericShape 对
`Vector<int>::GreaterThanAll` 未进生成代码。

### 18c. Vector<T> 泛型 kernel 路径存在但派发不达（2026-08-12 深挖）

kernel（RuntimeSkeletonVectorKernelCore）**有** Vector<T> 泛型 plumbing：
- Descriptors 表含 `/System.Numerics.Vector<` + `Vector`1::` 的 vector-fixed-comparison（line 131/133）。
- `TryResolveBySubjectId` 用 subjectId.Contains(descriptor.SubjectIdPrefix) 匹配（line 291-293）——
  `System.Numerics.Vectors/System.Numerics.Vector<System.Int32>::GreaterThanAll...` 应命中
  `/System.Numerics.Vector<`。
- `GreaterThanAll` → VectorFixedComparison → TryCreateFixedComparisonPlan 的 GreaterThanAll 分支
  生成 VectorFixedCompareGreaterThan + AllLanesNonZero（真 SIMD）。

**但生成代码无引用 + fact 走 fallback fake-stub** ⇒ 这些 method 的 call 未达
TryResolveBySubjectId 或被 CollectExternalRuntimeDispatchEntries 抢先路由为外部。疑点：
- `ClassifyIntrinsicsType` 只分类 System.Runtime.Intrinsics.Vector*，**不分类 System.Numerics.Vector<T>**
  （非 intrinsics family）→ 若 dispatch 先走 ClassifyNumericsType/ClassifyIntrinsicsType 而非
  TryResolveBySubjectId 的 Descriptors.FirstOrDefault，则 Vector<T> 被 kernel 完全跳过。

**下一步**：确认 codegen 主派发是否调用 TryResolveBySubjectId（Descriptors 表）for
`Vector<T>::GreaterThanAll`，还是先被 external-runtime 收集（_externalRuntimeSubjects）。二选一:
(a) 确保 kernel 派发在 external 之前，(b) 或在 TryCreateExternalRuntimeHelperDefinition 为泛型
Vector<T> _All 补 false（引 native chaos_vector_*）。

### 18d. Vector<T> 泛型 _All：helper 不达，结论固化（2026-08-12）

把 `TryCreateVectorAllComparerHelper` 泛化以处理 `Vector<T>` 泛型（`Vector<System.Int32>::GreaterThanAll`
→ `chaos_vector_greater_than_all_i32`，命名 vs 泛型 carrier 调用约定分离）+ runtime_core 无改动。
**结果**：Generator 编译 OK，但 numerics fact 仍 176/180（泛型 4 未动），生成代码无
`Vector<System.Int32>::GreaterThanAll` 引用 → **确认 codegen 根本不把泛型 `Vector<T>` `_All`
送进 `TryCreateExternalRuntimeHelperDefinition`**。它走的是 type-load fallback →
`_TryExecuteViaSimdStub` fake 常量（非 helper 机制）。泛型 helper 分支正确但未达 => 保留为
正确意图，路由缺口才是真问题。

**结论**：`Vector<T>` 泛型 `_All` 的修复锚点不在 external-helper（未被调用），而在
`CollectExternalRuntimeDispatchEntries`/`TryCreateExternalRuntimeHelperDefinition` 对泛型形式上
出现前，让 kernel（`TryResolveBySubjectId`/Descriptors 表有 Vector<T>）或 StructuredIR 内联
先接住。这是 deep lowering 层问题，非 helpers 层单点。Vector2/3/4（非泛型命名）已修 12/12。
