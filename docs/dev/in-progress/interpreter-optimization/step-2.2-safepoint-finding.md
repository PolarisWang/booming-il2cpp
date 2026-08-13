# Step 2.2 调研：解释器 in-band SafepointPoll — GC 挂起机制微妙，建议谨慎单独做

> **日期**：2026-08-13 ｜ **来源**：读 fast_dispatch_execute.inc + thread_state.cpp SafepointPoll

## 现状（已定位）
- 解释器 `fast_dispatch_execute.inc` 主循环（computed-goto :484-494 与 MSVC switch :673-677）有"每 64 指令"decimated check，只 `pending_abort/pending_interrupt` load→`threw_exception`，**不调 `threading::SafepointPoll()`**。
- GC 挂起解释器线程现在靠抢占式信号/APC 重定向 `thread_state.cpp:327-400`（`ggc_suspend_trampoline` 在正常栈跑 `SafepointPoll`）。

## SafepointPoll 语义（thread_state.cpp:236）
- fast path：`suspend_seq==0` → 只查 abort/interrupt（**throw chaos_managed_exception**）。
- slow path：safepoint active → `suspend_ack` 确认；cooperative 模式则**等待 GC 完成**。

## 为什么不能草率内联
1. **异常语义冲突**：SafepointPoll 的 abort/interrupt 是 **C++ throw chaos_managed_exception**；解释器路径预期 `frame.threw_exception`（编 throughed 状态而非 C++ 异常）。直接内联会破坏解释器异常传播。
2. **长循环等待的帧可扫描性**：若 safepoint active 且 cooperative，解释器线程会在 SafepointPoll 内**阻塞**，GC 扫 `interp_frame_scanner` 的 FastFrame 链——此时帧必须处于可扫描一致态（任意 pc 每 64 指令）。当前无验证此点。
3. **与信号挂起交互**：现有抢占式重定向目标也是 SafepointPoll；解释器自身再内联调它可能与其竞争/重复（如 suspend_ack 重复写）。

## 推荐的安全实现方式（不急于 commit）
- 在 decimated check 内，**保留现有 abort/interrupt 手动检查**（转 threw_exception），**额外**用 `try{ threading::SafepointPoll(); } catch(chaos_managed_exception&){ frame.threw_exception=true; frame.pc=9999; }` 捕获转译——避免 C++ throw 泄漏。
- 但长循环 wait 的帧可扫描性需 **GC-stress + 长循环场景**验证（非快速单测能覆盖）。
- 与信号挂起的双路径竞态需专项。

## 判定
Step 2.2 是**真 gap 但 GC 挂起机制微妙**：正确内联需处理异常语义 + 帧可扫描 + 信号双路径。**不建议本长会话草率提交**——应作为一个需要 GC-stress 强回归的独立 careful change。若用户决定现在做，我会按上述 try/catch 转译 + 加 GC-stress 场景执行；否则留作专项。
