---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-C1
lifecycle_status: active
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-C1: enum-parsing 瓶颈深潜

## Scope
深度分析 enum-parsing 为什么 chaos-aot 比 .NET 8 慢 10.7x，识别具体的瓶颈路径。

## 已有数据
- enum-parsing 13 methods, chaos-aot = 602ns, net8-jit = 56ns
- 最慢的方法：Enum.GetName (2506ns, 133x), Enum.ToString(String) (1455ns, 186x), Enum.Format (1540ns, 76x)
- ROI 门禁分类为 metadata-bound

## 分析内容

1. **读取 profile scope 数据**：
   - 如果有 profile 输出，提取 hot path 的耗时分布
   - 识别哪些 handler/function 占用最多时间

2. **代码分析**：
   - 查看 codegen 为 Enum 方法生成的 C++ 代码
   - 检查是否走了 interpreter 路径而不是 native dispatch
   - 检查是否有大量 metadata lookup 调用

3. **对比分析**：
   - chaos-aot vs chaos-jit vs net8-jit 的 per-method 数据
   - 高 slowdown 的方法是否有共同模式

4. **输出瓶颈报告**：
   写入 `testing/foundation-dll/System.Private.CoreLib/enum-parsing/bottleneck-analysis.md`
   包含：
   - 瓶颈类型和根因
   - 每个方法的 breakdown
   - 建议的优化方向和预期收益

## 预期输出
- `enum-parsing/bottleneck-analysis.md`（瓶颈分析报告）

## 分析起点
```bash
# 查看 enum-parsing codegen 输出
ls testing/foundation-dll/System.Private.CoreLib/enum-parsing/codegen/

# 查看 per-method 数据
python -c "
import json
mr = json.load(open('testing/foundation-dll/System.Private.CoreLib/enum-parsing/multi-run/multi-run-report.json'))
for m in mr['methods']:
    ca = m['samples'].get('chaos-aot', {}).get('mean_ns', 0)
    nj = m['samples'].get('net8-jit', {}).get('mean_ns', 0)
    ratio = ca/nj if nj > 0 else float('inf')
    print(f'{m[\"subject_id\"][:60]:60s} AOT={ca:>8.2f}ns NET8={nj:>8.2f}ns R={ratio:>6.1f}x')
"

# 查看 unified-verification-report.json
cat testing/foundation-dll/System.Private.CoreLib/enum-parsing/unified-verification-report.json | python -m json.tool | head -100
```

## STATUS.md 路径
`D:/agent/booming-il2cpp/docs/dev/in-progress/20260530-optimization-campaign-reboot/children/t-C1-enum-bottleneck/STATUS.md`
