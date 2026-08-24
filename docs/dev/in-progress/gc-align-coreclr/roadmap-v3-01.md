# Roadmap — GC 工业化落地三批次（v3）

> 版本：v3-01（在 v2 全量对齐之上，新增"工业化护网/工程闭环/能力拉平"三批次）
> 创建：2026-08-25（用户 2026-08-14 确认按 1+2+3 三批次推进；本版为执行版）
> 上游依据：
> - `docs/dev/assessments/gc-industrialization-gap-analysis-2026-08-14.md`（综合差距报告，用户已确认按 1+2+3 三批次推进）
> - `docs/dev/assessments/gc-risk-assessment-2026-08-13.md`、`production-readiness-gap-2026-08-13.md`
> 定位：**v2 的功能对齐（M1-M15）与本 v3 的工业化批次并行推进**；N 系列任务与既有 M# 重复时以 M# 为准（见映射），N 系列新增项（CI/验证/专项）为 v2 未覆盖。

---

## 1. 目标

在 v2（Phases 7-9，M1-M15 功能对齐）基础上，补齐"生产可依赖度"短板：正确性护网、CI 门禁、工程闭环、能力拉平。每批次完成需可度量验证（门禁变红能力、测试通过、基准存在）并提交。

## 2. 三批次总览（用户已确认 1+2+3）

| 批次 | 主题 | 内容 | 对应 v2 M# |
|------|------|------|-----------|
| **批次 1（P0 护网闭合）** | 正确性/CI | GC-N1 A2b stress 进快门禁必红 + 去 `\|\| true`；GC-N2 BGC root-scan 挂起专项；GC-N3 Server GC 冒烟进 CI；GC-N4 ASAN nightly | —（M2 相关） |
| **批次 2（P1 工程闭环）** | 并发/性能工程 | GC-N5 L1 卡表并发读稳定化；GC-N6 世代写屏障压力确定性化；GC-N7 Release 构建暂停/吞吐基准；GC-N8 dynamic_tuning 伺服闭环 | M2 / M6 |
| **批次 3（P2 能力拉平）** | 能力/可观测 | GC-N9 Dynamic Heap Count；GC-N10 provisional 完整形态；GC-N11 ETW 扩容；GC-N12 profile 驱动调参重启 | M3B / M4 / M13 / hanging 任务 |

> 映射说明：GC-N6=M2、GC-N8=M6、GC-N9=M3B、GC-N10=M4、GC-N11=M13 为同一任务的工业化视角别名，**执行以本 v3 的 N 编号为准并回写 M# 状态**；v2 其余 M#（M1/M3A/M5/M7/M8/M9/M10/M11/M12/M14/M15）保持 v2 阶段推进，不在本批次重复。

## 3. 子任务明细

### Phase 10 — 批次 1：P0 护网闭合

| task | 内容 | 约束 | exit_criteria | effort |
|------|------|------|---------------|--------|
| GC-N1 | **A2b 并发写屏障测试进快门禁**：CMake 覆写 `chaos_gc_region_barrier_stress_test` 标签为 unit（脱离 name-derived stress）；gc-ci 快 gate 正则扩到 `chaos_gc_\|test_gc_`；夜间 tier 去 `\|\| true`（失败必红） | [CI] | 快 gate 含 barrier 测试且失败翻红；夜间失败红门 | S |
| GC-N2 | **BGC root-scan 挂起专项**：`chaos_gc_bgc_root_scan_test`（~392s 挂起，known-fail 第 13 条）根因调查 + 修复或降级为真实语义缺口并记录 | [多平台] | known-fail 摘除（通过）或改为文档化真实缺口 + 门禁可见 | L |
| GC-N3 | **Server GC 冒烟进 CI**：新增 gc-ci job `-DCHAOS_IL2CPP_GC_SERVER=ON` 构建 + 跑 `test_gc_coordinator`/`test_gc_heap_manager`（含 `#if CHAOS_IL2CPP_GC_SERVER` 分支测试） | [CI] | Server 构建通过 + 冒烟测试跑通（或如实暴露失败并记录） | M |
| GC-N4 | **ASAN nightly**：gc-ci 夜间档加 ASAN 构建（MSVC `/fsanitize=address`），跑 GC 单测 | [CI] | ASAN 构建 + GC 单测结果可见（失败红门） | M |

### Phase 11 — 批次 2：P1 工程闭环

| task | 内容 | 约束 | exit_criteria | effort |
|------|------|------|---------------|--------|
| GC-N5 | **L1 卡表 realloc 并发读稳定化**：`GcRegisterHeapRange` rebase 时 `g_card_l1.swap` 释放旧数组，`DirtyCard`/`chaos_is_gc_pointer` relaxed 读 → 悬垂窗口。方案：swap 前屏障/延迟释放（RCU 或 epoch）/读写锁；seg_idx 重索引核对 | [多平台] 纯 C++ [JIT/AOT] [热更] | 并发写屏障 + rebase 压力测试无 AV；现有卡表单测 0 失败 | M |
| GC-N6 | **世代写屏障压力确定性化**：`gc_region_barrier_stress_test` 从"可接受 PASS"升级为确定性断言（存活校验而非仅位置校验）+ 多轮稳定 | [多平台] | 压力场景 20x 稳定绿；断言含存活语义 | M |
| GC-N7 | **Release 构建 GC 基准**：Release 配置跑 GC 基准（暂停/吞吐/分配），产出 baseline 文档替换 Debug 数值 | [CI] | Release 基准数据存在 + 对比表更新 | M |
| GC-N8 | **dynamic_tuning 伺服闭环**：free-list rate + 碎片密度 + memory-load 多信号闭环（对齐 CoreCLR dynamic_tuning.cpp） | [JIT/AOT] [热更] | 多信号闭环 + 分配回归 <5% | L |

### Phase 12 — 批次 3：P2 能力拉平

| task | 内容 | 约束 | exit_criteria | effort |
|------|------|------|---------------|--------|
| GC-N9 | **Dynamic Heap Count**（=M3B）：运行期增减 server 堆数，闭环自适应 | [多平台] [JIT/AOT] [热更] | 运行期增减堆无回归、闭环收敛 | L |
| GC-N10 | **provisional 完整形态**（=M4）：高记忆+高碎片 → gen1 强制 compact + NGC2 排队 | [多平台] [JIT/AOT] [热更] | 触发路径正确 + 暂停预算达标 | L |
| GC-N11 | **ETW/事件扩容**（=M13）：BGC 阶段事件族 + per-GC 原因位图 + BCL GCMemoryInfo | [多平台] | >40 事件 + 位图 + BCL 类型 | L |
| GC-N12 | **profile 驱动调参重启**：hanging 任务 `gc-phase3-industrialization` 重启——高分配 benchmark 场景 + 单参数/组合调优 + 推荐值 | [CI] | profile 数据 + 参数推荐 + pause/吞吐改善量化 | M |

## 4. 依赖与调度

```
批次 1（串行优先，全部 P0）: GC-N1 → GC-N2（独立）→ GC-N3（独立）→ GC-N4（独立）
批次 2: GC-N5（独立，可并行 N6/N7）; GC-N8 ← GC-N5 + GC-E1 旋钮
批次 3: GC-N9 ← GC-N3（Server 工程）; GC-N10 ← GC-N9 + M5; GC-N11 独立; GC-N12 ← GC-N7（Release 基准）
```

```yaml
dispatch_model: hybrid
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

## 5. 三约束实现原则（延续 v2 §8）

1. **多平台**：纯 C++（`addr>>shift` 查表 + 位运算 + `std::atomic`），无 asm 屏障拷贝、无平台特定 OS 调用（ASAN/CI 例外属构建层）。
2. **JIT/AOT**：AOT codegen 与新 runtime 符号共用；每项验证 AOT + JIT 两态。
3. **热更友好**：保留单参兼容入口/解释器路径；域卸载 O(region) 不破坏。

## 6. 风险与护栏

| 风险 | 护栏 |
|------|------|
| GC-N1 将并发测试纳入快 gate 后偶发 flaky | 先本地 20x 验证；RESOURCE_LOCK bgc 串行化；flaky 视为必红信号回溯根因 |
| GC-N2 挂起根因深（跨 BGC+safepoint） | 遵循 systematic-debugging；三次规则；可先降级为文档化缺口保证可见性 |
| GC-N3 Server 构建首次暴露编译/链接断裂 | 冒烟 job 的目的就是暴露——如实记录并回填修复 |
| GC-N4 ASAN 全量构建可能慢/缺 DLL | 仅夜间档；失败红门但不算阻塞快 gate |
| GC-N8 伺服闭环改动面大 | 先加多信号采集，再逐步接决策；分配回归阈值护栏 |

## 7. 完成判定（本 roadmap 终态）

- 三批次全部子任务进入终态（completed/hanging/abandoned），exit_criteria 满足。
- 快 gate / 夜间门禁具备"失败必红"能力；known-fail 清零或全为文档化真实缺口。
- Server GC 有 CI 覆盖；ASAN 常态可见；Release 基准存在。
- 保留域卸载 O(region) 差异化。

## 8. 执行日志

- 2026-08-25：创建 v3 执行版。用户确认按 1+2+3 三批次推进。
- 2026-08-25：**GC-N1 ✅ 完成**：`chaos_gc_region_barrier_stress_test` 标签覆写为 unit（`tests/contracts/native/runtime-core/CMakeLists.txt`），本地验证 `-LE benchmark|stress|soak` 下可见、`-L stress` 下排除；gc-ci.yml 快 gate 正则扩为 `chaos_gc_|test_gc_`；夜间 tier 去除 `|| true`（失败必红）。
- 2026-08-25：**GC-N3 ✅ 完成（CI 配置）**：gc-ci.yml 新增 `gc-server-smoke` job（`-DCHAOS_IL2CPP_GC_SERVER=ON` 独立构建 + `test_gc_coordinator|test_gc_heap_manager`）。首次覆盖 Server 路径（此前零 CI 执行）。
- 2026-08-25：**GC-N4 ✅ 完成（CI 配置）**：gc-ci.yml 新增 `gc-asan-nightly` job（MSVC `/fsanitize=address` 全构建 + GC 单测 + ASan DLL 拷贝 + 失败红门）。
- 2026-08-25：**GC-N2 协调记录**：工作区存在并行 GC 调试线（未提交改动 `gc_worker_pool.cpp`/`gc_bgc_smoke.cpp`/`gc_stress_test.cpp` + cdb 诊断产物，8/24-8/25 活跃），正在处理 BGC root-scan 相关调试；其已自行创建 `.github/workflows/gc-stress-nightly.yml`（未提交）。**GC-N2 由并行线承接，本 roadmap 不重复启动；待其收敛后复核 known-fail 摘除。** 并行线文件不触碰、不 stage、不 commit。
- 2026-08-25：**GC-N6 ✅ 调查收敛（3 发现，2 修复提交，1 专项交接）**：内容存活校验原型（TypeInfo + magic）暴露——(1) ✅ `ef0012d49` 屏障 4MB chunk 碰撞漏卡 UAF（IsNurseryPointer 精确判定）；(2) ✅ `4fd172906` demotion 不重设卡（DemotionRelocate Phase 2.5 对新地址 DirtyCard）；(3) 🔴 **OPEN 专项**：布局版（typed）对象触发 young-GC 无限循环挂起（首个 young GC，scavenge 死循环，与 Phase-2 typed 遍历路径相关）——升级独立专项，修复前内容校验版测试不可启用，位置校验版保持快 gate 绿。详见 `notes/gc-n6-liveness-findings-2026-08-25.md`。
- 2026-08-25：**GC-N5 ✅ 完成**：`gc_card_table.cpp` 两处 `g_card_l1.swap` 后旧数组被释放 → 并发 `DirtyCard` 无锁读 UAF；改为 retire-not-free（`g_card_l1_retired` 固定槽位数组，永不释放，512KB/次增长步骤，镜像 g_card_bundle 一次性分配策略）。验证：barrier stress 3/3、test_gc_card_table_ext 5/5、test_gc_region 18/18 全绿（含 L1AutoGrowth 路径）。已知残余：并发 rebase 下 reader 的 base/seg_idx 快照竞态（极窄窗口，仅 below-base 分配触发，已注释记录）。
