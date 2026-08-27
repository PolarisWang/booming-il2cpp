# CRAG GC 工业化差距 — 独立证据采集笔记（草稿）

> 随 24-CRAG-GC架构参考.md 等 wiki 与源码核查整理。最终评估结论会合并三个并行子代理（源码盘查 / 测试尽调 / STATUS 评审）。

## 架构基线（已核实，wiki 24-CRAG-GC架构参考.md）
- 三代分代：Nursery(256KBbump/Cheney) + Gen1(16MB survivor) + Old(28 size-class×64KB, mark-sweep+compact) + LOH(>85KB, AUTOMATIC compact)
- 写屏障：世代感知 region 写屏障(GC-K2) + 两层卡表(SATB)
- BGC 并发标记（SATB + work-stealing 并行 mark）+ per-thread handshake safepoint
- 精确扫描 GcLayout + 保守扫描堆范围过滤兜底

## Docs/Code 漂移（重要发现：代码已前进，wiki 未更新）
wiki 24-…（最后更新 2026-05-21）仍列"已知问题/剩余瓶颈/未实现"，但源码已落地：
- 【Page 回收池】gc_old_gen.h：page_pool_ + PoolEntry + kMaxPoolSize=16，MEM_DECOMMIT 池回收（非 VirtualFree，符合 fault 约束）→ wiki"page_count 无界增长"瓶颈已解决
- 【FindPage O(N)】gc_old_gen.h：PageArray 排序数组 + RebuildPageArray → O(log n) → wiki"FindPage O(N)"瓶颈已解决
- 【BGC-YoungGC segfault】gc_bgc.h：PauseForYoungGc/ResumeAfterYoungGc/DrainNurseryFromWorkDeques + bgc_pause_requested_/bgc_paused_ → G-3 协调协议已实现（wiki smoke 已知问题未同步）
- 【生产加固】存在 gc_low_mem（低内存监控/触发 GC）、emergency reserve（finalizer OOM 兜底）、finalizer watchdog（kFinalizerMaxRetries=3）、incremental marking 时间片（2ms budget）、gc_demotion、gc_coordinator/gc_heap_manager（Server GC）、gc_etw

## 测试证据缺口（子代理C已确认 + 独立核实）
- test-report.json（2026-08-14）只含 unit(dotnet) 层，GC native 集成层缺席 → 仓库产物无法证明"全部 PASS"
- CI 门禁：integration 层 `ctest_exclude: benchmark|stress|soak` → 压力测试（gc_stress/bgc_stress/loh_stress/finalizer_stress/gc_stress）默认不跑，仅 --stress-only
- baseline：known-failures.native.yaml 不存在；integration 基线记录 `chaos_gc_bgc_root_scan_test` 为已知失败（~392s 挂起/不通过，2026-08-07）→ 与 wiki"全部 PASS"矛盾
- wiki 3 个已知问题在代码是"规避而非断言修复"：bgc_smoke Test3 检测 concurrent mark 即 ForceComplete 跳过宣称 PASS；poh Test8 避免直接收集 POH；gc_handle 弱引用软断言
- 若干断言是"不崩溃"级（finalizer "verified no crash"）

## 平台覆盖
- CMakeLists（runtime-core）：Server GC 默认 OFF；thread hijacking trampoline 仅 Linux x64；Windows 用 APC；Apple 无 trampoline（iOS/Android 走 other 平台源）
- CI 原生目标仅 windows-x64-reference 单一平台

## 压测声明 vs 源码实测（子代理E + 独立核实）
- Scenario F：wiki 称 "24-core × 100 线程"，源码 kFWorkers=20、kFAllocsPerThread=20（gc_stress_test.cpp L991-992）→ 参数不符
- Scenario G：注释 "Background GC thread — DISABLED FOR DIAGNOSTIC"（L1165-1181），并交错 full GC 未真跑；wiki 称 G "interleaved with full GC cycles"
- `--new-only` 用于 "Avoid pre-existing hangs in A-K" → A-K 存在预存 hang
- 压测报告路径硬编码 `D:/agent/booming-il2cpp/artifacts/...`（非 chaos-il2cpp）→ 当前仓库无 gc_stress_report_*.json，不可复现（跨项目污染）
- gc_stats.h：pinned_objects_count / index / pause_time_percentage / gen3_size_* / frag_* 多处 always 0（GCInfo/告警空壳）
- GCMemoryInfo BCL 托管类型缺失，.NET GC 语义链断
- 已知问题：chaos_gc_bgc_root_scan_test 挂起(~392s) 被记录为已知失败(2026-08-07)，与"全部 PASS"矛盾

## CN 待补充：子代理A(源码TODO/stub)
