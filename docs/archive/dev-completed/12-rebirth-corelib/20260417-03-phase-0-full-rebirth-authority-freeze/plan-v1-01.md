# Phase 0 Full Rebirth Authority Freeze 实现计划 v1.01

Date: 2026-04-17 13:03:34 +08:00
Status: in-progress

## 计划清单

- [x] 对 `subject.manifest.json`、collection、registry、planner、dashboard 做窄范围 authority audit
- [x] 写出 Phase 0 权威冻结设计，明确三类 truth source 的职责边界
- [x] 把 `subject.features.json` 作为仓库级权威种子资产引入 3 个 canonical subject
- [x] 新增 contract test，锁住最小 scenario 集合与 `subject.features.json` 的存在性
- [x] 更新父任务 / 子任务 `STATUS.md`、`ACTIVE.md`、索引与 progress note

## 本阶段输出

- `docs/dev/in-progress/20260417-03-phase-0-full-rebirth-authority-freeze/design-v1-01.md`
- `docs/dev/in-progress/20260417-03-phase-0-full-rebirth-authority-freeze/STATUS.md`
- `subjects/*/subject.features.json`
- `tests/contracts/shared/test_subject_authority_freeze.py`

## 下一阶段入口

- 将 `capability_coverage.py` 的硬编码责任迁移到 `subject.features.json`
- 统一 collection 的 Python/Runtime 契约
- 重建 generated managed/native/hotupdate host contract
- 让 Phase 1 之后的 planner / registry / dashboard 逐步脱离旧 manifest 字段
