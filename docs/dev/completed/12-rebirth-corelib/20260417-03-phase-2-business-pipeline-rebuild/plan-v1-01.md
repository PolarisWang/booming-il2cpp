# Phase 2 Business Pipeline Rebuild 实施计划 v1.03

Date: 2026-04-17 15:18:40 +08:00
Status: in-progress

## 计划清单

- [x] 梳理 planner / registry / workspace 对旧 manifest 测试字段的真实依赖
- [x] 新增 contract test，锁定业务消费层的目标输入面
- [ ] 继续切 planner / registry / workspace / run pipeline 到 collection + `subject.features.json` + 精简 manifest
- [ ] 对齐 reporting / dashboard 的 capability-first / declared-case-first 消费模型
- [ ] 更新 parent roadmap / ACTIVE / INDEX / progress note，并准备 Phase 3 输入

## 当前切入点

- 已完成 `planner / registry / benchmark_dashboard_generator / project_workspace / perf_dashboard` 的第一批 legacy cutover。
- 当前优先处理 `subject_workers.py` 与 reporting 链路里剩余的 `workloadEntry` / `entrySelection` 业务真源依赖。

## 本轮已完成

- `subject_planner.py`
  - `declared-benchmark` 无显式 override 时不再隐式回读 manifest `workloadEntry`
  - `declared-benchmark` 不再把 manifest `source.entrySelection` 混入默认选择
- `registry.py`
  - subject registry item 删除 `displaySubjectEntrySelection`
  - subject registry item 删除 `displayWorkloadEntry`
- `benchmark_dashboard_generator.py`
  - summary workload/case 改为 declared benchmark case 优先
- `project_workspace.py`
  - workspace Windows native proof host 改成模板物料化
  - 不再依赖 `subjects/*/validation/proof/native-reference/main.cpp`
- `perf_dashboard.py`
  - summary workload/case 改成 declared benchmark case 优先，manifest 仅回退
- `subject_workers.py`
  - collection-backed structured benchmark host 已不再透传 `--workload-entry`
  - managed/interpreter benchmark host 改为只消费 `collectionPath + entryIndex (+ binding manifest)`
  - `native dispatch manifest` 已不再写入 `workloadEntry`

## 下一刀

- 盘点 `subject_workers.py` 中所有剩余 `workloadEntry` / `entrySelection` 写入点
- 区分哪些字段仍参与真实运行路由，哪些只是证据/展示输出
- 继续审计非结构化反射 fallback 是否可完全改成 declared metadata
- 明确 runtime/result manifest 中哪些 `workloadEntry` 保留为 trace-only 字段
