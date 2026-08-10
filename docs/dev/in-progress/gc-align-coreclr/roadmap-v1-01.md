# Roadmap — GC 代码级对齐 CoreCLR

> 版本：v1-01
> 创建：2026-08-10
> 目标读者：GC 域 + 全平台适配域 + AOT/JIT 域
> 上游依据：`docs/dev/assessments/memory-allocation-risk-assessment.md`、`wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md`、`wiki/03-功能模块/08-GC子系统/CRAG-GC-Phase3-并发能力.md`、`docs/dev/designs/c-phase-region-aware-generational-gc.md`、`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`（对标基准）
>
> 决策输入（用户 2026-08-10 拍板）：
> - 对齐深度 = **P0 + P1 + 区域化重构**（不只是补洞，对齐 CoreCLR region-based 内存模型）
> - 纪律 = **遵循架构优先开发**（每个子任务开工前先查架构表，确认与既有 CRAG 架构一致再动码）
> - 形态 = **正式 roadmap 文档**（跨会话、可由 executing-plans 接手）
> - 保留 **CRAG 热更新差异化**（MemoryDomain + 域卸载 = O(region) 批量释放），不做会破坏它的对齐

---

## 1. 目标

将 CRAG GC（`src/native/runtime-core/gc/`）在**不影响 il2cpp 热更新差异化能力**的前提下，代码级对齐 CoreCLR GC（`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`）的**正确性、工业化韧性、可调性、可观测性**四大维度，并逐步收敛内存模型到 CoreCLR 的 **region-based 区域化**模式。

对齐的最终判定：每子任务产出可运行、可验证、不破坏既有 24 项 CRAG 完成度矩阵的硬化结果。

---

## 2. 范围边界（做什么）

| # | 主题 | 对标 CoreCLR 基准 | 现状(CRAG) |
|---|------|------------------|-----------|
| A | **正确性根集修复** | full root set 扫描（全线程栈/寄存器） | 🔴 young GC 只扫触发线程栈（`gc_young_collector.cpp:418-447`） |
| B | **压测接线** | `CHAOS_GC_STRESS` env 真正生效 | 🔴 `InitFromEnv()` 从未被调用 |
| C | **声明式 mark 终止** | per-worker flag + join 屏障（`mark_phase.cpp:3107`） | 🔴 last-idle-worker 拍板（有边界竞态） |
| D | **OOM 逐级降级链** | 半量→provisional→逐页 decommit→OOM（`allocation.cpp:2055`） | 🔴 直接 fail，无降级链 |
| E | **配置旋钮体系** | 72 个 INT/STRING_CONFIG（`gcconfig.h`） | 🔴 硬编码 + 1 个 env |
| F | **handle 代剪枝扫描** | clump 代字节 + 按代剪枝（`handletablepriv.h:51`） | 🔴 全表遍历 `unordered_dense_map` |
| G | **Dependent 运行期收敛** | 布尔短路 do-while（`objecthandle.cpp:1203`） | 🟠 固定 3 轮 `kMaxFixedPointRounds` |
| H | **完整 GC 事件 + 触发原因** | >40 事件 + per-GC 原因位图（`gcevents.h`/`gcrecord.h`） | 🟠 8 事件，无原因追踪 |
| I | **写屏障代感知 + 记账层级** | card bundle + region→gen 映射（`gcpriv.h:4362`） | 🟠 两级卡表，全记 |
| J | **BGC 并发 sweep + 分相状态机** | 并发 mark+sweep+STW plan（`background.cpp`） | 🟠 并发 mark，串行 sweep |
| K | **区域化内存重构** | region-based allocation（`region_allocator.cpp`/`regions_segments.cpp`） | 🟠 固定 segment/TLAB，无 region 重绑 |
| L | **动态堆数 + 碎片/压力伺服** | dynamic_heap_count + dynamic_tuning | 🟠 EMA 时域，无堆数自适应 |

---

## 3. 非目标（刻意不做）

按 il2cpp 热更新场景 ROI 判定不做，**保留 CRAG 差异化**：

| 项 | CoreCLR 基准 | 不做原因 |
|----|-------------|---------|
| SoftwareWriteWatch 全程脏页 | `softwarewritewatch.h` | AOT 全静态 + NativeAOT 专用，热更新游戏场景 ROI 低 |
| standalone GC 插件加载 | `GCName`/`GCPath` | CRAG 与 runtime 深度耦合热更新，插件化破坏域卸载 |
| CDAC/SOS 调试器扩展 | `datadescriptor.inc` | il2cpp 无配套 SOS，需自建调试器成本高 |
| 内联对象头 hash/finalizer 位 | `vm/syncblk.h` | 会动对象布局，波及 codegen/解释器/热更新 metadata |
| 从零移植 `plan_phase.cpp` 全量 compact | — | CRAG 已有 parallel compact，重构代价>收益 |
| GCRegionSize 精细自适应 | `interface.cpp:455` | 依赖 4/2/1MB 动态择区，等 K 阶段后按需 |

---

## 4. 阶段列表

按「先正确性 → 再工业化韧性 → 再可调性/可观测性 → 最后区域化重构」序推进。每个阶段内部子任务遵循**架构优先**（开工前读对应架构文档）。

### Phase 1 — 正确性堵洞（P0）

**裁决理由**（全局优先级 P1 方案完美性 > P3 HotUpdate）：这三项是**潜在的 use-after-free / 丢对象 / 压测失真**，属正确性红线，必须先于一切。不破坏热更新差异化。

| 子任务 | 内容 | 完成定义 |
|--------|------|---------|
| GC-A1 | young GC 全根集扫描：STW 下扫全部线程栈+寄存器（修复 nursery 低于 g_heap_base 的过滤问题） | 跨线程引用存活测试通过；无 UAF |
| GC-B1 | 移除 GC 核心 `CHAOS_GC_STRESS`：删热路径 8 处 `GcStressShouldTrigger()` + `gc_stress.h/cpp` + CMake option（对齐 CoreCLR「GC 核心零 stress」）+ 补直连 `gc_helpers.h` | 核心热路径无 stress；`chaos_runtime_core` 编译通过；分配路径 GC 测试 0 失败 |
| GC-C1 | 声明式 mark 终止：每 worker 独立完成 flag + join 屏障（对齐 CoreCLR 终止协议） | 高并发 stress 无终止竞态 |

### Phase 2 — 工业化韧性（P1，崩溃抗性 + 可调性基础设施）

**裁决理由**：OOM 降级链是生产崩溃韧性刚需；配置旋钮摆脱硬编码重编译；这两者 ROI 最高且不重构内存模型。

| 子任务 | 内容 | 完成定义 |
|--------|------|---------|
| GC-D1 | OOM 逐级降级链：半量预算 → provisional mode → 逐页 decommit → OOM | 内存耗尽时逐级降级而非直接 fail；对齐 `allocation.cpp:2055` |
| GC-E1 | 配置旋钮体系（首批 ~20 个）：`<int/str>_config` 宏 + env 注入 + 运行时 setter | 核心参数（nursery/阈值/hardlimit/concurrent）可用 env 调，无需重编译 |

### Phase 3 — 可观测性与 handle 硬化（P1，诊断 + 规模化）

| 子任务 | 内容 | 完成定义 |
|--------|------|---------|
| GC-H1 | 完整 GC 事件 + 触发原因位图（对齐 `gcevents.h`/`gcrecord.h`） | 事件数 >40；per-GC 原因可追踪；GCHandle 生命周期事件 |
| GC-F1 | handle 表分代剪枝：clump 代字节 + 按代扫描（对齐 `handletablepriv.h`） | 只扫 condemned 代相关 handle；BGC 并发按 dirty clump 增量重扫 |
| GC-G1 | Dependent 运行期收敛：去 3 轮硬上限，改 do-while 布尔短路 | 深层 Ephemeron 链(>3) secondary 不丢 |

### Phase 4 — 并发能力强化（P1，BGC 成熟化）

| 子任务 | 内容 | 完成定义 |
|--------|------|---------|
| GC-J1 | BGC 并发 sweep + 分相状态机（1st/2nd 快照 + 并发子阶段，对齐 `background.cpp`） | BGC 并发 mark+sweep；专用后台线程；finalizer/weak 两阶段模型 |

### Phase 5 — 区域化内存重构（核心重构）

> ⚠️ **最高风险阶段**。必须严格架构优先，以 `c-phase-region-aware-generational-gc.md` 为上游设计基线。**关键护栏**：分配入口 `CHAOS_IL2CPP_NEW_GC → GcAllocate → NurseryAllocate` 是稳定收敛点，region 重构**内部切换 GcAllocate 实现，codegen 零改动**（wiki 17 号明确支持），且**不破坏跨域分配约束**（GC/Domain/Raw 三域边界、禁止全局 operator new）。

| 子任务 | 内容 | 完成定义 |
|--------|------|---------|
| GC-K1 | Region 框架引入：`RegionAllocator` + region 大小自适应(4/2/1MB)，大 region 给 UOH | region 分配路径跑通；对齐 `region_allocator.cpp` |
| GC-K2 | 世代感知写屏障：region→gen 映射，gen0→gen0 不设卡 | 写屏障代感知；对齐 `gcpriv.h:4362`；card bundle 层级 |
| GC-K3 | 分代决策区化：region 整体重绑 generation 替代逐对象搬移 | young/promote 走 region 绑定；对齐 `regions_segments.cpp` |
| GC-K4 | 区域化回归：全部 GC 单元 + stress + 热更新域卸载测试 | 24 项完成度矩阵无回归；域卸载仍 O(region) |

### Phase 6 — 自适应增强（P2，性能天花板）

| 子任务 | 内容 | 完成定义 |
|--------|------|---------|
| GC-L1 | 动态堆数（对齐 `dynamic_heap_count`，结合 NUMA per-node heap 已有基础） | 运行期增减 worker/堆数，受 GCHeapCount/MaxHeapCount env 约束 |
| GC-L2 | 碎片/内存压力伺服调优（对齐 `dynamic_tuning`：free-list rate + 碎片密度 + memory load 阶梯） | 多信号闭环调晋升/压缩阈值 |

---

## 5. 关键字依赖关系

```
Phase1: A1 ─┬─> C1
            └─> B1 ──>（验证 A1 用）
Phase2: D1（独立）  E1（独立）
Phase3: F1 <─ G1 <─ H1（H1 提供观测，G1/F1 硬化）
Phase4: J1
Phase5: K1 ─> K2 ─> K3 ─> K4（严格串行，强依赖）
Phase6: L1 <─ L2（依赖 K 区域化完成 + E1 旋钮 + D1 降级）
```

- 区域化重构（K）完成后，**L1/L2 自适应才能在其上叠加**——这就是动态堆数/伺服调优的依赖前置于 K 的原因。
- E1（旋钮）是 L1（动态堆数需 GCHeapCount env）的前置。

---

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| GC-A1 | 1 | planned | — | 修 young GC 根集不完整(UAF) | — | — | 跨线程引用存活 proof + hotupdate proof | `gc_young_collector.cpp` | 全根集扫描；stress 无丢对象 | `src/native/runtime-core/gc/` | M |
| GC-B1 | 1 | completed | — | 移除 GC 核心 CHAOS_GC_STRESS 测试宏（对齐 CoreCLR 零 stress） | — | — | 编译 + 分配路径测试 proof | `gc_alloc_stubs.*`/`gc_region.cpp`/删 `gc_stress.*`/CMake | 核心无 stress，0 失败 | `src/native/runtime-core/gc/`+`core/` | S |
| GC-C1 | 1 | completed | — | 声明式 mark 终止（收敛复查硬化） | GC-A1 | — | 高并发 stress proof | `gc_parallel_mark.cpp` + `gc-c1-mark-termination/` | 无终止竞态 + 证明收敛 | `gc_parallel_mark.*` | M |
| GC-D1 | 2 | completed | — | OOM 降级链：半量预算+归因+provisional force-blocking+压力 decommit | — | — | 内存耗尽降级 proof | `gc_api.cpp`+`gc_scheduler.*`+`gc_old_gen.cpp` (`gc_d1-oom/`) | 四级降级+半量+归因+压力强制blocking+decommit | `gc_api.*`,`gc_scheduler.*`,`gc_old_gen.*` | M |
| GC-E1 | 2 | completed | — | 配置旋钮体系(env + native API) | — | — | env 调参 proof | `gc_config.h/.cpp` + `InitYoungGeneration` | env 生效 + 0 回归 | `src/native/runtime-core/gc/` | M |
| GC-F1 | 3 | completed | — | handle 分代剪枝（per-handle nursery bit 短路） | GC-E1 | — | 按代扫描 proof | `engine_lifecycle.*` (`gc-f1-handle-gen/`) | 扫描免 region 查询，0 回归 | `engine_lifecycle.*` | L |
| GC-G1 | 3 | completed | — | Dependent 运行期收敛（去 3 轮 cap，深链不丢） | — | — | 深链 Ephemeron proof | `engine_lifecycle.cpp` (`gc-g1-dependent/`) | 运行期收敛 + 0 回归 | `engine_lifecycle.*` | S |
| GC-H1 | 3 | completed | — | 触发原因追踪（GcTriggerReason + stats 暴露） | GC-E1 | — | 事件 proof | `gc_scheduler.*`+`gc_stats.*` (`gc-h1-events/`) | 每GC原因可追踪，0 回归 | `gc_scheduler.*`+`gc_stats.*` | L |
| GC-J1 | 4 | completed | — | BGC 并发 sweep（GcWorkerPool 并行，disjoint 页） | GC-C1 | — | BGC 并发 proof | `gc_old_gen.cpp` (`gc-j1-bgc-sweep/`) | 无 hang + 0 回归 | `gc_old_gen.*` | L |
| GC-K1 | 5 | planned | — | Region 框架引入 | GC-E1 | — | region 分配 proof | `gc_region.*` 新 region_allocator | region 路径跑通 | `gc_region.*` | XL |
| GC-K2 | 5 | planned | — | 世代感知写屏障 + card bundle | GC-K1 | — | 写屏障代感知 proof | `gc_card_table.*`/codegen | gen0→gen0 不设卡 | `gc_card_table.*` | L |
| GC-K3 | 5 | planned | — | 分代决策区化重绑 | GC-K2 | — | region 重绑 proof | `gc_young_collector.cpp`/`gc_gen1.cpp` | 走 region 绑定 | `gc_young_collector.*`/`gc_gen1.*` | XL |
| GC-K4 | 5 | planned | — | 区域化回归验证 | GC-K3 | — | 全量回归 proof | tests + 热更新域卸载 | 24 项无回归 | `tests/contracts/native/runtime-core/` | L |
| GC-L1 | 6 | planned | — | 动态堆数 | GC-K4, GC-E1 | — | 堆数变化 proof | `gc_coordinator.*`/`gc_heap_manager.*` | 运行期增减堆 | `gc_coordinator.*`/`gc_heap_manager.*` | L |
| GC-L2 | 6 | planned | — | 碎片/压力伺服调优 | GC-K4, GC-E1 | — | 调优闭环 proof | `gc_scheduler.*`/`gc_old_gen.*` | 多信号闭环 | `gc_scheduler.*`/`gc_old_gen.*` | L |

> 注：`conflict_scope` 用于并行派生冲突校验；Phase 5 K-K 系列有强文件依赖，建议**串行**执行，不并行派生。

---

## 7. 风险与护栏

| 风险 | 等级 | 护栏 |
|------|------|------|
| 区域化重构(K)破坏热更新域卸载差异化 | 🔴 高 | K 阶段硬性要求域卸载保持 O(region)；架构优先 + K4 专门回归 |
| 全根集扫描(A1)引入性能回退（扫全线程栈） | 🟠 中 | 只对 young GC 加；保守扫描 + 堆范围预过滤保留 |
| 配置旋钮(E1)改动入口契约波及 codegen | 🟠 中 | 只加 getter/setter，不改 CHAOS_IL2CPP_NEW_GC 宏 |
| 跨域分配/free 不匹配(D1/K) | 🔴 高 | 严格遵守 `17-统一内存分配体系` 三域红线；禁止全局 operator new |
| 事件/诊断(H1)过度膨胀拖慢快速路径 | 🟢 低 | 事件走 ring buffer，profile scope 分离 |
| 并行派生文件冲突(K系列) | 🟠 中 | K 系列串行执行，conflict_scope 校验 |

---

## 8. 备选路径

1. **若分区化重构风险超预期（Phase 5）**：降级为「保留固定 segment/TLAB，仅对齐分代决策代感知写屏障」，放弃 region 整体重绑。保全正确性与硬化成果，牺牲面积效率。**触发条件**：K1/K2 架构审查发现 region 重绑与 MemoryDomain 域隔离不可兼得。
2. **若动态堆数(L1)与 NUMA per-node heap 冲突**：退回固定 per-node heap + 固定 worker，只做伺服调优(L2)。**触发条件**：L1 在真实 multi-socket 复现 no-win。

---

## 9. 当前建议推进顺序

串行优先，前阶段做完再做后阶段（强依赖）。**Phase 1 三项 + Phase 2（GC-E1 + GC-D1）已完成**。下一步进入 **Phase 3**（GC-G1 Dependent 收敛最独立，先做；GC-F1 handle 代剪枝 依赖 GC-E1; GC-H1 事件+原因）。

本轮 roadmap 创建后，`recommended_next_child = GC-A1`。

---

## 10. 子任务执行策略

```yaml
execution_mode: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
```

- 每个子任务按 `dev-roadmap` 生命周期派生：进入 `docs/dev/in-progress/<task_id>/`，写独立 STATUS.md，`parent_task_id: gc-align-coreclr`。
- 每个子任务按用户要求**遵循架构优先**：开工前读 `24-CRAG-GC架构参考.md` + 对应领域架构文档，确认与既有架构一致再动码。
- 子任务进入终态后，物理移到 `docs/archive/dev-completed/` 或 `docs/dev/hanging/`、`docs/dev/abandoned/`；父 roadmap 只同步 child mapping。
- 相关既有任务：`docs/dev/hanging/gc-phase3-industrialization/`（profile 驱动调参）作为 E1/L2 的数据来源参考，不并入本 roadmap 作为子任务。

---

## 11. 关联文档

- 对标基准代码：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`（以 `gcpriv.h`/`allocation.cpp`/`background.cpp`/`handle*`/`dynamic_*`/`gcconfig.h`/`gcevents.h` 为权威）
- CRAG 现状：`wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md`
- 分配体系红线：`wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/17-统一内存分配体系.md`
- 区域化上游设计：`docs/dev/designs/c-phase-region-aware-generational-gc.md`
- 并发能力现状：`wiki/03-功能模块/08-GC子系统/CRAG-GC-Phase3-并发能力.md`
