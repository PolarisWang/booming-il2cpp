# STATUS — 架构边界修复 & AI 合规体系

> 更新: 2026-06-12

## phase: roadmap
## blocking_questions: []
## question_clearance: cleared
## clearance_confirmed_by_user: true
## child_execution_mode: auto
## auto_continue: true
## auto_stop_policy: blocking-only
## dispatch_model: hybrid
## recommended_next_child: A-3
## task_type: roadmap
## roadmap_or_plan: roadmap-v1-01.md
## dispatch_doc: DISPATCH.md

---

## 调度状态

dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: [batch-a3]
completed_batches: [batch-a1, batch-b]
pending_batches: [batch-c, batch-d]
## updated_dispatch
- A-1: ✅ completed — assert stubs moved to SDK runtime_stubs/
- A-2: ✅ completed — JitVehHandler/CHAOS_FACT_CHECK/FactAbortHandler → Scriban; runtime_entry_patcher.py deleted
- B-1: ✅ completed — cache key versioning with codegen/TPG/runtime hashes; escape hatch in pipeline-config.yaml
- B-2: ✅ completed — .cache-contract.json written after each build
- A-3: ✅ completed — Interop stub → Codegen emitter (BuildExternalRuntimeDispatchTable fixed)
- A-4: ✅ completed — C# wrapper generation → ATG --generate-wrappers (WrapperEmitter.cs + build.py delegation)
- A-5: 🔄 executing — Flat layout copy hack fix
- recommended_next_child: A-3

---

## 设计摘要

### 边界拍板

1. **修复范围**: 全量 12 项违规（P0 × 2 + P1 × 3 + P2 × 5 + Low × 2）
2. **AI 合规保障**: 全链路强制（Hook + pre-commit + CI）
3. **Hephaestus 缓存**: cache key 包含 codegen/TPG 版本 hash

### Authority 决策

- Assert stubs → SDK runtime_stubs/（非 TPG，非 Python）
- C# wrapper 生成 → ATG `--generate-wrappers` 命令（从 Python 迁移）
- 混合方案: SEH fix + MACRO → Scriban 模板；interop stub → Codegen emitter
- BOUNDARY_OVERRIDE 标注机制：必须含 ticket link + 过期时间

### 当前结论

位于 brainstorm → roadmap 过渡阶段。设计文档已写入 `design-v1-01.md`。

### 批处理划分

| Batch | 名称 | 任务数 | 预估 | 依赖 |
|-------|------|--------|------|------|
| A | C++ 修补迁移 | 5 | 5-7d | Batch D 依赖此完成 |
| B | 缓存架构 | 2 | 1-1.5d | 无（与 A 并行） |
| C | 工程债务清理 | 6 | 2-3d | 无（全并行，与 A/B 同时） |
| D | AI 合规体系 | 4 | 2-3d | 依赖 A/B/C（需要稳定状态） |

### 风险评估摘要

- Codegen emitter 修改 → 需要独立 verifiation test + worktree 隔离
- Cache 版本化 → 正常开发无损，仅 codegen/TPG 变更时重构建
- BOUNDARY_OVERRIDE 过期 → CI 检查自动 fail（30天容忍期）

### 三优先级权衡结论

P1(性能) ✅ 无影响 · P2(方案) ✅ 显著提升 · P3(热更) ✅ 无影响

---

## 下一步入口

进入 `roadmap` 技能 → 将 4 个 Batch 拆分为 roadmap 阶段和子任务。
