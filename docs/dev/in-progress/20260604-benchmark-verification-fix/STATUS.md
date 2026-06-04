# Benchmark Verification Mechanism Redesign

## Phase
brainstorming → writing-plans

## 边界拍板
- C++ 输出层加固 + 逐行 JSON（方案 A）
- 自适应迭代策略（方案 B）
- 输出格式 A1（每行一个完整 JSON 对象 + 总结行）

## Authority 决策
- C++ 层（TPG Scriban 模板）：输出格式、guard、异常路径信息
- Python 层（benchmark_chunk.py）：自适应迭代、自适应采样、解析逻辑适配
- Codegen 层：不涉及本次改动

## 当前结论
基准测试验证机制存在 6 个问题，分两组方案修复：
- **方案 A**：C++ 输出层改用逐行 JSON，增加 elapsed_ms == 0 guard，异常路径保留 allocatedBytes
- **方案 B**：Python 端自适应迭代（试测→自动提高 iterations）+ 自适应采样（CV<5% 停止）

## 风险评估
- 逐行 JSON 改变 stdout 协议格式，benchmark_chunk.py 的 JSON 解析器必须同步修改
- 自适应迭代增加每个方法的执行时间，大 chunk（5000+ entries）需要更保守的迭代上限
- 自适应采样在方法间耗时差异大时可能有的方法跑 3 轮有的跑 5 轮，统计口径不一致

## 三优先级权衡结论
- P1（性能）：不涉及翻译路径性能，只影响测试框架
- P2（架构）：输出协议标准化（逐行 JSON）是架构改善
- P3（热更新）：不涉及

## blocking_questions
- [x] 逐行 JSON 格式确认 → A1
- [x] 自适应迭代是否一起做 → 是
- [x] 影响基准测试性能数据的量化问题是否优先修 → 是

## question_clearance
cleared

## clearance_confirmed_by_user
true

## 关键文档
无额外设计文档。设计摘要即本 STATUS.md。

## 下一步入口
writing-plans：创建实现计划，按 C++ 输出层修复 → Python 解析层适配 → 自适应策略的顺序推进

## 设计摘要

### 方案 A：C++ 输出层 + 逐行 JSON

C++（TestProject.RuntimeEntry.cpp.scriban）：

1. RunBenchmarkAllMode 改为逐行 JSON：每方法一行完整 JSON 对象，
   每行末尾 \n，最后一行是 `{"summary":{...}}`
2. elapsed_ms == 0 时 opsPerSecond 输出 0（不是 inf，也不是除零）
3. 异常路径输出 allocatedBytes（warmup 通过后 alloc_after - alloc_before）
4. RunBenchmarkMode 同步修改
5. 移除 CHAOS_IL2CPP_LOG_LEVEL 日志剥离的依赖

Python（benchmark_chunk.py）：

1. 逐行读取而非整体 json.loads
2. 移除 `:inf → :1e308` hack
3. 移除 `]}` 修复尝试
4. `_strip_log_lines` 可以简化或移除（逐行模式下日志行不会混淆 JSON）

### 方案 B：自适应迭代 + 自适应采样

benchmark_chunk.py:

1. `_run_entry_once` 增加 probe 阶段：先用 10 iterations 试测
   - 若 total elapsed < 50ms → 提高 iterations 到 `ceil(50ms / per_call_ms)`
   - 上限 50000 iterations
   - 大 chunk（>5000 entries）上限 10000
2. 采样轮数自适应：在 3-10 轮范围内，当最近 3 轮 CV < 5% 时提前停止
3. Warmup 增加：每个方法在 RunBenchmark 的 warmup 阶段至少 100 次调用
