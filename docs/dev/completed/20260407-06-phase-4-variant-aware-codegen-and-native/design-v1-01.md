# Phase 4 Variant-Aware Codegen And Native Design v1.01

Date: 2026-04-07
Status: active

## 语义模型

- selector 输入扩展为 `subject / validationProfile / validation / variant / matrix`
- registry 暴露 capability 与默认 profile/validation/variant 的解析结果
- planner 产出的 plan request、selection、fingerprint 都写入 `variant`
- report / subject summary / matrix report 记录 selected 与 effective `variant`

## 执行链路

- `subject.manifest.json` 中每个 validation 继续声明 `defaultVariant`
- planner 先解析默认 `variant`，再应用 CLI override
- subject worker 把 `variant` 映射成 codegen 宏与 native 编译选项
- codegen/native 使用同一份变体定义，避免 CHECK/PROFILE/SHIP 语义漂移

## 产物与追踪

- run 级 canonical root 继续保持在 `artifacts/subjects/<id>/runs/<run-id>`
- 变体至少要进入 plan、matrix report 和 stage 结果元数据
- 如需变体专属目录，必须保持 subject-local、run-scoped，并避免绝对路径泄漏

## 关键验证

- `tests.unit.run.test_subject_planner`
- `tests.unit.run.test_fingerprints`
- `tests.unit.run.test_subject_workers`
- `tests.unit.run.test_subject_reporting`
- `tests.tooling.run.test_subject_command`
