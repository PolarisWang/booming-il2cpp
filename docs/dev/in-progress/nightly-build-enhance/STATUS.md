# Nightly Build 增强 — STATUS

## Phase
`brainstorming → design completed → 待 roadmap 分流`

## 边界拍板
- 5 个维度全部选最高强度 C 方案
- 新增 `verification/analysis/` 模块（value_checker, trend_detector, regression_grading）
- 改 6 个 stage/aggregate/delta/summary 文件
- Profile stage 默认开启（--no-profile 可关闭）
- Hotupdate benchmark 分段安全执行

## Authority
- Owner: nightly CI/CD 域
- 不改变 codegen 或 runtime 的翻译路径
- 不影响 agnostic 的测试语法/规则

## 当前结论
设计已完成并获用户确认。

## 关键文档
- `design-v1-01.md` — 五维度完整方案设计

## blocking_questions
- [x] 全部 5 维度的 C 方案已确认
- [x] Float tolerance 比较策略已定
- [x] 回归分级阈值已定
- [x] Profile default + 回退方案已定
- [x] Hotupdate 安全策略已定

## question_clearance
`cleared`

## clearance_confirmed_by_user
`true`

## 下一步入口
`roadmap` — 5 个独立子任务 × 多阶段推进
