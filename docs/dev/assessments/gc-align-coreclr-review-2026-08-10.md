# Review — 本会话全部改动 + 测试覆盖评估

> 日期：2026-08-10
> 范围：Phases 1-5 全部 GC 改动（git log 中 ~40 个 GC commit）
> 状态：Phase 6 暂停（用户指示），先 review + 测试评估 + 文档

---

## 1. 整体改动盘点（Phases 1-5）

| Phase | 改动 | commit | 行为影响 |
|-------|------|--------|---------|
| P1 | GC-B1 移除 CHAOS_GC_STRESS / GC-A1 全根集 / GC-C1 声明式终止 + 2 测试修复 | `6659812d4`~`b9311d2e9` | 正确性：修 UAF、修终止竞态、去测试钩子 |
| P2 | GC-E1 配置旋钮 / GC-D1 OOM 降级(3增量) | `589baadfd`+`5dc595cf7`+`226990998`+`6eeeed4b9` | 韧性：可调参、OOM 逐级降级、provisional、压力 decommit |
| P3 | GC-G1 Dependent 收敛 / GC-F1 handle 剪枝 / GC-H1 触发原因 | `10608b337`+`b6333ae54`+`007e57e96` | 正确性+可观测：深链收敛、handle 剪枝、触发归因 |
| P4 | GC-J1 BGC 并发 sweep | `76be683f4` | 性能：BGC sweep 并行 |
| P5 | GC-K1(+K1b)+K2(a/b/c/d)+K3+K4 区域化重构 | `69613b545`~`f8799b829` + `ec71dce0b` | 架构：region size class、世代写屏障链路（skewed 表+双参屏障+card bundle+Gen1 rebind） |

**总体验证**：完整可运行的 GC 单测套（11+ 项确定性测试）全部 0 失败，覆盖分配路径/年轻代/老年代/LOH/POH/scheduler/root-scan/handle/card-tab。worktree 干净（GC 相关全部提交）。

## 2. 测试覆盖评估（🔴 新增功能缺直接测试）

**关键发现**：本会话新增的核心 GC 功能**多是"增量/保守"设计（默认不改现有行为），因此既有回归测试全绿；但它们的具体行为没有直接的单元测试**。这是覆盖缺口。

| 新功能 | 直接单测 | 缺口严重度 | 建议测试 |
|--------|---------|-----------|---------|
| **K2a region→gen 表**（`GetRegionGen`/`SetRegionGen`） | ❌ 无 | 🔴 高 | 挂 `gc_region_test`：nursery→young(0)、tenured/LOH→old(2)、gen1→young(0) 返回正确；uncovered addr→old 保守 |
| **K2b 双参屏障**（`chaos_gc_dirty_card_dst_ref`） | ❌ 无 | 🔴 高 | 挂 `gc_card_table_ext_test`：gen0→gen0 不设卡 / old→gen0 设卡 / ref==null 不设卡 / 同代不设卡 |
| **K2d card bundle**（`CardBundleSet`/`CardBundleTest`/bundle fast-skip） | ❌ 无 | 🟠 中 | 挂 `gc_card_table_ext_test`：DirtyCard 设 bundle；clean bundle 跳扫；ClearAllCards 清 bundle |
| **K2c codegen 发射 `_dst_ref`** | ❌ 无（需生成 native） | 🟠 中 | foundation-dll 管线 regen 后，验证生成代码用 `_dst_ref` + 编译链接 |
| **D1 provisional / 半量预算 / OOM 归因** | ❌ 无 | 🟠 中 | 挂 `gc_scheduler_test`：provisional 激活→DecideCollection 强制 blocking；OOM 上报半量 size |
| **E1 config env override** | 🟠 手动验证 | 🟠 中 | 自动化：设 `CHAOS_GC_NurserySize` → `InitYoungGeneration` 用新值 |
| **C1 terminate / J1 BGC sweep** | 🟠 靠现有 stress | 🟢 低 | 现有 `gc_parallel_mark_test` + `gc_bgc_*` stress 已覆盖并发健壮性 |

### 压力测试评估
- 现有压力测试（`capacity_stress_test`/`delegate_stress_test`/`gc_bgc_stress_test` 等）覆盖**并发健壮性**（多线程分配/GC/BGC）。
- **未专门覆盖**：region 世代屏障的 gen0→gen0 短路在高并发/大堆下是否正确（短期可能无直接 stress 场景）。
- **建议**：K2b/K2a 出直接单测后，可加一个"多线程 ref-store 到各自 nursery + 触发 young GC"的压力场景验证世代屏障不丢跨代引用（长期）。

## 3. 建议补充的测试（按优先级）

### P0（正确性直接验证，2 项）
1. **K2a region-gen 单测**（挂 `gc_region_test`）：验证 `GetRegionGen` 按 kind 返回正确代 + uncovered→old 保守。
2. **K2b 双参屏障单测**（挂 `gc_card_table_ext_test`）：验证 gen0→gen0 不设卡、old→gen0 设卡。

### P1（完整性，2 项）
3. **K2d card bundle 单测**（挂 `gc_card_table_ext_test`）：DirtyCard 设 bundle + clean 跳扫 + clear 同步。
4. **D1 provisional 单测**（挂 `gc_scheduler_test`）：provisional→强制 blocking。

### P2（管线/自动化）
5. K2c 完整 foundation-dll 管线 regen 验证（生成 native 用 `_dst_ref`）。
6. E1 config env override 自动化测试。

## 4. 文档更新

- [x] **roadmap**：Phase 1-5 全部标记 completed，Phase 6 标记待续。
- [x] **父 STATUS**：进度表更新至 Phase 5 完成。
- [x] **子任务 STATUS**：GC-A1/C1/E1/D1/G1/F1/H1/J1/K1-K4 各子目录 STATUS 记录实现+验证+commit。
- [ ] **wiki `24-CRAG-GC架构参考.md`**：新增 region 世代写屏障（K2a 表/双参屏障/card bundle/Gen1 rebind）章节，更新架构参考。
- [ ] **README/INDEX**：无大的新增功能清单，可补 K 系列记录。

## 5. 结论

- **改动齐全、可验证**：Phases 1-5 全部 0 回归，worktree 干净。
- **测试缺口明确**：K2a/K2b/K2d 三个新功能缺直接单测（P0 建议先补）。
- **Phase 6** 暂停，待后续会话。
