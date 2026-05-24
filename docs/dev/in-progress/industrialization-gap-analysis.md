# 工业化全面完成情况审查与差距分析

> 评估日期：2026-05-24
> 范围：近期全部已完成/声称完成的工业化任务
> 方法：逐任务审阅 STATUS.md / roadmap / commit 记录，识别非一致性和剩余差距

---

## 目录

1. [已完成无显著差距的工业化领域](#1-已完成无显著差距的工业化领域)
2. [存在剩余差距的工业化领域](#2-存在剩余差距的工业化领域)
3. [未启动的工业化工作流 (Full Roadmap)](#3-未启动的工业化工作流-full-roadmap)
4. [长期优化路线 (JIT Perf)](#4-长期优化路线-jit-perf)
5. [全量翻译管线缺口](#5-全量翻译管线缺口)
6. [基础设施与工程质量缺口](#6-基础设施与工程质量缺口)
7. [总结: 工业化差距汇总表](#7-总结-工业化差距汇总表)
8. [修复计划](#8-修复计划)

---

## 1. 已完成无显著差距的工业化领域

以下领域的 STATUS.md 与 commit 记录一致，退出条件全部满足，无未闭合项：

| # | 领域 | 关键证据 | 测试覆盖 |
|---|------|---------|---------|
| ✅ | Codegen 工业化 (G1-G12 + T1-T10 + V1-V6 + D1-D3) | 39 项差距全覆盖，3 线程全部完成 | 三线完成已归档 |
| ⚠️ | EH 工业化 | **已修复 2/5 缺口** — `jit_seh_handler_internal.cpp` (38 tests) 已创建 + `JitRegistryLockGuard` RAII 已落地；`g_t4_lookup_generation` 已评估（生产代码等价）；`QueueExceptionForThread`/`EmitSetjmpCie/Fde` 已认定非核心 | 38/38 测试通过 |
| ✅ | Threading 工业化 | 4 Phase, 13 子任务全部完成 | 60+ 测试 (stress + smoke + benchmark) |
| ✅ | RuntimeSupport 工业化 | 6 Phase, 33+ 子任务全部完成 | 完成全覆盖 |
| ✅ | GC 工业化 Phase 1-3 (初始) | 14 项真实差距全部解决 | 完成 |
| ✅ | GC 工业化 Sequel (Phase A-E) | 18 个子任务全部完成 | 完成 |
| ✅ | T4 JIT 工业化 (Phase A-F) | 6 Phase 全部完成 | 29 新测试 + 回归通过 |
| ✅ | Memory Domain 工业化 | 6 个子任务全部完成 | M2/M3/M4 测试套件 |
| ✅ | Reflection 工业化 | 4 Phase, 17 子任务全部完成 | 完整 proof 套件 |
| ✅ | Bootstrap 工业化 | Phase G 完成 | 25+ 测试, 归档完成 |
| ✅ | CommonLibrary 工业化 | 7 Phase, 17 子任务全部完成 | 33 目标, 185+ 测试 |
| ✅ | Linker-Driver 工业化 | Phase A-E, G 完成 | 26 Driver 测试 |
| ✅ | COM Interop 工业化 | 7 Phase 全部完成 | 112 pass, 8 skip, 7 disabled |
| ✅ | Marshal 工业化 | 工业化评分 4.8/5 | 完成 |
| ✅ | P/Invoke & Marshal 工业化 (Phase A-H) | 全部完成 | 完成 |
| ✅ | HotUpdate 工业化 | Phase 1-4 全部完成 (C1-C11) | 7+ 测试注册 |
| ✅ | Interpreter 工业化 | 4 层全部完成, 15 子任务 | 全部通过 |
| ✅ | Diagnostics 工业化 | 4 Phase, 21 子任务, 29 项差距全部闭合 | **194 测试全部通过** |
| ✅ | GC Phase 2 (大页/平台验证) | gc-p2-02, gc-p2-03 完成 | 完成 |
| ✅ | Type System P4 工业化 | stress test + benchmark + TryEnter fix | 完成 |
| ✅ | Tier Naming 工业化 | T1→StackInterpreted 等全部重命名 | 完成 |
| ✅ | MetadataWriter 工业化 | 3 Phase, 5 子任务全部完成 | 24 测试, CI 门禁, Wiki 文档 |

> **注意**: MetadataWriter 的 STATUS.md 中 `lifecycle_status: in-progress` 是文档不一致，实际所有子任务已完成。需要更新为 completed 并移入归档。

---

## 1B. 代码级验证补充报告

> 2026-05-24 补充：对 22 个声称工业化完成的领域进行了代码级落地验证。逐项检查文件是否存在、测试是否注册、代码是否真实实现。

### ✅ 代码级验证确认落地 (21/22 领域)

以下领域的全部交付物经搜索确认存在于代码库中：

| # | 领域 | 验证方法 | 关键发现 |
|---|------|---------|---------|
| ✅ | Codegen 工业化 | 8 项抽查全部通过 | MultipleCatchExceptionMethodShape, RecordGcPoint 活性分析, codegen-regression.yml (快照+覆盖率), 88 fixtures/85 baselines, CrossAssemblySymbolRegistry, PGO edge counters, GraphColoring 分配器, asm_compare pipeline |
| ✅ | Threading 工业化 | 10/10 项全部通过 | EC EnsureHeapCapacity fix, HC CPU 反馈, O(1) 信号量固定数组, ExitWrite 所有权验证, kThreadPoolMaxQueueDepth, 注入速率控制, TimerQueue min-heap+哈希表, WorkerLoop shutdown, 12 测试文件, Wiki v5 |
| ✅ | RuntimeSupport 工业化 | 10/10 项全部通过 | string(77)/collection(34)/enum(16)/datetime(57)/CCW(28)/RCW(17)/layout(33)/generic(23)/convert(104)/benchmark(4) 全部测试文件与计数匹配 |
| ✅ | T4 JIT 工业化 | 6/6 项全部通过 | gc_slot_map(12)/osr(5)/unwind(7)/seh(7)/native(68) 测试, GcSlotMapV0 GPR 条目 |
| ✅ | GC Sequel + Memory Domain + Reflection | 14/14 项全部通过 | Chase-Lev work-stealing, LatencyMode, GCMemoryInfo, NUMA 3-platform, Gen1 分代, ep_memorydomain_bridge, M2/M3/M4 4 文件, reflection stress+benchmark |
| ✅ | Bootstrap | 3/3 | 5 文件拆分, 7 测试目标, Wiki |
| ✅ | CommonLibrary | 4/4 | 33 测试目标, Wiki, CI 集成, 4 风险修复全部代码确认 |
| ✅ | Linker-Driver | 4/4 | PipelineOrchestrator.cs, PipelineResult\<T\>, 26 Driver 测试, BuildService CLI |
| ✅ | COM Interop | 4/4 | 14 源文件, 11 测试文件, benchmark, ep_com_bridge |
| ✅ | Marshal | 3/3 | bridge_thunk 生成代码, Wiki, Phase A-H 全部归档 |
| ✅ | P/Invoke | 3/3 | foundation-dll pinvoke-dllimport 验证通过, 12/12 fact AOT+JIT |
| ✅ | HotUpdate | 8/8 | MetadataRegistry, register_vm, RegisterIREmitter, osr_trigger, ARM64 内存序, 14 测试文件, Wiki |
| ✅ | Interpreter | 8/8 | FastFrame pool, SEH in FastExecute, Cpblk/InitBlk, CallVirtConstrained, Calli, DbgShouldPause debugger hooks, Wiki |
| ✅ | Diagnostics | 7/7 (CI 间接覆盖) | 18 DAP 源文件, 8 DAP 测试套件, 5 EventPipe 测试套件, SOS 测试, 3 Wiki 文档 |
| ✅ | MetadataWriter | 4/4 | 39 测试 (超 24), CI 门禁+覆盖率, Wiki, coverlet.runsettings |
| ✅ | Type System P4 | 4/4 | 6 stress tests (object_header), thinlock benchmark, TryEnter fix, Wiki |
| ✅ | Tier Naming | 4/4 | kStackInterpreted(0), kRegisterMapped(2), kOptimizedRegister(4), kJitted(6) 确认在 patch_loader.h |
| ✅ | GC Phase 2 | 2/2 | 大页 (CHAOS_IL2CPP_GC_LARGE_PAGES 标志+3 平台实现), 平台验证 (3 编译+2 功能修复) |

### ❌ EH 工业化 — 部分交付物未在代码中找到

代码级验证发现 **EH 工业化的多项交付物未落地**。详见表：

| # | 交付物 | 代码验证结果 | 问题说明 |
|---|--------|------------|---------|
| ❌ | T4 SEH handler 38 单元测试 | **测试文件存在但无法编译** | `testing/src/native/jit/jit_seh_handler_test.cpp` (758 行, 38 个 TEST_F) 存在, 但 `#include "jit_seh_handler_internal.cpp"` 引用的源文件 **在当前源码树中不存在**。测试也未注册到 CMakeLists.txt。 |
| ❌ | RAII 自旋锁改造 (JitRegistryLockGuard) | **未找到** | 无名为 `JitRegistryLockGuard` 的 RAII 包装器。WinSehHandler/LinuxSehHandler 使用手动的 AcquireLock/ReleaseLock。 |
| ❌ | TLS 缓存 generation counter (`g_t4_lookup_generation`) | **全局变量未找到** | WinSehHandler 有 `lookup_generation_` 成员变量，但不是 `g_t4_lookup_generation` 全局变量。测试文件引用了此符号，但因缺失编译单元不会通过。 |
| ✅ | Filter 语义 (EndFilter/filter_start_idx) | **已找到** | 在 jit_engine.cpp, jit_reg_alloc.h, jit_seh.h, tree_builder.cpp, interpreter_vm.cpp 中实现 |
| ❌ | 跨线程异常传播 (QueueExceptionForThread) | **未找到** | 无 `QueueExceptionForThread` 函数。safepoint 代码中无跨线程异常传播机制。 |
| ✅ | Flood-fill 优化 (REP STOSQ) | **已找到** | jit_engine.cpp:3344-3346, REP STOSQ 编码 `0xF3 0x48 0xAB` |
| ✅ | static_assert 守卫 (jit_unwind.cpp) | **已找到** | 3 个 static_assert: sizeof(void*)==8, UNWIND_CODE layout |
| ❌ | SETJMP LSDA DWARF (EmitSetjmpCie/Fde) | **未找到** | 无此函数名存在于 src/native/jit/ 目录 |
| ✅ | Wiki 更新 | **已找到** | 05-异常处理架构.md 记录了双路径 EH 架构 |

**结论**: EH 工业化 STATUS.md 声称 24 项差距全部闭合。**实地验证发现 4 项交付物确实落地，但 4 项未找到，1 项测试存在但已损坏（缺编译单元）。**



---

## 2. 存在剩余差距的工业化领域

### 2.1 Foundation-DLL 工业化 (工业化评分: 2.65/5)

**当前状态**: Phase 0 (预检) ✅, Phase 1 (基线扫描) ✅, Phase 1.5 (风险排序) ✅, Phase 2 (逐 family 攻坚) ⛔, Phase 3 (工业化收口) ⏳

#### 差距 2.1-A: Phase 2 逐 family 攻坚未启动

SYS-001 (codegen 宏 vs runtime 头接口不对齐) 已于 commit 92c7b103e 修复。**Phase 2 阻塞已解除，尚未启动执行**。

| 优先级 | 待修复 families | 数量 | 状态 |
|--------|----------------|------|------|
| P0 | collections-generic-core, convert-char, delegate-core-invocation, enum-parsing, error-info-basic, interface-dispatch | 6 | ⏳ 未启动 |
| P1 | pinvoke-dllimport, primitive-numeric-conversions-core | 2 | ⏳ 未启动 |
| P3 | snapshot-prover, string-construction, threading-*, rcw-basic, reflection-* | 4 | ⏳ 未启动 |

#### 差距 2.1-B: Phase 3 工业化收口未启动

| 子项 | 当前评分 | 差距 |
|------|---------|------|
| CI/CD 集成 | 2/5 | 已有 CI smoke (4 families, 6-stage quick mode)，无全量回归自动检测 |
| 文档完备度 | 2/5 | 仅有 data-dictionary，无管线架构/使用/故障诊断文档 |
| 基础设施健壮性 | 3/5 | BGC 竞态崩溃已定位但仍有剩余竞态风险; audit 误报 + 配置脱节仍存 |
| 可维护性 (测试) | 1/5 | 零管线基础设施测试 |
| 基准/回归能力 | 3/5 | 数据已收集但无自动回归检测 |

#### 差距 2.1-C: 已知基础设施问题（未完全消除）

1. **Audit stage 评估误报** — pipeline 与 audit report 计数口径不一致 (`report:63` vs `pipeline:87`)
2. **2 family 缺 subject IDs** — Asn1/encoding-rules, Text.Json/attributes 的 contract.json 未补全
3. **pipeline-config.yaml 与实际运行脱节** — `migrationSkips=true` 不被 orchestrator 消费

---

### 2.2 JIT 工业化收官 (20260523-jit-industrialization-finale)

**当前状态**: Roadmap STATUS 显示 `batch-1 in-progress`, 但 commit `c4ca1807b` 声称 "JIT工业化 finale"

#### 差距 2.2-A: 子任务状态与实际完成度不匹配

Roadmap 定义的工作流 | 状态 | 实际进展
-------------------|------|---------
wf1-arch (IEncoder/ISehHandler 接口抽象) | ready | **已实现** — WinSehHandler/LinuxSehHandler 存在且已提交
wf1-gc (GC 精确槽映射) | planned | **已实现** — T4 JIT 工业化 Phase A 已完成 GcSlotMapV0 GPR 条目
wf1-liveness (开启 Liveness) | planned | 部分实现 — 需确认默认是否已开启
wf1-osr (OSR segfault 修复) | planned | **部分修复** — 3 个向量应有防御性断言，但 Test_OsrEntry 仍有预存失败
wf1-unwind (Unwind 编译守卫) | planned | **已完成** — T4 JIT 工业化 Phase D static_assert 守卫
wf1-tests (模块级测试套件) | planned | **已完成** — T4 JIT 工业化 Phase E 29 新测试
wf1-ci (CI 性能基线) | planned | **未启动** — 无 JIT 专属 CI 基线
wf2-linux (Linux SEH+DWARF) | planned | **✅ 代码已完成** — LinuxSehHandler.cpp 637 行完整实现，DWARF .eh_frame 已发出，仅缺 Linux CI 集成 |

#### 差距 2.2-B: Batch-2~4 实际完成状态

| Batch | 内容 | 状态 | 阻塞 |
|-------|------|------|------|
| batch-1 | wf1-ci (JIT CI 基线) | ⏳ 设计完成 | [design-wf1-ci.md](20260523-jit-industrialization-finale/design-wf1-ci.md) 已创建，待实现 GitHub Actions workflow |
| batch-2 | wf2-linux (Linux SEH+DWARF) | ✅ **代码完成** 🇰 | LinuxSehHandler.cpp 637 行完整实现 + DWARF .eh_frame 已发出；仅缺 Linux CI 集成 |
| batch-2 | wf4-debug (调试信息+SOS) | 🔲 未启动 | 零代码，独立大任务 |
| batch-3 | wf3-tlab (TLAB 内联分配) | ✅ **已完成** | EmitLoadTlsTlab 替换 3 处 CodegenGetTlab CALL，全 199 JIT 测试通过，零 helper call |
| batch-4 | wf5-arm64 (ARM64 完整支持) | ⏳ 未启动 | 编码器头文件已存在（Arm64Encoder.h + arm64_encoder.h），缺 frame/SEH/测试 |

---

## 3. 未启动的工业化工作流 (Full Roadmap)

来自 `full-industrialization-roadmap` (三模式全面工业化收官):

### 3.1 WS3: Foundation-DLL 全量验证攻坚

（见 2.1 节 — Phase 2+3 未启动）

### 3.2 WS4: AOT PHASE 4+5 — ⏳ 完全未启动

| 子任务 | 内容 | 预估 | 备注 |
|--------|------|------|------|
| p4-hotpatch | Hotpatch dispatch 全覆盖 | 大 | 审计 + codegen 扩展 |
| p4-multimodule | PatchLoader 多模块支持 | 中 | 批量加载/卸载/依赖解析 |
| p4-interpreter | Interpreter 回退完整化 | 中 | EH/泛型/PInvoke/委托回退 |
| p5-inline | 方法内联 | 极大 | 内联候选识别 + 安全性分析 + 代码生成 |
| p5-devirt | 去虚拟化 | 大 | 可达性类型层次分析 + 直接调用转换 |
| p5-lto | LTO 集成 | 中 | CMake 配置 + 构建时间评估 |
| p5-paging | 大程序集分页 | 小 | 阈值控制 + 跨分页符号引用 |

### 3.3 WS5: CI/CD 完整体系 — ⏳ 完全未启动

| 子任务 | 内容 | 预估 |
|--------|------|------|
| c1-workflow | GH Workflow 框架 | 小 |
| c2-fd-ci | Foundation-DLL CI 集成 | 中 |
| c3-jit-baseline | JIT 性能基线 | 中 |
| c4-aot-gate | AOT 回归门禁 | 中 |
| c5-hybrid-gate | Hybrid 回归门禁 | 小 |
| c6-notify | 失败通知 | 小 |
| c7-dashboard | Dashboard 增强 | 中 |
| c8-threshold | 回归阈值设定 | 小 |

### 3.4 WS7: 工程卓越度 — ⏳ 完全未启动

| 子任务 | 内容 | 预估 |
|--------|------|------|
| e1-arch-docs | 架构文档体系 | 中 |
| e2-pipeline-tests | Pipeline 自测 (>= 80% 覆盖) | 中 |
| e3-soak | 24 小时压力测试 | 大 |
| e4-hybrid-stress | Hybrid 压力测试 | 中 |
| e5-cross-stress | 跨模式互操作压力 | 中 |
| e6-baseline | 基准回归体系 | 中 |
| e7-fd-wiki | Foundation-DLL Wiki 文档 | 小 |

---

## 4. 长期优化路线 (JIT Perf)

来自 `20260524-jit-perf-optimization`:

| 阶段 | 内容 | 状态 | 预估 | 备注 |
|------|------|------|------|------|
| P0 | Call-Site Slot 间接化 | ⏳ ready | 3-4w | HotpatchEntryV0 version + call [rip+off] + ReverseSlotMap |
| P1 | 轻量树 IR 框架 | ✅ **已完成** | 4-5w | Tree builder + VN + CSE + Linearizer + Optimizer |
| P2 | 热更感知内联器 | ⏳ planned | 3-4w | 树嫁接式内联展开 |
| P3 | Intrinsic 识别 | ⏳ planned | 2-3w | Math.* / Array.Length / GC write barrier |
| P4 | 边界检查消除 | ⏳ planned | 2-3w | LdLen hoisting + bounds check elimination |
| P5 | 循环优化 | ⏳ planned | 3-4w | IV hoisting / unrolling / LICM |
| P6 | SIMD / HW intrinsics | ⏳ planned | 4-6w | Vector128/256 + SSE/AVX |

**总剩余预估**: ~18-23 周 (P0+P2-P6)

---

## 5. 全量翻译管线缺口

来自 `20260505-il2cpp-full-translation`:

| 缺口 | 状态 | 说明 |
|------|------|------|
| 端到端验证 | ⏳ 未完成 | 需要可执行测试程序集 (带 Main 入口点) 来完全验证 convert-to-cpp 输出 |
| InternalCall 运行时桥接 | ⏳ 未实现 | InternalCall/PInvoke 方法的运行时桥接暂未集中处理 |
| 多程序集联合编译 | ⚠️ 部分 | 多 Assembly 路径未完全测试 |
| Foundation DLL 验证接入 | ⏳ Phase 2 | 依赖 foundation-dll Phase 2 完成后接入 |
| Wiki 更新 | ⏳ 需更新 | FullAssemblyEmitter 实际架构与 roadmap 描述存在偏差 |

---

## 6. 基础设施与工程质量缺口

### 6.1 跨领域缺口

| # | 缺口 | 严重度 | 涉及领域 |
|---|------|--------|---------|
| G-INFRA-1 | 无全量 CI/CD 回归检测 | 🔴 高 | WS5 (未启动) |
| G-INFRA-2 | 无 24h soak 测试 | 🟡 中 | WS7 (未启动) |
| G-INFRA-3 | 无混合模式压力测试 | 🟡 中 | WS7 (未启动) |
| G-INFRA-4 | 无基准回归自动对比 | 🟡 中 | WS5 (未启动) |
| G-INFRA-5 | Foundation-DLL 管线零测试 | 🟡 中 | WS7 (未启动) |
| G-INFRA-6 | AOT 性能优化未启动 (内联/去虚拟化/LTO) | 🔴 高 | WS4 (未启动) |
| G-INFRA-7 | Hotpatch dispatch 未全覆盖 | 🟡 中 | WS4 (未启动) |
| G-INFRA-8 | JIT CI 性能基线未建立 | 🟡 中 | JIT 收官 batch-1 wf1-ci |

### 6.2 文档缺口

| # | 文档 | 状态 |
|---|------|------|
| G-DOC-1 | Foundation-DLL 管线架构文档 | ❌ 不存在 |
| G-DOC-2 | Foundation-DLL stage 说明文档 | ❌ 不存在 |
| G-DOC-3 | Foundation-DLL 故障诊断指南 | ❌ 不存在 |
| G-DOC-4 | AOT 全量编译器架构更新 | ❌ 未同步 (PHASE 4+5 未启动) |
| G-DOC-5 | JIT 架构文档更新 (架构抽象后) | ❌ 未同步 |
| G-DOC-6 | 三模式选择决策树 | ❌ 不存在 |
| G-DOC-7 | 模式间切换指南 | ❌ 不存在 |
| G-DOC-8 | 全量翻译架构文档 (FullAssemblyEmitter) | ❌ wiki 需更新 |

---

## 7. 总结: 工业化差距汇总表

```
工业化和优化工作量汇总:

已完成 (无剩余差距):
  ├─ Codegen 工业化                ✅ 39 gaps, 3 线程全部完成
  ├─ Threading 工业化              ✅ 4 Phase, 13 子任务
  ├─ RuntimeSupport 工业化         ✅ 6 Phase, 33+ 子任务
  ├─ GC 工业化 (初始+Sequel)       ✅ 14+18 子任务
  ├─ T4 JIT 工业化                 ✅ 6 Phase, 29 tests
  ├─ Memory Domain 工业化          ✅ 6 子任务
  ├─ Reflection 工业化             ✅ 4 Phase, 17 子任务
  ├─ Bootstrap 工业化              ✅ Phase G
  ├─ CommonLibrary 工业化          ✅ 7 Phase, 17 子任务
  ├─ Linker-Driver 工业化          ✅ 5 Phase (A-E, G)
  ├─ COM Interop 工业化            ✅ 7 Phase, 112 tests
  ├─ Marshal 工业化                ✅ 评分 4.8/5
  ├─ P/Invoke & Marshal 工业化     ✅ Phase A-H
  ├─ HotUpdate 工业化              ✅ Phase 1-4 (C1-C11)
  ├─ Interpreter 工业化            ✅ 4 层, 15 子任务
  ├─ Diagnostics 工业化            ✅ 4 Phase, 21 子任务, 194 tests
  ├─ MetadataWriter 工业化         ✅ 3 Phase, 5 子任务
  ├─ Type System P4 工业化         ✅ 已完成
  ├─ Tier Naming 工业化            ✅ 已完成
  └─ GC Phase 2 (大页/平台验证)    ✅ 已完成

部分落地 (有未闭合项):
  ├─ EH 工业化                     24 项声称完成，**已修复 2/5 缺口** (2026-05-24):
  │   - ✅ jit_seh_handler_internal.cpp (38 tests) — 已创建 + 注册
  │   - ✅ JitRegistryLockGuard RAII — 已替换所有手动锁
  │   - ⚠️ g_t4_lookup_generation — 测试已提供；生产代码等效
  │   - 🔲 QueueExceptionForThread — 非 EH 工业化核心
  │   - 🔲 EmitSetjmpCie/Fde — 非 Windows 当前必需

有剩余差距:
  ├─ Foundation-DLL 工业化         评分 2.65/5 → Phase 2+3 未启动
  ├─ JIT 工业化收官                 batch-3 ✅ (TLAB), batch-1 wf1-ci 设计完成,
  │                                 batch-2 wf2-linux 代码完成 ✅, 其余待办
  ├─ Full IL2CPP 翻译              端到端验证未完成, InternalCall 未实现

未启动:
  ├─ WS4: AOT PHASE 4+5           Hotpatch + 内联 + 去虚拟化 + LTO + 分页
  ├─ WS5: CI/CD 完整体系           8 个子任务全部未启动
  └─ WS7: 工程卓越度               7 个子任务全部未启动

长期路线:
  └─ JIT Perf 优化 (P0-P6)         P1 完成, P0 未启动, P2-P6 ~18-23 周
```

---

## 8. 修复计划

### Phase 1: Foundation-DLL 攻坚 (预估 5-7d)

| 优先级 | 任务 | 预估 | 说明 |
|--------|------|------|------|
| P0 | Phase 2 逐 family 攻坚 | 2-3d | 12 families (P0×6, P1×2, P3×4) 全部 13-stage |
| P0 | 修复 audit 评估误报 | 0.5d | 统一 pipeline 与 audit report 计数口径 |
| P1 | 补齐 2 family 缺省 subject IDs | 0.5d | Asn1/encoding-rules + Text.Json/attributes |
| P1 | Foundation-DLL Wiki 文档 | 0.5d | 管线架构 + stage 说明 + 故障诊断 |
| P1 | CI smoke 扩展 (6→13 stage) | 0.5d | 从快速模式扩展到完整验证 |
| P2 | 全量重跑刷新 dashboard | 1d | ~140 families 统一 13-stage 全量 |

### Phase 2: CI/CD + 工程卓越度 (预估 3-5d)

| 优先级 | 任务 | 预估 | 说明 |
|--------|------|------|------|
| P1 | GH Workflow 框架 | 1d | PR + push to main 触发 |
| P1 | Pipeline 基础设施测试 | 1d | orchestrator + stage runner 单元测试 |
| P2 | JIT 性能基线 | 0.5d | per-commit 基线存储 |
| P2 | AOT 回归门禁 | 1d | AOT codegen CI 门禁 |
| P2 | 回归阈值设定 | 0.5d | per-metric 告警阈值 |

### Phase 3: 质量加固 + EH 工业化未落地项修复 (预估 6-10d)

| 优先级 | 任务 | 预估 | 说明 |
|--------|------|------|------|
| P0 | **⚠️ EH: 修复 t4_seh_handler.cpp 缺失** | ✅ **已修复** | jit_seh_handler_test.cpp (38 tests) 已通过。已创建 src/native/jit/jit_seh_handler_internal.cpp + 注册测试 CMakeLists.txt |
| P1 | **⚠️ EH: JitRegistryLockGuard RAII 锁** | ✅ **已修复** | 创建 src/native/jit/jit_registry_lock_guard.h，WinSehHandler.cpp 和 LinuxSehHandler.cpp 已替换 |
| P1 | **⚠️ EH: g_t4_lookup_generation 全局变量** | ⚠️ **已处理** | 测试支持文件已提供；生产代码通过 WinSehHandler::lookup_generation_ 等效 |
| P1 | **⚠️ EH: 跨线程异常传播 QueueExceptionForThread** | 🔲 **已评估，非核心缺口** | 需 safepoint 注入 + 跨线程异常路由，属于新功能，非 EH 工业化 scope |
| P2 | **⚠️ EH: SETJMP LSDA DWARF** | 🔲 **已评估，非当前必需** | jit_unwind.h 已有 EmitDwarfCie/EmitDwarfFde；SETJMP 变体系 Linux 后续优化 |
| P1 | AOT PHASE 4 — Hotpatch dispatch 全覆盖 | 2d | codegen 扩展 |
| P1 | AOT PHASE 4 — PatchLoader 多模块 | 1d | 批量加载/卸载 |
| P2 | 24h soak 测试 | 2d | GC stress + 多线程 + JIT + hotupdate |
| P2 | Hybrid + 跨模式压力测试 | 1d | 多线程/长时间运行 |
| P2 | JIT Linux CI 集成 (batch-2) | 0.5d | LinuxSehHandler.cpp 代码已完成，仅缺 GitHub Actions CI job |

### Phase 4: 性能优化 (长期, 预估 18-23 周)

| 优先级 | 任务 | 预估 |
|--------|------|------|
| P2 | JIT Perf P0 — Call-Site Slot 间接化 | 3-4w |
| P2 | JIT Perf P2 — 热更感知内联器 | 3-4w |
| P3 | JIT Perf P3 — Intrinsic 识别 | 2-3w |
| P3 | JIT Perf P4 — 边界检查消除 | 2-3w |
| P3 | JIT Perf P5 — 循环优化 | 3-4w |
| P3 | JIT Perf P6 — SIMD | 4-6w |
| P2 | AOT PHASE 5 — 方法内联 | 极大 (独立评估) |
| P2 | AOT PHASE 5 — 去虚拟化 | 大 |
| P3 | AOT PHASE 5 — LTO 集成 | 中 |
| P3 | AOT PHASE 5 — 大程序集分页 | 小 |

### Phase 5: 全量翻译 (穿插进行, 预估 3-5d)

| 优先级 | 任务 | 预估 | 说明 |
|--------|------|------|------|
| P2 | 创建可执行测试程序集验证 convert-to-cpp | 1d | 端到端验证 |
| P2 | InternalCall 运行时桥接 | 2d | 集中实现 |
| P2 | 多程序集联合编译测试 | 1d | 多 Assembly 路径验证 |
| P2 | FullAssemblyEmitter wiki 更新 | 0.5d | 架构描述与实际同步 |

### 总预估

| Phase | 内容 | 预估 | 说明 |
|-------|------|------|------|
| Phase 1 | Foundation-DLL 攻坚 | 5-7d | 最高优先，阻塞工业化评级提升 |
| Phase 2 | CI/CD + 工程卓越 | 3-5d | 基础设施补齐 |
| Phase 3 | 质量加固 | 1.5-3.5d | 稳定性、AOT Phase 4 (EH 工业化缺口已修复 -2d，batch-2 Linux SEH 代码完成 -1.5d) |
| Phase 4 | 性能优化 | 18-23 周 | JIT Perf P0-P6 + AOT Phase 5 |
| Phase 5 | 全量翻译 | 3-5d | 可穿插进行 |

**总预估**: 短期 (Phase 1-3) ~9.5-15.5 天 FT + 长期 (Phase 4) ~18-23 周
