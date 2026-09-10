# TestFramework 架构治理

## phase
`completed`

## 完成摘要

全部 roadmap 4 个阶段已执行完毕：

| Phase | 状态 | 说明 |
|-------|------|------|
| TF-01 | ✅ | SDK: 新增 [Fact]/[Benchmark]/[HotUpdate] annotation，Assert 合并 ChaosAssertState |
| TF-02 | ✅ | Runtime: 合并 Runner + PerfHarness，单 Exe 入口，反射扫描 annotation 调度 |
| TF-03 | ✅ | Python 管线清理：删除 6 个 .tmpl 模板、2 个 generated_hosts.py、benchmark.py managed harness 生成 |
| TF-04 | ✅ | 下游引用更新：65 个 NativeEntry.csproj 移除 Runner 引用，family_entrypoint.py 清理 |

### 最终架构

两件套架构：`SDK (Library) + Runtime (Exe)`，从 4 个项目、~30 个 .cs 精简到 2 个项目、~6 个 .cs。

### 架构映射

- `src/reference/Chaos.TestFramework.Sdk/` — annotation 定义 + Assert API
- `src/reference/Chaos.TestFramework.Runtime/` — 唯一 Exe 入口（合并 Runner + PerfHarness）
- `src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/` → 已删除
- `src/reference/Chaos.TestFramework.Runner/` → 已删除
