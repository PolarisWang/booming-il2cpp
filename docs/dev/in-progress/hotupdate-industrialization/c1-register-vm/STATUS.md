# C1 — 寄存器式 IR 全新 VM + codegen emitter

> **task_id**: c1-register-vm
> **创建日期**: 2026-05-23
> **parent_task_id**: hotupdate-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: in-progress

## 范围

寄存器式 IR VM 实现 + codegen RegisterIREmitter。注意：native 侧（register allocator、execution engine、OSR）已经在 roadmap 创建前完成，本子任务聚焦剩余的缺口。

## 已完成部分（roadmap 前已存在）

| 组件 | 文件 | 状态 |
|------|------|------|
| RegisterInstruction 16字节指令格式 | `src/native/interpreter/ir_reg_alloc.h:52-91` | **已存在** — packed bit-field header + 8字节 imm union |
| RegisterFile 寄存器文件 | `src/native/interpreter/ir_reg_alloc.h:96-125` | **已存在** — 64 GP + 32 FP，含 tag 数组 |
| RegisterFrame 执行帧 | `src/native/interpreter/ir_reg_alloc.h:133-215` | **已存在** — 含 SEH 状态、OSR 状态、tracked objects |
| RegisterMethod + RegStackMap | `src/native/interpreter/ir_reg_alloc.h:247-254` | **已存在** |
| AllocateRegisters 寄存器分配器 | `src/native/interpreter/ir_reg_alloc.cpp:34` | **已存在** — linear-scan, 256条虚拟栈 |
| RegisterExecute 执行引擎 | `src/native/interpreter/ir_reg_alloc.cpp:2307` | **已存在** — 100/100 opcodes 全部有 handler |
| TryOsrPromotion T4升级 | `src/native/interpreter/ir_reg_alloc.cpp:2227` | **已存在** — V2 OSR（mid-stream）+ V1 fallback |
| tier_manager 背景升级 | `src/native/runtime-core/tier_manager.h/.cpp` | **已存在** — T1→T2→T3 background promotion |
| PatchDataLoader reg_ir 段 | `src/native/runtime-core/patch_loader.cpp:71` | **已存在** — 读取 reg_ir 段 offset/instr_count/seh_count |
| entry_direct.cpp Step B | `src/native/interpreter/interp_entry/entry_direct.cpp:475` | **已存在** — RegisterExecute 是默认执行路径 |

## 待补充

| 交付物 | 文件 | 状态 | 预估 |
|--------|------|------|------|
| 合约头文件 | `contracts/native/v0/register_ir.h` | **待创建** — 从 ir_reg_alloc.h 提取 RegisterInstruction 到 C-compatible 接口 | ~30min |
| Managed RegisterIREmitter | `src/managed/Chaos.IL2CPP.Generator/Emission/RegisterIREmitter.cs` | **待创建** — codegen 组件，直接发射 reg_ir 到 .patchdata | ~2周 |
| register_vm.h/.cpp 封装 | `src/native/interpreter/register_vm.h/.cpp` | **待创建** — 薄封装层，暴露 InterpreterVM 兼容接口 | ~1天 |
| OSR 组件分离 | `src/native/runtime-core/osr_trigger.h/.cpp` | **待创建** — 从 ir_reg_alloc.cpp 分离 OSR 策略 | ~1天 |
| 性能调优 ≤100ns/hotpatch-call | — | **待验证** — 当前有 profiling 数据未确认 | ~1周 |

## 当前进度

- native 侧（VM/allocator/executor/OSR/tiers）：**~90% 完成**
- 合约头文件：**0%**
- Managed codegen emitter：**0%**
- 性能验证：**未开始**

## 下一步

创建合约头文件 `register_ir.h`，然后继续推进后续子任务。
