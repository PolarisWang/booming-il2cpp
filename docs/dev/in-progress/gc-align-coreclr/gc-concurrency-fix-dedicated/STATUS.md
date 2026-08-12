# STATUS — 修复 young-GC 跨代崩溃（两层根因）

## 架构映射

- 域：GC(6) + 运行时(2)。两层根因：层1=旧代 region-gen 归 0 → 2参屏障跳卡；
  层2=屏障正确后 young-GC 晋升/回写仍毒 212/thread7。
- 复现器：`tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp`（known-failing，P1 后改善至 14/20）。
- 相关文件：见 `plan-v5-01.md`。

## phase

`executing` — **P1 主线 A 已结算**（commit `35285c431`）；剩余 M2/M3/M4 里程碑跨会话待推进

## 当前结论（plan-v5-01，落地导向）

### ✅ 已结算：P1 主线 A（commit `35285c431`，11 files +355/-8）
- **P1-A3** `GcVerifyPromotedTracked`：GC 出口核验晋升对象 fall-in-tracked 旧代页（IsInOldGen + region-gen OLD）。测试 diagnostics 4/4。
- **P1-A4** `CardBundleSet` 原子 OR（InterlockedOr8/__atomic_fetch_or），杜绝并发设置丢 bundle bit。测试 card_table_ext 7/7。
- **P1-A2b**（仓库级 bug 修复）：full-GC mark 播种时扫描注册静态根（`GcScanStaticRoots`，原从未被调用）→ ALC 静态字段 / OldMessage[] 不再被 full GC 误回收；stress 测试注册 `g_old_slot[]` 静态根。
- GC 测试全绿、构建 exit 0；`gc_diagnostics_test` 4/4、`gc_card_table_ext_test` 7/7。
- **stress 改善至 14/20**（HEAD~50% → 系统性 region-gen + static-root 修复）。残余 **young-GC 晋升** 深因（known-issue，见 memory `gc-crossgen-fix-implementation-status`）。

### ✅ 已结算：M11（M5 P2 工程，commit 待填）
- **GC 配置旋钮扩展 + 死旋钮接线**（plan-v5 M11，务实扩展，用户确认范围）。
  - 接线 3 个死旋钮到真实热路径：`MaxTlabAlloc`→gc_region 分配路径、`LohThreshold`→gc_loh/gc_old_gen 分类、`ParallelMarkWorkers`→gc_parallel_mark + BGC worker（`kMaxTlabAlloc`/`kLohThreshold`/`kMaxParallelMarkWorkers` 从 `static constexpr` 改为 `inline` 可变值，`InitYoungGeneration` 一次性 latch，热路径仅 1 次机器 load）。
  - 扩展宏表至 24 旋钮：新增调度自适应（trigger 乘数 fp*1000 / cooldown / min-GC 间隔 / promote age / nursery-gen1 上下界）、BGC（BgcWorkers / MarkSliceBudgetUs）、旧代（CrossPageFragThresholdFP / EmergencyReserveSize），全部接线到真实读取点。
  - `GcConfigImpl` 字段改为**构造期即初始化为 DEFAULT**（非 0），消除「Initialize() 前读到 0」整族 bug（修复 scheduler 触发/尺寸的早读回归）。
  - 新增 `gc_config_test.cpp`（24 旋钮全量 env-override 读取 + hot-path latch 传播 + C-API get 可达）：**0 failures**。
  - 验证：`ctest -R "gc_" -L unit` 28/29（唯一 FAIL=`gc_finalizer_integration_test`，HEAD 既有与本改动无关）；affected GC 单测 region/loh/parallel_mark/young_collector/scheduler 全 0；构建 exit 0。
  - **已知非回归**：`bgc_race_test` 在 baseline 与改动后都悬挂（BGC 并发-mark 相变死锁，pre-existing，非 unit 绿基线，RESOURCE_LOCK bgc 组）。`test_driver.py --layer unit` OVERALL 因 managed CodeGen snapshot 基线不匹配（52-stind-wide/23-instance-fields，CodeGen 域，pority 与本 GC 改动无关）。

### ✅ 已结算：M12（M5 P2 工程，commit 待填）
- **GCHandle 内部类型：REFCOUNTED + WEAK_INTERIOR_POINTER**（plan-v5 M12，对齐 CoreCLR `HNDTYPE_REFCOUNTED=5` / `HNDTYPE_WEAK_INTERIOR_POINTER=10`）。在独立 worktree `chaos-gc-m12` 推进。
  - `GcHandleEntry` 扩字段：`refcounted`(bool) / `refcount`(int32) / `interior_offset`(size)——尾插新字段，10 处既有聚合初始化兼容（缺省值初始化 0/false）。
  - 新增 4 个 C-API：`GcCreateRefCountedHandle`(refcount=1) / `GcAddRefHandle`(±1) / `GcReleaseHandle`(到 0 释放=等效 GcFreeHandle，含 pinned 清理) / `GcCreateWeakInteriorHandle`(weak+offset)。实现于 `engine_lifecycle.cpp` 分片锁模式内。
  - `GcGetHandleTarget` 对 WEAK_INTERIOR（offset≠0）返回 `object+offset`（内部指针解析）。
  - REFCOUNTED 是 `weak=false` → 弱清空路径(young+full GC)天然免疫；WEAK_INTERIOR 弱清空只 null `object_instance`、**保留 offset**（可重建）。即**无需改弱清空函数**。
  - `gc_handle_test.cpp` 增 `TestRefCountedHandle`(create→addref→GC 存活→release→free) + `TestWeakInteriorHandle`(offset 解析 + content)：**11 tests 0 failures**。
  - 回归：config/region/loh/parallel_mark/young_collector/scheduler/card_table/diagnostics/sanity/handle 10 测试全 0 failures；构建 exit 0。
  - **已知独立**：`gc_atomic_alloc_test.cpp:108` constexpr 求值失败（pre-existing，该测试文件本体的 C2131，与 M12 无关）。

### ✅ 已结算：M13（M5 P2 工程，commit 待填）
- **GC 事件集 ETW 覆盖测试**（plan-v5 M13，eventtrace → ETW/EventPipe 测试角度）。
  - `gc_events_test.cpp` 增 2 测：`TestEtwInitializeShutdown`（provider 幂等 init/shutdown/重 init/double-shutdown 安全）+ `TestEtwFireFunctions`（全部 `GcEtwFire*` 事件——GcStart/End/Young/Full/Oom/Gen1Collect/AllocationTick——无 provider 时安全 no-op、跨 init/shutdown 可调不崩溃）。
  - include `gc_etw.h`，接入既有事件测试 (`chaos_runtime_core` 已链 `gc_etw.cpp`)。
  - `gc_events_test` 5 → 7 tests：**7 tests 0 failures**。
  - 回归：config/handle/scheduler/region 全 0 failures；构建 exit 0。

### ✅ 已结算：M15（M5 P2 工程，commit 待填）
- **oom_budget gen 级缩放**（plan-v5 M15，allocation.cpp → CRAG 对应 `gc_api.cpp`）。
  - `kOomReportHalfBudget`（硬编码 32KB）替换为 `GcGetOomReportBudget()`（公开 API）：从 config 驱动的 gen0/nursery 最小预算 `GcConfig().MinNurserySize / 2` 派生，对齐 CoreCLR `allocation.cpp oom_budget = dd_min_size(gen0)/2`。默认 64KB/2=32KB（行为不变），配置可缩放。
  - `gc_api.cpp` include `gc_config.h`；`gc_api.h` 公开 `GcGetOomReportBudget`。
  - `gc_config_test.cpp` 增 `TestOomReportBudgetScaled`（默认 32KB / override 128KB→64KB / clamp ≤ min）：config test **4 groups 0 failures**。
  - 回归：handle/events/scheduler/region 全 0 failures；构建 exit 0。

### ⬜ 剩余里程碑（跨会话，每里程碑独立验证+测试全绿再进）
- **M5 主线 B2 并发**：M5 BGC 分相 → M4 provisional；M3B/M6。
  - **M5-A 已落地（commit 待填）**：`StwRemark` 两快照 clear-as-scan — 重扫旧代卡 + Gen1 卡时 mark+ClearCard 消费，使 remark 幂等不重复 mark。验证 `bgc_race_test` 由 M11 期 round-3 悬挂推进到 5 轮全过（残余 phase-6 COMPACT_NEEDED wait 为 pre-existing，与 baseline 相同）。`bgc_smoke` 6/0 无回归。

### ⬜ 剩余里程碑（跨会话，每里程碑独立验证+测试全绿再进）
- **M2 主线 B1**：P2-M1(GC-M1 K2c regen，foundation-dll 集成，本会话评估 blocked-on-pipeline) / P2-M3A(Server GC 多堆) / P2-M10(gen>condemned，M9 后)。
  - M3A Server：**scaffold 未跑通**（`GcHeapManager::Initialize()` 零生产调用；GC_SERVER=1 崩空 heap 数组；CMake 默认 ON vs 文档 OFF 不一致）。**未在本会话落地**，体量大，需独立里程碑 + CI 构建矩阵。
  - M10 gen>condemned：**本会话已落地**（见下 M9 链）。
- **M3 主线 B2 三代链**：M9/M10/M8/M7-A **本会话已落地**。
  - **M9**（A1 gen1 标 + A2 激活 condemned + gen-aware scavenge + gen1 tag 测试）：完结。
  - **M10**（gen>condemned 过滤实激活 + 测试）：完结。
  - **M8**（plan-gen 重绑验证）：完结。
  - **M7-A**（region demotion region-gen 验证，CoreCLR `set_region_plan_gen_num` demotion 对齐）：完结。`gc_gen1_test` 13/0。
  - **M7-B**（age-based evacuation + 域卸载不碎片）**未落地**：net-new 深 GC 手术，跨会话。
- **M4 主线 B2 并发**：M5 BGC 两快照 → M4 provisional；M3B/M6。
  - **M5-A** clear-as-scan 已落地；M5 完整两快照纪律 + M4/M3B/M6 未落地。
- **M5 主线 B3 全部完结**：M11/M12/M13/M15 已完成（M14 ProvStress 降优后置）。
- 完整规格/步骤/测试/判据见 `plan-v5-01.md`。

## blocking_questions

- [x] 三层优先级：P1>P2>P3；P1 正确性已结算，剩余按里程碑推进。
- [x] 裁决原则：方案有疑问选对齐 CoreCLR（CoreCLR 存在性已核实）。

## question_clearance

`cleared`

## clearance_confirmed_by_user

`true`


## phase

`executing`（已验证增量已提交；按 plan-v5 P1 推进，里程碑式）


## blocking_questions

- [x] 层1 修复方式：旧代/LOH 页 `GcMarkRangeOld` 标 OLD（已落地，回归内）。`IsNurseryPointer` 变体**回滚**（破坏 K2b region-gen 契约）。
- [x] 层2 推进：确定性诊断已完成（fwd=0/unfwd=全部 + Phase-1 total=nursery=promoted），确凿指向 young-GC Phase-1 扫描覆盖缺口。
- [x] 三层优先级：P1>P2>P3；层1 纯增量无热路径代价；单参路径不变。

## question_clearance

`cleared`

## clearance_confirmed_by_user

`true`

## 下一步入口（plan-v3-01）

1. **A2b（核心残余）**：旧代 mark 跟进存活 raw-struct 对象引用（message slot→promoted），对齐 CoreCLR，深 GC 手术。
2. **B1**：GC-M1 K2c regen / M3A Server GC / M10 gen>condemned。
3. **B2**：M9 三代 → M8 plan-gen → M7 demotion；M5 BGC 分相 → M4 provisional；M3B/M6。
4. 复现器 20x 全绿 + `test_driver.py --layer unit` 无回归。

## 风险评估摘要

- 已验证增量（层1 + 系统性 region-gen + VerifyHeap + condemned）已提交 `8904326a0`，无回归。
- A2b 深 GC 手术（旧代 mark 引用跟进）风险高，需专门会话 + Verify 自检护航。
- `gc_region_test` K2a flake（GetRegionGen(nursery)!=young）为 **pre-existing**（改动前后都 fail）。
- `gc_finalizer_integration_test` 失败为 HEAD 既有，与本改动无关。

## 三优先级权衡结论

- P1：2参 barrier 非绝对 hot path，加 `IsNurseryPointer` 查询代价可接受；单参 `DirtyCard` 不变。
- P2：旧代页标 OLD + 精确 range 判定，对齐 CoreCLR region 语义，架构一致。
- P3：解释器单参路径不受影响；热更/域卸载不变。低优先级让位于 #1 高性能→保持升+精确。
