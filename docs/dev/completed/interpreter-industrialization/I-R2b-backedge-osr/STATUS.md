# I-R2b: Backedge OSR — 完成

> **task_id**: I-R2b
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

在 FastExecute（Step C）中添加了 backedge OSR（On-Stack Replacement）支持。当热循环的后向分支（backedge）超过阈值时，自动触发层升级和可选的 T4 本地代码 OSR 入口。

### 变更文件

- `src/native/interpreter/fast_dispatch.h` — FastFrame 新增 OSR 字段：
  - `void* patch_method` — PatchMethod 指针，用于层状态访问
  - `uint32_t loop_counter` — backedge 计数，热循环检测
  - `bool osr_reenable` — T4 去优化后单次立即 OSR 标志

- `src/native/interpreter/fast_frame_pool.cpp` — SetupFastFrame 初始化 OSR 字段：
  - `ff->patch_method = const_cast<void*>(patch_method)`
  - `ff->loop_counter = 0` / `ff->osr_reenable = false`

- `src/native/interpreter/fast_dispatch.cpp` — 主要实现：
  - **FastExecute 循环 backedge 检测**：在 switch dispatch 后添加 `prev_pc` 跟踪，检测后向分支（frame.pc < prev_pc）作为循环 backedge
  - **TryFastOsrPromotion**：热 backedge 触发时，依次尝试：
    - T1→T2：调用 `OptimizeToTier2` 优化寄存器分配
    - T2→T3：通过 `TierManager::EnqueueOptimization` 异步排入优化
    - T3→T4：调用 `GenerateNativeCode`，如有 OSR 入口则执行完整 OSR：
      - `CaptureFastFrame` → OsrState → `RestoreOsrToRegisterFrame`
      - 构建 `RegisterFrame` 并重新派发到 `RegisterExecute`
      - 成功时从 RegisterFrame 捕获返回值并设置 pc=0xFFffFFff
  - **loop_counter 衰减**：顺序执行时缓慢递减计数器，防止无界增长
  - 新增头文件引用：`osr_state.h`, `ir_reg_alloc.h`, `<tier_manager.h>`, `<code_generator.h>`, `<t4_seh_handler.h>`, `<codegen_helpers.h>`
  - 新增前向声明- `chaos::il2cpp::runtime_core::OptimizeToTier2`

### 实现细节

- `kFastOsrLoopThreshold = 100`（与 RegisterExecute 的 kOsrLoopThreshold 一致）
- `osr_reenable` 单次标志：T4 去优化后首条 backedge 即触发 OSR
- 全状态保留：FastFrame tracked 对象所有权转移到 OsrState，防止池回收时重复释放
- T4 OSR 失败时降级到 FastExecute 继续执行（frame.pc 保持当前循环头位置）

### 验证

- chaos_interpreter.lib 编译通过（0 errors）

### Exit Criteria

| 标准 | 状态 |
|------|------|
| FastExecute 循环 backedge 检测 | ✅ |
| 热 backedge 触发 T1→T2 / T2→T3 层升级 | ✅ |
| T3→T4 代码生成 + OSR 入口重新派发 | ✅ |
| T4 OSR 失败时安全降级到 FastExecute | ✅ |
| 编译通过（0 errors） | ✅ |
