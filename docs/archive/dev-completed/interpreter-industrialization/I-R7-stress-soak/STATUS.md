# I-R7: Stress / Soak 测试 — 完成

> **task_id**: I-R7
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

为 FastExecute 执行引擎构建了完整的压力/浸泡测试套件，覆盖并发执行、SEH 处理、循环 backedge OSR、混合 opcode 序列和 tier upgrade 场景。

## 测试套件概览

| 测试名 | 类型 | 场景 | 迭代数 | 线程数 |
|--------|------|------|--------|--------|
| BasicSmoke | Smoke | 单线程 ldcI4+Ret | 1 | 1 |
| Concurrent4Thread | Stress | 4 线程并发算术运算 | 4×10k | 4 |
| Concurrent8Thread | Stress | 8 线程并发算术运算 | 8×10k | 8 |
| Concurrent16Thread | Stress | 16 线程并发算术运算 | 16×10k | 16 |
| MixedOpcode | Stress | 混合 opcode 序列（算数/分支/栈操作） | 50k | 1 |
| LoopBackedge | Stress | 循环 backedge OSR 路径（N=500） | 1k | 1 |
| SEHThrowNoCatch | Stress | 未捕获异常的安全传播 | 10k | 1 |
| SEHTryCatch | Stress | try-catch 异常处理正确性 | 10k | 1 |
| SEHConcurrent | Stress | 4 线程并发 SEH try-catch | 4×5k | 4 |
| LeaveWithFinally | Stress | Leave + finally 执行路径 | 10k | 1 |
| LeaveFinallyConcurrent | Stress | 4 线程并发 Leave+finally | 4×5k | 4 |
| TierUpgradeStorm | Stress | 快速循环升级风暴（N=200） | 2k | 1 |
| TierUpgradeStormConcurrent | Stress | 4 线程并发升级风暴（N=100） | 4×500 | 4 |

**总迭代数**: Debug: ~377,000 次 FastExecute 调用 — Release: ~377,000 次 — **全部通过，零错误**

## 实现细节

### 测试文件
- `testing/src/native/runtime-core/interpreter/interpreter_stress_test.cpp`
- 注册在 `testing/src/native/runtime-core/interpreter/CMakeLists.txt`（`test_interpreter_stress` 目标）
- Label: `stress;interpreter`

### 关键修复
1. **Frame 重置**：MixedOpcode 测试原本在循环外创建 FastFrame，导致 sp 跨迭代残留。修复为每迭代创建新 frame `= {}`。
2. **RegisterThread**：所有 FastExecute 调用前必须注册线程 TLS。添加了 `RegisterThread()` / `UnregisterThread()` 配对。
3. **异常传播语义**：FastExecute 在未捕获异常时返回 `false`（`threw_exception=true, pc=9999` 信号路径）。测试断言相应调整。

### 构建注意事项
- Debug 和 Release 构建必须使用同一版本的 `chaos_interpreter.lib`，否则 FastFrame/IRInstruction 的 struct layout 差异会导致访问违例。
- Release 构建需要在修改测试代码后同时重建 `chaos_interpreter.lib` 以确保 struct layout 一致。

## Exit Criteria

| 标准 | 状态 |
|------|------|
| Debug 全部 13 测试通过 | ✅ |
| Release 全部 13 测试通过 | ✅ |
| 并发测试覆盖 4/8/16 线程 | ✅ |
| SEH 覆盖 throw/catch/finally | ✅ |
| Loop backedge OSR 覆盖 | ✅ |
| Tier upgrade storm 覆盖 | ✅ |
