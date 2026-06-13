# STATUS

## Phase
`brainstorming` → 设计已更新（第二阶段修复），等待进入 writing-plans

## 本次范围（第二阶段修复）
- **P0**: revert loop 跳过基线已失败方法（A2）
- **P2**: dashboard 增加 hotupdate 诊断字段（B2）
- **WON'T FIX**: 语义变化检测（A1）— keep-native 设计使 Subject_N 不经过解释器

## 边界拍板（第二阶段）
- 只有 2 处修改：C++ Scriban 模板（revert loop）+ Python aggregate（dashboard）
- 不涉及 codegen、PatchDataExtractor、dispatch 路径
- 不改变 pass criterion（保持 `assert_failed == 0 && all_revert && passed > 0`）

## Authority 决策
- C++ 层（RuntimeEntry.cpp.scriban）：修改 RunHotupdateMode revert loop
- Python 层（aggregate.py）：修改 dashboard 输出
- 不修改其他文件

## 当前结论
方案已收敛，blocking_questions 已清零。

## 风险评估摘要
- Revert skip 新字段 `"skipped":true` 对下游解析的影响：低风险，已有 `"passed"` 字段
- Dashboard 新字段命名兼容性：低风险，嵌套结构不破坏现有

## 三优先级权衡结论
- P1 性能：无影响（不改变 dispatch 路径）
- P2 架构：提升（pass criterion 更准确）
- P3 热更：无影响

## blocking_questions
`[]`

## question_clearance
`cleared`

## clearance_confirmed_by_user
用户已确认：2026-06-04

## 关键文档
- 无独立设计文档，设计摘要见本 STATUS.md

## 修改清单

| 文件 | 改动 | 规模 |
|------|------|------|
| `TestProject.RuntimeEntry.cpp.scriban` | Revert loop 增加 baseline_ok 检查，跳过基线已失败方法 | ~5 行 |
| `verification/stages/aggregate.py` | Dashboard 增加 hotupdate 诊断 | ~10 行 |

## 下一步入口
进入 `writing-plans` 生成执行计划。
