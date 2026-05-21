---
task_id: foundation-dll-full-development
title: Foundation DLL 全量 il2cpp 开发 Roadmap
task_type: roadmap
lifecycle_status: in-progress
phase: roadmap
created_at: 2026-05-02 01:00:00 +08:00
updated_at: 2026-05-05 21:00:00 +08:00
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
**2026-05-05 修正：此前 scaffolding-only 判断有误 — 所有 121 个 family 的 native-aot.generated.cpp 均包含真实 IL 翻译代码。RuntimeSkeletonPage*.cpp 是附带骨架文件，与真实翻译代码并行存在，不影响完成度评估。**

### Phase A 完成（2026-05-03）
1. **16 个缺失 emitter opcode 补齐** — nop, neg, div.un, rem.un, clt.un, conv.r.un, starg.s, sizeof, volatile., readonly., ldvirtftn, endfinally, rethrow, endfilter, constrained., refanytype
2. **stub_detector 创建** — `build/toolchains/run/testing/foundation_dll/stub_detector.py`
3. **verification_kernel 集成 stub 检测** — native-proof 评估现在包含 stub 扫描
4. **native-proof 重新验证完成**（2026-05-05）— 0 stub 发现, CoreLib 33 families 全部 100%

### Phase A 完成度
- CoreLib 33 families: native-proof 100% 通过, 0 stub
- 其他 13 DLL (92 families): native-proof pending, 等待 Phase B 开发

## 子任务状态
| task_id | phase | status |
| --- | --- | --- |
| `foundation-dll-phase-0-infra` | `phase-0` | `completed` |
| `foundation-dll-phase-1-corelib` | `phase-1` | `in-progress (assessment-corrected)` |
| `foundation-dll-phase-2-collections-immutable` | `phase-2` | `blocked` |
| ...后续 phase 2-11 | | `blocked` |

## 下一步
Phase 1 进入 Phase B: 逐 family il2cpp 翻译 + managed/native/hotupdate proof

## latest_stop_point
Phase A 完成: emitter 补齐 + stub 检测 + 验证体系修复。等待 Phase B 启动指令。
