# A2b 跨代引用 UAF — known-hard 归档（2026-08-13）

> 状态：**known-hard / 归档**。不关闭，等待 hard-STW 立项（对齐 CoreCLR 唯一根治路径）。
> 复现：`gc_region_barrier_stress_test`（8 thread × 128 objs, 8 GC cycles），4-5/10 失败，dangling 32-235/1024 恒 thread-7。
> 三规则：本专项已耗尽 3+ 次修复机会，全部 revert/收窄不根治，按规则停线归档。

---

## 一句话

mutator `*old_slot = nursery_ptr`（store）与 `dirty_card(old_slot)`（barrier）是**非原子两步**；GC safepoint 落在两者之间 → young-GC Phase-1 扫该页时卡仍干净 → 仅从此槽可达的 nursery 对象被收集 → dangling。**根治只能 hard-STW（真 SuspendThread），使线程不可能停在 store 与 card 之间。**

## 已尝试并失败的修复（勿重复）

| 尝试 | 结果 | 结论 |
|------|------|------|
| Option B 保守 old-mark（TryMarkRootConservative） | 随机 32-307 未降 | old-mark 不是根因（被杀的是 young 目标，非 old 页）|
| P-1 naive 全槽重扫 | 回归 1023/1024 污染 | 全槽无法区分布局，已 revert |
| γ' barrier_inflight 原子段 + safepoint wait | **收窄 32→95-117，从未消除** | 收窄但 C++ 无法做到"store 后不可被 SuspendThread 中断" |
| 卡侧三次结构修复（rebase 重索引 / base floor-ceil / 物理地址槽） | 逐字节相同 dangling | 卡读侧索引自洽，loss 不在卡索引 |
| **A' 全套（2026-08-13 本会话）**：Part R 协调器等 barrier_inflight + Part E-NATIVE 写站点 scope + 改 stress worker | **74-117 原样，未修** | 独立复现 γ' 失败史，坐实 A' 非根治 |
| FULL mark 保留无-TypeInfo OldMessage（C1-C5） | 修 FULL 塌缩（真 bug）但不降 dangling | FULL 是次要 bug，"necessary not sufficient" |

## 决定性诊断（DIAG-4, 2026-08-12）

```
barrier 决策: carded=1024  dst_young=1024  refge_dst_skip=0   ← 写侧判定正确
Phase-1 扫描: dirty_cards=8  nursery_refs_seen=84            ← 扫到 8 卡/84 ref
```
barrier 写 1024 卡，Phase-1 只读 8 → 4 个 old-gen 页的卡**写读断开**。但卡侧修复 no-op → loss 在 Phase-1 之外/young 晋升或 store-barrier 竞态。最终（test 自述 + 全域收敛）锁死 = store-then-barrier 竞态。

## 为什么根治只能 hard-STW

- CoreCLR 写屏障是真 assembly **LEAF**（无 poll），线程 store+card 一体不可挂起。
- CRAG 无法用 C++ `barrier_inflight` + 协调器等待模拟"不可挂起"：协作式 poll + APC park（`PalCaptureReliable()==false`，`pal_preempt_win32.cpp:52`）在线程处于 store 与 card 之间时仍可被 park。
- **唯一能保证"线程不处于 store→card 中间态"的是真 `SuspendThread` 停所有线程到一致点**（CoreCLR/WKS 的 STW）。

## 正确根治方向（单独立项，勿在本归档续）

1. **hard-STW（最优）**：Linux 优先（POSIX SIGUSR2 ucontext `PalCaptureReliable()==true`，只需 SuspendThread 统一一致点）。跨平台改造 `pal_preempt` + safepoint。这是对齐 CoreCLR 的唯一根治路径。
2. **验证可见性**（归档期间）：
   - `gc_region_barrier_stress_test` 已挂 `stress` label 被 `-LE` 排除在 CI 门外 → **提为 nightly 必跑的已知失败**，让红灯可见，不静默。
   - 不写死"已修复"。

## 已保留的真实加固（非 A' 修 stress，但正确关闭一缺陷类）

本轮实现了 CoreCLR 对齐的防御性加固，**编译通过、无回归**（`chaos_gc_young_collector_test` PASS），即使不修 A2b 也值得保留：
- **Part P**（`chaos_is_gc_pointer` nursery 盲点）：镜像 DirtyCard nursery 检查，below-base nursery 指针不再被误判为栈地址 → 不丢卡。
- **Part R+E-NATIVE**：`BarrierCriticalSectionScope`（store→card 原子段，`barrier_inflight` 跨线程发布）+ 协调器 Phase-1 前等 `barrier_inflight==0` + 三套写站点（jit_helpers / interpreter / fast_dispatch）包 store+card。消除一个真实竞态缺陷类（即便非 A2b 全因）。
- `chaos_barrier_enter/exit` extern "C" bridge 供 E-MANAGED（generated code）发射。

> ⚠️ 这些加固**不宣称修复 A2b**（实测未动 stress）。仅作为正确性加固与 hard-STW 的中间态/前置。

## 文档
- 根因史：memory `gc-crossgen-stress-true-rootcause` + `plan-v6/investigations.md`
- A' 方案设计：`docs/dev/assessments/gc-fix-proposals-1-2-coreclr-deepdive-2026-08-13.md`
