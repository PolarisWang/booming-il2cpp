# Phase 3 Generic Byref Mixed Dispatch Closure Plan v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## Checklist

1. 扩 generic + byref + value-type mixed dispatch 的 reachable-method / slot 解析。
2. 扩 linker receiver tracking，消掉 mixed dispatch 路径中的隐式假设。
3. 把 generic/interface/virtual mixed dispatch 的 owner proof 与 native AOT RED 锁定到 compatibility 测试。
4. 确认 mixed dispatch widening 能被 Phase 4 的 emitter / Scriban 稳定消费。

## 验收标准

- generic mixed dispatch 不再依赖 legacy shell 或 subject 特判。
- interface / virtual dispatch 的关键路径能够给出稳定 native AOT lowering 结果。
