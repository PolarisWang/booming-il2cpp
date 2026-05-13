# CRAG 全面就绪计划: 消除 BDWGC 依赖 + 对标 CoreCLR

> 日期: 2026-05-14
> 目标: 完全启用自研 GC (CRAG) + 自研内存分配，消除 BDWGC 依赖，支持 200+ native DLL + 200+ patch DLL 高性能热更新场景

---

## 一、当前 BDWGC 依赖清单

### 1.1 直接依赖（必须替换）

| # | 用途 | 文件:行 | 当前方式 | 替换方案 |
|---|------|---------|---------|---------|
| D1 | DelegateInstance 分配 | bootstrap.cpp:579 | `GC_MALLOC(sizeof(DelegateInstance))` | CRAG NurseryAllocate 或 domain 分配 |
| D2 | 部分 GC 收集 | gc_safepoint.cpp:11 | `GC_collect_a_little()` | 自研 `GcCollectPartial()` |
| D3 | 终结器注册 | gc_safepoint.cpp:46 | `GC_register_finalizer_no_order()` | 自研 finalizer 表 |
| D4 | 终结器触发 | gc_safepoint.cpp:62 | `GC_invoke_finalizers()` | 自研 `RunFinalizers()` (已有部分) |
| D5 | 线程注册 (非 Windows) | runtime_init.cpp:85 | `GC_get_stack_base()` + `GC_register_my_thread()` | 自研栈边界查询 |
| D6 | 链接依赖 | CMakeLists.txt:120 | `target_link_libraries(... chaos_bdwgc ...)` | 移除链接 + 删除 vendored 代码 |

### 1.2 间接依赖（遗留代码，已废弃但仍引用）

| # | 用途 | 文件:行 | 说明 |
|---|------|---------|------|
| D7 | GcBumpArena 页分配 | gc_bump_arena.h:96 | 已废弃 (Phase A 遗留)，被 CRAG 区域分配取代 |
| D8 | GcBumpCache 页分配 | gc_bump_cache.h:150 | 已废弃 (Phase A 遗留)，被 CRAG 区域分配取代 |
| D9 | GC_END_STUBBORN_CHANGE 引用 | generated_code_compat.h:14 | 仅注释引用，未实际调用 |

### 1.3 BDWGC 数据统计

```
chaos_bdwgc 构建产物: ~500 KB STATIC library
依赖深度: chaos_runtime_core → chaos_bdwgc
源文件: ~60+ C 文件 (third_party/bdwgc/)
```

---

## 二、CRAG vs CoreCLR GC 功能差距

### 2.1 功能矩阵

| 功能 | CoreCLR (WKS/SVR) | CRAG (当前) | 差距等级 | 工作量估计 |
|------|-------------------|-------------|---------|-----------|
| **分代** | 3 代 (gen0/1/2) | 2 代 (young/old) | **低** — 2 代已覆盖 95% 场景 | 可选 |
| **Nursery bump** | ✅ TLAB bump | ✅ NurseryAllocate TLS bump | **无差距** | — |
| **精确栈扫描** | ✅ 精度 100% | ✅ GcSlotMap 精确 + 保守回退 | **无差距** | — |
| **写屏障** | ✅ Card table + DC | ✅ Card table (512B card) | **低** — 缺少 DC (卡片脏标记优化) | ~2d |
| **卡表动态扩展** | ✅ 随堆大小自动调整 | ❌ 固定 2M 条目 (1GB 上限) | **中** | ~3d |
| **并行标记** | ✅ SVR 多线程并行标记 | ❌ 单线程标记 | **高** — 大堆场景性能差距大 | ~2w |
| **并行清除** | ✅ 并行 sweep | ⚠️ 原子索引并行 sweep | **中** — 已有基础但缺优化 | ~1w |
| **压缩 (Compaction)** | ✅ 完整标记-压缩 | ❌ 标记-清除 + 空闲列表 | **高** — 大堆碎片问题 | ~1m |
| **LOH (大对象堆)** | ✅ 独立 >85KB 堆 | ⚠️ VirtualAlloc 直配 + 回收 | **中** — 需独立管理 | ~1w |
| **终结器 (Finalizer)** | ✅ 自研 finalization | ⚠️ BDWGC 终结器 (D3/D4) | **高** — 终结器是 BDWGC 存留主因 | ~2w |
| **弱引用 (WeakRef)** | ✅ GCHandle, WeakReference | ❌ 不存在 | **高** | ~2w |
| **对象固定 (Pinning)** | ✅ GCHandleType.Pinned | ⚠️ pinned_roots_ (基础) | **中** — 已有 pinned_roots_ 但缺完整 GCHandle | ~1w |
| **GC 通知** | ✅ GC.RegisterForFullGC | ❌ 不存在 | **低** — 游戏引擎场景需求不高 | ~3d |
| **GC 压力模式** | ✅ GCStress | ❌ 不存在 | **中** — 影响稳定性验证 | ~1w |
| **堆硬限制** | ✅ GCHeapHardLimit | ❌ 不存在 | **低** | ~2d |
| **GC 统计报告** | ✅ 丰富计数器 | ⚠️ GcStats 基本计数器 | **低** | ~3d |
| **Safepoint** | ✅ 两阶段停止 + 超时中断 | ⚠️ 一代翻转 + 自旋 (V4-H1/H2 已修复) | **低** — 正确性已解决需增强 | ~1w |

### 2.2 核心差距分类

```
紧急 (BDWGC 替换阻塞项):
  终结器 (D3/D4)       ─── 替换 BDWGC finalization 体系
  DelegateInstance (D1) ─── 迁移到 CRAG 分配
  部分收集 (D2)         ─── 自研 partial GC

功能完整 (CoreCLR 对齐):
  弱引用              ─── WeakReference 语义
  对象固定 + GCHandle ─── 完整 pinning 支持
  并行标记             ─── 大堆性能
  压缩                ─── 碎片控制

性能:
  卡表动态扩展         ─── >1GB 堆场景
  写屏障优化 (DC)      ─── reduce young GC card scan
  并行 sweep 增强      ─── 减少 STW 时间
```

---

## 三、400+ DLL 架构需求

### 3.1 关键约束

```
200+ native DLL     = 200+ AOT 编译模块 (GameAssembly, plugins, etc.)
200+ patch DLL      = 200+ 热更新模块 (动态加载/卸载)
                    = ~400+ MemoryDomain 实例
                    = ~400+ GcLayout 注册表条目/模块
                    = 跨 DLL GC root 扫描 (每帧播报)
```

### 3.2 当前架构容量

| 组件 | 当前容量 | 所需容量 | 状态 |
|------|---------|---------|------|
| module_id | int32 (2B limit) | 400+ | ✅ 足够 |
| MemoryDomain 注册 | VECTOR 无上限 | 400+ | ✅ 动态扩展 |
| GcLayout hash 表 | 4096 slot | 400 × avg 100 类型 = 40K | ⚠️ **需扩容** |
| 方法表 | 65536 slot | 400 DLL × 5000 方法 = 2M | ❌ **严重不足** |
| 卡表条目 | 2M (覆盖 1GB) | >1GB 堆 | ❌ **需动态扩展** |
| 模块注册表 | VECTOR | 400+ | ✅ 动态扩展 |

### 3.3 扩容关键行动

1. **方法表扩容**: g_method_table[65536] → 动态/分层方法表
   - 当前: 16 byte/slot × 65536 = 1MB 固定数组
   - 目标: 分层方案 (直接索引 + overflow fallback)
   - 工作量: ~2w (涉及 codegen emitted index + fast_dispatch 查询路径)

2. **GcLayout 哈希表扩容**: 4096 → 65536 slot
   - 当前: 4096 slot 开放寻址
   - 目标: 65536 slot + 动态 rehash (或 per-DLL 分段)
   - 工作量: ~1w (需考虑 lock-free 并发注册)

3. **卡表动态扩展**: 参见 V4-M5

---

## 四、分阶段工作计划

### Phase 0: 紧急 — BDWGC 依赖消除 (3-4 周)

**目标**: 移除全部 BDWGC 直接依赖，`chaos_runtime_core` 不再链接 chaos_bdwgc

| # | 任务 | 工作量 | 前置 | 说明 |
|---|------|--------|------|------|
| 0.1 | **自研终结器体系** | 2w | — | 替换 D3/D4 |
| 0.1a | Finalizer 注册表: `unordered_map<void*, FinalizerCallback>` + mutex | 2d | — | 替代 GC_register_finalizer_no_order |
| 0.1b | Finalizer 可达性检测: 在 full GC mark phase 中检查 finalizer 存活 | 3d | 0.1a | 只有不可达对象的 finalizer 才入队 |
| 0.1c | `GcInvokeFinalizers()`: 支持批量 + 重入安全 | 2d | 0.1b | 替代 GC_invoke_finalizers |
| 0.1d | Finalizer 线程: 专用线程 drain + 与 GC safepoint 协作 | 3d | 0.1c | CoreCLR 有独立 finalizer 线程 |
| 0.2 | **DelegateInstance 迁移到 CRAG** | 2d | — | 替换 D1 |
| 0.2a | 分析 DelegateInstance 内存语义: 含 managed 指针 | 0.5d | — | 确认需要 GC tracing |
| 0.2b | 创建 `GcAllocDelegate(size)` 或 domain 分配 + 手动 root 注册 | 1d | 0.2a | 替代 GC_MALLOC |
| 0.2c | 验证 delegate Combine/Remove/Invoke 全链路 | 0.5d | 0.2b | — |
| 0.3 | **自研部分 GC 收集** | 1w | 0.1 | 替换 D2 |
| 0.3a | `GcCollectPartial()`: 只执行 young GC + 部分 old-gen 清理 | 3d | — | GC_collect_a_little 替代 |
| 0.3b | Safepoint 中接入部分收集通路 | 2d | 0.3a | — |
| 0.4 | **自研栈边界查询** | 1d | — | 替换 D5 |
| 0.5 | **移除 chaos_bdwgc 链接** | 1d | 0.1-0.4 | 替换 D6 |
| 0.5a | CMakeLists.txt 取消链接 chaos_bdwgc | 0.5d | — | — |
| 0.5b | 删除 `third_party/bdwgc/` (或移入 attic) | 0.5d | 0.5a | — |
| 0.5c | 删除 `#include <gc.h>` 残留 | — | — | gc_young_collector.cpp + generated_code_compat.h |
| 0.6 | **废弃代码清理** | 1d | 0.5 | 替换 D7-D9 |
| 0.6a | 删除 `gc_bump_arena.h` | 0.5d | — | — |
| 0.6b | 删除 `gc_bump_cache.h` | 0.5d | — | — |

**Phase 0 出口条件**:
- [ ] grep "GC_MALLOC" 返回值 0
- [ ] grep "#include <gc.h>" 返回值 0
- [ ] chaos_runtime_core link target 不再包含 chaos_bdwgc
- [ ] stress test 全部 7 场景 PASS (移除 BDWGC 后 CRAG 全权负责)
- [ ] delegate Combine/Remove/Invoke 测试 PASS

---

### Phase 1: 功能完整 — GC 特性补齐 (4-6 周)

**目标**: 达到 CoreCLR GC 核心功能集 (weak ref, finalization, pinning, compaction)

| # | 任务 | 工作量 | 前置 | 说明 |
|---|------|--------|------|------|
| 1.1 | **弱引用体系** | 2w | — | — |
| 1.1a | WeakRef 表: `vector<WeakRefEntry>` (object*, GCHandle) | 3d | — | GC mark phase 中标记 weak ref |
| 1.1b | WeakRef 清理: mark phase 后清理不可达 weak ref | 2d | 1.1a | — |
| 1.1c | GCHandle API: Alloc/Free/GetTarget/Target=null | 2d | 1.1a | — |
| 1.1d | 线程安全: per-GCHandle lock 或 striped locking | 2d | 1.1c | — |
| 1.1e | 压力测试: 多线程并发 GCHandle 创建/释放 | 2d | 1.1d | — |
| 1.2 | **完整 pinning + GCHandle** | 1w | — | 增强 pinned_roots_ |
| 1.2a | GCHandleType.Pinned: 集成到现有 pinned_roots_ | 2d | — | — |
| 1.2b | GCHandle 表: 全局 GCHandle 存储 (Fix/Ref/Pinned/Weak) | 3d | 1.2a | — |
| 1.2c | GC mark phase 中扫描全部 GCHandle | 2d | 1.2b | — |
| 1.3 | **标记-压缩 (Mark-Compact)** | 1m | 1.2 | CoreCLR 关键特性 |
| 1.3a | 压缩计划: 计算对象移动目标地址 | 1w | — | — |
| 1.3b | 指针修正: 更新所有引用指向新地址 | 1w | 1.3a | 需要 GcLayout 配合 |
| 1.3c | 压缩执行: 内存块移动 (memmove) | 3d | 1.3b | — |
| 1.3d | 细粒度控制: 按 page 分段压缩 | 3d | 1.3c | 避免全量压缩 STW 过高 |
| 1.4 | **GC 压力/验证模式** | 1w | — | — |
| 1.4a | GCStress: 每次分配后触发的 GC 验证 | 2d | — | Debug 模式 |
| 1.4b | 堆完整性验证: `GcVerifyHeap()` | 3d | — | mark-bit + card-table 交叉验证 |
| 1.4c | GC 行为验证自动化 | 2d | 1.4b | — |

**Phase 1 出口条件**:
- [ ] WeakReference 创建/解析/清理单元测试 PASS
- [ ] GCHandle.Alloc/Free (Fix/Ref/Pinned/Weak) 全部 PASS
- [ ] Mark-Compact 基本路径 PASS (stress test 扩展场景)
- [ ] GCStress 模式在 debug 构建下 PASS
- [ ] Finalizer 自研实现 100% 替换 BDWGC 终结器

---

### Phase 2: 扩容 — 400+ DLL 架构 (4-6 周)

**目标**: 方法表、GcLayout、卡表全面支持 400+ DLL

| # | 任务 | 工作量 | 前置 | 说明 |
|---|------|--------|------|------|
| 2.1 | **方法表扩容 (最大瓶颈)** | 2w | — | g_method_table[65536] → 分层方案 |
| 2.1a | 方案设计: 分层方法表 (直接索引 + overflow per-DLL 二级表) | 2d | — | — |
| 2.1b | codegen 输出适配: method_token 编码方式修改 | 3d | 2.1a | 影响 fast_dispatch dispatch 索引 |
| 2.1c | fast_dispatch 查询路径适配: 直接 → 分层查找 | 3d | 2.1b | 热点路径性能敏感 |
| 2.1d | Interpreter 路径适配 | 2d | 2.1c | interpreter 也使用方法表 |
| 2.1e | 回退/缓存: 常用方法入口缓存 | 2d | 2.1d | 减少分层查找开销 |
| 2.2 | **GcLayout 哈希表扩容** | 1w | — | — |
| 2.2a | 65536 slot lock-free 哈希表 | 3d | — | 或 per-DLL 分段方案 |
| 2.2b | 注册性能验证: 400 DLL × 100 类型批量注册 | 2d | 2.2a | — |
| 2.3 | **卡表动态扩展** | 3d | — | V4-M5 |
| 2.3a | AllocatePage 中按堆大小动态调整卡表 | 2d | — | — |
| 2.3b | 卡表溢出标志 `g_card_table_overflowed` 删除 | 1d | 2.3a | — |
| 2.4 | **跨域引用扫描 O(N) → O(dirty)** | 1w | — | V4-M4 |
| 2.4a | region→card 反向映射 | 3d | — | — |
| 2.4b | 域卸载只扫描脏卡片 | 2d | 2.4a | — |
| 2.5 | **虚拟机表 (vtable) 扩容** | 4d | 2.1 | — |
| 2.5a | `vtable_registry.cpp` 扩容 | 2d | — | — |
| 2.5b | 分模块 vtable 缓存 | 2d | 2.5a | — |

**Phase 2 出口条件**:
- [ ] 方法表支持 2M+ 条目 (400 DLL × 5000 方法)
- [ ] fast_dispatch 性能退化 < 5%
- [ ] GcLayout 4096 → 65536 slot 无冲突率验证
- [ ] 卡表支持 >1GB 堆 (动态扩展)
- [ ] 域卸载卡表扫描 O(dirty 卡片数) 非 O(总卡片数)
- [ ] 400 DLL 模拟测试 PASS

---

### Phase 3: 性能 — STW 优化 (3-4 周)

**目标**: 大堆场景 young GC < 5ms, full GC < 50ms

| # | 任务 | 工作量 | 前置 | 说明 |
|---|------|--------|------|------|
| 3.1 | **并行标记** | 2w | 1.2 | 大堆场景核心优化 |
| 3.1a | 多线程 mark stack: per-thread 分片 + 负载均衡 | 1w | — | — |
| 3.1b | root 并行扫描 | 3d | 3.1a | — |
| 3.1c | 标记终结 barrier: 标记完成前检查所有线程完成 | 2d | 3.1b | — |
| 3.2 | **并行 sweep 增强** | 1w | — | V4-L5 安全化 + 性能 |
| 3.2a | SweepPage 超时 + fallback 顺序 sweep | 2d | — | V4-L5 修复 |
| 3.2b | 多线程空闲列表合并优化 | 3d | 3.2a | — |
| 3.3 | **写屏障优化 (Card table + DC)** | 1w | — | — |
| 3.3a | DC (Dirty Card) bit 集成 | 3d | — | 减少 young GC card scan 范围 |
| 3.3b | Card table 批量清理优化 | 2d | 3.3a | — |
| 3.4 | **Nursery 自适应调整** | 1w | — | — |
| 3.4a | 基于存活率(survival_rate_)的动态 nursery sizing | 3d | — | 已有基础, 需要调优 |
| 3.4b | 多线程 + per-thread nursery 协调 | 2d | 3.4a | — |

**Phase 3 出口条件**:
- [ ] 1GB 堆 full GC 暂停 < 50ms
- [ ] 256KB nursery young GC 暂停 < 5ms
- [ ] 并行标记加速比 > 3x (4 线程)
- [ ] 写屏障开销 < 5 指令 (当前已 5 指令)
- [ ] 400 DLL 并发分配压力测试 PASS

---

### Phase 4: 验证 — 生产就绪 (3-4 周)

**目标**: 全面测试覆盖 + 稳定性验证 + 性能基线

| # | 任务 | 工作量 | 前置 | 说明 |
|---|------|--------|------|------|
| 4.1 | **综合压力测试** | 2w | 0-3 | — |
| 4.1a | Scenario H: 混合线程 + 跨域引用 + GC | 3d | — | — |
| 4.1b | Scenario I: 超大堆 (>1GB) 稳定性 | 3d | 2.3 | — |
| 4.1c | Scenario J: 400 DLL 模拟 (multi-domain) | 3d | 2.1-2.2 | — |
| 4.1d | Scenario K: 并发终结点器 + GC | 2d | 0.1 | — |
| 4.1e | Scenario L: 弱引用 + pinning + GC | 2d | 1.1-1.2 | — |
| 4.2 | **GC 性能基线** | 1w | 3, 4.1 | — |
| 4.2a | 微基准: 分配/GC/暂停时间分布 | 2d | — | — |
| 4.2b | 大堆基准: 512MB/1GB/2GB 堆 full GC | 2d | 4.2a | — |
| 4.2c | 多线程基准: 4/8/16/32 线程 | 2d | 4.2b | — |
| 4.3 | **长期稳定性 (Soak Test)** | 1w | 4.1 | — |
| 4.3a | 12 小时连续压力测试 | 3d | — | — |
| 4.3b | 泄漏检测: 内存增长监控 | 2d | 4.3a | — |
| 4.3c | 热更新卸载循环: 100 次加载/卸载 | 2d | 4.3b | — |
| 4.4 | **GcStats 仪表板** | 3d | — | — |
| 4.4a | 实时 GC 监控导出接口 | 1d | — | — |
| 4.4b | 柱状图输出 (暂停时间分布) | 1d | 4.4a | — |
| 4.4c | GcDumpStats 扩展 | 1d | 4.4b | — |

**Phase 4 出口条件**:
- [ ] 全部 Scenario A-L PASS (12 场景)
- [ ] 12h soak test 0 泄漏 0 崩溃
- [ ] 热更新 100 次加载/卸载循环 PASS
- [ ] 性能基线文档化 (wiki/)

---

## 五、依赖关系图

```
Phase 0 (BDWGC 消除) ──────────────────────────────────────── 紧急
  │
  ├── 0.1 终结器 ─────────────────────────────────────────────
  │     └──→ 1.1 弱引用 (共享 finalization mark 逻辑)
  │
  ├── 0.2 DelegateInstance ───────────────────────────────────
  │     └──→ (独立, 无下游依赖)
  │
  └── 0.3 部分收集 ─────────────────────────────────────────
        └──→ 3.1 并行标记 (collect partial 共享 mark 逻辑)

Phase 1 (CoreCLR 对齐) ─────────────────────────────────────── 功能
  │
  ├── 1.1 弱引用 ────────────────────────────────────────────
  │     └──→ 4.1e 弱引用测试
  │
  ├── 1.2 Pinning ───────────────────────────────────────────
  │     └──→ 4.1e pinning 测试
  │
  └── 1.3 标记-压缩 ────────────────────────────────────────
        └──→ 3.1 并行标记 (共享 mark bit)

Phase 2 (400+ DLL 扩容) ────────────────────────────────────── 扩容
  │
  ├── 2.1 方法表 ────────────────────────────────────────────
  │     ├──→ 2.5 vtable 扩容
  │     └──→ 4.1c 400 DLL 模拟测试
  │
  ├── 2.2 GcLayout ──────────────────────────────────────────
  │     └──→ 4.1c 400 DLL 测试
  │
  ├── 2.3 卡表扩展 ──────────────────────────────────────────
  │     └──→ 4.1b 大堆测试
  │
  └── 2.4 域卸载扫描优化 ────────────────────────────────────
        └──→ 4.3c 热更新卸载循环

Phase 3 (性能) ──────────────────────────────────────────────── 性能
  │
  ├── 3.1 并行标记 ──────────────────────────────────────────
  │     └──→ 4.2 GC 性能基线
  │
  ├── 3.2 并行 sweep ───────────────────────────────────────
  │     └──→ 4.2 GC 性能基线
  │
  ├── 3.3 写屏障 ────────────────────────────────────────────
  │     └──→ (性能增量)
  │
  └── 3.4 Nursery 自适应 ─────────────────────────────────────
        └──→ 4.2 GC 性能基线

Phase 4 (验证) ──────────────────────────────────────────────── 验证
  └── 所有 Phase 0-3 的验证
```

---

## 六、工作量汇总

| Phase | 周数 | 总人天 | 关键路径 |
|-------|------|--------|---------|
| Phase 0: BDWGC 消除 | 3-4 | 18-22d | 0.1 终结器 (2w) |
| Phase 1: CoreCLR 对齐 | 4-6 | 30-40d | 1.3 标记-压缩 (1m) |
| Phase 2: 400+ DLL 扩容 | 4-6 | 24-32d | 2.1 方法表 (2w) |
| Phase 3: 性能 | 3-4 | 18-22d | 3.1 并行标记 (2w) |
| Phase 4: 验证 | 3-4 | 15-20d | 4.1 压力测试 (2w) |
| **总计** | **17-24** | **105-136d** | — |

> 注意: 并行开发时可以缩短工期。Phase 0.1 (终结器) 与 Phase 0.2 (DelegateInstance) 可并行。
> Phase 1.1 (弱引用) 需要 0.1 自研终结器完成后的 mark phase 信息, 必须顺序。

---

## 七、风险与缓解

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| 终结器自研实现 bug 导致资源泄漏 | **高** | Phase 0 期间保持 BDWGC 终结器并行运行作为对照 |
| 方法表分层设计影响 fast_dispatch 性能 >10% | **高** | 添加常用方法入口缓存 (2.1e) |
| 标记-压缩导致 STW 超长 | **中** | 先实现细粒度 per-page 压缩 (1.3d), 全量压缩做 fallback |
| 400 DLL 实际工作负载弱于预期 | **中** | 用模拟器 + memory-mapped DLL 数据做 benchmark |
| GcLayout 4096→65536 slot rehash 锁竞争 | **低** | 采用 per-DLL 分段方案避免全局锁 |
| Stress test 覆盖不足 | **中** | 每个 Phase 出口条件含特定的 stress scenario |

---

## 八、建议执行优先级

```
立即 (本月):
  Phase 0.1 ─── 终结器自研 (BDWGC 消除最大阻塞项)
  Phase 0.2 ─── DelegateInstance 迁移

短期 (1-2 月):
  Phase 0.3-0.6 ─── BDWGC 完全消除
  Phase 1.1 ─── 弱引用
  Phase 1.2 ─── Pinning + GCHandle

中期 (2-3 月):
  Phase 2.1 ─── 方法表扩容 (400+ DLL 最大瓶颈)
  Phase 2.2 ─── GcLayout 扩容
  Phase 2.3 ─── 卡表动态扩展
  Phase 3.3 ─── 写屏障优化

长期 (3-6 月):
  Phase 1.3 ─── 标记-压缩
  Phase 3.1 ─── 并行标记
  Phase 3.2 ─── 并行 sweep 增强
  Phase 3.4 ─── Nursery 自适应
  Phase 2.4 ─── 域卸载扫描优化
  Phase 4 ─── 全面验证
```

---

## 九、对比 CoreCLR: 当前 CRAG 成熟度

```
CoreCLR WKS GC           CRAG 当前              CRAG 目标 (完成后)
────────────────────────────────────────────────────────────────────
分代: gen0/gen1/gen2     分代: young/old        分代: young/old (足够)
分配: TLAB bump          分配: Nursery bump      分配: Nursery bump ✅
精确扫描: ✅              精确扫描: ✅            精确扫描: ✅
写屏障: card + DC         写屏障: card only       写屏障: card + DC
并行标记: ✅              并行标记: ❌            并行标记: ✅
并行清除: ✅              并行清除: ⚠️ 基础        并行清除: ✅
压缩: ✅                  压缩: ❌                压缩: ✅
弱引用: ✅                弱引用: ❌              弱引用: ✅
终结器: ✅                终结器: ❌(BDWGC)        终结器: ✅
LOH: ✅                   LOH: ⚠️ VirtualAlloc    LOH: ✅
GC 通知: ✅               GC 通知: ❌             GC 通知: ⚠️ 低优
压力模式: ✅              压力模式: ❌             压力模式: ✅
堆限制: ✅                堆限制: ❌              堆限制: ⚠️ 低优

核心差距数:  0            核心差距数:  10          核心差距数:  2 (压缩+LOH 完整)
```

---

## 十、附录: 当前 CRAG 资产状态

### 已实现 (Phase C M1 完成)

- [x] NurseryAllocate/NurseryAllocateAtomic TLS bump 快速路径
- [x] Young GC (Cheney BFS 复制): 4 阶段 (脏卡扫描 → 精确扫描 → BFS → 清理)
- [x] Full GC (MarkSweep): MarkRoots → DrainMarkStack → Finalizers → Sweep
- [x] GC Scheduler: EMA 存活率, 自适应 nursery sizing [128KB, 1MB]
- [x] Card Table: 512B 卡片, 2M 条目, 5 指令写屏障
- [x] GcLayout: 4096 slot lock-free 哈希表, 32 inline 指针偏移
- [x] GcSlotMap: codegen 发射的精确栈帧根槽描述符
- [x] Safepoint: 代翻转 STW, CAS 所有者, TLS 嵌套深度
- [x] MemoryDomain: per-module HeapCreate/HeapDestroy, tagged 分配
- [x] DomainUnloader: 5 阶段 STW (扫描 → 清理 → 释放 → 注销 → 释放)
- [x] GcStats: 计数器 (分配/young/full), at-exit dump
- [x] 旧代 28 尺寸类别 [16B, 32KB]
- [x] 并行清除: 原子索引 dispatch
- [x] Old-gen 物理内存: VirtualAlloc/mmap (非 GC_MALLOC)
- [x] Root 扫描: 精确 (GcSlotMap) + 保守回退 (线程栈)
- [x] Pinned roots: 基础 pinned_roots_

### 待实现 (本计划范围)

- [ ] 自研终结器 (Phase 0.1)
- [ ] DelegateInstance 迁移 (Phase 0.2)
- [ ] 自研部分收集 (Phase 0.3)
- [ ] 栈边界查询自研 (Phase 0.4)
- [ ] BDWGC 完全移除 (Phase 0.5)
- [ ] 弱引用 (Phase 1.1)
- [ ] Pinning + GCHandle 完整 (Phase 1.2)
- [ ] 标记-压缩 (Phase 1.3)
- [ ] GC 压力模式 (Phase 1.4)
- [ ] 方法表分层扩容 (Phase 2.1)
- [ ] GcLayout 扩容 (Phase 2.2)
- [ ] 卡表动态扩展 (Phase 2.3)
- [ ] 跨域扫描 O(dirty) (Phase 2.4)
- [ ] 并行标记 (Phase 3.1)
- [ ] 并行 sweep 增强 (Phase 3.2)
- [ ] 写屏障 DC 优化 (Phase 3.3)
- [ ] Nursery 自适应调优 (Phase 3.4)
- [ ] 综合压力测试 (Phase 4)

---

*本计划覆盖 4 个维度: BDWGC 消除 → CoreCLR 功能对齐 → 400+ DLL 扩容 → 性能优化, 总估计 17-24 周 (4-6 月) 到生产就绪。*