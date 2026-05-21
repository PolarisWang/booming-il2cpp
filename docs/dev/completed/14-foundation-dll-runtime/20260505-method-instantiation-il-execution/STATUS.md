---
task_id: method-instantiation-il-execution
title: 方法泛型实例化 IL 执行 — IL→IR Lowering + 延迟集成
task_type: plan
lifecycle_status: completed
phase: B
created_at: 2026-05-04 20:00:00 +08:00
updated_at: 2026-05-04 20:00:00 +08:00
parent_task_id: 20260504-generic-runtime-instantiation-phases-3-5
source_task_id: roadmap-v1-01.md
source_relation: roadmap-child
clearance_source: parent-roadmap
---

## Scope

实现泛型方法运行时 IL→IR 降低引擎，并与 Phase B2 方法泛型 metadata 集成。

**边界**：
- IL opcode 子集（~30 个核心 opcode）：nop, ldarg, ldloc, stloc, starg, ldc.i4, ldc.i8, ldnull, add, sub, ceq, call, ret, br, brtrue, brfalse, beq, blt, bgt, ble, bge, newobj, box, leave, endfinally
- 两遍算法：第一遍解码为 RawInsn（含 IL offset 跟踪），第二遍解析分支目标 （relative offset → absolute index）和 metadata token
- Token resolver 回调抽象（ILTokenResolver）
- 延迟降低模式：LowerMethodBody() 在首次调用时降低并缓存
- 不包含完整异常处理（SEH）支持

## 交付物

- `src/native/runtime-core/il_to_ir_lowerer.h` (new)
- `src/native/runtime-core/il_to_ir_lowerer.cpp` (new) — 完整 IL→IR 降低实现
- 修改 `generic_method_instantiation.h` — 添加 il_bytes/il_length 字段、LowerMethodBody 声明、ir_method_body 类型修正
- 修改 `generic_method_instantiation.cpp` — 实现 LowerMethodBody（placement new 缓存）
- 修改 `CMakeLists.txt` — 添加 il_to_ir_lowerer.cpp/.h、添加 interpreter include 路径
- `tests/unit/compatibility/test_method_instantiation_il_execution.py` (new, 46 tests)

## Exit Criteria

- ✅ 46/46 兼容性测试通过
- ✅ IL→IR lowerer 支持核心 opcode 子集
- ✅ 两遍解码/分支解析算法正确
- ✅ Token resolver 回调抽象存在
- ✅ LowerMethodBody 延迟降低/缓存模式实现
- ✅ RuntimeInstantiatedMethod 结构包含 IL bytecode 存储字段
- ✅ CMakeLists.txt 包含 il_to_ir_lowerer.cpp 和 interpreter include 路径

## 设计要点

1. **ILTokenResolver**: `bool (*)(uint32_t token, IRInstruction& instruction, void* user_data)` — 调用方提供的回调，负责把 metadata token 翻译为 IRInstruction 字段（call_target, field_offset 等），支持泛型类型参数替换
2. **两遍降低**: Pass 1 将 IL 字节码解码为 RawInsn 数组（保留 IL offset）；Pass 2 计算分支目标绝对索引并调用 token resolver
3. **ECMA 335 opcode 尺寸表**: kOpcodeSizes[0x100] + kOpcodeSizesExtended[0x100] 覆盖单字节和 0xFE 前缀双字节 opcode
4. **LowerMethodBody**: 若 ir_method_body 非空则直接返回（缓存命中）；否则调用 LowerILToIR 后通过 placement new 堆分配 IRMethod
5. **延迟集成**: ir_method_body 缓存在 RuntimeInstantiatedMethod 中，cleanup 推迟到后续阶段

## Terminal Notes

B3 作为 roadmap child 自动推进，46 个兼容性测试全部通过。
所有 5 个 roadmap 子任务完成，roadmap 可进入终态。
