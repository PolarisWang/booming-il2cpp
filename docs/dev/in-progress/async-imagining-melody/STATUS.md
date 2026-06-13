---
task_id: async-imagining-melody
task_type: roadmap
phase: completed
lifecycle: completed
created: 2026-06-13
clearance_source: brainstorm-approved
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
completed: 2026-06-13
---

# Chaos IL2CPP 架构重构 — ABI 薄层 + capabilities

## 最终状态

| Phase | 进度 | 状态 |
|-------|------|------|
| P1a: ChaosAbiExportCollector | 100% | ✅ 完成 |
| P1b: 移除 build.py post-TPG | 100% | ✅ 完成 |
| P2: ABI 函数表 V1 | 100% | ✅ 完成 |
| P3: TPG BuildProject 简化 | 100% | ✅ 完成 |
| P4: capabilities ATG 集成 | 100% | ✅ 完成 |
| P5: codegen-capabilities Expert | 100% | ✅ 完成 |

## 提交记录

| Commit | Phase | 内容 |
|--------|-------|------|
| `e6464ed1a` | P1a + P5 | ChaosAbiExportCollector + codegen-capabilities Expert |
| `13170b79e` | P1b | 移除 build.py post-TPG 三段 |
| `3e8af46b3` | P3 | CppProjectEmitter BuildProject() 简化 |
| `fa3031e74` | P2 + P4 | ABI V1 函数表 + DllScanner capabilities 集成 |

## 关键产出物

- `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExportCollector.cs` — 自动补齐 extern 声明
- `contracts/native/v0/runtime_abi.h` — RuntimeAbiV1（12 个新增指针）
- `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/DllScanner.cs` — capabilities 驱动的黑名单
- `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/Program.cs` — --capabilities-path 参数
- `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Emission/CppProjectEmitter.cs` — 简化BuildProject()
- `.ai/skills/library/skills/dev-il2cpp-codegen-capabilities/` — 新 Expert

## 关键文档
- 架构设计: `C:\Users\haochuan.wang\.claude\plans\async-imagining-melody.md`
