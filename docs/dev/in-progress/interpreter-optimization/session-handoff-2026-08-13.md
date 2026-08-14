# 解释器修复计划 — Session 交接文档

> **交接日期**：2026-08-13 ｜ **交接 commit**：`cbe842a50`（main, 工作区干净）
> **承接前置**：`docs/dev/in-progress/interpreter-optimization/interpreter-fix-plan-2026-08-13.md`（主计划文档）+ `docs/dev/assessments/interpreter-deep-analysis-2026-08-13.md`（4 维度发现）
> **本文档目的**：让新 session 无缝承接剩余任务清单，避免重读全部源码。

---

## 〇、任务框架（为什么做这些）

用户要求"review 解析器 → 修复所有问题 + 补测试"（依据 4 维度深度分析产出的 `interpreter-deep-analysis-2026-08-13.md`）。计划分轨道：**正确性 P1 → P2 → 性能 → 基建**。所有修复在共享工作区 main 上，但**该工作区被并行线（jit/gc）实时编辑**——commit 时务必只 stage 自己的文件（见 §四 环境坑）。

---

## 一、已完成并提交（9 个 commit，全部编译 + 单测验证绿）

| commit | 任务 | 修复内容 | 验证 |
|--------|------|---------|------|
| `300125cde` | **D2** | 跨层一致性门禁：新 `cross_tier_consistency_test.cpp` 驱动同 IL 走 FastExecute vs RegisterExecute 断言一致（防层间漂移） | cross_tier 6/6 |
| `300125cde` | **A1(P1)** | OSR 跨层 pool-flag 丢失→错配 free：`OsrState`+`RegisterFrame` 加 `tracked_is_pool[]`，CleanupTracked 按位 skip FREE；CaptureFastFrame/CaptureRegisterFrame/TryFastOsrPromotion 拷 pool 位 | osr_state 15/15, ir_reg 46/46 |
| `d9c7a296d` | **A3+C1(P1)** | back-edge-only safepoint 缺口 + 回边税：把 `suspend_seq` 检查并入 kOp_Next **每 64 指令 decimated 槽**，撤回边 probe | profile 3/3, cross_tier 6/6 |
| `a1ce897de` | **B6(P2)** | T1 2-instr fast path 不递增 call_count：op0 是 LdArg/LdcI4/LdNull 时补 fetch_add | profile 3/3 |
| `16b7e58c3` | **B2(P2)** | LdLoc/StLoc 无 `idx<kMaxLocals` 越界：加 bound-check fault（>32 局部方法防越界写） | cross_tier 6/6 |
| `f0068e380` | **B1(P2)** | direct_ptr 后台 recompile 跨平台内存序：`JitRecompileToTier1` release-store + dispatch kQuickJitted acquire-load（std::atomic_ref） | cross_tier 6/6 |
| `494cd2f8d` | **B3(P2)** | SEH 异常对象丢跨 DoMIC/RegisterExecute：`PalTryCallNoExcept` 传 `out_exception_object`，caught 填 `frame.exception_obj_val`/`result.exception_obj` | profile 3/3 |
| `0611480a3` | 文档 | 更新修复计划进度 + A2 重新分类 | — |

### A2 的**重要方向修正**（调查推翻原前提）
原判"解释器调 raw native `direct_fn` 无 GC transition"。**调查发现前提错误**：`direct_ptr` = codegen 发射的函数体/stub，**不是 raw native**。P/Invoke 的 `direct_ptr` 是 `EmitPInvokeMethod` 发射的 stub，该 stub **内建** `GC_TRANSITION_TO_PREEMPTIVE/COOPERATIVE`（`needsGcTransition = !IsSuppressGCTransition`，`MethodEmission.PInvoke.cs:121`）。managed 的 `direct_ptr` 是 AOT managed 体（应 stay COOPERATIVE）。**盲包所有 direct_fn 会破坏 managed 语义**。唯一真缺 transition 的是 `[DllImport(SuppressGCTransition=true)]`——那是开发者显式选择，非 bug。**A2 重新分类为"非缺口"，不改代码。**

---

## 二、剩余任务 —— 精确状态与决策点

### 🔴 B4(P2)：g_static_fields 双策略 —— **基本已闭合**，只需验证 + 文档确认
- 问题：interpreter 三层 StSFld barrier-free（假设非 GC `InterpreterObject*`）vs `CodegenStSFld`（jit_helpers）带 SATB/root-change，同一 `g_static_fields` 槽跨模式不一致。
- **已部分闭合**：上一轮 `4b2900225` 已给 `CodegenStSFld` 加 `chaos_is_gc_pointer` guard（只有真 GC 值才发 barrier）→ 与三层在"非 GC 值不发 barrier"上一致。
- **剩余动作**：无代码改动。**可选防御**：把 `g_static_fields` 语义固化为"非 GC"并注释统一三层 + codegen（减少未来误用）。**依赖决策**：若未来做对象模型统一（用户已中止 1.2），此双策略需按新模型重审。
- **建议**：标记为「已由 4b2900225 闭合，文档确认即可」，不再改代码。

### 🔴 B5(P2)：double-ack 竞态（thread_state 信号 + in-band poll 同线程）
- 问题：cooperative 线程已 in-band `PalEventWait` parked 时，100ms 超时信号 handler 又 re-ack + RIP 重定向到 trampoline → double-ack + RSP 改写冲突。
- **复杂度**：高并发敏感改动，涉及 `thread_state.cpp:261/344/373-389`，与并行 GC 线的 A2b barrier critical-section guard 交互。仓库已有 `ack-and-continue`（barrier_inflight guard）。**需 GC-stress 验证圆型**（长 straight-line 已由 A3 覆盖，但 double-ack 需并发 GC 周期测试）。
- **建议**：**需要专门 session + GC-stress 验证环境**，不宜在无 stress 环境盲改。改法参考 `thread_state.cpp` `PreemptiveSuspendHandler`：分离"already-parked"（in-band wait 中）与"hijacked-mid-run"，避免重复 ack + RSP 改写。

### 🟠 C2(P3)：RegisterVM 性能 —— **架构级重构，需专门专项**
- 问题：RegisterExecute 每 op 独立函数调用 + 分支存器文件（`reg()/set_reg()` 64-bound 分支 ×3/算术 op），对小型方法比 FastExecute 慢。
- 方案：(a) 扁平单索引数组（去 bounds 分支）+ 内联高频 op + 复用 LazyBox（中风险）; (b) FastExecute 接管 T2、RegisterVM 降级 SEH-only（架构级，需重定义 tier 语义，2-3d）; (c) 4A+4C 组合。
- **需先做 D1**（ns/op 锚）才能量化收益/风险。**依赖已确认**：RegisterVM 是核心 tier 路径（`kTerminalTier=kOptimizedRegister`），不能随便删。
- **建议**：**待 D1 建锚后**做，优先 4A（扁平存器文件+内联），4B 作 long-term。改 reg 布局**需连带验证 deopt/OSR 的 reg 恢复**（CaptureNativeFrame 靠 reg index）。

### 🟠 C3(P3)：QuickJIT 升温经济学
- 问题：`kQuickJitThreshold=1`（首调即编译），单调用方法付全量编译；到 T4 稳定前最多 5 次编译。
- **建议**：评估 interpret-first（提升 threshold）或"仅真 hot 才 QuickJIT"。低风险试验性改动，可独立做，但需 D1 锚量化"短生命周期方法"收益。

### 🟠 C4(P3)：热路径隐藏堆分配 —— **call>8 已 done，SmallFieldArray 几何增长已 done**
- 问题：非 flat `ArrayStorage.elements` vector resize、`SmallFieldArray`>2 字段 malloc（Box/NewObj）、RegisterVM Call/Calli >8 参数 **malloc/free×2/次**。
- **✅ 已做（`806a4f107` call>8 合并单块 + `d80230fd3` SmallFieldArray 几何增长）**：`CoalescedCallArgs(ac)` layout helper 消除 call>8 双 malloc/free；`SmallFieldArray::reserve` 从 exact-fit 改 ~1.5x/2x 几何增长，消除增量写字段(resize 1,2,6)的重分配 churn。
- **剩余**：`ArrayStorage.elements` vector resize（新数组每次单次 malloc，属固有，非 churn；是 `std::vector` 已几何增长，无跨层优化价值）。测试：SmallFieldArray inline+geometric 已补。

### 🟢 D1(P3)：解释器 ns/op 锚 + profiler 接线 —— **代码已落地 + 回归绿，PROFILE 输出验证待跑**
- 问题：`register_vm_profiler`（`CHAOS_IL2CPP_VM_PROFILER_ENABLED` 默认 0，只 method-replacement 分支）+ `DumpFastExecuteOpcodeHistogram`（PROFILE 宏门控）都无调用者/默认关；唯一完整 ns/op 目标是 `tiering_benchmark`。
- **已做部分（上轮 `ae93376a4`）**：entry Scriban shutdown 加双 profiling hook（`DumpFastExecuteOpcodeHistogram` + `DumpProfilerToFile` + `ResetProfiler`，宏门控）。
- **✅ 本轮已落地（commit `b4d1ec85b`）**：
  1. `VmProfileScope` 接入 **RegisterExecute 热路径两处**：`entry_direct.cpp` Step-B + `fast_dispatch_execute.inc` OSR promotion（key=patch_method->token）。
  2. `register_vm_profiler.h`：`CHAOS_IL2CPP_VM_PROFILER_ENABLED` 挂到 **PROFILE tier**（`CHAOS_IL2CPP_CONFIG_PROFILE` 时自动 =1），使单个 profile 构建同时产出 opcode 直方图 + per-method RDTSC/GC 锚；debug/ship tier 仍全编译出（热路径零开销）。
  3. `fast_dispatch_core.inc` 加 `#include "register_vm_profiler.h"`（preamble，供 execute.inc 解析）。
- **验证（本轮，debug tier）**：`chaos_interpreter` 编译绿；`test_interpreter_profile` 3/3、`cross_tier` 6/6、`osr_state` 15/15、`ir_reg_alloc` 46/46、`smoke` 1/1；`test_driver --layer unit` **OVERALL OK 2249/2277（known=28）**。
- **剩余（待 profile 构建专项）**：`--preset profile`（`artifacts/presets/profile`，`CHAOS_IL2CPP_CONFIG_TIER=profile`）构建后跑 `entry --benchmark / tiering_benchmark`，落盘全 tier ns/op 基线 + opcode 直方图。**所有性能改动（C2/C3）的量化的前置。**

---

## 三、已确认的既有 bug（非本次引入，单独立项）

1. **`Interpreter_Stress.MixedOpcode` 单测崩溃（EXIT=3）**：~~单帧 FastFrame 50000 次循环~~ **✅ 已修（commit `817ba761a`）**：根因=复用单 `FastFrame` 跨 50000 迭代，帧内 eval-stack `sp` 残留进下次调用 → 非平衡 opcode 序列(BrFalse 回边)逐轮漂移 → OOB → 硬件崩溃。修复=每轮新建帧（对齐 StressWorker/LoopBackedge）。验证 MixedOpcode 5 万次 0 错 EXIT=0。
2. **`SimdFma`(110) 表越界空指针**：上轮 `4b2900225` 已补 110-127 为 `kOp_Unsupported`，**已闭合**。
3. **寄存器层 `Reg_LdElem`/`Reg_StElem` null/越界原静默**：上轮 `4b2900225` 已对齐抛异常，**已闭合**。

### 三.1（本轮 review，2026-08-13 round-3 + round-4）新发现的真缺口 — 全部 P1/P2 已修

**✅ 已修（5 commit，全部回归绿）**：
1. **RegisterVM div/rem 无守卫（P1，`f6e3a3335`）**：`Reg_Div/Rem/DivUn/RemUn` 对 `r==0`/`INT32_MIN/-1` 无守卫 → 硬件 #DE/SIGFPE + UB。已镜像 FastExecute 守卫(`r==0`→fault; INT32_MIN/-1→wrap)；新增 `RegisterExecuteDivByZeroFaultsNoCrash` + `RegisterExecuteDivInt32MinMinus1Wraps` 探针。
2. **RegisterFile reg/set_reg 无越界防护（P1，`f6e3a3335`）**：`reg/reg_tag/set_reg/reg_f32/f64` 无 `kTotalRegisters` 边界，≥88 局部方法 OOB 腐化相邻帧。加 `idx>=96` 越界返 0/no-op 地板。
3. **NoChk/barrier opcode 103-106 层不一致（P1，`b13116729`）**：allocator 发射 `StFldBarrier/LdElemNoChk/StElemNoChk/LdElemANoChk`，RegisterVM dispatch 表只到 99→`op>99` fault；FastExecute 有。已在 AllocateRegisters 发射前改写为 checked ≤99 等价(103→StFld, 104/106→LdElem, 105→StElem)；新增 `RewritesNoChkAndBarrierOpcodes` 测试。
4. **Reg_StInd/StObj 缺 GC 写屏障（P1，`b9653cb5d`）**：裸 `*ptr=val` 无 SATB/card/BarrierCriticalSection。镜像 Handle_StInd/StObj：`chaos_is_gc_pointer` 守卫 + `BgcSatbPreWriteBarrier(&slot)` + 临界区 store→`dirty_card`；新增 `StIndStackLocalRoundTrips` 证明非 GC plain-store 未破坏。
5. **Reg_LdArgA/LdLocA null stub（P2，`1ffa775a3`）**：`Reg_LdArgA` 现返回 `&args[idx]` 真地址（镜像 Handle_LdArgA）；`LdLocA`（局部在 register file 不可寻址）→ `AllocateRegisters` 预扫拒该 method（空返回）路由 FastExecute（其 `Handle_LdLocA` 正确）→ 更新原 LdLocA 单测为新拒绝语义；新增 `LdArgAReturnsRealAddress` + `LdLocARejectsToFastExecute`。

**⏳ 剩（非 P1/P2 推理/done 边界）**：
- **P2② D1 per-method 锚无 in-tree caller**：`DumpProfilerToFile` 只在 Scriban(TestProject.RuntimeEntry.cpp.scriban，codegen 不在原生树) 调；`tiering_benchmark` 只调 `DumpFastExecuteOpcodeHistogram`。需在 tiering_benchmark 加 PROFILE/VM_PROFILER 宏 dump（待 profile 构建专项一起）。
- **P3 测试缺口**：>8arg call / 高本地 ≥96 / stobj 写 GC-ptr 端到端（div/NoChk/LdArgA 已有 Probe 补）。跨层 D2 门禁已含 div/INT32_MIN。

---

## 四、环境坑（必须注意）

1. **共享工作区被并行线（jit & gc）实时编辑**：session 期间 `jit_deopt.cpp`/`jit_helpers.cpp`/`git_osr_test.cpp`（jit 线）、`gc_card_table`/`thread_state`/`gc_old_gen`（gc 线, A2b demo）被并发改。**commit 前务必 `git status` 确认只 stage 自己的文件**，勿 `git add -A` 打包并行线的工作。当前工作区干净（`git status` 空）。
2. **`fast_dispatch_*.inc` 是"源"非生成**：`fast_dispatch.cpp` 只有 18 行聚合器 `#include` inc 文件。头部"Auto-extracted from fast_dispatch.cpp"注释**过时**，直接 edit inc 是对的（不会被覆盖）。但 `fast_dispatch_seh.inc` 头部真说"Do NOT edit, run split_fast_dispatch.py"——**只有 seh.inc 是生成需注意**。
3. **禁 `git stash`**（项目规则）：切换验证用**临时 worktree** 或显式提交。MixedOpcode 验证用的 `/tmp/chaos-baseline` worktree 已用 `git worktree remove --force` 清理。
4. **构建命令**：`cd build/native && cmake --build . --target <target>`。增量编译若遇 MSB6003 tlog 冲突（git checkout 切换文件后），用 `-- /t:Rebuild` 清缓存。workspace cwd 常被 git 命令切到仓库根，build 前确认 cwd。
5. **测试运行**：解释器单测在 `build/native/tests/unit/runtime-native/runtime-core/interpreter/Debug/*.exe`。全量门禁：`python tests/runner/test_driver.py --layer unit`（OVERALL OK 基线 2249/2277）。

---

## 五、推荐新 session 执行顺序

1. **D1（性能量化前置，代码已落地 `b4d1ec85b` + 回归绿）**：VmProfileScope 已接入 RegisterExecute 热路径 + VM_PROFILER 挂 PROFILE tier。**剩余**：profile 构建专项（`--preset profile`）跑 `entry --benchmark` 落盘 ns/op 锚 + opcode 直方图 → 量化 C2/C3。
2. **C4（call>8 已 done `806a4f107`；剩 ArrayStorage/SmallFieldArray，低风险独立）**：热路径堆分配消除，用 D1 锚量化。**C3 QuickJIT 阈值评估未做**，用 D1 锚量化短生命周期方法。
3. **B4（已闭合）**：文档确认 + 统一注释，无代码。
4. **C2（架构）**：先 4A（扁平存器文件+内联+验证 deopt/OSR reg 恢复），4B 作后续。
5. **B5（高并发，需专门验证）**：分离 already-parked vs hijacked-mid-run，需要 GC-stress 环境。**建议独立 session 深做**。
6. 全程可跑 `test_cross_tier_consistency`（D2 门禁）回归，防层间漂移。

---

## 六、关联文档链
- 主计划（含每项方案/测试策略）：`docs/dev/in-progress/interpreter-optimization/interpreter-fix-plan-2026-08-13.md`
- 4 维度发现：`docs/dev/assessments/interpreter-deep-analysis-2026-08-13.md`
- 现行态复核（上轮已修）：`docs/dev/assessments/interpreter-review-current-2026-08-13.md`
- 峰平谷总评：`docs/dev/assessments/interpreter-capability-peak-plateau-valley-2026-08-13.md`
- memory：`interpreter-review-current-findings`（复核洞：chaos_is_gc_pointer 盲点/CodegenStSFld 等，均已修已记录）
