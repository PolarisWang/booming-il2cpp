# TestFramework 架构治理 — Roadmap

## 目标

将 TestFramework 从当前 4 个项目、~30 个 .cs 文件中重构为极简的 2 项目架构（SDK + Runtime），实现 annotation 驱动、Python 代码生成清零、三层职责归并。

## 范围边界

- `src/reference/Chaos.TestFramework.*` — SDK 和 Runtime 的重构
- `src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness` — 删除
- `testing/foundation-dll/verification/stages/benchmark.py` — 删除 managed harness 生成
- `build/toolchains/run/testing/templates/managed-proof-host.cs.tmpl` — 删除
- `build/toolchains/run/testing/templates/hotupdate-proof-host.cs.tmpl` — 删除
- `testing/foundation-dll/**/*.csproj` 中引用 Runner 的更新为 SDK

## 非目标

- 不改变 native C++ 侧的 dispatch 逻辑
- 不改变 TPG 的 .scriban 模板生成逻辑（TPG 后续单独改读 annotation）
- 不涉及 codegen 的修改

## 三优先级确认

P1（性能最优）和 P3（热更新支持）不直接受本重构影响。P2（方案完美性）是本重构的核心驱动力。

## 阶段列表

| Phase | 名称 | 模式 | 依赖 |
|-------|------|------|------|
| 1 | SDK 重构 — annotation + Assert 统一 | 串行 | 无 |
| 2 | Runtime 重写 — 合并 Runner + PerfHarness | 串行 | Phase 1 |
| 3 | Python 管线清理 + Template 删除 | 串行 | Phase 1, 2 |
| 4 | 下游引用更新 | 串行 | Phase 1, 2, 3 |

## 子任务映射

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|------------|----------------|-----------------|
| TF-01 | 1 | ready | SDK: 新增3个annotation, 重写Assert合并ChaosAssertState | — | `src/reference/Chaos.TestFramework.Sdk/` | small |
| TF-02 | 2 | planned | Runtime: 合并Runner+PerfHarness, 删除无用代码 | TF-01 | `src/reference/Chaos.TestFramework.Runtime/` | medium |
| TF-03 | 3 | planned | 删除Python managed harness生成, 删除template | TF-01, TF-02 | `testing/foundation-dll/verification/stages/benchmark.py` | small |
| TF-04 | 4 | planned | 更新foundation-dll下游csproj引用 | TF-01, TF-02 | `testing/foundation-dll/**/*.csproj` | large |

## 依赖图

```
TF-01 (SDK) → TF-02 (Runtime) → TF-03 (Python) → TF-04 (下游)
```

## 风险

| 风险 | 缓解 |
|------|------|
| TF-04 涉及 154 个 .csproj，手动修改易遗漏 | 用脚本批量替换，grep 验证 |
| ChaosAssertState 缺失导致 TF-04 前编译失败 | TF-01 立即修复此问题 |
| Runtime 重构后 API 不与 foundation-dll 原有用法兼容 | TF-04 统一替换 |

## 调度策略

dispatch_model: `sequential`

child_execution_mode: `auto`
auto_continue: `true`
auto_stop_policy: `blocking-only`
