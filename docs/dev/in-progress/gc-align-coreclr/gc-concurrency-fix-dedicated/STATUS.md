# STATUS — 修复 young-GC 跨代崩溃（两层根因）

## 架构映射

- 域：GC(6) + 运行时(2)。两层根因：层1=旧代 region-gen 归 0 → 2参屏障跳卡；
  层2=屏障正确后 young-GC 晋升/回写仍毒 212/thread7。
- 复现器：`tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp`（known-failing）。
- 相关文件：见 `design-v1-01.md` 关键文件节。

## phase

`executing`（已验证增量已提交 branch `fix/gc-crossgen-region-gen-and-verify` commit `8904326a0`；A2b/B2 深项待专门）

## 当前结论（plan-v3-01）

- **已提交 `8904326a0`**：层1 GcMarkRangeOld + **系统性 region-gen 全 chunk 标记**（stress 47%→75%）+ T2 VerifyHeap
  （`gc_diagnostics` + HeapVerify 旋钮）+ T3 condemned 字段 + 文档（plan-v2/v3, review, design, handoff）。
- **A2a 负结果**：page_array_ 一致；晋升页被真释放。A2b（旧代 mark 不跟进 raw-struct message 引用）= 残余 ~25% 真根因，
  深 GC 手术，跨会话。K2a 测试 flake 为 pre-existing（改动前后都 fail）。
- **裁决原则**：方案有疑问一律选对齐 CoreCLR。


## blocking_questions

- [x] 层1 修复方式：旧代/LOH 页 `GcMarkRangeOld` 标 OLD（已落地，回归内）。`IsNurseryPointer` 变体**回滚**（破坏 K2b region-gen 契约）。
- [x] 层2 推进：确定性诊断已完成（fwd=0/unfwd=全部 + Phase-1 total=nursery=promoted），确凿指向 young-GC Phase-1 扫描覆盖缺口。
- [x] 三层优先级：P1>P2>P3；层1 纯增量无热路径代价；单参路径不变。

## question_clearance

`cleared`

## clearance_confirmed_by_user

`true`

## 下一步入口（plan-v3-01）

1. **A2b（核心残余）**：旧代 mark 跟进存活 raw-struct 对象引用（message slot→promoted），对齐 CoreCLR，深 GC 手术。
2. **B1**：GC-M1 K2c regen / M3A Server GC / M10 gen>condemned。
3. **B2**：M9 三代 → M8 plan-gen → M7 demotion；M5 BGC 分相 → M4 provisional；M3B/M6。
4. 复现器 20x 全绿 + `test_driver.py --layer unit` 无回归。

## 风险评估摘要

- 已验证增量（层1 + 系统性 region-gen + VerifyHeap + condemned）已提交 `8904326a0`，无回归。
- A2b 深 GC 手术（旧代 mark 引用跟进）风险高，需专门会话 + Verify 自检护航。
- `gc_region_test` K2a flake（GetRegionGen(nursery)!=young）为 **pre-existing**（改动前后都 fail）。
- `gc_finalizer_integration_test` 失败为 HEAD 既有，与本改动无关。

## 三优先级权衡结论

- P1：2参 barrier 非绝对 hot path，加 `IsNurseryPointer` 查询代价可接受；单参 `DirtyCard` 不变。
- P2：旧代页标 OLD + 精确 range 判定，对齐 CoreCLR region 语义，架构一致。
- P3：解释器单参路径不受影响；热更/域卸载不变。低优先级让位于 #1 高性能→保持升+精确。
