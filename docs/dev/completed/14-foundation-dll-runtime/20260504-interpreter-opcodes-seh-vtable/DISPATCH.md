# DISPATCH.md — 调度总纲

dispatch_model: hybrid
parallel_merge_policy: independent-completed

## 批次定义

batches:
  - batch_id: batch-1
    mode: parallel
    description: "解释器 opcode 补全 + 泛型虚方法派发 — 无文件冲突，可并行"
    tasks:
      - id: interpreter-opcodes
        conflict_check: no-shared-output
        conflict_scope:
          - src/native/interpreter/interpreter_vm.cpp
          - src/native/runtime-core/il_to_ir_lowerer.cpp
          - src/native/interpreter/interpreter_vm.h (可能)
          - tests/contracts/native/interpreter/interpreter_integration.cpp
      - id: generic-vtable-dispatch
        conflict_check: no-shared-output
        conflict_scope:
          - src/native/runtime-core/vtable_registry.*
          - src/native/runtime-core/runtime_instantiation.* (可能)
    wait_for: []
    merge_after: all-completed

  - batch_id: batch-2
    mode: sequential
    description: "SEH 异常处理纯状态机 — 依赖 opcode 覆盖完成"
    tasks:
      - id: seh-exception-handling
        conflict_check: shared-with-batch1
        conflict_scope:
          - src/native/interpreter/interpreter_vm.cpp
          - tests/contracts/native/interpreter/interpreter_integration.cpp
    wait_for: [batch-1]
    merge_after: all-completed

## 合并协议

batch-1 合并：
1. 确认两个子任务均完成并已移入 completed/
2. 运行全部集成测试确认无回归
3. 更新父 STATUS.md 的 `active_batches` / `completed_batches`
4. 检查 batch-2 的 wait_for 条件是否满足
5. 满足后自动推进到 batch-2

batch-2 合并：
1. 确认 SEH 子任务完成并移入 completed/
2. 运行全部集成测试
3. 更新父 STATUS.md 标记 roadmap 完成
4. 进入收尾链路：结构告警 → 测试 → wiki 更新
