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

dotnet test ConvertChar.csproj      entry.exe           → Fact AOT Assert
  ↓ xUnit Fact                        ↓ 所有 Subject_N
  Convert.ToChar(65) == 'A'           exit code 0 = Pass

                                    entry-jit.exe       → Fact JIT Assert
                                      ↓ 同 Subject_N，走 interpreter 路径

dotnet run ConvertChar.csproj       entry-aot.exe --bench N → AOT 性能
  ↓ JIT 基线 timing                   entry-jit.exe --bench N → JIT 性能

                                    entry-aot.exe --hotupdate → 补丁 AOT
                                    entry-jit.exe --hotupdate → 补丁 JIT
```

## 14 阶段管线

`run foundation-dll verify-family` 自动执行以下 14 阶段：

| #  | 阶段 | 名称 | 说明 |
|----|------|------|------|
| 0  | Preflight | Contract 完整性检测 | 加载 `.contract.json`，检查 family 元数据 |
| 1  | Codegen (AOT) | AOT 构建 + 保存 entry-aot.exe | chaos-il2cpp → `codegen/` → cmake → `entry.exe`，另存 `entry-aot.exe` |
| 2  | JitCodegen | JIT 构建 entry-jit.exe | codegen_mode=jit → cmake → `entry-jit.exe`，恢复 `entry.exe` |
| 3  | Fact AOT | AOT 事实验证 | `entry.exe` 无参数，验证 Assert 全部通过 |
| 4  | Fact JIT | JIT 事实验证 | `entry-jit.exe` 无参数，验证 interpreter 路径通过 |
| 5  | Audit | 机制 + 原则审计 | 检查 false-passing、stub、skip、原则对齐 |
| 6  | AsmCompare | JIT vs AOT 指令分析 | 对比 IL → IR → asm 的扩展比 |
| 7  | Microbench | 解释器内部指标 | FramePool、FastExecute、CallVirt 等微基准 |
| 8  | Benchmark | 3-way 性能对比 | managed(.NET JIT) vs entry-aot.exe vs entry-jit.exe |
| 9  | HotUpdate AOT Fact | 补丁 AOT 验证 | `entry-aot.exe --hotupdate`，验证补丁正确性 |
| 10 | HotUpdate AOT Bench | 补丁 AOT 性能 | `entry-aot.exe --hotupdate-and-benchmark` |
| 11 | HotUpdate JIT Fact | 补丁 JIT 验证 | `entry-jit.exe --hotupdate`，验证 interpreter 路径补丁 |
| 12 | HotUpdate JIT Bench | 补丁 JIT 性能 | `entry-jit.exe --hotupdate-and-benchmark` |
| 13 | Aggregate | 评分 + 报告 | 生成 `unified-verification-report.json` |

### 关键设计说明

- **两个 exe 的分工**: Codegen 阶段构建 `entry.exe` 并另存 `entry-aot.exe`；JitCodegen 阶段构建 `entry-jit.exe` 后恢复 `entry.exe` 为 AOT 版本。后续阶段按需使用 `entry-aot.exe` 或 `entry-jit.exe`。
- **JIT codegen 失败不阻塞**: JIT 构建失败不影响后续 AOT 阶段，但会在汇总中标记为 failed。
- **HotUpdate 双路验证**: 补丁同时通过 AOT dispatch 和 JIT/interpreter 两条路径验证，确保补丁机制的跨路径正确性。
- **benchmark 基线**: AOT benchmark 基线写入 `native/native-aot-benchmark.json`，JIT benchmark 基线写入 `native/native-jit-benchmark.json`。

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
# 完整 14 阶段管线
run foundation-dll verify-family --family convert-char
run foundation-dll verify-family --family convert-char --mode strict

# 指定跳过某些阶段
python build/toolchains/run/testing/foundation_dll/family_verification_orchestrator.py \
  convert-char --skip jit_codegen hotupdate_jit_fact hotupdate_jit_benchmark

# 直接运行 pipeline（仅 codegen + fact verify）
python build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py \
  --assembly System.Private.CoreLib --families convert-char

# 仅运行 entry EXE（需要先完成 codegen）
python build/toolchains/run/testing/foundation_dll/fact_verifier.py convert-char
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
