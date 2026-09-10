# gc-iw-b2：Parallel mark lock-free deque 改造（C7）实现计划

> **面向执行 Agent：** 使用 dev-executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。完成时先归档本子任务，再同步父任务。

**目标：** 替换 `gc_parallel_mark.cpp` 中基于 `std::mutex` 的 worker deque 为 Chase-Lev lock-free work-stealing deque，消除偷取路径的锁争用。

**架构：** Chase-Lev lock-free deque（CoreCLR GC、TBB、Java ForkJoinPool 同款算法）。Owner 操作 `bottom` 指针（push/pop 无锁），thief 操作 `top` 指针（steal 用 CAS）。

- **Owner push/pop**: store/load with relaxed/release ordering, no CAS
- **Thief steal**: CAS on `top`, with `bottom` as emptiness guard
- **Last-element race**: Owner detects `b == t+1` after decrement and CAS-es with thieves

**技术栈：** C++20, `<atomic>`, MSVC intrinsics

**架构审核模式：** critical

**结构告警重点：** 重复代码；LockFreeDeque 与 overflow fallback 的交互

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: GC/parallel-mark
- capabilityItem: lock-free-deque
- ownerSubjectId: C7
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: [parallel mark stress test (GC stress with multiple threads)]
- requiredGates: collector -> stress

**设计文档：** n/a

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

---

## 修改文件

### `src/native/runtime-core/gc/gc_parallel_mark.h`

1. 新增 `LockFreeDeque` 类，包含：
   - `std::atomic<int64_t> top_` —  thieves 读取，CAS 修改
   - `std::atomic<int64_t> bottom_` — owner 读写
   - `MarkChunk* entries_` — power-of-2 循环缓冲
   - `int64_t log_capacity_` — 当前容量 log2
   - `int64_t mask_` — 容量 - 1

2. `MarkWorkerState` 中：
   - 删除 `std::vector<MarkChunk> deque`
   - 删除 `std::mutex steal_mutex`
   - 新增 `LockFreeDeque lockfree_deque`

### `src/native/runtime-core/gc/gc_parallel_mark.cpp`

1. 重写 `PushChunk` — owner 无锁 push：
   ```
   bottom = worker->bottom++
   entries[bottom & mask] = chunk
   ```

2. 重写 `PopChunk` — owner 无锁 pop：
   ```
   bottom = worker->bottom - 1
   if bottom <= top: 竞争处理
   ```

3. 重写 `StealChunk` — thief CAS steal：
   ```
   top = victim->top
   if top >= victim->bottom: empty
   CAS(&victim->top, top, top+1)
   ```

4. 更新 `DestroyParallelMarkContext` — 释放 lock-free deque entries

5. 删除所有 `steal_mutex` 相关代码

### 算法细节

**Push（owner 调用）：**
```cpp
int64_t b = worker->bottom_.load(std::memory_order_relaxed);
int64_t t = worker->top_.load(std::memory_order_acquire);
int64_t size = 1LL << worker->log_capacity_;
if (b - t >= size - 1) {
    // Grow: double capacity (rare)
    int64_t new_log = worker->log_capacity_ + 1;
    auto* new_entries = new MarkChunk[1LL << new_log];
    int64_t new_mask = (1LL << new_log) - 1;
    for (int64_t i = t; i < b; i++) {
        new_entries[i & new_mask] = worker->entries_[i & worker->mask_];
    }
    delete[] worker->entries_;
    worker->entries_ = new_entries;
    worker->log_capacity_ = new_log;
    worker->mask_ = new_mask;
}
worker->entries_[b & worker->mask_] = chunk;
worker->bottom_.store(b + 1, std::memory_order_release);
```

**Pop（owner 调用）：**
```cpp
int64_t b = worker->bottom_.load(std::memory_order_relaxed) - 1;
worker->bottom_.store(b, std::memory_order_relaxed);
std::atomic_thread_fence(std::memory_order_seq_cst);
int64_t t = worker->top_.load(std::memory_order_relaxed);
if (t <= b) {
    // Non-empty or last-element race.
    MarkChunk chunk = worker->entries_[b & worker->mask_];
    if (t == b) {
        // Last element: compete with thieves.
        if (!worker->top_.compare_exchange_strong(t, t + 1,
                std::memory_order_release, std::memory_order_relaxed)) {
            worker->bottom_.store(b + 1, std::memory_order_relaxed);
            return false; // thief stole it
        }
        worker->bottom_.store(b + 1, std::memory_order_release);
    }
    *out = chunk;
    return true;
}
// Empty
worker->bottom_.store(b + 1, std::memory_order_relaxed);
return false;
```

**Steal（thief 调用）：**
```cpp
auto& victim = ctx->workers[victim_idx];
int64_t t = victim.top_.load(std::memory_order_acquire);
std::atomic_thread_fence(std::memory_order_seq_cst);
int64_t b = victim.bottom_.load(std::memory_order_acquire);
if (t >= b) return false; // empty
MarkChunk chunk = victim.entries_[t & victim.mask_];
if (victim.top_.compare_exchange_strong(t, t + 1,
        std::memory_order_release, std::memory_order_relaxed)) {
    *out = chunk;
    return true;
}
return false; // race lost
```

## 验证

1. dotnet build Chaos.IL2CPP.Generator — 确认 codegen 编译通过
2. native build (cmake) — 确认 native 编译通过
3. GC stress 测试 — 确认 parallel mark 在多线程场景下不崩溃
4. foundation-dll GC verification pipeline — 确认 10 subjects 仍然 pass
