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
│   └── runtime-patchdata.cpp              → Stage 2d 生成
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
              native/build/entry.exe
                    │
                    ├── (no args)        → Fact 模式: run all subjects
                    ├── --bench N [I]    → Benchmark 模式
                    ├── --hotupdate      → HotUpdate 模式
                    └── --patch-bench    → Patch + Benchmark
```

### 三个并行验证流

```
Managed 侧 (dotnet)                 Native 侧 (entry.exe)
─────────────────────               ─────────────────────

dotnet test ConvertChar.csproj      entry.exe           → Fact Assert
  ↓ xUnit Fact                        ↓ 所有 Subject_N
  Convert.ToChar(65) == 'A'           exit code 0 = Pass

dotnet run ConvertChar.csproj       entry.exe --bench N → 性能
  ↓ JIT 基线 timing                   ↓ Codegen timing

                                    entry.exe --hotupdate → 补丁
                                      ↓ 加载 patch → 再跑
```

## 7 阶段管线

`run foundation-dll verify-family` 自动执行以下阶段：

| 阶段 | 名称 | 说明 |
|------|------|------|
| 0 | Preflight | Contract 完整性检测 |
| 1a | Build subjects DLL | 生成 + 编译 Subjects DLL |
| 1b | Codegen | chaos-il2cpp convert-to-cpp → `codegen/` |
| 1c | [删除] | **不做任何后处理** |
| 1d | Patch data | 生成 `native/runtime-patchdata.cpp` |
| 2 | Build entry.exe | cmake → `native/build/entry.exe` |
| 3-7 | Fact/Audit/Bench/HU/Aggregate | 运行 + 报告 |

## 中间产物

| # | 产物 | 用途 |
|---|------|------|
| 1 | `managed/subjects/ConvertCharSubjects.dll` | il2cpp 输入 |
| 2 | `codegen/native-aot.generated.cpp` | il2cpp 翻译结果（核心产出） |
| 3 | `native/runtime-patchdata.cpp` | HotUpdate patch 数据 |
| 4 | `native/build/entry.exe` | 最终可执行 |
| 5 | `managed/ConvertChar.dll` | 托管 dotnet test/run |

## 入口命令

```bash
# 完整 7 阶段管线
run foundation-dll verify-family --family convert-char
run foundation-dll verify-family --family convert-char --mode strict

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