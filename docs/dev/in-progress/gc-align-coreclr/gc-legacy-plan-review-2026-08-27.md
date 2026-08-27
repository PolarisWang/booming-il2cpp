# GC 遗留问题与开发计划（整体 Review 梳理）

> 日期：2026-08-27 | 类型：整体复盘 + 验证流程重新走查 + 遗留与计划
> 依据：本会话从头重跑 GC 验证流程（实测实证）+ 既有 `gc-production-gap-assessment`（工业化三支柱）+ `roadmap-v3` STATUS + memory 已知遗留 + 源码层新实证（上一轮 demotion GcVerify / region-gen clobber 已修）
> 原则：**以实测为准**（评估文档部分已过时）；产出可直接执行的排期计划。

---

## 〇、一句话结论

CRAG GC 的**架构/功能内核真实且大体完成**（分代+Region+精确扫描+并发 BGC+两层卡表+并行压缩+热更域隔离落地），但存在 **4 个稳定测试失败 + 1 个非确定性堆破坏（GC-N7）+ 3 项 verify 工具可靠性缺口 + 大批已登记但被"规避"的 stress hang/并发遗留**。工业化可交付度约 5.0/10：短板在**验证可信度（不可复现报告）、可观测（无 perf counter/泄漏检测）、平台（仅单平台 CI）、文档审计**。

---

## 一、验证流程重新走查 — 实测基线（2026-08-27，全部实测）

### 1.1 稳定 PASS（9/13 测试套件）
| 套件 | 结果 | 备注 |
|------|------|------|
| `test_gc_demotion` | **6/6** ✅ | 本轮已修 demotion GcVerify（`0adc9c12b`）|
| `test_gc_full_gc` | 5/5 ✅ | |
| `test_gc_old_gen` | 6/6 ✅ | |
| `test_gc_bgc_smoke` | 7/7 ✅ | |
| `test_gc_bgc_unit` | 11/11 ✅ | |
| `test_gc_parallel_mark` | 6/6 ✅ | |
| `test_gc_bgc_root_scan` | **4/4 ✅ 0.22s** | **评估说的 ~392s 挂起已闭环**（`3d11cca10` RootChangeBufferWraparound）；评估文档此条已过时 |
| `test_gc_worker_pool` | 7/7 ✅ | 偶发 flake（ReusePool），重跑绿 |

### 1.2 稳定 FAIL（4 项，需专项）
| 套件 | 失败测试 | 现象 | 已知/状态 |
|------|---------|------|----------|
| `test_gc_young_collector` | `ConservativeSweepSelfRefs` | 断言 `b_first_word & 1 != 0`（L294）或偶发 SEH | **clean-HEAD 预存在**（`b3b0231f0` worktree 实测），Phase 0 vs Phase 2 晋升时序，非本批引入 |
| `test_gc_gen1` | `SingleLiveObject` | `r.bytes_promoted >= 64 == 0` | **预存在**：gen1_bump 跨 TEST_F 泄漏（Tier-1 early-exit 被 bypass，memory `contracts-native` 已记） |
| `test_gc_max_promote` | `MultipleYoungGcsWithTimeout` | **SEH 0xC0000005**（3 次循环 + 全 GC） | memory 记"timing 敏感 flake"，但当前树**稳定崩**。与 young_GC timeout + card 保持一致相关，需定位 |
| `test_gc_stress` | ScenarioS_3 项（长时） | 后台 120s **3/3 PASSED**（Light/100Threads/Gen1Pressure） | 现存 stress 冒烟绿；评估强调的"A-K 预存 hang"在 `--new-only` 规避下未暴露（需去规避验证）|

### 1.3 verify 工具（`CHAOS_GC_HeapVerify=2` kFull）可靠性缺口
| 项 | 现状 | 状态 |
|----|------|------|
| demotion GcVerify 误判 | in-place demoted 被当 OLD 校验 → 刷 ERROR | **已修**（`0adc9c12b`）|
| 跨池 region-gen clobber | old-gen/LOH 页共享 4MB chunk vs nursery gen0 | **已分类良性**（`049d6e65c` clobber-aware）|
| bitmap-poison 假阳性 | poison 仅 debug 写，非 debug 恒读 0 | **已修**（`049d6e65c` DEBUG 门控）|
| **A2b 未类型对象 first-word** | ~4k `first-word not valid TypeInfo`，**非 interior 噪声**（连续 8 槽全无合法 head TypeInfo） | **遗留**：raw/untyped marked old-gen 对象，需定性（测试伪影 vs 生产合法 vs 真 bug）|

---

## 二、源码/roadmap 层已登记录但未闭环的遗留（roadmap-v3 STATUS + memory）

| 项 | 状态 | 阻塞 |
|----|------|------|
| **GC-N7** `YoungGcPauseUnderLoad` | **非确定性堆破坏**：`GcYoungCollection:537` AV + teardown `c0000374`，同代码 8%~73% 频率，A/B/C revert | 需**真机 page-heap**（Release 基准前置，阻塞 GC-N12）|
| **GC-N6** gen1↔old-gen 重叠 | 屏障 `ef0012d49` 已修一缺陷；残余重叠专项 | |
| GC-N2 | BGC root-scan 挂起专项 | 由并行 GC 调试线承接（实测 bgc_root_scan 已 4/4 绿，可能已闭环）|
| GC-N12 | profile 驱动调参 | 依赖 GC-N7 稳定 Release 基准 |
| Server GC（GC-N9 Dynamic Heap Count） | `a77aff4dd` 已提交，runtime 待 GC-N3 harness | Server 默认 OFF，无 CI |
| BGC parallel mark `yield()` Livelock | mark-hang 全局死锁根因 + WCT 盲区实证 | `mark-hang-wct-handoff-2026-08-27.md`，已 cv 修复/降并行止血 |

---

## 三、工业化三支柱缺口（`gc-production-gap-assessment` 6.0 版，5.0/10）

| 支柱 | 现状 | 差距 |
|------|------|------|
| **可靠性验证可信度**（40%） | stress 参数与文档不符、并发 GC 交叠被禁用、报告流向外项目、native 证据缺失 | 需可复现 native `test-report.json` + CI 基线 |
| **可观测/运维/兜底**（3.0） | 仅进程内计数 + Windows ETW；**无 perf counter、无泄漏检测、GCInfo 空壳** | 需多平台可观测 + 告警 + 泄漏钩子 |
| **文档审计**（2.5） | 40+ 项 100% 自评、无独立 STATUS/sign-off、文档与源码漂移 | 需独立 STATUS + 评审 + 反向同步 |
| **平台完整性** | Thread Hijack 仅 x64；Server GC OFF；CI 仅 windows-x64-reference | 需 iOS/ARM64 兜底 + 多平台 CI |

---

## 四、开发计划（排期，含依赖与并行轨道边界）

> 原则：**先可信度（证明基线存在）→ 再修复（消灭稳定失败/已知挂起）→ 再可观测 → 再平台/文档**。撞并行轨道（gc_old_gen/gc_parallel_mark/gc_worker_pool 未提交改动）的任务需定界。

### P0 — 修复稳定失败 + 可信度地基（1-2 周）
1. **`test_gc_max_promote.MultipleYoungGcsWithTimeout` SEH**（`.cpp` + young_collector 定位）
   - 系统调试：多次 young GC timeout + card 保持一致路径，3 次循环崩因。可能与 `timed_out` 后 `ClearCardRange` skip 的跨 cycle 交互。
   - 交付：稳定绿 + 根因 message。
2. **`test_gc_gen1.SingleLiveObject`**（`gc_gen1_test.cpp` + gen1_bump reset）
   - 根因（memory 已记）：gen1_bump 跨 TEST_F 泄漏 bypases Tier-1 early-exit。修 = 测试间重置 gen1_bump 或 collection 启动时校准。
   - 交付：14/14 绿。
3. **`test_gc_young_collector.ConservativeSweepSelfRefs`**（clean-HEAD 预存在）
   - Phase 0 栈根 vs Phase 2 精确扫描对 objB 晋升时序。独立专项，勿与本批其他项耦合。
   - 交付：断言对齐 Phase 0 真实语义 或 明确测试意图。
4. **A2b 未类型对象 first-word 定性**（verify 工具）
   - 区分测试伪影 / 生产合法 raw 分配 / 真 bug；按类修 verify 或测试。
   - 交付：kFull 下 `test_gc_demotion` 无 ERROR（仅 benign WARN）。

### P0 — 可信度闭环
5. **GC stress/stress-matrix 纳入 CI + 报告落库**（`test_driver --layer native` + `gc-stress` preset）
   - 产出可复现 native `test-report.json`；修复 stress 被削弱（场景参数对齐、恢复 Scenario G、去 `--new-only` 规避）。
   - 交付：native 证据 + 基线 + 门禁必红能力。

### P1 — 修复已知并发/堆破坏
6. **GC-N7 `YoungGcPauseUnderLoad` 堆破坏**（真机 page-heap）
   - 需真机环境（page-heap）复现 `GcYoungCollection:537` AV / teardown `c0000374`。阻塞 GC-N12 基准。
   - 交付：根因 + 修复 + Release 稳定基准。
7. **GC-N6 gen1↔old-gen 重叠残余**
8. **BGC mark-hang 降并行方案收敛**（mark-hang-wct-handoff）

### P1 — 可观测 + 平台
9. **GC perf counter / 泄漏检测 / GCMemoryInfo 真实值**
10. **Server GC 默认开启 + 多堆 CI**；**iOS/ARM64 Thread-Hijack 兜底**

### P2 — 文档治理
11. GC 独立 STATUS.md + 评审/sign-off + 修 24-CRAG 文档与源码漂移。

---

## 五、并行轨道边界（本次 review 实测确认）

- 并行 GC 调试线当前修改：`gc_old_gen.cpp`（S2 mark-watchdog + DIVERGE detector）、`gc_parallel_mark.cpp`、`gc_worker_pool.cpp`（均未提交）。
- 本批 P0 修复 1-4 涉及 `gc_young_collector.cpp`、`gc_gen1.cpp`、`gc_diagnostics.cpp`、测试 `.cpp`——**不与并行轨道文件冲突**（除 `gc_old_gen.cpp` 的 gen1/teardown 若涉，需定界）。
- 本会话已提交：`2a84b6695/8cb5a945d`（young empty）、`0adc9c12b`（demotion verify）、`049d6e65c`（region-gen benign + poison gate）。

## 六、证据/交接索引
- 本 review 手写：`docs/dev/in-progress/gc-align-coreclr/notes/gc-verify-demotion-fix-and-region-gen-caveat-2026-08-27.md`
- 工业化评估：`docs/archive/assessments/repo-cleanliness-2026-08-27/gc-production-gap-assessment.md` + `gc-reliability-options-analysis.md`
- roadmap STATUS：`docs/dev/in-progress/gc-align-coreclr/STATUS.md`
- memory：`gc-pre-existing-issue-fixes-batch`、`gcverify-demotion-and-regiongen-clobber-resolution`、`contracts-native-codegen-symbol-gap`(gen1 deep-dive)、`youngcollectionempty-fixed-conservativesweep-preexisting`
