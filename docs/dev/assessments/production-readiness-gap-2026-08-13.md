# chaos-il2cpp 生产级差距综合分析报告

> 评估日期：2026-08-13
> 基线：HEAD `c7605b19a`
> 方法：5 个并行领域探针（GC / JIT / 验证 / 解释器+运行时+热更 / codegen+平台+构建）+ 仓库规模/提交史交叉核验。
> 原则：每条差距标注来源 —— `[本次代码实证]` / `[memory 复核]` / `[May 文档(已过时需修正)]`。

---

## 〇、开场定调

这是一个**单人主导**的深度 IL2CPP 运行时复刻项目：~216 个原生 C++ 子系统构成 8.5 万行 C++、219 次月均提交、架构野心极大（三代分代 GC + 完整 JIT + 热更 + 解释器 + NativeAOT）。**但"宣称完成度"远高于"可证实完成度"。** 全库真理性验证覆盖 **0/2571 方法（nativeProof 全 pending）**，STATUS.md 却对同一批组件标 "✅ 完成 / ✅ Passed"。这是贯穿所有域的第一根因。

一句话结论：**当前状态是"令人惊艳的科研/原型级运行时骨架"，但不是"可放心跑生产负载的产品级运行时"。**

---

## 一、仓库规模与形态（事实框）

| 维度 | 数值 |
|---|---|
| 总提交 | 2162（≤1 名作者；6月1177 / 7月9 / 8月391，突发式工作） |
| 原生 C++ | 8.5 万行（runtime-core 51K+27K，jit 14K+9K，interpreter 8.6K+1.8K，diagnostics 4.2K） |
| managed (C# codegen) | **仅 8,632 行**（对一套 IL→C++ 翻译管线而言异常精简） |
| 单测文件 | tests/unit 1279，tests/integration 30 |
| CI workflows | 11 个 |
| 平台声明 | Windows / Linux-x64 / Android-arm64（README）+ CMakePresets 另声明 macOS/ios |
| foundation-dll 验证审计 | 14 目标 DLL，**13 not-started / 0 completed / closure 0%** |

---

## 二、分域生产级就绪度评分

| 域 | 评分 | 一句话定性 |
|---|----|-----------|
| **GC** | 50–58% | 骨架高度对齐 CoreCLR，多数 P0 内存安全已修；但 A2b 残余仍 CI 门外、Server GC 默认 OFF |
| **JIT** | 6.5/10 | GC 安全侧"保守生产级"，但浮点 deopt 裂步、fuzz 逃逸 ≥64 vreg、ARM64 仅 QEMU |
| **Codegen+翻译+PAL+构建** | 2.5/10 | SIMD AV 已修但 SIMD 语义仍假值、LINQ 全 fallback、7 平台只有 Windows+QEMU 有 CI |
| **解释器+运行时+热更** | 2/10 | 解释器对象模型边界裸指针未闭合、反射大块 stub、BGC 挂起 open |
| **验证/测试可信度** | 2/10 | 真理性验证 0/2571 pending，STATUS "✅ Passed" 与 dll-matrix 0% 闭包互为矛盾 |
| **加权综合** | **~25/100** | 科研原型之上、生产级之下 |

---

## 三、分域差距清单

### D1 — GC（50–58%）

| # | 严重度 | 差距 | 状态 | 证据 |
|---|--------|------|------|------|
| 1 | **P0 内存安全** | **A2b 跨代写屏障竞态残余** | **止血未根治（KNOWN-FAIL + CI 门外 + 仅夜间 `\|\| true`）** | 根因锁死 store-then-barrier 窗口（`gc_region_barrier_stress_test.cpp:110-145`）；主根因 `cf0609cd5` 已把 dominant dangling 压到残余级，但断言仍 `GC_CHECK(dangling==0)` 挂 known-fail（`known-failures.integration.yaml:36`） |
| 2 | **P2** | **Server GC 默认 OFF 且零 CI** | **open** | `gc_features.h:38` `CHAOS_IL2CPP_GC_SERVER =0`，全树 `GC_SERVER 1` 零 caller；gc-ci 为 WKS 单堆矩阵 |
| 3 | P1 | **并发正确性测试被排除出通过门** | **open** | `gc-ci.yml:72-78` `-LE "benchmark\|stress\|soak"`；nightly `|| true`（:120）失败不红 |
| — | P0 | ~~chaos_is_gc_pointer nursery 盲点~~ | **✅ 已修** | `gc_api.h:105-124` 三段判定回退 `RegionManager::IsNurseryPointer()`（below-base 段不再漏判） |
| — | P0 | ~~CodegenStSFld 反向 GC-root 注册~~ | **✅ 已修** | `jit_helpers.cpp:489-503` 加 `chaos_is_gc_pointer` 守卫，对齐另两路 StSFld |
| — | P0 | ~~终结器 stub~~ | **✅ 已修（May 文档过时）** | `gc_api.cpp:618-634` 真实注册 + resurrect 路径（`gc_old_gen.cpp:3138/3146/3206`） |
| — | P1 | ~~页回收 + FindPage O(N)~~ | **✅ 已修** | `gc_old_gen.h:325` O(log n) + `VirtualFreePage`（08-12 差距已关闭） |

### D2 — JIT（6.5/10）

| # | 严重度 | 差距 | 状态 | 证据 |
|---|--------|------|------|------|
| 1 | **P0 正确性** | **FPR deopt/OSR stride 不一致（8 vs 32）** | **open（本次代码新发现）** | 生产者 `StoreFpr` stride 32（`jit_codegen_memory.cpp:170` / `jit_engine.h:117-119`），消费者 `DeoptSaveFrameState` stride 8（`jit_helpers.cpp:857`、`jit_deopt.cpp:100`）→ ≥65 的 live FPR 在 deopt→OSR 读到错误浮点值 |
| 2 | **P0 正确性** | **Fuzz 逃逸 ≥64 vreg emit 越界路径** | **open（无回归护栏）** | 原 1000-variant Fuzz SEGFAULT 根因（tree vreg≥64 越界）修复未闭环；当前 `jit_native_test.cpp:2264` max_reg=2-7，从不产生 vreg≥64 临时体 |
| 3 | P1 | **ARM64 仅 QEMU user-mode，无真机 CI** | **open** | `arm64-jit-ci.yml` 交叉编译 + qemu-aarch64；不覆盖权限页/SVE/NEON/W^X 真机行为 |
| 4 | P1 | **deopt spill 基址 + is_spilled 死分支** | **open** | `ReadSpillSlot` 按 trap-RSP 读，注释称 RBP 相对（`jit_deopt.cpp:20`）；`ReconstructRegisterFile` 两分支相同（:60-64） |
| 5 | P1 | **热更 demotion 未注销 GC map** | **open** | `DemoteJittedMethod` 未注销旧 GcPointMapV0（T2.3-C 未闭环） |
| — | P1/P2 | GC 寄存器窗口捕获（Windows 豁免） | 已决策落地 | Windows APC 无法可靠捕 JIT safepoint reg → 退守恒栈槽地板（纯加性，不欠保留，无 UAF） |

### D3 — 验证/测试可信度（2/10，跨域总根因）

| # | 严重度 | 差距 | 状态 | 证据 |
|---|--------|------|------|------|
| 1 | **P0 治理** | **真理性验证覆盖 0%**：`family-verification.json` 全库 nativeProof **0/2571 pending**，无任何 AOT native 正确性被认证 | **open** | `family-verification.json` |
| 2 | **P0 治理** | **STATUS "✅ Passed" 与真实闭包矛盾**：STATUS.md:88-114 标 25 assembly ✅ Passed，`dll-matrix.json` 却 13/14 not-started、closure 0% | **open** | `dll-matrix.json` |
| 3 | P1 | **111 AOT 缺口仍在且是下界**（未重测、8 大 chunk 不可跑、checksum 盲区数千 passed=true/value=0 无法判定） | **open** | `a1-external-fallback-gap-report.md` |
| 4 | P1 | **CI 绿门靠 known-fail 过滤**：known-fail integration 36 条 / unit 61 条，KNOWN-FAIL 不阻塞合并 | **open** | `known-failures.*.yaml` |
| 5 | P1 | **覆盖分母不可信**：`verif-cov.log` Coverage 144.4%（覆盖数 > 声明数）、chaosAot vs net8 基准 N/A% | **open** | root `verif-cov.log` |

### D4 — 解释器 + 运行时 + 热更（2/10）

| # | 严重度 | 差距 | 状态 | 证据 |
|---|--------|------|------|------|
| 1 | **P0 内存安全** | **解释器对象模型边界裸指针未闭合**：3 出入口物化（Q1/1.1/2.2/3.1）被用户中止，真实 GC 指针仍裸进解释器槽，8 处写屏障靠 `chaos_is_gc_pointer` 地址近似守卫（依赖"解释器堆地址恒 < g_heap_base"未证实假设） | **止血 open** | `interpreter_vm.cpp:842,874,1603` 等；memory 三期架构否决 |
| 2 | **P0/P1 正确性** | **反射大块硬编码 stub 静默返空/0**：`GetExportedTypes→0`、`GetModules→0`、`GetEntryPoint→空` 等（`reflection/remaining_stubs.cpp:109-226`） | **open** | `remaining_stubs.cpp` |
| 3 | P1 | **BGC root_scan 挂起** | **open（known-fail）** | `chaos_gc_bgc_root_scan_test` 仍列 `known-failures.integration.yaml:25` |
| 4 | P1 | **解释器 blocking P/Invoke 未做 preemptive 切换** | **open** | `preemptive_fn/cooperative_fn` 仅 JIT codegen 消费，解释器 call 路径未调用 |
| 5 | P2 | **热更旧代码单调累积不回收** | 已知取舍 | `ReclaimDemoted` 永不 VirtualFree（T2.3-C 方案3，防 UAF换内存增长） |
| — | — | 终结器 / WeakTable / ConditionalWeakTable / DependentHandle | ✅ 真实实现（非 stub） | `gc_api.cpp` / `gc_old_gen.cpp` |

### D5 — Codegen + 翻译 + PAL + 构建（2.5/10）

| # | 严重度 | 差距 | 状态 | 证据 |
|---|--------|------|------|------|
| 1 | **高** | **SIMD 方法语义假值（fake-green）**：`interop_stubs.cpp:616-627` 按 subject_id 字符串 return 硬编码 1/0，任意 Vector 方法非 zero-input 语义不可信 | **open** | `interop_stubs.cpp:616-627` |
| 2 | **高** | **LINQ / 泛型委托缺真实 AOT 翻译**：~176 泛型方法全走 `ChaosExternalRuntimeFallback` 字符串路由 | **open** | `linq-external-runtime-fallback.md` + `interop_stubs.cpp:604-713` |
| 3 | **高** | **7 平台声明 vs 仅有 Windows + QEMU-arm64 真 CI**：11 workflow 无 macos/android/ios；arm64 仅 QEMU user-mode | **open** | `.github/workflows/`；`arm64-jit-ci.yml:868-898` |
| 4 | P1 | **泛型/ABI slot 解析缺口**：`TryResolveManagedType` 模糊匹配，泛型参数 `!!0` 不保证解析，靠 `FallbackTypeSubjectId` 约定串联 | **部分缓解** | `AotCoreIrLowering.Resolve.cs:205-289` |
| — | — | **SIMD 操作数物化缺陷（A2 AV）** | **✅ 已修** | `EmitVectorOperandMaterialization`（ExceptionEmission.Utilities.cs:580） |
| — | P1 | ~~stale 构建产物病灶~~ | **✅ 已修** | `build.py:287` reconcile + ensure_sdk `.source_hash` 指纹 |

---

## 四、最致命差距 TOP 5（跨域合并）

按"对生产级杀伤力"排序：

1. **真理性验证覆盖 0 / 2571，STATUS 却宣称全绿**（D3）—— 没有一根 AOT native 正确性被认证，"✅ 完成"是全仓失真之根，掩盖了其余所有问题。**这是治理级 P0。**
2. **A2b 跨代写屏障竞态残余**（D1）—— 并发正确性链最高风险点，仍 KNOWN-FAIL + CI 门外 + 夜间 `|| true`，残余 dangling 无法对生产稳定运行给出保证。
3. **解释器对象模型边界裸指针未闭合**（D4）—— 真实 GC 指针裸进解释器槽，仅靠未证实的地址近似守卫，内存安全闭环未建立。
4. **SIMD 语义假值 + LINQ 全 fallback**（D5）—— AOT 翻译正确性在数值敏感（Vector）与泛型委托（LINQ）两域静默失真，假绿。
5. **JIT 浮点 deopt 裂步 + fuzz 逃逸**（D2）—— live FPR 在 OSR/deopt 读错 + 曾崩溃的 vreg≥64 路径无回归护栏。

---

## 五、可落地的优先修复建议

不按"宣称"按"风险"排序：

| 优先 | 建议 | 域 | 估计 |
|---|------|---|-----|
| P0-1 | **把 A2b 达成 PASS 并从 known-fail 摘除**，拆掉 CI `-LE stress` + 夜间 `\|\| true` 过滤器 | GC | 关键路径 |
| P0-2 | **处理解释器 3 出入口物化**（或改写守卫为正式 GC 声明），撤销"地址近似"未证实假设 | 解释器/GC | ~1-2 周 |
| P0-3 | **修 FPR deopt stride**：将消费者侧读取改为 stride 32，加"live FPR + 触发 deopt/OSR"回归测试 | JIT | 数天 |
| P0-4 | **填真理性验证**：至少让 CoreLib completion-certification 真正 reachable，把 `nativeProof 0/2571` 拉起来 | 验证 | 中长期 |
| P1 | **SIMD 方法语义真实现**或诚实标"不支持"（撤掉假值 return） | codegen | ~1 周 |
| P1 | **把 Server GC 至少接一条 CI 门**（先不默认 ON，但要有一行覆盖） | GC/CI | ~1 周 |
| P1 | **修解释器 blocking P/Invoke preemptive 切换** | 解释器 | 数天 |

> 核心建议：**先把验证体系做成诚实** —— 让 "✅ Passed" 与 `dll-matrix.json` 的真实闭包对齐。没有可信的验证背书，所有"完成"都只是工程自证。

---

## 六、结论

- **优点（如实记录）**：三代分代 GC、并发/后台 GC、终结器 + 弱引用/依赖句柄全链真实；JIT 图着色 + 精确根映射 + 省写穿已落地；解释器 5 级 dispatch + tier/OSR/deopt 完整；近期多数内存安全 P0（nursery 盲点、StSFld 反向 root、SIMD AV、页回收/FindPage）确实已修。单人多月把这些做出来，架构与工程密度远超个人项目常态。
- **核心差距**：**验证可信度崩塌（0/2571）+ 2 个纯内存安全洞（A2b、解释器边界）未闭合 + 子域大量"假绿"stub（SIMD/LINQ/反射）+ Server GC 未启用**。
- **生产级就绪度综合 ~25/100**。它现在能跑一套可演示的解释器/JIT/GC 探针，但**不能**对生产负载给出稳定性、正确性或跨平台真机背书。
