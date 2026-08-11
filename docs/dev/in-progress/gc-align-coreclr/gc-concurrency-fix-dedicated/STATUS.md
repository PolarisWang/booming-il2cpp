# STATUS — 修复 young-GC 跨代崩溃（两层根因）

## 架构映射

- 域：GC(6) + 运行时(2)。两层根因：层1=旧代 region-gen 归 0 → 2参屏障跳卡；
  层2=屏障正确后 young-GC 晋升/回写仍毒 212/thread7。
- 复现器：`tests/contracts/native/runtime-core/gc_region_barrier_stress_test.cpp`（known-failing，P1 后改善至 14/20）。
- 相关文件：见 `plan-v5-01.md`。

## phase

`executing` — **P1 主线 A 已结算**（commit `35285c431`）；剩余 M2/M3/M4 里程碑跨会话待推进

## 当前结论（plan-v5-01，落地导向）

### ✅ 已结算：P1 主线 A（commit `35285c431`，11 files +355/-8）
- **P1-A3** `GcVerifyPromotedTracked`：GC 出口核验晋升对象 fall-in-tracked 旧代页（IsInOldGen + region-gen OLD）。测试 diagnostics 4/4。
- **P1-A4** `CardBundleSet` 原子 OR（InterlockedOr8/__atomic_fetch_or），杜绝并发设置丢 bundle bit。测试 card_table_ext 7/7。
- **P1-A2b**（仓库级 bug 修复）：full-GC mark 播种时扫描注册静态根（`GcScanStaticRoots`，原从未被调用）→ ALC 静态字段 / OldMessage[] 不再被 full GC 误回收；stress 测试注册 `g_old_slot[]` 静态根。
- GC 测试全绿、构建 exit 0；`gc_diagnostics_test` 4/4、`gc_card_table_ext_test` 7/7。
- **stress 改善至 14/20**（HEAD~50% → 系统性 region-gen + static-root 修复）。残余 **young-GC 晋升** 深因（known-issue，见 memory `gc-crossgen-fix-implementation-status`）。

### ⬜ 剩余里程碑（跨会话，每里程碑独立验证+测试全绿再进）
- **M2 主线 B1**：P2-M1(GC-M1 K2c regen，foundation-dll 集成) / P2-M3A(Server GC 多堆) / P2-M10(gen>condemned，M9 后)。
- **M3 主线 B2 三代链**：M9 三代 → M8 plan-gen → M7 demotion（XL，拆 A/B）。
- **M4 主线 B2 并发**：M5 BGC 分相 → M4 provisional；M3B/M6。
- **M5 主线 B3**：M11/12/13/15（并行）；M14 ProvStress 降优。
- 完整规格/步骤/测试/判据见 `plan-v5-01.md`。

## blocking_questions

- [x] 三层优先级：P1>P2>P3；P1 正确性已结算，剩余按里程碑推进。
- [x] 裁决原则：方案有疑问选对齐 CoreCLR（CoreCLR 存在性已核实）。

## question_clearance

`cleared`

## clearance_confirmed_by_user

`true`


## phase

`executing`（已验证增量已提交；按 plan-v5 P1 推进，里程碑式）


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
