# Chaos IL2CPP 核心能力综合盘点 — 基础能力待补足评估报告

> **评估日期**：2026-08-12 ｜ **方法**：4 个并行只读调研 agent 分域深挖（GC / JIT / Runtime-翻译-CodeGen / 测试-验证-构建）+ 权威自评文档交叉验证 + git 增量核对比
> **参考基线**：`docs/dev/assessments/il2cpp-core-capability-guide.md`（2026-08-08，现行指引）+ 各域最新 review 档
> **声明**：本报告为只读综合分析，不含任何代码改动。所有缺口均附 code/file:line 证据。

---

## 一、总览：一句话结论

这是**一个"功能架构完备、核心正确性自证能力强、但工程/验证真实性断裂 + 性能未达生产级"的 IL2CPP**。

自 08-08 能力指南以来（**166 commits**），进展显著：JIT 从"无真寄存器分配/保守栈扫描"跃升到"带省写穿的 Chaitin-Briggs 图着色 + 每-safepoint 精确 GC 根映射 + 4 层优化器"；GC 三代链 + 并发 BGC + 世代写屏障全落地；codegen 单体 6229 行拆分完成。**核心架构骨架已超过"能跑"，向"生产级"靠拢。**

但**最硬的短板全部指向同一根因：验证体系不能证明"翻译等价、覆盖真实、门禁可靠"**——coverage 只看 lowering 子集（CorLib 5/75 塌方）、67 个预编译 entry 扫出 111 个 AOT fallback-return-0 正确性缺口、JIT 核心测试被 known-baseline 冻结成"假绿"、统一入口 `ci_test.py` 未被任何 CI workflow 实际调用。**性能上 AOT 相对 .NET8 在核心路径仍落后 2.5–30×**，根源是中间层 dispatch + 跨程序集绕行 + 外部 runtime 兜底。

---

## 二、成熟度总表

| 能力域 | 成熟度 | 一句话（含较 08-08 的增量） |
|---|---|---|
| 翻译管线（IL→C++） | 🟡 中 → **🟡 中高** | 覆盖广、20+ 程序集 fact 全 pass；但复杂 async/display-class 直接 FAIL、运行时泛型落解释器 |
| Runtime-core | 🟢 高 | VTable/类型/线程/异常/同步/GC 全在位 |
| Interpreter | 🟢 高 | 4 层降级 + SEH/OSR 完整，functional 最成熟 |
| **GC** | 🟢 高 → **🟢 高** | 4 代 + SATB-BGC + 世代写屏障 + 精确根映射；骨架对齐 CoreCLR ~80%，工程深度缺 |
| **JIT** | 🔴 **低 → 🟡 中高** | **最大进步域**：图着色+精确 GC 根+树IR优化器已落地；残差=Windows 寄存器窗捕获/FPR 着色/SIMD 发射 |
| 热更新 | 🟡 中 | 双层分派完整生产可用；生态件缺 |
| 跨平台 ABI | 🟡 中 | Windows 强；iOS/Android 仅 Smoke/Packaging 门，无 ABI 自动化回归 |
| 工具链 | 🟡 中 | 成体系；缺 IL-dump/Linker CLI |
| **性能** | 🔴 **低** | AOT 相对 .NET8 核心路径 2.5–30× 落后（中间层 dispatch 根因） |
| **验证体系** | 🟡 中 → **🔴 真实性断裂** | 入口/治理成熟，但覆盖失真 + JIT 假绿 + CI 双轨未消除 |

---

## 三、分域能力详情

### 域 1：GC（CRAG）— 架构高，工程深度待补

**已就绪（高成熟度，证据见 `gc-vs-coreclr-review-2026-08-12.md`）**
- 4 代分代（Nursery→Gen1→Old/Gen2→LOH）+ condemned 过滤 + demotion；`gc_region.h:49-57`
- 世代感知写屏障（两级卡表 + card bundle + SATB 预写 + 域卸载）、三层接线（codegen/JIT/解释器）；`gc_card_table.cpp:95-117`
- 并发 BGC 分相状态机 + 并行 mark（≤8 worker work-stealing）+ 并发 sweep；BGC−Young 协调暂停（防死锁）
- 精确根扫描：`GcSlotMapV0` 栈 + `GcPointMapV0` 寄存器根 + 保守兜底；`gc_root_scanner.h:81-142`
- 动态代大小（EMA 生存率伺服）、TLAB 自适、POH/LOH、MemoryDomain 域卸载（CoreCLR 所无的差异化优势）、verify_heap 3 级、24 配置旋钮、40+ GC 测试

**核心能力缺口**
| 优先级 | 缺口 | 证据 | 补齐价值 |
|---|---|---|---|
| **P0** | young-GC Phase-1 晋升回写深因（A2b，旧代引用跟进不完整）——已知 open 正确性 bug | review:21,194 | 消除 UAF/漏晋升，stress 全绿 |
| **P0** | 世代写屏障压力验证缺失（B2，"漏跨代引用 UAF"未证伪）——最大"实现 vs 验证"落差 | review:110,191 | 世代屏障从"实现级"到"生产级" |
| **P1** | 100%-free 页永不回收 + `FindPage O(N)`（长期内存线性增长 + 慢 mark） | review:103 | 消除长期内存膨胀 |
| **P1** | Server GC 多堆默认 OFF + **零 CI 验证** | `gc_features.h:38` | 默认开启即生产风险；解锁多核吞吐/Dynamic Heap Count |
| P2 | BGC servo 空间域动态调优缺失（仅 EMA 时域）+ 旋钮 24 vs ~75 | `gc_scheduler.h:464-495` | 空间调优闭环 |
| P2 | VerifyHeap 深度（kFull referenced-walk 标注 future）+ 缺 poison | `gc_diagnostics.cpp:69-74` | 防簿记漂移 |
| P2 | GC ETW ~8 事件偏少（缺 BGC 阶段族）+ BGC Gen1 payload 硬编码 0 | `gc_bgc.cpp:321` | 生产可观测 |

### 域 2：JIT — 最大进步域，残差集中在寄存器/GC/异常路径

**已就绪（中高成熟度，较 08-08"无寄存器分配/保守栈扫描"大幅跃升）**
- 0→8 分层编译状态机 + precode stub + RCU 退役；`patch_loader.h:136-149`
- Chaitin-Briggs 图着色寄存器分配（GPR x64 9 色/ARM64 23 色）+ **省写穿（write-through 消除，非 arg caller-saved 完全驻留）** + def-kill 修复；`jit_reg_alloc.h:246-759`
- 每-safepoint 精确 GC 根映射（GcPointMapV0）+ 31bit 扩位 + 寄存器根；`jit_codegen_gc.cpp:121-170`
- T4↔interpreter deopt/OSR、双平台 SEH/unwind、**树 IR 优化器（内联+折叠+CSE+边界消除）**、PGO Tier1、CODEGEN_STATS

**核心能力缺口**
| 优先级 | 缺口 | 证据 | 补齐价值 |
|---|---|---|---|
| **P0** | Windows 寄存器窗捕获不可靠（APC-park）→ 寄存器 GC 根下探受限，需依赖 safepoint 栈写抵消驻留收益 | `pal_preempt_win32.cpp:54` `PalCaptureReliable()=false`；`jit_engine.h:554-557` | 寄存器根免栈写，GC 暂停耗时降 |
| **P0** | FPR 图着色漏边（多定义 FPR live-range 未做 GPR 式保守修复）→ SIMD 栈往返 | `jit_reg_alloc.h:663,649-664` | 减少 SIMD 栈写 |
| **P0** | 精确 GC slot map 在 deopt/OSR 与 GC 扫描间未完全对齐（寄存器根仅为"前向元数据"） | `jit_method.h:69-71`；`jit_codegen_gc.cpp:147-151` | 避免 root 漂移 |
| P1 | SIMD 发射覆盖不全（170 op 仅部分，其余靠解释执行/deopt） | `jit_codegen_simd.cpp:44-135` | 数学/渲染方法成本降低 |
| P1 | Win .pdata/.xdata 仅 stack-walk，未集成 EH personality | `jit_unwind.h:10-12,98` | 调试器/unwind 识别 T4 帧 |
| P1 | JIT/AOT 一致性差分测试 + fuzz 缺失（x64 CI 抓不到 ARM64 专用问题） | CMakeLists:108,112 | 防 regalloc/deopt/GC root 回归 |
| P2 | SEH 方法跌线性 fallback（无内联/CSE）；图着色溢出无 split/rewrite，大方法整方法放弃着色 | `jit_codegen_generate.cpp:1117-1119`；`jit_reg_alloc.h:592-596` | 异常路径性能 + 大方法覆盖 |

### 域 3：Runtime / 翻译 / CodeGen — 核心可用，被三类兜底侵蚀

**已就绪**
- 基础 IL→原生 C++ 翻译管线（Loader→SemanticWorld→Linker→MetadataWriter→CodeGen）+ Scriban 发射；System.Linq/Expressions/Text.Json/Xml/Net 等 20+ 程序集 fact 全 pass
- VTable/method_table/type_registry、静态字段/cctor、StringTable、Monitor（ThinLock+膨胀）、异常（SEH/SETJMP 5 形状）、layout_engine、热更新基础设施

**核心能力缺口**
| 优先级 | 缺口 | 证据 | 补齐价值 |
|---|---|---|---|
| **P0** | 复杂 async 状态机直接 `CHAOS_IL2CPP_FAIL()`（未走解释器兜底） | `MethodEmission.cs:206` | 含复杂 await 的方法当前直接崩溃 |
| **P0** | display-class `<>c` 闭包构造器 stub 化 → `CHAOS_IL2CPP_FAIL()` | `MethodEmission.cs:163-176` | Span/闭包密集 BCL 路径中断 |
| **P0** | 外部 runtime 兜底返回哨兵 0（crypto/PipeReader/Vector/TryRead）——"伪正确"掩盖语义错误 | `interop_stubs.cpp:742-835` | 消除虚假 pass |
| P1 | 运行时泛型实例化落解释器（MakeGenericType/MakeGenericMethod 无 AOT 特化） | `interpret_method_call.cpp:7` | 反射泛型激活提速 |
| P1 | Crypto 仅 88%（1077/1222），走解释器非原生 | STATUS.md P0#5 | 加密热路径 native 化 |
| P1 | 反射 stub 化严重（CustomAttr/MethodInfo 等大批 deferred） | `reflection/remaining_stubs.cpp:2` | 完整元数据语义 |
| P2 | COM 未工业化/Alc 未完全语义化/MemoryDomain 卸载残留/ARM64 仅基础 | `COM_INDUSTRIALIZATION_ANALYSIS.md` | 平台与互操作性 |

### 域 4：测试 / 验证 / 构建 — 入口成熟，真实性断裂

**已就绪**
- 四层测试金字塔 + 统一入口 `test_driver.py`（读 suite_contract.yaml 全量派发，无 skip 纪律，JSON 契约稳定）
- known-failure baseline 体系 4 层齐全 + stale-known 自证明检测
- foundation-dll 验证管线（build/fact/coverage_audit/benchmark/hotupdate/aggregate 等 stages）
- CMakePresets 12 档 + 6 平台 toolchain；12 条 CI workflow（含 QEMU ARM64 交叉、GC 单测门）

**核心能力缺口**
| 优先级 | 缺口 | 证据 | 补齐价值 |
|---|---|---|---|
| **P0** | 覆盖率"5/75 塌方"——coverage 只 lowering codegen 子集，非真实覆盖 | `dispatch-intermediate-layer-research.md:76` | 所有性能/正确性度量失真 |
| **P0** | 111 个 AOT 正确性缺口（fallback-return-0，67 预编译 entry 扫出）：Xml xsl 46/runtime-compiler 16/numerics 12/interop 13/pipelines 6；8 个大 chunk fact 不可跑 | `a1-external-fallback-gap-report.md:12-19` | "能翻译能跑但语义缺失"硬伤 |
| **P0** | foundation-dll 世代写屏障（`_dst_ref`）从未 regen 验证——AOT 生成码吃不到短路 | `gc-vs-coreclr-gap-assessment-2026-08-10.md:39-42` | UAF 风险点 |
| P1 | `ci_test.py` 已写好但**零接入**——CI 双轨未消除（workflow 仍用 raw dotnet test/ctest） | `ci-test-unification.md:28-66` | 消除本地与 CI 口径分裂 |
| P1 | JIT 核心测试常红被 baseline 冻结（test_jit_native 39/68 fails 全 known）——"假绿"门禁 | `known-failures.integration.yaml:13-18` | 消除假绿，回归真翻 gate |
| P1 | sanitizer（ASAN/TSAN）存在但 CI 零引用 | `cmake/sanitizers/` | GC/JIT 内存安全常态门 |
| P2 | 跨平台 ABI 无自动化回归（iOS/Android 仅 smoke/packaging） | `il2cpp-core-capability-guide.md:70-71` | 跨平台正确性前提 |
| P2 | MetadataWriter 测试项目零执行（缺 Test.Sdk）、验证工具链缺 IL-dump/Linker CLI | `known-issues.md:12` | 数据治理与裁剪 |

---

## 四、能力基线问题（横切）

跨 4 域的底层断裂，是产能基线无法信任的根因：

1. **覆盖真实性**：`coverage-audit` 判定的是"subject 存在"而非"native 翻译正确"——`InvocationPlanning.cs` 仅 `_isAot`（命中 `_methodsBySubjectId` 且 instructions>0）才生成真 native body，其余全走 `kChaosExternalRuntimeFnTable`→interpreter。
2. **性能度量可信度**：AOT 微方法 ns 级/alloc=0/fast-path=1.0，无法区分"真实优化"与"短路未测"；aggregate 均值 vs 中位数双口径分叉。
3. **门禁可靠性**：known-fail 不 flip red + ci_test.py 未接入 → 本地全绿与 CI 子集不一致。

---

## 五、待补足基础能力清单（按优先级 × 成本）

| 批次 | 内容 | 关联最大短板 | 重要×成本 |
|---|---|---|---|
| **P0-A** | 修 benchmark harness 可信度（真实 work + mean/median 口径）——唯一无架构侵入、让所有性能决策可信的抓手 | 验证真实性 | 高×低 |
| **P0-B** | Codegen 外部 runtime 兜底 0 → 消灭 111 AOT 正确性缺口（Xml/Interop/SIMD/Unsafe/PipeReader 六大域） | AOT 正确性 | 高×高 |
| **P0-C** | 修复杂 async `CHAOS_IL2CPP_FAIL()` + display-class 闭包（改解释器兜底而非崩溃） | 运行时正确性 | 高×中 |
| **P0-D** | 世代写屏障压力验证 + young-GC A2b 晋升深因（GC 收敛） | GC 正确性 | 高×高 |
| **P1-A** | Windows 寄存器窗捕获（SuspendThread/寄存器根下探）+ FPR 着色保守修复 | JIT 正确性 | 高×中 |
| **P1-B** | ci_test.py 接入 CI 消除双轨 + JIT 假绿解除 + sanitizer 常态门 | 门禁可靠 | 高×低 |
| **P1-C** | coverage 真实化（对 CoreLib 内层重建真实 subject 覆盖）+ 5/75 塌方修复 | 覆盖真实性 | 高×高 |
| P2 | Server GC CI 矩阵 / Page 回收 / JIT-SIMD 发射 / Win EH personality / Crypto native 化 / 运行时泛型特化 | 各域工程深度 | 中 |
| P3 | GVM + 一等 Span/ref struct（解锁现代库）/ ETW 扩容 / 跨平台 ABI 回归门 / Linker CLI | 架构完整性 | 中×高 |

---

## 六、立即可做第一抓手

**修复 benchmark harness 可信度（P0-A）**：已在 `tests/e2e/verification/` 部分推进（见 memory：fd-verification-harness-fixes-complete），但 AOT/managed 覆盖不对称（dashboard.coverage 暴露 chaosAotVsNet8 空）仍使其不可信。这是唯一无架构侵入、能让后续所有性能决策可信的抓手。

---

## 七、风险提示（自评可信度边界）

1. 4 个 agent 为只读并行调研，行号指向读到的源码，但部分编译期宏/生成代码路径（如 `_isAot` 判定、`CanCompile` 放行）需运行期验证。
2. 08-08 能力指南的部分判定已被 166 commits 超越，本报告已按当前源码刷新。
3. 各域"缺口"按影响分级，部分 P2/P3 属工程增强而非正确性。
