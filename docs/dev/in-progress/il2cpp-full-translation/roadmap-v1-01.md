# Roadmap: 传统 IL2CPP 全量翻译（方案 C — 混合策略）

## 目标

将 managed DLL（System.Private.CoreLib 等）的 IL bytecode 全量翻译为语义等价的 C++ 代码，替代现有的 extern stub (`return 0`) 方案。实现真正的 IL→C++ 全量翻译，在此之上重建 Fact、Benchmark、HotUpdate 验证。

## 当前资产

| 资产 | 状态 | 位置 |
|------|------|------|
| PE 文件加载 & IL 字节码读取 | ✅ | `LoaderStage`, `InstructionDecoding.cs` |
| 跨程序集可达性分析 | ✅ | `LinkerStage.Reachability.cs` |
| IL→IR lowering（208 opcodes） | ✅ | `il_to_ir_lowerer.cpp` |
| IR→C++ emission | ✅ | `NativeAotLoweringPlanner.*.cs` |

## 当前缺口

```
Managed DLL (System.Private.CoreLib)
  │
  ▼
LoaderStage (已加载全部 method body)       ✅
  │
  ▼
LinkerStage (已追踪全部 reachable method)   ✅
  │
  ▼
Driver convert → aot-core-ir.json  ❌ 只输出 entrypoint 的 IR
  │                                   不包含 CoreLib 方法
  ▼
emit-native-aot → extern stub           ❌ 没拿到 CoreLib 的 IL
```

## 架构：三层混合模型（方案 C）

```
┌─────────────────────────────────────────────────────────┐
│ Layer 3: Cold Path (Interpreter)                        │
│ 从未被静态 reachability 触及的方法                       │
│ 保留 IL 字节码，运行时解释执行                            │
│ 反射调用 / 动态生成 / 边界情况                            │
├─────────────────────────────────────────────────────────┤
│ Layer 2: Warm Path (Dispatch AOT) — 本次 roadmap 主要工作│
│   System.Private.CoreLib.dll → corelib.generated.cpp     │
│   System.Linq.dll            → linq.generated.cpp        │
│   ... 200+ reachable DLLs    → *.generated.cpp           │
│   跨模块调用 → dispatch table (已有)                     │
│   每 assembly 独立 C++ translation unit                  │
├─────────────────────────────────────────────────────────┤
│ Layer 1: Hot Path (Inlined AOT) — 后续优化              │
│   PGO / profiling 驱动，选择高频方法内联到调用方          │
│   消除 dispatch table 间接调用开销                        │
└─────────────────────────────────────────────────────────┘
```

---

## Phase 1：全量 IR 输出（开启全闭包翻译）

### 目标

让 `chaos-il2cpp convert` 输出的 `aot-core-ir.json` 包含所有 reachable method 的 IR，而不仅仅是 entrypoint 的 IR。

### 改动

| 文件 | 改动 |
|------|------|
| `DriverEntry.cs — RunConvert()` | 传递 `FullAssemblyClosure = true` 给 pipeline |
| `PipelinePlan.cs` | 确保 `ManagedClosureRequest.FullAssemblyClosure` 被 LinkerStage 消费 |
| `LinkerStage.Reachability.cs` | 现有逻辑已支持：当 `FullAssemblyClosure=true` 时包含所有方法 |
| `CodeGenStage.cs` | 确保 `TypedIlIrArtifact` 包含所有 reachable method 的 IL body |

### 验证

```bash
# 跑 convert 后检查 IR 中的 CoreLib 方法数
python -c "
import json
ir = json.load(open('.../aot-core-ir.json'))
splib = [m for m in ir['methods'] if 'System.Private.CoreLib' in m['subjectId']]
print(f'CoreLib methods in IR: {len(splib)}')  # 期望: >0
"
```

### 风险

- **IR 体积暴增**：19 entrypoint methods → ~2000+ reachable methods
- **convert 内存占用增加**：所有 reachable method 的 IL body 需要加载到内存
- **trim 步骤需要保留 CoreLib 方法**：当前 trim 按 entry prefix 过滤，需要改为也保留所有被引用的方法

---

## Phase 2：Per-Assembly C++ Emission

### 目标

按程序集为单位生成独立 C++ 文件，每个文件包含该 assembly 中所有 reachable method 的 lowered C++ 代码。

### 改动

| 文件 | 改动 |
|------|------|
| `NativeAotLoweringPlanner.InvocationPlanning.cs` | 修改 planner 遍历所有 reachable method（不限于 entry assembly） |
| `NativeAotLoweringPlanner.MethodEmission.cs` | 修复 `CanEmitMethodBody` 逻辑，确保含指令的方法均被 emit |
| `NativeAotEmitter.cs` | 改为 `emit-native-aot` 输出多文件：每 assembly 一个 `.generated.cpp` |
| `ExternalRuntimeHelpers.InvocationAbi.cs` | 将跨程序集 `call` 解析为 dispatch table 调用，而非 extern stub |

### Dispatch Table 调用（替换 extern stub）

```
当前:  extern stub → return 0
目标:  dispatch_table::Invoke("Math.Abs", args) → real implementation
```

当两个 assembly 都已翻译：
```
A.dll → call B.dll::Foo()
  │
  ▼
生成:  #include "B.generated.cpp" 中的符号
       直接 C++ 函数调用 B_generated_Foo()
```

### 文件结构

```
il2cpp_dist/
  genuine/
    generated/
      native-aot.generated.cpp          (main entrypoint dispatch)
      System.Private.CoreLib.cpp        (CoreLib methods)
      System.Linq.cpp                   (Linq methods)
      ...
    module-registry.cfa.json            (per-module coverage)
```

### 风险

- **C++ 编译时间**：200 files × 30s each = ~100 min (可并行)
- **链接时间**：200 .obj → 1 executable
- **跨模块 ABI 一致性**：需要确保函数签名在不同 .cpp 间一致

---

## Phase 3：Metadata + GC + VTable 全覆盖

### 目标

为所有 translated types 生成完整的类型元数据、GC 栈映射、虚表，使 native exe 能正确运行翻译后的代码。

### 改动

| 领域 | 当前状态 | 目标 |
|------|---------|------|
| **TypeInfo** | 只有 entry types 有 | 所有 reachable types 都有 |
| **GC stack maps** | skeleton-only | 每个 method 有精确的 GC 根描述 |
| **VTable** | skeleton-only | 所有 virtual 方法有完整 vtable |
| **Exception handling** | try/catch lowering 已有 | 需要扩展到所有 translated methods |

---

## Phase 4：Native Correct 全绿

### 目标

`fact_l2_verifier.py` 对每个 family 的验证全部通过（checksum 匹配）。

### 改动

| 文件 | 改动 |
|------|------|
| `fact_l2_verifier.py` | 更新编译流程：编译多文件 IL2CPP output |
| `native_verify_main.cpp` | 更新 include 路径 |
| `dashboard` | Native Correct 列显示实际通过率 |

### Phase 4 的时间线

在 Phase 1-3 完成后，**每个 family 逐个验证**。从 simplest family（如 `convert-char`）开始，逐步扩展到全部 33 families。

---

## 阶段依赖

```
Phase 1: 全量 IR 输出   ← 无前置依赖
   │
   ▼
Phase 2: Per-Assembly C++  ← Phase 1 完成后
   │
   ▼
Phase 3: Metadata+GC+VTable  ← 可以与 Phase 2 部分并行
   │
   ▼
Phase 4: Native Correct 全绿  ← Phase 2+3 完成后
```

## 风险评估

| 风险 | 影响 | 缓解 |
|------|------|------|
| IR 暴增导致 OOM | 高 | 增量输出，batch 处理 method groups |
| C++ 编译时间过长 | 中 | 并行编译 200+ .cpp，增量编译 |
| 跨模块 ABI 不一致 | 高 | 统一 `AbiManifestV0` 校验，编译期 assert |
| GC 栈映射不准导致 crash | 高 | 逐步验证每个 family，Native Correct 门 |
| Dispatch table 性能开销 | 低 | Phase 4 后可通过 Hot Path 内联优化 |

## 优先级评估

| 优先级 | 评估 |
|--------|------|
| P1 性能 | 方案 C 的 Layer 2 (Dispatch AOT) 性能可接受；Layer 1 (Hot Path Inlining) 是后续优化 |
| P2 架构 | Per-assembly translation unit 清晰可维护，与 managed assembly 结构 1:1 映射 |
| P3 热更新 | Dispatch table 原生支持 patch 注入；Layer 3 (Interpreter) 无缝加载 patch IL |
