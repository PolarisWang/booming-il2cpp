# CRAG GC 工业化生产能力差距综合分析报告

> 评估日期：2026-08-25（数据截至 2026-08-14，含 `cf0a69aec` g_heap_base 修复）
> 评估对象：`src/native/runtime-core/gc/`（CRAG — Chaos Region-Aware GC，64 个源文件）
> 对标基准：CoreCLR `src/coreclr/gc/`（_V8_ 当代模块化 GC）＋ .NET/Java 生产级 GC 惯例
> 方法：只读综合分析 —— 既有评估文档（08-10/08-12/08-13 四份）+ 实读源码关键点 + git log + CI 配置 + known-failures 基线交叉核验
> 声明：本报告为**只读评审**，不含任何代码改动

---

## 零、TL;DR（一页结论）

CRAG GC 的**核心架构语义已高度对齐生产级分代 GC 的骨架**（3 代 + LOH + POH、精确扫描、SATB 并发标记、并行 mark/sweep/compact、世代写屏障、per-thread safepoint），且热更新域卸载（MemoryDomain O(region)）是 CoreCLR/Mono/Boehm 都不具备的差异化优势。

但**"能跑"与"能在并发下不错、能被 CI 守护、能被生产工具链消费"之间仍有系统差距**：

| 维度 | 一句话定性 | 评级 |
|------|-----------|------|
| 正确性/并发安全 | A2b 已摘 known-fail 但并发护网仍在 CI 门外；BGC root-scan 仍 known-fail | 🟠 中风险 |
| 验证与 CI | 单测已进 CI 但仅 Windows Debug；stress 夜间 `\|\| true` 失败不红门；无 sanitizer/Release/ARM64 GC 门禁 | 🔴 弱 |
| Server GC 工程 | 多堆骨架存在但默认 OFF、零 CI 验证、无 Dynamic Heap Count | 🔴 未验证 |
| 动态调优 | 24/~75 旋钮；空间域伺服闭环未做；profile 驱动调参任务 hanging | 🟠 未闭环 |
| 可观测性 | 11 回调事件 + 9 ETW ID vs CoreCLR 40+；无 BGC 阶段事件族；BCL GCMemoryInfo 缺失 | 🟡 浅 |
| 边缘能力 | GCHandle ~7/13 型；无 vxsort/Frozen segment/分代 HardLimit；HandleTable 剪枝回退 | 🟡 缺 |
| 性能 | 大堆 Full GC 暂停 ~130ms vs CoreCLR ~50ms；并行 8 worker 上限；page pool 延迟返 OS | 🟠 中 |
| 治理/可信度 | STATUS "✅" 与验证闭包 0% 矛盾；文档与源码漂移 | 🔴 失真 |

**量化**：核心架构语义对齐度 **~80%+**；功能完整度 **~55-60%**；**生产可依赖度（验证深度 + 工程闭环）约 40-50%**。对"演示/探针"负载可用，对"生产负载"还不能背书。

---

## 一、评估范围与方法

### 1.1 信息源

1. **既有评估链**（本仓 `docs/dev/assessments/`）：
   - `gc-vs-coreclr-remaining-features-2026-08-10.md`（26 项功能域清单）
   - `gc-vs-coreclr-gap-assessment-2026-08-10.md`
   - `gc-vs-coreclr-review-2026-08-12.md`（模块化 CoreCLR _V8_ 逐 bucket 对照）
   - `gc-risk-assessment-2026-08-13.md`（3 路并行 Explore + 源码实证）
   - `production-readiness-gap-2026-08-13.md`（5 路跨域探针）
   - `gc-test-engineering-review-2026-08-10.md`
2. **实读源码**：`gc_config.h`（24 旋钮全表）、`gc_events.h`（11 事件）、`gc_etw.cpp`（9 ETW ID）、`gc_features.h`（Server GC=0）、`gc_api.h`（chaos_is_gc_pointer 三段判定）、`gc_region.h`、`gc_card_table.*` 等。
3. **工程事实**：`git log`（近 50 条）、`tests/runner/baselines/known-failures.integration.yaml`、`.github/workflows/gc-ci.yml`、roadmap `gc-align-coreclr/roadmap-v2-01.md` + STATUS。

### 1.2 判定口径

- **工业化生产能力** = 正确性 × 并发安全 × 验证/CI 守护 × 可调优性 × 可观测性 × 平台覆盖 × 工具链消费（诊断器/剖析器/调试器）的乘积。任何一项为 0，整体不可称"工业化"。
- 每条差距标注**证据出处**：`[代码实证]` / `[评估文档]` / `[CI 配置]` / `[已知失败基线]`。

---

## 二、现状画像：已具备的工业化能力（先肯定）

以下均已**代码级核实**，不是纸面宣称：

| 能力 | 证据 | 状态 |
|------|------|------|
| **3 代 + LOH + POH 分代模型** | `REGION_NURSERY`/`REGION_GEN1`/`MarkSweepOldGen`/`LargeObjectHeap`/`REGION_POH` | ✅ |
| **精确扫描（GcLayout 4096 槽）+ 保守兜底（堆范围过滤）** | `gc_layout.*`、`GcScanConservativeFrame` | ✅ |
| **SATB 后台并发标记（BGC）+ 事件驱动唤醒（无轮询）** | `gc_bgc.*`、状态机 `IDLE→CONCURRENT_MARK→STW_REMARK→CONCURRENT_SWEEP` | ✅ |
| **并行 mark（work-stealing，≤8 worker，chunked deque）** | `gc_parallel_mark.*` + 5 个已修缺陷记录 | ✅ |
| **并行 sweep / 并行 compact / 动态 evacuation budget** | `gc_worker_pool.*`、`DecideCompactMode` | ✅ |
| **世代写屏障 3 短路 + card bundle 粗卡** | `chaos_gc_dirty_card_dst_ref`、`GetRegionGen`、`kCardBundleShift=21` | ✅ |
| **per-thread safepoint handshake + event wait + APC 回退** | `ManagedThread::suspend_seq/ack/event` | ✅ |
| **内存域热更新集成（O(region) 批量释放）** | MemoryDomain + domain_unloader（CoreCLR 不具备） | ✅ 差异化 |
| **NUMA 感知（win/linux 双实现）** | `gc_numa_win.cpp` / `gc_numa_linux.cpp` | ✅ |
| **Page 回收（100%-free → pool）+ FindPage O(log n)** | `gc_old_gen.*`（08-12 后已闭合） | ✅ |
| **OOM 三档归因 + provisional force-blocking + NGC2 queue** | `gc_scheduler.*` | ✅ 部分 |
| **GCHandle strong/weak/pinned/dependent + WeakTrackResurrection + finalizer 线程** | `engine_lifecycle.*`、`gc_old_gen.cpp:3138+` | ✅ |
| **配置旋钮 24 个（env + AOT/JIT API 双通道）** | `gc_config.h` 全表 | ✅ |
| **GCStress 模式 / NoGCRegion / LatencyMode / GCCollectionMode** | `gc_stress.*`、`gc_api.*` | ✅ |
| **大规模确定性单测（56 个 CTest 目标）+ 17 场景 stress** | `tests/unit/runtime-native/runtime-core/gc/` | ✅ |
| **近期内存安全修复已登 main** | g_heap_base 覆写 UAF（`cf0a69aec`）、A2b 校验缺口（`33697f00d`）、StSFld 反向 root、nursery 盲点 | ✅ |

---

## 三、差距分析（按维度）

### 3.1 正确性与并发安全（P0 级，最高优先）

**3.1.1 🔴 A2b 跨代引用 UAF —— 已止血但"翻案"与"真实根因"并存，护网仍未进快门禁**

- 时间线：
  1. `gc_region_barrier_stress_test` 报间歇 dangling（store-then-barrier 窗口假说），三次修复（保守 mark / naive 全槽重扫 / 原子屏障段）全部 revert，触发三次规则归档 known-hard；
  2. `33697f00d`（08-13）**翻案**：判定为测试校验缺口 —— post-check 只认 `IsNurseryPointer`+`IsInOldGen`，漏认 GEN1(Survivor) 中被晋升的存活对象，修正谓词后 20/20 PASS，**摘除 known-fail**；
  3. `cf0a69aec`（08-14）**又找到真实 UAF 根因**：`MarkSweepOldGen::AllocatePage` 首页分配覆写 `g_heap_base` → 已注册 segment 的 `seg_idx` 失效 → young-GC Phase-1 扫不到 dirty old-gen 卡 → 跨代引用悬垂（复现 0xC0000005），已修复 + 新增回归测试。
- **剩余风险**：
  - 该 stress 测试仍挂 `stress` label，**被 `-LE benchmark|stress|soak` 排除在快门禁外**；夜间 tier 用 `|| true`，**失败不红门**（`gc-ci.yml:121`）；
  - `ConservativeSweepSelfRefs` 仍在 commit 注记中列为 A2b known-fail 类；
  - 修复走的是"测试谓词 + 单点根因"路径，**store 与 card 非原子性的并发窗口本身并未结构性消除**——下一次布局/路径变化可能以新形态复现。

**3.1.2 🔴 BGC root-scan 挂起仍是 known-fail**

- `chaos_gc_bgc_root_scan_test` 仍列 `known-failures.integration.yaml:13-14`（~392s 极慢 + 挂起），BGC hang 是已记录实物（`gc_bgc.cpp:167-172`）。**并发 GC 的正确性基线没有全绿证据。**

**3.1.3 🟠 卡表 L1 realloc 与并发 DirtyCard 的指针稳定性**

- `GcRegisterHeapRange` 向下 rebase 时 `g_card_l1.swap(new_table)`（`gc_card_table.cpp:168/193`）释放旧数组，而 `DirtyCard` 用 relaxed load 无锁读 → 并发写屏障可能悬垂读。`g_card_bundle` 靠"预分配永不 realloc"规避，`g_card_l1` 缺同样保护。`[代码实证]`

**3.1.4 🟠 解释器↔GC 边界：地址近似守卫的残余依赖**

- `chaos_is_gc_pointer` 已补 nursery 回退（`gc_api.h:105-128`，`RegionManager::IsNurseryPointer`），P0 盲点已修；
- 但解释器对象模型**物化方案被用户中止**，真实 GC 指针仍裸进解释器槽，8 处写屏障依赖地址判定守卫 —— 这是已知遗留，决定解释器路径的 GC 安全上限。`[评估文档 + 代码]`

### 3.2 验证与 CI 工程化（工业化最短板之一）

| # | 差距 | 证据 | 严重度 |
|---|------|------|--------|
| 1 | **GC 单测进 CI 但仅 Windows + Debug**；无 Release/shipping GC job、无 ARM64 GC job | `gc-ci.yml` matrix 仅 `config: [Debug]`，`runs-on: windows-latest` | 🔴 |
| 2 | **并发/stress/soak 全部在快门禁外**：14+ stress 测试 `-LE` 排除；夜间 tier `\| true` 失败不红 | `gc-ci.yml:72-78,121` | 🔴 |
| 3 | **零 sanitizer**：`cmake/sanitizers/asan.cmake`+`rtc.cmake` 存在但 CI 零引用；无 ASAN/TSAN/UBSAN 档（GC 是内存安全最高危模块） | `[测试工程评审]` | 🔴 |
| 4 | **双测试树过滤不匹配**：gc-ci 的 `-R "chaos_gc_"` 只匹配旧 contracts 树，匹配不到新 `test_gc_*` 56 目标 | `[08-13 风险评估 §6]` | 🟠 |
| 5 | **HeapVerify 深度/入口不全**：kFull 仅 WKS 显式 collect 入口/出口；分配触发 GC、Server、ARM64 不跑；无 CoreCLR 分级 HeapVerify + poison | `[08-13 风险评估 §4.4]` | 🟠 |
| 6 | **真理性验证 0/2571**（全仓 nativeProof 全 pending），STATUS "✅" 无独立背书 | `production-readiness-gap` D3 | 🔴 治理 |
| 7 | 已知失败纪律脆弱：`bgc_smoke` 曾靠"源码注释排最后"隔离（后根因修复），`bgc_root_scan` 靠 baseline 挂起 | `[测试工程评审]` | 🟠 |

### 3.3 Server GC 多堆工程化

- **现状**：`GcHeapManager`/`GcCoordinator::ExecuteMultiHeapGc`/per-NUMA `GcHeapContext` 骨架存在，但 `gc_features.h:38` `CHAOS_IL2CPP_GC_SERVER=0` 默认 OFF，**全仓无任何 caller 把 server 置 ON，零 CI 验证**，Server 堆甚至不跑 `GcVerifyHeap`。
- **差距**：M3A（Server 多堆集成测试）未做；**Dynamic Heap Count**（运行期增减堆数）完全缺失（CoreCLR `.NET 9/10` 主打能力）；线程↔heap 绑定在并发卸载/堆调整下的协调未验证。
- **风险定性**：默认 OFF ≠ 无风险 —— 一旦用户按文档开启即进入"从未被 CI 跑过"的路径。`[代码 + CI 配置]`

### 3.4 动态调优与自适应（性能工程化）

| 差距 | 现状 | 目标（CoreCLR） |
|------|------|----------------|
| **dynamic_tuning 伺服闭环** | 仅 EMA 时域调度器，无 free-list rate/碎片密度/memory-load 空间域伺服 | `dynamic_tuning.cpp` 多信号闭环 |
| **配置旋钮** | 24 个（`gc_config.h` 全表核实） | ~75 个 |
| **BGC 伺服调优（FL/ML 双伺服）** | 无 | `BGC_SERVO_TUNING` |
| **profile 驱动调参** | `gc-phase3-industrialization` 任务 **hanging**（高分配 benchmark 场景未建、参数组合未跑） | — |
| **HardLimit 分代（SOH/LOH/POH 独立）** | 单硬限 | 分代独立限 |

**影响**：无法针对负载特征（高分配率/高碎片/大堆）做数据驱动的参数收敛，长期运行内存/暂停行为不可控。

### 3.5 可观测性与诊断

- **事件集**：进程内回调 11 种 `GcEvent` + ETW provider 9 个 event ID（Windows-only，非 Windows 为 no-op）vs CoreCLR 40+ ETW/EventPipe 事件；**缺 BGC 阶段事件族**（BGCBegin/1st/2nd/DrainMark/Revisit/Overflow/AllocWait）、FullGCNotify、动态 SizeAdaptation；`gc_bgc.cpp:322` Gen1 ETW payload **硬编码 0**。
- **per-GC 原因**：`GcTriggerReason` 单值 vs CoreCLR condemn/compact/expand 原因位图。
- **BCL 侧**：native `GcMemoryInfoNative` 11 字段已实现，但 **BCL `GCMemoryInfo` 托管类型缺失**（`GC.GetGCMemoryInfo` 消费者拿不到完整数据）。
- **调试器**：CDAC/SOS 判为非目标（il2cpp 无配套 SOS，合理取舍，但意味着生产事故现场无第一方调试工具链）。
- **Heap verifier**：有轻量 `GcVerifyHeap`（region-gen 采样），无 CoreCLR 分级 + poison。`[代码 + 评估]`

### 3.6 运行时 API 与边缘能力完备度

| 能力 | CRAG | CoreCLR | 影响 |
|------|------|---------|------|
| GCHandle 类型 | ~7/13（+M12 补 REFCOUNTED/WEAK_INTERIOR） | 13 种（ASYNCPINNED/SIZEDREF/WEAK_NATIVE_COM/CROSSREFERENCE/R2R…） | 边缘 COM/诊断场景 |
| HandleTable clump 代剪枝 | 分类型 iterators 被**收敛移除（回退）** | COMPUTE_CLUMP_MASK | 大 handle 表扫描开销 |
| vxsort SIMD 排序 | 无 | AVX2/AVX512/NEON | compact 阶段吞吐 |
| Frozen/RO segment | 无 | 有 | NativeAOT 只读堆 |
| SoftwareWriteWatch | 无（非目标） | 有 | AOT 专用，可接受 |
| no_gc region 完整（finalizer 回调阈值） | 部分 | 完整 | 边界语义 |
| 增量三色 GC | 无（**非缺口**：CoreCLR 亦 SATB-based） | SATB | — |

### 3.7 性能与可扩展性

| 项 | CRAG 实测 | CoreCLR | 差距 |
|----|-----------|---------|------|
| 大堆 Full GC 暂停（100MB+） | ~130ms（mark 90ms + compact 80ms 量级） | ~50ms（BGC） | 🟠 2-3x |
| 并行 worker 上限 | 8（高端设备可能不足，评估文档自标） | 动态 | 🟡 |
| Page pool 返回 OS | 延迟（pool 超限 + STW trim） | RetainVM/ConserveMem 策略化 | 🟡 内存平台期 |
| 分配快路径 | inline ~10 指令 bump | 同量级 | ✅ |
| 暂停分布 | Young ~350µs；Full 波动大（5µs–150ms） | BGC 下长暂停罕见 | 🟠 |

> 注意：以上为 Debug 构建实测值（wiki 24 号文档），**无 Release 构建基准、无跨平台基准**，性能结论只可作相对参考。

### 3.8 治理、文档与可信度

- **STATUS "✅ 完成" 与验证闭包矛盾**：`dll-matrix.json` 13/14 not-started、closure 0%，而 STATUS 标 25 assembly ✅ Passed —— 宣称完成度高于可证实完成度，这是全仓第一根因（跨域，非 GC 独有）。
- **文档与源码漂移**：架构文档引用的 `gc_handle.h/cpp` 独立文件不存在（GCHandle 已并 `engine_lifecycle.cpp`）；世代感知 handle iterators 实际已收敛移除；`GcEndStubbornChange` 占位注释（"incremental in later milestones"）与现状不符。
- **known-fail 纪律改善中但未闭环**：no-skip 规则已建立（RUN + 标 FAIL），但 `bgc_root_scan` 挂起条目长期存在，`|| true` 让夜间门形同虚设。

---

## 四、与 CoreCLR 对齐度量化（2026-08-12 口径 + 08-14 增量）

| 能力域 | CoreCLR 项 | 已对齐 | 完成率 | 备注 |
|--------|-----------|--------|--------|------|
| 内存管理/分配 | 6 | 3 | 50% | +M15；Page 回收/FindPage 已闭合（08-12 后） |
| 写屏障/卡表 | 5 | 3 | 60% | +M10 condemned；世代 `_dst_ref` 缺压力级验证 |
| 并发 GC | 4 | 3 | 75% | +M5-A 两快照；provisional 半成品 |
| 分代/晋升 | 3 | 3 | **100%** | +M9 三代 / M8 plan-gen |
| 动态调优/配置 | 3 | 0.5 | 17% | 24/75 旋钮；伺服未闭环 |
| 句柄/弱引用 | 2 | 1 | 50% | ~7/13 型；剪枝回退 |
| 诊断/可观测 | 4 | 1 | 25% | 9 ETW ID；无 BGC 阶段族 |
| **核心 7 域总计** | **~27** | **~14.5** | **~55%** | 功能完整度口径 |

**双视角结论**：按"核心架构语义"（分代/并发/写屏障/精确扫描/压缩骨架）对齐度 **~80%+**；按"生产完整度"（验证深度 + Server 工程 + 缩放调优 + 边缘能力）**~55%**；考虑 CI 护网缺口与 known-fail 残余，**"生产可依赖度"实际 40-50%**。

---

## 五、差距分级汇总表

### 🔴 P0 —— 正确性/生产必须（不闭合则不能背书生产）

| # | 差距 | 状态 | 证据 |
|---|------|------|------|
| P0-1 | A2b 并发窗口结构性消除 + stress 测试进快门禁（摘 `-LE` 或至少"必跑必红"） | 止血已做，窗口未结构消除 | `[commit 33697f00d/cf0a69aec]` |
| P0-2 | BGC root-scan 挂起 known-fail 根治（或降级为真实语义缺口并记录） | open | `known-failures.integration.yaml:13` |
| P0-3 | 并发/stress 夜间门禁 `\|\| true` 去除，失败必须红门 | open | `gc-ci.yml:121` |
| P0-4 | Server GC 至少一条 CI 门（先不默认 ON，但要一行覆盖 + Verify 覆盖 Server） | open | `gc_features.h:38` |
| P0-5 | GC 接入 ASAN（nightly 全量）+ TSAN（并发 stress） | open | `cmake/sanitizers/` 闲置 |

### 🟠 P1 —— 能力/性能（工业化工程闭环）

| # | 差距 | 状态 |
|---|------|------|
| P1-1 | 世代写屏障压力级验证（M2，多线程 ref-store + young GC 压力场景） | 已建测试但依赖 P0-1 结论 |
| P1-2 | dynamic_tuning 伺服闭环 + BGC servo（E1/M6/H3） | open |
| P1-3 | Dynamic Heap Count（M3B） | open |
| P1-4 | provisional 完整形态（gen1 强制 compact + NGC2 排队，M4） | 部分 |
| P1-5 | Verify 分级 + poison + 分配触发 GC 入口（G4） | open |
| P1-6 | L1 卡表 realloc 并发读指针稳定化（swap 前栅栏/引用计数/读写锁；seg_idx 重索引） | open |
| P1-7 | Release 构建 GC 基准（暂停/吞吐基线，替代 Debug 值） | open |
| P1-8 | 双测试树过滤统一（`-R "chaos_gc_|test_gc_"`） | open |

### 🟡 P2 —— 边缘/可观测/工程

| # | 差距 | 状态 |
|---|------|------|
| P2-1 | ETW 事件扩容（BGC 阶段族、FullGCNotify、per-GC 原因位图） | open |
| P2-2 | BCL `GCMemoryInfo` 托管类型 | open |
| P2-3 | GCHandle 13 型全量 / HandleTable clump 剪枝回补 | open |
| P2-4 | 配置旋钮 24→~75 + 分代 HardLimit | open |
| P2-5 | vxsort / Frozen segment | backlog |
| P2-6 | profile 驱动调参任务重启（hanging 任务 `gc-phase3-industrialization`） | hanging |
| P2-7 | 文档-源码同步（`gc_handle.h/cpp`、handle iterators、占位注释） | open |
| P2-8 | 验证可信度治理（nativeProof 0/2571、STATUS 与 dll-matrix 对齐） | 全仓治理 P0 |

---

## 六、工业化路线建议（按风险 × 成本排序）

**第一步（1-2 周，正确性护网闭合）—— 全部 P0：**
1. A2b 结论固化：将 `gc_region_barrier_stress_test` 从 `stress` label 拆出或建立"快门禁必跑 + 必红"通道（`-LE` 语义修正 + `|| true` 移除）；
2. BGC root-scan 挂起专项（这是并发 GC 唯一未绿基线，也是 BGC hang 历史的直接残留）；
3. Server GC 冒烟进 CI（一行覆盖 + Verify 补 Server 分支）；
4. ASAN nightly 档（GC 单测全量跑，成本低收益最高）。

**第二步（2-4 周，工程闭环）—— P1 中性价比最高的：**
5. L1 卡表并发读稳定化（纯 C++ 修改，低风险）；
6. 世代写屏障压力场景收敛为确定性测试；
7. Release 构建 + 暂停/吞吐基准（替代 Debug 数值，让性能差距可度量）；
8. dynamic_tuning 伺服（接 24 旋钮已有框架，逐步闭环）。

**第三步（中长期，能力拉平）—— P1 剩余 + P2：**
9. Dynamic Heap Count、provisional 完整、Verify 分级；
10. 可观测性扩容（ETW/BGC 阶段族/BCL GCMemoryInfo）；
11. profile 驱动调参（重启 hanging 任务，接 Release 基准）。

**护栏**：每项改动遵守三约束（多平台纯 C++ / JIT+AOT 同符号 / 热更兼容入口）；保留域卸载 O(region) 差异化，不做破坏它的对齐。

---

## 七、结论

CRAG GC 是一套**架构水准高于其工程成熟度**的分代回收器：

- **值得肯定**：核心 GC 语义（精确扫描、并发/并行、世代写屏障、压缩、域卸载）已达"学术级→工业级骨架"的 80%+；近期（08-13/08-14）A2b 翻案 + g_heap_base 真实根因修复表明**正确性收敛在真实推进**；56 个 CTest 目标 + 17 场景 stress 是实打实的资产。
- **核心差距一句话**：**"串行/单线程能力面已相当完整，但并发正确性（A2b 窗口、BGC root-scan）、Server GC 工程、CI 护网（stress 门禁外 + `|| true` + 零 sanitizer）三者既未闭合也未受守护"** —— 最危险的缺陷恰好在 CI 门外。
- **量产判定**：**当前不适合作为生产负载的默认运行时**；建议按第六章第一步先闭合正确性护网与 CI 门禁（1-2 周量级），再谈性能拉平与能力全等。达到"生产可依赖"的判定标准：known-fail 清零、stress 进快门禁且必红、Server GC 有 CI 覆盖、ASAN 常态绿、Release 基准存在。

---

## 附录：事实核查清单

| 事实 | 结论 | 出处 |
|------|------|------|
| 配置旋钮数 | 24 个 | `gc_config.h` CHAOS_GC_CONFIGURATION_KEYS 全表（本报告实读） |
| 进程内 GC 事件 | 11 种 GcEvent | `gc_events.h`（本报告实读） |
| ETW 事件 ID | 9 个（Windows-only） | `gc_etw.cpp:35-43`（本报告实读） |
| Server GC 默认 | OFF（=0） | `gc_features.h:38`（本报告实读） |
| chaos_is_gc_pointer | 已含 nursery 回退 | `gc_api.h:105-128`（本报告实读） |
| BGC root-scan | known-fail（~392s 挂起） | `known-failures.integration.yaml:13-14`（本报告实读） |
| A2b 摘除 known-fail | 2026-08-13（测试谓词缺口）+ 08-14 真实根因（g_heap_base 覆写）修复 | commit `33697f00d` / `cf0a69aec`（本报告实读） |
| stress 快门禁排除 | `-LE benchmark\|stress\|soak`；夜间 `\|\| true` | `gc-ci.yml:72-78,121`（本报告实读） |
| GC CI 矩阵 | 仅 Windows + Debug | `gc-ci.yml:50-54`（本报告实读） |
| 单测规模 | 56 个 CTest 目标（新树 `test_gc_*`） | `[08-13 风险评估 §6]` |
| 大堆 Full GC 暂停 | ~130ms（Debug 实测） | wiki 24 号文档 + 08-12 评估 |
| 对齐度 | 骨架 ~80%+ / 功能 ~55% | `[08-12 评估]` + 08-14 增量 |
