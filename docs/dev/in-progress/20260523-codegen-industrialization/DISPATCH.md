# DISPATCH.md — 调度总纲

> 父任务: docs/dev/in-progress/20260523-codegen-industrialization/
> 调度模型: hybrid（阶段内三线程并行，阶段间串行）
> 并行容量: 1 FT + 2 HT（实际执行中可能调整）

---

## 调度说明

本 roadmap 采用 hybrid 调度模式：
1. **Phase 内并行**：每个 Phase 的三个线程（A/B/C）在同一个时间段内并行推进
2. **Phase 间串行**：Phase 2 须在 Phase 1 关键依赖（A-P1-2 稳定）满足后启动
3. **线程内串行**：每个 Thread 的子任务按依赖顺序执行

## 线程职责

| 线程 | 人力 | 执行终端 | 任务链 | 冲突域 |
|------|------|----------|--------|--------|
| Thread A | FT | terminal-1 | A-P1-1 → A-P1-2 → A-P2-1 → A-P2-2 → A-P2-3 → A-P3-1 → A-P3-2 → A-P3-3 | src/managed/Chaos.IL2CPP.Generator/ |
| Thread B | HT | terminal-2 | B-P1-1 → B-P1-2 → B-P2-1 → B-P2-2 → B-P3-1 → B-P3-2 → B-P3-3 | src/native/codegen/ |
| Thread C | HT | terminal-3 | C-P1-1 → C-P1-2 → C-P1-3 → C-P2-1 → C-P2-2 → C-P2-3 → C-P3-1 → C-P3-2 → C-P3-3 | testing/, wiki/ |

## 批次定义

### Phase 1 Batches

| batch_id | mode | description | tasks | wait_for | merge_after |
|----------|------|-------------|-------|----------|-------------|
| P1A | sequential | Thread A Phase 1 | [A-P1-1, A-P1-2] | 无 | all-completed |
| P1B | sequential | Thread B Phase 1 | [B-P1-1, B-P1-2] | 无 | all-completed |
| P1C | parallel | Thread C Phase 1（C-P1-3 与 C-P1-1/2 无文件冲突，可并行） | [C-P1-1, C-P1-2, C-P1-3] | 无 | all-completed |
| **P1-merge** | — | P1 汇总 | 所有 P1 子任务 | P1A, P1B, P1C | all-completed |

### Phase 2 Batches

| batch_id | mode | description | tasks | wait_for | merge_after |
|----------|------|-------------|-------|----------|-------------|
| P2A | sequential | Thread A Phase 2 | [A-P2-1, A-P2-2, A-P2-3] | P1A（A-P1-2 稳定） | all-completed |
| P2B | sequential | Thread B Phase 2 | [B-P2-1, B-P2-2] | P1B（B-P1-2 完成） | all-completed |
| P2C | sequential | Thread C Phase 2 | [C-P2-1, C-P2-2, C-P2-3] | P1A（A-P1-2 稳定） | all-completed |
| **P2-merge** | — | P2 汇总 | 所有 P2 子任务 | P2A, P2B, P2C | all-completed |

### Phase 3 Batches

| batch_id | mode | description | tasks | wait_for | merge_after |
|----------|------|-------------|-------|----------|-------------|
| P3A | sequential | Thread A Phase 3 | [A-P3-1, A-P3-2, A-P3-3] | P2A（A-P2-3 完成） | all-completed |
| P3B | sequential | Thread B Phase 3 | [B-P3-1, B-P3-2, B-P3-3] | P2B（B-P2-2 完成） | all-completed |
| P3C | parallel | Thread C Phase 3（C-P3-3 与 C-P3-1/2 无冲突，可提前） | [C-P3-1, C-P3-2, C-P3-3] | P2C（C-P2-2 完成） | all-completed |
| **P3-merge** | — | P3 汇总 + 收官核查 | 所有 P3 子任务 | P3A, P3B, P3C | all-completed |

## 文件冲突检测

| 线程对 | 潜在冲突域 | 冲突概率 | 处理方式 |
|--------|-----------|---------|---------|
| A ↔ B | 无重叠 | 无 | 托管 C# vs 原生 C++，完全独立 |
| A ↔ C | testing/ 中 codegen 相关 Python 测试 | 低 | Thread C 修改测试文件，Thread A 修改 codegen 源；验证时通过 CI 检测 |
| B ↔ C | testing/src/native/codegen/ | 低 | Thread B 修改 codegen 实现，Thread C 新增测试；通过 PR review 协调 |
| A ↔ B ↔ C | 无三方重叠 | 无 | 三个独立目录树 |

## 合并协议

1. **每个 Phase 内的 batch 完成**后，更新父 STATUS.md 的 `## 调度状态`
2. **Phase merge 条件满足后**，执行 Phase exit criteria 核查
3. **Phase exit 通过后**，推进到下一 Phase，更新 `latest_stop_point` 和 `recommended_next_child`
4. **并行终端只写 child STATUS.md**，不写父 roadmap 文件
