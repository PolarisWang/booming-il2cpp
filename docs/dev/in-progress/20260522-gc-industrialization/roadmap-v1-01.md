# CRAG GC 工业化补齐路线图（修正版）

> 基于 `docs/discuss/20260522-crag-gc-industrialization-assessment.md` 及实际代码审计修正
> 目标：CRAG GC 从 5.5/10 → 9.0/10 工业化评分

---

## 目标

修复 CRAG GC 与工业生产标准之间的 **14 项真实差距**（原 30 项中 14 项已实现，7 项部分实现）。通过三阶段推进使 GC 子系统达到"接近 CoreCLR WKS"的工业化程度：

1. **Phase 1 — 基础设施与平台补齐**（~1 个月）：ARM64 写屏障、大页、平台验证、OOM 保护、编译时选择、测试修复
2. **Phase 2 — 可观测性与质量保障**（~1 个月）：ETW 诊断、基准测试、SOS 调试、升温验证
3. **Phase 3 — 高级并发能力**（~3 个月）：Server GC 模式、并发根扫描、堆布局优化

## 范围边界

- 代码范围：`src/native/runtime-core/gc/` 及必须配套的 BCL/ABI 对接、工具链
- 测试范围：`tests/contracts/native/runtime-core/gc_*.cpp` + 新增基准测试
- 非目标：不重新设计 GC 架构，不改 codegen/interpreter，不改 MemoryDomain

## 全局优先级评估

| Phase | 方案选型 | P1 性能 | P2 架构 | P3 热更新 | 裁决 |
|-------|---------|---------|---------|----------|------|
| 1 | 基础设施补齐 | 正面（大页、ARM64 barrier） | 正面（编译时选择） | 无影响 | PASS |
| 2 | 可观测性 | 正面（基准可发现退化） | 正面（ETW/SOS 标准诊断） | 无影响 | PASS |
| 3 | 高级并发 | 正面（Server GC 吞吐量） | 正面（并发根扫描） | 无影响 | PASS |

---

## 阶段列表

### Phase 1 — 基础设施与平台补齐

- **goal**: 补齐 GC 基础能力缺失项，使 CRAG GC 在 ARM64 平台可用、大页高效、OOM 有保护
- **exit_criteria**: 全部子任务实现 + 全部现有 GC 测试 PASS + 无新增已知问题
- **dependencies**: 无（基线已稳定）

### Phase 2 — 可观测性与质量保障

- **goal**: 建立 ETW 诊断、基准回归检测、SOS dump 分析、72h 长时间稳定性四个质量保障体系
- **exit_criteria**: 基准套件可运行并产生可对比报告、72h 升温无新问题
- **dependencies**: Phase 1（基础平台稳定）

### Phase 3 — 高级并发能力

- **goal**: 实现 Server GC 模式 + 并发根扫描 + 堆布局优化，达 CoreCLR WKS 同等能力
- **exit_criteria**: Server GC 可配置启用并通过 full soak；并发根扫描减少 BGC STW pause
- **dependencies**: Phase 2（基准套件就绪，可评估性能提升）

---

## 子任务映射

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| **Phase 1 — 基础设施与平台补齐** | | | | | | |
| gc-p2-01 | phase-1 | **completed** | **ARM64 硬件写屏障（G-5）** | 无 | gc_card_table.h/cpp | 1 周 |
| gc-p2-02 | phase-1 | **completed** | **大页支持 THP/LargePages（G-21）** | 无 | gc_region.cpp, gc_old_gen.cpp | 2 周 |
| gc-p2-03 | phase-1 | **completed** | **iOS/Android 平台验证 + 平台适配测试（G-27）** | 无 | gc_numa_*.cpp, gc_region.cpp | 2 周 |
| gc-p2-04 | phase-1 | **completed** | **Finalizer OOM 保证：预留内存/专用堆（G-12）** | 无 | gc_finalizer.*, engine_lifecycle.cpp | 1 周 |
| gc-p2-05 | phase-1 | **completed** | **内存压力主动调度反馈闭环（G-28）** | 无 | gc_scheduler.cpp | 1 周 |
| gc-p2-06 | phase-1 | **completed** | **GC 编译时选择（G-29）** | 无 | CMakeLists.txt, gc/*.h | 1 周 |
| gc-p2-07 | phase-1 | planned | **测试覆盖增强：修复预存问题 + 补充用例（G-30）** | 无 | 测试文件 | 1-2 周 |
| gc-p1-vfy | phase-1 | planned | **Phase 1 集成验证** | 全部 P1 | 验证目录 | 3 天 |
| **Phase 2 — 可观测性与质量保障** | | | | | | |
| gc-p3-01 | phase-2 | planned | **ETW 原生诊断事件系统（G-7）** | phase-1 | gc_events.h/cpp | 2 周 |
| gc-p3-02 | phase-2 | planned | **GC 基准回归测试套件（G-15）** | phase-1 | 新基准测试文件 | 2 周 |
| gc-p3-03 | phase-2 | planned | **SOS 调试扩展（G-16）** | phase-1 | 新模块 | 2-3 周 |
| gc-p3-04 | phase-2 | planned | **Arrhenius 72h 升温测试（G-17）** | phase-1 | 测试/验证目录 | 1 周 |
| gc-p2-vfy | phase-2 | planned | **Phase 2 集成验证** | 全部 P2 | 验证目录 | 3 天 |
| **Phase 3 — 高级并发能力** | | | | | | |
| gc-p4-01 | phase-3 | planned | **Server GC 模式（G-4）** | phase-2 | gc_old_gen.cpp, gc_bgc.cpp, gc_numa.h | 2-3 月 |
| gc-p4-02 | phase-3 | planned | **真正并发根扫描（G-25）** | phase-2 | thread_state.cpp, gc_root_scanner.cpp | 2-3 周 |
| gc-p4-03 | phase-3 | planned | **高级堆布局优化（G-23）** | phase-2 | gc_layout.h/cpp, gc_old_gen.cpp | 2 周 |
| gc-p3-vfy | phase-3 | planned | **Phase 3 全平台 72h soak 验收** | 全部 P3 | 全平台测试 | 1 周 |

---

## 依赖图

```
Phase 1:
  gc-p2-01 ─┐
  gc-p2-02 ─┤
  gc-p2-03 ─┤ (无相互依赖，任意顺序)
  gc-p2-04 ─┤
  gc-p2-05 ─┤
  gc-p2-06 ─┤
  gc-p2-07 ─┘
       └── gc-p1-vfy
                  │
Phase 2:          ▼
  gc-p3-01 ─┐
  gc-p3-02 ─┤ (无相互依赖，任意顺序)
  gc-p3-03 ─┤
  gc-p3-04 ─┘
       └── gc-p2-vfy
                  │
Phase 3:          ▼
  gc-p4-01 ─┐
  gc-p4-02 ─┤ (无相互依赖)
  gc-p4-03 ─┘
       └── gc-p3-vfy
```

---

## 风险

| 风险 | 影响 | 缓解 |
|------|------|------|
| Server GC 模式复杂度极高（3 个月） | 占 Phase 3 全部时间 | 按里程碑检查，若进度 <70% 拆为独立路线图 |
| ARM64 写屏障无真机测试 | 正确性风险 | 至少完成 Windows ARM64 编译验证 + 单元测试 |
| 72h 升温发现隐藏 bug | Phase 2 延误 | 发现的 bug 按 P0/P1/P2 分级处理 |

---

## 当前建议推进顺序

串行推进，当前进度：**gc-p2-04→05→06 已完成。下一步：gc-p2-07（测试覆盖增强）**。

---

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- 每个子任务启动前做最小 `preflight self-review`
- 子任务默认先进入 `writing-plans` → `executing-plans`
