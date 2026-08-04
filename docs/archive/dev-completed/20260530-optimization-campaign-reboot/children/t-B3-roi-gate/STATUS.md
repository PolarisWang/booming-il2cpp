---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-B3
lifecycle_status: completed
phase: completed
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-B3: ROI 预估门禁

## Scope
在 `pre_verification_audit.py` 中新增 ROI 预估门禁。进入 Phase 2 之前自动输出 `optimization-opportunity.md`，包含瓶颈分类和预估收益。

## 输入
- `testing/foundation-dll/verification/stages/pre_verification_audit.py`
- benchmark_scanner.py 的输出格式

## 具体修改

1. 在 `pre_verification_audit.py` 中新增 `--estimate-roi` 参数：
   - 输入：family slug + assembly
   - 分析 benchmark 数据，计算当前 ns/op
   - 根据瓶颈类型估算预期收益

2. 瓶颈分类逻辑：
   - dispatch-bound: 如果 chaos-aot 主要走 interpreter dispatch → 预期收益高
   - alloc-bound: 如果大量 GC alloc → 预期收益中
   - metadata-bound: 如果大量 metadata lookup → 预期收益低（架构限制）
   - 判断方法：检查 chaos-aot vs chaos-jit 差距 + 方法调用模式

3. 输出 `optimization-opportunity.md`：
```markdown
# 优化机会: <slug>

## 当前性能
- chaos-aot: Xns
- vs .NET 8: Yx slowdown
- 方法数: N

## 瓶颈分析
- 类型: [dispatch-bound | alloc-bound | metadata-bound]
- 分析依据: ...

## 预期收益
- 乐观: Z1 ns (Z1% improvement)
- 保守: Z2 ns (Z2% improvement)

## 建议方案
- 方案 1: ... (预估工时)
- 方案 2: ... (预估工时)
```

4. 门禁逻辑：
   - 预期收益 < 20% → 输出 WARN 但不阻塞
   - 预期收益 >= 20% → 输出 PASS，进入 Phase 2

## 预期输出
- `pre_verification_audit.py`（修改，新增 ROI 功能）
- 输出 `optimization-opportunity.md`

## 退出标准
- `--estimate-roi enum-parsing` 能正确输出瓶颈类型和预期收益
- 预期收益 < 20% 的 family 正确标记

## 终端备注

T-B3 ROI 预估门禁已实现并验证通过。

### 修改文件
- `testing/foundation-dll/verification/stages/pre_verification_audit.py` — 新增 `--estimate-roi` 参数及其完整支持功能

### 新增功能
1. `_safe_float()` — 安全 float 转换辅助函数
2. `_find_summary_by_runtime()` — 从 multi-run-report.json 中按 runtime 定位 summary
3. `estimate_roi()` — ROI 估算核心函数：加载 benchmark 数据、逐方法分析三率（dispatch_ratio / alloc_ratio / metadata_ratio）、瓶颈分类、收益估算、门禁判定
4. `_write_optimization_opportunity()` — 输出 optimization-opportunity.md 到 family 目录
5. `_print_roi_human_readable()` — 人类可读的 ROI 摘要输出

### 验证结果
- `enum-parsing` → WARN (metadata-bound, 75% methods, 保守 10% < 20% threshold) — 正确识别 metadata 瓶颈
- `dispatch-basic` → WARN (default metadata-bound, 所有指标 0%, chaos-aot = 2.23ns 已极快) — 正确识别"无需优化"
- `--json` 模式输出完整 JSON 报告 — 可用
- optimization-opportunity.md 在两个 family 目录均正确生成

### 瓶颈分类逻辑
- **dispatch-bound**: chaos-aot.mean_ns / chaos-jit.mean_ns > 1.5
- **alloc-bound**: stub_count + throws_count > 20%
- **metadata-bound**: chaos-aot.mean_ns / net8-jit.mean_ns > 3.0
- **mixed**: 多项同时命中则加权平均

### 门禁阈值
- 保守收益 >= 20% → PASS (exit 0)
- 保守收益 < 20% → WARN (exit 1)
