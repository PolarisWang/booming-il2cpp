---
name: dev-trace-enforcement
description: Trace 系统强制检查 — 扫描新增/修改代码中缺少 trace 点的场景
---

# Trace 强制检查

## 机制

trace 强制检查已通过 `.githooks/pre-commit` 钩子自动执行。安装方式：

```bash
git config core.hooksPath .githooks
```

## 自动检查项

提交时自动扫描 `src/native/*.cpp` 中是否包含 `CHAOS_IL2CPP_TRACE()` 调用：

- 如果文件缺少 trace 点，输出 advisory warning，不阻断提交
- 开发者应确认是否需要补充 trace 点

## 手动检查场景

以下场景需要人工审查（无法通过静态扫描覆盖）：

### Python 检查

1. 数据过滤/转换函数是否在 `_project_artifacts` 过滤逻辑中有 `trace()` 调用
2. 文件读取函数在 PATH 解析后是否记录路径 + exists
3. 状态决策函数在 gate 评估/状态跳转时是否记录输入状态 + 决策分支 + 输出状态
4. pipeline 步骤在入口/出口是否记录阶段 + 耗时

### C# 检查

1. Pipeline 步骤新增是否有 `ChaosTrace.Point()` 调用
2. 数据转换如 CodeGenStage/planner 的输入/输出 method 数是否 trace
3. 外部进程调用如 dotnet build 是否记录命令行 + 退出码

### C++ 检查

1. Bridge 函数的新增入口/出口是否有 `CHAOS_IL2CPP_TRACE()`
2. 生成代码中每个 method dispatch 点是否有 trace
3. Benchmark/hotupdate 运行结果是否有 trace

## 命令

```bash
# 查看最新 trace 会话
python run trace

# 按阶段过滤
python run trace --stage audit

# 按操作过滤
python run trace --op filter

# JSONL 原始输出
python run trace --json
```
