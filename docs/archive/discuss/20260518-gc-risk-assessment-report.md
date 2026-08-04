# CRAG GC 风险评估报告

## 概述

本报告对 Chaos IL2CPP CRAG GC 进行完整的横向对比分析，对照 CoreCLR GC（.NET 8+）、Mono SGen、Unity IL2CPP Boehm GC 三个业界参考实现，识别缺陷和风险。

---

## 1. 架构对比矩阵

| 维度 | CRAG GC | CoreCLR GC (.NET 8+) | Mono SGen | Unity IL2CPP (Boehm) |
|------|---------|---------------------|-----------|---------------------|
| **代数** | 2 代（Nursery + Old） | **3 代**（Gen0/1/2） | 2 代（Nursery + Major） | **无代数**（非分代） |
| **分代复制** | Cheney BFS Copying | Gen0/1: 提升/降级复制 | Nursery: Copying | 无（全量 mark-sweep） |
| **老年代算法** | Mark-Sweep-Compact | Mark-Sweep-Compact | Mark-Sweep + 可选 Evacuation | (不存在老年代) |
| **大对象堆** | LOH (>85KB, mark-sweep) | LOH (>85KB, mark-sweep) | LOS (>8KB, mark-sweep) | (无独立 LOH) |
| **精确扫描** | 精确（GcLayout）+ 保守回退 | **全精确**（GCInfo + stack map） | **大部分精确**（堆精确，栈保守） | **全保守** |
| **并行标记** | ✅ GcWorkerPool work-stealing | ✅ 并行 mark/plan/relocate | ✅ marksweep-conc-par | ❌ |
| **后台 GC (BGC)** | ✅ 并发标记 + STW 暂停 | ✅ 并发标记 + Foreground FGC | ✅ marksweep-conc | ✅ Incremental (非并发) |
| **SATB 写屏障** | ✅ SATB pre-write barrier | ✅ SATB pre-write barrier | ❌ (Incremental update) | ❌ (Incremental mode 有) |
| **卡片表** | ✅ 二级（L1 64K+L2 per-segment） | ✅ 写屏障 + card table | ✅ Remembered set | ❌ |
| **压缩** | ✅ 页内 + 跨页 evacuation | ✅ Bump pointer + demotion | ✅ Evacuation >66% 碎片 | ❌（非搬移） |
| **安全点协议** | 每线程 seq/ack/event + APC | 每线程 suspend + handler | 全局 stop-the-world | 全局 stop-the-world |
| **TLAB** | ✅ TLS bump-pointer, 256KB | ✅ Thread Local Allocation Buffer | ✅ TLAB, 4KB | ❌（无） |
| **POH (固定对象堆)** | ✅ 独立 POH region | ✅ Pinned Object Heap (LOH-like) | ✅ Pinned Chunks | (保守 GC 天然"固定") |
| **终结器线程** | ✅ 专用线程（不阻塞 BGC） | ✅ Finalizer thread | ✅ Finalization queue | ✅ Finalization queue |
| **弱引用处理** | ✅ Strong/Weak/Pinned/Dependent | ✅ Strong/Weak/Pinned/Dependent + TrackResurrection | ✅ 完整 | ✅ WeakReference |
| **GC 句柄表** | ✅ GcCreateHandle / GcFreeHandle | ✅ HandleTable (多类型) | ✅ MonoGCDescriptor | ✅ GCHandle（有限） |
| **域隔离** | ✅ MemoryDomain + gc_allocated_bytes | ❌（.NET 无 domain 概念） | ✅ AppDomain（EOL） | ❌（无） |
| **弹性 Nursery** | ✅ EMA 生存率自适应, 128KB-1MB | ✅ 基于分配预算 | ✅ 可配置（power of 2） | N/A |
| **GC 事件/统计** | ✅ Ring buffer + 直方图 + 快照 | ✅ ETW events + API | ✅ DTrace | ❌（有限 profiler 计数） |

---

## 2. 功能完成度评估

基于文档/wiki/源代码的综合评估（占比：架构 20% + 代码成熟度 40% + 测试覆盖 25% + 文档 15%）：

| 模块 | 完成度 | 评估依据 |
|------|--------|----------|
| RegionManager（区域管理） | 95% | 完整实现；free list 最多保留 16 项（非 CoreCLR 的按需释放） |
| 年轻代收集 | 90% | Cheney BFS + 卡片扫描；保守回退精度受限（EstimateObjectSize 上限 2048） |
| 老年代 Mark-Sweep | 95% | 并行 mark/sweep/compact 完整；分离 init_mutex_ 设计干净 |
| 大对象堆 | 90% | AUTOMATIC compact；max 4 free segments（非 CoreCLR 完整段的标记清理） |
| 后台 GC (BGC) | 85% | SATB + 并行 worker + 冻结协议完整；**无单元测试验证对象存活** |
| 并行标记 | 85% | work-stealing CHUNK；std::mutex 保护存在线程争用（CoreCLR 用无锁 deque） |
| 卡片表 | 95% | 二级动态增长；ClearAllCards O(allocated_segments) |
| 精确根扫描 | 80% | GcSlotMap + 保守回退；**无代码生成端到端验证**（codegen verification failed） |
| GC 调度器 | 85% | EMA 自适应；50ms rate limit；**无 GC 饥饿恢复测试** |
| GC 句柄 | 80% | Weak/Dependent 在 standalone 模式不可用；**需 engine 集成 EPHEMERON_GC** |
| POH | 90% | 独立 region bump allocator；固定对象的正确性已验证 |
| 终结器 | 80% | 基本 + 复活；**无并发终结+ BGC 测试** |
| GC 统计/事件 | 90% | 环+直方图+快照；回调函数表存在 |
| GC API (System.GC.*) | 70% | **codegen verification failed**；GC.GetTotalMemory 等无运行时验证 |

**综合完成度：86%**

---

## 3. 与 CoreCLR GC 的关键差距（高风险）

### P1: 代码生成集成未经验证
- **状态**: `verification/foundation-dll/garbage-collection/` 的 unified verification 报告显示 `codegen failed`（entry.exe 构建失败）
- **风险**: 所有 6 个 GC 相关 managed API（Collect, WaitForPendingFinalizers, KeepAlive, GetGeneration, CreateInstance, GetTotalMemory）从 managed 通过 AOT 代码调用的路径**从未被端到端运行验证**
- **影响**: 即使 native GC 实现正确，managed→native 的桥接可能因为参数传递、返回类型 ABI、异常处理等问题在运行时崩溃
- **对标 CoreCLR**: CoreCLR 有数十万测试用例覆盖 GC + JIT 集成（dotnet/runtime 仓库的 GC 测试套件）

### P2: 弱/依赖句柄脱离 Engine 不可用
- **状态**: `gc_handle_test.cpp` 注释明确写道 `dependent handle secondary retrieval after GC is explicitly DISABLED in standalone mode (crashes without engine-integrated EPHEMERON_GC path)`；弱句柄 nulling 同样声明不可用
- **风险**: 全部 GC 单元测试在 standalone 模式运行（无实际运行时引擎），导致**弱句柄和依赖句柄的核心 GC 行为未经过任何测试验证**
- **修复依赖**: 需要 engine 集成 EPHEMERON_GC 路径，目前无计划

### P3: BGC 对象存活未测试
- **状态**: `gc_bgc_smoke.cpp` 中测试线程未注册为 ManagedThread，所有对象被 BGC 视为不可达——测试仅验证"cycle machinery correctness (no crashes)"，**不验证对象存活**
- **影响**: BGC 并发标记的正确性（SATB 写屏障是否能防止遗漏存活对象）完全没有测试覆盖
- **对标 CoreCLR**: CoreCLR 的 BGC 有大量验证（CORPROF, GCStress, 内部断言检查 SATB 完整性）

### P4: 无代码生成的 GC 负载集成测试
- **状态**: 所有 GC 测试使用原生 C++ 分配和伪造对象头，**没有在真实 managed 代码（IL->AOT）条件下运行**
- **影响**: 分代 GC 的写屏障（codegen 发射）、精确根扫描（codegen 发射 GcSlotMap）、数组元素引用写屏障等关键路径**未以集成方式测试**
- **对标 Unity IL2CPP**: Unity 有完整运行时在 managed + native 混合场景下测试 Boehm GC

---

## 4. 与 Mono SGen 的差距（中风险）

### P5: 无 Generational Card Table 跨代累积测试
- 卡片表在年轻代收集后被 ClearAllCards 清零。如果老年代对象在两次年轻代收集之间被写入（dirty card），第二次年轻代会**丢失第一次脏卡信息**
- **修复**: CoreCLR 风格的做法是保留脏卡直到完整 GC；或使用 "card table merging" 技术。当前设计中这不是 bug（每次 young GC 扫描全部 old-gen 脏卡），但**性能问题**在大规模老年代上会指数级放大
- **对标 SGen**: SGen 使用 remembered set（更精确的跨代引用记录），避免全脏卡扫描

### P6: 并行标记线程争用
- `DrainMarkStackParallel` 使用 `std::mutex` 保护 MarkChunk 的 vector——8 个线程抢一把锁
- **对标 CoreCLR**: CoreCLR 的并行标记使用 per-worker 无锁 deque + work-stealing，无需全局锁
- **影响**: 在 8+ worker 场景下锁争用显著（压力测试中 MarkChunk 访问成为瓶颈）

---

## 5. 与 Unity IL2CPP Boehm 相比的优势

此项非缺口分析，但值得注意 CRAG GC 相对于 Unity IL2CPP（当前游戏行业事实标准）的显著优势：

| 优势 | CRAG GC | Unity Boehm GC |
|------|---------|----------------|
| 分代收集 | ✅ 2 代 | ❌ 无（每次全量扫描） |
| 压缩 | ✅ Mark-Sweep-Compact | ❌ 非搬移→碎片化 |
| 精确扫描 | ✅ GcLayout + 保守回退 | ❌ 全保守→假指针 (false positive) |
| 并发 BGC | ✅ SATB + 并行 worker | ❌ Incremental（非并发） |
| LOH | ✅ 独立 LOH + 可选压缩 | ❌（大对象与普通对象混合） |
| 写屏障 | ✅ 二级卡片表 | ❌ Incremental 模式有额外开销 |

---

## 6. 风险汇总表

| ID | 风险 | 等级 | 影响范围 | 修复难度 | 状态 |
|----|------|------|---------|---------|------|
| R1 | managed→native GC API 桥接未验证 | **P0** | 全部 GC 相关 managed 调用 | 中 | open |
| R2 | Weak/Dependent 句柄脱离 engine 不可用 | **P0** | 弱引用/GC.KeepAlive/条件弱表 | 大 | open |
| R3 | BGC 对象存活无测试覆盖 | **P1** | BGC 并发标记正确性 | 中 | open |
| R4 | 无 managed 代码 GC 集成测试 | **P1** | 写屏障/精确扫描/AOT 栈帧 | 大 | open |
| R5 | 并行标记全局锁争用 | **P2** | 多核性能（>8 线程退化） | 中 | open |
| R6 | 跨年轻代卡片累积性能退化 | **P2** | 大堆老年代年轻代暂停增长 | 小 | open |
| R7 | GC 饥饿/分配失败恢复无测试 | **P2** | 极端内存压力稳定性 | 小 | open |
| R8 | codegen verification pipeline broken | **P2** | GC 验证管线 | 中 | open |
| R9 | 终结器线程 + BGC 并发无测试 | **P2** | 终结器正确性 | 中 | open |
| R10 | 无 3 代分离（Gen 0/1/2 互斥） | **P3** | 中长期老年代碎片化率 | 大 | 已知设计决策 |
| R11 | Monitor/ThinLock + GC 交互无集成测试 | **P3** | SyncBlock 膨胀 + GC 安全点 | 中 | open |
| R12 | `GcEndStubbornChange` 显式存根 | **P3** | 增量 GC 写屏障 | 小 | 已知 C3 stub |
| R13 | 手柄表溢出无测试 | **P3** | 大量 GCHandle 场景 | 小 | open |
| R14 | LOH compaction 仅有 AUTOMATIC 模式 | **P3** | LOH 碎片化控制策略 | 小 | open |

---

## 7. 风险等级分布

```
P0: ████ 2项  — 必须修复，阻塞正式发布
P1: ████ 2项  — 高优先级，功能正确性保障
P2: ██████ 5项 — 中优先级，稳定性/性能
P3: ██████ 5项 — 低优先级，增强/边缘情况
```

CRAG GC 的**核心 native GC 引擎实现完成度很高**（85-95%），主要风险集中在：
1. **managed ↔ native 集成边界**（R1, R2, R4, R8）— codegen 路径未验证
2. **测试覆盖缺口**（R3, R7, R9, R11）— BGC、终结器并发、饥饿恢复
3. **性能扩展**（R5, R6）— 多核锁争用、卡片累积

---

## 8. 建议修复优先级（按 ROI 排序）

1. **立即（P0）**: 修复 GC verification pipeline codegen 构建失败（R8）→ 解锁端到端 GC API 验证（R1）
2. **立即（P0）**: 集成 EPHEMERON_GC 路径或为 standalone 弱/依赖句柄提供模拟引擎（R2）
3. **短期（P1）**: 添加 BGC 对象存活验证测试（R3）— 现有 gc_bgc_smoke.cpp 可以扩展
4. **短期（P1）**: 添加 managed 代码 + GC 集成测试（R4）— 至少一个完整的 AOT 编译 + 运行时 GC 场景
5. **中期（P2）**: 将并行标记的 `std::mutex` 替换为无锁 per-worker deque（R5）
6. **中期（P2）**: 实现 card table 合并策略或 remembered set 优化（R6）
7. **中期（P2）**: 扩展 gc_finalizer_test.cpp 验证终结器 + BGC 并发（R9）
