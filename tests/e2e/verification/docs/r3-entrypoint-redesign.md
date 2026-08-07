# R3: Entrypoint 工程方案重构

## 问题

当前每个 family 验证需要经历：
1. **PROBE 阶段**: 生成 C# probe 代码 → 编译 → 运行捕获返回值
2. **EMIT 阶段**: 根据 probe 结果生成真实 entry 代码 → 编译
3. **MSBuild 项目生成**: 为每个 family 动态生成 `.csproj`
4. **runtime-entry.cpp 生成**: 从 Python 模板生成 native 入口

这导致 `family_entrypoint.py` (1079 行) + `test_code_generator.py` (2277 行) 共 ~3350 行代码，且每次验证都需要完整 C# 编译流程。

## 方案：预编译统一 Runner

### 核心思路

用一个**预编译的 C# 控制台项目**替代 per-family 代码生成，运行时通过 JSON manifest 加载契约和预期值。

### 架构

```
managed/
  Chaos.Verification.Runner/          ← 预编译，不变
    Chaos.Verification.Runner.csproj  ← 固定，引用 xunit + Sdk
    Program.cs                        ← 固定，读取 manifest.json
    probe.cs                          ← 固定，反射调用目标方法 + 捕获返回值
    emit.cs                           ← 固定，Assert.Equal / Assert.Throws

每个 family 只生成:
  manifest.json                       ← 方法列表 + 参数 + 预期返回值（由 Python 直接生成，无需编译）
```

### 工作流

```
旧流程:                                新流程:
1. Python 生成 C# code               1. Python 生成 manifest.json
2. dotnet build (probe)              2. Runner.exe --probe manifest.json  (反射调用)
3. probe.exe → 捕获返回值             3. Runner 输出 probe_result.json
4. Python 读取 probe 结果            4. Python 读取 probe_result.json
5. Python 生成 emit C# code          5. Python 写入预期值到 manifest.json
6. dotnet build (emit)               6. Runner.exe --verify manifest.json  (断言验证)
7. entry.exe → 验证
```

### 优点

| 维度 | 当前方案 | 新方案 |
|------|---------|--------|
| 代码量 | ~3350 行 (test_code_generator + family_entrypoint) | ~500 行固定 runner + ~50 行 Python manifest 生成 |
| 编译次数 | 每次验证 2-3 次 dotnet build | 0 次（runner 已预编译） |
| 验证时间 | 每次增加 10-30s 编译开销 | 无编译开销 |
| 维护成本 | 修改 C# 逻辑需改 Python 字符串模板 | 直接在 C# 项目改 |
| 灵活性 | 每个 family 生成独立 exe | 单 exe + JSON 驱动 |

### 缺点与缓解

| 风险 | 缓解 |
|------|------|
| 反射调用无法处理所有方法签名 |保留退路：对无法反射的方法生成专用 C# 桩（只有这些需要编译） |
| 需要额外维护一个 C# 项目 | 项目极小（3 文件），且不经常变动 |
| JSON manifest 契约版本化 | manifest 包含 schema version 字段 |
| 当前 `METHOD_OVERRIDES` 特殊处理逻辑 | 迁移到 manifest 的 `overrides` 字段 |

### 实施步骤

1. 创建 `Chaos.Verification.Runner` C# 项目（Program.cs + probe.cs + emit.cs）
2. 实现 JSON manifest schema（v1）
3. 实现反射 probe：给定 methodSubjectId → 反射调用 → 返回值序列化
4. 实现 emit verify：给定 methodSubjectId + expected → Assert.Equal/Throws
5. Python 端：`generate_entrypoint_source()` → 简化/删除，改为 `generate_manifest()`
6. Python 端：`generate_and_build()` → 改为 `run_verification_runner()`
7. 渐进式迁移：先对 1-2 个 family 启用新路径，验证通过后再全量切换

### 保留项

- `runtime-entry.cpp` 生成机制（`generate_runtime_entry`）保持不变 — 它生成的是 native AOT 入口，与 managed 验证无关
- 对无法通过反射调用的边缘方法保留当前代码生成路径作为 fallback
