# T-B2 — 单 Region 分配器设计

> 标题：单 Region 分配器（统一分配路径，消除 A2b 跨分配器窗口）
> 作者：Chaos IL2CPP 基础设施
> 日期：2026-08-31
> 上位文档：`roadmap-v1-01.md` T-B2 / `a3-safepoint-research.md`（research）+ `a3-safepoint-design.md`（design）§6

---

## 0. 现状调查结论

### 当前分配路径全景

| 路径 | 入口 | 内部 | 状态 |
|:---|:---|:---|:---:|
| **Hot path（codegen）** | `GcAllocateFast` → `NurseryAllocate` | TLAB bump（per-thread 256KB） | 活跃 |
| **Hot path（慢退）** | `NurseryAllocateSlow` → `G_OldGen().Allocate` | 触发 GC → 重试 → old-gen fallback | 活跃 |
| **Old-gen** | `G_OldGen().Allocate()` | 28 size-class × 64KB pages, freelist | 活跃 |
| **LOH** | `G_Loh.Allocate()` | >85KB, VirtualAlloc segments | 活跃 |
| **POH** | `PohAllocate()` | REGION_POH, std::mutex（⚠️ 死锁风险） | 活跃 |
| **Domain** | `DomainAllocate()` | REGION_DOMAIN, GcSpinLock | 活跃 |
| **Raw** | `RawAllocate()` | malloc fallback | 活跃 |
| **Runtime built-in** | `AllocateBytes()` | `config.allocator` 函数指针 | 活跃 |
| **GcBumpCache** | 类定义存在但**无全局实例** | 20 size-class, 8-block batch refill | **僵尸**（未使用） |

### 关键发现

- **GcBumpCache 已死**（仅类定义，无全局实例），不是三套并行系统
- **真正的分裂是**：Nursery/TLAB（young-gen 快速路径）与 `G_OldGen().Allocate`（old-gen fallback）之间的割裂——这是 A2b 的根因之一
- **RegionAllocate 统一入口已被移除**（`gc_region.cpp:1502` 注释 "P2-3: RegionAllocate removed — dead code, callers use NurseryAllocate, G_OldGen().Allocate, or DomainAllocate directly"）
- **PohAllocate 在 cooperative mode 持 std::mutex** 是已知 HIGH 死锁风险

---

## 1. 设计目标

1. **统一所有小对象分配入口**（<85KB）到一个 region-based 分配器
2. **消除 A2b 跨分配器窗口**（young-gen 与 old-gen 不再有独立的 bump 域）
3. **保留 TLAB 快速路径**性能（~10 指令，per-thread 零锁）
4. **消除 PohAllocate std::mutex 死锁风险**
5. **与 A3 Hybrid safepoint 兼容**（alloc 路径在 cooperative mode 完成后走 rendezvous）

---

## 2. 架构设计

### 2.1 顶层：单 Region 分配器（Single Region Allocator）

```
                    ┌──────────────────────────────┐
                    │   AllocateRegion(size)        │ ← 统一入口
                    │   ┌────── RegionManager ────┐ │
                    │   │  region_table_[]         │ │
                    │   │  free_list_              │ │
                    │   │  pin_list_               │ │
                    │   └──────────────────────────┘ │
                    └──────────────┬──────────────────┘
                                   │
          ┌──────────┬─────────────┼──────────────┬──────────────┐
          ▼          ▼             ▼              ▼              ▼
    ┌─────────┐ ┌─────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐
    │ NURSERY │ │  GEN1   │ │ TENURED  │ │ DOMAIN   │ │   RAW    │
    │ bump    │ │ bump    │ │ 28-SC x  │ │ bump     │ │ malloc   │
    │ per-TLS │ │ shared  │ │ 64KB     │ │ per-reg  │ │ fallback │
    └─────────┘ └─────────┘ └──────────┘ └──────────┘ └──────────┘
                              │
                     ┌────────┴────────┐
                     ▼                  ▼
               ┌──────────┐      ┌──────────┐
               │  POH     │      │  LOH     │
               │ pinned   │      │ >85KB    │
               │ bump     │      │ VAlloc   │
               └──────────┘      └──────────┘
```

**关键设计点**：`AllocateRegion` 是唯一入口，所有分配路径最终都通过 `RegionManager` 路由。`RegionManager` 是**分配路径的唯一 owner**——这直接消除 A2b 的「跨分配器」窗口，因为 GC 只需与一个分配器协调。

### 2.2 快速路径（保留 TLAB 性能）

```
Allocate(size):
  if size > kMaxTlabAlloc:
    return AllocateLarge(size)     // LOH / old-gen bypass
  
  // Per-thread TLAB bump (same as current, ≈10 instructions)
  ptr = tls_tlab.current
  next = ptr + align8(size)
  if next <= tls_tlab.end:
    tls_tlab.current = next
    return ptr
  
  // Slow path: get new TLAB from RegionManager
  return AllocateSlow(size)

AllocateSlow(size):
  // 1. Try soft-limit GC check
  // 2. Claim new TLAB from RegionManager (TlabClaimFromSharedPool)
  // 3. Emergency TLAB during safepoint (SPB)
  // 4. Trigger GC (scheduler-mediated)
  // 5. Post-GC retry
  // 6. Old-gen fallback (still through RegionManager)
  // 7. Emergency reserve
  // 8. OOM chain
```

### 2.3 消除 PohAllocate 死锁

POH 当前在 cooperative mode 持 `std::mutex`。改为：

```
POH 分配 → 在 preemptive mode 下分配（ScopedPreemptiveMode Guard）
         → 使用 GcSpinLock（非 std::mutex，不 park）
         → 或纳入 RegionManager 的 pin_list 管理，无需独立锁
```

**推荐方案**：POH 纳入 RegionManager 管理，用 `GcSpinLock` 保护 POH 区域分配（已有 `GcSpinLockGuard` RAII），消除 kernel-wait 死锁风险。

### 2.4 与 A3 Hybrid safepoint 的兼容

```
Allocate 路径 → 在 cooperative mode 中执行
              → 进入 Allocate 前已完成 rendezvous 检测（由 A3 §2 保证）
              → 分配器不需额外的 safepoint 检测
              → 一次分配完成后，若线程还需继续分配，继续在 cooperative 中
              → RegionManager 内部锁用 GcSpinLock（不 park，不违反 safepoint）

AllocateSlow 中旧-gen fallback：
              → 切换到 preemptive mode（ScopedPreemptiveMode）
              → 在 preemptive 中调旧-gen 分配
              → 恢复 cooperative
              → 符合 A3「mode switch 保证 barrier 一致性」
```

---

## 3. 文件变更清单

| 文件 | 变更类型 | 说明 |
|:---|:---:|:---|
| `gc/region_manager.h`（新） | 新建 | RegionManager 统一接口，从 `gc_region.h` 中提取 |
| `gc/region_manager.cpp`（新） | 新建 | 统一分配路径实现，含 AllocateRegion |
| `gc/region_alloc.h`（新） | 新建 | 快速路径 Allocate inline（TLAB bump） |
| `gc/gc_region.h` | 重构 | 保留 Region/RegionKind 定义，移除分散的 NurseryAllocate 定义 |
| `gc/gc_region.cpp` | 重构 | 移除 NurseryAllocateSlow，迁移到 RegionManager |
| `gc/gc_old_gen.h` | 重构 | 保留 MarkSweepOldGen 但不暴露为外部分配入口 |
| `gc/gc_old_gen.cpp` | 重构 | 旧-gen 分配降级为 RegionManager 内部调用 |
| `gc/gc_bump_cache.h` | 删除 | 确认已僵尸，正式移除 |
| `gc/gc_helpers.h` | 更新 | 分配路径更新 |
| `core/gc_alloc_stubs.h` | 更新 | GcAllocateFast 调用新的 Allocate 入口 |
| `core/gc_alloc_stubs.cpp` | 更新 | 同上 |
| `core/object_creation.cpp` | 更新 | AllocateBytes / ObjectNew / ArrayNew 统一路径 |

---

## 4. 与既有设计的关系

### 4.1 与 A3 Hybrid safepoint（T-B1）

- 分配器不负责 safepoint 协调——safepoint 由 A3 的全局 trap 标志 + 强制驱赶保证
- 分配器内部锁用 GcSpinLock（不 park），不违反 safepoint 协议
- 分配完成后立即出 cooperative mode，进入 safepoint 检测窗口

### 4.2 与 LEAF barrier（T-B3）

- 分配器不关心 barrier 实现——barrier 在 store 操作中触发，在分配器外部
- 分配器确保返回的内存是 zeroed（TLAB 已 zeroed，old-gen 已 zeroed）

### 4.3 与 forbid_suspend（T-B5）

- 分配器内部锁用 GcSpinLock（短临界区，spin-then-yield）
- 长临界区（old-gen page allocation）用 ScopedPreemptiveMode 包裹
- 不直接使用 forbid_suspend——由 A3 的 forbid-suspend 护栏统一管理

---

## 5. 风险与缓解

| 风险 | 概率 | 缓解 |
|:---|:---:|:---|
| 统一入口引入性能回归（TLAB 快速路径多一层函数调用） | 中 | 快速路径仍 inline（`Allocate` 在 `region_alloc.h` 中仍然是 `__forceinline`），不增加间接调用 |
| POH 的 RegionManager 集成引入新锁竞争 | 低 | POH 分配极少，GcSpinLock 足够 |
| DomainAllocate 与 RegionManager 的集成点复杂 | 中 | DomainAllocate 已通过 RegionManager 分配，只需改路由不变语义 |
| 移除 GcBumpCache 文件后若未来需要 size-class 缓存 | 低 | 可以在 RegionManager 内部重新实现，不暴露为外部接口 |

---

## 6. 已拍板决策

- [x] 单 Region 分配器是唯一入口，GC 只与一个分配器协调
- [x] 快速路径 TLAB bump 保留 inline，不引入性能损失
- [x] GcBumpCache 正式移除（已僵尸）
- [x] POH 纳入 RegionManager 管理，用 GcSpinLock 替代 std::mutex
- [x] AllocateRegion 替代 NurseryAllocate + G_OldGen().Allocate 双入口
- [x] 分配器内部锁用 GcSpinLock（不 park），不违反 safepoint 协议

---

## 7. 与设计-v1 的 Authority 决策一致性

- ✅ **A3 Hybrid 兼容**：分配器在 cooperative mode 中工作，safepoint 由 A3 保证
- ✅ **mode switch 保证**：old-gen fallback 走 ScopedPreemptiveMode，符合 mode switch 语义
- ✅ **forbid_suspend 保留并强化**：分配器短锁不涉及 forbid_suspend，长锁走 ScopedPreemptiveMode
- ✅ **跨平台**：纯 C++ 实现，无平台特定代码