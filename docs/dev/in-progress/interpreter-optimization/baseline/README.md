# Phase 0 基线说明

> **日期**：2026-08-13 ｜ **commit**：46a417b23

## 已固化

### 正确性基线
- unit 套件：`OVERALL: OK` — pass=2248, known_fail=29（全部 [KNOWN-FAIL] 既有,非回归）, total=2277
- unit/driver：26/26；unit/snapshot：108/108
- numerics fact：`aot: 180/180, jit: 180/180`（A2 修复后）

### 性能基线
- `entry.exe --benchmark-all 1000`：180 subject 的 ops/sec + allocatedBytes JSON（`perf_ops_per_sec_1000_iters`）
- 数据落盘 `baseline/baseline.json`

## 已知测量缺口（诚实标注）

1. **解释器专属性能场景缺失**：`--microbench` 原为 stub。**尝试实现 standalone 驱动 `InterpreterVM::Execute` 的合成算术基准，因 segfault 回退**（`/tmp` 的 D:\...microbench.cpp 试版）：说明解释器 VM 需要完整 runtime/thread/GC 装配，裸驱动不安全，复杂度被计划低估。
2. → 要量化"每项解释器改动的前后性能差异"，需改用**生产路径**：benchmark 一个"走 `InterpreterEntryDirect` 的真实方法"（如 hotpatch 应用后 `kHotpatchActive` 的 slot），而非 hand-built IR 驱动裸 VM。此为 Phase 0 的未决子任务，需用户对"测量方式"再定。
3. GC bytes 直方图未归一化到 benchmark 输出（profiler 有计数器未接线）。

## 影响计划的事实（修正）

- 正确性基线（fact 180/180 + unit/snapshot）**可用**于所有 8 项的"不回归"gate。
- **解释器专属 ns/op 性能锚尚未建立**（standalone VM 驱动 segfault；需改生产路径测量）。这是计划当前的真实阻塞——后续性能敏感项（对象模型 1.1/1.2、safepoint 2.1/2.2）缺乏可信性能 diff。
- **选项**：(a) 补一个 hotpatch 应用的真实解释器方法做 benchmark；(b) 先以"正确性 + 正确性差异（GC 悬挂/崩溃数）+ 减少 keep-native 依赖数"作为这些项的量化指标，解释器 ns/op 作为后续专项；(c) 花 ~0.5-1d 调试 standalone VM 驱动直到安全。
