# CRAG GC 工业化后续路线图

> 基于 brainstorm 设计摘要（2026-05-23），全串行推进。
> 目标：解决 C1-C20 全部差距项，CRAG GC 达到 CoreCLR 对等工业化程度。

## 目标

在已完成工业化路线图（17 子任务）基础上，修复 20 个差距项（P0×3、P1×4、P2×8、P3×5），贯穿正确性修复、引擎硬化、可观测性、平台扩展、性能优化五大阶段，使 CRAG GC 综合工业化评分从 7.5/10 提升到 9.0+/10。

## 范围边界

- **代码范围**：`src/native/runtime-core/gc/` + 配套的 BCL 侧/诊断/平台适配/AOT codegen
- **测试范围**：`testing/src/native/runtime-core/gc/` + `contracts/` + `testing/verification/`
- **文档范围**：`wiki/03-功能模块/08-GC子系统/`
- **非目标**：不重新设计 GC 架构；不改变 MemoryDomain/域卸载；不改 core interpreter 架构

## 非目标

- 不引入新的 GC 算法（如三代分代是改良 Gen1，不是重写 full GC）
- 不改动 codegen/interpreter 核心调度架构
- 不改动热更新主流程

## 全局优先级评估

| Phase | 方案选型 | P1 正确性 | P2 性能 | P3 架构 | 裁决 |
|-------|---------|-----------|---------|---------|------|
| A | 生产正确性修复 | 正面（消除 segfault/时序/codegen 断链） | 无影响 | 正面（测试覆盖加固） | PASS |
| B | GC 引擎硬化 | 正面（消除竞态/泄漏/线程爆炸） | 正面（lock-free 改善并行） | 正面（架构干净） | PASS |
| C | 可观测性与 API 完整 | 正面（诊断准确） | 无影响 | 正面（API 完整） | PASS |
| D | 平台扩展 | 正面（ARM64 正确性） | 正面（Server GC 吞吐量） | 无影响 | PASS |
| E | 性能优化与高级功能 | 正面（消除假阳性） | 正面（card bundle/亲和性/NUMA） | 正面（三代分代对齐） | PASS |

## 阶段列表

### Phase A — 生产正确性修复

- **goal**: 修复 P0 阻塞项，消除生产崩溃/测试红/集成断链
- **exit_criteria**: A1/A2/A3 全部实现 + 对应测试 0 failures + 回归套件全 PASS + BGC-YoungGC SEH 隔离验证通过（不再随机 crash）
- **deliverables**: 修复后的 gc_bgc.cpp/gc_young_collector.cpp + verification pipeline 完整运行报告
- **dependencies**: 无（基线已稳定）
- **resolved_decisions**: 无
- **watch_items**: A3 codegen pipeline 可能暴露 codegen 侧隐藏缺陷

### Phase B — GC 引擎硬化

- **goal**: 消除引擎层竞态条件、锁争用、内存泄漏、线程模型缺陷
- **exit_criteria**: B1-B5 全部实现 + 全部 GC 测试 0 failures + stress 套件（含 SATB/bgc 17 场景）全 PASS
- **deliverables**: lock-free parallel mark、Finalizer thread pool、并发根扫描集成
- **dependencies**: Phase A（基线稳定 + codegen pipeline 可运行）
- **resolved_decisions**: B 组内部串行（B1→B2→B3→B4→B5）
- **watch_items**: B2 lock-free deque 改造可能引入新的并发 bug；需 stress 长时间验证

### Phase C — 可观测性与 API 完整

- **goal**: LatencyMode 语义完整、GCMemoryInfo 托管侧、增量 GC stub 补全
- **exit_criteria**: C1-C3 全部实现 + 对应测试 0 failures + GCMemoryInfo 托管侧通过 codegen verification
- **deliverables**: 托管 GCMemoryInfo 类型、LatencyMode 完整行为、增量 GC 写屏障
- **dependencies**: Phase A（codegen pipeline 就绪）
- **resolved_decisions**: 无
- **watch_items**: GCMemoryInfo 托管侧需要 BCL 类型定义，可能涉及新的 managed→native marshalling

### Phase D — 平台扩展

- **goal**: Windows ARM64 编译验证 + Server GC 生产级硬化
- **exit_criteria**: D1: Windows ARM64 编译通过 + ARM64 写屏障单元测试 PASS；D2: Server GC 多 NUMA 环境 stress 全部 PASS
- **deliverables**: ARM64 写屏障代码 + Server GC E2E 测试报告
- **dependencies**: Phase C（GC 引擎稳定，可观测性就绪）
- **resolved_decisions**: D1 范围为最小（Windows ARM64 编译 + barrier UT），D2 含 Server GC E2E 压力测试
- **watch_items**: ARM64 交叉编译环境可能遇到 CMake/工具链问题

### Phase E — 性能优化与高级功能

- **goal**: NUMA 完善、写屏障优化、三代分代对齐、高级句柄类型
- **exit_criteria**: E1-E3 全部实现 + 基准套件无退化 + 72h soak 全 PASS
- **deliverables**: Card bundle、OS 低内存通知、AsyncPinned handle、Gen1 部分保留
- **dependencies**: Phase D（ARM64/Server GC 已验证）
- **resolved_decisions**: C20 Gen1 改为部分保留语义；C15 runtime GC-heap-pointer 检测函数
- **watch_items**: E3 Gen1 改造面积大（3 代分代对齐），可能影响 promotion 语义和 full GC 路径

## 子任务映射

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| **Phase A — 生产正确性修复** | | | | | | |
| gc-iw-a1 | phase-a | **completed** | **BGC-YoungGC 并发 segfault 修复（C1）** | 无 | gc_bgc.cpp, gc_young_collector.cpp | 1-2 周 |
| gc-iw-a2 | phase-a | **completed** | **Young collector card dirty 时序失败修复（C2）** | gc-iw-a1 | gc_young_collector.cpp, gc_card_table.cpp | 3-5 天 |
| gc-iw-a3 | phase-a | **planned** | **Codegen verification 管线打通 + 托管 API 完整验证（C3）** | gc-iw-a2 | verification/, codegen/ | 2-3 周 |
| **Phase B — GC 引擎硬化** | | | | | | |
| gc-iw-b1 | phase-b | **planned** | **并发根扫描接入点集成（C6）** | phase-a | gc_root_change.cpp, gc_bgc.cpp | 1 周 |
| gc-iw-b2 | phase-b | **planned** | **Parallel mark lock-free deque 改造（C7）** | gc-iw-b1 | gc_parallel_mark.cpp | 2-3 周 |
| gc-iw-b3 | phase-b | **planned** | **CrossPageCompact 空转优化（C8）** | gc-iw-b2 | gc_old_gen.cpp | 3 天 |
| gc-iw-b4 | phase-b | **planned** | **retired_arrays_ 泄漏修复（C9）** | gc-iw-b3 | gc_old_gen.cpp | 2 天 |
| gc-iw-b5 | phase-b | **planned** | **Finalizer 线程池替代 std::async（C10）** | gc-iw-b4 | gc_bgc.cpp, gc_api.cpp | 1 周 |
| **Phase C — 可观测性与 API 完整** | | | | | | |
| gc-iw-c1 | phase-c | **planned** | **LatencyMode 差异化行为实现（C11）** | phase-b | gc_scheduler.cpp, gc_api.cpp | 1-2 周 |
| gc-iw-c2 | phase-c | **planned** | **GCMemoryInfo 托管侧结构体（C12）** | gc-iw-c1 | managed/BCL, gc_api.h | 1 周 |
| gc-iw-c3 | phase-c | **planned** | **GcEndStubbornChange 实现（C13）** | gc-iw-c2 | gc_card_table.h, gc_card_table.cpp | 3 天 |
| **Phase D — 平台扩展** | | | | | | |
| gc-iw-d1 | phase-d | **planned** | **ARM64 编译验证 + 写屏障 UT（C4）** | phase-c | gc_card_table.h, CMakeLists.txt | 2-3 周 |
| gc-iw-d2 | phase-d | **planned** | **Server GC 生产硬化 + E2E 压力测试（C5）** | gc-iw-d1 | gc_coordinator.cpp, gc_heap_manager.cpp | 2-3 周 |
| **Phase E — 性能优化与高级功能** | | | | | | |
| gc-iw-e1 | phase-e | **planned** | **NUMA 完善 + Card bundle + CPU 亲和性（C14/C16/C17）** | phase-d | gc_numa.cpp, gc_card_table.cpp, gc_worker_pool.cpp | 3-4 周 |
| gc-iw-e2 | phase-e | **planned** | **写屏障假阳性消除 + OS 低内存通知（C15/C18）** | gc-iw-e1 | gc_helpers.h, gc_scheduler.cpp | 2-3 周 |
| gc-iw-e3 | phase-e | **planned** | **AsyncPinned handle + Gen1 部分保留（C19/C20）** | gc-iw-e2 | gc_handle.cpp, gc_gen1.cpp, gc_young_gen.h | 3-4 周 |

## 依赖图

```
Phase A:                    Phase B:                Phase C:            Phase D:                Phase E:
gc-iw-a1 ─┐                                          
gc-iw-a2 ─┤ (串行)                                   
gc-iw-a3 ─┘                                          
     │                                                
     ▼                                                
gc-iw-b1 ─┐                                          
gc-iw-b2 ─┤                                          
gc-iw-b3 ─┤ (串行)                                   
gc-iw-b4 ─┤                                          
gc-iw-b5 ─┘                                          
     │                                                
     ▼                                                
gc-iw-c1 ─┐                                          
gc-iw-c2 ─┤ (串行)                                   
gc-iw-c3 ─┘                                          
     │                                                
     ▼                                                
gc-iw-d1 ─┐ (串行)                                   
gc-iw-d2 ─┘                                          
     │                                                
     ▼                                                
gc-iw-e1 ─┐                                          
gc-iw-e2 ─┤ (串行)                                   
gc-iw-e3 ─┘                                          
```

## 风险

| 风险 | 影响 | 缓解 |
|------|------|------|
| A1 BGC-YoungGC segfault 根因复杂（>2 周未解） | Phase A 阻塞 | 首周精确定位；若 >2 周先做 workaround（序列化 BGC+Young GC），A3 依赖改为 workaround 版本 |
| A3 codegen pipeline 暴露出大量隐藏缺陷 | Phase A 延期，连锁影响后续全部阶段 | 提前排查 verification pipeline 构建依赖链；无法一次跑通时先做最小 API subset 验证 |
| D1 ARM64 工具链不可用 | Phase D 延期 | 回退方案：只做 ARM64 写屏障代码写 + 单元测试（x64 模拟验证），编译验证放后续 |
| E3 Gen1 改造波及 full GC 路径 | Phase E 回归风险 | 增加 Gen1 独立单元测试覆盖率；stress 套件全量验证；72h soak 兜底 |

## 备选路径

- 如果总体进度过半时发现剩余工作量远超预期（如 >3 个月），可将 E 组中的 C19（AsyncPinned handle）拆为独立后续任务，不作为本次必经
- 如果 D2 Server GC 压力测试持续失败，可选择在 D 阶段交付 D1 后暂停，E 组仅在 WKS 模式下验证

## 当前建议推进顺序

串行推进 Phase A → Phase B → Phase C → Phase D → Phase E

当前进度：Phase A — gc-iw-a1 ✅ completed, gc-iw-a2 ✅ completed。**下一步：gc-iw-a3（Codegen verification 管线打通）**。

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- 每个子任务启动前做最小 `preflight self-review`
- 子任务默认先进入 `writing-plans` → `executing-plans`
