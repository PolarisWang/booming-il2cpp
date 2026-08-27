# 补全 CRAG GC "可靠性验证可信度" — 方案与深度分析

> 目标：把评估中"可靠性验证闭环约 40%、证据不可复现、stress 注水、断言软化、已知挂起被隔离规避"这一最短支柱补到工业化水准。
> 前置事实（已核实）：现有 test 基础设施已具备 —— 统一驱动 `test_driver.py`、no-skip + `known-failures.{layer}.yaml` baseline、生成本地 `test-report.json`（开 `--cases` 有 per-case 明细）、native 层已含 `--stress-only` 通道（`CHAOS_GC_TEST_STRESS_ONLY=1` 切换 `-L`/`-LE`）。缺口不在机制，而在**没跑、没接 CI、场景注水、断言软化、基线缺位**。

---

## 〇、根因（不是"缺测试"，而是"验证链断在五处"）

1. **证据不落库**：native/GC 层从未跑进 `test-report.json`（现报告只含 unit/dotnet），无可复现 native 产物。
2. **压力层未门禁**：`-LE benchmark|stress|soak` 把压力测试排除在 CI 外，`--stress-only` 仅手动、无 nightly/常驻门禁。
3. **场景与文档背离**：Scenario F 声称 100 线程实为 20×20；Scenario G 并发 GC 线程被 `DISABLED`；`--new-only` 规避 A-K 预存 hang。
4. **断言软化**：BGC-YoungGC 检测到并发就 `ForceComplete` 跳过宣称 PASS；POH 避免直接收集；finalizer/弱引用是"不崩溃"级软断言。
5. **报告路径外流**：压测转储硬编码到 `D:/agent/booming-il2cpp/`（他仓），本仓库无法复现。

任何方案都必须先解决"**证明基线存在且稳定**"，否则后面都是空中楼阁。

---

## 方案 A：建立可复现的 native/GC 门禁基线（据实达标基线）
> 定位：最底层的"可信度地基"，修复证据不落库 + 已知挂起不闭环。

**A1 把 native 层跑进 driver 并落库**
- 在 `tests/suite_contract.yaml` 的 `contracts-native` 组上补一个 native GC 专用组（或用现有组）跑一次完整 ctest，用 `--cases` 产出 per-case 明细，把 `test-report.json` 升级为含 native 层。
- 把 `chaos_gc_bgc_root_scan_test`（~392s 挂起）**修到真正通过**（这是当前唯一已登记 native GC 已知失败），或至少拆成可收敛的分片并给出根因。**不能靠 baseline 永久压下**——不修复则门禁永远无法全绿。
- 报告路径从其他项目改回 `tests/runner` / `artifacts/<preset>/reports`（消除跨项目污染）。

**A2 建立"据实达标基线"而非"声明基线"**
- 以首次全绿运行的实际值（各 GC 场景的真实吞吐、Young/Full/BGC 暂停 P50/P95/P99、线程数、时延）固化低线，存入 `tests/runner/baselines/gc.perf.yaml`。
- 用 `--cases` 的 per-case duration 做**回归 diff**：新提交若某场景时间跳变 > X%，或失败数突破基线，门禁红。

**深度分析**
- **价值**：直接消灭"证据不可复现"，且把"全部 PASS"变成可运行可回归的事实。是所有后续方案的先决条件。
- **成本**：中（修 1 个已知挂起 + 接线测试组 + 固化基线）；不依赖新硬件。
- **风险**：低。唯一风险是"修 1 个挂起"可能牵出深层并发 bug——但这正是该做的。
- **关键支撑证据**：`test_driver.py` no-skip 门判、`suite_contract.yaml` ctest_* 契约字段、`native.py` 的 `-LE`/`-L` 切换、`known-failures.integration.yaml` 已登记的 `chaos_gc_bgc_root_scan_test`。
- **局限**：只证明"能跑通、没崩溃"，不证明"在真实压力下正确"——需 B/C/D 配合。

---

## 方案 B：修复并强化压力层，使声明与实现一致
> 定位：消除"注水"与"规避"，让 stress 真正做压力。

**B1 对齐场景声明**
- Scenario F：参数与文档要么改成真实值（如扩到接近 24 处理器 × 高线程数），要么把文档改回"20×20"。**偏好改代码扩测试**，因为 100 线程×100MB 是正确方向的验收目标；但要有硬件语义（用 `std::thread::hardware_concurrency` 动态取核数，而非硬编码）。
- Scenario G：**恢复被注掉的并发 GC 线程**（去掉 `DISABLED FOR DIAGNOSTIC`），让"与 full GC 交错"是真的。
- `--new-only`：把 A-K 预存 hang 的**根因修掉**，而不是靠 new-only 规避。逐场景 A-K 补 `RunScenarioX` 收敛性断言。
- 删硬编码到他仓的报告路径。

**B2 修复"规避"式断言**
- BGC-YoungGC：`gc_bgc_smoke.cpp` Test3 去"ForceComplete 跳过"，改成**真实 interleaved 压力**（让 BGC 并发标记期间持续触发 young GC + nursery evacuation），去掉 `KNOWN ARCHITECTURAL LIMITATION` 的 `CHECK(true,...)` 软断言。
- POH：`gc_poh_test.cpp` Test8 去掉"避免直接 Collect"的规避，落到可安全执行的路径（POH 页纳入 GC 可收集的路径），并加地址稳定性硬断言。
- finalizer/弱引用：把"不崩溃"级 `CHECK(true,...)` 升级为**可观测语义断言**（验证 finalizer 确实被调用、弱引用确实被置空），必要时引入 mock 或可观测回调。

**B3 接入夜间 stress 门禁**
- 新增 nightly（或每周）workflow，把 `contracts-native` 以 `stress-only` 模式（`-L stress|soak|benchmark`）跑全量 GC 压力（A-P 全场景、多轮、长时间），产物落库（`artifacts/gc-stress-reports/<date>.json`）。
- 在 `scripts/ci_test.py` 增加 `--preset gc-stress`，复用 native adapter 的 stress 通道。

**深度分析**
- **价值**：把"声明 100% 但参数不符/并发被禁/软断言"的注水全部洗掉；这是诚实可信度的核心。
- **成本**：高（每项都要改既有测试、部分要动运行时查找根因：A-K hang、POH 收集、YoungGC/BGC 交叠的 segfault）。这是评估里"已知问题以规避取代修复"的本质，躲不开。
- **风险**：中-高。恢复并发压力可能暴露真 UAF/死锁（这正是目的）。要有长时间跑的时间预算。
- **关键支撑证据**：`gc_stress_test.cpp:991-992,1165-1181`、`gc_bgc_smoke.cpp:191,238,416`、`gc_poh_test.cpp` Test8、`gc_handle_test.cpp` 软断言、`gc_finalizer_test.cpp` "verified no crash"。
- **局限**：自写 stress 仍是自己证自己，无外部 oracle——用 C 配合交叉验证。

---

## 方案 C：建立"独立性 / 交叉验证"以打破自证
> 定位：解决"stress 自证 + 无审计痕迹"，引入外部可信参照。

**C1 语义正确性交叉验证**
- 用已知不变的**语义测试**（如 `gc_handle`/`DependentHandle`/Ephemeron/弱引用在回收后行为）到**内存语义等价**的程度：在同样输入下，与 .NET/CoreCLR 语义对齐的断言作为 oracle。
- BGC-YoungGC、POH、finalizer 的断言从"不崩溃"升级为"行为正确"（C2 支撑）。

**C2 对拍 / 参照实现**
- 对 GC 语义的子集（如 finalize 顺序、WeakReference 复活、GCHandle pin 稳定性、Ephemeron 键值对 GC 敏感性）用**熟知的参考实现语义**写基准断言，或与 CoreCLR 行为对照记录差异并逐个判定"设计差异 or 缺陷"。
- 产出"与 .NET 语义兼容性矩阵"，把兼容项固化为测试。

**C3 第三方式验收包**
- 仿照黑盒 QA：定义一份"**GC 验收清单**"（分代正确、晋升年龄、跨代引用不漏扫、并发期内引用变化、OOM 兜底、域卸载回收、pin 稳定性、低内存响应），每条有可执行脚本 + 判定口径 + 通过基线，作为可独立/追审的验收产物（对文档"无 sign-off"的补充）。

**深度分析**
- **价值**：把"self-claim"升级为"可独立验收、可追审"，补齐文档审计短板；C2/C3 的兼容矩阵也是工业交付的证据。
- **成本**：中高。需要编写语义 oracle 与兼容矩阵；部分依赖分析 .NET 语义，属知识密集。
- **风险**：低-中。主要为"语义判定口径"争议，需明确 owner 与判定规则。
- **关键支撑证据**：评估中"无独立 STATUS/评审/sign-off、单篇自评"、"GCMemoryInfo 托管缺失、.NET 语义断链"。
- **局限**：解决"可信 + 可审计"，但绕不开"要先有能正确跑的 GC"——前置 A/B。

---

## 方案 D：真实负载 + 可观测基线验收
> 定位：从"合成 stress"升级到"接近生产负载 + 可观测"。

**D1 生产负载压测**
- 建一个贴近真实 Play game 负载的 GC 冒烟/压测（高频分配 + 跨代引用 + 周期大对象 + 域加载/卸载 + 类型装载），而非纯 alloc/release 压力。
- 跑多种内存配置（不同 nursery 大小、是否开 BGC、是否 Server GC、堆硬/软上限）矩阵，验证负载行为。

**D2 可观测断言**
- 接入现有 `GcStats`/暂停直方图/事件 ring buffer，在测试末尾断言**低内存告警、堆硬/软上限命中、OOM 兜底（emergency reserve 生效）能被观测到**且行为符合预期。
- 补 perf 基线：Young/Full/BGC 暂停 P50/P95/P99 与吞吐落库（`gc.perf.yaml`），与 A2 呼应。

**深度分析**
- **价值**：把"暂停时间、内存上限、OOM 兜底"这些工业化指标变成**可验证事实**而非文档断言；也直接驱动 GCMemoryInfo/GCInfo 空壳字段的补齐。
- **成本**：高（需要定义生产负载 fixture + 指标采集 + 基线判定）。
- **风险**：中。真实负载的"代表性"与"可复现"需要投入打磨；指标基线在 CI 机抖动下需容差设计。
- **关键支撑证据**：`gc_low_mem.cpp`、`gc_old_gen.h` emergency reserve、`gc_stats.h` always-0 字段、`GC_HEAP_*_LIMIT_MB`。
- **局限**：最接近"工业化"但投入最大；宜作为 B/C 稳定后的进阶，不宜作为第一步。

---

## 推荐落地路径（分阶段组合，把可信度 40% → 85%）

| 阶段 | 方案 | 交付/里程碑 | 可信度 |
|-----|------|-----------|-------|
| **P0（1-2 周）** | A1+A2 | native/GC 层进 `test-report.json`（可复现）、修 `bgc_root_scan_test` 挂起、报告路径回本仓、固化 GC 暂停基线 | 40%→60% |
| **P1（2-4 周）** | B1+B2 | 场景参数对齐、恢复 Scenario G 并发、去 new-only、修 A-K hang、去"规避式"软断言（BGC-YoungGC/POH/finalizer/弱引用） | 60%→72% |
| **P1（并行）** | B3 | nightly stress 门禁（`--preset gc-stress`）+ 报告落库 | 72%→75% |
| **P2（4-6 周）** | C1+C2+C3 | .NET 语义兼容矩阵 + 可追审验收清单（文档 sign-off 起点） | 75%→80% |
| **P3（6-10 周）** | D1+D2 | 生产负载矩阵 + 可观测（低内存/OOM/堆上限）断言 + perf P95 基线 + 补 GCMemoryInfo | 80%→85% |

**优先级原则**：先证明"能稳定跑且证据可复现"（A）→ 再证明"压力是真的、断言是硬的"（B）→ 再引入"独立性/可审计"（C）→ 最后"贴近生产负载与可观测指标"（D）。**不要跳过 A 直接做 B/D** —— 没有可落库的基线，后续再多的压力测试仍是无证据的自证。

---

## 关键权衡（决策点）
1. **"修已知挂起" vs "baseline 暂时压住"**：推荐**必须修**（A1）。用 baseline 永久压住会让门禁永远无法全绿，等于认可缺陷；与评估的"规避而非断言修复"同病。
2. **Scenario F 参数"扩核数" vs "改文档"**：推荐**扩到动态硬件并发**（`hardware_concurrency`）+ 补能通过的高并发验收，而非把文档调回低级值——目标应是逼近工业负载，而非迎合当前单元级规模。
3. **BGC-YoungGC 交叠**：恢复真交错会暴露 segfault 根因（尽管 `gc_bgc.h` 已有 G-3 `PauseForYoungGc` 协议）。这是一项真实正确性修复，必须做，不可再靠 ForceComplete 规避。
4. **夜间 vs PR 门禁**：stress 建议**夜间 + on-demand**（`--preset gc-stress`），PR 上只跑非 stress 的快速 GC 组（A 阶段建立的那个）；避免 PR 被数小时压力拖垮，同时保证长期有人在跑。
5. **成本预算**：若按"一步到位全方案"，约需 6-10 周工程投入 + 必要时 1-2 台多核压测机；若按"最快见效"，仅 P0（约 1-2 周）即可把可信度从 40% 提到 60% 并产出第一份可复现的 GC 报告。

---

> 一句话建议：**先做方案 A（把 native 证据落库 + 修挂起 + 固化基线），一个月内把"可靠性验证闭环"从 40% 拉到 60% 并拿到第一份可复现 GC 报告；随后以 B→C→D 顺序扩展，工业化可信度可到 85%。**
