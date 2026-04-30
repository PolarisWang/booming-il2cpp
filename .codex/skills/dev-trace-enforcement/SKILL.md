---
name: trace-enforcement
description: Trace 系统强制检查 — 扫描新增/修改代码中缺少 trace 点的场景
---

# Trace 强制检查

## 检查范围

在代码审查或开发完成前，检查以下场景：

### Python 检查

1. **数据过滤/转换函数**：扫描 `_project_artifacts` 类别的过滤逻辑是否有 `trace()` 调用
2. **文件读取函数**：PATH 解析后是否记录路径 + exists
3. **状态决策函数**：gate 评估/状态跳转是否记录输入状态 + 决策分支 + 输出状态
4. **pipeline 步骤序**: 入口/出口是否记录阶段 + 耗时

### C# 检查

1. **Pipeline 步骤**：DriverEntry.cs 新增步骤是否有 `ChaosTrace.Point()` 调用
2. **数据转换**：CodeGenStage/planner 的输入/输出 method 数是否 trace
3. **外部进程调用**：dotnet build 等调用是否记录命令行 + 退出码

### C++ 检查

1. **Bridge 函数**：新增 bridge 入口/出口是否有 `CHAOS_IL2CPP_TRACE()`
2. **生成代码**：每个 method dispatch 点是否有 trace
3. **Benchmark/hotupdate**：运行结果是否有 trace

## 命令

```bash
# 查看最新 trace 会话
python -m build.toolchains.run run trace

# 按阶段过滤
python run trace --stage audit

# 按操作过滤
python run trace --op filter

# JSONL 原始输出（供其他工具消费）
python run trace --json
```

## 常用 trace 操作名

| 操作名 | 阶段 | 含义 |
|--------|------|------|
| `session_start` | — | 会话开始 |
| `derive.start` | derive | 开始推导 |
| `derive.done` | derive | 推导完成 |
| `refresh.start` | audit | 开始刷新审计 |
| `filter_artifacts` | audit | 过滤 artifact |
| `build_projects` | audit | 构建项目 |
| `evaluate_native_proof` | kernel | 评估 native-proof gate |
| `evaluate_generic_gate` | kernel | 评估通用 gate |
| `load_case_indexes` | kernel | 加载 case index |
| `load_method_contracts` | kernel | 加载 method contracts |
| `load_truth_contracts.start` | kernel | 开始加载 truth contracts |
| `load_truth_contracts.done` | kernel | 完成加载 truth contracts |
| `native_aot_runner.start` | codegen | 批量 runner 开始 |
| `native_aot_runner.done` | codegen | 批量 runner 完成 |
| `driver.run` | codegen | C# driver 运行（C# 侧） |
| `driver.emit_native_reference` | codegen | C# native reference 生成（C# 侧） |
| `driver.emit_native_aot` | codegen | C# native aot 生成（C# 侧） |
