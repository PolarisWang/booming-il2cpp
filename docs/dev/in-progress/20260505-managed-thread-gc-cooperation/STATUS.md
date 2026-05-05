# STATUS — 托管线程模型 + GC 协作设计

## 任务来源
Phase 5 完成后用户指出的最大架构缺口："托管线程模型 + GC 协作 — 最大架构缺口"

## 已知问题（5 项）
1. `std::thread` + global mutex table，无托管生命周期管理
2. `std::unordered_map` + global mutex 的 monitor，每个 `lock(obj)` 走全局锁
3. 无线程挂起机制 → bdwgc 无法协作（无 cooperative→preemptive GC_TRANSITION）
4. 无 ThreadPool（`Task.Run` 无调度器）
5. `Interlocked.MemoryBarrier` 空实现（x64 强内存模型下不影响正确性，但违反 ECMA 规范）

## 阶段
- phase: brainstorming
- task_type: brainstorm
- blocking_questions: []  # 还未确认
- question_clearance: not_cleared
