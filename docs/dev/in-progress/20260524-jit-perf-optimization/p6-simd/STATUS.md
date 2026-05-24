---
task_type: brainstorm-lightweight
phase: brainstorming
roadmap_or_plan: n/a
source: roadmap-child
parent_task_id: 20260524-jit-perf-optimization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: lightweight-brainstorm
created: 2026-05-24
---

# P6: SIMD / HW Intrinsics — STATUS

> 父任务：20260524-jit-perf-optimization（JIT 性能优化路线）
> 前置依赖：P2 内联器 ✅、P3 Intrinsic 识别 ✅、P4 边界消除 ✅

## 继承的父级上下文

### 边界拍板（来自父设计文档）
- 采用轻量树 IR 框架，不做完整 GenTree
- 函数式 pass 编排，预留 PassManager 迁移
- 三优先级：性能最优 > 方案完美性 > HotUpdate 支持

### 已完成的基础设施
- P0 Call-Site Slot 间接化 + Version 失效追踪
- P1 树 IR：表达式树 + VN + CSE + Linearizer
- P2 热更感知内联器
- P3 Intrinsic 识别（Math.Abs/Min/Max, Array.Length）
- P4 边界检查消除（常量索引 + NewArr 传播 + NULL coalescing）
- P5 循环优化（CFG + 支配树 + LICM + IV 削减 + Unrolling 框架）

### 本次子任务范围（来自父 roadmap）
- SIMD / HW intrinsics（Vector128/256 生成 + SSE/AVX 指令）
- 估算：4-6w

## 现有 x64 编码器支持评估

### 已支持的 SSE/FP（x64_encoder.h）
- 标量双精度：movsd/addsd/subsd/mulsd/divsd/cvtsi2sd/cvttsd2si/ucomisd/comisd/cvtsd2ss
- 标量单精度：movss/addss/subss/mulss/divss/cvtsi2ss/cvttss2si/ucomiss/cvtss2sd
- 128位移动：movups（load/store）
- GPR↔XMM：movd/movq（双向）
- XMM 零初始化：pxor/xorps

### 缺失的关键能力
| 类别 | 缺失内容 | 优先级 |
|------|---------|--------|
| SSE2 整型 ALU | paddb/w/d/q, psubb/w/d/q, pmullw/pmuludq, pand, por | **P0** |
| SSE2 整型比较 | pcmpeqb/w/d/q, pcmpgtb/w/d/q | **P0** |
| SSE2 整型 Shuffle | pshufd, punpckl*/punpckh*, packsswb/packuswb, pshufb | **P0** |
| SSE 单精度 Packed | addps/subps/mulps/divps, minps/maxps, shufps, sqrtps | **P1** |
| SSE2 双精度 Packed | addpd/subpd/mulpd/divpd, minpd/maxpd, shufpd | **P1** |
| SSE4.1 插入/提取 | pinsr*/pextr*, pmovsx/pmovzx | P2 |
| POPCNT/LZCNT | popcnt, lzcnt, tzcnt | P2 |
| VEX/AVX | VEX 编码工具 + 3 操作数形式 | P3 |
| 256-bit YMM | 所有 YMM 操作 | P3 |
| FMA | vfmadd*/vfmsub* | P4 |

### IR 和 Intrinsic 系统现状
- 无 SIMD IR opcode（完全缺失）
- IntrinsicMutator 只支持 Math.* 和 Array.Length
- jit_engine.cpp 无 SIMD codegen 路径
- IEncoder 接口只有 1 个 SSE2 整型操作（PxorRR）

## 推荐方案：分层增量式 SIMD 支持

**目标：** 最小可行 SIMD → 覆盖 Vector128\<T\> 最常用操作 → 扩展到 Vector256

### 分层策略

| 层 | 内容 | 估算 |
|----|------|------|
| Layer 1：编码器扩展 | x64_encoder.h 添加 SSE2 整型 ALU + Shuffle + 比较（P0 级缺失项） | 1w |
| Layer 2：IR + Intrinsic | 新 SIMD IR opcode + IntrinsicMutator 扩展识别 Vector128 方法 | 1w |
| Layer 3：Codegen | jit_engine.cpp SIMD codegen 路径 + 寄存器分配扩展 | 1-2w |
| Layer 4：测试 + POPCNT | 测试 subject + POPCNT/LZCNT 编码器 + Intrinsic | 1w |
| Layer 5：AVX/VEX（可选） | VEX 编码工具 + 256-bit 操作 | 1-2w |

### 关键设计决策

**D1 — SIMD 建模方式**
选择：**操作数展开**（Option A）
- 不在 IR 中新增复杂 SIMD 类型系统，每个 SIMD 操作展开为对应 IR opcode
- 例如 `Vector128.Add(lhs, rhs)` → IR opcode `SimdAdd` → 直接发射 `paddd`/`paddq` 等
- 理由：简单、与现有树 IR 框架一致、不需要处理 SIMD 向量寄存器生命周期

**D2 — 寄存器分配**
- 利用现有 FPR 文件（XMM0-XMM15 已分配）
- Vector128<T> 映射到一个 XMM 寄存器
- 不需要额外的向量寄存器分配器

**D3 — 测试策略**
- 新增 Managed C# subjects: 基本的 Vector128 操作
- 验证 codegen 正确发射对应 SSE/SSE2 指令
- 用 asm-compare 验证生成的指令序列

### 风险评估
| 风险 | 影响 | 缓解 |
|------|------|------|
| SSE 指令编码错误可能导致静默数据损坏 | 高 | 每个新 Emit* 与 XED/Intel SDM 对照验证 |
| Vector128<T> 类型擦除（16 种 T） | 中 | 只在 IntrinsicMutator 层做类型分派，codegen 层只操作 XMM |
| 对齐要求（movdqa vs movdqu） | 中 | 统一使用 movdqu（同 CoreCLL JIT 策略） |
| 现有基准测试无 SIMD 覆盖 | 低 | 新增简单 Vector128 测试 subject |
