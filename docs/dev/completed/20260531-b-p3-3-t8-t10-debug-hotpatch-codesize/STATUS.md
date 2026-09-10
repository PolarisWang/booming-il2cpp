# B-P3-3: T8-T10 Debug Info, Hotpatch Metadata, Code Size

> 父任务: 20260523-codegen-industrialization
> source_task_id: B-P3-3
> source_relation: roadmap-child
> 创建日期: 2026-05-23

---

## Phase

`completed`

## Type

`plan`

## 设计摘要

### 边界拍板

- **目标**: 实现 T8 (debug info)、T9 (hotpatch metadata)、T10 (code size optimization) 三项增强
- **范围**: 修改 code_generator.cpp 和 codegen_helpers.h，不修改 IR 格式或 GC 运行时
- **非目标**: 不引入新的调试信息格式(DWARF/PDB)、不修改 ABI、不改 VTable 布局

### Authority 决策

- 属于 Thread B (HT) Phase 3 任务
- 冲突域: src/native/codegen/ (code_generator.cpp, codegen_helpers.h)
- 依赖 B-P3-2 (已完成)
- 不影响 Thread A (托管 C#) 或 Thread C (测试)

### 当前结论

#### T8 Debug Info — IL 偏移映射
- `NativeMethod` 新增 `il_offsets` 数组（从 `RegisterMethod::il_offsets` 复制）
- 结合既有 `instr_offsets` 表，可重建 native offset → IL offset 映射
- 用于运行时诊断和 crash dump 分析，release 零开销

#### T9 Hotpatch Metadata — 可替换 Call 标记
- `CallSiteInfo` 新增 `is_hotpatchable` / `hotpatch_nop_offset` 字段
- `CodeGenConfig::enable_hotpatch` 开关（默认 false）
- `EmitHotpatchPrologue()` 为 hotpatchable call 插入 5-byte NOP sled
- 不改 ABI、不改多态分发表、不改 VTable 布局

#### T10 Code Size — 冷路径分离 + 紧凑帧
- **冷路径 trampoline**：激活 `cold_branch_patches_` 机制，PGO <10% taken 分支通过冷段 JMP 重定向
- **紧凑帧**：方法 ≤ 4 条指令且无 call/FPR/localloc 时，帧从 864B → 544B（紧凑）或 800B（有 FPR）
- `CodeGenConfig::optimize_for_size` 开关（默认 false）
- OSR entry 方法始终使用完整帧

### 三优先级权衡结论

| 维度 | T8 | T9 | T10 |
|------|-----|-----|-----|
| P1 性能 | debug build 生效，release 零开销 | NOP sled 无指令依赖，不影响出口 | 冷路径分离提升 I-cache locality |
| P2 架构 | 最小侵入，不改 NativeMethod 版本 | 在 CallSiteInfo 结构上扩展 | 冷路径分离是 T5 PGO 的自然延伸 |
| P3 HotUpdate | 帮助定位热更后异常来源 | 直接支撑 hotupdate call 替换 | 紧凑帧减少热更补丁尺寸 |

无优先级冲突，三项增强互为独立。

---

## 最近摘要

2026-05-23: B-P3-3 实现完成。
- T8: NativeMethod.il_offsets 已添加，通过 RegisterMethod::il_offsets 填充，提供 native→IL offset 映射
- T9: CallSiteInfo.is_hotpatchable/hotpatch_nop_offset 已添加，EmitHotpatchPrologue 插入 5-byte NOP sled
- T10: cold_branch_patches_ 已激活（PGO <10% taken），紧凑帧模式已实现（544B/800B vs 864B）
- 全部 33+ 测试通过（pre-existing OSR crash 不变）
- 计划文档: plan-v1-01.md

## 关键文档

- [Roadmap 父任务](../20260523-codegen-industrialization/roadmap-v1-01.md)

## Question Clearance

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## Entry Skill

`lightweight-brainstorm` (父 roadmap 已充分定义边界和接口契约)
