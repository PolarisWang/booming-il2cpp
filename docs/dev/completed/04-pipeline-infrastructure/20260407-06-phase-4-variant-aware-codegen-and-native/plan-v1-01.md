# Phase 4 Variant-Aware Codegen And Native Plan v1.01

Date: 2026-04-07
Status: active

## Tasks

- [ ] 先用 TDD 扩展 planner/fingerprint 测试，锁定默认 `variant` 与显式 override 行为
- [ ] 扩展 selector / registry / planner，把 `validationProfile -> validation -> variant` 解析成正式 plan 语义
- [ ] 把 `variant` 贯穿到 subject worker、codegen/native 宏定义与 report 元数据
- [ ] 更新 tooling/tests，继续清理 `tests/` 中残留的 subject 语义耦合
- [ ] 记录本阶段结果并回写父 roadmap 状态

## Verification

- [ ] `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_fingerprints`
- [ ] `python -m unittest tests.unit.run.test_subject_workers tests.unit.run.test_subject_reporting`
- [ ] `python -m unittest tests.tooling.run.test_subject_command`
- [ ] `python -m unittest tests.integration.run.test_reporting_layout`
