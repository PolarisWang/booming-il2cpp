# Foundation DLL Family 验证 Gate 流程

## 概述

Foundation DLL 验证管线将 C# 入口点 DLL 通过 il2cpp 翻译为原生可执行文件，直接运行来验证 Assert 全部通过。

## 目录结构（Option 1）

每个 family 的验证目录统一如下：

```
convert-char/
│
├── managed/
│   ├── subjects/                          ← [自动生成] il2cpp 输入
│   │   ├── ConvertCharSubjects.cs         → Subject_N + Run(int)
│   │   └── ConvertCharSubjects.csproj     → Library 输出
│   │
│   ├── ConvertChar.csproj                 → 包含 tests + benchmark
│   ├── ConvertCharTests.cs                ← [手写] xUnit Fact
│   ├── ConvertCharBenchmark.cs            ← [手写] 基准
│   │
│   └── patch/                             ← [手写] 独立工程
│       ├── ConvertCharPatch.csproj
│       └── ConvertCharPatchEntry.cs       → Patch 存根
│
├── codegen/                               ← [il2cpp 产出] 纯翻译结果
│   └── native-aot.generated.cpp           ← 仅此一个文件，零后处理
│
├── native/                                ← [执行入口] Native wrapper
│   ├── CMakeLists.txt
│   ├── runtime-entry.cpp
│   ├── runtime-patchdata.cpp              → Stage 2d 生成
│   ├── entry.exe                          (AOT dispatch, 默认)
│   ├── entry-aot.exe                      (AOT dispatch, 显式副本)
│   └── entry-jit.exe                      (JIT/interpreter dispatch)
│
└── reports/                               ← 验证报告
```

## 架构

```
                    managed/
                    subjects/ConvertCharSubjects.csproj
                    │
                    │ (auto-gen subjects)
                    │
                    ▼
              ConvertCharSubjects.dll
                    │
                    ├── chaos-il2cpp convert-to-cpp ──→ codegen/native-aot.generated.cpp
                    │
                    ▼
              native/CMakeLists.txt
              + native/runtime-entry.cpp
                    │
                    cmake + cl
                    ▼
              native/build/entry.exe        (AOT, 默认)
              native/build/entry-aot.exe    (AOT, 显式)
              native/build/entry-jit.exe    (JIT/interpreter)
                    │
                    ├── (no args)        → Fact AOT 模式
                    ├── --bench N [I]    → Benchmark 模式
                    ├── --hotupdate      → HotUpdate 模式
                    └── --hotupdate-and-benchmark N [I] → Patch + Benchmark
```

### 三个并行验证流

```
Managed 侧 (dotnet)                 Native 侧 (entry.exe)
─────────────────────               ─────────────────────

dotnet run -f net8.0                entry-aot.exe       → Fact AOT Assert
  ManagedFactHarness                  ↓ 所有 Subject_N
  ↓ 自动生成 C# harness               exit code 0 = Pass
  逐方法 try-catch 验证
                                    entry-jit.exe       → Fact JIT Assert
                                      ↓ 同 Subject_N，走 interpreter 路径

dotnet run -f net8.0/net10.0       entry-aot.exe --bench N → AOT 性能
  ↓ 多端 benchmark harness           entry-jit.exe --bench N → JIT 性能
  .NET8 JIT / .NET10 JIT / Mono

                                    entry-aot.exe --hotupdate → 补丁 AOT
                                    entry-jit.exe --hotupdate → 补丁 JIT
```

## 13 阶段管线

`python _core/python/cli.py` 自动执行以下 13 阶段：

| #  | 阶段 | 名称 | 说明 |
|----|------|------|------|
| 0  | preflight | Contract 完整性检测 | 加载 `contract.json`，检查 family 元数据 |
| 1  | codegen | Codegen (AOT) | chaos-il2cpp → `codegen/` → cmake → `entry.exe`，另存 `entry-aot.exe` |
| 2  | jit_codegen | JitCodegen | codegen_mode=jit → cmake → `entry-jit.exe`，恢复 `entry.exe` |
| 3  | fact | Fact AOT | `entry-aot.exe` 无参数，验证 Assert 全部通过 |
| 4  | fact_jit | Fact JIT | `entry-jit.exe` 无参数，验证 interpreter 路径通过 |
| 5  | audit | 机制 + 原则审计 | 检查 false-passing、stub、skip、原则对齐 |
| 6  | asm_compare | AsmCompare (JIT vs AOT) | 对比 IL → IR → asm 的扩展比 |
| 7  | microbench | Microbench (Interpreter) | FramePool、FastExecute、CallVirt 等微基准 |
| 8  | benchmark | Benchmark (3-way) | Chaos AOT / Chaos JIT / Interpreter 三端性能对比 |
| 9  | hotupdate | HotUpdate AOT Fact | `entry-aot.exe --hotupdate`，验证补丁正确性 |
| 10 | hotupdate_aot_benchmark | HotUpdate AOT Bench | `entry-aot.exe --hotupdate-and-benchmark` |
| 11 | hotupdate_jit_fact | HotUpdate JIT Fact | `entry-jit.exe --hotupdate`，验证 interpreter 路径补丁 |
| 12 | hotupdate_jit_benchmark | HotUpdate JIT Bench | `entry-jit.exe --hotupdate-and-benchmark` |

> **注意**：旧版管线包含独立的 .NET8 Managed Fact、Dashboard 和 Aggregate 阶段。在当前 13 阶段管线中，Managed Fact 已合并到 Fact AOT/JIT 阶段（无需额外 .NET SDK 依赖）；Dashboard 和 Aggregate 由 orchestrator 在内部调用 `aggregate.py` 生成，不再是独立阶段。

### 关键设计说明

- **双路事实验证**: Stage 3 (Chaos AOT) + Stage 4 (Chaos JIT) 两层验证确保翻译语义与原生运行时一致。
- **3 路 Benchmark**: Stage 8 通过 Chaos AOT / Chaos JIT / Interpreter 三端性能对比。
- **两个 exe 的分工**: Codegen 阶段构建 `entry.exe` 并另存 `entry-aot.exe`；JitCodegen 阶段构建 `entry-jit.exe` 后恢复 `entry.exe` 为 AOT 版本。后续阶段按需使用 `entry-aot.exe` 或 `entry-jit.exe`。
- **JIT codegen 失败不阻塞**: JIT 构建失败不影响后续 AOT 阶段，但会在汇总中标记为 failed。
- **HotUpdate 双路验证**: 补丁同时通过 AOT dispatch 和 JIT/interpreter 两条路径验证，确保补丁机制的跨路径正确性。
- **aggregate 内部汇总**: 所有阶段执行完毕后，orchestrator 调用 `aggregate.py` 生成 `UnifiedReport`，包含覆盖率统计、dashboard 数据和回归检测。这一过程在 orchestrator 内部完成，不是独立阶段。

## 中间产物

| # | 产物 | 用途 |
|---|------|------|
| 1 | `managed/subjects/ConvertCharSubjects.dll` | il2cpp 输入 |
| 2 | `codegen/native-aot.generated.cpp` | il2cpp 翻译结果（核心产出） |
| 3 | `native/runtime-patchdata.cpp` | HotUpdate patch 数据 |
| 4 | `native/build/entry.exe` | AOT dispatch 可执行 |
| 5 | `native/build/entry-aot.exe` | AOT dispatch 显式副本 |
| 6 | `native/build/entry-jit.exe` | JIT/interpreter dispatch 可执行 |
| 7 | `managed/ConvertChar.dll` | 托管 dotnet test/run |

## 入口命令

```bash
# 完整 13 阶段管线（标准模式）
python _core/python/cli.py --slug convert-char --assembly System.Private.CoreLib

# 严格模式（强制 hotupdate 阶段必须通过）
python _core/python/cli.py --slug convert-char --assembly System.Private.CoreLib --mode strict

# 指定跳过某些阶段
python _core/python/cli.py --slug convert-char --assembly System.Private.CoreLib \
  --skip jit_codegen hotupdate_jit_fact hotupdate_jit_benchmark

# 全量运行所有 families
python _core/python/batch_run_all.py

# CI smoke（仅关键 families + 快速模式）
python _core/python/ci_smoke.py
```

## Handwrite 源覆盖保护

`handwritten/` 目录是管线的**只读源**：

| 路径 | 访问模式 | 说明 |
|------|---------|------|
| `handwritten/` | **READ ONLY** | 管线仅读 `.cs` 文件，绝不修改 |
| `managed/subjects/` | WRITABLE | auto-generate 可覆盖 |
| `managed/ConvertCharTests.cs` | **READ ONLY** | 手写，仅首次自动生成骨架 |
| `managed/ConvertCharBenchmark.cs` | **READ ONLY** | 手写，仅首次自动生成骨架 |
| `managed/patch/ConvertCharPatchEntry.cs` | **READ ONLY** | 完全手写 |

## 关键原则

1. **subjects 独立工程** — 与 tests/benchmark 分离，避免 il2cpp 闭包扫描拖入 xUnit 等依赖
2. **codegen 零后处理** — `native-aot.generated.cpp` 是 il2cpp 翻译能力的真实凭证，不被外部脚本修改
3. **命名清晰** — `Subjects` 前缀表示"被测方法"，`Tests` 表示 Fact，`Benchmark` 表示性能，`PatchEntry` 表示热更新

## 与旧结构的区别

| 维度 | 旧结构 | Option 1 |
|------|--------|----------|
| il2cpp 输入 | `il2cpp_dist/entrypoint/ConvertCharNativeEntry.cs` | `managed/subjects/ConvertCharSubjects.cs` |
| codegen 输出 | `il2cpp_dist/genuine/<Asm>/generated/native-aot.generated.cpp` | `codegen/native-aot.generated.cpp` |
| native wrapper | 混在 genuine/ 树中 | 独立 `native/` 工程 |
| 后处理 | 5 个 patch 脚本修改生成的 .cpp | **无**（零后处理） |
| 工程数 | 4 个分散 | 3 个（subjects + main + patch） |
