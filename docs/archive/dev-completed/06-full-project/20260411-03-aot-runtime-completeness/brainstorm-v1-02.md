# Brainstorm v1-02

## 背景

`Phase 2 / Batch 3` 的原计划覆盖 `ThreadStatic`、`Monitor.Enter/Exit`、GC safepoint、root reporting 和 finalizer queue。当前代码基线只有：

- `runtime_core` 已具备 `ThreadAttach/ThreadDetach` 骨架
- 进程级 `std::mutex` 已被用于 engine binding 辅助逻辑
- `GcHandleNew/GcHandleFree` 仍是空桩
- managed pipeline 还没有 `ThreadStatic` / `Monitor` 的语义标记与 proof lowering family

这意味着 Batch 3 不能直接跳到“完整多线程 GC runtime”，否则会同时把 runtime-core、bootstrap、metadata、codegen 全部拉进来，超出当前 proof-first 节奏。

## 候选方案

### 方案 A：直接实现完整线程与 GC 运行时

- 内容：把 `ThreadStatic`、monitor、safepoint、root reporting、finalizer queue 一次性做成通用 runtime 能力
- 优点：能力最完整
- 缺点：范围过大，缺少现成验证支架，容易把 Batch 3 拖成长期分支

### 方案 B：proof-first 的 runtime-core 最小闭环

- 内容：
  - `runtime_core` 增加真实可运行的最小 helper：
    - `ThreadStatic` int slot
    - `MonitorEnter/Exit`
    - `GcSafepoint`
    - `ReportThreadRoot`
    - `EnqueueFinalizer/DrainFinalizerQueue`
  - managed pipeline 只补最小语义标记与 narrowing lowering family
  - 新建 `ThreadingProof` subject，由 native proof 直接调用 helper 验证线程隔离、互斥和 GC 表面
- 优点：能形成可验证闭环，且对现有架构侵入较小
- 缺点：仍然不是完整 GC，也还没有把 `ThreadStatic` / `lock` 完整 lowering 到通用 generated code

### 方案 C：只补 runtime_core helper，不接入 managed pipeline

- 内容：只在 native runtime 做 helper，不加 semantic/linker/codegen/subject 冻结
- 优点：实现最快
- 缺点：无法把 Batch 3 固化成主线能力，后续容易回退

## 选择

选择方案 B。

## 最小 proof slice

- subject：`ThreadingProof`
- source 形态：
  - 含 `[ThreadStatic]`
  - 含 `lock`
  - 含 `volatile`
- runtime proof 形态：
  - 主线程与工作线程各自 `ThreadAttach`
  - 两条线程分别操作同名 thread-static slot，证明隔离
  - 两条线程通过同一 monitor 保护共享计数，证明互斥
  - 调用 `GcSafepoint` / `ReportThreadRoot`
  - 调用 `EnqueueFinalizer` / `DrainFinalizerQueue`
- pipeline 目标：
  - loader 能识别 `ThreadStatic` 字段
  - semantic 能产出 `requires-thread-static-storage` / `requires-monitor-enter-exit`
  - linker 能补充 thread-static closure / monitor dependency reason
  - codegen 能冻结新的 threading lowering family 与模板

## 非目标

- 本轮不实现真正的 moving GC / mark-sweep collector
- 本轮不实现完整 `Monitor` 重入等待队列与对象头 sync block
- 本轮不把任意 `ThreadStatic<T>` 泛化为通用存储系统
- 本轮不处理 finalizer 线程、后台 GC 线程或 safepoint 协调协议
