# P6: SIMD / HW Intrinsics 实现计划

> **面向执行 Agent：** 使用 dev:subagent-driven-development 或 dev:executing-plans 执行。步骤使用复选框（`- [ ]`）跟踪。完成当前子任务目录必须归档到 `docs/dev/completed/` 并更新父 roadmap。

**目标：** 为 JIT 引擎添加 SIMD 支持，覆盖 Vector128\<T\> 最常用运算操作（算术、位运算、比较、Shuffle、Load/Store）以及 POPCNT/LZCNT 位操作 intrinsic。

**架构：** 采用单 `kSimd` NodeKind + SimdOperation 枚举的紧凑建模方式。不在 IR 中添加大量独立 opcode，而是用单个 `Simd` opcode + 子操作编码。x64_encoder.h 先补齐缺失的 SSE/SSE2/SSSE3/SSE4.1 packed 指令，IntrinsicMutator 识别 System.Numerics.Vector128 方法并展开为 kSimd 节点，Linearizer 发射 Simd opcode，jit_engine.cpp 统一调度到对应的 SSE 指令发射。

**架构审核模式：** normal（树 IR 框架已在 P1 稳定，P6 在其上扩展编码器 + Intrinsic + Codegen，不改变现有架构模式）

**结构告警重点：** x64_encoder.h 新增大量 Emit* 函数需保持命名一致；jit_engine.cpp SIMD codegen 路由不混入其他操作；IntrinsicMutator 的 SIMD 表规模较大（数十个方法）需关注查找效率。

**设计文档：** 父设计文档 design-v1-01.md（D1-D4 架构决策覆盖；SIMD 在树 IR 上的集成方式与 P3 Intrinsic 模式一致）

**问题清零来源：** parent-roadmap（p6-simd STATUS.md 继承父任务清零，clearance_source: parent-roadmap）

**计划来源：** roadmap-child-auto（轻量级 brainstorm 确认分层策略）

**预期知识沉淀：** n/a（SIMD 是树 IR 框架与编码器扩展，不引入新架构模式）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 父 roadmap 同步"固定链路。

---

## 已确认设计决策

| 维度 | 方案 | 说明 |
|------|------|------|
| SIMD 建模 | 单 `kSimd` NodeKind + SimdOperation 枚举 | 不新增 50+ 独立 NodeKind，打包到 `operand_index` 字段 |
| IR opcode | 单 `Simd` opcode + 子操作编码在 imm 中 | codegen 路由统一调度，不污染 opcode 命名空间 |
| 寄存器分配 | 复用 XMM0-XMM15（FPR 文件） | Vector128\<T\> 映射到单个 XMM，不新增寄存器分配器 |
| 对齐策略 | 统一使用 movdqu（不对齐版本） | 同 CoreCLR JIT 实际策略，避免对齐 crash |
| 测试顺序 | 新增 Managed C# subjects 后验证 | 先写测试再验证 codegen 正确性 |
| Vector256 | 推迟到 Layer 5 | 需要 VEX 编码工具，P6 可选范围 |
| FMA | 推迟 | 非 hotpath，属于后续优化 |

## Preflight Self-Review

- 父 roadmap 状态: p6-simd = planned, child_execution_mode = auto
- 子任务目录存在: `docs/dev/in-progress/20260524-jit-perf-optimization/p6-simd/`
- 入口技能: lightweight-brainstorm（已完成）
- 设计输入: parent-roadmap design-v1-01.md + p6-simd STATUS.md
- P0-P5 完成状态: 全部 completed
- **结论: pass** — 可自动继续执行

---

## 任务 1：x64_encoder.h SSE2 整型 ALU 扩展

**目标：** 补齐最常用的 SSE2/SSSE3/SSE4.1 packed 整型指令，覆盖 Vector128\<byte/int16/int32/int64\> 的算术、位运算、比较、Shuffle。

**文件：**
- 修改: `src/native/jit/x64_encoder.h`

### 1.1 算术运算（SSE2）

| 函数 | 指令 | 编码模式 |
|------|------|---------|
| EmitPaddbRR | paddb xmm1, xmm2 | 66 0F FC /r |
| EmitPaddwRR | paddw xmm1, xmm2 | 66 0F FD /r |
| EmitPadddRR | paddd xmm1, xmm2 | 66 0F FE /r |
| EmitPaddqRR | paddq xmm1, xmm2 | 66 0F D4 /r |
| EmitPsubbRR | psubb xmm1, xmm2 | 66 0F F8 /r |
| EmitPsubwRR | psubw xmm1, xmm2 | 66 0F F9 /r |
| EmitPsubdRR | psubd xmm1, xmm2 | 66 0F FA /r |
| EmitPsubqRR | psubq xmm1, xmm2 | 66 0F FB /r |
| EmitPmullwRR | pmullw xmm1, xmm2 | 66 0F D5 /r |
| EmitPmuludqRR | pmuludq xmm1, xmm2 | 66 0F F4 /r |

编码模式：`0x66 + REX(false, dst, src) + 0x0F + opcode + ModRM(3, dst, src)` — 与现有 EmitPxorRR 一致。

### 1.2 位运算（SSE2）

| 函数 | 指令 | 编码模式 |
|------|------|---------|
| EmitPandRR | pand xmm1, xmm2 | 66 0F DB /r |
| EmitPorRR | por xmm1, xmm2 | 66 0F EB /r |
| EmitPandnRR | pandn xmm1, xmm2 | 66 0F DF /r |

### 1.3 比较运算（SSE2/SSE4.1）

| 函数 | 指令 | 编码模式 |
|------|------|---------|
| EmitPcmpeqbRR | pcmpeqb xmm1, xmm2 | 66 0F 74 /r |
| EmitPcmpeqwRR | pcmpeqw xmm1, xmm2 | 66 0F 75 /r |
| EmitPcmpeqdRR | pcmpeqd xmm1, xmm2 | 66 0F 76 /r |
| EmitPcmpeqqRR | pcmpeqq xmm1, xmm2 | 66 0F 38 29 /r |
| EmitPcmpgtbRR | pcmpgtb xmm1, xmm2 | 66 0F 64 /r |
| EmitPcmpgtwRR | pcmpgtw xmm1, xmm2 | 66 0F 65 /r |
| EmitPcmpgtdRR | pcmpgtd xmm1, xmm2 | 66 0F 66 /r |
| EmitPcmpgtqRR | pcmpgtq xmm1, xmm2 | 66 0F 38 37 /r |

### 1.4 Shuffle/Unpack 运算（SSE2/SSSE3）

| 函数 | 指令 | 编码模式 |
|------|------|---------|
| EmitPshufdRR | pshufd xmm1, xmm2, imm8 | 66 0F 70 /r ib |
| EmitPshufbRR | pshufb xmm1, xmm2 | 66 0F 38 00 /r |
| EmitPunpcklbwRR | punpcklbw xmm1, xmm2 | 66 0F 60 /r |
| EmitPunpcklwdRR | punpcklwd xmm1, xmm2 | 66 0F 61 /r |
| EmitPunpckldqRR | punpckldq xmm1, xmm2 | 66 0F 62 /r |
| EmitPunpcklqdqRR | punpcklqdq xmm1, xmm2 | 66 0F 6C /r |
| EmitPunpckhbwRR | punpckhbw xmm1, xmm2 | 66 0F 68 /r |
| EmitPunpckhwdRR | punpckhwd xmm1, xmm2 | 66 0F 69 /r |
| EmitPunpckhdqRR | punpckhdq xmm1, xmm2 | 66 0F 6A /r |
| EmitPunpckhqdqRR | punpckhqdq xmm1, xmm2 | 66 0F 6D /r |
| EmitPacksswbRR | packsswb xmm1, xmm2 | 66 0F 63 /r |
| EmitPackssdwRR | packssdw xmm1, xmm2 | 66 0F 6B /r |
| EmitPackuswbRR | packuswb xmm1, xmm2 | 66 0F 67 /r |

对于带立即数的指令（EmitPshufdRR），增加 `imm8` 参数。
对于 3 字节 opcode 的指令（SSSE3/SSE4.1 的 `66 0F 38 XX` 系列），编码模式为：`0x66 + REX + 0x0F + 0x38 + opcode + ModRM`。

### 1.5 Shift 运算（SSE2）

| 函数 | 指令 | 编码模式 |
|------|------|---------|
| EmitPsllwRR | psllw xmm1, xmm2 | 66 0F F1 /r |
| EmitPslldRR | pslld xmm1, xmm2 | 66 0F F2 /r |
| EmitPsllqRR | psllq xmm1, xmm2 | 66 0F F3 /r |
| EmitPsrlwRR | psrlw xmm1, xmm2 | 66 0F D1 /r |
| EmitPsrldRR | psrld xmm1, xmm2 | 66 0F D2 /r |
| EmitPsrlqRR | psrlq xmm1, xmm2 | 66 0F D3 /r |
| EmitPsrawRR | psraw xmm1, xmm2 | 66 0F E1 /r |
| EmitPsradRR | psrad xmm1, xmm2 | 66 0F E2 /r |

### 1.6 插入/提取（SSE2/SSE4.1）

| 函数 | 指令 | 编码模式 |
|------|------|---------|
| EmitPinsrwRR | pinsrw xmm, r32, imm8 | 66 0F C4 /r ib |
| EmitPextrwRR | pextrw r32, xmm, imm8 | 66 0F C5 /r ib |
| EmitPinsrbRR | pinsrb xmm, r32, imm8 | 66 0F 3A 20 /r ib |
| EmitPinsrdRR | pinsrd xmm, r/m32, imm8 | 66 0F 3A 22 /r ib |
| EmitPextrbRR | pextrb r32, xmm, imm8 | 66 0F 3A 14 /r ib |
| EmitPextrdRR | pextrd r32, xmm, imm8 | 66 0F 3A 16 /r ib |
| EmitPmovmskbRR | pmovmskb r32, xmm | 66 0F D7 /r |

### 1.7 附加支持

| 函数 | 指令 | 编码模式 |
|------|------|---------|
| EmitMovdqaRR | movdqa xmm1, xmm2 | 66 0F 6F /r |
| EmitMovdqaMR | movdqa [mem], xmm | 66 0F 7F /r |
| EmitMovdqaRM | movdqa xmm, [mem] | 66 0F 6F /r |
| EmitPabsbRR | pabsb xmm1, xmm2 | 66 0F 38 1C /r |
| EmitPabswRR | pabsw xmm1, xmm2 | 66 0F 38 1D /r |
| EmitPabsdRR | pabsd xmm1, xmm2 | 66 0F 38 1E /r |
| EmitPmaddwdRR | pmaddwd xmm1, xmm2 | 66 0F F5 /r |

**验证：** 每个 Emit* 函数对照 Intel SDM 确认 opcode 和 ModRM 编码。编写最小单元测试（在 `code_buffer` 上构造指令序列，用 XED 或读字节验证）。

### 1.8 IEncoder.h 接口扩展（可选）

为 X64Encoder 类添加对应的虚方法。由于当前 jit_engine.cpp 直接使用自由函数而非 IEncoder 接口，这一步可在批处理阶段最后完成。

**验证：** build 通过，所有新指令编码正确

## 任务 2：POPCNT / LZCNT 编码器

**目标：** 添加 POPCNT（人口计数）和 LZCNT（前导零计数）指令，用于 BCL `BitOperations.PopCount()` / `LeadingZeroCount()` intrinsic。

**文件：**
- 修改: `src/native/jit/x64_encoder.h`

```cpp
/// popcnt r64, r/m64 (REP + 0F B8)
inline void EmitPopcntRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);  // REP prefix
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0xB8);
    buf.EmitByte(ModRM(3, dst, src));
}
```

```cpp
/// lzcnt r64, r/m64 (REP + 0F BD)
inline void EmitLzcntRR(CodeBuffer& buf, uint8_t dst, uint8_t src) noexcept {
    buf.EmitByte(0xF3);
    EmitREX(buf, true, dst, src);
    buf.EmitByte(0x0F);
    buf.EmitByte(0xBD);
    buf.EmitByte(ModRM(3, dst, src));
}
```

**验证：** build 通过

## 任务 3：IR opcode 扩展 — Simd + Popcnt/Lzcnt

**目标：** 添加新的 IROpCode，使 Linearizer 能发射 SIMD 指令。

**文件：**
- 修改: `src/native/interpreter/generated/ir_opcodes.h`

在 `LdElemANoChk = 106` 后追加：

```cpp
// SIMD operations (single opcode with sub-op in imm)
Simd        = 107,  // SIMD operation (sub-op + element type in imm)

// Bit manipulation intrinsics
Popcnt      = 108,  // population count
Lzcnt       = 109,  // leading zero count
```

### SimdSubOperation 枚举

新增枚举（放在 `jit_tree_node.h` 或独立的 `jit_simd.h` 中）：

```cpp
enum SimdSubOperation : uint8_t {
    kSimdAdd,       // paddb/w/d/q (element type distinguishes width)
    kSimdSub,       // psubb/w/d/q
    kSimdMul,       // pmullw / pmuludq
    kSimdAnd,       // pand
    kSimdOr,        // por
    kSimdXor,       // pxor
    kSimdAndNot,    // pandn
    kSimdEq,        // pcmpeqb/w/d/q
    kSimdGt,        // pcmpgtb/w/d/q
    kSimdShuffle,   // pshufd (with imm8)
    kSimdShuffleB,  // pshufb
    kSimdUnpackLo,  // punpckl*
    kSimdUnpackHi,  // punpckh*
    kSimdPackS,     // packss* / packus*
    kSimdAbs,       // pabsb/w/d
    kSimdNeg,       // negate
    kSimdShl,       // psllw/d/q
    kSimdShr,       // psrlw/d/q
    kSimdSar,       // psraw/d (arithmetic)
    kSimdExtract,   // pextrb/w/d (extract element to GPR)
    kSimdInsert,    // pinsrb/w/d (insert scalar from GPR)
    kSimdMoveMask,  // pmovmskb
    kSimdLoad,      // movdqa/movdqu from memory
    kSimdStore,     // movdqa/movdqu to memory
    kSimdZero,      // pxor (zero XMM)
};
```

### SimdElementType 枚举

```cpp
enum SimdElementType : uint8_t {
    kElemInt8   = 0,
    kElemInt16  = 1,
    kElemInt32  = 2,
    kElemInt64  = 3,
    kElemFloat32 = 4,
    kElemFloat64 = 5,
};
```

**验证：** build 通过

## 任务 4：kSimd NodeKind 树节点扩展

**目标：** 添加 kSimd NodeKind + kPopcnt/kLzcnt，使树 IR 能表达 SIMD 操作。

**文件：**
- 修改: `src/native/jit/tree/jit_tree_node.h`

### NodeKind 扩展

在 `kLdElemA = 56` 后插入：

```cpp
kSimd       = 57,  // SIMD operation (sub-op in operand_index, children for inputs)
kPopcnt     = 58,  // population count (unary)
kLzcnt      = 59,  // leading zero count (unary)
```

### 编码约定

kSimd 节点使用 `ExprNode` 现有字段编码 SIMD 元数据：

```
operand_index: bit[0..7]   = SimdSubOperation
               bit[8..15]  = SimdElementType
               bit[16..31] = immediate (shuffle mask, etc.)
```

访问器：

```cpp
inline uint8_t simd_op() const noexcept { return static_cast<uint8_t>(operand_index & 0xFF); }
inline uint8_t simd_elem_type() const noexcept { return static_cast<uint8_t>((operand_index >> 8) & 0xFF); }
inline uint16_t simd_imm() const noexcept { return static_cast<uint16_t>((operand_index >> 16) & 0xFFFF); }
```

child0 = lhs operand（或 load 的内存地址），child1 = rhs operand（binary 操作）或 unused（unary 操作）。

**验证：** build 通过

## 任务 5：IntrinsicMutator SIMD 扩展

**目标：** 识别 System.Numerics.Vector128\<T\> 方法并展开为 kSimd 节点。

**文件：**
- 修改: `src/native/jit/tree/jit_intrinsics.h`
- 修改: `src/native/jit/tree/jit_intrinsics.cpp`

### 新增 IntrinsicId

```cpp
enum class IntrinsicId : uint8_t {
    // ... existing entries ...
    kVector128Load,
    kVector128Store,
    kVector128Add,
    kVector128Sub,
    kVector128Mul,
    kVector128And,
    kVector128Or,
    kVector128Xor,
    kVector128Equals,
    kVector128Shuffle,
    kVector128Abs,
    kVector128Negate,
    kVector128Zero,
    kVector128Insert,
    kVector128Extract,
    kVector128As,
    kBitOpsPopCount,
    kBitOpsLeadingZeroCount,
};
```

### IntrinsicMutator::Visit 扩展

在现有 Intrinsic table 扫描之后，增加专门的 SIMD dispatch 路径。当 `match->id` 为 kVector128* 时：

1. 解析参数 vreg → SimdElementType（通过方法 token 后面的类型信息推断）
2. 创建 kSimd 节点，设置 child0/child1 和 `operand_index`（打包 SimdSubOperation + SimdElementType）
3. 返回新节点

对于入口参数，由于 Vector128\<T\> 在调用惯例中通过引用传递，IntrinsicMutator 需要特别的参数解包逻辑。

**简化策略（P6 初始实现）：** 当前阶段对 Vector128 方法做简化的内置识别：
- 硬编码已知的 Vector128 方法签名模式
- 通过 arg_count 和预期 token 识别
- skip 类型复杂度过高的重载（如 Shuffle(Vector128, Vector128) 等）

**验证：** ConvertChar subjects 中添加 Vector128 运算后，验证树 IR 中出现 kSimd 节点

## 任务 6：Linearizer — Simd/Popcnt/Lzcnt 发射

**目标：** 扩展 Linearizer 以处理 kSimd/kPopcnt/kLzcnt 节点。

**文件：**
- 修改: `src/native/jit/tree/jit_linearizer.cpp`
- 修改: `src/native/jit/tree/jit_linearizer.h`

### kSimd 线性化

在 LinearizeNode 中新增 kSimd 分支：

```cpp
if (k == kSimd) {
    uint32_t src1_vreg = LinearizeNode(node->child0, out);
    uint32_t src2_vreg = (node->child1) ? LinearizeNode(node->child1, out) : 0;
    // src2 可能为 0（unary 操作如 Abs、Zero）
    
    uint32_t dst = NextVReg();
    uint64_t simd_meta = static_cast<uint64_t>(node->simd_op()) |
                         (static_cast<uint64_t>(node->simd_elem_type()) << 8) |
                         (static_cast<uint64_t>(node->simd_imm()) << 16);
    
    interpreter::RegisterInstruction ri;
    ri.header = MakeHdr(interpreter::IROpCode::Simd, dst,
                         static_cast<uint8_t>(src1_vreg),
                         static_cast<uint8_t>(src2_vreg),
                         interpreter::kRegHasDst |
                         interpreter::kRegHasSrc1 |
                         (src2_vreg ? interpreter::kRegHasSrc2 : 0));
    ri.imm.i8 = static_cast<int64_t>(simd_meta);  // pack simd_op + elem_type + imm in imm field
    out.push_back(ri);
    return dst;
}
```

### kPopcnt/kLzcnt 线性化

在 unary 处理分支中增加：

```cpp
case kPopcnt: case kLzcnt:
    opc = (k == kPopcnt) ? interpreter::IROpCode::Popcnt : interpreter::IROpCode::Lzcnt;
    // 标准 unary 发射
    break;
```

### LinearizeNode 签名调整

为支持 kSimd 的 simd_op/simd_elem_type/simd_imm 访问器，在 `jit_tree_node.h` 中新增内联方法或直接通过 `operand_index` 位操作访问。

**验证：** kSimd 节点正确发射为 Simd opcode + imm 元数据

## 任务 7：jit_engine.cpp SIMD Codegen

**目标：** 为 Simd/Popcnt/Lzcnt opcode 添加代码生成路径。

**文件：**
- 修改: `src/native/jit/jit_engine.cpp`

### Simd 指令调度表

在 `NativeCodeGenerator::Generate()` 的指令处理循环中添加新分支：

```cpp
case IROpCode::Simd: {
    if (!instr.has_src1() || !instr.has_dst()) return false;
    uint8_t xmm_dst = AllocFpr(instr.dst_reg());
    uint8_t xmm_src1 = AllocFpr(instr.src1_reg());
    uint8_t xmm_src2 = instr.has_src2() ? AllocFpr(instr.src2_reg()) : 0;
    
    int64_t meta = instr.imm.i8;
    uint8_t simd_op = static_cast<uint8_t>(meta & 0xFF);
    uint8_t elem_type = static_cast<uint8_t>((meta >> 8) & 0xFF);
    uint16_t simd_imm = static_cast<uint16_t>((meta >> 16) & 0xFFFF);
    
    // Load vector operands from stack to XMM
    LoadFpr(kXMM_DST_PHYS, instr.dst_reg());
    LoadFpr(kXMM_SRC1_PHYS, instr.src1_reg());
    if (instr.has_src2()) LoadFpr(kXMM_SRC2_PHYS, instr.src2_reg());
    
    // Dispatch on (simd_op, elem_type)
    return EmitSimd(xmm_dst, xmm_src1, xmm_src2, simd_op, elem_type, simd_imm);
}
```

### EmitSimd 私有方法

```cpp
bool NativeCodeGenerator::EmitSimd(uint8_t dst, uint8_t src1, uint8_t src2,
                                    uint8_t op, uint8_t elem, uint16_t imm) noexcept {
    switch (op) {
    case kSimdAdd:
        switch (elem) {
        case kElemInt8:   enc_.EmitPaddbRR(dst, src1); break;
        case kElemInt16:  enc_.EmitPaddwRR(dst, src1); break;
        case kElemInt32:  enc_.EmitPadddRR(dst, src1); break;
        case kElemInt64:  enc_.EmitPaddqRR(dst, src1); break;
        }
        return true;
    case kSimdShuffle:
        // pshufd dst, src1, imm8
        enc_.EmitPshufdRR(dst, src1, static_cast<uint8_t>(imm & 0xFF));
        return true;
    case kSimdEq:
        // pcmpeq*
        ...
    // ...等
    }
}
```

### Popcnt/Lzcnt codegen

```cpp
case IROpCode::Popcnt: {
    if (!instr.has_src1() || !instr.has_dst()) return false;
    LoadGpr(kGprSrc, instr.src1_reg());
    enc_.EmitPopcntRR(kGprDst, kGprSrc);
    StoreGpr(kGprDst, instr.dst_reg());
    return true;
}
```

### FPR Load/Store 辅助

为支持从栈帧加载 XMM 寄存器值，在 NativeCodeGenerator 中添加：

```cpp
/// Load FPR from stack frame (virtual FPR → physical XMM)
void LoadFpr(uint8_t xmm, uint32_t vreg) noexcept {
    enc_.EmitMovSDRM(xmm, kRSP, FprOff(vreg));  // 使用 movsd 作为 XMM load
}

/// Store FPR to stack frame (physical XMM → virtual FPR)  
void StoreFpr(uint32_t vreg, uint8_t xmm) noexcept {
    enc_.EmitMovSDMR(kRSP, FprOff(vreg), xmm);
}
```

注意：当前 FPR 文件分配了 32 个虚拟寄存器（kGPRegisters=64 到 kGPRegisters+32），但只有标量 load/store 通过 movsd。对于 SIMD 需要完整的 16 字节 load/store。在 `FprOff()` 保持不变（每个虚拟 FPR 8 字节）的前提下，SIMD 操作需要额外的 16 字节 spill 区或使用 `XMM 寄存器直接映射` 而非栈传递。

**简化策略：** SIMD 操作不使用虚拟 FPR 栈帧传递，而是在 codegen 中直接分配物理 XMM 寄存器，通过寄存器分配器（`jit_reg_alloc.h`）管理生命周期。这需要在 `AllocFpr` 的基础上增加保留和释放机制。

**验证：** Simd opcode 正确发射对应 SSE 指令序列

## 任务 8：管线集成

**目标：** 在优化管线中启用 SIMD intrinsic 展开。

**文件：**
- 修改: `src/native/jit/tree/jit_optimizer.cpp`

### 管线位置

```
ConstFoldMutator → BoundCheckEliminator → IntrinsicMutator(SIMD) → CSEMutator → Linearizer
```

IntrinsicMutator 扩展到同时处理原有 intrinsic + SIMD intrinsic。管线位置不变。

### OptimizeWithTreeIR 扩展

```cpp
// IntrinsicMutator: replace known BCL calls (including SIMD)
IntrinsicMutator intrinsic_mut(arena_pos, arena_end,
                                kIntrinsicTable, kIntrinsicTableSize,
                                &builder);
for (uint32_t ri = 0; ri < result.root_count; ++ri)
    result.roots[ri] = intrinsic_mut.Mutate(result.roots[ri]);
```

原有的 IntrinsicMutator 管线不变。SIMD table 作为 kIntrinsicTable 的扩展行（token → kVector128* 映射）。

**验证：** 启用优化时 SIMD intrinsic 在管线中被正确展开

## 任务 9：测试 Subjects + 验证

**目标：** 新增 C# 测试 subject 验证 SIMD 运算的正确性。

### 新增测试 subject（Managed C#）

在 `convert-char/managed/subjects/` 中添加：

**Simd128Subjects.cs：**
```csharp
// 1. Vector128<int> Add
static int Simd128AddTest() {
    var v1 = Vector128.Create(1, 2, 3, 4);
    var v2 = Vector128.Create(5, 6, 7, 8);
    var result = Vector128.Add(v1, v2);
    // result[0] == 6, result[3] == 12
    return result.GetElement(0) + result.GetElement(3);  // 18
}

// 2. Vector128<byte> bitwise and
static int Simd128AndTest() { ... }

// 3. Vector128<int> equals
static int Simd128EqTest() { ... }

// 4. Vector128<int> shuffle
static int Simd128ShuffleTest() { ... }

// 5. BitOperations.PopCount
static int PopCountTest() {
    return BitOperations.PopCount(0x0F0F0F0F);  // 16
}
```

### 验证命令

```bash
cd testing/foundation-dll/System.Private.CoreLib/convert-char/native/build-jit
cmake -S . -B . && cmake --build . --config Debug
./Debug/entry.exe --fact all        # 期望 18/18 + 新增 SIMD tests
./Debug/entry.exe --benchmark 1 10000  # 期望无退化
```

**检查项：**
1. build 通过
2. 原有 fact 18/18 仍然通过
3. 新增 SIMD subject 返回正确结果
4. `--no-opt` 下所有 fact 仍正确（Simd 走回退路径）
5. asm-compare 验证生成指令中包含正确的 SSE 指令

## 任务 10：CMakeLists.txt — 注册新文件

**文件：**
- 修改: `src/native/jit/CMakeLists.txt`

当前 P6 不需要新增 .cpp 文件（`jit_intrinsics.cpp`、`jit_linearizer.cpp`、`jit_engine.cpp`、`x64_encoder.h` 都是已有文件，只做修改和扩展）。如有新文件，按以下模板注册：

```cmake
add_library(chaos_jit STATIC
    ...
    tree/jit_simd.cpp     # 如果新增
    ...
)
```

## 文件修改汇总

| 文件 | 修改类型 | 说明 |
|------|----------|------|
| `src/native/jit/x64_encoder.h` | 修改 | +45 个 SSE2/SSSE3/SSE4.1 指令函数 + POPCNT/LZCNT |
| `src/native/interpreter/generated/ir_opcodes.h` | 修改 | 新增 Simd(107), Popcnt(108), Lzcnt(109) |
| `src/native/jit/tree/jit_tree_node.h` | 修改 | 新增 kSimd(57), kPopcnt(58), kLzcnt(59) + SimdSubOperation/SimdElementType 枚举 + 访问器 |
| `src/native/jit/tree/jit_intrinsics.h` | 修改 | 新增 kVector128*/kBitOps* IntrinsicId |
| `src/native/jit/tree/jit_intrinsics.cpp` | 修改 | IntrinsicMutator SIMD dispatch |
| `src/native/jit/tree/jit_linearizer.cpp` | 修改 | kSimd/kPopcnt/kLzcnt 线性化发射 |
| `src/native/jit/jit_engine.cpp` | 修改 | Simd/Popcnt/Lzcnt codegen + EmitSimd 调度表 + FPR load/store |
| `testing/foundation-dll/.../managed/subjects/Simd128Subjects.cs` | **新增** | Vector128 + POPCNT 测试 subject |

## 注意事项

1. **x64 编码器模式一致性**：所有新 Emit* 函数必须严格遵循现有编码风格（`0x66/0xF2/0xF3` + REX + `0x0F` + opcode + ModRM），SSSE3/SSE4.1 的三字节 opcode 需要额外 `0x38` 或 `0x3A` 字节。

2. **FPR 寄存器分配**：当前 jit_engine.cpp 通过 `FprOff()` 在栈帧中分配每个虚拟 FPR 8 字节。SIMD 操作需要 16 字节 XMM 值。P6 初始实现选择**不通过栈传递向量值**，而是在 codegen 中直接管理物理 XMM 寄存器。这意味着：
   - SIMD 操作的 src/dst 寄存器的值在 SIMD codegen 路径中保持在 XMM 中
   - 非 SIMD 路径（如标量 FP）仍然使用 8 字节栈帧
   - 当一个值需要在 SIMD 和非 SIMD 路径之间传递时，通过栈 spill/reload

3. **GC 安全性**：XMM 寄存器不包含 GC 引用，SIMD codegen 路径不需要额外的 GC 信息。现有 RecordGcPoint 只跟踪 GPR（RAX/RCX/RDX/R8 等调用参数寄存器），SIMD 不影响 GC 语义。

4. **Intrinsic 方法匹配精度**：Vector128\<T\> 方法有 16 种 T 的重载。P6 使用简化的 token + arg_count 匹配，不依赖完整的泛型实例化签名。更精确的匹配需要 JIT registration 时提供泛型参数的上下文，留到后续改进。

5. **测试范围**：初始测试只覆盖最常用的 Vector128\<int\> 和 Vector128\<byte\> 操作，使用 `.NET 标准 `System.Numerics.Vector128` API（Create, Add, BitwiseAnd, Equals, Shuffle, GetElement, Extract 等）。后续扩展到 float/double/int64。

6. **HotUpdate 集成**：SIMD intrinsic 展开是纯 JIT 编译时优化，不影响 hotpatch 路径。JIT 编译后的代码包含直接的 SSE 指令，没有与 slot 间接化或 version 追踪的冲突。
