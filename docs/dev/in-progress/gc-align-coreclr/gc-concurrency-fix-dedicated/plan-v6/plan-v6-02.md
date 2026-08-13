# plan-v6-02 — GC P1/P2 落地计划 v2（调查校正版，可自动执行）

> 域：GC(6)+构建(7)+运行时(2)。创建：2026-08-12。
> 输入：`docs/dev/in-progress/gc-align-coreclr/gc-concurrency-fix-dedicated/plan-v6/investigations.md`（8 项代码级调查设计）。
> 本版相对 plan-v6-01 **重大校正**：经 8 个并行调查 agent 实读源码，修正了多处错误前提（NGC2 已实现、Page pool 已实现、FindPage 已 O(log n)、晋升 Option A 结构不可行）。
> 裁决原则：方案疑问一律选对齐 CoreCLR。arch_review_mode: `critical`。

## 执行约定（每项完成后才进下一项）
1. 改代码 → 2. 写/改测试 → 3. `ctest -R gc_ -L unit` 全绿 → 4. `test_driver.py --layer unit` 无非 known FAIL → 5. 域 STATUS 记 completed → 6. commit（含 root_cause/fix_strategy/regression_check）。

---

# 校正后 P1 任务（正确性/生产）

## T6 — young-GC 晋升回写深因（A2b）【P1·最优先】
> **调查校正 v1**：方案A（GcLayoutRegistry 注册 OldMessage 128-slot layout）**结构不可行**——`pointer_offsets` 定长 [32]（gc_layout.h:41/65）且扫描协议要求首字=TypeInfo（gc_old_gen.cpp:1024, gc_layout.h:304）。故原计划选方案B。
>
> **⚡ 实证否决 v2（2026-08-12，本会话实测）**：按调查建议实现方案B（`TryMarkRootConservative` 保守 old-mark 推进 raw struct 引用），编译通过、stress 20x 实测 **dangling 仍随机 32~307/1024，未降低**。→ **方案B 无效，已 revert（工作区干净）**。这与项目既有 memory `gc-crossgen-stress-true-rootcause` 一致：**残留根因 = young-GC Phase-1 扫描覆盖缺口 + `IsInNursery` 注册/边界**，不是「old-mark 不推进 raw struct」。被杀的是 OldMessage 引用的 **nursery 目标**，由 young-GC Phase-1 决定生死；旧代 mark 保 OldMessage 无济于事。
>
> **M1 正确修复方向（依 memory 既有诊断）**：
> - a) 旧代页 region 归属 / 精确 nursery-range 屏障（`GcMarkRangeOld`+`IsNurseryPointer`，层1已做）；
> - b) 追 `gc_young_collector.cpp` Phase-1 `ScanDirtyCardsInPagesBatched` 是否漏扫 thread-7 的 old 页（page_list_/in_use 覆盖）；
> - c) 晋升 `GcScavengeObjectKnownNursery` 后 slot 回写 + `IsInOldGen(tenured)` 一致性；
> - d) `IsInNursery`（`RegionManager::IsNurseryPointer`）依赖的 `nursery_slots_` 注册/边界覆盖——多 TLAB/共享 region 边界处返回 false → Phase-1 跳过。
> 必须先做**系统性调试**（Instrument 每个方向），不可再凭调查假设直接改。如连续多轮无进展 → 终止交人类（CLAUDE.md 三次修复规则）。

## T4 — Server GC 多堆集成（M3A-2/3）【P1·正确性核心】
> 校正后 6 处缺口（实读证据在 investigations.md#T4）。**顺序：F2(跨堆根标记→F3(epilogue)→F4(accessor)→F1(入口旁路)→F5(通知去重)**。

- **F2 并行根扫描（最高，正确性根因）**：`ExecuteMultiHeapGc`（gc_coordinator.cpp:74-79）每 worker 在各自堆跑 `G_OldGen().Collect`，Phase1 根扫描 `G_OldGen().ScanRangeForRoots`（SERVER 下解析为 worker 自己 heap）扫共享 young/gen1 并只标自己位图 → **跨堆漏标→UAF**。
  - 改：coordinator Phase2 前做一次**根分发 mark**——单线程 safepoint 下遍历共享 young+gen1+线程栈，对每个 root 用 `G_HeapManager().IsInOldGen/FindPage` 判归属堆，标入对应堆 mark_stack；再让各 worker 只 DrainMarkStackParallel+Sweep。
- **F3 epilogue 缺失**：`ExecuteMultiHeapGc`（gc_coordinator.cpp:43-89）无 `RunFinalizers/RecordFullCollection/RecordYoung/RecordGen1/GcVerifyHeap/GcEtwFireGcEnd`。WKS 有（gc_region.cpp:1458-1466）。改：提取 WKS 收尾为 `GcFinishFullGc()` 复用。
- **F4 accessor 裸全局**：`gc_young_gen.h:160-173` FlushTlsAllocCounter 用 `extern GcScheduler g_gc_scheduler` 而非 `G_Scheduler()` → per-heap 分配计数永为 0 → 触发失真。补 include `gc_heap.h`。
- **F1 入口旁路**：`gc_api.cpp:235-277` gen==1/gen==0 分支直接调单堆，SERVER 下改 `GcCoordinator::RequestGlobalGc()`。
- **F5 通知重复**：`gc_old_gen.cpp:2402` SignalFullGcApproach/Complete 每堆重复 → coordinator 触发一次。
- **CI**：`cmake` 新增 `-DCHAOS_IL2CPP_GC_SERVER=ON` 独立 preset（不入默认 reference）；`gc-ci.yml` 增加 SERVER 矩阵。**注意**：无 NUMA 硬件 CI 上 num_heaps 自动回退 1 → 用 `GcHeapManager::Initialize(2/4)` 显式注入堆数 mock。
- **测试**：新增 `gc_server_multiheap_test.cpp`（2 堆：跨堆引用→GC 后两者存活；per-heap 独立回收；epilogue 状态更新；RequestGlobalGc 幂等；worker 路径收敛）。
- **完成判据**：Server ON 构建 + 多堆测试全绿；WKS 无回归。

## T5 — Page 回收 + FindPage 索引【P1·长期内存/加固】
> **校正**：Page pool（`page_pool_`+`PoolEntry`+kMaxPoolSize=16）与 FindPage O(log n)（`page_array_` 二分）**已实现**。真实缺口 = 加固 + 补测试 + 压力下池回收统一入口。

- **真实子缺口**：
  1. `Free()` 普通页（gc_old_gen.cpp:907-916）只 memset 64B，**页入池延迟到下一次 GC 的 PageFragmentation>=1.0** → 无 GC 则 64KB 页长期挂着。补**低代价即刻回收开关**或 RecordFreeListRelease 反向信号，异步（不进分配热路径）。
  2. 无测试覆盖 pool 复用/回收/FindPage 二分/NUMA trim（现有 `gc_old_gen_unit_test.cpp` 仅 6 TEST）。
  3. `DecideCollection` 无"高内存压力下主动 trim pool"统一入口（仅 BgcSweep/Sweep 顺带）。
- **测试**：新增 `gc_old_gen_pool_test`（回收进池/page_count 减/复用页/NUMA trim 上限）+ FindPage 二分边界断言 + `gc_old_gen_free_reclaim`（Free→BgcSweep→入池→无 dangling）。
- **完成判据**：pool 测试全绿；stress 长时间 page_count 有界；无回归。

## T7 — 世代写屏障压力验证（B2）【P1·漏卡 UAF】
> **校正（关键）**：现存 K2b 断言只验证**位置合法**（IsNurseryPointer/IsInOldGen，gc_region_barrier_stress_test.cpp:127-148），**位置合法≠存活** → 对漏卡 UAF 是**假阴性**。且 `GcVerifyHeap` kFull 目前是空壳（见 T8）。

- **实施步骤**：
  1. 新增 `gc_region_multigen_stress_test.cpp`：4 代（Nursery/Gen1/Old/LOH）交叉对象图 + 8 线程并发写 `_dst_ref`，祖先注册 static root。
  2. 用**存活级校验**替代位置校验：GC 后遍历 cross-gen 引用，断言 target 存活（old: `G_OldGen().IsMarked`；loh: `G_Loh().IsMarked`；nursery: `IsNurseryPointer`+`GetRegionGen==young`；gen1: 落在 gen1_region..gen1_bump）。
  3. 实现 `GcVerifyCrossGenEdges()`（gc_diagnostics，供测试直接调），kFull 消费。
  4. 对照组（跳过 `_dst_ref` 写 slot）断言 verify **必失败**（self-check）。
- **完成判据**：4 代压力测试全绿；对照组证明 detector 有效；无回归。

# 校正后 P2 任务（能力/性能）

## T8 — Verify 深度强化（G4）【P2】
> **校正（重要）**：`GcVerifyHeap` kFull **是空壳**（gc_diagnostics.cpp:66-71 注释 "future walk"，kFull==kCrucial）；`GcVerifyPromotedTracked` 有**误框 bug**（全断言 IsInOldGen，但 gen1 晋升合法落 gen1-region → 误报）且只在遗留 `PromoteNursery`（gc_region.cpp:1095）被调，主 young-GC 流程**不调用**（死断言）；`GcVerifyHeap` 仅非-Server chaos_gc_collect 调。

- **实施步骤**：
  1. 重写 `GcVerifyPromotedTracked`：按目标地址 GetRegionGen 分类校验（OLD→IsInOldGen；Gen1→IsInGen1 区间；gen0/其它→报错）。先修误框。
  2. kFull 实现 referenced-allocation walk：遍历 PageArray，marked 对象首址校验 (a)region-gen==OLD (b)IsInOldGen (c)合法 TypeInfo；并消费既有位图 poison（gc_old_gen.cpp:214-218 16B 0xCD）。
  3. 挂 `GcYoungCollection` 出口（gc_young_collector.cpp:931 return 前）：kFull 时调 GcVerifyPromotedTracked(result)。
  4. 补 Server/主路径：GcCoordinator GC 完成点 + allocation-triggered young GC。
- **测试**：`gc_diagnostics_test` 增（误框回归：gen1 地址不报错、假 OLD 越轨地址报错；kFull 分级；poison 翻转检测）。
- **完成判据**：diagnostics 全绿；`CHAOS_GC_HeapVerify=2` stress 晋升全过；=0 与 =2 指标一致。

## T9 — dynamic_tuning 伺服 + BGC servo（E1/H3/E2）【P2】
> **校正**：frag/surv 已有采集（gc_young_collector.cpp:908、gc_scheduler.cpp:78-92/167-221）但**只记录不闭环**到 gen0 触发阈值（gc_scheduler.cpp:420-431 scaled_young_multiplier 现仅压力驱动）；free-list rate 从未采样；memory_load 非闭环。M11 已 24 旋钮。

- **实施步骤**：
  1. 新增 `gc_dynamic_tuning.h/.cpp`（GcServoState 单例，全 atomic）+ 伺服状态（free_list_rate/frag_density/memory_load）+ 采集点（BgcSweep 收尾、SetOldGenFragmentation、GetPlatformMemoryStatus 处）。
  2. 反馈闭环：`scaled_young_multiplier`（gc_scheduler.cpp:422-427）改为 `clamp(Gen0BudgetFp*(1-FragWeightNurseryFP*frag), Floor, Ceil)`；RecommendedNurserySize(496) 硬编码 frag 折扣改读旋钮；Gen1 预算 fed by gen1 EMA。
  3. 新增 `gc_bgc_servo.h/.cpp`：FL servo（`mark_slice_us = clamp(mark_slice±P*error)`），读点 gc_bgc.cpp:949/843；ML 后置留空壳。
  4. 旋钮 24→~50，全部接读取点（违 gc_config.h:30-32 死旋钮禁令会被 hook 拦截——**声明告警**）。
- **测试**：`gc_dynamic_tuning_test`（无输入预算恒 1.0 回归闸；frag 降预算；free-list 提前触发；FL 吞吐增减；节流；clamp）。
- **完成判据**：伺服单测过；分配回归 <5%；旋钮读取全覆盖。

## T10 — Dynamic Heap Count（A2）【P2·强依赖 T4】
> 依赖 T4（Server ON + CI 绿）先落地。设计见 investigations.md#T10，5 步（ResizeHeaps/MaybeResize/GC-join-per-heap 一致性/测试）。挂起至 T4 完成后实现。

## T11 — provisional 完整形态（C2）【P2·只剩 gen1 强制 compact】
> **校正（前提反转）**：NGC2 排队（gc_scheduler.h:343-365，SetProvisionalMode→ngc2_queued_，TestNgc2Queue 已过）**已实现**。force-blocking 已实现。plan-v6-01 说"缺 NGC2"是**错误**。真正待补 = **gen1 强制 compact**（`GcGen1Collection` 触发只看 threshold/occupancy，gc_young_collector.cpp:723-737，无 InProvisionalMode 分支）。

- **实施步骤**：
  1. `gc_scheduler.h` 新增 `force_gen1_compact_` 原子标志 + `SetProvisionalMode(on)` 时置位。
  2. 消费点（gc_coordinator.cpp:59-64 Server、gc_young_collector.cpp:737-745 WKS）：`GcGen1Collection(bool force_compact=false)` 签名扩展，force=true 走全量 compact（复用 gc_gen1.cpp:545-671 compact_buf 管线，finalizer 采样为"老对象也 compact 而非 promote"）。
  3. fire-once 消费清除（对齐 NGC2 ngc2_queued_ 模式）。
  4. 可选：`DecideCompactMode`（gc_old_gen.cpp:1597）provisional 时强压 CROSS_PAGE。
- **测试**：`gc_provisional_test` 增 TestProvAllCompact + TestProvFireOnce。
- **完成判据**：provisional→gen1 强制 compact 单测过 + fire-once + 无回归。

---

## 里程碑/阶段切分（本会话自动推进顺序）

| 里程碑 | 内容 | 依赖 | 判据 |
|--------|------|------|------|
| **M1** | T6 晋升深因（Option B） | 无 | stress 20x 全绿 |
| **M2** | T7 写屏障压力 + T8 Verify 强化 | 无（共享 kFull 实现） | 4 代压力全绿 + diagnostics 全绿 |
| **M3** | T4 Server CI（F2→F5）+ T5 Page 加固 | 无（CI 并行） | Server ON 多堆全绿 + page pool 全绿 |
| **M4** | T11 provisional gen1 compact | 无 | provisional 单测全绿 |
| **M5** | T9 dynamic_tuning 伺服 | 无 | 伺服单测过 |
| **M6** | T10 Dynamic Heap Count | **M3 (T4)** | 堆数变化测试全绿 |

每里程碑结束：`ctest -R gc_ -L unit` 无回归 + commit；STATUS 记录。

## 退出条件（全部完成）
- P1：stress 20x 全绿 + Server 多堆正确性全绿 + page pool 有界 + 漏卡 detector 有效。
- P2：Verify 分级 + 伺服闭环 + provisional 完整 + dynamic heap（T4 后），各单测全绿。

## 验证命令（统一）
```bash
ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "gc_" -L unit --output-on-failure
python tests/runner/test_driver.py --layer unit
CHAOS_GC_HeapVerify=1 artifacts/native-runtime-core-test/Debug/chaos_gc_region_barrier_stress_test.exe
```

## 删除/降级/前提修正（相对 plan-v6-01）
- ✅ NGC2：**确已实现**，从 C2 删除（改 T11 只剩 gen1 强制 compact）。
- ✅ Page pool + FindPage O(log n)：**确已实现**，T5 重定义为加固+补测。
- ✅ T6 方案A：结构不可行（pointer_offsets[32] + 首字 TypeInfo），**判死**，用方案B。
- ✅ T8 kFull：现为空壳，确有梯度缺口 + 误框 bug。
- 关于 Workflow：实际代码实现按依赖顺序串行执行（M1→M6），不能全部并行（T10 依赖 T4；GC 共享不变量需逐项构建+验证）。
