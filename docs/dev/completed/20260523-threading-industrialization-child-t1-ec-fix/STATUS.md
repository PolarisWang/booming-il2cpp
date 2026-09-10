# T1-EC-FIX: 修复 EC EnsureHeapCapacity 忽略失败

> **Phase**: writing-plans (preflight)
> **parent_task_id**: 20260523-threading-industrialization
> **source_task_id**: T1-EC-FIX
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true
> **entry_skill**: dev-writing-plans

---

## Scope

修复 `execution_context.cpp:219-231` 中 `ExecutionContextRun` 调用 `EnsureHeapCapacity` 后忽略返回值的 bug。

当线程上有 5+ AsyncLocal 值时，`ExecutionContextRun` 安装 context 时如果 heap 扩容失败，`ValueAt()` 访问越界堆内存，导致 segfault 或静默内存损坏。

## Inputs

- `src/native/runtime-core/execution_context.cpp` — 需要修复的文件
- `src/native/runtime-core/execution_context.h` — 可能需要调整的接口定义
- `testing/src/native/runtime-core/threading/execution_context_smoke_test.cpp` — 现有 EC 测试

## Expected Outputs

- 修复后的 `execution_context.cpp`
- 补充 TDD 测试（测试 5+ AsyncLocal 路径）
- 全量 threading 测试通过

## Exit Criteria

1. TDD: 先写一个会触发 5+ AsyncLocal 值并执行 `ExecutionContextRun` 的失败测试
2. 修复实现
3. 测试通过（不再 segfault）
4. 现有 threading smoke + benchmark + EC 测试全部 pass
5. Wiki 同步更新

## Estimated Effort

0.5 天
