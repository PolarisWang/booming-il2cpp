---
task_id: foundation-dll-full-development
title: Foundation DLL 全量 il2cpp 开发 Roadmap
task_type: roadmap
lifecycle_status: in-progress
phase: roadmap
created_at: 2026-05-02 01:00:00 +08:00
updated_at: 2026-05-03 19:00:00 +08:00
current_dir: docs/dev/in-progress/foundation-dll-full-development
roadmap_or_plan: docs/dev/in-progress/foundation-dll-full-development/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: foundation-dll-phase-1-corelib
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Foundation DLL 全量 il2cpp 开发 Roadmap

## 设计摘要

### 范围
14 foundation DLL，每个 DLL 独立走 Phase A（批量 IL 扫描 + emitter 补齐）+ Phase B（逐个 family 完整 il2cpp 开发 + 验证闭环）

### 边界拍板
- method 合约保持泛化形式，不做真实类型签名
- IL 分析源 = 项目已有 managed DLL 副本
- 分析手段综合：IL 扫描 + 指令模式分析 + AI + wiki 查表 + error-driven
- Family 顺序：AI 分析 IL 复杂度推荐，从简单到复杂

### 失败处理
- A(emitter缺失) → 切回 Phase A 补齐
- B(测试代码问题) → 立即修复
- C(运行时崩溃) → 立即修复
- D(性能退化) → 记录不阻塞

### 开发原则
每个 family 核心开发阶段必须给出多方案设计，从 4 维度评估：性能最优(P1) > 架构完美(P2) > HotUpdate(P3) + CodeGen 代码优雅

### 三优先级权衡
P1(性能) > P2(架构) > P3(HotUpdate)

## 关键文档
- roadmap: `roadmap-v1-01.md`

## 最近摘要
**2026-05-03 修正：Phase 1 (System.Private.CoreLib) 实际完成度评估为 scaffolding-only，非此前记录的"全部通过"。**

全量审计确认 CoreLib 42 families 的生成代码全部为 skeleton stub（RuntimeSkeletonPage0001.cpp 中所有方法返回 CHAOS_BRIDGE_STATUS_OK，BenchmarkNativeEntry.cpp 所有方法 return 42），无真实 il2cpp 翻译代码。此前验证体系仅检查文件存在和编译通过，未检查代码真实性。

修正动作：
- foundation-dll-phase-1-corelib 从 completed 移回 in-progress
- phase 标记为 assessment，STATUS.md 已重写
- 子任务目录已从 docs/dev/completed/ 移回 docs/dev/in-progress/
- roadmap 的 recommended_next_child 修正为 foundation-dll-phase-1-corelib
- Phase 2-11 标记为 blocked，等待 Phase 1 实质性完成后解锁

## 子任务状态
| task_id | phase | status |
| --- | --- | --- |
| `foundation-dll-phase-0-infra` | `phase-0` | `completed` |
| `foundation-dll-phase-1-corelib` | `phase-1` | `in-progress (assessment)` |
| `foundation-dll-phase-2-collections-immutable` | `phase-2` | `blocked` |
| ...后续 phase 2-11 | | `blocked` |

## 下一步
1. CoreLib 42 families 逐 family codegen review 确认真实完成度
2. 修复验证体系（区分 stub vs 真实代码）
3. 从 Phase A 开始 CoreLib 真实 il2cpp 开发

## latest_stop_point
Phase 1 corrected from completed to scaffolding-only. All subsequent phases blocked pending Phase 1 substantive progress.
