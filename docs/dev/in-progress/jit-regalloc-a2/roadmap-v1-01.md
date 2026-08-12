# Chaos IL2CPP — JIT 寄存器分配 A-2 可执行计划（Roadmap v1-01）

> **状态**：待用户确认排版后启动执行
> **创建**：2026-08-10
> **上游**：`docs/dev/assessments/jit-regalloc-a2-continuation-roadmap.md`（遗留缺口清单）
> **关联记忆**：`[[jit-engine-modularization]]`、`[[jit-regalloc-a2-continuation-roadmap]]`
> **调度模式**：`hybrid`（JIT 主线串行 + 能力线并行批次）

---

## 1. 目标

把 JIT 寄存器分配 A-2 延续文档中的**全部遗留未闭合任务**转化为可落地的执行计划，并按
全局优先级（性能最优 > 方案完美性 > HotUpdate 支持）完成实现与验收。

最终验收总定义（继承上游文档）：
- **全 `ctest -L jit` 100% 通过**（当前 < 100%，因 §1.1/§4.1 未闭合）
- **`codegen_native_test` / `test_jit_native` 保持 18/18、61/61**（回归护栏）
- 图着色默认化后，`test_jit_native` 寄存器相关断言 + 性能数据证明**无栈往返**

## 2. 范围边界（in scope）

| 任务簇 | 内容 | 域 |
|---|---|---|
| P0 | §0 已交付成果固化（ABI thunk + 测试恢复） | JIT |
| P1 | §1.1 T4 kQuick 发射 bug 修复 | JIT |
| P2 | §2 A-2 本体：图着色默认化 + 精确 GC + spill 契约 + monolith 拆分 | JIT |
| P3 | §4 系统性技术债：测试群恢复、L11 清理、孤儿检查器 | JIT/构建 |
| P4 | §3 全局能力缺口：benchmark 可信度、消除中间层、翻译语义、数据治理 | JIT/benchmark/翻译/数据 |

## 3. 非目标（out of scope）

- **AOT codegen**（C# 翻译管线，`src/managed/Chaos.IL2CPP.Generator/`）——超出 JIT Expert 域
- **Interpreter VM**（T1/T2：FastExecute/RegisterExecute）内部的寄存器分配——外域
- **ARM64 JIT 完整实现**——外域
- import/export-header、GC 分配写屏障等无关线——不纳入本计划

## 4. 已对齐决策（用户拍板）

| # | 决策 | 结论 | 理由 |
|---|---|---|---|
| D1 | §0 成果固化方式 | **就地提交到当前分支** `fix/sdk-build-path-authority-converge` | 用户选省事优先。⚠️ 风险：JIT 成果混入 SDK 分支 diff，后续拆 PR 需 cherry-pick，如实记录 |
| D2 | §3 全局能力缺口 | **全部纳入同一条计划** | 覆盖全面，跨域并行 |
| D3 | Task 2.4 monolith 拆分时机 | **先拆再改**（架构优先） | 改动受控、可单测；符合 CLAUDE.md P2 架构一致性，且为 2.1 图着色改动提供受控边界 |
| D4 | P4 跨域启动时机 | **先落成完整计划文档供排版，确认后再 Workflow 并行启动** | 对齐优先，避免未确认方案返工 |
| D5 | 图着色默认化范围 | **A1 全量默认化**，A2（tier 分梯队）作为数据触发的备选兜底 | 架构完美 + 热更统一帧布局 + 单一验收口径；冷方法风险用 C1 benchmark 实测，若冷方法图着色开销≥15% 热点收益才触发降级 A2 |
| D6 | P4 能力线执行方式 | **B1 并行批次组织，但不用 worktree**（同一终端/仓库内按 DISPATCH 批次纪律推进 C 线） | C2 文件面在 `src/managed/Chaos.IL2CPP.Generator/` 不碰 JIT 主线 `src/native/jit/`，无冲突；C1 benchmark 是 J2 图着色验收证据源须领先完成；跨域 C2/C3 用 Agent 子代理注入 expert；不用 worktree 避免隔离开销 |
| D7 | P0 提交后是否拆 PR | **C1 直接混在 SDK 分支到底，不 cherry-pick 拆分** | 该分支为个人聚合分支（不做公共 JIT PR），就地提交即达。仍保留"每阶段 commit 自包含"纪律（与拆不拆无关，对任何追溯有利） |

> **优先级裁决**：D3（先拆再改）在"性能最优先"与"架构一致性"之间选择架构优先。
> 裁决理由：monolith 6229 行单类上直接改图着色（regalloc 核心）风险高且无法单测，
> 拆出独立模块后图着色改动可受控验证。性能目标（无栈往返）不变，只是验收次序后移，
> 不构成"故意破坏 P1 性能优先级"。

## 5. 阶段列表（hybrid 双线）

```
JIT 主线（串行顺序推进）          能力线（P4, 文档确认后排并行批次）
┌──────────────────────────┐    ┌──────────────────────────┐
│ Phase J0  §0 固化成果      │    │
│ Phase J1  §1.1 kQuick bug │    │ Phase C1  §3.2 benchmark  │
│ Phase J2  §2 A-2本体      │ →  │ Phase C2  §3.1 消除中间层  │
│   ├ 2.4 拆 monolith      │    │ Phase C3  §3.3 翻译语义    │
│   ├ 2.1 图着色默认化      │    │ Phase C4  §3.4 数据治理    │
│   ├ 2.2 精确 GC          │    │      （跨域，需 Workflow）  │
│   └ 2.3 spill 契约+热更   │    └──────────────────────────┘
│ Phase J3  §4 技术债       │
└──────────────────────────┘
```

**双线汇合点**：Phase C1（benchmark 可信度）是 Phase J2 图着色"无栈往返"性能验收的证据来源，
两条线必须并行而非串行——这是 hybrid 模式的核心依据。

## 6. 阶段完成定义

### Phase J0 — §0 成果固化

- `goal`：把已验证的 ABI thunk + JIT 测试恢复成果固化提交，防丢
- `exit_criteria`：当前分支干净；`ctest -L jit` 基线记录在当前 commit
- `deliverables`：一个 commit（就地提交到 `fix/sdk-build-path-authority-converge`）
- `dependencies`：无
- `resolved_decisions`：D1（就地提交）
- `watch_items`：分支名带 SDK 语义，后续若需拆 PR 要 cherry-pick

### Phase J1 — §1.1 kQuick 发射 bug

- `goal`：修复 T4 `enable_optimizer=false` / `compile_tier=kQuick` 发射返回 0 的正确性缺陷
- `exit_criteria`：`PgoTest.Tier0CodeWorks` 期望 42 返回 42；`test_jit_pgo` 全过
- `deliverables`：`jit_engine.cpp` kQuick/tier0 发射分支修复 + 测试转绿
- `dependencies`：无（可与 J0 并行准备）
- `resolved_decisions`：kQuick 保持纯栈式设计（`jit_engine.h:34`），不引入寄存器分配
- `watch_items`：kQuick 返回值从栈槽捞回寄存器是否与 §2.1 的 write-through 同源——修复时避免与后续图着色改动冲突

### Phase J2 — §2 A-2 本体

- `goal`：图着色寄存器分配**全量默认化** + 精确 GC slot maps + spill 契约权威化 + monolith 拆分
- `exit_criteria`：图着色接入 T4 发射主路径（全量，D5），`test_jit_native` 寄存器断言通过无栈往返；精确 GC 只扫存活引用；spill 契约文档 + 热更重编译后 GC map 一致
- `deliverables`：regalloc 模块化拆分 + 图着色全量接入 + 精确 slot map + 契约文档 + 热更回归测试
- `dependencies`：Phase J1（依赖 T4 发射正确）；Phase C1（性能验收证据）
- `resolved_decisions`：D3（先拆再改）；D5（A1 全量默认化，A2 数据兜底）；图着色独用 T4（`jit_reg_alloc.h` Chaitin-Briggs），不牵动 T2/T3 Interpreter 的 Linear Scan
- `watch_items`：`static_assert(kFrameSize <= 4096)` 是否符合（图着色扩大寄存器可能改变帧布局，监控不踩上限）；C1 benchmark 监控冷方法图着色编译开销是否 ≥15% 热点收益（触发 A2 降级）

### Phase J3 — §4 系统性技术债

- `goal`：恢复全部 `test_jit_*` + 清理 L11 残留 + 建孤儿检查器防复发
- `exit_criteria`：`ctest -L jit` 100%通过；无指向已删源的 stale 产物；孤儿测试检查器接入 CI
- `deliverables`：测试群恢复 + stale 产物清理 + `check_workarounds.py` 孤儿门禁
- `dependencies`：Phase J0
- `resolved_decisions`：无新增分叉
- `watch_items`：逐一确认 `jit_abi_test`/`jit_gc_slot_map_test`/`jit_osr_test`/`jit_unwind_test`/`jit_seh_*`/`jit_inline_test` 的 ABI

### Phase C1..C4 — P4 全局能力缺口（跨域，需 Workflow）

- `goal`：benchmark 可信度 → 消除中间层 dispatch → 翻译语义缺口 → 覆盖盲区/数据治理
- `exit_criteria`：benchmark 无 ns/alloc=0/fast-path=1.0 伪影；中间层 dispatch 消除；GVM/Span/vararg/tail 语义补齐；aggregate 均/中位口径统一
- `deliverables`：按子任务映射
- `dependencies`：C1 是 C2 前提 & J2 性能验收证据；C1 无依赖
- `resolved_decisions`：D2（全纳入）；D4（文档确认后 Workflow 并行启动）
- `watch_items`：C3 翻译语义是 `other-family`，执行走翻译 expert，不阻塞 JIT 主线

## 7. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| T0.1 | J0 | ready | JIT | §0 成果固化提交 | — | batch-j0 | commit含三段元数据 | 1 commit (t4_jit_call.asm+测试恢复) | 分支干净, ctest基线记录 | `src/native/jit/`, `tests/unit/runtime-native/jit/`, `tests/contracts/native/runtime-core/` | 0.5d |
| T1.1 | J1 | ready | JIT | 修 kQuick 发射返回0 | — | batch-j1 | 正确性修复 | jit_engine.cpp kQuick分支 | Tier0CodeWorks=42, pgo全过 | `src/native/jit/`, `tests/unit/runtime-native/jit/jit_pgo_test.cpp` | 1.5d |
| T2.4 | J2 | planned | JIT | monolith 8模块拆分 | T0.1 | batch-j2 | 架构拆分 | 8独立模块 (regalloc/gc/unwind/emit/tier/osr/seh/encoding) | 类职责单一, 原测试全过 | `src/native/jit/` | 2.5d |
| T2.1 | J2 | planned | JIT | 图着色默认化 | T2.4, T1.1 | batch-j2 | 图着色接入T4主发射路径 | regalloc图着色接入+适配层 | 寄存器常驻无栈往返, test_jit_native断言过 | `src/native/jit/` | 4d |
| T2.2 | J2 | planned | JIT | 精确 GC slot maps | T2.1 | batch-j2 | liveness→精确物理寄存器掩码+溢出栈槽 | GcSlotMap精确生成 | GC只扫存活引用, gc_slot_map_test过 | `src/native/jit/`, `tests/unit/runtime-native/jit/jit_gc_slot_map_test.cpp` | 3d |
| T2.3 | J2 | planned | JIT | spill契约+热更一致性 | T2.2 | batch-j2 | GcSlotMapV0序列化+deopt同offset语义 | 契约文档+热更回归测试 | 热更重编译后GC map一致 | `src/native/jit/`, `wiki/` | 2d |
| T3.1 | J3 | planned | JIT | 测试群ABI补全 | T0.1 | batch-j3 | 全ctest -L jit建基线+逐一确认ABI | 各test_jit_*恢复 | ctest -L jit 100% | `tests/unit/runtime-native/jit/` | 2d |
| T3.2 | J3 | planned | JIT/构建 | L11 stale产物清理 | T0.1 | batch-j3 | 删artifacts/presets/testing陈旧vcxproj | CMake干净重生成 | 无stale产物, 构建干净 | `artifacts/`, `tests/` | 1d |
| T3.3 | J3 | planned | JIT/构建 | 孤儿测试检查器 | T3.1 | batch-j3 | check_workarounds.py+API引用检查 | 孤儿门禁脚本 | CI检测孤儿引用 | `tests/e2e/verification/preflight/` | 1.5d |
| T4.1(C1) | C1 | planned | benchmark | benchmark harness 可信度 | — | batch-c1 | 修ns/alloc=0/fast-path=1.0伪影 | benchmark harness修复+口径统一 | 无伪影, mean/median统一 | `tests/`, `tools/*/benchmark*` | 2d |
| T4.2(C2) | C2 | planned | benchmark | 消除中间层dispatch | T4.1 | batch-c2 | 优化kChaosExternalRuntimeFnTable绕行+intrinsic | 中间层dispatch消除 | 核心路径性能跃升 | `src/native/runtime/`, `src/native/jit/` | 3.5d |
| T4.3(C3) | C3 | planned | 翻译 | 翻译语义缺口(GVM/Span/vararg/tail) | — | batch-c3 | 翻译语义补齐 | GVM/Span/vararg/tail支持 | 语义缺口闭合 | `src/managed/`, `src/tools/` | 5d |
| T4.4(C4) | C4 | planned | 数据 | 覆盖盲区+数据治理 | — | batch-c4 | CoreLib语义方法覆盖+均/中位口径+audit台账 | 覆盖补齐+口径统一 | 无覆盖盲区, 台账新 | `tests/`, `docs/` | 2d |

> `P4 全部纳入`（D2）→ 子任务跨 benchmark/翻译/数据域。
> C1/C2 同 `batch-c1/c2`，其中 C3 翻译域执行走翻译 expert（`other-family`），不阻塞 JIT 主线。

## 8. 依赖

```
T0.1 ──────► T1.1 ─► T2.4 ─► T2.1 ─► T2.2 ─► T2.3   (JIT 主线串行)
  │                                      ▲
  │                                ┌─────┴─────┐
  ▼                                │  T3.1      │
T3.1/T3.2/T3.3 (J3)                │  (ctest基线) │
                                   │  T4.1(C1)──┘  ► T4.2(C2)  (能力线, 依赖C1)
                                   └── C1 同时是 J2 性能验收证据 ──┘
T4.3(C3) T4.4(C4)  独立, 并行
```

## 9. 风险

| 风险 | 等级 | 缓解 |
|---|---|---|
| 图着色改变帧布局 → 踩 `static_assert(kFrameSize<=4096)` 12位编码上限 | 中 | Task 2.3 显式验证；图着色通常减小帧（少spill），但需兜底 |
| §0 成果混入 SDK 分支难拆 PR | 低-中 → **已接受(D7)** | D1/D7 明确就地到底，不做公共 JIT PR，无拆分待办 |
| kQuick 与图着色 write-through 同源病灶 → 修复冲突 | 低 | Task 1.1 修复时不引入寄存器分配，守住 kQuick 纯栈式边界 |
| monolith 拆分回归现有测试 | 中 | 先拆再改（D3）→ 每步模块可单测，原测试做护栏 |
| C3 翻译语义横跨翻译域 → 阻塞 JIT 主线 | 低 | 标记 `other-family`，独立并行，不阻塞 |
| benchmark 伪影掩盖真实性能对比 | 高 | C1 先行，作为 J2 验收证据的前置 |

## 10. 备选路径

- **Task 2.1 图着色默认化范围**（D5 已定 A1 全量）：若 C1 benchmark 实测冷方法图着色开销 ≥ 热点方法收益的 15%，降级回 **A2 按 tier 分梯队**（热方法图着色，冷方法保留 write-through）——数据触发，不预设。降级会扩大热更一致性验证面（跨 tier 帧切换），需在 §9 风险中追加。
- **P4 若 Workflow 并行成本过高**：C3/C4 降级为后续单独立项，优先 C1/C2（性能证据链）。

## 11. 当前建议推进顺序

1. **J0**（T0.1 提交固化，防丢）
2. **J1**（T1.1 修 kQuick 正确性）
3. **J2**（T2.4 先拆 → T2.1 图着色 → T2.2 精确GC → T2.3 契约+热更）
4. **J3**（T3.1/3.2/3.3 技术债）
5. **C 线**（C1 benchmark → C2 中间层；C3/C4 并行）——与 J2 并行推进，C1 供 J2 验收证据

## 12. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child`：J 线 = `T0.1`（无前置，ready）；C 线 = `T4.1`（无前置，ready）
- 每子任务自动启动前做最小 `preflight self-review`（dev-executing-plans 定义）

## 13. 调度策略（DISPATCH.md）

见 [`DISPATCH.md`](./DISPATCH.md) —— `dispatch_model: hybrid`，J0-J3 串行、C1-C4 并行批次。

---

## 附：计划决策锁定记录

全部方案决策已对齐拍板，无剩余待决项：

- **D1** §0 就地提交到 `fix/sdk-build-path-authority-converge`
- **D2** §3 全局能力缺口全部纳入同一计划
- **D3** Task 2.4 monolith **先拆再改**（架构优先）
- **D4** P4 先出文档确认后再启动
- **D5**（决策 A）图着色 **A1 全量默认化**，A2 数据兜底
- **D6**（决策 B）P4 **B1 并行批次，不用 worktree**
- **D7**（决策 C）JIT 成果 **C1 直接混在 SDK 分支到底，不拆 PR**

> ⚠️ D7 与 D1 叠加：分支为个人聚合分支，§0 就地提交即达终局，无需 cherry-pick 兜底。§9 风险表中"D1 混入 SDK 分支难拆 PR"一项因 D7 已接受，降级为"已接受，无待办"。
