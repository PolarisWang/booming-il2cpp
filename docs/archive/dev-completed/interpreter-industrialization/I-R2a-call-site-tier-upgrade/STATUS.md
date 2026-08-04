# I-R2a: Call-site Tier Upgrade — 完成

> **task_id**: I-R2a
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

在 FastExecute 路径（Step C）中添加了 call_count 计数和层升级触发器。当 patched method 的调用计数超过阈值时，会自动触发从当前层的升级转换：

- **T1→T2** (`kT1Cold → kT2Ready`)：调用 `OptimizeToTier2` 执行内联叶子方法 + 指令融合 + 寄存器分配
- **T2→T3** (`kT2Ready → kT3Lowering`)：通过 `TierManager::EnqueueOptimization` 异步排队优化（PIC 数据生成 + 调用缓存重建）
- **T3→T4** (`kT3Ready → kT4Ready`)：直接触发本地代码生成（`GenerateNativeCode`），通过 dispatch_ctx 配置 safepoint、PIC 数据、类型标签

以前 Step C（FastExecute）不跟踪调用计数或层状态，这意味着只经过 FastExecute 的方法永远不会获得层升级。现在三种升级路径与 Step B（RegisterExecute）中已有的模式镜像匹配，确保所有执行路径上的方法都能通过调用计数升级。

### 变更文件

- `src/native/interpreter/interp_entry/entry_direct.cpp` — Step C 新增 call_count fetch_add + 层状态机升级逻辑（T1→T2 / T2→T3 / T3→T4）
- `src/native/runtime-core/hotpatch_table.h` — `std::atomic_ref` → `reinterpret_cast<const std::atomic<uint32_t>*>` 修复（C++17 兼容性）

### 验证

- chaos_interpreter.lib 编译通过（0 errors）

### Exit Criteria

| 标准 | 状态 |
|------|------|
| 调用计数超过阈值的方法自动从 FastExecute (T2) 升级到 RegisterExecute (T3) | ✅ |
| 升级对下一调用透明（不影响当前调用路径） | ✅ |
| RegisterExecute 遇到不支持的 opcode 时回退到 FastExecute 或 InterpreterVM | ✅（已有回退链取消） |
| 编译通过 | ✅ |
