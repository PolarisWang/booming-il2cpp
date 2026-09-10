# M5 — Interpreter 域分配泄漏修复 (R1+R3)

> **task_id**: m5-interpreter-leaks
> **parent_task_id**: memory-domain-industrialization
> **source_task_id**: memory-domain-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

修复 interpreter 路径上两个内存泄漏，使其遵守三域分配模型。

## Inputs

风险评估报告中的 R1 + R3：
- R1：fast_dispatch.cpp Handle_Box/Handle_NewObj/Handle_NewArr 使用 `::operator new` 而非 domain 分配，且无对应 delete
- R3：runtime_instantiation.cpp struct_data 通过 CHAOS_IL2CPP_MALLOC 分配后无释放路径

## Sub-tasks

### R1：fast_dispatch.cpp 泄漏修复
- Handle_Box / Handle_NewObj / Handle_NewArr 改为 domain 分配（`CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE` + placement new）
- 或使用 arena 分配器 + 帧销毁回收
- 验证：hotupdate 回退 interpreter 路径无累积泄漏

### R3：runtime_instantiation.cpp struct_ret_buf 泄漏修复
- struct_data 改为 domain 分配，或在 InterpreterValue::from_struct 中接管并释放
- 明确 struct_data 所有权契约

## Expected Outputs

- fast_dispatch.cpp 修改（R1）
- runtime_instantiation.cpp 修改（R3）
- 现有测试全部通过

## Exit Criteria

- R1 泄漏路径关闭
- R3 泄漏路径关闭
- Build 通过
- 现有 memory_domain 测试通过
