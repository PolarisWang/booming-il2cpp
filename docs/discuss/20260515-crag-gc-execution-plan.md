# CRAG GC 全面就绪执行方案

> 目标: 完全消除 BDWGC 依赖 + CoreCLR 功能对齐 + 400 DLL 扩容 + 性能优化
> 总预估: 17-24 周 (4-6 月) 至生产就绪
> 当前: CRAG M1 已完成，BDWGC 残留 65+ 处 API 调用

---

## 一、整体路线图

```
Phase 0: BDWGC 移除 (3-4周)    ─── 紧急，本月
        ↓
Phase 1: CoreCLR 对齐 (4-6周)  ─── 功能补齐
        ↓
Phase 2: 400 DLL 扩容 (4-6周)  ─── 架构扩容
        ↓
Phase 3: 性能优化 (3-4周)      ─── 性能达标
        ↓
Phase 4: 验证与发布 (3-4周)    ─── 生产就绪
```

依赖关系：
```
┌─────────────────────────────────────────────────────┐
│ Phase 0: BDWGC 移除 (3-4周)                        │
│  0.1 终结器 ───→ 1.1 弱引用                          │
│  0.3 部分收集 ───→ 3.1 并行标记                       │
└──────────┬──────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────────────┐
│ Phase 1: CoreCLR 对齐 (4-6周)                       │
│  1.1 弱引用 ───→ 4.1e 弱引用测试                      │
│  1.3 标记压缩 ───→ 3.1 并行标记                       │
└──────────┬──────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────────────┐
│ Phase 2: 400 DLL扩容 (4-6周)                        │
│  2.1 方法表 ───→ 2.5 vtable扩容                     │
│  2.3 卡表扩展 ───→ 4.1b 大堆测试                     │
│  2.4 域卸载扫描 ───→ 4.3c 卸载循环                    │
└──────────┬──────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────────────┐
│ Phase 3: 性能优化 (3-4周)                            │
│  3.1 并行标记 → 4.2 GC性能基线                        │
└──────────┬──────────────────────────────────────────┘
           ↓
┌─────────────────────────────────────────────────────┐
│ Phase 4: 验证与发布 (3-4周)                          │
│  4.1 压力测试 + 4.2 性能基线 + 4.3 长期稳定性          │
└─────────────────────────────────────────────────────┘
```

---

## 二、Phase 0: BDWGC 依赖消除（紧急，3-4 周）

**目标**: 移除全部 BDWGC 直接依赖，`chaos_runtime_core` 不再链接 libgc。出口检查：`grep "GC_" src/native/runtime-core/` 返回 0。

### 0.1 自研终结器体系 (2w)

**现状**: `gc_safepoint.cpp` 使用 `GC_register_finalizer_no_order` + `GC_invoke_finalizers`

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 0.1a Finalizer 注册表 | 2d | `unordered_map<void*, FinalizerCallback>` + mutex |
| 0.1b Finalizer 可达性检测 | 3d | 在 full GC mark phase 中检查 finalizer 存活对象 |
| 0.1c GcInvokeFinalizers() | 2d | 批量执行 + 重入安全 |
| 0.1d Finalizer 专用线程 | 3d | CoreCLR 模式独立线程 drain + safepoint 协作 |

**验收**: 自研终结器覆盖全部 `GC_register_finalizer` / `GC_invoke_finalizers` 调用点，gc_finalizer_smoke.cpp 通过。

### 0.2 DelegateInstance 迁移到 CRAG (2d)

**现状**: `bootstrap.cpp:579` 使用 `GC_MALLOC(sizeof(DelegateInstance))`

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 0.2a 分析 DelegateInstance 内存语义 | 0.5d | 确认含 managed 指针 |
| 0.2b 创建 GcAllocDelegate() | 1d | domain 分配 + domain root 注册 |
| 0.2c delegate 全链路验证 | 0.5d | Combine/Remove/Invoke 测试 |

**验收**: delegate 测试全部通过，无 `GC_MALLOC` 调用。

### 0.3 自研部分 GC (1w)

**现状**: `gc_safepoint.cpp:11` 使用 `GC_collect_a_little()`

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 0.3a GcCollectPartial() | 3d | 只执行 young GC + 部分 old-gen 清理 |
| 0.3b Safepoint 接入 | 2d | 在 safepoint 轮询中接入部分收集通路 |

**验收**: GcCollectPartial 覆盖全部 `GC_collect_a_little` 调用点。

### 0.4 自研栈边界查询 (1d)

**现状**: `runtime_init.cpp:85` 使用 `GC_get_stack_base()`

**替代方案**: Windows 用 `GetCurrentThreadStackLimits()` (Vista+), POSIX 用 `pthread_getattr_np`

**验收**: 所有线程的栈边界查询不再调用 BDWGC。

### 0.5 移除 BDWGC 链接 (1d)

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 0.5a CMakeLists.txt 取消链接 | 0.5d | 删除 `target_link_libraries(... chaos_bdwgc)` |
| 0.5b 删除 `third_party/bdwgc/` | 0.5d | 移入 attic 或直接删除 |
| 0.5c 删除 `#include <gc.h>` 残留 | — | 运行时代码清理 |

### 0.6 废弃代码清理 (1d)

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 0.6a 删除 gc_bump_arena.h/cpp | 0.5d | Phase A 遗留代码 |
| 0.6b 删除 gc_bump_cache.h/cpp | 0.5d | Phase B 遗留代码 |
| 0.6c 更新文档 | 0.5d | wiki 更新为 CRAG-only |

### Phase 0 出口条件

- [ ] `grep "GC_MALLOC\|GC_INIT" src/native/runtime-core/` 返回 0
- [ ] `grep "#include <gc.h>" src/native/runtime-core/` 返回 0
- [ ] chaos_runtime_core 不再链接 chaos_bdwgc
- [ ] 压测 7 场景全部 PASS
- [ ] delegate 全链路测试 PASS
- [ ] finalizer 测试 PASS

---

## 三、Phase 1: CoreCLR 功能对齐（4-6 周）

**目标**: 对标 CoreCLR WKS GC 核心功能集（弱引用、GCHandle、压缩、GCStress）。验收：功能差距数从 10 降至 2。

### 1.1 弱引用体系 (2w)

**现状**: 仅 Young GC 后弱句柄处理，缺 Full GC WeakReference 完整语义

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 1.1a WeakRef 表 | 3d | `vector<WeakRefEntry>` (object*, GCHandle) |
| 1.1b WeakRef 清理逻辑 | 2d | Full GC mark phase 后清理不可达弱引用 |
| 1.1c GCHandle API 完整 | 2d | Alloc/Free/GetTarget/Target=set (Fix/Ref/Pinned/Weak) |
| 1.1d 线程安全 | 2d | per-GCHandle 或 striped locking |
| 1.1e 压力测试 | 2d | 多线程并发 GCHandle 创建/释放 |

**验收**: WeakReference 创建/解析/清理 PASS；GCHandle 四种类型全部 PASS。

### 1.2 Pinning + 完整 GCHandle (1w)

**现状**: 仅有基础 `pinned_roots_`，缺完整 GCHandleType 支持

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 1.2a GCHandleType.Pinned | 2d | 集成到现有 pinned_roots_ |
| 1.2b GCHandle 全局表 | 3d | Fix/Ref/Pinned/Weak 统一存储 |
| 1.2c GC mark phase 中扫描 GCHandle | 2d | 全类型遍历 |

**验收**: GCHandle.Alloc/Free 四类型全部 PASS。

### 1.3 标记-压缩增强 (1m)

**现状**: 仅 page 内 compact（30% 碎片触发），缺完整 mark-compact

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 1.3a 跨 page 压缩计划 | 1w | 计算对象移动目标地址 |
| 1.3b 指针修正 | 1w | 更新所有引用指向新地址（需 GcLayout） |
| 1.3c 压缩执行 | 3d | memmove 内存块 |
| 1.3d 细粒度控制 | 3d | per-page 分段压缩，避免全量 STW |

**验收**: 跨 page 压缩基本路径 PASS，压测扩展场景 PASS。

### 1.4 GC 压力/验证模式 (1w)

**现状**: 缺失 GCStress，稳定性验证依赖手动

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 1.4a GCStress 模式 | 2d | 每次分配后触发的 GC 验证（Debug 模式） |
| 1.4b GcVerifyHeap() | 3d | mark-bit + card-table 交叉验证 |
| 1.4c 自动化验证 | 2d | 集成到压测框架 |

**验收**: GCStress 模式 Debug 构建 PASS；GcVerifyHeap 无 false positive。

### Phase 1 出口条件

- [ ] WeakReference/ 弱引用全部测试 PASS
- [ ] GCHandle.Alloc/Free (Fix/Ref/Pinned/Weak) PASS
- [ ] Mark-Compact 基本路径 PASS（压测扩展场景）
- [ ] GCStress 模式 Debug 构建 PASS
- [ ] 自研终结器 100% 替换 BDWGC

---

## 四、Phase 2: 400+ DLL 架构扩容（4-6 周）

**目标**: 方法表、GcLayout、卡表全面支持 400+ DLL = ~2M 方法 + ~40K 类型 + >1GB 堆

### 2.1 方法表扩容 (2w) — 最大瓶颈

**现状**: `g_method_table[65536]` 固定数组 — 400 DLL × 5000 方法 = 2M 条
**影响**: codegen emitted index + fast_dispatch 查询路径均需修改

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 2.1a 方案设计 | 2d | 分层方法表（直接索引 + overflow per-DLL 二级表） |
| 2.1b codegen 输出适配 | 3d | method_token 编码方式修改 |
| 2.1c fast_dispatch 查询路径适配 | 3d | 直接 → 分层查找（热点路径，性能敏感） |
| 2.1d Interpreter 路径适配 | 2d | interpreter 使用方法表的代码路径 |
| 2.1e 常用方法入口缓存 | 2d | 减少分层查找开销 |

### 2.2 GcLayout 哈希表扩容 (1w)

**现状**: 4096 槽 — 400 DLL × 100 类型 = 40K 条

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 2.2a 65536 slot lock-free 哈希表 | 3d | 或 per-DLL 分段方案（避免全局 rehash） |
| 2.2b 批量注册性能验证 | 2d | 400 DLL 模拟批量注册 |

### 2.3 卡表动态扩展 (3d)

**现状**: 固定 2M 条目（1GB 上限）— `g_card_table_overflowed` 标志导致全量扫描

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 2.3a 按堆大小动态调整卡表 | 2d | AllocatePage 中自动扩展 L1 |
| 2.3b 删除溢出标志 | 1d | 移除 `g_card_table_overflowed` 及相关 fallback |

### 2.4 跨域扫描 O(N) → O(dirty) (1w)

**现状**: 域卸载时线性扫描所有 region

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 2.4a region → card 反向映射 | 3d | 快速定位域相关卡片 |
| 2.4b 域卸载只扫描脏卡片 | 2d | 利用卡表 dirty 位 |

### 2.5 vtable 扩容 (4d)

**现状**: `vtable_registry.cpp` 容量与方法表绑定

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 2.5a vtable_registry 扩容 | 2d | 匹配方法表扩容方案 |
| 2.5b 分模块 vtable 缓存 | 2d | 减少跨模块查找 |

### Phase 2 出口条件

- [ ] 方法表支持 2M+ 条目（400 DLL × 5000 方法）
- [ ] fast_dispatch 性能退化 < 5%
- [ ] GcLayout 4096 → 65536 slot 无冲突率验证
- [ ] 卡表支持 >1GB 堆（动态扩展）
- [ ] 域卸载卡表扫描 O(脏卡数) 非 O(总卡数)
- [ ] 400 DLL 模拟测试 PASS

---

## 五、Phase 3: 性能优化（3-4 周）

**目标**: 大堆场景 young GC < 1ms, full GC < 50ms（共识：并发标记是长期投资）

### 3.1 并行标记增强 (2w)

**现状**: 基础并行标记（work-stealing deques）已实现，缺优化

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 3.1a per-thread mark stack 分片 | 1w | 负载均衡 + 工作窃取 |
| 3.1b root 并行扫描 | 3d | 线程栈/GC handles 并行 |
| 3.1c 标记终止 barrier | 2d | 确所有线程标记完成 |

### 3.2 并行 Sweep 增强 (1w)

**现状**: 基础 atomic index dispatch 已实现

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 3.2a SweepPage 超时 + fallback | 2d | 防止 worker 线程饿死 |
| 3.2b 空闲列表合并优化 | 3d | 多线程并发合并 |

### 3.3 写屏障 DC 优化 (1w)

**现状**: 仅卡表（~6 insn），缺 DC (Dirty Card) 位

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 3.3a DC bit 集成 | 3d | 减少 young GC card scan 范围 |
| 3.3b 卡表批量清理 | 2d | O(dirty segments) 清理 |

### 3.4 Nursery 自适应调优 (1w)

**现状**: 基础自适应实现 [128KB, 1MB]

| 子任务 | 工作量 | 产出 |
|--------|--------|------|
| 3.4a 基于存活率的动态 sizing | 3d | 已有 `survival_rate_` 基础，调优参数 |
| 3.4b 多线程 per-thread 协调 | 2d | 平衡各线程 nursery 大小 |

### Phase 3 出口条件

- [ ] 1GB 堆 full GC 暂停 < 50ms
- [ ] Young GC 暂停 < 1ms
- [ ] 并行标记加速比 > 3x (4 线程)
- [ ] 写屏障开销 < 5 指令
- [ ] 400 DLL 并发分配压力测试 PASS

---

## 六、Phase 4: 验证与发布（3-4 周）

**目标**: 全面测试覆盖 + 稳定性验证 + 性能基线文档化

### 4.1 综合压力测试 (2w)

| 场景 | 工作量 | 描述 |
|------|--------|------|
| H: 混合线程 + 跨域引用 + GC | 3d | 模拟真实游戏负载 |
| I: 超大堆 (>1GB) 稳定性 | 3d | 验证卡表动态扩展 |
| J: 400 DLL 模拟 | 3d | multi-domain 并发 |
| K: 最终器 + GC 并发 | 2d | 自研终结器压力 |
| L: 弱引用 + pinning + GC | 2d | GCHandle 综合压力 |

### 4.2 GC 性能基线 (1w)

| 项目 | 工作量 | 描述 |
|------|--------|------|
| 微基准: 分配/GC/暂停时间分布 | 2d | 单次分配/GC 延迟 |
| 大堆基准: 512MB/1GB/2GB | 2d | 堆大小 vs 暂停时间 |
| 多线程基准: 4/8/16/32 线程 | 2d | 并发伸缩性 |

### 4.3 长期稳定性 (1w)

| 项目 | 工作量 | 描述 |
|------|--------|------|
| 12h 连续压力测试 | 3d | 0 泄漏 0 崩溃 |
| 泄漏检测: 内存增长监控 | 2d | 周期采样 |
| 热更新卸载循环: 100 次 | 2d | 加载/卸载稳定性 |

### 4.4 GcStats 仪表板 (3d)

| 项目 | 工作量 | 描述 |
|------|--------|------|
| 实时监控导出接口 | 1d | 游戏引擎集成 |
| 暂停时间分布直方图 | 1d | 可视化 |
| GcDumpStats 扩展 | 1d | 诊断输出 |

### Phase 4 出口条件

- [ ] 全部 Scenario A-L PASS (12 场景)
- [ ] 12h soak test 0 泄漏 0 崩溃
- [ ] 热更新 100 次加载/卸载 PASS
- [ ] 性能基线文档化

---

## 七、工作量汇总

| Phase | 周数 | 人天 | 关键路径 |
|-------|------|------|---------|
| Phase 0: BDWGC 移除 | 3-4 | 18-22d | 0.1 终结器 (2w) |
| Phase 1: CoreCLR 对齐 | 4-6 | 30-40d | 1.3 标记-压缩 (1m) |
| Phase 2: 400 DLL 扩容 | 4-6 | 24-32d | 2.1 方法表 (2w) |
| Phase 3: 性能优化 | 3-4 | 18-22d | 3.1 并行标记 (2w) |
| Phase 4: 验证与发布 | 3-4 | 15-20d | 4.1 压力测试 (2w) |
| **总计** | **17-24** | **105-136d** | — |

### 并行执行建议

```
Week  1-2    3-4    5-8    9-12   13-16  17-20
      ┌──────┐ ┌────────────────────────────────┐
P0.1  │终结器│ │                                │
      └──────┘ │                                │
P0.2  │Delegate│ │                                │
P0.3  │部分收集│ │ (P0 完成后的清理 0.4-0.6)      │
P0.4  │栈边界 │ │                                │
      └───────┘ └────────────────────────────────┘
                  ┌──────┐ ┌────────────────────┐
P1.1 弱引用       │      │ │                    │
P1.2 GCHandle     │      │ │                    │
P1.3 标记压缩     │      │ │                    │
P1.4 GCStress     │      │ │                    │
                  └──────┘ └────────────────────┘
                            ┌──────┐ ┌─────────┐
P2.1 方法表扩容             │      │ │         │
P2.2 GcLayout 扩容          │      │ │         │
P2.3 卡表扩展               │      │ │         │
P2.4 域卸载扫描             │      │ │         │
                            └──────┘ └─────────┘
                                      ┌──────┐
P3.1 并行标记增强                     │      │
P3.2 并行 Sweep                       │      │
P3.3 写屏障 DC                        │      │
P3.4 Nursery 调优                     │      │
                                      └──────┘
P4.1 压力测试                         │           │
P4.2 性能基线                         │           │
P4.3 长期稳定性                       │           │
```

---

## 八、验收检查清单

### Phase 0 完成标志
- [ ] `grep "GC_" src/native/runtime-core/` 返回 0
- [ ] `grep "#include <gc.h>" src/` 返回 0（third_party 除外）
- [ ] CMakeLists.txt 中无 chaos_bdwgc 链接
- [ ] Stress test 7 场景全部 PASS
- [ ] 终结器测试 PASS、delegate 测试 PASS

### Phase 1 完成标志
- [ ] WeakReference 测试 PASS
- [ ] GCHandle 四类型测试 PASS
- [ ] Mark-Compact 压测场景 PASS
- [ ] GCStress 模式 Debug PASS

### Phase 2 完成标志
- [ ] 方法表 2M+ 条目测试 PASS
- [ ] fast_dispatch 性能退化 < 5%
- [ ] GcLayout 65536 slot 测试 PASS
- [ ] 卡表 >1GB 堆动态扩展 PASS
- [ ] 域卸载 O(脏卡) 扫描 PASS
- [ ] 400 DLL 模拟测试 PASS

### Phase 3 完成标志
- [ ] 1GB 堆 full GC < 50ms
- [ ] Young GC < 1ms
- [ ] 并行标记加速比 > 3x (4 线程)

### Phase 4 完成标志
- [ ] 全部 12 场景 PASS
- [ ] 12h soak test 0 泄漏 0 崩溃
- [ ] 100 次热更新卸载循环 PASS
- [ ] 性能基线文档化

---

## 九、风险与缓解

| 风险 | 等级 | 缓解 |
|------|------|------|
| 终结器自研 bug 导致资源泄漏 | **高** | Phase 0 期间保留 BDWGC 终结器并行作为对照 |
| 方法表分层影响 fast_dispatch > 10% | **高** | 添加常用方法入口缓存 (2.1e) |
| 标记压缩 STW 过长 | **中** | 先 per-page 细粒度，全量压缩做 fallback |
| 400 DLL 工作负载弱于预期 | **中** | memory-mapped DLL 模拟做 benchmark |
| GcLayout rehash 锁竞争 | **低** | per-DLL 分段方案避免全局锁 |
| GCStress Debug 模式误报 | **中** | 缓慢上线，先对已知类型启用 |
