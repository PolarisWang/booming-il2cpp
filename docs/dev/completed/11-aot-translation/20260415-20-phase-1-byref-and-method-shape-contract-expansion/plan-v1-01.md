# Phase 1 Byref And Method Shape Contract Expansion Plan v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## Checklist

1. 扩 loader 的 metadata decode，补齐 function pointer、generic method reference、widened signature 的输入面。
2. 扩 `AotCoreIr` 需要的 method shape / carrier 建模。
3. 把新 shape 接入 typed-il 到 `AotCoreIr` 的 lowering 流程。
4. 用 loader / IR compatibility 测试锁定 contract。

## 验收标准

- widened method shape 不再在 loader / IR 阶段报“未知形状”。
- 后续 planner widening 可以直接依赖 `AotCoreIr`，不需要回头补 metadata 特例。
