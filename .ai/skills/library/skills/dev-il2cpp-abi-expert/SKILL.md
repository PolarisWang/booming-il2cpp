---
name: dev-il2cpp-abi-expert
description: 跨平台 ABI 调用约定专家 — Windows x64/Linux x64/ARM64、CHAOS_IL2CPP_INTPTR 语义、struct 传递规则、extern stub ABI 正确性
---

# dev-il2cpp-abi-expert — ABI 调用约定专家

## 领域边界

### 我负责的

1. **跨平台 ABI 规则** — Windows x64 (MSVC) / Linux x64 (System V) / ARM64 (AAPCS64)
2. **CHAOS_IL2CPP_INTPTR 抽象语义** — 什么类型用 CHAOS_IL2CPP_INTPTR (RAX)，什么类型用 float/double (XMM0/V0)
3. **`CarrierKindCode → C++ 类型 → 返回值寄存器` 映射表维护**
4. **struct 参数传递规则** — 寄存器拆分 vs 栈 vs 内存引用，HFA (ARM64) 优化
5. **extern stub 生成的 ABI 正确性** — static inline / extern "C" / C++ linkage 的 ABI 含义
6. **reinterpret_cast ABI 安全分析** — `reinterpret_cast<float(*)()>(fn)()` 等

### 我不负责的

- **代码生成优化** (死代码消除、指令选择) → route to codegen-expert
- **JIT 编译 ABI** (Precode, Stub dispatching) → route to runtime-expert
- **P/Invoke marshal** (Marshalling 类型映射) → route to build-fixer

---

## ABI 速查参考

> 完整跨平台 ABI 参考见 `.ai/skills/knowledge/abi/abi-rules-reference.md`

| 平台 | 整数参数寄存器 | 浮点参数寄存器 | 整数返回值 | 浮点返回值 | struct ≤16B |
|------|--------------|--------------|-----------|-----------|------------|
| Windows x64 | RCX, RDX, R8, R9 | XMM0-XMM3 | RAX | XMM0 | 整数寄存器拆分 |
| Linux x64 | RDI, RSI, RDX, RCX, R8, R9 | XMM0-XMM7 | RAX | XMM0 | 整数寄存器拆分 |
| ARM64 | X0-X7 | V0-V7 | X0 | V0 | X0+X1 |

### CHAOS_IL2CPP_INTPTR 语义（核心规则）

```
CHAOS_IL2CPP_INTPTR = 通用载体类型
  ├── 整数类型 → RAX 返回 ✅
  ├── 指针类型 → RAX 返回 ✅
  ├── float/double → XMM0 返回 ❌ 绝不能使用
  └── struct by-value → 按 ABI 规则 ⚠️ 需要逐案判断
```

**三条铁律：**

1. `Float32`/`Float64` CarrierKindCode → C++ 类型必须是 `float`/`double`，**绝不能是** `CHAOS_IL2CPP_INTPTR`
2. 所有 `static inline CHAOS_IL2CPP_INTPTR` stub 定义必须与真实定义返回类型**完全一致**，否则 linker 报 C2556/C2371
3. `reinterpret_cast<T(*)()>(fn_ptr)()` 中 T 必须与 fn_ptr 的实际返回类型匹配 — 否则读取错误的寄存器

---

## 触发条件

| 信号 | 动作 |
|------|------|
| `C2556` (overloaded differs only by return type) | 检查所有 TU 中该符号的返回类型一致性 |
| `C2371` (redefinition; different basic types) | 同上 — 检查 namespace 冲突 |
| CarrierKindCode switch 修改 | 更新对应平台 ABI 映射 |
| 新增 `chaos_external_runtime_*` stub | 验证返回类型与真实定义一致 |
| reinterpret_cast 相关 crash | 检查 ABI 寄存器是否匹配 |
