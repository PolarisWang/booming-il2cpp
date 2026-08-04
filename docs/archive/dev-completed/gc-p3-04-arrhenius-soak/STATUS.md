---
task_id: gc-p3-04
title: Arrhenius 72h 升温测试（G-17）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-22 20:40:00 +08:00
updated_at: 2026-05-22 20:50:00 +08:00
current_dir: docs/dev/completed/gc-p3-04-arrhenius-soak
parent_task_id: 20260522-gc-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-writing-plans
---

# STATUS — gc-p3-04: Arrhenius 72h 升温测试（G-17）

## 目标

创建 CRAG GC 长时间稳定性测试框架，能够在持续运行中通过周期性快照检测性能退化（暂停时间上升、内存泄漏、GC 频率增加等）。

## 实现

### 新增文件

- `tests/contracts/native/runtime-core/gc_soak_test.cpp` — Arrhenius 升温测试

### 设计

**5 阶段循环：**

| 阶段 | 持续时间 | 行为 |
|------|---------|------|
| AllocBurst | 15 s | 快速 50000 次 nursery 分配，保持 200 个对象存活 |
| GCHeavy | 10 s | 5000×20 轮 nursery 分配触发 young GC，手动触发 full GC |
| Mixed | 20 s | 30000 次混合大小分配，保持 500 个对象作为 survivor |
| Idle | 5 s | 最小分配，让 GC 追赶 |
| Promo | 10 s | 2000 次 old-gen 分配（每个 256 字节），10 s 后排放 |

**退化检测（每 60 秒运行一次，5 项检查）：**

1. **Young pause trend** — 最近 10 个快照的 young GC 暂停斜率应 < 500 ns/sample
2. **GC frequency** — 最近 young 收集次数/快照的斜率 < 10
3. **Finalizer buildup** — 待处理 finalizer 不超过 10000
4. **Region growth** — 活跃 region 数量斜率 < 5/sample
5. **Full pause trend** — full GC 暂停斜率 < 2000 ns/sample

**输出：**

- `ARRHENIUS|` 行：结构化控制台输出
- JSONL 指标文件：`artifacts/native-runtime-core-test/reports/gc_soak_<timestamp>.jsonl`
- 每 5 秒一次快照记录到 JSONL

### 使用方式

```bash
chaos_gc_soak_test --minutes 10       # 运行 10 分钟
chaos_gc_soak_test --minutes 4320     # 运行 72 小时（最大）
chaos_gc_soak_test                    # 默认 10 分钟
```

### 文件改动

| 文件 | 改动 |
|------|------|
| tests/contracts/native/runtime-core/gc_soak_test.cpp | 新建 — 72h 升温测试 |
| tests/contracts/native/runtime-core/CMakeLists.txt | 新增 chaos_gc_soak_test 构建目标 |

## 验证

- 1 分钟运行测试通过：所有阶段正常执行，11 个快照，0 失败
- 构建成功
