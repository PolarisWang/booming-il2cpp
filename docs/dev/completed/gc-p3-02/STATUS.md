---
task_id: gc-p3-02
title: GC 基准回归测试套件（G-15）
task_type: plan
lifecycle_status: in-progress
phase: executing
created_at: 2026-05-22 21:40:00 +08:00
updated_at: 2026-05-22 21:40:00 +08:00
current_dir: docs/dev/in-progress/gc-p3-02
parent_task_id: 20260522-gc-industrialization
source_task_id: gc-p3-02
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
reentry_skill: dev-writing-plans
---

# STATUS — GC 基准回归测试套件（G-15）

## 范围

补齐现有的 `gc_regression_benchmark.cpp` 基准测试套件到工业化标准：

1. **bench-compare 回归对比工具** — Python 脚本，对比两次 BENCH| 输出，报告退化
2. **gtest 框架评估** — 评估当前自定义 runner 是否需要迁移到 gtest
3. **多线程分配基准** — 补充 MT 并发分配场景
4. **基准基线记录** — 系统化基线存储 + 退化告警

非范围：CI 集成门禁（gap 5）

## 最近摘要

2026-05-22 21:40：启动 gc-p3-02。按用户要求顺序：gap 2 (bench-compare) → gap 3 (gtest 评估) → gap 4 (MT 基准) → gap 1 (基线记录)。

## 下一步

实现 gap 2：bench-compare 回归对比工具
