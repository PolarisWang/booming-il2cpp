# AOT/JIT/Hybrid 三模式编译架构设计 v1

## 1. 概述

在现有 T4 JIT（code_generator.cpp）基础上，新增 JIT 和 Hybrid 两种 codegen 模式，实现 CoreCLR 风格的三模式编译体系。

### 核心目标

- **AOT 模式**（不变）：纯 AOT C++ 编译，零运行时开销
- **JIT 模式**（新）：CoreCLR 风格，首调用直接 JIT 编译到机器码，**不走解释器**
- **Hybrid 模式**（新）：冷时 AOT 原生执行，热时升级到 JIT 优化代码

## 2. 三种模式 — 从 codegen 到 runtime 完整路径

### 2.1 AOT 模式（现有）

```
codegen:
  {Assembly}.cpp → AOT C++ 函数体
  s_hotpatch_entries[slot] = { direct_ptr = &AOTFunction, flags = 0 }

build:
  C++ 编译器 → .o → 静态链接

runtime call site:
  entry.direct_ptr(args) → &AOTFunction(args) → 直接执行
  ★ 零额外开销
```

### 2.2 JIT 模式（全新）

```
codegen:
  {Assembly}.cpp → RegisterMethod Binary IR 数据段（无 AOT 函数体）
  s_hotpatch_entries[slot] = { direct_ptr = &JitStub_XXX, flags = 0 }
  + kHybridMethodEntries[] 表（Binary IR ptr, token）

runtime 启动:
  RegisterJitMethodsV2():
    for each entry:
      BinaryIR → RegisterMethod（反序列化）
      构造 JitStub_XXX，attach RegisterMethod
      （不设置 kHotpatchActive）

runtime 首次调用:
  entry.direct_ptr(args) → JitStub_XXX:
    1. CAS(s_state, kUncompiled → kCompiling)
       → 胜出线程: jit::Compile(reg_m) → JitMethod
       → entry.direct_ptr = JitMethod.code
       → s_state = kCompiled
    2. 其他线程: spin-wait (T4 < 100µs)
    3. JitMethod.code(args)

runtime 后续调用:
  entry.direct_ptr(args) → JitMethod.code → 直通执行
  ★ 零额外开销

可选 Tier 0 → Tier 1（PGO 增强）:
  后台收集 profile → 重编译 → 原子替换 entry.direct_ptr
```

### 2.3 Hybrid 模式（全新）

```
codegen:
  {Assembly}.cpp → AOT C++ 函数体 + RegisterMethod Binary IR 数据段
  s_hotpatch_entries[slot] = { direct_ptr = &HybridStub_XXX, flags = 0 }
  + kHybridMethodEntries[] 表（Binary IR ptr, token, aot_symbol）

runtime 启动:
  RegisterHybridMethods():
    for each entry:
      BinaryIR → RegisterMethod
      构造 HybridStub_XXX，attach RegisterMethod + AOT entry
      s_counter = kT4Threshold

runtime 冷调用:
  entry.direct_ptr(args) → HybridStub_XXX:
    1. remain = s_counter.fetch_sub(1, relaxed)
    2. if remain == 0:
         CAS 保护 → jit::Compile(reg_m) → JitMethod
         entry.direct_ptr = JitMethod.code
    3. AOTFunction(args)  ← 冷时 AOT 原生速度

runtime 热调用（T4 已就绪）:
  entry.direct_ptr(args) → JitMethod.code → 直通
  ★ 零额外开销
  ★ 去优化回退到 AOT（比解释器快）
```

## 3. dispatch 统一模型

### 3.1 调用站点代码（所有模式共享）

```cpp
// 生成的调用站点（EmitHotpatchResolvedInvocation 一致输出）
auto& entry = s_hotpatch_entries[slot];
if (entry.flags & kHotpatchActive)          // ← 仅热更新时启用
    InterpreterEntryDirect(entry.method_key, ...);
else
    entry.direct_ptr(args);                  // ← 正常执行，所有模式统一
```

### 3.2 各模式在 hotpatch entry 中的状态

| 模式 | flags | direct_ptr | method_key | 调用效果 |
|------|-------|------------|------------|---------|
| AOT | 0 | &AOTFunction | 0 | 直通 AOT |
| JIT | 0 | &JitStub_XXX | PatchMethod* | stub 管理 JIT 编译 |
| Hybrid | 0 | &HybridStub_XXX | PatchMethod* | stub 管理计数+升级 |
| 热更激活 | kHotpatchActive | — | 替换方法 key | InterpreterEntryDirect |

## 4. 核心数据结构

### 4.1 JitPrecode（JIT 模式入口 stub）

```cpp
// src/native/jit/jit_precode.h
struct JitPrecode {
    // 状态: kUncompiled=0, kCompiling=1, kCompiled=2
    std::atomic<uint32_t> state;
    // JIT 编译所需的 RegisterMethod IR
    const interpreter::RegisterMethod* ir;
    // CompileConfig（safepoint_fn, pic_dispatch_data 等）
    jit::CompileConfig config;
};
```

### 4.2 HybridPrecode（Hybrid 模式入口 stub）

```cpp
// src/native/jit/jit_precode.h
struct HybridPrecode {
    // 调用计数器（初始 = kT4Threshold）
    std::atomic<uint32_t> call_counter;
    // JIT 目标（null = 尚未 JIT 编译）
    std::atomic<jit::JitMethod*> jit_target;
    // AOT 函数入口
    void* aot_entry;
    // JIT 编译所需的 IR
    const interpreter::RegisterMethod* ir;
    jit::CompileConfig config;
};
```

### 4.3 JitMethod（编译输出）

```cpp
// src/native/jit/jit_output.h（原名 native_method.h）
struct JitMethod {
    void* code;                     // 可执行代码指针
    uint32_t code_size;             // 代码大小
    uint32_t slot_map_count;        // GC slot map 条目数
    const uint32_t* slot_map_data;  // GC slot map 数据
    // ... 其余字段同 NativeMethod
};
```

## 5. Binary IR 格式

### 5.1 设计目标

- O(1) 反序列化（memcpy 级）
- 比 JSON 小 10x+ 
- Self-contained（不依赖外部元数据）
- Version 兼容

### 5.2 顶层结构

```
[HDR]         magic(4B) + version(2B) + flags(2B)
              + method_token(4B) + total_size(4B)
[INSTRUCTIONS] count(4B) + Instr[count]
               每个 Instr: opcode(2B) + operand1(8B) + operand2(8B)
[BLOCKS]      count(4B) + BasicBlock[count]
               每个 Block: offset(4B) + flags(4B) + instr_count(4B)
[LOCALS]      count(4B) + LocalVar[count]
               每个 Local: type(2B) + lifetime_start(4B) + lifetime_end(4B)
[CONSTANTS]   pooled_strings + float64[] + float32[] + int64[]
```

### 5.3 C# 序列化

```csharp
// src/managed/Chaos.IL2CPP.Generator/JitOutput/RegisterMethodBinarySerializer.cs
public static byte[] Serialize(RegisterMethod rm);
```

### 5.4 C++ 反序列化

```cpp
// src/native/jit/jit_binary_reader.cpp
namespace chaos::il2cpp::jit {
    bool Deserialize(const uint8_t* data, size_t size, 
                     interpreter::RegisterMethod* out_rm);
}
```

## 6. 文件清单

### 6.1 C# codegen 侧（新增/修改）

| 文件 | 操作 | 说明 |
|------|------|------|
| `Contracts/ManagedClosureArtifactModels.cs` | **修改** | 新增 `CodegenMode.Hybrid` |
| `Driver/ConvertToCpp/ConvertToCppConfig.cs` | **修改** | 新增 `--codegen-mode` CLI 参数 |
| `Generator/NativeAotLoweringPlanner.cs` | **修改** | JIT/Hybrid 模式下 emit Binary IR |
| `Generator/JitOutput/RegisterMethodBinarySerializer.cs` | **新增** | Binary IR 序列化 |
| `Generator/NativeAotEmitter.cs` | **修改** | Hybrid 模式同时输出 AOT + IR |

### 6.2 C++ runtime 侧（新增/修改）

| 文件 | 操作 | 说明 |
|------|------|------|
| `jit/` 目录（原 `codegen/`） | **重命名** | 整个目录从 codegen 改为 jit |
| `jit/jit_engine.h/cpp` | **重命名** | 入口 `jit::Compile()` |
| `jit/jit_helpers.h/cpp` | **重命名** | 辅助函数 |
| `jit/jit_output.h/cpp` | **重命名** | `JitMethod` 结构体 |
| `jit/jit_precode.h/cpp` | **新增** | JitPrecode / HybridPrecode |
| `jit/jit_binary_reader.h/cpp` | **新增** | Binary IR 反序列化 |
| `jit/jit_seh.h/cpp` | **重命名** | SEH 注册 |
| `runtime-core/jit_registration.h/cpp` | **修改** | 新增 RegisterJitMethodsV2 / RegisterHybridMethods |
| `runtime-core/codegen_bridge.h` | **修改** | 可能新增 PrecodedSlotV0 |

### 6.3 测试

| 文件 | 说明 |
|------|------|
| `testing/jit/test_precode.cpp` | JitStub / HybridStub 功能测试 |
| `testing/jit/test_binary_ir.cpp` | Binary IR 序列化/反序列化 roundtrip |
| `testing/jit/test_jit_mode.cpp` | JIT 模式端到端测试 |
| `testing/jit/test_hybrid_mode.cpp` | Hybrid 模式端到端测试 |

## 7. 三优先级权衡

| 维度 | JIT 模式 | Hybrid 模式 | AOT 模式 |
|------|----------|-------------|----------|
| **P1 性能** | 热: T4 优化代码 = AOT+ | 冷: AOT 原生；热: T4 优化 | 始终 AOT 原生 |
| | 首调用: 1x T4 编译 | 冷: +1 cycle 计数 | 首调用: 零延迟 |
| **P2 架构** | Precode stub 模型，对齐 CoreCLR | 同左 | 现有架构不变 |
| **P3 热更** | Hotpatch flags 保留 | 同左 + 去优化回退 AOT | Hotpatch flags 保留 |

## 8. 风险评估

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| Binary IR 格式不稳定 | 中 | RegisterMethod IR 演进需同步格式 | 加 version 字段，Deserialize 做兼容检查 |
| JIT 首调用延迟 > 1ms | 低 | 用户感知卡顿 | Tier 0 快速模式（减少优化 pass）+ 后台 Tier 1 |
| Precode Stub 内存膨胀 | 低 | 100k 方法 × 64 bytes = 6.4MB | 可接受，stub 复用共享模板 |
| 旧 `--mode jit` 兼容 | 中 | 现有用户迁移 | 保留旧 JIT 模式作为 deprecated 过渡 |
