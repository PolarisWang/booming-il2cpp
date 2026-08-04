# 执行路径风险再评估报告 — Phase 4

## Meta
- task_id: 20260604-execution-path-risk-reassessment
- lifecycle: active
- phase: assessment
- based_on: approach-b-foundation-first roadmap completed (Phase 1-3, 43 items)

## 总览

基于完成全部 43 项修复后对 AOT/JIT/Interpreter 执行路径的重新审计，发现 **27 项新风险项**（4 CRITICAL, 6 HIGH, 12 MEDIUM, 5 LOW），其中多数在原始 roadmap 范围之外（domain unloader, deopt 全局竞态, SEH 自旋锁重入等）。

---

## CRITICAL（4 项）

### C-01: deopt 全局状态 `g_jit_deopt_state` 无同步
- **文件**: `interp_entry/entry_direct.cpp:630-648`, `jit/jit_deopt.cpp`
- **根因**: `g_jit_deopt_state` 是进程级全局 struct（非 thread_local），多线程同时去优化时竞态覆盖寄存器文件，导致解释器状态损坏
- **修复建议**: 改为 thread_local，或加 atomic ownership guard
- **原始 roadmap 覆盖**: 未覆盖

### C-02: Domain unloader 无 SATB 记录清除非域引用
- **文件**: `runtime-core/domain_unloader.cpp:91-108`
- **根因**: `ScanAndClearCrossDomainRefs` 在全局 safepoint 下用 `*ptr_slot = nullptr` 清除非域指针，但未记录 SATB。BGC concurrent mark 期间可能丢失活动对象
- **修复建议**: 清零前调用 `BgcSatbPreWriteBarrier` 记录旧值
- **原始 roadmap 覆盖**: domain_unloader 在 Phase 1 中修复了 `ClearDomainPointers`，但 SATB 问题未覆盖

### C-03: VEH 自旋锁不可重入（JIT SEH）
- **文件**: `LinuxSehHandler.cpp:81-95`, `WinSehHandler.cpp:56-77`
- **根因**: `AcquireLock()` 自旋锁不可重入。如果在 `RegisterCode`/`FindCodeByAddress` 持有锁时发生异常，VEH Handler 再次尝试获取同一锁 — 死锁
- **修复建议**: 改为可重入锁（recursive mutex）或添加重入检测
- **原始 roadmap 覆盖**: 未覆盖

### C-04: sigaltstack 未配置，栈溢出无法恢复
- **文件**: `pal/pal_eh_posix.cpp:83`
- **根因**: `SA_ONSTACK` 已设置但从未调用 `sigaltstack()` 分配替代栈。栈溢出触发的 SIGSEGV 在已耗尽的主栈上运行 handler = 双重故障
- **修复建议**: 线程初始化时调用 `sigaltstack` 分配 `SIGSTKSZ * 2` 替代栈
- **原始 roadmap 覆盖**: P1-1 添加了 SIGSEGV handler 但遗漏了 altstack 配置

---

## HIGH（6 项）

### H-01: `ClearMethodTableByGeneration` 释放顺序竞态
- **文件**: `runtime-core/method_table.cpp:67-69`
- **根因**: `fn_ptr.store(nullptr, relaxed)` 在 `module_gen.store(kInvalid, release)` 之前。编译器/CPU 可重排 relaxed store 到 release store 之后，使读者在 module_gen 已失效时仍读到非空 fn_ptr
- **修复建议**: 先 store gen（release），再 store fn_ptr（relaxed），或使用同一 release 原子操作
- **原始 roadmap 覆盖**: 未覆盖（atomic ordering bug）

### H-02: MIC fast path 无 `PalTryCallNoExcept` 保护
- **文件**: `interpreter/fast_dispatch.cpp:1424-1425`
- **根因**: `Handle_CallVirt` MIC hit/miss 路径直接 `CallDirectVoidPtr` 调用 native 函数，无 EH 保护。而 `Handle_Call_DoAotDirect` 有 `PalTryCallNoExcept`。MIC 路径性能更优但安全性更差
- **修复建议**: MIC 路径也包裹 `PalTryCallNoExcept`
- **原始 roadmap 覆盖**: P1-5 修复了 zero-arg 路径的 EH 保护，但 MIC 路径被遗漏

### H-03: `UnregisterTypeVTable` 可能释放 .rodata 指针
- **文件**: `runtime-core/vtable_registry.cpp`
- **根因**: codegen 注册的 TypeVTable 的 `slots`/`vtable_array` 指向 `.rodata`（非堆）。`UnregisterTypeVTable` 无条件 `CHAOS_IL2CPP_FREE` 这些指针 = 释放只读内存
- **修复建议**: 添加标志位区分堆分配与 .rodata 来源，或禁止对 codegen vtables 调用 Unregister
- **原始 roadmap 覆盖**: 未覆盖

### H-04: `Handle_StInd` SATB barrier 未检查 GC 指针
- **文件**: `interpreter/fast_dispatch.cpp:1908`
- **根因**: `BgcSatbPreWriteBarrier` 在任意地址上读取旧值。如果 `ptr` 指向栈或非托管内存，读取到伪对象指针会污染 SATB buffer，导致 GC concurrent mark 阶段误标记
- **修复建议**: 添加 `chaos_is_gc_pointer(ptr)` 守卫
- **原始 roadmap 覆盖**: 未覆盖（写屏障审计遗漏）

### H-05: Domain unload 未失效 JIT code 和 dispatch 条目
- **文件**: `runtime-core/domain_unloader.cpp`
- **根因**: UnloadDomain 清除了 vtable 指针，但未遍历 dispatch table / code registry 移除指向该 domain JIT code 的条目。线程仍可能通过 stale dispatch 条目调用已释放的 JIT code
- **修复建议**: domain unload 时迭代所有 dispatch 条目重置为安全 handler，取消注册 domain 的 JIT code
- **原始 roadmap 覆盖**: 未覆盖

### H-06: T4/AOT entry 无 GC 模式转换
- **文件**: `interp_entry/entry_direct.cpp:614-628`
- **根因**: T4 entry 直接 `native_entry(args_buf, ret_buf)` 在 cooperative 模式下调用。如果 native/JIT 代码进入阻塞系统调用，线程保持 cooperative 模式 = GC 无法安全挂起
- **修复建议**: 添加 `GC_TRANSITION_TO_PREEMPTIVE()` / `GC_TRANSITION_TO_COOPERATIVE()`
- **原始 roadmap 覆盖**: 未覆盖（runtime 基础设施问题）

---

## MEDIUM（12 项）

### M-01: `g_patch_generation` relaxed ordering — TCVC vs MIC 不一致
- **文件**: `fast_dispatch.cpp:1420-1421`, `patch_loader.cpp:827`
- **描述**: TCVC 使用 acquire/release epoch 提供顺序一致性，MIC 全部使用 relaxed ordering。MIC 读者可能在看到新 generation 时仍观察到 stale dispatch_ptr
- **原始 roadmap 覆盖**: 未覆盖

### M-02: Handle_CallVirt monomorphic shortcut 可能调用错误 override
- **文件**: `fast_dispatch.cpp:1471-1481`
- **描述**: vtable 解析失败时回退到 declaring type 的 AOT direct_fn，多态调用点会调用基类方法而非派生类 override = 语义错误
- **原始 roadmap 覆盖**: 已文档化为设计权衡

### M-03: JitStubDispatchImpl 无锁读 `precode->compiled` 竞争
- **文件**: `jit_precode.cpp:313`
- **描述**: `stale.exchange(false)` 在读取 `compiled` 指针后执行 CAS，窗口期间 `JitRecompileToTier1` 可能替换/释放 `compiled` = UAF
- **原始 roadmap 覆盖**: P2-1 (RCU) 部分覆盖但无锁读路径仍有窗口

### M-04: `ReconstructRegisterFile` 越界读（reg_index >= 16）
- **文件**: `jit_deopt.cpp:64-88`
- **描述**: `out_regs[reg_index] = ctx.gpr[reg_index]` 在 `reg_index >= 16` 时越界读 16 元素数组。当前所有 deopt value 为 `is_spilled=true` 未触发，但未来扩展时危险
- **原始 roadmap 覆盖**: P3-2 调研了 deopt 但未发现此编码 bug

### M-05: RCU retired 释放期不足（单次 recompile cycle）
- **文件**: `jit_precode.cpp:536-538`
- **描述**: `delete precode->retired` 仅延迟一个重新编译周期。如果方法不再重新编译，旧代码内存永远不释放（泄漏）；如果多线程竞争，可能释放仍在使用的 `JitMethod`
- **原始 roadmap 覆盖**: P2-1 (RCU) 只保护了 Tier1 重编译，单线程 TierManager 假设不全

### M-06: MIC `InterpreterObject*` cast 无 tag 验证
- **文件**: `fast_dispatch.cpp:1406-1407`
- **描述**: `pa.args[0] != 0` 只检查非空，未验证 `pa.tags[0] == ObjectRef`。非 ObjectRef 值的非零原始位通过 reinterpret_cast 产生无效指针
- **原始 roadmap 覆盖**: 未覆盖

### M-07: `InlineReverseMap` 无锁竞态
- **文件**: `jit_inline.cpp:144-174`
- **描述**: `Add()` 在编译线程无锁调用，`InvalidateCallers` 在 `HotpatchNameRegistry` 锁下修改 entries。数据竞态损坏 vector
- **原始 roadmap 覆盖**: 未覆盖

### M-08: PalGetMemoryStatus /proc/meminfo 解析失败静默返回 0
- **文件**: `pal/pal_mem_posix.cpp:66-96`
- **描述**: fopen/fgets 失败或无 MemTotal/MemAvailable 行时返回全零 struct，GC 用零计算 heuristics = 错误内存压力决策
- **原始 roadmap 覆盖**: 未覆盖（PAL 基础设施问题）

### M-09: PalVirtualDecommit 始终返回 true
- **文件**: `pal/pal_mem_posix.cpp:38-43`
- **描述**: `madvise` 返回值被忽略，调用者无法检测去提交失败
- **原始 roadmap 覆盖**: 未覆盖

### M-10: struct return ABI 在 direct AOT path 不处理 `ValueTag::Struct`
- **文件**: `fast_dispatch.cpp:1075-1084`
- **描述**: `Handle_Call_DoAotDirect` 只检查 `Void` vs non-Void。struct 返回值按 raw uint64 处理，不跟踪 struct_data 指针 = 生命周期管理不一致
- **原始 roadmap 覆盖**: Phase 2 部分（CachedCallInfo ret_tag）

### M-11: SEH 查找缓存 TOCTOU
- **文件**: `WinSehHandler.cpp:202-213`
- **描述**: TLS 缓存验证 `generation == gen` 后解引用 `nm`。重置/释放 `nm` 可能在验证后但在解引用前发生 = 释放后使用
- **原始 roadmap 覆盖**: 未覆盖

### M-12: Domain unloader 扫描堆时可能清除非指针数据的误命中
- **文件**: `domain_unloader.cpp:91-108`
- **描述**: 遍历整个 card table 读 `void**`，认为每个指针对齐的 slot 都是 managed pointer。非指针数据中恰好落在卸载域地址范围内的值被清零 = 数据损坏
- **原始 roadmap 覆盖**: 未覆盖

---

## LOW（5 项）

### L-01: `RegisterRuntimeVTable` 和 `ResolveVirtualMethodPointerByHandle` 缺少 `noexcept`
- **文件**: `vtable_registry.h:59-63, 117-119`

### L-02: `Handle_Call` 非 noexcept（MSVC C2712）
- **文件**: `fast_dispatch.cpp:1244`

### L-03: `sigjmp_buf` 被重入调用覆盖（单 slot 无栈）
- **文件**: `pal/pal_eh_posix.cpp:32, 103`

### L-04: `Handle_EndFinally` 缺少 kMaxStack 边界检查
- **文件**: `fast_dispatch.cpp:1606`

### L-05: PrecodeArena 编译器线程崩溃后无超时的自旋等待
- **文件**: `jit_precode.cpp:380-393`

---

## 优先级建议

### 第一优先（立即修复）
| 编号 | 文件 | 预估工作量 |
|------|------|-----------|
| C-04 | pal_eh_posix.cpp — sigaltstack 配置 | 1 天 |
| C-01 | entry_direct.cpp / jit_deopt.cpp — g_jit_deopt_state 线程局部化 | 2 天 |
| H-01 | method_table.cpp — 修正 atomic ordering | 0.5 天 |
| H-02 | fast_dispatch.cpp — MIC 路径加 PalTryCallNoExcept | 1 天 |

### 第二优先（本周修复）
| 编号 | 文件 | 预估工作量 |
|------|------|-----------|
| C-03 | LinuxSehHandler.cpp — 自旋锁改为可重入 | 2 天 |
| C-02 | domain_unloader.cpp — 添加 SATB recording | 1 天 |
| H-04 | fast_dispatch.cpp — StInd 加 gc_pointer 守卫 | 0.5 天 |
| H-03 | vtable_registry.cpp — codegen vtable 标记 | 1 天 |

### 第三优先（后续修复）
| 编号 | 文件 | 预估工作量 |
|------|------|-----------|
| H-05 | domain_unloader.cpp — JIT code 失效 | 3 天 |
| H-06 | entry_direct.cpp — GC 模式转换 | 2 天 |
| M-01~M-12 | 各处 | 各 0.5-2 天 |
