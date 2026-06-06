# STATUS — IL2CPP Closure 精度重构

## 元信息

```yaml
task_type: roadmap
phase: roadmap
roadmap_or_plan: docs/dev/in-progress/closure-precision-roadmap/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
source_task_id: ""  # 当前正在执行的任务（BridgeAOT fix + numerics analysis）
clearance_source: user-confirmed
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## 设计摘要

numerics chunk 的 `--full-closure` 模式下 Loader 扫描了 NuGet 缓存中的 FSharp.Core/xunit/Newtonsoft.Json 等测试框架 DLL，导致：

- AOT IR: 49,862 methods（预期 ~3,000）
- 生成代码: 1.5GB
- 构建时间: 6min+
- BRIDGE-AOT 尝试 23,648 callee，失败 23,335

根本原因链：
1. `ConvertToCppHandler.cs:76-101` 的 `CollectDependencyDlls` 扫描 `--assembly-dir` 中所有 DLL
2. CodegenOrchestrator 把 subjects DLL 构建目录（含 NuGet 传递依赖）作为 `--assembly-dir`
3. `--full-closure` 导致 Loader 加载所有发现的 DLL
4. `GenericInstantiationDemandGraph` 跨 assembly 的重复 SubjectId 导致 BuildGenericDemandLookup 的 ToDictionary 抛异常

方案：D5（Quick Win：限制 assembly-dir）→ D1（配置化 assembly 目录管理）→ D2（Subject-Rooted Call-Graph Closure）→ Wiki

## 阶段

| Phase | Status |
|-------|--------|
| D5 | not-started |
| D1 | not-started |
| D2 | not-started |
| Wiki | not-started |

## 子任务状态

| task_id | status |
|---------|--------|
| T-D5-1 | ready |
| T-D5-2 | planned |
| T-D5-3 | planned |
| T-D1-1 | planned |
| T-D1-2 | planned |
| T-D1-3 | planned |
| T-D2-1 | planned |
| T-D2-2 | planned |
| T-D2-3 | planned |
| T-D2-4 | planned |
| T-W-1 | planned |

## 最近摘要

根因分析完成。设计已拍板（D5 → D1 → D2 → Wiki）。roadmap 已创建。

## latest_stop_point

roadmap 创建完成，等待启动第一个子任务 T-D5-1。

## 下一步

启动 T-D5-1：修复 CodegenOrchestrator 的 assembly-dir
