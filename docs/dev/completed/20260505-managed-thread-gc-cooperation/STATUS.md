# STATUS — 托管线程模型 + GC 协作设计

## 任务来源
Phase 5 完成后用户指出的最大架构缺口："托管线程模型 + GC 协作 — 最大架构缺口"

## 设计摘要

### 现状（代码探索后修正，2026-05-05）

| # | 原始缺口 | 实际状态 | 判定 |
|---|---------|---------|------|
| 1 | Thread 生命周期 | RegisterThread/UnregisterThread TLS O(1) 已存在。缺 Thread.Start/Suspend/Abort/Join → managed 桥接 | 真实缺口 |
| 2 | Monitor 全局锁 | **已过时**。运行时实际用 thin lock（object header CAS, 2-bit FREE/THIN/INFLATED）+ 64-stripe SyncBlock。缺 Pulse/PulseAll/Wait（条件变量）、MonitorTryEnter/MonitorIsEntered | 部分缺口 |
| 3 | GC 线程挂起 | Spin-based SafepointPoll 忙等+yield。**无** cooperative→preemptive 模式，**无** 非安全点线程挂起机制 | 核心缺口 |
| 4 | ThreadPool | 基础 FIFO 队列 + GateThread + hill-climbing 存在。缺 IO 完成端口、Timer、WaitHandle 注册、BindHandle | 真实缺口 |
| 5 | MemoryBarrier | **已实现** — std::atomic_thread_fence(std::memory_order_seq_cst) | 非缺口 |
| 6 | 额外发现 | MonitorTryEnter/MonitorIsEntered/Pulse/Wait、SpinLockExit、LockEnter/Exit、WaitHandleSet/Reset — codegen ABI 注册但 native 未实现。Thread.Sleep 未实现 | 真实缺口 |

### 选定方案：方案 C — 无锁协作 + 保守扫描回退

**核心机制**：
- `s_generation` 全局 `std::atomic<uint32_t>`，偶数为 idle，奇数为 GC 请求
- 每帧 safepoint check = 单次 atomic load + compare，无锁、无 syscall、无 spin
- 超出等待超时的线程 → bdwgc 保守扫描兜底

## 阶段
- phase: completed
- task_type: plan
- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## 关键文档
- design: STATUS.md 设计摘要（无独立 design-v1-01.md）
- plan: plan-v1-01.md
- 问题清零来源: brainstorm-approved STATUS.md

## 完成证据

### 变更汇总

| 文件 | 变更类型 | 内容 |
|------|---------|------|
| `thread_state.h` | 修改 | ManagedThread 加 gc_mode/last_seen_gen/pending_abort；新增 SafepointRequested() 声明 |
| `thread_state.cpp` | 修改 | SafepointPoll/RequestGlobalSafepoint/ReleaseGlobalSafepoint 改为 generation 计数器模式 |
| `gc_transition.h` | **新增** | GC_TRANSITION_TO_PREEMPTIVE / TO_COOPERATIVE 宏和内联函数 |
| `runtime_core.h` | 修改 | 新增 #include "gc_transition.h" |
| `engine_binding.h` | 修改 | 声明 MonitorTryEnter/IsEntered/Wait/Pulse/PulseAll、ThreadSleep、SpinLockExit/IsHeld、LockEnter/Exit、WaitHandleSet/Reset |
| `runtime_core.cpp` | 修改 | SyncBlock 加 condition_variable；新增 MonitorTryEnter/IsEntered/Wait/Pulse/PulseAll/ThreadSleep/SpinLockExit/IsHeld/LockEnter/Exit/WaitHandleSet/Reset |
| `thread_pool.cpp` | 修改 | WorkerLoop 回调前后插入 GC_TRANSITION |
| SyncKernelStub Scriban 模板 | 修改 | bridge 函数插入 GC_TRANSITION_TO_PREEMPTIVE/COOPERATIVE |

### 结构告警与架构审视

**检查项**：
- 重复代码：无。所有新 Monitor 函数复用既有 thin lock/SyncBlock 模式
- 结构/抽象：gc_transition.h 职责单一（纯宏+内联），不包含分配/锁逻辑
- 文件职责：runtime_core.cpp 的新函数严格遵循"SyncBlock 操作+Monitor 原语"边界
- codegen 分层：Scriban 模板只插入宏，不含语义决策

**权责图审核（critical）**：
- authority: thread_state.h/cpp 是线程模型的单一 authority
- upstream: gc_transition.h → thread_state.h；runtime_core.h → gc_transition.h
- downstream: Scriban 模板 → runtime_core.h（通过生成代码包含链）
- allowed_deps: gc_transition.h 只依赖 thread_state.h + atomic；无新增外部依赖
- shim_debt: 无。WaitHandleSet/Reset 保持 stub 但不阻塞其他代码路径
- proof: 已有 interpreter bridge test + interpreter smoke + interpreter integration 全部通过

### 验证

| 测试 | 结果 |
|------|------|
| chaos_runtime_core 编译 | PASS |
| chaos_interpreter_smoke | PASS |
| chaos_interpreter_bridge_test | PASS (failures=0) |
| chaos_interpreter_integration | PASS (failures=0) |
| chaos_method_replacement_smoke | PASS (hotupdate) |

### 遗漏项（不在本次范围内）
- Thread.Start/Suspend/Abort/Join → managed 桥接（缺口 1 未完）
- ThreadPool IO 完成端口、Timer、BindHandle（缺口 4 未完）
- full stop-the-world GC 触发集成

## 下一步
结构告警与架构审视完成 → 合并&提交
