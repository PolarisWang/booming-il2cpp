# chaos-il2cpp 解释器 — 综合深度分析报告（功能 + 性能 + 正确性）

> **日期**：2026-08-13 ｜ **性质**：对解释器各功能模块与性能结构的**第一手工程深度剖析**（非 gap 列举）
> **方法**：4 个维度独立首读源码 + 人工交叉验证。file:line 为现行 HEAD 实测。
> **定位收敛**：解释器是"热更专用分层解释 VM + 到 JIT 的梯子"——不是 il2cpp 全功能 fallback。本报告在此定位下评估工程成熟度。

---

## 〇、综合一句话

**工程成熟度整体是 B（高于多数自研解释器），缺陷集中在三处**：(1) **跨层状态转移**（OSR/deopt 的 pool 归属、tier_state 竞态）——真实内存安全缺陷；**(2) 名称承诺超过实际**（"精确扫描"实为保守安全、"回边自 drain"实为抢占兜底、P/Invoke 边界无 GC 转换）；**(3) 最热路径付费**（每回边 safepoint + 双 OSR 检查、RegisterVM 每 op 函数调用）——性能结构问题。

---

## 一、功能工程成熟度评级（4 维度代理汇总）

| # | 功能 | 评级 | 关键证据 | 工程评估 |
|---|------|------|---------|---------|
| 1 | **执行管线 5-tier** | **B** | entry_direct.cpp | 分层诚实；但 T1 2-instr fast path 提前 return 不递增 call_count → 热 getter 永不升级；Step C/D 路由 gating（`reg_ir_data==nullptr`）与"全 SEH 支持"能力声明矛盾 |
| 2 | **Tier 升级/OSR/deopt** | **C** | osr_state.cpp, fast_dispatch_execute.inc:104-110 | **OSR 跨层丢 pool-flag → 错配 free（真实缺陷）**；tier_state/direct_ptr 读取未配对写者 release fence（并发 JitRecompileToTier1 竞态）；deopt 跨格式 pc 不验证 |
| 3 | **Computed-goto 内核** | **B+** | fast_dispatch_execute.inc:202-255 | 128 表 + `_op>127` guard + kOp_Unsupported 补齐 100-127——工程成熟度高；但 `_NC` 内联 handler 无 sp 越界检查 + MSVC/goto 双源真相 |
| 4 | **Frame 内存模型** | **B-** | fast_dispatch.h:44-45, fast_dispatch_core.inc:402-426 | 定长 64/32 + TLS 池干净；但 **LdLoc/StLoc 无 idx<32 检查 → >32 局部方法越界写**；SetupFastFrame 非全清零 |
| 5 | **SEH/异常** | **C+** | fast_dispatch_seh.inc:8-191, entry_direct.cpp:51-87 | 帧内两阶段真实现；但 TypedCatchCheck **线性 module/type 扫描无索引**、跨帧传播塌缩成裸 chaos_managed_exception 丢异常身份、DoMIC 调用丢异常对象、rethrow 从 handler-body pc 搜索 |
| 6 | **热更新集成** | **B-** | patch_loader.cpp, patch_method_lower.cpp | .patchdata v2 注册路径是减 JSON+regalloc 的真优化；但 v2 的 cached_ir 是 1-instr placeholder hack、**patch body 更新无 re-lowering/call_cache 失效** 只靠机会重查 |
| 7 | **对象模型沙箱** | **C+** | interpreter_vm.h:184-323 | 内部自洽（小缓冲字段、tag 引用数组、flat 快路径）；LazyBox/BoxedValue/InterpreterObject 三表示重叠，正确性依赖每个消费者记得边界物化；static 无 barrier |
| 8 | **P/Invoke & interop** | **D+** | precache_dispatch_raw.cpp:277-295, interop_stubs.cpp | **解释器路径调 native 无 cooperative→preemptive 转换**（对照 JIT 路径 entry_direct.cpp:808）→ GC 挂起契约破坏；COM/GetExceptionCode/MarshalGetExceptionForHR 是 return-0 stub |
| 9 | **delegate/泛型/反射** | **C** | interpreter_vm.cpp:1647-1673, delegate_helpers.cpp | 虚分派真实（epoch 缓存+vtable）；但 LdFtn/LdVirtFtn push 裸 fn 指针非真实 delegate、组合是自定义 vector 模型、泛型构造真实但反射多 stub |

---

## 二、性能结构深度剖析

### 每 tier 真实 per-op 开销（结构模型，非测量值）

| Tier | 每 op 固定开销构成 | 估算 | 相对 FastExecute |
|------|------------------|------|------------------|
| **FastExecute**（computed-goto） | 间接跳转（mispredict 40+cy）+ epilogue 双回边检查 + 每回边 suspend_seq acquire-load + 1/64 decimated 中断 | straight-line ~11-25 cy | 基线 |
| **RegisterVM**（RegisterExecute） | 每 op 独立函数调用（`Reg_Add`...）+ `reg()/set_reg()` 每操作数 64-bound 分支（2 读+1 写）+ 大 frame 差寄存器分配 | ~2-3× FastExecute | **最重的单层** |

**关键结论**：RegisterVM 不是比 FastExecute 更慢（同样 computed-goto），而是 **(a) 无内联 (b) 分支存器文件 3×/op (c) 大 frame → 差寄存器分配** 三因叠加。对 RegisterVM 目标的小型方法，这反而比 FastExecute 慢——**它是性能最差的单层**。

### 成本中心排序（性能瓶颈）

1. **Call/CallVirt**（两层）：pop args + EH 包裹调用 + MIC/PIC/vtable 解析 + 任何 miss 走 InterpreterDispatchRaw ~100+ cy；calls 主导真实 bytecode。
2. **FastExecute 每回边 safepoint + 双 OSR 检查**：热形状（紧循环）的**常数每迭代税** ~20-40 cy，整个解释期都付。
3. **多态 computed-goto mispredict**：基底层（解释器硬结构下限）。
4. **RegisterVM per-op 函数调用 + 分支存器文件**。
5. **热路径隐藏堆分配**：非 flat `ArrayStorage.elements` vector resize（`new T[]` of refs）、`SmallFieldArray` >2 字段 malloc（Box/NewObj）、RegisterVM Call/Calli >8 参数 **malloc/free×2/次**。
6. **GC 写屏障**：`StFldBarrier`/gc-array `StElem` ~50-150+ cy（真 GC target 时）。
7. **Tier 升温经济学**：**QuickJIT 在 call count=1 触发**——单调用方法付全量编译（无 interpret-first 守卫）；到 T4 稳定前最多 5 次编译。

### 每层最大提速机会

- **FastExecute**：把 suspend_seq safepoint probe 从「每回边」移到「1/64 decimated 中断槽」，合并双 `pc<last_pc` epilogue。省 ~20-40 cy/迭代，无正确性改动——**最高 ROI**。
- **RegisterVM**：选项 (a) 扁平单索引数组（去掉 reg()/set_reg() 64-bound 分支）+ 内联简单 handler；或 (b) **鉴于它对比 FastExecute 是 wash-to-worse，直接让 FastExecute 成为 T2 target、RegisterExecute 降为 legacy/SEH-only**——消除整条执行路径。

---

## 三、正确性风险索引（跨 4 维度综合）

| # | 风险 | 严重度 | 证据 | 性质 |
|---|------|--------|------|------|
| 1 | **OSR 跨层 pool-flag 丢失 → 错配 free（堆损坏）** | 🔴 P1 | osr_state.cpp:142-156 不复制 `tracked_is_pool`；ir_reg_alloc.h:230-236 CleanupTracked 无条件 FREE | 真实缺陷 |
| 2 | **interpreted→native 无 cooperative→preemptive 转换** | 🔴 P1 | precache_dispatch_raw.cpp:277-295；对照 entry_direct.cpp:808-811 | GC 挂起契约破坏 |
| 3 | **back-edge-only safepoint**：straight-line/递归方法可超 100ms 需抢占劫持 | 🟠 P1 | fast_dispatch_execute.inc:471（仅 `pc<last_pc`）；kOp_Next:516 只看 pending_abort/interrupt 不看 suspend_seq；thread_state.cpp:551 | 覆盖缺口 |
| 4 | **tier_state/direct_ptr 读取未配对写者 release fence** | 🟠 P2 | entry_direct.cpp:802/842 双 relaxed/acquire load | 并发竞态 |
| 5 | **LdLoc/StLoc 无 idx<32 越界；_NC 内联 handler 无 sp 越界** | 🟠 P2 | fast_dispatch_core.inc:402-426, fast_dispatch_execute.inc:262-281 | 越界写 |
| 6 | **"精确扫描"实为保守安全**：value-before-tag 时序短暂把整数标 ObjectRef → 假 root | 🟢 P2 | fast_dispatch.h:47-57, interp_frame_scanner.cpp:28-45 | 只 over-retain 不 UAF |
| 7 | **`g_static_fields` 双策略跨模式别名**：interpreter barrier-free vs JIT helper rooted | 🟢 P2 | interpreter_vm.cpp:587 vs jit_helpers.cpp:462-507 | 模式不一致 |
| 8 | **SEH 异常身份跨帧丢失**：DoMIC 丢异常对象、rethrow 从 handler pc 搜索 | 🟢 P2 | fast_dispatch_call.inc:90-95 | 语义保真 |
| 9 | **double-ack 竞态**：in-band poll + 信号劫持同线程 | 🟢 P2 | thread_state.cpp:261/344/373-389 | 边界竞态 |
| 10 | 热点隐含堆分配（vector/SmallFieldArray/call>8 malloc） | 🟢 P3 | 见 §二.5 | 性能 |

**结论**：**无 P0（立即崩溃）**。最坏现实结果（>100ms straight-line 无法主动 drain → 抢占、或 OSR 错配 free）是 P1。

---

## 四、性能可测量性现状

- **唯一可用 ns/op harness**：`tests/unit/.../interpreter/tiering_benchmark.cpp`——测 T1-T4 的 21-op 算术/8-op 寄存器方法 ns/op、PIC vs NO-PIC callvirt、并调 `DumpFastExecuteOpcodeHistogram()`。
- **缺口**：直方图计数器 `g_fast_op_freq[]` 默认关（`CHAOS_IL2CPP_PROFILE_ENABLED` 默认 0）；RegisterVM profiler `VmProfileScope` 只在 method-replacement 分支构造、未接热路径、`DumpProfilerToFile` 无调用者。README 声明的 standalone VM 驱动 segfault 回退 → **解释器专属 ns/op 锚仍未落地**（tiering_benchmark 是唯一完整目标）。
- **已落地（上轮 `ae93376a4`）**：entry Scriban shutdown 加双 profiling hook（宏门控），使 profile build 下 `--benchmark` 暴露 opcode 直方图。

---

## 五、优先攻坚建议（按 ROI）

**正确性（先）**：
1. 修 **OSR pool-flag 丢失**（`osr_state` 增 `tracked_is_pool` 转移，CleanupTracked 分池/堆路径）——真实内存安全。
2. 修 **interpreted→native preemptive 转换**（对齐 T4，调用 native 前 `GcTransitionGuard`）——GC 契约。
3. 给 back-edge-only safepoint 补 straight-line 覆盖 / 把 suspend_seq 并入 decimated 检查——去抢占依赖。

**性能（后）**：
4. FastExecute 撤「每回边 safepoint + 双 OSR」——最高 ROI ~20-40 cy/迭代。
5. 重估 RegisterVM：或修存器文件 + 内联，或让它降级为 legacy（FastExecute 接 T2）。
6. 补 tiering_benchmark + PROFILE build 的真实 opcode 直方图——量化化，再优化。

---

## 六、关联
- 峰平谷总评：`interpreter-capability-peak-plateau-valley-2026-08-13.md`
- 现行态复核（上轮已修项）：`interpreter-review-current-2026-08-13.md`
- 攻坚深化：`interpreter-deepdive-2026-08-13.md`
- 性能基线现状：`docs/dev/in-progress/interpreter-optimization/baseline/README.md`
