# T-A5 — known-fail 数据库带 expiry

## Scope

给 known-failures 基线添加 expiry 机制，使过期的已知失败自动从基线中移除（而非人工手动清理）。

## 现状调查

- `stale_known` 检测已存在：`test_driver.py` 自动检测已知失败测试通过的事件，在报告中标记 `stale_known`
- `ghost_known` 检测已存在：检测已知失败测试不存在（被重命名/移除）的事件
- **但两者均不自动从基线移除**——只报告，需人工 prune
- 当前 known-fail 条目仅 6 个，每个都有详细根因注释

## 决策

**建议延后至 T-A5 被实际需要时再实施**。理由：
1. 当前 `stale_known` 机制已有效阻止已知失败被遗忘（每次跑都报告）
2. 6 个条目量小，人工 prune 成本低
3. 不建议在沙箱中给 `known-failures.integration.yaml` 加 expiry 语法——会改变驱动解析逻辑，可能引入回归

## 延期后的 Exit Criteria

- 移除 `roadmap-v1-01.md` 中 T-A5 的依赖（不影响 G-P1 gate）
- 转至 `watch_items`：当 `stale_known` 数量 > 3 时，触发人工 prune

## 元信息

- `status: deferred`（不阻塞 P1 门禁）
- `deferred_to: PRUNE-ON-STALE (watch_item)`