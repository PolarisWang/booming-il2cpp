# STATUS — Foundation-DLL HotUpdate DML Pipeline Fix

## Meta
- task_id: 20260525-hotupdate-dml-pipeline
- lifecycle: planning
- phase: roadmap
- task_type: roadmap
- roadmap_or_plan: docs/dev/in-progress/20260525-hotupdate-dml-pipeline/roadmap-v1-01.md

## 架构映射
- il_feature: hotupdate-dml-pipeline
- problem: runtime-patchdata.cpp 空桩 + kHotpatchKeepNative 阻止 interpreter + RunHotupdateBenchmarkMode 未 apply patch
- root_cause_chain:
  1. foundation-dll 管线未接入 emit-patch-data 流程
  2. RunHotupdateBenchmarkMode 不调用 ApplyHotpatchIfAvailable
  3. SetPatchedBySlot 不清除 kHotpatchKeepNative
- solution_path: 4 阶段修复，见 roadmap-v1-01.md

## 优先级评估
| 维度 | 满足情况 |
|------|---------|
| P1 性能最优 | benchmark 计时反映真实 interpreter 耗时，识别性能瓶颈 |
| P2 架构完美 | 复用 project-test 现有 emit-patch-data 流程，不改 codegen |
| P3 热更新适配 | 修复的核心目的 — 使 hotupdate 验证真实可信 |

## 关键文档
- roadmap: roadmap-v1-01.md

## 当前进展
- roadmap 已完成，待用户确认
- 子任务待拆分

## blocking_questions
- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: false
