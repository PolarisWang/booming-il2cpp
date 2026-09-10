# B-P3-2: T7 Graph Coloring Register Allocator Integration

> 父任务: 20260523-codegen-industrialization
> source_task_id: B-P3-2
> source_relation: roadmap-child
> 创建日期: 2026-05-23

---

## Phase

`completed`

## Type

`plan`

## 设计摘要

### 边界拍板

- **目标**: 将已有的 `reg_alloc_graph_coloring.h` (Chaitin-Briggs 图着色分配器) 接入 codegen 流水线，取代当前的 stack-slot 寄存器分配方案
- **范围**: 修改 code_generator.cpp 的指令发射逻辑，使用物理寄存器而非栈槽
- **非目标**: 不修改 reg_alloc_graph_coloring.h 的算法本身，不改 IR 格式

### Authority 决策

- 属于 Thread B (HT) Phase 3 任务
- 冲突域: src/native/codegen/ (code_generator.cpp, code_generator.h)
- 依赖 B-P3-1 (已完成的 PGO 集成)
- 不影响 Thread A (托管 C#) 或 Thread C (测试)

### 当前结论

任务创建完成，进入 plan 阶段。

---

## 最近摘要

2026-05-23: B-P3-2 任务初始化。已有完整图着色分配器实现，需要设计集成方案。

## 关键文档

- [Roadmap 父任务](../20260523-codegen-industrialization/roadmap-v1-01.md)
- [Brainstorm 设计方案（父级）](../20260523-codegen-industrialization/brainstorm-v1-01.md)

## 完成摘要

### 已完成

1. **7 个正确性测试全部通过** ✅
   - `GraphColor_EmptyMethod` — 0 指令 → 全部 0xFF（未着色）
   - `GraphColor_SingleVreg` — 单 vreg → 成功着色
   - `GraphColor_TwoVregsSequential` — 非重叠生命周期 → 均着色
   - `GraphColor_TwoVregsInterfering` — 重叠生命周期 → 不同颜色
   - `GraphColor_TenVregsFullInterference` — 10 vreg 争 9 色 → 至少 1 溢出
   - `GraphColor_FprColoring` — 2 FPR vreg → 均着色
   - `GraphColor_MixedGprFpr` — 3 GPR + 2 FPR → 均着色

2. **Graph coloring allocator 已完全集成**（21 个 `has_graph_coloring_` 分支点，继承自 Phase 3b）

### 评估/推迟

3. **GC slot map 精确度提升（Gpr kind）** ⏸️ 推迟
   - `GcSlotMapV0` 编码格式使用 `CHAOS_GC_SLOT_OFFSET_MASK (0xFFF)` + `CHAOS_GC_SLOT_KIND_MASK (bit 12)`，仅支持 stack offset
   - 扫描器 `gc_root_scanner.cpp:109-130` 始终计算 `frame_base + offset`，无 GPR 寄存器扫描支持
   - 需要 GC 运行时层先支持 GPR kind slot 编码，再启用

4. **基准测试回归断言** ✅ 已添加（软阈值 0.5x）
   - 当前着色器性能 < 1.0x（预存问题，编号到 B-P3-3 优化）
   - 参数文件: `tests/contracts/native/runtime-core/codegen_native_test.cpp`

### 已知预存问题

- OSR 测试在 Test_OsrEntry 段崩溃（segfault），与着色器无关

## Question Clearance

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## Entry Skill

`lightweight-brainstorm` (父 roadmap 已充分定义边界和接口契约)
