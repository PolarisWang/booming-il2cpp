# Branch Summary — feat/jit-allocation-quality-diagnostics

> **日期**：2026-08-11
> **分支**：`feat/jit-allocation-quality-diagnostics`
> **用途**：JIT 图着色分配质量量化基线 + 修复（FPR 着色打通 + write-through 跨call细化）
> **关联**：[M1] 分配质量量化工具、[M2] FPR 着色、[M3] write-through 细化 —— 均完成并提交

---

## 1. 分支内容：两个 Workstream 混合

本分支相对 `main` 有 6 个 commit，实际是**并行两线共用一个分支**：

### 我的 JIT 主线（3 commit，本分支的主题）
| Commit | 内容 | 验证 |
|---|---|---|
| `22161fbbb` | `CHAOS_IL2CPP_CODEGEN_STATS` 分配质量诊断工具（自包含头 `jit_codegen_stats.h`，env 门控，量化 reg-vs-stack + per-opcode 栈往返 ranking） | gate off 零开销；gate on 出 JSON |
| `a23ff863f` | **FPR(XMM) 图着色打通**（allocator 并行 FPR liveness + 保守活区间 + 传递闭包；`has_graph_coloring_` 任一 GPR/FPR 色判定） | FPR 着色 reg=0→2；全 254 测试绿 |
| `86f7491f1` | **write-through 跨call细化**（`cross_call_mask_` per-vreg，替代方法级 `has_caller_clobber_`；写穿 + post-call reload 同步 gate） | writethrough 16134→14595(-9.5%)，store.reg 766→2305(3x)；全 254 测试绿 |

### GC 平行线（3 commit，另一 session 在本分支提交）
`e4d765d79`/`35285c431`/`a66b6139a` — gc-concurrency-fix-dedicated（region-gen all-chunk marking、static-root mark scan、atomic card bundle、VerifyHeap）。**与 JIT 主线无文件冲突**（不同 `src/native/runtime-core/gc/` vs `src/native/jit/`）。

---

## 2. 关键成果与数据

**诊断基线**（`test_jit_native`, 1090 方法, kFull 彩色路径）：
- GPR load **97.5% 寄存器命中**（24850 reg / 637 stack）
- GPR store 原先 92% 是 write-through（16134），细化后 16134→14595
- FPR 原先 100% 走栈（着色死路径）→ 修复后彩色

**根因发现**（两处，均已修）：
1. **FPR 着色死路径**：浮点 vreg 编号 64+，但 allocator 的 FPR pass 从 GPR-only 的 64-bit `live_in` 读 bit≥64 → 永远空 → `fpr_color` 恒 0xFF。加并行 FPR liveness 修复。
2. **write-through 方法级二值**：`has_caller_clobber_` 整方法一个 flag，无法区分 vreg 是否真跨 call 存活。加 per-vreg `cross_call_mask_`（基于 liveness `live_in_`）。

**RDI ABI 调研（诚实结论）**：看似 callee-saved 错标，实测改 e callee-saved 致 `OsrRepromotion` 挂起——RDI 是本 codegen frame-init REP STOSQ 的保留 scratch。**保留原设计**，补注释防误改。

---

## 3. 验证状态

- **全 15 套件 254 项测试全绿**（test_jit_native 69 / il_smoke 19 / abi 31 / gc_slot_map 16 / seh_handler_internal 38 / ...）
- 核心改动仅在 `src/native/jit/`，未触翻译/运行时其他域
- 每 commit 带 `root_cause`/`fix_strategy`/`regression_check` 三段元数据，通过全部 pre-commit 门禁

---

## 4. 下一步候选（重启后接续）

按 `jit-regalloc-a2-continuation-roadmap.md` §5 启动表 + 本分支已验证成果，剩余可推进：

| 优先级 | 任务 | 说明 |
|---|---|---|
| 🔴 高 | **扩持久寄存器预算**（当前 x64 仅 R12-R15 4 个在 prologue save/restore）| 提升图着色可用寄存器数，减少 spill。需同步 unwind/SEH/GC slot |
| 🔴 高 | **GC 读寄存器省写穿**（CoreCLR `GetRegisterSlot` 读物理寄存器）| 需 GcInfo 支持 per-safepoint 寄存器根；可进一步消除 GC-ref 写穿 |
| 🟡 中 | test_jit_native 浮点常量被 optimizer 折叠 → fpr total=0（harness 伪影）| 补非折叠 float 测试验证 FPR 真实收益 |
| 🟡 中 | 分支清理：GC 平行线提交是否要留在本分支 | 按需 rebase/分离 |

**重启入口**：读本文件 + `jit-regalloc-a2/TASK-HANDOFF.md` + `jit-regalloc-a2-continuation-roadmap.md` + memory `jit-graphcoloring-allocation-quality-report.md`、`jit-writethrough-elimination-coreclr-analysis.md`。确认分支 + 工作区，从 §4 表中选高优项启动。
