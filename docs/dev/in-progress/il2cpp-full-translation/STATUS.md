# STATUS — 传统 IL2CPP 全量翻译

## Meta
- task_id: il2cpp-full-translation
- lifecycle: in-progress
- phase: roadmap
- task_type: roadmap
- roadmap_or_plan: docs/dev/in-progress/il2cpp-full-translation/roadmap-v1-01.md

## 最近摘要
完成全量 codegen 架构设计。确认了关键资产已有（LoaderStage 可读 IL body、LinkerStage 可做 reachability analysis、IL→C++ lowering 已存在）。当前缺口是 Driver 的 convert 命令只输出 entrypoint 的 IR，不含 CoreLib method body。

方案选型：方案 C（混合策略）— Layer 2 Dispatch AOT 为主，Layer 3 Interpreter 兜底，Layer 1 Hot Inlining 后续优化。

## 设计摘要
详见 roadmap-v1-01.md。核心结论：
- IL body reader 已存在（LoaderStage.InstructionDecoding.cs）
- Reachability analysis 已存在（LinkerStage.Reachability.cs）
- 缺口在 Driver → aot-core-ir.json 的 IR 输出层
- 修复后按 Phase 1→2→3→4 推进

## 关键文档
- roadmap: roadmap-v1-01.md

## blocking_questions
- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## 下一步
Phase 1: 全量 IR 输出
