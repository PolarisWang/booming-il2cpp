# Chaos IL2CPP 核心能力综合评估 — 后续开发指南

> **状态**：当前指引（现行）｜ **评估日期**：2026-08-08
> **方法**：4 个并行只读调研 agent 分域深挖 + 性能/验证基线交叉核对
> **用途**：作为后续开发的**能力基线 + 优先级指南**。缺哪补哪，先治最关键的两条硬短板。

---

## 总览：一句话结论

这是一个**功能架构完备、但执行质量与性能未达生产级的 IL2CPP**——能翻译能跑、动态特性（interpreter/热更新）甚至比 JIT 更成熟，但 **JIT 执行质量**和 **AOT 相对 .NET 8 的性能**是最硬的两条短板，且都指向**同一根因：中间层 dispatch 与环境建模不牢**。

## 成熟度表

| 能力域 | 成熟度 | 一句话 |
|---|---|---|
| 翻译管线（IL→C++） | 🟡 中 | 覆盖广、架构完整；缺 GVM/一等 Span/vararg |
| AOT 运行时 | 🟢 高 | vtable/类型/线程/GC 分配/反射 interop 全在位 |
| Interpreter | 🟢 高 | 4 层降级 + SEH/OSR 完整，functional 最成熟 |
| GC | 🟢 高 | 分代/并发 mark/写屏障/Compaction 齐备 |
| **JIT** | 🔴 **低** | 最大短板：无真寄存器分配/保守栈扫描/EH 精简 |
| 热更新 | 🟡 中 | 双层分派完整生产可用；生态件缺 |
| 跨平台 ABI | 🟡 中 | Windows 强，iOS/Android 仅 Smoke 门 |
| 工具链 | 🟡 中 | 成体系；缺 IL-dump/Linker CLI |
| **性能** | 🔴 **低** | AOT 相对 .NET 8 在核心路径 2.5–30× 落后 |
| 验证体系 | 🟡 中 | 链路完整；覆盖口径失真、审计台账过期 |

---

## 分域能力详情

### 1. 翻译/CodeGen —— 覆盖广、架构完整、有几个"语义天花板"

**已具备**
- 218 opcode 解码开关（`LoaderStage.InstructionDecoding.cs`）
- `constrained.`/`calli`/`refany`/`jmp`/filter(规划层) 均处理
- Planning/Emitter 分层 + 结构化 IR + 150+ Scriban 模板
- 泛型共享、async 状态机、delegate thunks、值类型 ABI carrier
- closure 契约模型完整

**核心缺口**
1. **泛型虚方法 dispatch（GVM）整体缺失**——全仓 `GenericVirtual/gvm` 零命中。泛型类型实现接口 + 泛型虚方法无法翻译。**架构上锁死完整 CLR 语义**。
2. **一等 `Span<T>`/ref struct(ByRefLike) 未建模**——契约层无 `IsByRefLike` 位，Span 密集代码走 RuntimeSkeleton 骨架回落而非原生 codegen。现代库(Text.Json/Regex)翻译率受限。
3. **`vararg` 无处理；`tail.` 前缀被静默丢弃**（`InstructionDecoding.cs:326`）——"能解码但语义丢失"的隐藏坑，产物不等价但不报错。
4. **无 opcode 级覆盖审计闭环**——emitter `default` 抛错+整方法降级存在，但没量化"多少方法因 unsupported opcode 降级"，缺口被 `factPassRate` 掩盖。

### 2. Runtime/Interpreter/JIT/GC —— Interpreter 与 GC 高，JIT 是全场最大短板

**已具备（强）**
- AOT runtime-core 承载完整（vtable/method_table、layout_engine、线程 safepoint 完整握手、异常 SEH、GcAllocateFast 热路径、反射/interop/marshal/com）
- Interpreter 4 层降级管线 + SEH filter/finally/fault + OSR
- GC 3 代 + SATB 写屏障 + 并发 mark + STW compact + stress 测试

**核心缺口（JIT 集中）**
1. **无真正的原生寄存器分配**——T4 是栈式寄存器文件（load→compute→store 栈往返），图着色分配器已写但未接入（`t4-limitations.md` #4）。
2. **保守栈扫描（无 GC slot maps）**——所有 64 位视为潜在引用，生存对象过度保留。
3. **JIT 层 EH 语义精简**——filter 当 catch-all、catch 变量 overwrite-all、finally 无正确 unwind、仅 Windows VEH。
4. **OSR/deopt 非真栈替换**——溢出用 `kDeoptMagic` 重跑整方法，大副作用方法执行两次。
5. **JIT 单 monolithic 类**（6229 行 `NativeCodeGenerator`）承载全部职责，8-module 拆分仅 proposal。
6. **server GC 默认 OFF**（WKS 对齐 Unity），多核吞吐受限。

### 3. 热更新 + 跨平台 ABI + 运行时服务

**已具备（强）**
- 热更新双层分派完整（HotpatchTable + HotpatchNameRegistry，ARM64 acquire 竞争已修）
- PatchLoader/.patchdata v2 register-IR 直载 + lazy IR + T2-T4 分层 + PGO
- method_replacement 双路径生产可用、PatchMetadataCache iOS 兼容 self-contained
- PAL 26 模块、诊断对标 Unity 三支(eventpipe/debugger/SOS)骨架、Linker 四阶段

**核心缺口**
1. **iOS/Android 只到 Smoke/Packaging 门，无真 ABI 验证门**（`CMakePresets.json`），XMM0-vs-RAX/HFA/struct-byref 全靠生成器注释对抗、**无一条自动化 ABI 回归测试**。
2. **`src/native/codegen/` 是空壳目录**（0 文件）——C++ codegen 全托管化，native 侧缺独立 codegen 里程碑。
3. **trace/profiling 缺可导出采样事件流**——`profile.h` 是内联 cycle scope，EventPipe 只有 init/shutdown 骨架。
4. **热更新生态件缺失**——patched method 的 debugger/EventPipe 符号一致、多版本灰度/回滚运行时策略。
5. **iOS HFA/decimal carrier 特化未证实**。

### 4. 工具链 + 性能 + 验证体系

**已具备**：5 工具成链（AutoTestGenerator/DllManifest/FoundationPack.Analysis/CollectionGen/TestProjectGenerator）；4 栈归一化 benchmark（chaos-aot/jit/net8/net10）；验证链路 fact→benchmark→coverage→managed→report→aggregate；10 条 CI workflow。

**核心缺口**
1. **【性能·最大缺口】AOT 相对 .NET 8 在核心路径系统性落后 2.5–30×**（CoreLib ~3.4–6.9×、Brotli ~30×、Reflection.Metadata ~14×、Parallel ~7×），瓶颈全指向 `kChaosExternalRuntimeFnTable` 中间层 + cross-assembly 绕行 + intrinsic 落 interpreter fallback。
2. **【性能证据·可信度未闭环】**AOT 微方法 ns 级、alloc=0、fast-path=1.0，无法区分"真实优化"与"短路未测"——部分 family"快 90%"与 CoreLib"慢 6.9×"同时成立，harness 疑似有伪影。
3. **【工具链】缺 IL-dump/metadata 查看器 + 死代码裁剪 Linker CLI/AOT profile 引导**。
4. **【覆盖盲区】CoreLib 内层语义核心方法未被 fact/benchmark 覆盖**（numerics/runtime-compiler 缺口 ~73%、buffers 92%），外层"全绿"内层塌方。
5. **【数据治理】双口径失真**——aggregate 均值 vs 中位数分叉；audit 台账过期。

---

## 与生产 IL2CPP 对标

| 能力 | Chaos 现状 | 生产 IL2CPP | 差距 |
|---|---|---|---|
| 原生寄存器分配 | 无（栈式） | 图着色 + Liveness | 🔴 功能级 |
| GC slot maps | 保守栈扫描 | 精确栈图 | 🔴 功能级 |
| JIT EH | 精简(仅 VEH) | 全精确语义 | 🟡 功能级 |
| OSR/deopt | 重跑整方法 | 精确 stack map 真 OSR | 🟡 功能级 |
| GVM dispatch | 缺失 | 全支持 | 🔴 能力缺失 |
| 一等 Span/ref struct | 骨架回落 | 原生 codegen | 🟡 |
| AOT 相对 .NET8 性能 | 2.5–30× 慢 | IL2CPP ≈0.9–1.5× | 🔴 量级 |
| Debugger/EventPipe | 骨架 | 生产级 | 🟡 |
| 平台 | Win 强/iOS-Android Smoke | 全平台+32-bit+Web | 🟡 |

---

## 开发指南：补齐优先级（按此推进）

> 遵循项目全局优先级 P1(性能/执行质量) > P2(架构完整性) > P3(热更新)。但注意 P2 的架构完整性高度耦合 P1 的项目。

### P1 — 性能与执行质量（最关键，先行）

1. **消除中间层 dispatch**：把 `kChaosExternalRuntimeFnTable`/cross-assembly 绕行的 dispatch 内联，让 CoreLib 核心路径脱离 interpreter fallback。**这是 2.5–30× 落后的根因。**
   - 前提：先修 benchmark harness 可信度（真实 work + mean/median 口径），否则无从度量优化成效。
2. **接入精确 GC slot maps + 图着色寄存器分配**：把已写好的分配器接入 + 精确栈图。JIT 从"能跑"到"生产"的分水岭，性能与内存双瓶颈。配套拆 monolithic 类。

### P2 — 架构完整性

3. **补 GVM + 一等 Span**：泛型虚方法 dispatch + ByRefLike(U IsByRefLike)建模，解锁泛型接口 + 现代库翻译率。
4. **验证可信**：按 chunk 严口径重铺 CoreLib 内层 subject；聚合口径改 median 基准；清理过期审计台账。

### P3 — 工具链与生态

5. **IL-dump/查看器 + 死代码裁剪 Linker CLI**：直接削代码体积与 dispatch 膨胀。

### 立即可做的第一抓手

**修复 benchmark 可信度**——它是唯一无架构侵入、又能让后续所有性能决策变可信的抓手（修 harness 伪影 + 聚合口径统一）。

---

## 关联文档
- `docs/dev/assessments/t4-limitations.md` — JIT/GC/T4 精确认局限性
- `docs/dev/assessments/gap-analysis.md` — 技术债务差距
- `docs/dev/assessments/roadmap-multi-assembly-aot.md` — 多程序集 AOT roadmap
- 评估数据源：`nightly-build-report/`、`tests/perf/`
