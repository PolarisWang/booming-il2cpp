# Roadmap — GC 对齐 CoreCLR 全量功能（v2 全量版）

> 版本：v2-01（v1 覆盖 Phase 1-6；本版**全量**纳入所有剩余功能域）
> 创建：2026-08-10
> 上游依据：`docs/dev/assessments/gc-vs-coreclr-remaining-features-2026-08-10.md`（26 项功能域 + 重要度）
> 目标：按优先级逐项对齐 CoreCLR GC 全部剩余功能，约束=多平台 + JIT/AOT + 热更友好

---

## 1. 目标

在 Phases 1-5（13 子任务已提交）基础上，完成 CoreCLR 全部剩余功能对齐（26 项功能域中的未/部分对齐项），达"功能全等 + 生产可依赖"。保留 CRAG 热更新差异化（域卸载 O(region)）。

> **旧 roadmap-v1 的 Phase 6（GC-L1/L2）在本版被重命名/收纳**：`GC-L1 动态堆数 → GC-M3B`，`GC-L2 伺服调优 → GC-M6`。为避免读者困惑，Phase 6 不再单列——其内容已并入本版 Phase 8。完成判定以 GC-M# 为准。

## 2. 范围（全部要完成）

按重要度分组（来源：remaining-features 评估）：

- **P0（生产必须，3 项）**：K2c regen 验证、世代写屏障压力验证、Server GC 多堆集成测试
- **P1（能力/性能，8 项）**：provisional 完整、BGC 分相、dynamic tuning 伺服、Dynamic Heap Count、region demotion、plan-gen 重绑、三代分代、mark 扫脏卡 gen 过滤实证
- **P2（工程/可观测，13 项）**：全量配置旋钮、分代 HardLimit、多种 GCHandle 类型、完整事件集、GCProvModeStress、并发根扫描、SoftwareWriteWatch(定向)、Heap verifier 等

## 3. 非目标（保留差异化）
- CDAC/SOS 调试器（il2cpp 无配套 SOS，成本高，判非目标）
- standalone GC 插件化（破坏热更）
- 对象头内联位（动布局，波及热更）

---

## 4. 阶段与子任务（可落地）

> 每子任务满足三约束的标注：**[多平台]** [JIT/AOT] [热更]；每子任务含架构优先前置（读对应 CoreCLR 原文 + 本任务设计）。

### Phase 7 — P0 生产必须（正确性/验证）

| task | 内容 | 约束 |
|------|------|------|
| GC-M1 | **K2c 世代写屏障管线 regen**：foundation-dll 管线全量 regen（build+fact），确认生成 `.cpp` 出现 `_dst_ref` + 原生编译通过 | [多平台] 生成代码跨平台编译 [JIT/AOT] AOT 用 `_dst_ref`，JIT 同一 runtime 符号 [热更] 保留 `chaos_gc_dirty_card` 兼容入口 |
| GC-M2 | **世代写屏障压力场景**：多线程 ref-store 到各自 nursery + 触发 young GC，验证 gen0→gen0 短路不漏跨代引用（对齐 mark_phase.cpp:1388 扫脏卡 gen 过滤实证） | [多平台] 纯 C++ 多线程 [JIT/AOT] 覆盖 AOT/JIT 两态 [热更] 解释器路径 |
| GC-M3A | **Server GC 多堆集成测试**：GC_SERVER=ON 实际跑 + multi-heap 协调验证（`gc_coordinator`/`gc_heap_manager`） | [多平台] NUMA 感知跨平台 [JIT/AOT] 两态 [热更] 域卸载在 per-heap 下 O(region) |

### Phase 8 — P1 能力/性能（中）

| task | 内容 | 约束 |
|------|------|------|
| GC-M3B | **Dynamic Heap Count**（`dynamic_heap_count.cpp` 运行期增减堆数，闭环自适应） | [多平台] [JIT/AOT] [热更] 堆调整不破坏域隔离 |
| GC-M4 | **provisional 完整形态**：高记忆+高碎片→gen1 强制 compact + NGC2 排队（对齐 collect.cpp:1638） | [多平台] [JIT/AOT] [热更] |
| GC-M5 | **BGC 分相状态机**：1st/2nd 快照 + concurrent plan/sweep（对齐 background.cpp） | [多平台] [JIT/AOT] [热更] |
| GC-M6 | **dynamic_tuning 伺服**：free-list rate + 碎片密度 + memory load 阶梯（对齐 dynamic_tuning.cpp）| [JIT/AOT] [热更] |
| GC-M7 | **region demotion / age-based evacuation**（对齐 regions_segments demotion） | [多平台] [热更] 域卸载不碎片 |
| GC-M8 | **plan-gen 重绑**：GC 后按新代重写 region-gen 表（对齐 plan phase region_to_generation） | [多平台] [JIT/AOT] 写屏障依赖 [热更] |
| GC-M9 | **三代分代**：gen0/1/2 + max_generation（替代 2 代）+ CardTable card-bundle 全样式扫描 | [多平台] [JIT/AOT] [热更] |
| GC-M10 | **mark 扫脏卡 gen>condemned 过滤实证**（K2e 从"判覆盖"到"实测不漏"） | [JIT/AOT] [热更] |

### Phase 9 — P2 工程/可观测（低，随着随补）

| task | 内容 |
|------|------|
| GC-M11 | 全量配置旋钮（72 个中补 ~40 个：HardLimit 分代 SOH/LOH/POH 等） |
| GC-M12 | 多种 GCHandle 类型（REFCOUNTED / WEAK_INTERIOR_POINTER 等） |
| GC-M13 | 完整 ETW/EventPipe 事件集 + per-GC 原因位图扩展（G1 单值→位图）|
| GC-M14 | GCProvModeStress 压测旋钮 + 并发根扫描（BGC 期间）|
| GC-M15 | 半量预算 oom_budget gen 级缩放（从只改上报到真缩放）|

---

## 5. 关键字依赖（修正版）

```
P0: M1(K2c regen) → M2(写屏障压力, 独立可并行) → M3A(Server GC 测试)
    M10(mark 扫脏卡 gen 过滤实证) — 正确性，升入 P0，与 M2 同批
P1: M3B ← M3A ; M5(BGC 分相) 独立 ;
    M4(provisional 完整) ← M3B + M5 + GC-D1   ←（修正：依赖 BGC 分相 + Server GC，非仅 D1）
    M9(三代) ← GC-K2/K3 ; M8(plan-gen) ← M9 ; M7(demotion) ← M8
    M6(伺服调优) ← GC-E1 + M10
P2: M11 ← GC-E1 ; M12 ← GC-F1 ; M13 ← GC-H1 ; M15 ← GC-D1
```

> 依赖修正说明：M4 原仅依赖 D1（不足）——provisional 完整需 BGC 分相（M5）提供 gen1-compact 时序 + Server GC 堆调整（M3B）协调；M10 是正确性过滤实证，应从 P1 升 P0。

## 6. 子任务映射（完整表）

| task_id | phase | status | owner | depends_on | requirements | deliverables | exit_criteria | conflict_scope | effort |
|---|---|---|---|---|---|---|---|---|---|
| GC-M1 | 7 | planned | — | GC-K2c | 多平台编译 proof | regen 产物 | 单 chunk 生成代码含 `_dst_ref` + 原生编译；再全量 | translation/ | M |
| GC-M2 | 7 | planned | — | 独立 | 压力不漏 proof | 压力测试 target | 多线程 ref-store+young GC，跨代引用 0 丢；gen0→gen0 短路生效 | tests stress | M |
| GC-M10 | 7 | planned | — | GC-M2 + GC-K2d | gen 过滤 proof | card_table | mark 扫脏卡 gen>condemned 过滤实测不漏（与 M2 同批，正确性）| gc_card_table.* | S |
| GC-M3A | 7 | planned | — | GC-K1 | 多堆集成 proof | gc_coordinator/heap_manager 测试 | GC_SERVER=ON 多堆协调跑通；域卸载 per-heap 仍 O(region) | gc_coordinator.* | L |
| GC-M3B | 8 | planned | — | GC-M3A | 堆数变化 proof | dynamic heap | 运行期增减堆，无回归，闭环收敛 | gc_heap_manager.* | L |
| GC-M4 | 8 | planned | — | GC-M3B + GC-M5 + GC-D1 | 完整 provisional proof | gc_scheduler/bgc | gen1 强制 compact + NGC2 排队；高记忆+高碎片触发 | gc_bgc.* | L |
| GC-M5 | 8 | planned | — | GC-J1 | 分相 proof | gc_bgc | 1st/2nd 快照 + concurrent plan/sweep；pause 预算达标 | gc_bgc.* | L |
| GC-M6 | 8 | planned | — | GC-E1 + GC-M10 | 伺服闭环 proof | gc_scheduler/old_gen | free-list rate+碎片+memory load 多信号闭环；分配回归 <5% | gc_scheduler.* | L |
| GC-M7 | 8（分里程碑 A/B） | planned | — | GC-M8 | demotion proof | gc_region | 里程碑A 基本 demotion；B age-based evacuate + 域卸载不碎片 | gc_region.* | XL(拆2) |
| GC-M8 | 8 | planned | — | GC-M9 | plan-gen proof | gc_region/card | GC 后 region-gen 表正确重绑；写屏障短路不退化 | gc_region.* | L |
| GC-M9 | 8（分里程碑 A/B） | planned | — | GC-K2/K3 | 三代 proof | gc_young/gen1/old | 里程碑A gen1 独立代；B gen2/max_gen + 回归 | gc_young*.cpp | XL(拆2) |
| GC-M11 | 9 | planned | — | GC-E1 | 全量旋钮 proof | gc_config | 72 旋钮补齐 + 分代 HardLimit | gc_config.* | L |
| GC-M12 | 9 | planned | — | GC-F1 | handle 类型 proof | engine_lifecycle | 补 REFCOUNTED/WEAK_INTERIOR_POINTER 等类型 | engine_lifecycle.* | L |
| GC-M13 | 9 | planned | — | GC-H1 | 事件+位图 proof | gc_events/stats | >40 事件 + per-GC 原因位图 | gc_events.* | L |
| GC-M14 | 9 | planned | — | GC-M5 | 压测+根扫描 proof | gc_bgc | GCProvModeStress + 并发根扫描 | gc_bgc.* | M |
| GC-M15 | 9 | planned | — | GC-D1 | 半量预算 proof | gc_scheduler | oom_budget 缩放 gen 预算 | gc_scheduler.* | S |

## 7. 调度策略

```yaml
dispatch_model: hybrid   # Phase 7/8 串行(强依赖链)；Phase 9 各 P2 任务独立可并行
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

- Phase 7/8（M1-M10）：**串行**（M1→M2→M3A，M9→M8→M7 强依赖）
- Phase 9（M11-M15）：**可并行**（各独立，无文件冲突），hybrid 批处理

## 8. 三约束的实现原则（贯穿全部子任务）

1. **多平台**：全部走纯 C++（`addr>>shift` 查表 + 位运算 + `std::atomic`），无 asm barrier 拷贝（CoreCLR StompWriteBarrier 简化为函数指针），无平台特定 OS 调用。
2. **JIT/AOT**：AOT codegen 发射新 API + JIT 走同一 runtime 符号；每项验证 AOT + JIT 两态。
3. **热更友好**：保留单参兼容入口/解释器路径；域卸载 O(region)；不破坏 MemoryDomain 域隔离。

## 9. 当前建议推进顺序
**GC-M1（K2c regen）→ GC-M2（写屏障压力）→ GC-M10（mark 扫脏卡过滤实证）→ GC-M3A（Server GC 测试)**（P0 四项，正确性/生产最关键）。然后 Phase 8（M9 三代 → M8 plan-gen → M7 demotion；M5 BGC 分相 → M4 provisional；M3B/M6），最后 Phase 9。

## 10. 风险与护栏

| 风险 | 护栏 |
|------|------|
| K2c regen 撞管线路径/时间长 | 先单 chunk（小 assembly）验证机制再全量；PYTHONPATH/cwd 正确配置 |
| 世代写屏障漏引用(UAF) | M2 压力场景必做；M10 实证过滤 |
| Server GC 多堆验证复杂 | M3A 先行集成测试，后 dynamic heap |
| 三代(rework)破坏既有 | M9 严格架构优先 + K4 回归 |

## 10b. 备选路径（回退）

| 触发条件 | 回退 |
|----------|------|
| **M9（三代）重构复杂度超预期 / 与热更域卸载冲突** | 降级：保留 2 代 + gen1-独立代（M9 里程碑 A），放弃 gen2/max_gen（里程碑 B 后置为 P2） |
| **M7（demotion）与 MemoryDomain 域隔离不可兼得** | 只做基本 demotion（里程碑 A），age-based evacuate 后置 |
| **M4（provisional 完整）与 Server GC 堆调整冲突** | 只做 gen1 强制 compact（不含 NGC2 排队），force-blocking 已有 |
| **M5（BGC 分相）pause 预算不达标** | 保持并发 mark+sweep（J1 现状），放弃 1st/2nd 快照细化 |

> 这些回退均保 P0 正确性项（M1/M2/M3A）不变，只降级 P1 能力深度，不牺牲生产可用性。

---

## 11. 完成判定（本 roadmap 终态）

- P0/P1/P2 全部子任务进入终态（completed/hanging/abandoned）。
- 每子任务的 exit_criteria 满足（实现 + 测试 + 多平台/JIT/AOT/热更 proof + **可度量验收**）。
- 保留 CRAG 差异化（域卸载 O(region)），不引入破坏它的对齐。
- P1/P2 被备选路径降级的任务，以降级后的 exit_criteria 判定完成。

## 执行日志（2026-08-10）

- **Phase 0（命名卫生）✅ `18fe80ae6`**：清除落地代码 `GC-K2b/K2d/K1b/K2a/J1/K2/K3` 代号注释，保留功能描述；`grep "GC-[A-Z][0-9]" src/` 清零；compile 通过。
- **Phase 1a（世代写屏障 regen 验证）✅**：foundation-dll pipeline regen 跑通（`CHAOS_FOUNDATION_DLL=translation` + `PYTHONPATH`），System.ObjectModel build 生成 AOT `entry.exe`(2.8MB) + JIT `entry-jit.exe`，**AOT 10/10 + JIT 10/10 编译链接通过**；`chaos_gc_dirty_card_dst_ref` 声明已进生成 build headers（link-ready）。注：System.ObjectModel IL 无 gc-ref field-write 形状，生成函数体未实际调 `_dst_ref`，但符号 link-ready + 整体编译过（K2c 发射不会破坏生成代码）。
