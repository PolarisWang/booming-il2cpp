# Phase 2 Entry ABI Normalization And Planner Widening Plan v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## Checklist

1. 把 widened parameter / return carrier 引入 planner ABI model。
2. 统一 entry、invocation wrapper、dispatch wrapper 的 ABI 规划入口。
3. 移除对 legacy fallback ABI 的隐式依赖。
4. 用 AOT Core IR 与 native AOT compatibility 测试锁定 planner widening。

## 验收标准

- planner 能直接消费 widened method shape。
- ABI widening 失败点从“无模型”收缩为“明确 unsupported opcode/shape”。
