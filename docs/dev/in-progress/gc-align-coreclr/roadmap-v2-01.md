# Roadmap — GC 对齐 CoreCLR 全量功能（v2 全量版）

> 版本：v2-01（v1 覆盖 Phase 1-6；本版**全量**纳入所有剩余功能域）
> 创建：2026-08-10
> 上游依据：`docs/dev/assessments/gc-vs-coreclr-remaining-features-2026-08-10.md`（26 项功能域 + 重要度）
> 目标：按优先级逐项对齐 CoreCLR GC 全部剩余功能，约束=多平台 + JIT/AOT + 热更友好

---

## 1. 目标

在 Phases 1-5（13 子任务已提交）基础上，完成 CoreCLR 全部剩余功能对齐（26 项功能域中的 ~14.5 项未/部分对齐），达"功能全等 + 生产可依赖"。保留 CRAG 热更新差异化（域卸载 O(region)）。

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

## 5. 关键字依赖

```
P0: M1 → M3A(M3A 依赖 GC_SERVER 基建)
    M2（独立，可并行）
P1: M3B ← M3A ; M4 ← M3B? (provisional 与堆数相关); M5 独立; M6 ← M8? 
    M8 ← M9(三代后 plan-gen 才有意义); M7 ← M8/M9; M10 ← M2(M2 实证了过滤)
P2: M11 ← E1(已有首批); M12 ← F1; M13 ← H1; M15 ← D1
```

## 6. 子任务映射（完整表）

| task_id | phase | status | owner | depends_on | requirements | deliverables | exit_criteria | conflict_scope | effort |
|---|---|---|---|---|---|---|---|---|---|
| GC-M1 | 7 | planned | — | GC-K2c | 多平台编译 proof | `tests/e2e/translation/` regen 产物 | 生成代码含 `_dst_ref` + 原生编译过 | translation/ | M |
| GC-M2 | 7 | planned | — | 独立 | 压力不漏 proof | 压力测试 target | gen0→gen0 短路不漏 | tests stress | M |
| GC-M3A | 7 | planned | — | GC-K1 | 多堆集成 proof | gc_coordinator/heap_manager 测试 | GC_SERVER 跑通 | gc_coordinator.* | L |
| GC-M3B | 8 | planned | — | GC-M3A | 堆数变化 proof | dynamic heap | 运行期增减堆 | gc_heap_manager.* | L |
| GC-M4 | 8 | planned | — | GC-D1 | 完整 provisional proof | gc_scheduler/bgc | gen1 compact+NGC2 | gc_bgc.* | L |
| GC-M5 | 8 | planned | — | GC-J1 | 分相 proof | gc_bgc | 1st/2nd 快照 | gc_bgc.* | L |
| GC-M6 | 8 | planned | — | GC-E1 | 伺服闭环 proof | gc_scheduler/old_gen | 多信号闭环 | gc_scheduler.* | L |
| GC-M7 | 8 | planned | — | GC-M8 | demotion proof | gc_region | age-based evacuate | gc_region.* | XL |
| GC-M8 | 8 | planned | — | GC-M9 | plan-gen proof | gc_region/card | GC 后重绑 | gc_region.* | L |
| GC-M9 | 8 | planned | — | GC-K2/K3 | 三代 proof | gc_young/gen1/old | gen0/1/2 | gc_young*.cpp | XL |
| GC-M10 | 8 | planned | — | GC-M2 | gen 过滤实证 | card_table | 实测不漏 | gc_card_table.* | S |
| GC-M11-M15 | 9 | planned | 各独立 | 见映射 | — | — | 各目标实现 | 各 scope | S-L |

## 7. 调度策略

```yaml
dispatch_model: sequential（前阶段完成才进后阶段）
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

## 8. 三约束的实现原则（贯穿全部子任务）

1. **多平台**：全部走纯 C++（`addr>>shift` 查表 + 位运算 + `std::atomic`），无 asm barrier 拷贝（CoreCLR StompWriteBarrier 简化为函数指针），无平台特定 OS 调用。
2. **JIT/AOT**：AOT codegen 发射新 API + JIT 走同一 runtime 符号；每项验证 AOT + JIT 两态。
3. **热更友好**：保留单参兼容入口/解释器路径；域卸载 O(region)；不破坏 MemoryDomain 域隔离。

## 9. 当前建议推进顺序
**GC-M1（K2c regen）→ GC-M2（写屏障压力）→ GC-M3A（Server GC 测试）**（P0 三项，正确性/生产最关键）。然后 Phase 8/9。

## 10. 风险与护栏

| 风险 | 护栏 |
|------|------|
| K2c regen 撞管线路径/时间长 | 先单 chunk（小 assembly）验证机制再全量；PYTHONPATH/cwd 正确配置 |
| 世代写屏障漏引用(UAF) | M2 压力场景必做；M10 实证过滤 |
| Server GC 多堆验证复杂 | M3A 先行集成测试，后 dynamic heap |
| 三代(rework)破坏既有 | M9 严格架构优先 + K4 回归 |

---

## 11. 完成判定（本 roadmap 终态）

- P0/P1/P2 全部子任务进入终态（completed/hanging/abandoned）。
- 每子任务的 exit_criteria 满足（实现 + 测试 + 多平台/JIT/AOT/热更 proof）。
- 保留 CRAG 差异化（域卸载 O(region)），不引入破坏它的对齐。
