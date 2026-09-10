# 托管线程模型 + GC 协作方案 C 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 实现方案 C 的完整线程/GC 协作系统：generation 计数器 safepoint、GC_TRANSITION 宏、SyncBlock 条件变量、缺失同步原语补齐。

**架构：** 全局 `s_generation` 原子计数器作为单一定点；每帧 safepoint check = 一次 atomic load + compare；超出等待超时的线程由 bdwgc 保守扫描兜底。所有 managed↔native 边界插入 GC_TRANSITION 宏切换 COOPERATIVE/PREEMPTIVE 模式。

**技术栈：** C++20（std::atomic、std::condition_variable）、bdwgc（GC_collect_a_little / 保守扫描）、Scriban 模板（bridge stubs）

**架构审核模式：** critical（触及 runtime_core 和 codegen bridge 模板双线）

**结构告警重点：** 文件职责与拆分（gc_transition.h 是否独立）、重复模式（SyncBlock 条件变量是否应复用 pattern）

**权责图审核主题：**
- `thread_state.h/cpp` — 线程对象模型 + safepoint 定点，不做 GC 触发决策
- `runtime_core.cpp` — SyncBlock 操作 + Monitor 原语，不做线程生命周期管理
- `gc_transition.h` — 纯宏内联，不含分配/锁逻辑
- Bridge 模板 — 只插宏，不含语义决策

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a（不是 foundation-dll 能力族）
- capabilityItem: n/a
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None（统一路径，无特殊热更处理）
- formalVerificationObjects: []
- requiredGates: []

**设计文档：** STATUS.md 设计摘要（无独立 design-v1-01.md）

**问题清零来源：** brainstorm-approved STATUS.md（blocking_questions: [], question_clearance: cleared, clearance_confirmed_by_user: true）

**计划来源：** direct-plan（brainstorm→plan 直接分流）

**预期知识沉淀：** n/a（全局架构知识已在 STATUS.md 记录，不需要额外 wiki 路径）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 任务 1: generation 计数器 safepoint 重写

### 涉及文件
- `src/native/runtime-core/thread_state.h` — ManagedThread 新增字段
- `src/native/runtime-core/thread_state.cpp` — SafepointPoll/RequestGlobalSafepoint/ReleaseGlobalSafepoint 重写

### 变更内容

**thread_state.h** — ManagedThread 加三个字段：
```cpp
struct ManagedThread {
    // ... 现有字段 ...
    std::atomic<uint32_t> gc_mode{0};  // 0=COOPERATIVE, 1=PREEMPTIVE
    uint32_t last_seen_gen{0};         // 上次确认的 generation
    std::atomic<bool> pending_abort{false}; // Thread.Abort pending 标志
};
```

并声明：
```cpp
/// Check if GC needs attention — generation-based fast path.
/// Returns true if the calling thread should yield for GC.
bool SafepointRequested() noexcept;
```

**thread_state.cpp** — 核心 safepoint 机制改为 generation 对比：

```cpp
namespace {
    // generation: even = idle, odd = GC in progress
    std::atomic<uint32_t> s_generation{0};
    constexpr uint32_t kGcGenerationMask = 1u;
}

bool SafepointRequested() noexcept {
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    if (gen == 0) return false;  // fast path: no GC activity ever
    auto* thread = tls_this_thread;
    if (thread == nullptr) return false;
    return (gen & kGcGenerationMask) != 0;  // odd = GC active
}

void SafepointPoll() noexcept {
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    if ((gen & kGcGenerationMask) == 0) {
        return;  // fast path: no GC pending
    }

    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    thread->at_safepoint = true;
    thread->last_seen_gen = gen;

    // Spin with yield until generation flips (even = released).
    // Timeout: after ~10ms, the GC proceeds with conservative scan anyway.
    int spins = 0;
    while ((s_generation.load(std::memory_order_acquire) & kGcGenerationMask) != 0) {
        if (++spins > 10000) {  // ~10ms at ~1µs/yield
            std::this_thread::yield();
            spins = 0;
        }
    }

    thread->at_safepoint = false;
}

uint32_t RequestGlobalSafepoint() noexcept {
    // Toggle to odd (GC in progress).
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    uint32_t desired = gen | kGcGenerationMask;
    s_generation.store(desired + 1, std::memory_order_release);
    // 返回新 generation，用于等待确认
    return desired + 1;
}

void ReleaseGlobalSafepoint(uint32_t /*generation*/) noexcept {
    // Toggle to even (released).
    uint32_t gen = s_generation.load(std::memory_order_acquire);
    s_generation.store((gen + 1) & ~kGcGenerationMask, std::memory_order_release);
}
```

### 退出条件
- `SafepointPoll()` fast path 只发出一条 atomic load + branch，无 spin，无 yield
- `RequestGlobalSafepoint()` 正确翻转到奇数
- `ReleaseGlobalSafepoint()` 翻转到偶数
- `ChaosInterlockedMemoryBarrier` 不受影响

### 预估行数
- thread_state.h: ~10 行
- thread_state.cpp: ~40 行

---

## 任务 2: GC_TRANSITION 宏

### 涉及文件
- **新增** `src/native/runtime-core/gc_transition.h`

### 变更内容

```cpp
#ifndef CHAOS_IL2CPP_GC_TRANSITION_H_
#define CHAOS_IL2CPP_GC_TRANSITION_H_

#include "thread_state.h"
#include <atomic>

namespace chaos::il2cpp::runtime_core {

// GC mode constants
constexpr uint32_t kGcModeCooperative = 0;
constexpr uint32_t kGcModePreemptive  = 1;

/// Transition from COOPERATIVE to PREEMPTIVE mode.
/// Must be called before entering native code that may block.
inline void GcTransitionToPreemptive() noexcept {
    auto* thread = threading::tls_this_thread;
    if (thread == nullptr) return;
    thread->gc_mode.store(kGcModePreemptive, std::memory_order_release);
    // Check if GC was requested while we were still cooperative.
    // If so, participate in safepoint before entering native code.
    threading::SafepointPoll();
}

/// Transition from PREEMPTIVE to COOPERATIVE mode.
/// Must be called after returning from native code.
inline void GcTransitionToCooperative() noexcept {
    auto* thread = threading::tls_this_thread;
    if (thread == nullptr) return;
    thread->gc_mode.store(kGcModeCooperative, std::memory_order_release);
    // Check if GC is active and participate if so.
    threading::SafepointPoll();
}

}  // namespace chaos::il2cpp::runtime_core

// Convenience macros for bridge stubs.
// Usage:
//   GC_TRANSITION_TO_PREEMPTIVE();
//   // ... native call ...
//   GC_TRANSITION_TO_COOPERATIVE();
#define GC_TRANSITION_TO_PREEMPTIVE() \
    ::chaos::il2cpp::runtime_core::GcTransitionToPreemptive()

#define GC_TRANSITION_TO_COOPERATIVE() \
    ::chaos::il2cpp::runtime_core::GcTransitionToCooperative()

#endif  // CHAOS_IL2CPP_GC_TRANSITION_H_
```

### 退出条件
- 头文件自包含，无额外依赖
- GcTransitionToPreemptive/Cooperative 内联实现，无函数调用开销
- 宏包装方便在 Scriban 模板中使用

### 预估行数
- gc_transition.h: ~55 行

---

## 任务 3: SyncBlock 条件变量 + Monitor 原语补齐

### 涉及文件
- `src/native/runtime-core/runtime_core.cpp` — SyncBlock 加 condition_variable，MonitorTryEnter/IsEntered/Pulse/Wait/ThreadSleep

### 变更内容

**3a. SyncBlock 新增条件变量：**
```cpp
struct SyncBlock {
    CHAOS_IL2CPP_RECURSIVE_LOCK_MUTEX mutex;
    std::condition_variable_any cond;  // for Monitor.Wait/Pulse
};
```

**3b. 新增 `MonitorTryEnter`：**
```cpp
bool MonitorTryEnter(void* monitor_target) noexcept {
    if (monitor_target == nullptr) return false;
    auto* header = static_cast<ObjectHeader*>(monitor_target);
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;

    uint64_t sync = header->sync_state;
    for (;;) {
        if ((sync & kSyncInflatedBit) != 0) {
            auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
            return sb != nullptr && sb->mutex.try_lock();
        }
        if ((sync & kSyncLockedBit) == 0) {
            // Free — try to acquire with thin lock.
            uint64_t desired = kSyncLockedBit |
                (static_cast<uint64_t>(tid) << kSyncThreadShift);
            if (AtomicCAS(&header->sync_state, sync, desired)) return true;
            continue; // CAS failed, reload
        }
        return false; // Already locked (thin), can't acquire
    }
}
```

**3c. 新增 `MonitorIsEntered`：**
```cpp
bool MonitorIsEntered(void* monitor_target) noexcept {
    if (monitor_target == nullptr) return false;
    auto* header = static_cast<ObjectHeader*>(monitor_target);
    const int32_t tid = threading::GetCurrentThreadId();
    if (tid == 0) return false;

    uint64_t sync = header->sync_state;
    if ((sync & kSyncInflatedBit) != 0) {
        // Inflated — check recursion count > 0 for our thread.
        // For simplicity: if the SyncBlock exists, we hold it.
        return sync != 0;
    }
    if ((sync & kSyncLockedBit) != 0) {
        uint64_t stored_tid = (sync >> kSyncThreadShift) & 0x3FFFFFFF;
        return stored_tid == static_cast<uint64_t>(tid);
    }
    return false;
}
```

**3d. Monitor.Wait / Pulse / PulseAll 条件变量：**
```cpp
bool MonitorWait(void* monitor_target, int32_t timeout_ms) noexcept {
    if (monitor_target == nullptr) return false;
    auto* header = static_cast<ObjectHeader*>(monitor_target);

    // Must inflate for condition variable.
    uint64_t sync = header->sync_state;
    if ((sync & kSyncInflatedBit) == 0) {
        // Force inflation.
        InflateAndEnter(monitor_target, sync);
        // ... MonitorExit after inflation logic ...
    }

    auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
    if (sb == nullptr) return false;

    // Release the lock and wait.
    if (timeout_ms < 0) {
        sb->cond.wait(sb->mutex);
        return true;
    }
    return sb->cond.wait_for(sb->mutex, std::chrono::milliseconds(timeout_ms))
           == std::cv_status::no_timeout;
}

bool MonitorPulse(void* monitor_target) noexcept {
    if (monitor_target == nullptr) return false;
    auto* header = static_cast<ObjectHeader*>(monitor_target);
    uint64_t sync = header->sync_state;
    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) { sb->cond.notify_one(); return true; }
    }
    return false;
}

bool MonitorPulseAll(void* monitor_target) noexcept {
    if (monitor_target == nullptr) return false;
    auto* header = static_cast<ObjectHeader*>(monitor_target);
    uint64_t sync = header->sync_state;
    if ((sync & kSyncInflatedBit) != 0) {
        auto* sb = reinterpret_cast<SyncBlock*>(sync & ~3ull);
        if (sb != nullptr) { sb->cond.notify_all(); return true; }
    }
    return false;
}
```

**3e. Thread.Sleep：**
```cpp
bool ThreadSleep(int32_t timeout_ms) noexcept {
    if (timeout_ms < 0) return false;
    // Check pending abort.
    auto* thread = threading::tls_this_thread;
    if (thread != nullptr && thread->pending_abort.load(std::memory_order_acquire)) {
        return false;
    }
    // Sleep with safepoint re-entry.
    // V1: simple sleep. V2: condition variable on thread object.
    if (timeout_ms == 0) {
        std::this_thread::yield();
        return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    return true;
}
```

### 退出条件
- MonitorTryEnter 返回 false 当锁已被其他线程持有（非阻塞）
- MonitorIsEntered 对当前线程持有锁时返回 true
- MonitorWait 释放锁并等待，Pulse/PulseAll 唤醒等待线程
- ThreadSleep 非负超时正常返回

### 预估行数
- runtime_core.cpp: ~120 行

---

## 任务 4: Bridge stub 模板插入 GC_TRANSITION

### 涉及文件
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.RuntimeSkeleton.ThreadingSyncKernelStub.cpp.scriban`

### 变更内容
在 stub 函数的入口和返回前插入 GC_TRANSITION 宏：

```scriban
struct {{ stub_name }}_KernelArgs
{
{{~ if this_field_declaration.size > 0 ~}}
    {{ this_field_declaration }}
{{~ end ~}}
{{~ if arg_field_declarations.size > 0 ~}}
    {{ arg_field_declarations }}
{{~ end ~}}
    void* return_value;
};

// ... contract_id / version assertions ...

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL {{ stub_name }}(
    ... existing params ...)
{
    (void)bridge;
    (void)code_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    if (managed_args == nullptr) {
        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;
    }

    GC_TRANSITION_TO_PREEMPTIVE();        // <-- INSERT

    auto* request = static_cast<{{ stub_name }}_KernelArgs*>(managed_args);
{{~ if arg_validation_statements.size > 0 ~}}
    {{ arg_validation_statements }}
{{~ end ~}}
    if (request->return_value == nullptr) {
        GC_TRANSITION_TO_COOPERATIVE();    // <-- INSERT before early return
        return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT;
    }

    const auto result = {{ helper_call_expression }};
    *static_cast<bool*>(request->return_value) = result;

    GC_TRANSITION_TO_COOPERATIVE();        // <-- INSERT before return
    return CHAOS_BRIDGE_STATUS_OK;
}
```

### 其他需要插入 GC_TRANSITION 的模板（检查后确认是否需要）
- `NativeReferenceProof.RuntimeSkeletonIdentityStructManagedInvokeStub.cpp.scriban`（如果用 hotupdate/managed-invoke 路径的也需要）

### 退出条件
- Sync kernel stub 在调用 helper 前切换到 PREEMPTIVE，返回前切回 COOPERATIVE
- 早期 return 路径也正确切回

### 预估行数
- Scriban 模板: ~5 行插入点

---

## 任务 5: thread_pool.cpp 插入 GC_TRANSITION

### 涉及文件
- `src/native/runtime-core/thread_pool.cpp`

### 变更内容
在 WorkerLoop 执行工作项前后插入 transition：

```cpp
void WorkerLoop() noexcept {
    int32_t tid = AllocateThreadId();
    RegisterThread(tid, nullptr);

    for (;;) {
        WorkItem item;
        {
            std::unique_lock<CHAOS_IL2CPP_MUTEX> lock(s_mutex);
            s_work_available.wait(lock, [] {
                return !s_queue.empty() || s_shutdown.load(std::memory_order_acquire);
            });
            if (s_shutdown.load(std::memory_order_acquire)) break;
            item = s_queue.front();
            s_queue.pop();
            s_queue_depth.fetch_sub(1, std::memory_order_relaxed);
        }

        // Execute work item in cooperative mode.
        // The bridge stubs will transition to preemptive as needed.
        s_busy_workers.fetch_add(1, std::memory_order_relaxed);
        if (item.callback) {
            GC_TRANSITION_TO_COOPERATIVE();      // <-- INSERT
            item.callback(item.context);
            GC_TRANSITION_TO_PREEMPTIVE();        // <-- INSERT
        }
        s_busy_workers.fetch_sub(1, std::memory_order_relaxed);
    }

    UnregisterThread();
}
```

### 退出条件
- 回调执行在 COOPERATIVE 模式下
- 等待工作项时在 PREEMPTIVE 模式（因条件变量等待会阻塞）

### 预估行数
- thread_pool.cpp: ~5 行

---

## 任务 6: 缺失同步原语补齐（SpinLockExit / LockEnter / WaitHandleSet/Reset）

### 涉及文件
- `src/native/runtime-core/runtime_core.cpp`

### 变更内容

**SpinLockExit：**
```cpp
bool SpinLockExit(void* spinlock_target) noexcept {
    if (spinlock_target == nullptr) return false;
    auto* header = static_cast<ObjectHeader*>(spinlock_target);
    const int32_t tid = threading::GetCurrentThreadId();
    uint64_t sync = header->sync_state;
    // SpinLock is always thin: clear locked bit + thread id.
    if ((sync & kSyncLockedBit) == 0) return false;
    uint64_t stored_tid = (sync >> kSyncThreadShift) & 0x3FFFFFFF;
    if (stored_tid != static_cast<uint64_t>(tid)) return false;
    AtomicStoreRelease(&header->sync_state, 0);
    return true;
}
```

**LockEnter / LockExit（for System.Threading.Lock）：**
```cpp
bool LockEnter(void* lock_target) noexcept {
    return MonitorEnter(lock_target);
}

bool LockExit(void* lock_target) noexcept {
    return MonitorExit(lock_target);
}
```

**WaitHandleSet / WaitHandleReset：**
```cpp
bool WaitHandleSet(void* /*wait_handle*/) noexcept {
    // V1: stub — WaitHandle not yet integrated.
    // Actual implementation requires event object integration.
    return false;
}

bool WaitHandleReset(void* /*wait_handle*/) noexcept {
    // V1: stub.
    return false;
}
```

### 退出条件
- SpinLockExit 正确释放 thin lock
- LockEnter/Exit 复用 Monitor 原语
- WaitHandleSet/Reset 为 V1 stub（名称匹配 codegen ABI，不链接失败）

### 预估行数
- runtime_core.cpp: ~40 行

---

## 执行顺序

| 步 | 任务 | 依赖 | 风险 |
|----|------|------|------|
| 1 | generation 计数器 safepoint 重写 | 无 | 中 — 当前 SafepointPoll 调用点需全部兼容 |
| 2 | GC_TRANSITION 宏 | 任务 1 | 低 — 纯新增内联函数+宏 |
| 3 | SyncBlock 条件变量 + Monitor 补齐 | 无 | 低 — thin lock 路径不变，新增 inflate path 条件变量 |
| 4 | Bridge stub 模板插入 GC_TRANSITION | 任务 2 | 低 — Scriban 模板单点插入 |
| 5 | thread_pool.cpp + GC_TRANSITION | 任务 2 | 低 — ~5 行插入 |
| 6 | 缺失原语补齐 | 无 | 低 — 各 8-40 行独立实现 |

执行顺序：**1 → 2 → [3, 4, 5, 6 可并行]**。任务 3/4/5/6 在任务 2 提供 `#include` 后互相独立。

## 验证

```bash
# 1. Native 编译
cmake --build build --target chaos_runtime_core

# 2. 完整 native 编译
cmake --build build

# 3. 运行现有 threading 测试
build/src/native/runtime-core/threading_tests

# 4. 运行 ABI 测试
build/src/native/runtime-core/abi_tests

# 5. 完整测试套件
python build/test_runner.py --suite smoke
python build/test_runner.py --suite full
```

## 收尾

1. **结构告警与架构审视**：确认 gc_transition.h 职责单一、SyncBlock condition_variable 使用正确、无重复宏定义
2. **测试通过**：所有现有测试通过，无回归
3. **归档 completed**：`mv docs/dev/in-progress/20260505-managed-thread-gc-cooperation docs/dev/completed/20260505-managed-thread-gc-cooperation`
4. **合并&提交**：创建 git commit
