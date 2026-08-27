# Chaos IL2CPP — CRAG GC 完成度与工业化差距综合评估报告

> 评估范围：`src/native/runtime-core/gc/`（CRAG GC）及其测试/验证/文档生态
> 评估方式：wiki 架构文档 + 源码核验 + CI/测试/基线交叉核对（glob/grep/pwsh 在本环境不可用，结论基于定向阅读的证据）
> 评估日期：2026

---

## 〇、一句话结论

CRAG GC 的**架构设计与功能内核是真实、高水平、大体完成**的（分代 + Region + 精确扫描 + 并发 BGC + 两层卡表 + 并行压缩 + 热更域隔离均落地），但从"**可交付、可运维、可被独立验收**"的工业化生产标准看，**尚差约 60%**：决定性短板不在"功能有没有"，而在"**验证可信度、可观测/兜底、文档审计、平台覆盖**"四大工业化支柱。

```

架构/功能完整度  ▆▆▆▆▆▆▆▆▆▂  约 90%（实现层）  /  约 60%（交付层）
可靠性验证闭环    ▆▆▆▄▂▂▂▂▂▂  约 40%（stress 被削弱/断言软化/报告不可复现）
可观测与兜底      ▆▆▆▂▂▂▂▂▂▂  约 35%（无 perf counter/泄漏检测/自愈，GCInfo 空壳）
文档审计          ▆▂▂▂▂▂▂▂▂▂  约 25%（单文档自评、无 STATUS、无 sign-off）
工业就绪综合      ▆▆▆▄▃▂▂▂▂▂  约 35%~45%
```

---

## 一、已完成且经过源码核验的（真实、成熟）

以下均有对应 native 实现并经本人/子代理定向读取确认，非文档臆想：

| 能力 | 证据（文件 + 实现） |
|------|------|
| 三代分代 GC | Nursery(256KB bump/Cheney) + Gen1(16MB survivor) + Old(28 size-class × 64KB, mark-sweep+compact) + LOH(>85KB, AUTOMATIC compact) |
| 精确扫描 | GcLayout 注册表 + TypeInfo 地址验证 + 保守扫描堆范围过滤兜底（gc_region.h/gc_layout.*） |
| 世代感知写屏障 | `chaos_gc_dirty_card_dst_ref` + 两层卡表 + card bundle（gc_region.h:g_region_to_gen, gc_card_table.*） |
| 并发 BGC | SATB 写屏障 + work-stealing 并行 mark + 事件驱动唤醒 + incremental marking 时间片（gc_bgc.h） |
| Per-thread handshake 安全点 | suspend_seq/suspend_ack/suspend_event + APC 回退 + event wait（thread_state.cpp） |
| Page 回收池 | `page_pool_`/PoolEntry/kMaxPoolSize=16，MEM_DECOMMIT 池回收（非 VirtualFree）（gc_old_gen.h） |
| Page O(log n) 索引 | `page_array_` 排序数组 + RebuildPageArray（gc_old_gen.h） |
| BGC-YoungGC 协调 | `PauseForYoungGc`/`ResumeAfterYoungGc`/`DrainNurseryFromWorkDeques` + bgc_pause_requested_/bgc_paused_（gc_bgc.h） |
| 生产加固 | 低内存监控(gc_low_mem.cpp)、finalizer OOM 应急池(emergency reserve)、finalizer watchdog(kMaxRetries=3)、堆硬/软上限(GC_HEAP_*_LIMIT_MB)、demotion(gc_demotion) |
| 热更域隔离 | MemoryDomain + 域卸载器 + 跨域引用扫描 + per-domain GC 分配追踪 |
| 诊断 | gc_stats/GcDumpStats/GCNotification 回调/gc_etw(Windows) |

**关键洞察**：代码已明显**走在 wiki 文档前面**。24-CRAG-GC架构参考.md（最后更新 2026-05）仍把"page_count 无界增长 / FindPage O(N) / BGC-YoungGC segfault"列为已知问题或剩余瓶颈，但这些在源码里已被解决（上述表）。文档欠更新。

### 1.5 源码层面完成度（子代理A 逐文件核验）
- **实测完成度约 80~85%**：~21 个源文件真实可编译可运行，核心管线（TLAB/region 分配、Cheney 新生代复制、老年代 mark-sweep+compact、两级 card 写屏障、协作/抢占 safepoint、调度器、BGC、Gen1、LOH 分配/sweep、stats/API/config）均齐备非空壳。
- 源码级显式占位/裁剪：
  - `gc_loh.cpp:40-43 Init()` 真·空壳（仅 LOG+return true），`GcConfig().LohThreshold` 未接线；
  - **增量 GC 未闭环**：`gc_old_gen.cpp:3038` CollectFull 标注"C3+ will replace with incremental slicing"，`GcEndStubbornChange` 为 no-op C3 stub；
  - `RecordGen1Allocation` 空函数体；`DefaultDeallocate`/`ExecuteSingleHeapGc` 空实现；
  - Server GC 下并行 mark 退化为顺序（非可重入风险），full-GC 并行 sweep 被禁（ASan 在 CoalescePage 报 UAF 改顺序兜底）；
  - **Windows 无可靠寄存器根捕获**（`gc_num_gprs=0`），STW 根扫描仅栈槽；
  - `gc_stress.*` 组件缺失（未见 `CHAOS_GC_STRESS_ENABLED` 开关）；
  - iOS/Other 平台无真实 NUMA（仅 win/linux 有实现）。
- **新增源码风险**：safepoint 静态共享变量（s_set_epoch/s_remaining、s_callback）依赖"owner CAS 唯一 GC 线程"假设 → 嵌套/异常第二条 GC 路径即数据竞争；BGC grey deque/SATB 全由 mutex 守护、`StealChunk` 用 `erase(begin())` O(n)，并非文档声称的"无锁"work-stealing。

---

## 二、已证实的差距（冲突/注水/未闭合）

### 2.1 "全部 PASS / 完成度 100%" 与可验证事实冲突
- **native 层测试证据在当前仓库缺失**：`tests/runner/test-report.json`（2026-08-14）**只含 unit(dotnet codegen) 层**，GC 所在的 native 集成层完全缺席，仓库产物**无法证明** wiki 声称的"全部 PASS"。
- **明确已知失败**：`tests/runner/baselines/known-failures.integration.yaml` 记录 `chaos_gc_bgc_root_scan_test` 为已知失败（极慢 ~392s、仍挂起/不通过，2026-08-07）——这是未解决的 GC 问题，与"全部 PASS"直接矛盾。
- **wiki 附录自曝已知问题**（行820-827）：POH 直接 `Collect()` 会挂起、BGC YoungGc segfault（预存，隔离到末尾）、pinned 标记后继校验不一致、gc_region_test tls 假设。

### 2.2 压测声明"注水"（文档 vs 源码实测，独立核实）
| wiki 声称 | 源码实测 |
|------|------|
| Scenario F "24-core × 100 线程" | `gc_stress_test.cpp:991` `kFWorkers=20, kFAllocsPerThread=20`，无 100 线程/24-core 绑定 |
| Scenario G "interleaved with full GC cycles" | `gc_stress_test.cpp:1165-1181` 后台 GC 线程 `DISABLED FOR DIAGNOSTIC`（整体注释掉） |
| A→Q 17 场景 | 实际 A–T 20 场景（文档滞后）；且 `--new-only` 注"**Avoid pre-existing hangs in A-K**" |
| 压测报告可复现 | 报告/转储路径硬编码 `D:/agent/booming-il2cpp/artifacts/...`（**非本仓库**）→ chaos 仓库读不到任何 `gc_stress_report_*.json`，为跨项目污染 |

### 2.3 已知问题以"规避"取代"修复"
经代码核验：
- **BGC-YoungGC segfault**：`gc_bgc_smoke.cpp:416` 注明预存 segfault、"kept last for isolation"；`:191/:238` 检测到 concurrent mark 即 `ForceComplete` 跳过并宣称 PASS——**不真正做 interleaved 压力**。尽管 `gc_bgc.h` 已有 G-3 协调协议，该恢复未反映到测试端。
- **POH FullGc 挂起**：`gc_poh_test.cpp` 注释直接说明"会挂起"，于是**只做 young-GC 压力，不直接收集 POH**。
- **pinned/弱引用**：`gc_handle_test.cpp` 弱引用断言软性（失败只打 INFO 不 FAIL）；pinned 地址稳定性断言仅在有值时执行 → **核心语义未被硬断言**。

### 2.4 .NET 语义兼容断链
- `GC.GetGCMemoryInfo()` 托管侧 `GCMemoryInfo` **类型缺失**（wiki 自述行502/727），且 `gc_stats.h` 多项（pinned_objects_count / index / pause_time_percentage / gen3_size_* / frag_*）**硬编码 always 0** → GCInfo/告警/剖析是空壳。

---

## 三、工业化生产的决定性缺口（不在"功能"，在"生产三要素"）

| # | 缺口 | 现状 | 工业化要求 |
|---|------|------|-----------|
| 1 | **可靠性验证可信度** | stress 参数与文档不符、并发 GC 交叠被禁用、报告产物流向外项目、已知 segfault 被隔离规避、native 证据缺失 | 第三方/CI 长期压测 + 达标基线 + 可复现报告 |
| 2 | **可观测/告警/兜底** | 进程内计数器 + 仅 Windows ETW + OOM 事件 hook；**无 perf counter、无泄漏检测、无存活剖析、无跨进程导出、无自愈/降级策略** | 多平台可观测 + 告警 + 泄漏检测 + 稳定兜底 |
| 3 | **文档审计** | 40+ 项 100%、R1-R16 全关、横向 8.71/10 几乎全部集中在单篇自评 wiki，无独立 STATUS、无评审/sign-off、无交叉验证 | 设计 + 评审 + 验收 + 合规记录 |
| 4 | **平台/并发完整性** | Thread Hijacking 仅 x64；Server GC 默认 OFF 且多 NUMA OOM 未测；CI 原生目标仅 windows-x64-reference 单平台 | iOS(ARM64)/移动端/JIT-禁环境兜底；多平台 CI |

---

## 四、未验证 / 需实际压力验收的项
1. "3×100 线程长期、24-core、~130ms 对标"性能声明——无真实基准（BDN/SPECjbb/游戏负载），只有自写 stress 自证 + 参数不符。
2. `chaos_gc_bgc_root_scan_test` 挂起根因未闭环。
3. 三项剩余演进方向（Region O(R) 扫描、值类型嵌套写屏障假阳性、GCMemoryInfo BCL）无 P1 排期。
4. ETW/PerfView 采集链路可复用性、iOS/ARM64 真机表现未实证。

---

## 五、定量的工业化就绪评分

| 维度 | 权重 | 现状 | 得分 |
|------|------|------|------|
| 功能/架构内核 | 40% | 实现层 ~80-85%（增量 GC/LOH Init/寄存器根/平台 NUMA 未闭环），交付层存在 .NET 语义断链 | 8.0 |
| 可靠性/压力验证 | 30% | stress 被削弱、断言软化、报告不可复现 | 3.0 |
| 可观测/运维/兜底 | 15% | 缺失 perf counter/泄漏检测/自愈，GCInfo 空壳 | 3.0 |
| 文档/审计/合规 | 15% | 单文档自评、无 sign-off | 2.5 |
| **工业就绪综合** | | | **≈ 5.0 / 10** |

> 换一种表述：**功能内核（约 40~50% 权重）已到位大半；但可靠性验证、运维闭环、文档审计三大工业化支柱基本缺失/自证。**

---

## 六、建议的优先级路径（把 5.0 → 8.0）
1. **P0 可信度重建**：修掉 `chaos_gc_bgc_root_scan_test` 挂起；把 GC 原生测试接入 driver，产出可复现的 native 层 `test-report.json`；压力场景参数与文档对齐、恢复 Scenario G 交叠 GC、报告路径改回本仓库。
2. **P0 验证闭环**：GC stress/stress-matrix 纳入 CI（或 nightly 常驻门禁），跑真实多核长时间回归 + 达标基线。
3. **P1 可观测**：补 GC perf counter / 内存告警 / 泄漏检测钩子；补齐 GCMemoryInfo 托管类型并用真实值填充（去 always 0）。
4. **P1 平台**：iOS/ARM64 Thread-Hijack 兜底、Server GC 多 NUMA OOM 测试、多平台 CI。
5. **P2 文档治理**：建立独立 STATUS.md + 评审/sign-off，修正 24-CRAG 文档与源码的漂移（已知问题已解决的同步更新）。

---

## 附：取证边界与证据索引
- 本环境 glob/grep/pwsh 崩溃(3221225794)，仅 read 可用；未能枚举 wiki/docs 全目录、未能读到独立 STATUS.md/roadmap/压测报告产物（这一点本身也是"审计痕迹缺失"的证据）。
- 关键证据：`wiki/.../24-CRAG-GC架构参考.md`、`wiki/.../CRAG-GC-Phase3-并发能力.md`、`wiki/.../CRAG-GC-ETW诊断事件.md`、`src/native/runtime-core/CMakeLists.txt`、`gc_region.h`、`gc_old_gen.h`、`gc_bgc.h`、`gc_low_mem.cpp`、`gc_stress_test.cpp`(L991-992, L1165-1181)、`tests/runner/test-report.json`、`tests/runner/baselines/known-failures.integration.yaml`、`tests/suite_contract.yaml`(ctest_exclude)、`scripts/ci_test.py`。
