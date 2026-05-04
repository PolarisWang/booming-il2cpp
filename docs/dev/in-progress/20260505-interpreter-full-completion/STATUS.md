---
task_id: 20260505-interpreter-full-completion
title: 解释器能力补齐 + RuntimeInstantiationBridge V1
task_type: plan
lifecycle_status: in-progress
phase: implementation
created_at: 2026-05-05
updated_at: 2026-05-05
---

# 解释器能力补齐 + RuntimeInstantiationBridge V1

## 批次划分

### Batch 1: 修复 opcode 映射 + 添加缺失 opcodes
**文件**: `il_to_ir_lowerer.cpp`（kOpcodeSizes 表 + switch）+ `interpreter_vm.h`（新 IROpCodes）+ `interpreter_vm.cpp`（VM case）
- 修复 kOpcodeSizes[0x13-0x15]  operand size bug
- 修复 switch 中 0x12(stloc.s)、0x13(ldnull)、0x14(ldc.i4.0)、0x15(ldc.i4.1) 位置错误
- 添加 unsigned 分支：BneUn, BgeUn, BgtUn, BleUn, BltUn（short + long）
- 添加 Break（NOP）
- 添加 EndFilter（已在 enum，补 lowerer）
- 添加 ConvOvfI, ConvOvfI4, ConvOvfI8, ConvOvfU, ConvOvfU4, ConvOvfU8
- 添加 AddOvf, SubOvf, MulOvf
- 添加 LdObj, StObj（与现有 LdInd/StInd 区分）
- 添加 LdElemA

### Batch 2: 接口派发 + Constrained prefix
**文件**: `vtable_registry.*` + `interpreter_vm.cpp` + `il_to_ir_lowerer.cpp`
- Interface vtable dispatch for CallVirt
- Constrained prefix (0xFE16) 在 CallVirt 前设置标志
- 调用侧在 VM 中处理 constrained CallVirt → 直接调用而非虚派发

### Batch 3: Cpblk/Initblk + 收尾 opcodes
**文件**: `il_to_ir_lowerer.cpp` + `interpreter_vm.cpp`
- Cpblk 内存拷贝
- Initblk 内存初始化（填零）
- Memset 类似语义

### Batch 4: RuntimeInstantiationBridge V1
**文件**: `runtime_instantiation.cpp` + `token_resolver.*` + `interpreter_vm.cpp`
- Signature-aware marshalling（从 MethodInfoHandle 读取 param count/type，按 signature 压栈 i32/i64/f32/f64/obj）
- Interpreter-to-runtime exception bridge（ManagedExceptionCarrier 携带真实 exception_value）

### Batch 5: 全量测试 + 清理 + 提交

## 测试策略
- 每批次完成后编译 + 运行全部集成测试确保不破坏回归
- 新增测试覆盖新功能
