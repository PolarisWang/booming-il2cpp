# Phase 4 Native AOT Emitter And Scriban Template Widening Plan v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## Checklist

1. 让 `NativeAotEmitter` 读取 widened lowering plan，并构建稳定的 Scriban model。
2. 为 translation unit、object model、method body 三段输出补齐模板。
3. 保证 widened native shape 不再走 emitter 内联字符串兜底。
4. 用 Scriban / native AOT artifact 测试锁定输出。

## 验收标准

- 新 shape 的 native AOT 代码生成只走 Scriban。
- emitter widening 与 planner widening 的接口稳定。
