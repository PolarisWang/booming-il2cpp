# Phase 4 Variant-Aware Codegen And Native Brainstorm v1.01

Date: 2026-04-07
Status: active

## 已确认约束

- `CHECK`、`PROFILE`、`SHIP` 是正式 `variant`，不能折叠进 `matrix`。
- `validationProfile` / `validation` 决定默认 `variant`，但 CLI 可以显式 override。
- `codegen` 就要带入 `variant` 宏定义，后续 native 编译选项必须消费同一组变体语义。
- correctness/proof 走 `CHECK`，性能测试走 `PROFILE`，发布产物走 `SHIP`。
- traceability 需要在 plan、report、artifact 元数据里显式看到 selected/effective `variant`。

## 本阶段目标

- 先把 `variant` 接入 planner、selector、fingerprint。
- 再把 `variant` 接入 subject worker / codegen / native / report 元数据。
- 保持现有 `goal` 仅为兼容层，不再继续承载正式变体语义。

## 非目标

- 本阶段不完成最终 wiki 交付。
- 本阶段不一次性清理全部旧 CLI 别名；那属于 `Phase 5/6`。
