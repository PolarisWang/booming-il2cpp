# 2026-04-15 14:13:26 +08:00 Phase 4C start

## 本轮目标

- 接管父 roadmap 的 `Phase 4C`
- 先盘点 retained generic owner proof 与 metadata closure 缺口
- 建立首批 RED 测试，冻结 generic context / instantiation / sharing / metadata closure 的 typed contract

## 约束

- Native AOT 后续改动必须沿用 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban`
- `Chaos.TestFramework` 不得混入 `AotCoreIr` / planner / emitter
- 不扩大 string protocol

## 下一步

- 检查 retained generic feature slice 与现有 compatibility tests
- 确定首批 `test_phase4c_*` RED 测试文件
