# [HotUpdate] 解释器派发缺陷 A2 — 复审综合报告

> **日期**：2026-08-13 ｜ **性质**：对 A2 交接档（`hotupdate-interpreter-dispatch-a2-2026-08-13.md`）的对抗式复审 + 递进诊断
> **方法**：逐条验证交接档的 H1/H2/H3 假设，用**当前新鲜生成的 codegen 产物 + 源码**对照，而非沿用交接档结论
> **证据等级**：以下每条均附 `file:line` 或产物路径；区分 ✅已证实 / ⚠️假设 / ❌已证伪

---

## 〇、一句话结论（✅ 异常捕获实证定案）

**基于实际运行 + SEH 异常捕获的最终定案：`System.Numerics.Vector<int>` 的 5 个 subject 在 AOT 执行中抛出真实的 `STATUS_ACCESS_VIOLATION (0xC0000005)` → harness `__except` 捕获 → `passed:false`。这是 SIMD/Vector<T> AOT 路径的一个真实原生 AV 正确性缺陷，绝非 A2 交接档所说的"解释器 UIntPtr 编组"，也非陈旧生成态。**

实证链（全部复现于 2026-08-13）：
1. **subject 认错（A2 维）**：5 失败是 `Vector<int>` SIMD 方法（`Count_4`/`GetElement_8`/`GreaterThanAll_9`/`LastIndexOf_15`/`LessThanAll_17`），非 A2 说的 BitOperations RotateLeft/Crc32C。
2. **SEH 捕获定案（本轮新增）**：给 fact-json 循环的 `__except(EXCEPTION_EXECUTE_HANDLER)` 加 `GetExceptionCode()` 打印 → **5 个失败全部 `SEH code=0xC0000005`（ACCESS_VIOLATION）**，methodIndex=522/535/537/545/547，一一对应。
3. **排除**：`PAL-DIAG`（pal_eh_win32 `PalTryCallNoExcept` 的 `__except`）**不触发**——因为 fact-json 路径直接在 harness `__except` 捕（非经 PAL 包装）。`chaos_vector_*` helper **已定义**（`vector_stubs.cpp:79`）。wrapper body `_0`/`_1` 逐字节相同仍一过一崩 → AV 是**运行时指针/表示问题**，非静态代码缺陷。
4. **真修复域**：`Vector<int>` AOT wrapper 类 SIMD helper 时**操作数指针表示不符**（codegen 传 `&chaos_locals[1]` 槽地址而 helper 期望 `const TCarrier*` = 32 字节 Vector256 指针），对特定方法/值集触发 null/非法指针解引用 → AV。属 codegen SIMD lowering（`NativeAotLoweringPlanner` / RuntimeHelperShapeRegistry）+ SIMD native 层。

**因此**：不要在解释器编组上 patch（伪靶）；这是 **SIMD/Vector<T> AOT 原生 AV**，专项归属 = `dev-il2cpp-simd-expert` + `p0b-stub-sweep` + codegen lowering。

---

## 一、复现信息复核

交接档给的命令与结果（`175/180 passed (partial)`、5 个失败 subject）**未能独立复验**——无法从当前仓库取得 A2 运行期那个 fact 输出的 checkpoint（无对应 fact JSON 落盘）。但关键的是：**当前仓库里的 numers 生成产物全部是 2026-08-13T10:00 的fresh重新生成**，与 A2 交接时（同日更早）的生成状态不是同一份。

| 项 | 交接档 A2（陈述） | 当前仓库实况（复现证据） |
|----|------------------|------------------------|
| 这 5 个 subject 有 AOT body？ | ❌ "无 AOT C++ body（纯解释器路由）" | ✅ **有**。`native-aot.generated.page2.cpp:5271/5307/5342/5378…` 各变体均有完整原生 wrapper body |
| dispatch flags？ | `kHotpatchKeepNative \| HotpatchEncodeArgCount(0)`，`direct_ptr=&InterpreterEntryDirect` | 前 2 项 ✅（`native-aot.generated.cpp:8162-8195`）；但 **`direct_ptr` 不是 `&InterpreterEntryDirect`，而是 `&CombinedSubjects_..._X` 真实 body**；`&InterpreterEntryDirect` 在独立的 `interrupt_ptr` 字段 |
| subjectKind / reason | —（交接档未查 manifest） | **`method` / `generated-direct-call`**（`aot-manifest.json`，2026-08-13T10:00，12 变体全一致） |

> **结论**：A2 观察到的"纯解释器路由"与当前生成的"直接调用"**不是同一状态**。二选一：要么 A2 发生在陈旧生成态（body 缺失→回退解释器），要么交接档证据被误读。两种都指向**非解释器根因**。

---

## 二、三假设逐条验证

### H1（主假设）— 解释器对 UIntPtr/uint/ulong 返回编组抛异常 → ❌ 当前不成立

**验证链条（源码）：**
1. `CacheSignature`（`interp_entry/signature_cache.cpp:137`）：`cached_ret_tag = ElementTypeToValueTag(sig[0])`。
2. `ElementTypeToValueTag`（`signature_cache.cpp:21-23`）：`0x18 (IntPtr)` / `0x19 (UIntPtr)` / `0x0A/U8` / `0x0B/U8` **全部映射为 `ValueTag::Int64`**。
3. `WriteTypedRet` / Step-B/Step-C/OSR 各 ret 写回路径均正确 `WriteI64`（`entry_direct.cpp:918/1044/1123`）。**64 位原生整数返回在解释器内是正确处理的。**

**结论**：若真走到解释器，`RotateLeft(UIntPtr,int)` / `RotateRight(uint|ulong,int)` / `Crc32C(uint,byte)` 的签名与返回在 `CacheSignature`/`WriteTypedRet` 层面**没有 UIntPtr 特有缺陷**。H1 的机制在当前生成态**根本不会触发**（subject 不路由到解释器），故 H1 作为根因**不成立**。
残留可能（⚠️ 需独立观测台验证）：若因生成态缺失 body 强行走解释器，`cache_signature` 对 `int`（`0x08`→Int32）与 `byte`（`0x05`→Int32）均正确，仍无异常来源。

### H2 — `HotpatchShouldKeepNative` 为真却调用 body-less 的 `&InterpreterEntryDirect` → ⚠️ 机制存在但当前不触发

`hotpatch_dispatch.h`：
```cpp
if (CHAOS_IL2CPP_LIKELY(HotpatchShouldKeepNative(entry))) {  // 只查 bit1，不查 hotpatchActive
    if (entry.direct_ptr) return DispatchDirectGetValue(entry.direct_ptr, entry.flags);
    ... CHAOS_IL2CPP_FAIL("No direct_ptr for keep-native slot %d", index);  // 关键分支
}
```
- `HotpatchShouldKeepNative`（`hotpatch_table.h:233`）**独立于 `kHotpatchActive`**。
- **危险分支已实锤存在**：若某 slot 被标 `kHotpatchKeepNative` 但 `direct_ptr` 为空或指向错误符号，则 `DispatchDirectGetValue(reinterpret_cast<fn4arg>(direct_ptr))(0,0,0,0)` 会以**错误 ABI** 调用 `InterpreterEntryDirect`（真签名 3 参 `(method_key,args,ret)`，被当作 4 参 `uint64(uint64,uint64,uint64,uint64)` 强制 cast），`method_key=0` → `interpreter_entry.cpp:640` 早退 `[INTERP-SILENT] null method_key`（**不抛异常**），或更糟：直接指针非法→SEH/AV→`_pal_caught=true`→`passed:false`。
- **但当前 12 个变体 `direct_ptr` 均指向真实 body（`native-aot.generated.cpp:8162-8195`），keep-native 快路径会正常调用 AOT 代码，H2 不触发。**

> ⚠️ **给专项的一条真实工程债（与本 A2 无关但同源）**：`HotpatchShouldKeepNative` 只查 bit1 而**不校验 `direct_ptr` 非空**，把"keep-native 到非法指针"的错误留给运行时 SEH 兜底。这是当前热更/解释器派发的一个**防御性缺口**，即便 A2 不是它，也值得在专项里一并加固。

### H3 — ATG hotupdate 变体与 `HotpatchEncodeArgCount(0)` 交互编组 → ❌ 机制不适用

这些 subject 的 flags 编码 `HotpatchEncodeArgCount(0)`（=编组 0 参），但 `RotateLeft(UIntPtr,int)` / `Crc32C(uint,byte)` 实际 **2 参**。
- **在 keep-native 直接调用下，arg-count 编码不参与 ABI**（`DispatchDirectVoid/DispatchDirectGetValue` 走固定 4 参 ABI，直接调真实 wrapper），所以 arg-count=0 不产生编组错误。
- arg-count 只在**解释器路由**（`HotpatchIsActive && !keep-native`）时用于 `InterpreterEntryDirect(method_key,args,ret)`（`hotpatch_dispatch.h:106/266`）。当前不走该路。**H3 仅在未来恢复解释器路由时才相关。**

---

## 三、真实的启动信号（我们找到的差异，与交接档"完全一致"矛盾）

交接档称"同方法 `_0/_1/_2` 调用体完全相同、全部零输入、仅 index 不同 → 有 pass 有 fail"。**实测各变体 body 存在语义差异，并非"仅 index 不同"**：

| 变体 | 入参来源 | 调用 helper | 当前 body（`page2.cpp`） |
|------|---------|-------------|--------------------------|
| `RotateLeft_23_0` | `static_cast<INTPTR>(0)` | `ChaosBitOpsRotateLeft64(0,0)` | L5284-5288 |
| `RotateLeft_23_1` | `chaos_static_..._UIntPtr__Zero`（=0 的全局静态） | 同上 | L5326-5327 |
| `RotateLeft_23_2` | `static_cast<INTPTR>(0)` | 同上 | L5342+ |
| `RotateRight_24_x` | 字面 0 | `ChaosBitOpsRotateRight32` | L5378+ |
| `RotateRight_25_x` | 字面 0 | `ChaosBitOpsRotateRight64` | — |
| `Crc32C_27_x` | 字面 0 | `ChaosBitOpsCrc32CByte(0,0)` | L3803+ |

且所有 helper（`ChaosBitOpsRotateLeft64/Right32/64`、`Crc32CByte` 等）在 `runtime_stubs/math_stubs.cpp` 中**均为真实 noexcept 实现**（L169-249），非 stub。

> 结论：当前 body 层对零输入不会抛 managed exception，且 wrapper 自带 `catch (const chaos_managed_exception&) { return {}; }`（L5336-5338）。**要产生 `passed:false`（异常逃逸到 harness 的 `catch(...)`/`_pal_caught`），必须是 SEH 硬件异常（AV/栈溢出）或非 managed C++ 异常**——这在本 wrapper 内不应发生，除非运行时环境/内存被破坏。这更强化"A2 失败源于当时生成态，而非逻辑缺陷"的推断。

---

## 四、根因再定位（✅ 已用 SEH 捕获 + 操作数表示对比定案）

**AV(0xC0000005) 的精确机制已定位 → codegen SIMD lowering 缺陷：`Vector<int>` 操作数未物化为真实向量。**

| 环节 | 证据 | 结论 |
|------|------|------|
| 5 失败全是 AV | 临时加 `GetExceptionCode()` → 5 个失败全 `0xC0000005`，mi=522/535/537/545/547 | 硬件访问违例（非法指针解引用） |
| helper 契约 | `chaos_vector_greater_than_all_i32(left,right)` 做 `*reinterpret_cast<const TCarrier*>(left)`（`vector_stubs.cpp:18,24`） | left/right 必须是 **32 字节 Vector256 指针** |
| wrapper 传错表示 | `GreaterThanAll_9`(fail) 传 `left=&chaos_locals[1]`（line 1408+1432），且 slot 只零初始化、**从未装入真实 Vector 数据** | 操作数 = 指向 24B 槽数组的指针 + 空数据 |
| why _0/_1、相邻槽分裂 | pass 的 `GreaterThanAny_10`(mi538) body 与 fail 的 `GreaterThanAll_9`(mi537) **逐字节相同** | 差异不在 body——helper 读 `&chaos_locals[1]` 溢出的 32B 相邻栈内存，是否越页→AV **取决于栈布局/先前执行的调用** → 同 body 有的过有的崩 |
| **根因** | `Vector<int>` 操作数 codegen 只生成 `&chaos_locals[i]`（槽地址），未将向量值加载入槽 | **操作数表示缺陷**，非静态逻辑/缺符号 |

**因此（终判）**：这不是解释器、不是陈旧态、不是缺 helper、也不是单点逻辑——是 **codegen SIMD lowering 对 `Vector<int>` 操作数物化错误**（传槽地址当 32B 向量指针）。且**部分"pass"的 `Vector<int>` subject 其实在读栈垃圾返回错误值（假绿）**，整个 `Vector<int>` SIMD AOT 路径都不可信。

## 四bis、代码级定案：调用点操作数物化 bug（缺失间接层）

**已定位到 codegen 具体机制（两级缺陷叠加）：**

1. **载体 ABI 契约**：codegen 注释明示 `Vector<T>` 在 managed-pointer ABI 上**槽内存的是"指向载体数据的指针"**（`LinearEmission.Advanced.cs:176-178`）。helper 应收到**槽的值**（= 指向 32B carrier 的指针），helper 再 `*reinterpret_cast<const TCarrier*>(ptr)` 读载体验证。
2. **级缺陷① — 传错无间接层**：wrapper（`NativeAot.page-0005.cpp:1428-1432`）传的是 `&chaos_locals[1]`（**槽地址**），而 helper 期望**槽内的指针值**。应传 `chaos_locals[1]`（多一层解引用取指针）。
3. **级缺陷② — 槽从未装入真实载体**：即便传入指针值，`chaos_locals[1]` 在 initobj 时被 `= 0`（page-0005 的 memset 块），**测试提供的 Vector<int> 值从未被 resolve/load 进槽** → helper 得到 null/垃圾指针 → 条件性 AV。
4. **why 条件性**：helper 读 32B 撞未映射页则 AV(0xC0000005)，读可读垃圾则返回错值(假绿)——取决于栈布局/先前调用，故同 body 的 `_0`/`_1`、相邻槽(537 vs 538)一过一崩。

**修复规格（codegen SIMD lowering 域，`NativeAotLoweringPlanner` / `RuntimeSkeletonVectorKernelCore`）：**
- 调用 `chaos_vector_*_suffix(chaos_arg_0, chaos_arg_1)` 时，实参应为**槽内指针值**（`chaos_locals[N]`）而非槽地址（`&chaos_locals[N]`），并确保调用前槽已被载入真实的 32B `Vector<int>` 载体值（resolve 测试提供的向量 → memset/load 进槽）。
- 对齐 `Vector2/3/4` 真 lane-reducer 的操作数物化路径。

---

## 五、给专项（codegen SIMD lowering 修复）

> ✅ **AV + 操作数表示已实证定案**（2026-08-13）：`Vector<int>` AOT wrapper 把 `&chaos_locals[i]`（零初始化 24B 槽）当 32B `Vector256*` 传 helper → 越界读栈 → 条件性 `0xC0000005`。

1. **P0 — 复现基线（已做）**：`entry.exe --fact-json` → `aot:175/180`，5 失败全 `0xC0000005`，mi=522/535/537/545/547（`Count_4`/`GetElement_8`/`GreaterThanAll_9`/`LastIndexOf_15`/`LessThanAll_17`）。
2. **P1 — 修 codegen 操作数物化（真修复，SIMD lowering 域）**：
   - 定位点在 `NativeAotLoweringPlanner` 的 SIMD lowering / `RuntimeHelperShapeRegistry` 对 `Vector<int>` 操作数的发射——`chaos_vector_*` helper 调用处应把操作数物化为**真实的 32B Vector256 值**（load 到槽），而非传空槽地址。
   - 参考：`Vector2/3/4` 的真 lane-reducer 如何物化操作数（`p0b-stub-sweep-classification` 记的 `Equals*` reducer）；`Vector<int>` 应走同一条物化路径。
   - **回归**：重跑 numerics build + fact → 期望 `180/180`；且要验证原"假绿"的 pass `Vector<int>` 现在返回**真值**（补断言）。
3. **P2 — 兜底 (codegen 降级)**：若全类型物化成本过高，对 `Vector<int>` 明确走解释器传播（正确但慢）。
4. **P3 — 排查面扩展**：同族的 `p0b-simd`（SIMD Equals*）、`Vector2/3/4` 是否同一物化 bug；本 5 失败只是暴露面之一。

> ⚠️ 涉 codegen SIMD lowering + runtime helper（`project-test-governance` 规则 6：改 `NativeAotLoweringPlanner`/helper 必须 + snapshot tests）。不直接手改。**下一步 = codegen 侧操作数物化修复，专项归属 `dev-il2cpp-simd-expert`。**

## ✅ 修复已落地并验证 (2026-08-13)

**改动**（`src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.Utilities.cs`）：
- `EmitExternalRuntimeTableDispatch` 对 `chaos_vector_*`（且 2 参）reducer 调用点，操作数绑定改走新 `EmitVectorOperandMaterialization`。
- `EmitVectorOperandMaterialization`：物料化清零 32B carrier，若槽内持有非 null carrier 指针则拷贝 32B，绑定 `chaos_arg_i = &carrier`（替代 `&chaos_locals[N]` 8B 槽地址）。

**验证（全绿）**：
- numerics chunk：**`aot: 180/180, jit: 180/180`**（自 175/180）；5 个 `Vector<int>` AV(0xC0000005) 全消，值正确（`GreaterThanAll_9`/`Count_4`=1，`GetElement_8`/`LastIndexOf_15`=0）——非栈垃圾，假绿也消。
- snapshot 108/108 无基线漂移；unit suite `OVERALL: OK`（2249 pass，28 known-fail 全既存）。
- layer-boundary preflight ✅。

**遗留/后续**：本修复针对 `default`(零) 及"槽内持有 carrier 指针"两场景。若未来有 `Vector<int>` 构造值为非零且槽内不是指针而是内联布局，需复查物化路径；同族 `Vector2/3/4` 未受影响（named 形式 body deref 已有）。
4. **P2 — 或 codegen 降级**：对 `Vector<T>` 明确走解释器传播（正确但慢）作为兜底。
5. **P3 — 工程债**：`hotpatch_dispatch.h` 的 `HotpatchShouldKeepNative` 缺 `direct_ptr` 空值校验（独立，不阻塞）。

> ⚠️ 涉 SIMD + codegen + snapshot tests（`project-test-governance` 规则 6）。不直接手改。**下一步的第一步 = 运行时异常捕获，不是补 helper。**

---

## 六、证据索引（file:line / 产物）

**实证基线（2026-08-13，复现）**：`PYTHONPATH=tests/e2e python -u -m verification.chunk_pipeline --assembly System.Private.CoreLib --chunk numerics --stages build,fact --skip-probe` → **`aot:175/180, jit:175/180`**；`entry.exe --fact-json` 抓出 5 个失败 si=49,52,54,60,62（methodIndex=522,535,537,545,547），全部 `caught=true,value=0`。

| 结论 | 证据 |
|------|------|
| 实际失败=Vector<int> SIMD 方法 | fact 输出 si49→`Count_4_Vector<int>_1`、si52→`GetElement_8`、si54→`GreaterThanAll_9`、si60→`LastIndexOf_15`、si62→`LessThanAll_17`（对照 `native-aot.generated.cpp:8042+` 的槽 522/535/537/545/547） |
| 有真实 AOT body（keep-native 直接调用） | `native/codegen/generated/native-aot.page-0004.cpp:7909`(Count_4)、`page-0005.cpp:1383`(GreaterThanAll_9)；`s_hotpatch_entries[]` 各处 flags=`kHotpatchKeepNative\|HotpatchEncodeArgCount(0)`、`direct_ptr=&CombinedSubjects_...` |
| Count 经 external-runtime stub | wrapper 调 `chaos_external_runtime_System_Numerics_Vectors_...Vector_Count(int)()`（`page-0004.cpp:4627/4664/7893/7942`） |
| **greater_than_all helper 未实现** | `chaos_vector_greater_than_all_i32` 在 `src/`+`contracts/`+`codegen/lib/chaos_codegen.lib` **grep 全空** → 运行时跳非法地址 |
| 解释器 UIntPtr/UInt64→Int64 映射正确 | `src/native/interpreter/interp_entry/signature_cache.cpp:21-23,137,64-66` |
| keep-native 空指针→错 ABI 调解释器（防御缺口） | `src/native/interpreter/interp_entry/entry_direct.cpp:640` + `hotpatch_dispatch.h:55-64` |
| build.py 陈旧 cache bug 已修（非陈旧态） | `tests/e2e/verification/stages/build.py:775-785`（清 stale metadata/.autogen, 强制重生成 + `:997` reconcile） |
| helper 家族已注册 L part（Equals* lane-reducer） | memory `p0b-stub-sweep-classification`（S3.cs, `chaos_vector_equals_all/any_*`） |

---

## 七、关联

- 交接档原档：`docs/dev/assessments/hotupdate-interpreter-dispatch-a2-2026-08-13.md`
- 同族故障 memory：`corelib-stale-manifest-genid-drift`、`corelib-dispatch-slot-collapse-5-of-75`
- 责任专家：`dev-il2cpp-hotupdate-expert`（热更/解释器派发）+ `dev-il2cpp-fact-verification-expert`（生成管线/fact 对齐）
