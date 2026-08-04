# CRAG 实施任务计划（Work Plan）

> 项目: 区域感知分代垃圾回收器（Chaos Region-Aware GC — CRAG）
> 文档: `docs/design/c-phase-region-aware-generational-gc.md`
> 开始: 2026-05
> 预估: ~20 周

---

## 阶段划分

### Stage C0: Region 框架 + Card Table（3 周）

| ID | 任务 | 文件 | 验收标准 |
|----|------|------|---------|
| C0.1 | **Region 数据结构 + RegionManager** | `src/native/runtime-core/gc_region.h` | Region 分配/释放/空闲链表正常 |
| | RegionId、RegionKind、Region 结构体定义 | | |
| | RegionManager：分配/释放/回收操作 | | |
| C0.2 | **统一分配入口 RegionAllocate** | `src/native/runtime-core/gc_region.h` | Nursery/Domain/Raw 三种 region 分配通过同一条路径 |
| | NurseryAllocate（bump pointer，inline fast path） | | |
| | DomainAllocate（per-domain region，bump） | | |
| | RawAllocate（通过 malloc 或内部 bump） | | |
| C0.3 | **Card table 运行时实现** | `src/native/runtime-core/gc_card_table.h` / `.cpp` | DirtyCard/ClearCard/ScanDirtyCards 正确 |
| | s_card_table 全局数组 + DirtyCard/IsDirty | | |
| | 堆基址注册（GcSetHeapBase） | | |
| C0.4 | **Card table codegen 插入** | `NativeAotLoweringPlanner.cs`（ArrayStore/StFld） | 所有 ArrayStore 和 StFld 后生成 write barrier stub |
| | StFld post-write barrier | | |
| | ArrayStoreReference barrier | | |
| C0.5 | **单元测试** | `gc_region_test.cpp`、`gc_card_table_test.cpp` | Region 分配/释放/跨代标记验证 |
| C0.6 | **AB 对比** | foundation-dll pipeline | 写屏障开销 ≤ 2% |

### Stage C1: 精确根扫描（3 周）

| ID | 任务 | 文件 | 验收标准 |
|----|------|------|---------|
| C1.1 | **GcSlotMap 结构定义** | `contracts/native/v0/codegen_bridge.h` | 栈帧 GC slot 描述能力覆盖 99% 方法 |
| | frame_size + num_gc_slots + slots[] 变长数组 | | |
| C1.2 | **Codegen GcSlotMap 生成** | `NativeAotLoweringPlanner.cs` | 每 managed 方法输出 GcSlotMap |
| | 栈帧中 GC 对象 slot 的追踪（LocalVarInfo → slot mapping） | | |
| | .gc_slot_maps section 输出 | | |
| C1.3 | **Runtime 精确根扫描器** | `src/native/runtime-core/gc_root_scanner.h` / `.cpp` | 遍历 managed_stack 帧链，每帧读 GcSlotMap 扫描 |
| | managed_stack 帧链遍历 | | |
| | GcSlotMap 解析 + 根报告 | | |
| | 非 managed 帧保守回退扫描 | | |
| C1.4 | **混合扫描集成到 Safepoint** | `src/native/runtime-core/thread_state.cpp` | SafepointPoll 在 GC 触发时同时执行精确根扫描 |
| C1.5 | **AB 对比验证** | foundation-dll pipeline | 精确根 vs 保守扫描的一致性（假指针比例 < 0.01% 即认可） |

### Stage C2: 分代 Nursery（3 周）

| ID | 任务 | 文件 | 验收标准 |
|----|------|------|---------|
| C2.1 | **Nursery bump-pointer 分配器** | `gc_region.h` (NurseryAllocate) | 替换 GcBumpCache 为 nursery bump pointer |
| C2.2 | **GcAllocate 重定向** | `runtime_core.cpp` | GcAllocate → NurseryAllocate + old heap fallback |
| C2.3 | **年轻代 GC（Cheney copy + promote）** | `gc_young_collector.h` / `.cpp` | Young collection 流程正确复制存活对象到 tenured region |
| C2.4 | **GcBumpCache 降级为 old heap 加速层** | `gc_bump_cache.h` | Nursery 不再使用 GcBumpCache；Old heap 可选保留 |
| C2.5 | **Card table 跨代引用扫描集成** | `gc_card_table.h` | Young GC 时扫描 dirty cards 提取 old→young 引用 |
| C2.6 | **单元测试 + 压力测试** | `gc_nursery_test.cpp` | 分配吞吐 > 当前 GcBumpCache 2x |

### Stage C3: 全功能分代 GC（4 周）

| ID | 任务 | 文件 | 验收标准 |
|----|------|------|---------|
| C3.1 | **Old heap 精确 mark-sweep** | `gc_old_collector.h` / `.cpp` | 替代 BDWGC 作为老年代回收器 |
| C3.2 | **三色标记实现** | `gc_old_collector.cpp` | 精确 mark 正确 traverse 存活图 |
| C3.3 | **Parallel mark 支持** | `gc_old_collector.cpp` | 多线程并行标记，100MB < 1ms |
| C3.4 | **老年代可选压缩（Cheney 复制）** | `gc_old_collector.cpp` | 碎片 > 阈值时触发的 compaction |
| C3.5 | **Full GC 调度策略** | `gc_scheduler.h` / `.cpp` | 自适应阈值触发 full GC |
| C3.6 | **FOH（大对象堆）管理** | `gc_old_collector.cpp` | >85KB 对象直接走 BDWGC mark-sweep |

### Stage C4: 热更新域卸载（3 周）

| ID | 任务 | 文件 | 验收标准 |
|----|------|------|---------|
| C4.1 | **域卸载跨域引用检查** | `gc_domain_unloader.h` / `.cpp` | 扫描 card table 提取 core→domain 跨代/跨域引用 |
| C4.2 | **ReleaseDomainRegions 实现** | `gc_region.h` (RegionManager 扩展) | 释放域占用的所有 DOMAIN + TENURED region |
| C4.3 | **域卸载阻塞报告** | `memory_domain.h` / `.cpp` | 返回跨域引用列表供上层决策 |
| C4.4 | **协作式卸载流程集成** | `runtime_core.cpp` | 卸载前 STW GC + region release 原子完成 |
| C4.5 | **HotUpdate pipeline 集成测试** | foundation-dll family (hotupdate) | 卸载后域完全不可访问 |

### Tuning + Hardening（4 周）

| ID | 任务 | 验收标准 |
|----|------|---------|
| T.1 | Nursery 自适应调整（根据分配率动态扩展/收缩） | GC 频率 < 50次/s |
| T.2 | Parallel mark 性能优化（prefetch + work stealing） | 100MB old heap < 1ms mark |
| T.3 | 压力测试（100 线程、100MB 堆、高强度分配） | 无 crash、无泄漏 |
| T.4 | Profile 埋点 + GC 统计工具 | ALLOC|format 输出完整 |
| T.5 | AB 模式 CI 集成（RuntimeConfig.GcMode） | 一键切换 PhaseB / PhaseC 模式 |

---

## 关键里程碑

| 里程碑 | 时间 | 交付物 | 可回退 |
|--------|------|--------|--------|
| **M0** (C0.1-C0.3) | Week 3 | Region 框架 + Card table runtime | ✅ BDWGC 完全不受影响 |
| **M1** (C0.4-C0.6) | Week 4 | Codegen barrier + AB 验证通过 | ✅ barrier 可关闭 |
| **M2** (C1.1-C1.5) | Week 7 | 精确根扫描 + 混合扫描上线 | ✅ 保守回退 |
| **M3** (C2.1-C2.6) | Week 10 | 分代 Nursery 上线，暂停 < 500μs | ✅ GcBumpCache 可恢复 |
| **M4** (C3.1-C3.6) | Week 14 | 完整分代 GC，不再依赖 BDWGC | ⚠️ 依赖 C2 稳定 |
| **M5** (C4.1-C4.5) | Week 17 | 热更新域卸载 ~5ms | ⚠️ 依赖 C3 稳定 |
| **M6** (T.1-T.5) | Week 20-22 | 生产就绪 + CI 集成 | — |

---

## 风险注册表

| 风险 | 阶段 | 缓解措施 |
|------|------|---------|
| Codegen GcSlotMap 生成工作量超预期 | C1 | 第一期只生成关键方法的 slot map，其余保守回退 |
| Write barrier 导致 10%+ 性能回退 | C0 | Post-write barrier 可关闭（codegen compile flag） |
| Nursery GC 频率过高导致性能倒退 | C2 | 自适应 nursery 大小 + 可调阈值 |
| Old heap 精确 mark-sweep fragment 问题 | C3 | 保留 BDWGC 作为可选的 old heap 后端 |
| 域卸载引用检查阻塞所有域无法卸载 | C4 | 提供"强制卸载"模式（风险自负） |