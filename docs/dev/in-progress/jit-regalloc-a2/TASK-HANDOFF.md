# JIT A-2 任务交接 / 续跑文档（session 重启入口）

> **用途**：本会话（2026-08-10）已耗尽上下文。重启 Claude Code 后，**用本文档 + PLAN.md + memory 续跑**。
> **关联**：`docs/dev/in-progress/jit-regalloc-a2/PLAN.md`（执行蓝本）、`roadmap-v1-01.md`（阶段结构）
> **memory**：`jit-verification-deepdive-results.md`

---

## 0. 重启会话第一步（固定动作）

1. 会话启动时加载 `dev-il2cpp` skill。
2. 读本文档（以下内容）+ `PLAN.md` + `jit-regalloc-a2-continuation-roadmap.md`。
3. 确认当前分支 `fix/sdk-build-path-authority-converge`、工作区状态。
4. 第一动作 = **在本会话已提交成果的基线上继续**（见 §1 已完成）。

---

## 1. 已完成任务（10 个 commit，均已提交）

| Commit | 任务 | 内容 / 验证 |
|---|---|---|
| `14532ebee` | **T0.1 + T1.1** | §0 ABI thunk(`t4_jit_call.asm` RBX/RSI 重绑) + tree_builder Dup/Box/Unbox 别名 + orphan 测试恢复。**§1.1 kQuick 返回 0 已随此消除**（pgo 6/6） |
| `25adce74a` | §1.2 | test_jit_bench 改合法 vreg loop（原 MakeAccumulateMethod 高 vreg 是测试构造 bug，非 gtest 边界） |
| `2b845d36e` | §4.1 | test_jit_unwind 断言对齐 Win64（原断言过时 UNWIND_CODE 布局） |
| `843f415bc` | **J2-A(R2a)** | Fuzz mismatch-recompile(`cfg_no_cache`) 禁 optimizer → **Fuzz 从崩溃(栈溢出)→完整运行** |
| `934569aba` | **J2-A(R2b)** | Fuzz landing Ret 用 entry-defined vreg → **mismatch 8→2** |
| `35ebe607e` | **T3.1** | **`EmitShift` 图着色加载顺序碰撞修复** → **Fuzz 2 mismatch→0，15/15 绿基线达成** |
| `f6464b23b` | **并行 flaky** | JIT 共用全局状态测试加 `RESOURCE_LOCK jit`（mode/bench/native 等 8 个）→ `ctest -j8` 稳定 |
| `a4e5fdb37` | **T2.1 A1** | **caller-colored write-through 消除 + register-residency 值守测试**（见 §2b） |
| `cf8e0c755` | **T2.2 A/B/C1/D** | **精确 per-safepoint GC root maps**（见 §2c） |

**当前测试状态**：`test_jit_native` = **69 PASS**；`jit_gc_slot_map_test` = **16 PASS（含 4 个新 T2.2 精确扫描用例）**；jit ctest **15/15**；unit/codegen + driver PASS（unit/snapshot 2-fail = managed Generator 基线漂移，与 JIT/GC 无关）。

---

## 2. 已完成任务（T3.1 已完结 → 见 §2b 根因）

### T3.1 ✅ run514 剩余 2 mismatch — 已根因断案并修复（2026-08-10）
- **根因**：`EmitShift`(jit_engine.cpp:970) 可变位移路径 `LoadGpr(op_reg, src1)` 先于 `LoadGpr(kScratchB, src2)` 执行。当 `color(dst)==color(src2)`（in-place `[12] Shl r0=r1<<r0`，dst 与 src2 同 vreg 故同色），先把 src1 载入 op_reg 破坏 src2 规范化寄存器 → CL 取到 src1 低位而非真计数。
- **具体**：`LoadGpr(RCX, src2=r0)` 因 `color(r0)==op_reg==r9` 发出 `mov rcx, r9`，但 r9 已被前一步载入 src1(=35890)→ cl=`0x32`→mask18。`35890<<18=0x30C80000`，`[13] Not` → `~0x30C80000=0xCF37FFFF`。RE=`0xffffffff`。
- **修复**：`EmitShift` 加与 `EmitGprArithmetic`(jit_engine.cpp:820-825) 同构碰撞保护——`color(src2)==op_reg` 时先 `LoadGpr(kScratchB, src2)` 再 `LoadGpr(op_reg, src1)`。x86+aarch64 双路同步。（`src/native/jit/jit_engine.cpp` 单文件，+20/-3）
- **RegisterExecute 参考语义**：实为 `src/native/interpreter/ir_reg_alloc.cpp`（`register_vm.h` 注释指向它），非 interpreter_vm.cpp。int32 标量，LdcI4 零扩展，shl/Shr/ShrUn 用 int32、移位计数=src2、掩码 `&0x1F`。
- **验证**：test_jit_native 68/68；jit ctest 15/15；unit OVERALL OK。memory `[[jit-fuzz-mismatch-shift-collision]]`。

### 进行中（后续轮次入口）
### T3.1 — 剩余 run514 的 2 个 mismatch（✅ 已完结，见上）
### ✅ 本会话（2026-08-10 第2轮）已完结：T3.1 + T2.1(A1) + 并行flaky + T3.2（见 commit `35ebe607e`/`a4e5fdb37`/`f6464b23b`）

---

## 2c. T2.1 A1 — caller-colored write-through 消除（已完结 2026-08-10）
- **现状**：图着色已是默认路径（`enable_register_caching=true`），caller-colored vregs（R8-R11/RDI）在非 call-free 方法 `StoreGpr/StoreFpr` 每 op 写穿栈。
- **修复**（`a4e5fdb37`）：加 `has_caller_clobber_`，pre-scan 白名单（纯 Int/FP 标量 op：arith/bitwise/shift/cmp/convert + Dup/Pop/Ret/Ldc const）→ 方法整体标量且 call-free 时跳过 caller write-through → value 纯寄存器常驻。任何非白名单 op（call/alloc/field/elem/box/cast/Ovf-deopt/safepoint）保持保守写穿（GC 扫描/写屏障/deopt 需要栈槽）。
- **测试**（`Test_RegisterResidency`，69 号）：call-free 运算链 **0 栈写穿**（原 4）、结果正确；对象 op 测试（LdFld_StFld/LdElem_StElem/StElemFix）靠保守门控保持绿。
- **验证**：test_jit_native 69/69；jit ctest 15/15；unit/codegen+driver PASS。
- **残余**：非标量方法的 caller 写穿仍存在（GC/deopt 安全需要）→ 真正消除需 §2.2 liveness + §2.3 deopt-contract（精确 spill-in，非总是 spill-all）。**T2.2/T2.3 是下一步核心**。

## 2d. T2.2 — 精确 per-safepoint GC root maps（已完结 2026-08-10, commit `cf8e0c755`）
调研文档：`docs/dev/in-progress/jit-regalloc-a2/T2.2-precise-gc-rootmaps-task.md`
- **A 精确路径**：`nm->gc_points_`（liveness 过滤的 per-safepoint 精确栈槽）原本是死代码（GC 扫描不读）。新增 `GcPointMapV0`（`GcSafepointV0` 数组，含 live 栈槽 + live volatile-reg 编码），`Generate()` 序列化；`GcScanPreciseSafepoint` 按 return offset 二分 `FindSafepoint` 只扫该点 live 根；thread_state.cpp JIT 帧扫描优先用它，保留 union `GcSlotMapV0` 为保守 floor。
- **B 寄存器根**：`GcPoint.live_reg_mask`（live ObjectRef vreg 且 color 到 volatile R0-R11）在 `RecordGcPoint` 计算，序列化为 reg 编码；新增可测的 `GcScanSafepointRegisterRoots(gpr_file, safepoint)` decode。**注**：当前 write-through 模型下调用点这些 refs 已 spill 到栈，reg 根是 data-path 完备 + 前瞻（消 call-crossing 写穿后有用）；Windows GC 悬挂的寄存器窗捕获是后续项。
- **C1 offset 扩位**：slot 编码 offset 12-bit(4096)→31-bit（bit31 kind），去 `static_assert(kFrameSize<=4096)`，`RecordGcPoint` 不再丢 >4095 offset。
- **D tracked/untracked**：per-safepoint 扫描=tracked 减 over-retention；union map=untracked 安全 floor；新增 `ScanPreciseSubsetOfUnion` 保证 precise⊆union 永不欠保留。
- **测试**：`jit_gc_slot_map_test` 16/16（新增 ScanPreciseSafepointPerOffset / ScanPreciseLargeOffsetBeyond4096 / ScanSafepointRegisterRoots / ScanPreciseSubsetOfUnion）。test_jit_native 69/69；jit ctest 15/15。
- **残余/后续**：B 的运行时寄存器窗捕获（GC 悬挂时取 mutator volatile regs，Windows 需 CONTEXT 捕获）+ §2.3 spill/deopt offset 契约统一。

## 3. 未完成任务清单（依赖顺序）

### JIT 主线
| 任务 | 依赖 | 说明 |
|---|---|---|
| ~~**T3.1** run514~~ | 无 | ✅ 已完结：EmitShift 碰撞修复 → 15/15 绿基线 |
| ~~**T2.1** (§2.1) 图着色默认化 A1~~ | T3.1 | ✅ 已完结：scalar-only call-free 方法 caller write-through 消除；残余需 T2.2/2.3 liveness |
| ~~**T2.2** (§2.2) 精确 GC slot maps~~ | T2.1 | ✅ 已完结（commit `cf8e0c755`）：per-safepoint 精确扫描 + 寄存器根 data-path + offset 扩位 + tracked/untracked floor。残余：运行时寄存器窗捕获 + §2.3 deopt 契约 |
| **T2.3** (§2.3) spill 契约 + 热更一致性 | T2.2 | GcSlotMapV0 序列化 + deopt ReadSpillSlot 同 offset + 热更重编译一致（registry 已 code→map，机制已具）|
| **T2.4** monolith 拆分 | 绿基线 | 6229 行→8 模块（memory jit-engine-modularization）|

### 卫生
- ~~**T3.2** L11 stale 清理~~：✅ 已完结 —— 删 `testing/`(6.3G, 230 vcxproj) + `native-runtime-core-test/`(1.7G)
- **T3.3** 孤儿测试检查器：`check_workarounds.py`(T3.3) + API 引用检查
- ~~**并行 flaky**~~：✅ 已完结 —— jit 共用全局状态测试加 `RESOURCE_LOCK jit`（8 个），`ctest -j8` 5/5 稳定

### 跨域能力线（D6: 并行，Agent 注入 expert）
- **T4.1** benchmark harness 可信度（ns/alloc=0 伪影）→ 供 J2-B 性能验收证据
- **T4.2** 消除中间层 dispatch（`src/managed/Chaos.IL2CPP.Generator/`）→ 依赖 T4.1
- **T4.3** 翻译语义 GVM/Span/vararg/tail → 翻译域 expert
- **T4.4** 数据治理（CoreLib 覆盖 + aggregate 口径）

---

## 4. 关键根因/结论速查（供续跑）

- **Fuzz 崩溃根因**：cfg_no_cache 未设 optimizer→树高 vreg(kBaseVReg=64, jit_linearizer.h:34) → emit 栈溢出。
- **Vreg 64+** = FPR 区 / tree 临时（kGprCount=64, `1ULL<<vreg` 掩码只到 63 → §2.2 精确 GC 需扩）。
- **GC 基址**：JIT 记 SP-rel offset(GprOff)，GC 扫描 `frame_ptr=t4_rbp-rbpoff`(thread_state.cpp:675)=SP → **一致，平台 OK**。
- **GcSlotMapV0** 12bit(4096) 上限 + 固定 slot_map_entries_ → §2.3 需扩/去重（对照 CoreCLR StackSlotIdKey）。

---

## 5. ⚠️ 用户并行 GC 线（勿干扰）

- 同一分支有 GC 并行 commit（align CoreCLR: `e4dae1f97` young GC, `6659812d4` GC_STRESS 移除, gen1/thread_state 等）。
- `src/native/runtime-core/thread_state.cpp`、`gc_*` 相关未提交改动 = **GC 线的，勿动**。
- JIT 只改 `src/native/jit/` + `tests/unit/runtime-native/jit/`。

## 6. 重启后的推荐第一动作
1. 确认基线：跑 `ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -L jit -LE "benchmark|stress|soak"`（应 **15/15**，已达成）。
2. **T3.1 / T2.1(A1) / 并行flaky / T3.2 全部已完结**。下一步核心 = **T2.2 §2.2 精确 GC slot maps**（将非标量方法的 caller write-through 从"全 spill-all"收敛为 liveness 精确 spill；A1 的残余写穿正是 T2.2 的输入）。T2.4 monolith 拆分可穿插（绿基线护栏已就位，`[[jit-engine-modularization]]`）。

> 已完结 3 个候选 + T3.1：`35ebe607e`(T3.1)、`a4e5fdb37`(A1)、`f6464b23b`(flaky)、T3.2 删 8G stale。unit/snapshot 2-fail 为 managed Generator 基线漂移，与本 JIT 线无关。

> ⚠️ 提交前注意：T3.1 修复 commit 未落库前不要覆盖此表。T3.1 commit hash 以 git log 为准填回 §1 表。

