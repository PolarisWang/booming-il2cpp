---
task_id: marshal-industrialization
task_type: roadmap
phase: executing
lifecycle_status: in-progress
roadmap_or_plan: roadmap-v1-01.md
design_doc: n/a (design summary in STATUS.md)
arch_review_mode: critical
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
clearance_source: brainstorm-design-confirmed
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
recommended_next_child: d1-internal-header
latest_stop_point: c1-struct-marshal-test (已完成)
---

# P/Invoke & Marshal 工业化完善

## 设计摘要

### 边界拍板
- 覆盖范围：5 marshal .cpp + 3 COM .cpp (com_rcw, com_ccw, com_connection_point) + 内嵌 variant 实现
- 不覆盖：COM wiki 已有独立测试计划的场景特定测试
- 测试基础设施复用 BootstrapTestFixture 模式（每个测试进程独立 GC 初始化）

### 结构决策
1. Unity build → 独立 TU（创建 marshal_internal.h 提取关键符号，先拆编译再物理搬迁）
2. struct_marshal 创建独立公开头文件 struct_marshal.h
3. ICustomMarshaler 缓存使用 std::atomic_flag 自旋锁修复并发安全问题

### 测试策略
- 每个测试用例 = 正确性断言 + benchmark 性能锚点
- 双模式：ctest -L marshal（正确性），ctest -L marshal-benchmark（性能基线检查）
- COM 测试提供 BSTR mock（非 Win32），真实 GC 堆

## 三优先级权衡结论

| 决策 | P1 (性能) | P2 (架构) | P3 (HotUpdate) | 裁决 |
|------|-----------|-----------|----------------|------|
| Unity build → 独立 TU | 无影响 | 显著改善 | 无影响 | 明确收益 |
| 自旋锁保护 ICache | 1次原子操作/cache miss | 消除数据竞争 | 支持 | 必须做 |
| 先测试后重构 | 无影响 | 保证重构安全 | 无影响 | Bootstrap 已验证 |
| struct_marshal 头文件 | 无影响 | 消除临时手法 | 无影响 | 干净架构 |

## 风险评估摘要

| 风险 | 级别 | 缓解 |
|------|------|------|
| Unity build 拆分符号暴露 | 中 | 先验证提取再改 CMake |
| 并发修复死锁 | 低 | 单标志自旋锁，无嵌套调用 |
| Managed 测试执行时间长 | 中 | 仅收口验证，不阻塞早期 phase |

## 最近摘要

a1-test-fixture (已完成): marshal_test_fixture.h 创建，7/7 smoke 测试通过。
a2-test-stubs (已完成): marshal_test_stubs.cpp 已创建并通过编译链接。
b1-alloc-test (已完成): marshal_alloc_test.cpp 创建，8/8 alloc/free/read/write 测试通过。
b2-string-test (已完成): marshal_string_test.cpp 创建，11/11 字符串编码转换测试通过。
b3-api-basic-test (已完成): marshal_api_basic_test.cpp 创建，12/12 marshal API 基础测试通过。
c1-struct-marshal-test (已完成): struct_marshal_test.cpp 创建，14/14 结构体编组描述符测试通过。

## 下一步

启动 d1-internal-header：创建 marshal_internal.h 提取关键符号，为 Unity build → 独立 TU 做准备。

