# chaos-il2cpp 解释器功能综合评审（峰平谷）

> **日期**：2026-08-13 ｜ **性质**：从"il2cpp 解释器应该做到的功能"基准，审视 OK 解释器实现覆盖
> **方法**：2 个 Explore agent 只读扫源码（执行模型 + 运行时集成）＋ 对照 wiki 声称设计（`19-解释器VM架构.md` / `21-interpreter-optimizations.md`）＋ 人工核对关键断言
> **范围**：`src/native/interpreter/` 解释器 + 与 `runtime-core`/`jit` 的集成

---

## 〇、一句话结论

**这是一个超出"实习级解释器"的现代化分层解释器**——5 级金字塔执行管线 + computed-goto 核心 + 111-opcode IR + 完整 SEH + 寄存器机 + 到 JIT 的 tier/OSR 回退链 + 热更深度集成 + 精确 GC 帧扫描和写屏障，覆盖面和工程成熟度是"峰"。
**但其对象模型是自研沙箱（非 GC 托管、非 il2cpp `Il2CppObject` 布局），与 runtime-core 的原生 GC 对象系统两套并存、不互通**——这是最大"谷"，也是它必须把 `Vector<int>`/泛型/带盒对象 keep-native 到 AOT（而非纯解释）的根本原因。介于峰谷之间的"平"是一批以预编译立即数占位、缺精确语义的 opcode（`ldtoken`/`sizeof`/`initobj`/null/越界检查）。

---

## 一、峰 — il2cpp 该有的，且做得出色的

### 1. 五级金字塔执行管线 + 逐级降级（远超普通解释器）
入口 `InterpreterEntryDirect`（`interp_entry/entry_direct.cpp:634`）按 tier 状态/IR 形状在同一函数内逐级降级：
- T1 `Step1c` 两指令 fast path（`LdArg;Ret`/`LdcI4;Ret`/`LdNull;Ret`，`:696`）直接读 ArgBuffer。
- T2/T3 `RegisterExecute` 寄存器机（虚拟寄存器文件 64 GPR+32 FPR，`ir_reg_alloc.cpp`）。
- T1-Computed-goto `FastExecute`（`fast_dispatch_execute.inc`）。
- T4 `kQuickJitted`/`kJitted` 原生码（GC preemptive 转换包裹，`entry_direct.cpp:802-849`）。
- 慢路径 `InterpreterVM`（`interpreter_vm.cpp` switch，SEH/溢出用 C++ 异常）。

### 2. computed-goto 分派内核
`FastExecute` 用 GCC/Clang `goto *kOpTable[op]`（`fast_dispatch_execute.inc:197-243`），简单 op 直接内联进标签循环、复杂 op 走 `Handle_*`；MSVC 回退同表 `switch`（`:595-650`）。1-3 条指令微优化。FastFrame 定长 `stack[64]+locals[32]` + tag 数组，无堆分配（`fast_dispatch.h:40-371`），配 TLS 帧池（`fast_frame_pool.cpp`）与 boxed 池。

### 3. IR opcode 覆盖广（99/111 在解释层处理）
`ir_opcodes.h` 111 个，两解释层覆盖除 SIMD 外的几乎所有类别：常量/栈/算术/比较/分支(`Switch`)/字段(`StFldBarrier` 写屏障)/调用(`CallVirtConstrained`/`calli`/`CallBridge`)/对象(`Box/Unbox/CastClass/IsInst`)/数组(含 NoChk)/转换(含 Ovf)/间接(`Cpblk/InitBlk/LocAlloc`)/指针(`LdArgA/LdLocA/LdFtn`)/SEH(`Throw/Rethrow/Leave/EndFinally/EndFilter`)。
冷指令也做了：`EndFilter`(interpreter_vm.cpp:1305)、`LdToken`(:1594)、`SizeOf`(:1607)、`LdVirtFtn`(慢路径 :1613)、`LocAlloc`(:1664 带 `localloc_blocks` 回收)、`Abs/Min/Max/Popcnt/Lzcnt`(ir_reg_alloc.cpp:143-164)。

### 4. 完整两阶段 SEH 异常模型
`SEHClause` 定义（`fast_dispatch_core.inc:179-305`）；三执行层都做 Phase1 搜索 + Phase2 展开：
- FastExecute 内联两段展开（`fast_dispatch_seh.inc:8-191`，`Throw/Rethrow/Leave-finally/EndFinally/EndFilter`，filter 拒绝外扩）。
- RegisterFrame 寄存器层（`ir_reg_alloc.cpp:2291-2386`）。
- 类型化 catch 靠 `TypedCatchCheck` 父链遍历（`entry_direct.cpp:51-87`）。
- 跨解释器↔AOT 异常传播经 `PalTryCallNoExcept`/`InterpreterDispatchRaw`→`method_invoke` 状态码。

### 5. 到 JIT 的 tier / OSR 回退链（加分项，非解释器本职）
`EvaluateTierPromotion` CAS 决策（entry_direct.cpp:592-632,764）；QuickJIT 无优化栈式快速预热（`:532-588`）→ Full JIT(带 optimizer/safepoint/liveness/PIC)（`:440-524`）。执行期热回边 OSR（`fast_dispatch_execute.inc:59-132` 检测向后分支）。T4 deopt→`CaptureNativeFrame`→`RestoreOsrToFastFrame` 回解释器（`entry_direct.cpp:870-958`，超限 `kJitSkip`+回滚 direct_ptr）。

### 6. 热更深度集成
`.patchdata`/PatchLoader/HotpatchNameRegistry 驱动（`.patchdata` 二进制 RegisterMethod 预载 / v1 JSON 懒 lowering）；`method_key→InterpreterEntryDirect`；调用级缓存 `call_cache`(MIC) 与 `direct_fn` 快路径；`method_replacement::Resolve` 兼容旧版热更（entry_direct.cpp:774-786）。

### 7. GC 集成（帧扫描精确 + 写屏障接入）
- **精确帧扫描**：`interp_frame_scanner.cpp` 按 `frame_type`+tag 只标记 ObjectRef 槽（非保守），注册进 `GcScanInterpFrameChain`。
- **写屏障**：`StFldBarrier`/`Stelem`/`StInd` 等对象写用 `BgcSatbPreWriteBarrier` + dirty card（interpreter_vm.cpp:584,831,854,1568；fast_dispatch_object.inc:147-156）。
- **寄存器分配器**：linear-scan（非图着色但够用，ir_reg_alloc.h:12-14）。

---

## 二、平 — 可用，但明显简化 / 语义半实现

| 面 | 现状 | file:line |
|----|------|-----------|
| `ldtoken` | 只 push 预编译 `immediate_i4` 整数，非 `RuntimeTypeHandle` 运行时语义 | fast_dispatch_object.inc:248-252 |
| `sizeof` | 直接读立即数（IR 期灌入），运行时不解析 | fast_dispatch_object.inc:264 |
| `initobj` | 只对 `InterpreterValue*` 指针 memset 16 字节，非真实对象布局 o | fast_dispatch_object.inc:254-262 |
| `LazyBox` | 延迟装箱到调用/ret 物化；但 boxed 对象是 `InterpreterObject`（非 GC） | fast_dispatch_core.inc:59-117,165-169 |
| 数组 | 引用数组元素写有 SATB barrier；flat 数组注释"无写屏障" | fast_dispatch_object.inc:86-87 |
| delegate | `LdFtn` 推 call_target（可用）；**`LdVirtFtn` 在 FastExecute 仅 `Handle_Unsupported`**（慢路径有实实现 :1613） | fast_dispatch_object.inc:270-279 |
| P/Invoke | 经通用 `InterpreterDispatchRaw→method_invoke→marshal bridge`，但解释器**不主动** transition 到 preemptive | 见谷#3 |
| profiling | 有 `register_vm_profiler` 进程级计数器 + PROFILE-build opcode 频率直方图 + PGO 分支计数；**无硬件异步栈采样 profiler** | register_vm_profiler.cpp |

---

## 三、谷 — 相对"il2cpp 解释器应有"的关键差距

### 谷#1（最大 · 架构性）：对象模型是自研沙箱，非 il2cpp 原生对象、非 GC 托管
- 解释器内 `InterpreterObject{SmallFieldArray; type_token}`(interpreter_vm.h:261-264)、`ArrayStorage`(:277-293)、`BoxedValue`(interpreter_vm.cpp:870) 全部 `CHAOS_IL2CPP_MALLOC`/`DOMAIN_CURRENT_ALLOCATE` 分配（:739/751/754/870），**不是 runtime-core 的 GC `ArrayHeader`/`BoxedValueHeader`**。
- AOT 侧 box 桥用 `GcAllocateAtomic` + `TypeInfoHandle` 头（runtime_mode.cpp:15-55）；解释器用 `type_token`。**两套并存不互通**。
- → 解释器不能直接解释 AOT 侧泛型/盒对象，必须 keep-native 到 AOT 实现（本仓 `Vector<int>` SIMD keep-native、泛型注册都源于此）。**这是一道架构分水岭**：解释器是"能跑自己沙箱对象模型的独立 VM"，不是"il2cpp 对象模型的加速器"。

### 谷#2（正确性）：Null 数组 / 越界检查缺失
`Handler_LdElem/StElem` 在 null 数组或越界时**静默 push null / skip**，不抛 `NullReferenceException`/`IndexOutOfRangeException`（fast_dispatch_object.inc:48-54,122-128）。正常 il2cpp 必须抛异常。这会让解释器在"非法访问"上比 .NET/AOT 更宽容，掩盖 bug。

### 谷#3（GC/线程正确性）：纯解释器无 in-band safepoint、不主动 GC-mode 切换
- 解释器执行循环只每 64 指令查 `pending_abort/interrupt`，**不调 `SafepointPoll()`**（fast_dispatch_execute.inc:484-494）。
- GC 挂起解释器线程靠**抢占式信号/APC trampoline** 把 RIP 重定向（thread_state.cpp:327-400），非解释器回边/调用点自 poll。
- blocking P/Invoke / 长运行时原生调用时**无 `GC_TRANSITION_TO_PREEMPTIVE`**（gc_transition.h 只在 T4 用）。→ 长循环/blocking 调用期间 GC 延迟与死锁窗口风险。

### 谷#4（对象语义）：`PalTryCallNoExcept` 丢弃异常对象指针
跨解释器↔AOT 调用被 `PalTryCallNoExcept` 捕获后只 `caught=true`，`direct_fn_exception_obj=0`（interpreter_vm.cpp:1047-1048）。→ 异常对象（Message/StackTrace/InnerException）跨边界丢失，fault 语义不精确。

### 谷#5（数据表示）：字符串不走 intern 表 / 编码转换
解释器 `ldstr` 经 `resolve_string_by_token` 拿裸 `const char*` 直接 push（fast_dispatch_core.inc:334-342），非 AOT 的 `CHAOS_IL2CPP_STRING_ID` intern 表 + 编码统一（string_table）。→ 解释器与 AOT 侧的字符串相等性/编码/GC 语义不一致。

---

## 四、相对"il2cpp 解释器应有功能"的对照矩阵

| il2cpp 解释器应有 | 本实现 | 评级 |
|------------------|--------|------|
| 标准执行管线（多级 + 加速） | ✅ 5 级金字塔 + computed-goto + 寄存器机 | 🏔️峰 |
| 完整 IL/IR 指令覆盖 | ✅ 99/111（缺 SIMD/向量） | 🏔️峰 |
| try/catch/finally/filter 精确语义 | ✅ 三层两阶段 SEH | 🏔️峰 |
| 到原生编译的分层 + deopt/OSR | ✅ tier T1→T4 + OSR + deopt | 🏔️峰 |
| 热更新（patchdata/懒lower/MIC） | ✅ 深度集成 | 🏔️峰 |
| 精确 GC 帧扫描 + 写屏障 | ✅ tag 扫描 + SATB barrier | 🏔️峰 |
| 与 AOT/native 对象互通（Il2CppObject 布局） | ❌ 沙箱对象模型，两套不互通 | 🕳️谷 |
| GC 托管的盒/数组/字符串 | ❌ MALLOC + manual Track | 🕳️谷 |
| 解释器内 safepoint / GC-mode 切换 | ❌ 靠抢占式信号重定向 | 🕳️谷 |
| null/越界抛异常 | ⚠️ 静默 | ⏺️平 |
| `ldtoken` 返回 RuntimeTypeHandle | ⚠️ 整数占位 | ⏺️平 |
| delegate 虚函数指针（LdVirtFtn） | ⚠️ 快路径 Unsupported | ⏺️平 |
| SIMD/向量 opcode | ❌ 两解释层均无（keep-native 到 AOT） | ⏺️平（有意取舍） |

---

## 五、总结与定性

**定位**：这不是"il2cpp 的解释器"——它更像一个**独立、自足、现代的分层解释型 VM**（自有对象模型/自有 GC 集成方式），通过 `InterpreterEntryDirect` + 预编译 IR + dispatch stub 与 AOT 侧对接，并负责 AOT/JIT 的 tier/OSR/deopt 全链路。

**它做得好（峰）**的执行/异常/热更/性能分层，是 il2cpp 解释器该有的硬功夫，且工程成熟度高于多数自研解释器。

**它最深的"谷"是对象模型不统一**（非 GC、非原生布局），这既解释了为何它必须把带盒对象/泛型/SIMD keep-native，也正是"降低解释器覆盖面 / 依赖 AOT"的根本。若目标是把解释器当作 il2cpp 全功能 fallback，谷#1-5 是要啃的硬骨头；若定位是"热更方法的专用解释器 + 到 JIT 的梯子"，则峰平已足够支撑主力路径，谷是可接受的边界（以 keep-native 隔离）。

**风险最高的三处**（正确性，值得排期）：谷#2(静默 null/越界)、谷#3(safepoint/GC-mode 切换)、谷#4(异常对象跨边界丢失)。

---

## 六、关联
- 本评审继承 A2 结论：`Vector<int>` SIMD AV 正是因为解释器不解释 SIMD → keep-native 到 AOT → AOT 侧码物化 bug（已修，`46a417b23`）。对象模型谷#1 是其架构根。
- wiki 声称设计基准：`01-翻译管线/19-解释器VM架构.md`、`21-interpreter-optimizations.md`、`18-热更新架构.md`。
- 证据由 2 个 Explore agent（`src/native/interpreter/` 执行模型 + 运行时集成）产出，附 file:line。
