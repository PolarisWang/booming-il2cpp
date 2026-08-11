# plan-v1-01 — 落地 GC vs CoreCLR 差距（7 项全量）

> 域：GC(6) + JIT(4)。来源：`review-vs-coreclr-2026-08-11.md` 的 4 域调研差距。
> 目标：把 review 发现的 7 项全部落地为可执行、可验证的子任务，按正确性>性能>热更排序，依赖串行。
> arch_review_mode: `critical`（触及 AOT/IL2CPP 写屏障 + 测试主线）。

---

## 0. 输入与权威

- 设计文档：`docs/dev/in-progress/gc-align-coreclr/gc-concurrency-fix-dedicated/review-vs-coreclr-2026-08-11.md`（本计划唯一 spec）
- 问题清零来源：`design-v1-01.md` + `STATUS.md`（question_clearance: cleared / clearance_confirmed_by_user: true）
- 计划文档：本文件
- 知识沉淀默认：`n/a`（各子任务 STATUS 沉淀）
- 权威 CoreCLR 参考：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`（只读对照）

## 1. 任务清单（依赖串行，按序执行）

| # | 子任务 | 要点 | 文件 | 依赖 |
|---|--------|------|------|------|
| T1 | 修层2 晋升页簿记缺口 | `G_OldGen().Allocate` 产物页进 page_array_ + GcMarkRangeOld | gc_old_gen.cpp/.h, gc_young_collector.cpp | — |
| T2 | 加 verify_heap + verify_region_to_generation_map | 自检 + poison + 配置分级 | gc_diagnostics.*/.h, gc_config.h, gc_api.cpp | T1(可为T1回归网) |
| T3 | gen>condemned 卡过滤 | 卡扫描只扫 condemned 及更老代 | gc_young_collector.cpp, gc_old_gen.cpp, gc_card_table.h | T1 |
| T4 | BGC 两快照/分相 | final_marking 第二遍 + foreground 交错 | gc_bgc.cpp/.h | T2(自检护航) |
| T5 | dynamic_tuning 伺服闭环 | frag/surv → gen 预算缩放 | gc_scheduler.cpp/.h, gc_young_collector.cpp | T1 |
| T6 | HandleTable 子系统聚合 | 集中 strong/weak/dependent + per-thread 表 | 新 gc_handle_table.* + 迁移现有句柄分散点 | T1 |
| T7 | software write-watch + 配置旋钮速记 | bump card-bundle 重建 + 关键配置对齐 | gc_card_table.cpp/.h, gc_config.h | T3 |

## 2. 各子任务详细规格

### T1 — 修层2 晋升页簿记缺口（正确性 open bug，最高优先）
- **目标**：让 `gc_region_barrier_stress_test` 从 known-failing → 10x/15x 0 失败。
- **已定位根因**（本会话）：dangling ref = 晋升产物地址 `region_gen=0, IsOoG=0, FindPage=null`，高于 nursery
  used-end → `G_OldGen().Allocate` 晋升产物页**未进 `page_array_` / 未 `GcMarkRangeOld`**。
- **待钉死 one-line**：在 `G_OldGen().Allocate` 返回前，对照 `page_array_`（`RebuildPageArray` 是否被晋升路径
  的 free-list/new-page 分配调用）与 `GcMarkRangeOld`（该页 4MB 字节是否 OLD）。若晋升经 free-list 复用旧页而
  page_array_ 未刷新，补 `RebuildPageArray`；若新页未 `GcMarkRangeOld`，补标 OLD。
- **验收**：复现器 `ctest -R region_barrier -L stress` 10x 0 失败 + `test_driver.py --layer unit` 无回归；
  `DIAG-DANGLE`（ref region_gen=0 且 IsNursery=0 且 IsOoG=0）计数归零。
- **风险**：晋升路径改动影响旧代分配，需全量旧代单测护航。

### T2 — 加 verify_heap + verify_region_to_generation_map（工程健壮，承接层1验证缺口）
- **目标**：GC 入口/出口自检 + 区代图一致性校验，防簿记漂移回归（T1 这类）。
- **落点**：新增 `gc_diagnostics.cpp/.h`；`VerifyHeap()`（入口/出口调用，可校验被引用对象至少被 mark、
  poison 填充、region→gen 映射与 region.gen 一致）；`gf_region_to_generation` 校验复用 `verify_region_to_generation_map`
  思路（`diagnostics.cpp:264`）。配置分级：`HeapVerify` 旋钮加进 `CHAOS_GC_CONFIGURATION_KEYS`（0/1/2 级）。
- **验收**：`gc_diagnostics_test` 覆盖：正确时无断言、人为破坏 region-gen 时触发校验失败。
- **风险**：校验开销，默认关（level 0），仅 CI/debug 开。

### T3 — gen>condemned 卡过滤（对齐 CoreCLR mark_phase.cpp:1393）
- **目标**：young-GC/Gen1 卡扫描只作用于 condemned 及更老代，跳过年轻代对象的冗余 mark，缩小扫描范围 + 正确语义。
- **落点**：`GcScavengeObject`/Phase-1 回调加 `condemned_gen`；`get_region_gen(o) > condemned` 时跳过。
  需先引入"condemned gen"概念（本仓库 2 代 young/old：young collection 时 condemned=young）。
- **验收**：`gc_card_table_ext_test` + young_collector 无回归；`dirty_cards_scanned` 数下降（性能）。
- **风险**：语义变化影响跨代边，需 T1 修复后基线上做，避免与 bug 交织。

### T4 — BGC 两快照/分相（降延迟，P1 性能）
- **目标**：补齐 CoreCLR 两快照——并发第一遍 mark + STW `final_marking` 第二遍 + foreground mini-GC 交错。
- **落点**：`gc_bgc.cpp/.h` 现有 IDLE/concurrent mark/sweep/compact 扩展 `final_marking`/`revisit` 状态；
  `current_c_gc_state` 协调 foreground 交错（对齐 background.cpp）。
- **验收**：现有 `gc_bgc_*` stress（bgc_smoke/race/root_scan）全过 + 并发下无漏（BGC 复扫）。
- **风险**：高改动面，需 T2 自检护航；若当前目标非降延迟，标记为 `deferred`（见 §3 开关）。

### T5 — dynamic_tuning 伺服闭环（P1 性能）
- **目标**：把 `gc_scheduler` 已记录的 surv_ratio/frag 闭环到 gen 预算（对齐 `desired_new_allocation`：
  碎片高降 gen2 预算、gen0 空列表率控分配、memory load 缩 gen0）。
- **落点**：`gc_scheduler.cpp/.h` 增 `DesiredNewAllocation()`；`NurseryAllocateSlow`/`UpdateTlabSize` 消费。
- **验收**：`gc_scheduler_test` 增伺服单测（碎片/surv 变化 → 预算变化）+ 现有无回归。
- **风险**：改分配决策影响吞吐，需 benchmark 对比。

### T6 — HandleTable 子系统聚合（P2 架构）
- **目标**：把分散在 gc_bgc/gc_old_gen/reflection 的句柄逻辑聚合成独立 `gc_handle_table.*`，
  明确 strong/pinning/weak/dependent 分类 + per-thread 表 + aging。
- **落点**：新 `gc_handle_table.*`；迁移 `GcAllocHandle` 等现用法；`GCScan` 句柄扫描走子系统。
- **验收**：现有依赖句柄/弱引相关测试全过（gc_bgc/gc_gen1/old_gen 涉及）。
- **风险**：迁移改动面大，但行为等价；热更/域卸载受益。

### T7 — software write-watch + 配置旋钮（低优 backlog）
- **目标**：卡 bundle 重建闭环（CoreCLR `update_card_table_bundle`）+ 关键配置对齐。
- **落点**：`gc_card_table.cpp` 增 card-bundle 重建；`gc_config.h` 增 `EnableWriteWatch` 等关键旋钮。
- **验收**：card_table_ext 覆盖 bundle 重建；无回归。
- **风险**：低；AOT 场景才关键。

## 3. 执行策略与阶段

- **串行主链**：T1 → T3 → T5 必须是纯正确性链（先修 bug 再优化语义），T2 可并行预置（自检护航 T1）。
- **P1 权衡**：按全局 P1 性能最优，T4/T5 在降延迟/吞吐目标下推进；若当前阶段不以降延迟为 KPI，
  T4 可 `deferred`（见 STATUS 记录），T5 作为吞吐优化保留。
- **每子任务收尾**：结构告警审视 → 测试通过（`test_driver.py --layer unit`）→ 域 STATUS 记 `completed` →
  合并提交（commit 含 root_cause/fix_strategy/regression_check，T1 必须）。

## 4. 关键验证命令

```bash
# 层2正确性（T1/T3）
ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "region_barrier|gc_old_gen|gc_young_collector" -L stress --output-on-failure
python tests/runner/test_driver.py --layer unit   # 全量 unit，无非 known FAIL
# 自检（T2）
ctest -R "gc_diagnostics"
# 并发（T4）
ctest -R "bgc_" -L stress --timeout 600
```

## 5. STATUS/ACTIVE/notes 维护

- `STATUS.md` 主记录每子任务 `phase`, `blocking_questions:[]`, `question_clearance:cleared`,
  `clearance_confirmed_by_user:true`, `arch_review_mode:critical`；`docs/dev/ACTIVE.md` 仅活动指针。
- `notes/progress-*.md` 仅在阻塞/重大决策/验证失败时追加；T1/T4 若遇 BGC/晋升根因横跨≥2域 → 走 workflow/deep-dive
  信号（CLAUDE.md 深度调试触发）。

## 6. 结构告警关注点

- T6 迁移句柄时防 `parallel helper`：统一进 `gc_handle_table.*`，不复制。
- T3 `condemned` 概念避免与 `kRegionGenOld=2` 混淆，命名 `kCondemnedYoung`/`CondemnedGenLevel`。
- T5 伺服逻辑与 `gc_scheduler` 既有 EMA 分开，不混入决策路径的分配热点。
- 所有改动遵守 native 编码规范 v1（命名/注释/内存宏 CHAOS_IL2CPP_*）。
