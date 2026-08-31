# T-B5 — forbid_suspend 强化为互斥护栏

> 标题：forbid_suspend 强化设计（对齐 CoreCLR `m_dwForbidSuspendThread`）
> 作者：Chaos IL2CPP 基础设施
> 日期：2026-08-31
> 上位文档：`roadmap-v1-01.md` T-B5（方向已修正）/ `a3-safepoint-design.md` §8 / `a3-contract-design.md`

---

## 0. 方向修正（来自 T-B1 研究）

**原 roadmap 方向「废弃 forbid_suspend」是错的。**

T-B1 研究证明 CoreCLR 恰恰用 `m_dwForbidSuspendThread` 作为核心互斥护栏：
> "`m_dwForbidSuspendThread` 互斥护栏：挂起他人前先自增自己的 forbid 计数，再查目标；挂起后重读目标 forbid，非零则 resume 重试。防止 A、B 两线程互相挂。这条必须有。"

因此 T-B5 的正确目标是：**保留并强化** CRAG 现有 forbid_suspend，使其在新 A3 硬驱赶路径（`pal_suspend.h`）下也正确。

## 1. 现状（CRAG 已有两块）

| 组件 | 现状 | 角色 |
|:---|:---|:---|
| `ForbidSuspendScope` | RAII，`tls_forbid_suspend_depth>0` 时 SafepointPoll acks-and-continues（防死锁） | 软路径防死锁 |
| `BarrierCriticalSectionScope` | RAII，`barrier_inflight` 发布，coordinator 等 0 才 Phase-1 | A2b 窗口关闭 |

这两块已经实现了 ack-and-continue + barrier_inflight，**方向正确**。

## 2. 缺口：A3 硬驱赶路径下缺互斥护栏

新 A3 新增 `pal_suspend.h` 的 `PalSuspendThread`（Windows 硬驱赶）。**硬驱赶线程互相挂起需要 `m_dwForbidSuspendThread` 式互斥**——CRAG 现有 `ForbidSuspendScope` 只处理「软握手中 GC 等待线程」，不处理「硬 SuspendThread 驱赶中线程互相挂」。

**缺口**：当一个线程 T1 正持某把锁（如 RegionManager GcSpinLock），A3 硬驱赶逻辑调用 `SuspendThread(T1)` 挂起 T1——若 T1 正等 T2 释放的锁，则死锁。

## 3. 强化设计

### 3.1 新增 forbid 互斥护栏（对齐 CoreCLR `m_dwForbidSuspendThread`）

在 `ManagedThread` 或 PAL 层新增**跨线程 forbid 计数**：

```cpp
// 对齐 CoreCLR m_dwForbidSuspendThread
std::atomic<uint32_t> forbid_suspend_count{0};  // ManagedThread 字段

// 挂起他人前:
ForbidSuspendThreadHolder self;          // 自增自己 forbid
auto* target = ...;
if (target->forbid_suspend_count.load(acquire) != 0)
    goto retry;                          // 目标正 forbid → 放弃/重试
SuspendThread(target, ...);
if (target->forbid_suspend_count.load(acquire) != 0) {
    ResumeThread(target);
    goto retry;                          // 挂起后重读，still forbidden
}
```

**关键**：CoreCLR 在挂起后**重读**目标 forbid，非零则 resume 重试——这是硬驱赶互斥的核心。

### 3.2 与现有 `ForbidSuspendScope` 的关系

- `ForbidSuspendScope`（软，本线程 forbid depth）→ **保留**，作为软握手 anti-deadlock
- 新增 `ForbidSuspendThreadHolder`（跨线程 forbid 互斥）→ **新增**，用于硬驱赶

两者互补，不冲突。

### 3.3 挂起期间禁 alloc（对齐 CoreCLR `IncCantAllocCount`）

CoreCLR 挂起期间置 `IncCantAllocCount()`（被停线程可能持堆锁）。CRAG 在硬驱赶路径（`pal_suspend.h`）也需：
- 挂起方在挂起期间自身不可分配（可能持堆锁）
- 用现有 `tls_forbid_suspend_depth` 或新增等价机制保证

## 4. 调用点

| 位置 | 需求 |
|:---|:---|
| `pal_suspend.h` 的 `PalSuspendThread` 实现 | 外包装 `ForbidSuspendThreadHolder` |
| `RequestGlobalSafepoint` 的硬驱赶兜底 | `PalSuspendThread` 调前检查 forbid |
| 所有持 GC 锁的临界区（RegionManager GcSpinLock） | 已由 `ScopedPreemptiveMode` 处理软路径；硬驱赶路径需补 forbid 互斥 |

## 5. 验证

1. **双线程互挂测试**：两个线程同时尝试硬驱赶对方 → 不死锁（`ForbidSuspendThreadHolder` 防护）
2. **并发满负载**：A3 硬驱赶开启下，ASAN/TSAN 无死锁/数据竞争
3. **回归**：现有 `ForbidSuspendScope` 行为不变

## 6. 已拍板决策

- [x] **不废弃** forbid_suspend（原方向修正，对齐 CoreCLR）
- [x] `ForbidSuspendScope`（软）保留
- [x] 新增 `ForbidSuspendThreadHolder`（跨线程硬驱赶互斥，对齐 `m_dwForbidSuspendThread`）
- [x] 硬驱赶期间禁 alloc（对齐 `IncCantAllocCount`）
- [x] 挂起后重读目标 forbid，非零 resume 重试

## 7. 归属

本设计为 P1 实现输入。「新增 ForbidSuspendThreadHolder 硬驱赶互斥」是 P1 的 `pal_suspend.h` 实现子项，在 ASAN/TSAN 护网下执行。