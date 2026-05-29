# 优化记录索引

| 日期 | 标题 | Family | 优化前 | 优化后 | 加速比 | 备注 |
|------|------|--------|--------|--------|--------|------|
| 2026-05-28 | convert-char 已收敛 | convert-char | AOT 平均 2.45ns/op | 与优化前一致（已收敛） | 1.0x | AOT 比 .NET 8 快 21.53%，无需优化 |
| 2026-05-30 | boxing-unboxing-casts 分析 | boxing-unboxing-casts | 8/11 方法优于 .NET 8 | 无 codegen 变更 | 见文档 | 3 个 Type.* 方法超限（runtime 限制），详见 blocker.md |
