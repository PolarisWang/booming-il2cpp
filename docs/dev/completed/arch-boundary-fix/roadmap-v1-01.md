# Roadmap — 架构边界修复 & AI 合规体系

> roadmap v1.0 — 2026-06-12
>
> 上游设计: `design-v1-01.md`
> 调度策略: hybrid (见 DISPATCH.md)

---

## 1. 目标

消除 DLL 验证管线中所有四层架构违规行为（共 12 项），建立全链路 AI 合规保障体系，确保后续开发严格遵循 Codegen/TPG/ATG/Python 四层边界。

## 2. 范围边界

- **涵盖**: `testing/foundation-dll/verification/`、`src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/`、`src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/`、`src/managed/Chaos.IL2CPP.Generator/`、`chaos-sdk/`
- **验收**: 全量 pipeline build + fact 通过，Python 零 write_text .cpp/.h

## 3. 非目标

- 不修改 native runtime C++ 代码
- 不修改 codegen 生成的 IL→C++ 翻译逻辑本身
- 不重构现有的 Scriban 模板体系
- 不改变 pipeline 的功能行为

## 4. 阶段列表

### Phase 1: C++ 修补迁移 + 缓存架构升级（Week 1）

| 字段 | 值 |
|------|-----|
| goal | 消除 Python 层生成/修补 C++ 的行为；消除缓存盲区 |
| exit_criteria | Python 零 write_text .cpp/.h（带 BOUNDARY_OVERRIDE 的白名单除外）；cache key 包含版本信息 |
| deliverables | A1-A5 + B1-B2 全部完成 |
| dependencies | 无 |
| resolved_decisions | Assert stubs → SDK；SEH fix → Scriban；Interop stub → Codegen；Cache key 版本化 |
| watch_items | Codegen emitter 修改需 worktree 隔离验证 |

### Phase 2: 工程债务清理（Week 2）

| 字段 | 值 |
|------|-----|
| goal | 清理重复代码、死代码、废弃代码、路径重复 |
| exit_criteria | 所有 C1-C6 子任务完成，不引入功能回归 |
| deliverables | C1-C6 全部完成 |
| dependencies | Phase 1 完成（确保基线稳定后再清理） |
| resolved_decisions | tool_helpers 提取到 `_pipeline/`；Obsolete 代码确认无消费方后删除 |
| watch_items | 删除 Obsolete 代码前需确认无测试依赖 |

### Phase 3: AI 合规体系（Week 3）

| 字段 | 值 |
|------|-----|
| goal | 建立 Hook + pre-commit + CI 三层门禁；更新 CLAUDE.md |
| exit_criteria | 三层门禁全部生效，BOUNDARY_OVERRIDE 白名单完整，全量 pipeline 通过 |
| deliverables | D1-D4 + E1 全部完成 |
| dependencies | Phase 1+2 完成（需要在修复后的稳定架构上建立门禁） |
| resolved_decisions | Layer Permission Matrix 作为检查依据；BOUNDARY_OVERRIDE 需含 ticket link + 过期时间 |
| watch_items | pre-commit hook 不要太激进影响正常开发流程 |

## 5. 子任务映射

| task_id | phase | status | purpose | depends_on | batch_id | conflict_scope | estimated_effort |
|---------|-------|--------|---------|------------|----------|----------------|-----------------|
| A-1 | Phase 1 | planned | Assert stubs → SDK runtime_stubs/ | — | batch-a1 | chaos-sdk/ + TPG/Templates/ | 0.5d |
| A-2 | Phase 1 | planned | SEH fix + CHAOS_FACT_CHECK → Scriban | A-1 | batch-a1 | TPG/Templates/ | 0.5d |
| A-3 | Phase 1 | planned | Interop stub → Codegen emitter | — | batch-a3 | managed/Chaos.IL2CPP.Generator/ | 2d |
| A-4 | Phase 1 | planned | C# wrapper → ATG --generate-wrappers | — | batch-a3 | tools/AutoTestGenerator/ | 1.5d |
| A-5 | Phase 1 | planned | Flat layout copy hack 修复 | A-3 | batch-a3 | TPG/Emission/ | 0.5d |
| B-1 | Phase 1 | planned | Hephaestus cache key 版本化 | — | batch-b | verification/stages/hephaestus_cache.py | 1d |
| B-2 | Phase 1 | planned | 缓存合约文件 | B-1 | batch-b | verification/stages/hephaestus_cache.py | 0.5d |
| C-1 | Phase 2 | planned | 提取共享 tool_helpers | Phase 1 | batch-c | verification/stages/ + _pipeline/ | 0.5d |
| C-2 | Phase 2 | planned | 清理 Obsolete 模板代码 | Phase 1 | batch-c | Codegen/BuildSystem/ | 0.5d |
| C-3 | Phase 2 | planned | 清理 TPG server mode | Phase 1 | batch-c | tools/TestProjectGenerator/Program.cs | 0.5d |
| C-4 | Phase 2 | planned | 消除 CSProj 重复写入 | Phase 1 | batch-c | verification/stages/build.py | 0.25d |
| C-5 | Phase 2 | planned | 清理 .autogen/ 缓存目录 | Phase 1 | batch-c | verification/ | 0.25d |
| C-6 | Phase 2 | planned | CodegenOrchestrator 归属明确 | Phase 1 | batch-c | Codegen/Orchestration/ | 0.5d |
| D-1 | Phase 3 | planned | Pre-commit hook 边界检查 | Phase 1+2 | batch-d | .githooks/ | 0.5d |
| D-2 | Phase 3 | planned | CI pipeline 边界检查 | Phase 1+2 | batch-d | verification/preflight/ | 1d |
| D-3 | Phase 3 | planned | CLAUDE.md 架构层规范 | Phase 1+2 | batch-d | CLAUDE.md | 0.5d |
| D-4 | Phase 3 | planned | PR template + Code Review checklist | Phase 1+2 | batch-d | .github/PULL_REQUEST_TEMPLATE.md | 0.25d |
| E-1 | Phase 3 | planned | 全量 pipeline 验收测试 | D-1~D-4 | batch-d | — | 0.5d |

## 6. 依赖关系图

```
Phase 1 (Week 1)
  batch-a1: A-1 → A-2（串行，共用 Scriban）
  batch-a3: A-3 → A-5（串行，Codegen emitter → flat layout），A-4 并行
  batch-b:  B-1 → B-2（串行，cache 重构）

Phase 2 (Week 2)
  batch-c: C-1 至 C-6 全并行
  wait_for: Phase 1 completed

Phase 3 (Week 3)
  batch-d: D-1 至 D-4 + E-1 半并行（D-2 可先于 D-1 开始）
  wait_for: Phase 1+2 completed
```

## 7. 风险

| 风险 | 缓解 |
|------|------|
| Codegen emitter 修改 A-3 引入回归 | worktree 隔离 + 独立 verification chunk 验证 |
| Scriban 模板被 linter revert | 修改前确认 linter 规则 |
| Cache key 版本化后冷启动 | 仅在 codegen/TPG 变更时重构建 |
| ATG --generate-wrappers 与 Python 版本不一致 | 迁移后立刻删除 Python 旧代码 |
| BOUNDARY_OVERRIDE 过期无人清理 | CI 检查过期 >30 天自动 fail |

## 8. 备选路径

- **A-3 失败回退**: 如果 Codegen emitter 修改范围过大，回退到 Option A（全部进 Scriban），将 interop stub 注册逻辑改为 Scriban 模板中的固定模板代码
- **B-1 冷启动成本不可接受**: 改为在 pipeline-config.yaml 中声明 CACHE_VERSION，手动 bump

## 9. 推进建议

**串行阶段边界**: Phase 1 → Phase 2 → Phase 3
**并行批次内**: batch-a1 与 batch-a3 与 batch-b 三流完全并行
**推荐首个启动**: A-1（Assert stubs → SDK，最安全、影响面最小）

## 10. 子任务执行策略

- child_execution_mode: auto
- auto_continue: true
- auto_stop_policy: blocking-only
- recommended_next_child: A-1
