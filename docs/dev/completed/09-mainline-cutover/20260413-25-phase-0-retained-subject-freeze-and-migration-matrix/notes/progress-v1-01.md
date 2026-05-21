# 进度记录 v1.01

Date: 2026-04-13 08:57:38 +08:00

## 本次完成

- 创建 Phase 0 子任务执行骨架
- 切换 `docs/dev/ACTIVE.md` 到本子任务
- 盘点当前 28 个顶层 `subjects/`
- 盘点当前 `tests/` 顶层 taxonomy
- 冻结方案 A 下的 retained subject 清单
- 冻结顶层 subject 的 executable plan 准入规则
- 输出全量 `legacy-to-retained` 迁移矩阵
- 明确 `compatibility alias` 最终必须一次性清零

## 关键结论

- retained subject 长期只保留三类：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 其余顶层 root 全部视为迁移源，不再保留长期 canonical 地位
- `GenericEcho`、`CompatibilityMatrixProof`、`MobileHelloWorldProof` 是实施阶段必须逐条重判的边界项
- `tests/perf/convert-golden-multi-project` 必须迁回 `SolutionCorePack` 的 engineering workload 语义

## 下一步

- 进入 Phase 1
- 开始显式 `.sln` 升级
- 验证现有 loader / resolver / planner 对 solution source 的支持缺口
