# JIT Hotupdate 修复与工业化品质差距风险评估

> **评估日期**: 2026-05-24  
> **评估范围**: JIT hotupdate 路径修复 + foundation-dll 工业化差距综合评估  
> **参考**: industrialization-assessment.md (2026-05-23, 2.65/5), STATUS.md (foundation-dll-industrialization)

---

## 1. JIT Hotupdate 修复摘要

### 1.1 已修复：jit_registration.h extern "C" 重载冲突

**问题**: `jit_registration.h` 声明了两个 `extern "C" void RegisterJitMethods(...)` 重载（参数分别为 `JitMethodEntry*` 和 `JitEntry*`），违反了 C 语言链接的禁止重载规则（MSVC C2733）。

**修复**: 将新路径函数更名为 `RegisterJitEntryMethods`，保留旧声明（`RegisterJitMethods(JitMethodEntry*)`）用于向后兼容旧的生成文件。

**影响范围**:
- `src/native/runtime-core/jit_registration.h` — 声明修正
- `src/native/jit/jit_precode.cpp` — 实现函数名匹配
- `src/native/runtime-core/jit_registration.cpp` — 旧实现不变

**验证**: entry.exe 一次性构建通过，lite 回归通过。

### 1.2 已修复：runtime-entry.cpp fail hook 不一致

**问题**: 非 fact 模式（hotupdate/benchmark/patch-bench）无条件使用 `throw chaos_managed_exception{}` 作为 `g_chaos_fail_hook`，而 fact 模式在 `CHAOS_IL2CPP_EH_WIN32_SEH` 下使用 `chaos_raise_exception(0)`（SEH RaiseException）。并发 BGC 线程执行 `CHAOS_IL2CPP_FAIL()` 时，C++ 异常在 BGC 栈展开导致内存损坏。

**修复**: 所有非 fact 模式的 fail hook 统一为与 fact 模式一致的预处理器分支。

**验证**: 构建通过。但 crash 率未改变（见下文）。

### 1.3 未修复（预存在）：GC 调度器竞态 (SignalFullGcComplete)

**根因定位**:
- 崩溃点: `G_Scheduler().SignalFullGcComplete()`（gc_old_gen.cpp:2883）在 `AFTER_ETW` 与 `AFTER_SIGNAL` 日志之间
- 现象: 间歇性 SIGSEGV (0xC0000005)，与 fail hook 类型无关
- Crash 率: fact mode 10%, hotupdate mode 24%

| Mode | Crash Rate | 说明 |
|------|-----------|------|
| fact (无参数) | 5/50 (10%) | GC 初始化阶段的竞态 |
| --hotupdate | 12/50 (24%) | 额外的 fail hook 设置/重置增加线程访问模式变化 |
| --benchmark 0 10 | 1/1 崩溃 | 与 --hotupdate 相同根因 |

**原因**: `G_Scheduler().SignalFullGcComplete()` 中访问全局调度器状态时，BGC 线程与主线程之间存在竞态。属于 GC 工程的已知并发同步缺口。非短期可修。

---

## 2. 当前工业化品质差距 (累计)

### 2.1 全量工业化成熟度评分

基于 2026-05-23 的评估，叠加本日修复影响后重新评估:

| 维度 | 权重 | 评分 | 说明 |
|------|------|------|------|
| 功能覆盖度 | 25% | ⭐⭐⭐⭐☆ (4/5) | ~140 families 已验证，2 family 缺 subject IDs |
| CI/CD 集成 | 25% | ⭐⭐☆☆☆ (2/5) | CI smoke 已集成 (4 families, 6-stage)，无全量回归 |
| 文档完备度 | 15% | ⭐⭐☆☆☆ (2/5) | 仅有 data-dictionary，无架构/使用/故障文档 |
| 基础设施健壮性 | 15% | ⭐⭐⭐☆☆ (3/5) | BGC 竞态崩溃 ~10-24%, audit 误报 + 配置脱节仍存 |
| 可维护性（测试） | 10% | ⭐☆☆☆☆ (1/5) | 零测试 |
| 基准/回归能力 | 10% | ⭐⭐⭐☆☆ (3/5) | 数据已收集但无自动回归检测 |

**加权总分：2.65 / 5.0**（未变化 — CI smoke 集成抵消了部分风险，但 BGC 竞态仍为 blocker）

### 2.2 与已工业化模块的差距

| 模块 | 工业化评分 | 与 foundation-dll 差距 |
|------|-----------|----------------------|
| marshal-industrialization | 4.8/5 | +2.15 |
| metadata-writer | 4.5/5 | +1.85 |
| **foundation-dll (当前)** | **2.65/5** | **baseline** |

### 2.3 当前阻塞项

| ID | 类型 | 状态 | 影响 |
|----|------|------|------|
| SYS-001 | codegen 宏 vs runtime 头接口不对齐 | ⛔ 阻塞 Phase 2 | 多 family 的 unified-verification-report 使用旧版 fail 路径 |
| BGC-RACE | `SignalFullGcComplete()` 竞态 | 未修复 | fact mode 10% + hotupdate 24% 崩溃，阻塞 stable |
| SYS-002~004 | 管线基础设施问题 | 部分缓解 | audit 误报、文档缺口、零测试 |

---

## 3. 风险清单

### 3.1 P0 — 阻断级

| # | 风险 | 概率 | 影响 | 缓解措施 |
|---|------|------|------|---------|
| R1 | **BGC 竞态崩溃 (BGC-RACE)** | 10-24% | 高 — 任何运行都可能在 10-24% 概率崩溃 | 短期: hotupdate 跳过首次 GC 后运行; 中期: GC 工程修复 SignalFullGcComplete |
| R2 | **extern "C" 重载残留** | 低 | 高 — 旧生成文件与新头不兼容；当前已隔离 | 已修复。旧 .cpp 文件通过链接时解析绕过声明 |

### 3.2 P1 — 高影响，但不能阻塞

| # | 风险 | 概率 | 影响 | 缓解措施 |
|---|------|------|------|---------|
| R3 | **Scheme D Route B 与新路径的兼容性** | 中 | 高 — 未验证的 family 可能使用旧路径 | codegen 全量重建后已验证 2 assembly, 需扩展到更多 family |
| R4 | **CI 无回归检测** | 高 | 高 — 任何 codegen 变更可能破坏已验证 family | 已有 smoke，需全量回归 + 自动 PR blocking |
| R5 | **Microbench 崩溃 (BGC 同一竞态)** | 中 | 高 — stage 7 持续失败 | `--microbench` 模式同样触发 `SignalFullGcComplete()` 竞态 |

### 3.3 P2 — 需规划修复

| # | 风险 | 概率 | 影响 | 缓解措施 |
|---|------|------|------|---------|
| R6 | **零管线基础设施测试** | 高 | 中 — 维护风险 | 补 orchestrator/stage runner 单元测试 |
| R7 | **文档缺口** | 高 | 中 — 新人 onboard 成本 | 0.5d 可补齐 |
| R8 | **SYS-001 codegen 宏阻塞** | 高 | 中 — 但当前已确定方向 | 等待 codegen 与 runtime 头接口对齐 |

---

## 4. 达到工业化品质（4.5/5）的行动路线

### Phase A: Blockers 解除 (预估 2-3d)

| 行动 | 预估 | 备注 |
|------|------|------|
| 修复 SYS-001: codegen 宏对齐 | 0.5-1d | 阻塞 Phase 2 |
| 修复 BGC-RACE: SignalFullGcComplete | 1-2d | 需要 GC 工程排查 lock/signal 逻辑 |

### Phase B: 全量通过验证 (预估 2d)

| 行动 | 预估 | 备注 |
|------|------|------|
| Phase 2 逐 family 攻坚 (P0→P3) | 1.5d | 12 family, 含 HotUpdate 验证 |
| Phase 3 工业化收口 | 0.5d | 全量 dashboard, perf 对比 |

### Phase C: 基础设施补齐 (预估 2-3d)

| 行动 | 预估 | 备注 |
|------|------|------|
| CI 全量回归 | 1d | 与已有 smoke 管线衔接 |
| 管线文档 + 故障诊断 | 0.5d | |
| 管线基础设施测试 | 1d | |

### 总预估: 6-8d FT

---

## 5. 结论

**JIT hotupdate 路径**: extern "C" 重载问题已修复，entry.exe 可正常构建和运行。hotupdate 模式下验证通过 10/10 方法。

**核心阻塞**: `G_Scheduler().SignalFullGcComplete()` 在 BGC 并发下的竞态，导致 10-24% 间歇性崩溃。这是当前工业化品质从 2.65 提升到 4.5+ 的第一 blocker。

**工业化差距**: foundation-dll 整体工业化成熟度 2.65/5，距 marshal (4.8/5) 和 metadata-writer (4.5/5) 的标线需要 6-8 天 FT 工作量。主要差距在 CI/CD 回归检测（2/5）、基础设施健壮性（3/5）和测试覆盖（1/5）。

**优先推荐**:
1. **P0**: 修复 BGC-RACE（GC 工程）— 否则任何验证都不可靠
2. **P0**: 修复 SYS-001 codegen 宏对齐 — 解除 Phase 2 阻塞
3. **P1**: 全量 family 重跑验证（Phase 2/P1→P3）
4. **P2**: CI 全量回归 + 文档 + 测试
