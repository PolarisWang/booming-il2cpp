# chaos-il2cpp 解释器 当前态复核 — 剩余问题 / 基础建设 / 性能方案

> **日期**：2026-08-13 ｜ **性质**：对 2026-08-13 解释器优化专项交付后（HEAD `25c66b016`）的**现行态复核**
> **前置**：`interpreter-capability-peak-plateau-valley-2026-08-13.md` + `interpreter-deepdive-2026-08-13.md` + `DELIVERY-2026-08-13.md`
> **方法**：人工首读关键路径 + 2 个 Explore agent 只读扫源码（GC/线程正确性 + opcode/性能）。所有 file:line 为现行 HEAD 实测。

---

## 〇、复核要点：几条"已交付修复"的真实状态

| 原评估项 | 评估时的断言 | 现行实测 | 判定 |
|---|---|---|---|
| null/越界抛异常（谷#2） | "静默 push null/skip" | **只修了 FastExecute 层**；Register VM 层 `Reg_LdElem/Reg_StElem` 仍静默 null/skip | ⚠️ **半修** |
| in-band SafepointPoll（谷#3 档B） | "未做" | 已在热回边接入（`fast_dispatch_execute.inc:452-471`），含 fast-path 原子闸 + try/catch 转译 | ✅ 已修（帧可扫描性/双路径仍待强回归） |
| LdVirtFtn 快路径虚分派 | "Handle_Unsupported" | 两层均接 `CodegenLdVirtFtn`→`ResolveVirtualMethodPointer`（`d7009096c`） | ✅ 已修 |
| 反向注册 GC root（STSFld） | "危险" | 三层 StSFld 全 barrier-free（`e6ea531e7`） | ✅ 已修（代价：static 无根） |
| 异常对象跨边界保留（谷#4） | "丢失" | `minimal-blast` 保留（`505f678e5`） | ✅ 部分 |

---

## 一、剩余问题（按严重度排）

### 🔴🔴 P0-P1 — `chaos_is_gc_pointer` 写屏障闸：**below-base 的 Gen1/多 nursery 段漏判 → 跨代边丢失 → BGC UAF**（GC agent 实测确认）
`gc_api.h:105-124` 的 nursery 判定只覆盖**单个主 nursery 窗口** `[g_nursery_range_begin,end)`（`:117`），而该窗口只由 `gc_region.cpp:647-649` 设为主 nursery；**Gen1 是独立 Region**（:640-643）、`RegionManager` 支持**多 nursery 段**（`AddNurseryRange` :1205-1225），posterior 都不更新那单个窗口。
→ 一个**真实** GC 对象若位于 `g_heap_base` 之下、主 nursery 窗口之外（secondary/Gen1 段），`chaos_is_gc_pointer` 返回 **false** → 受 guard 的 barrier **跳过 SATB/card** → old→young 跨代边被 drop → BGC 期间该对象被误回收 → **UAF**。这正是 `c606783cc` 想关掉的失败模式被**反转**。
- 应改用 `RegionManager::IsNurseryPointer`（`gc_region.cpp:1177-1203`，覆盖全全局范围），而非单个 DirtyCard fast-skip 窗口。使用点：`interpreter_vm.cpp:842,874,1883`、`fast_dispatch_object.inc:112,169,249,385`、`fast_dispatch_arithmetic.inc:447`、`jit_helpers.cpp:99,103,130` 及生成 AOT `native-aot.generated.cpp:9076,9113,9128,9798,9826`。

### 🔴 P1 — `CodegenStSFld` 仍反向注册 GC root（P0 止血不完整，GC agent 确认）
interpreter 自己三层 StSFld 已 barrier-free（`e6ea531e7`），但 **codegen/AOT 侧 `CodegenStSFld`（jit_helpers.cpp:486）仍无 guard 地反向注册**：
- `jit_helpers.cpp:492-495`：`JitSatbPreWriteBarrier(&g_static_fields[offset].obj); BgcRecordRootChange(&g_static_fields[offset].obj, old);` — **无 `chaos_is_gc_pointer` 守卫**。
- **live**：`jit_codegen_emit.cpp:1367`（`EmitRuntimeHelperCall(::CodegenStSFld)`）为 AOT/codegen StSFld 发射。
- 这就是 memory 记录的 "P0止血不完整(CodegenStSFld 仍反向注册 root, 三路StSFld不一致)" 的第三路。`g_static_fields` 是非 GC `CHAOS_IL2CPP_VECTOR(InterpreterValue)`（`interpreter_vm.cpp:38`，未注册 `GcRegisterStaticRootRange`）。Root-change drain 虽经 `FindPage` 过滤（`gc_bgc.cpp:206-215`），但反向注册仍把垃圾槽写进 `MT->root_change_buffer_`，且当值恰好别名 GC 段时是真实悬挂 root 源。

### 🟠 P2 — null/越界检查**层间不一致**（谷#2 只修了一半）
快递层 `FastExecute`（computed-goto）的 `Handle_LdElem/StElem/LdElemA` **已抛**（`fast_dispatch_object.inc:42-61,129-184,428-447`，`15cf21583`）。
但**寄存器机层 T2/T3**（`ir_reg_alloc.cpp` `Reg_LdElem`/`Reg_StElem`）**仍静默**：
- `Reg_LdElem` null→置 Null 静默（1307-1311）、flat OOB→Null（1314-1318）、ref OOB→Null（1364-1368）。
- `Reg_StElem` null→skip（1403-1406）、flat OOB skip（1409-1412）、ref OOB `resize`（自动扩容，非抛 IOORR）。
- 快递层 flat OOB store 也静默 skip（141-143），ref OOB store 自动 `resize`。

→ **同一条 IL 在两层抛/不抛不一致**：走 T1-computed-goto 会抛，走 T2/T3 寄存器机不抛。这会让解释器对非法的 null/越界表现不一致，掩盖 bug。需把 `15cf21583` 的语义补齐到 `ir_reg_alloc.cpp` 的数组 op。

### 🟠 P2 — 快路径 fault 型 threw 无异常对象类型区分
FastExecute 各 handler（越界、除零 `fast_dispatch_arithmetic.inc:60,79`、栈下溢、null 数组）都只置 `frame.threw_exception=true; frame.pc=9999`，**不填 `exception_obj_val`**，也不区分 NRE/IOORR/DivByZero。与显式 `Throw`（`fast_dispatch_seh.inc:31` 填 exception_obj_val）不同。→ 用户 `catch (NullReferenceException)` 与 `catch (IndexOutOfRangeException)` **拿到的是同一裸状态**（或 null 异常对象），语义不精确。需核实对 fault 型异常是否物化并上抛正确类型。

### 🟡 P3 — `ldtoken`/`sizeof`/`initobj`/`ldstr` 仍是占位/沙箱
- `ldtoken`：push 整数 token，非 `RuntimeTypeHandle`（`fast_dispatch_object.inc:282-286`、`ir_reg_alloc.cpp:1623-1628`）。
- `sizeof`：读立即数，不运行期解析类型大小（`fast_dispatch_object.inc:298-302`）。
- `initobj`：memset 16 字节 `InterpreterValue`，非真实对象布局（`fast_dispatch_object.inc:288-296`、`jit_helpers.cpp:696-705`）。
- `ldstr`：push 裸 `const char*`，非 intern/UTF-16/GC 托管 String（`fast_dispatch_core.inc:336-344`、`fast_dispatch_execute.inc:329-337`）。

### 🟡 P3 — SIMD/向量 opcode 完全缺失 + **`SimdFma`(110) 表越界空指针（潜在崩溃）**
- `Simd(107)/Popcnt(108)/Lzcnt(109)` → `kOp_Unsupported`（`fast_dispatch_execute.inc:232`）；`Abs/Min/Max(100-102)` → `kOp_Unsupported`（:229）。
- **`SimdFma`(110)：`kOpTable[128]` 只初始化 0-109，索引 110 是空指针，而边界检查是 `_op > 127`**（:239,552）→ 若 IR 曾发射 `SimdFma`，`goto *nullptr` 直接崩溃。当前不可达仅因发射器不产，但属**潜在 crash 隐藏雷**，应加表项或把边界收窄。
- 寄存器层 `kRegDispatchTable[100]` 只覆盖 0-99，`op_val>99` 判为 threw（:2704）→ 100+ 全 Unsupported。

### 🟡 P3 — `chaos_is_gc_pointer` 反向误判（Area 1a：沙箱对象地址 ≥ base 被当 GC 对象）— contained
补充方向与上 P0-P1（Area 1b）相反，但**风险已被 GC 实扫过滤含容**：`chaos_is_gc_pointer` 用 `addr ≥ g_heap_base` 启发式（`gc_api.h:110`），GC 实扫用**精确** `IsValidManagedObject`（首字是否为已注册 TypeInfo，`gc_layout.h:304`）+ `FindPage` 段归属过滤（`gc_old_gen.cpp:1033`、`gc_bgc.cpp:1671`）。解释器沙箱对象（`std::malloc`/`PalHeapAlloc`，`fast_dispatch_core.inc:106-117`）地址若恰好 ≥ base，会被误发 SATB/card（浪费），但 re-mark 会先 `FindPage`/`IsValidManagedObject` 拒掉非托管值 → **不促成损坏，仅性能/健壮性**。真正危险的是上 P0-P1 的 below-base 漏判方向。

### 🟡 P3 — blocking 调用 / 长原生调用无 preemptive 切换（谷#3 档A 未做）
解释器慢路径与长 P/Invoke / 原生调用期间**无 `GC_TRANSITION_TO_PREEMPTIVE`**（`gc_transition.h` 仅在 T4 `entry_direct.cpp:802-849` 用）。blocking 调用 + 热回边 poll 够不到 → GC 只能靠信号抢占，长调用期间 GC 延迟/死锁窗口仍在。评估将其列 P1，现仍未落地。

### 🟢 P4 — 解释器专用性能锚缺失（测量缺口未决）
`baseline/README.md` 诚实标注：standalone `InterpreterVM::Execute` 驱动 segfault 回退；**解释器专属 ns/op 无可信基线**。`register_vm_profiler` 实际**惰性**（`CHAOS_IL2CPP_VM_PROFILER_ENABLED` 默认 0，唯一调用点 `entry_direct.cpp:780`，`DumpProfilerToFile` 无调用者）；opcode 直方图 `DumpFastExecuteOpcodeHistogram` 也无调用者。

---

## 二、需要搭建的基础建设

### 1. 解释器专属 benchmark/测量基础设施（当前硬阻塞）
- 无内联 standalone 驱动，`InterpreterVM::Execute` 需完整 runtime/thread/GC 装配，裸驱动 segfault。
- **务实路径**：在 `entry_direct.cpp` 5-tier 入口加 wall-clock/分层 tier 计数器（`TierCounters` 已有聚合 hits），或走"hotpatch 应用一个真实方法 + 循环"的生产路径测 ns/op。**建议用户拍板测量方式**（现状 options (a)/(b)/(c) 未定）。
- 接线 `DumpFastExecuteOpcodeHistogram`/`register_vm_profiler` 到可执行入口（`--benchmark` flag），让 opcode 频率/每方法 cycles 可观测。

### 2. 层间一致性测试（防止再次只修一层）
- 现 null/越界只在 FastExecute 层测过；需**同时跑 T2/T3 寄存器层** 的数组异常用例，或加"跨层同 IL 行为一致"的门禁。

### 3. GC-stress 回归（解释器 safepoint 专用）
- 长循环 + BGC 期间，解释器帧被 GC 扫的一致性（`interp_frame_scanner`）无 GC-stress 覆盖；信号挂起 + 回边 poll 双路径竞态无专项。需补 long-loop + GC 周期交集测试。

### 4. 写屏障闸：改用 `RegionManager::IsNurseryPointer`（P0-P1 修复基建）
- `chaos_is_gc_pointer` 的 nursery 判定需从"单个主 nursery 窗口"换成**完整 `RegionManager::IsNurseryPointer`**（`gc_region.cpp:1177-1203`，覆盖 Gen1 + 多 secondary 段），否则 below-base 真实对象漏判 → 跨代边 drop → BGC UAF。同时消引 Area 1b。
- 附：沙箱分配区 vs `g_heap_base` 的地址分布实证，给反向误判（Area 1a）一个确定性结论。

---

## 三、需要调整的性能方案

### 1. 寄存器机层（T2/T3）是最重的解释层 — 建议优先优化
- `RegisterExecute` **每个 op 都是函数调用**（无内联），加上 `dispatch_next` 每 op 的 OSR 比较/回边检查 → 结构固定开销最高。
- 对比 FastExecute：11 个内联 op，其余一个函数调用，epilogue 2 次计算跳转。**寄存器层若能内联 几 个高频 op（Ldc/LdLoca/Add 等）或减免每 op 的 dispatch 循环，收益最大。**
- 注意 `reg_f32/f64` 从 `gpr` 读（`ir_reg_alloc.cpp:128-137`）对 FPR 索引是潜在未定义行为 → 修，避免高 FP 寄存器读错。

### 2. FastExecute epilogue 每 op 固定成本
- 每个非内联 op 都：handler 调用+返回 → `goto kOp_Epilogue` → `if (frame.pc < frame.last_pc)`（含 suspend_seq load + OSR 计数）→ `goto kOp_Next` → 再 `goto *table`。**2 次计算跳转 + 总是执行的分支比较**。建议：把 suspend_seq fast-path 与 OSR counter 合并成单次 likely-predicted load，或对内联 op 跳过 epilogue 直接查表。

### 3. 层选择策略：是否应多用 T2/T3 寄存器机？
- 寄存器机层虽较慢，但覆盖 T1-computed-goto 之外的 op。当前 tier 升级路径 `kStackInterpreted → kQuickJit`，绝大多数方法会升至 JIT（T4）。**解释器 per-op 速度优先级低于"让更多方法尽快上 JIT"**。性能方案应优先：
  1) 提高 JIT 覆盖率（减少永久 `kJitSkip` 回退到解释器的方法，尤其是 try-heavy / 递归 / SIMD keep-native），
  2) 再优化解释器本身。
- 应量化：到底多少方法永久停在解释器（`kJitSkip`），决定解释器 per-op 优化值不值得做。

---

## 四、一句话收敛

**两个确认 P0-P1 内存安全洞最优先**：(1) `chaos_is_gc_pointer` nursery 只盖主窗口 → below-base Gen1/多段对象漏判 → 跨代边 drop → BGC UAF，改用 `RegionManager::IsNurseryPointer`；(2) `CodegenStSFld`（jit_helpers.cpp:492-495）仍反向注册非 GC `g_static_fields` 为 root，补 `chaos_is_gc_pointer` guard（对齐已修的 interpreter 三层 StSFld）。**即时补修**：把 `15cf21583` 的 null/越界抛异常语义对齐到 `ir_reg_alloc.cpp` 寄存器层（两层不一致）；补 `SimdFma(110)` 表空指针（潜在 crash）；接线 profiler/histogram 到可执行入口。**基础建设**：解释器专属 benchmark（测量方式待用户定）+ 跨层一致性门禁 + 解释器 safepoint 的 GC-stress 回归。**性能**：寄存器机层 per-op 函数调用是最大可改项，但优先级低于提高 JIT 覆盖率；先量化 `kJitSkip` 永久解释占比再决定投入。

---

## 五、关联
- 能力总评：`interpreter-capability-peak-plateau-valley-2026-08-13.md`
- 攻坚深化：`interpreter-deepdive-2026-08-13.md`
- safepoint 根因/选项：`interpreter-inband-safepoint-rootcause-options-2026-08-13.md`
- 专项交付/测量缺口：`docs/dev/in-progress/interpreter-optimization/DELIVERY-2026-08-13.md` + `baseline/README.md`
- 证据 agent：opcode/性能首读 + GC/线程正确性首读（`chaos_is_gc_pointer` gate + `CodegenStSFld` + safepoint 覆盖 + frame scanner）
