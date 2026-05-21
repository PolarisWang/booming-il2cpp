# STATUS — 统一测试链路追踪

## Meta
- task_id: unified-test-trace
- lifecycle: completed
- phase: closed
- task_type: roadmap
- roadmap_or_plan: docs/dev/in-progress/unified-test-trace/roadmap-v1-01.md

## 最近摘要
三阶段方案全部执行完成。统一测试链路追踪系统已集成到整个测试管线。

## 完成清单

### Phase 1: 激活现有基础设施 ✅
| 项目 | 状态 |
|------|------|
| Python `trace.py` — 增加 `traceId` 字段，自动初始化 | ✅ |
| `batch_native_aot_runner.py` — 移除 try/except，`--trace` 默认启用 | ✅ |
| `batch_hotupdate_runner.py` — 同上 | ✅ |
| `batch_semantic_patch_runner.py` — 同上 | ✅ |
| `Driver/ChaosTrace.cs` — 标记已废弃，使用 Diagnostics 版本 | ✅ |
| `Diagnostics/ChaosTrace.cs` — 增加 `CHAOS_TRACE_ID` 读取 + `traceId` 字段 | ✅ |
| `Driver.csproj` — 增加 Diagnostics 引用 | ✅ |
| `DriverEntry.cs` — 增加 `ChaosTrace.InitFromEnv()` + `Point()` 调用 | ✅ |
| `CMakeLists.txt` — 加入 `trace.h` | ✅ |
| `trace.h` — 增加 `traceId` 字段支持 | ✅ |

### Phase 2: Span 层级 + Runtime 逐方法 instrument ✅
| 项目 | 状态 |
|------|------|
| Python `trace.py` — 增加 `spanId`/`parentSpanId` + span 栈管理 | ✅ |
| Python `trace_call` — 自动 span ID 生成 + 父子关系链 | ✅ |
| `RuntimeTrace.cs` — 新建，Runtime 轻量 trace 实现 | ✅ |
| `ChaosProofRunner.cs` — `TraceCall` 包裹 test 调用 | ✅ |
| `ChaosBenchmarkRunner.cs` — `TraceCall` 包裹 benchmark 调用 | ✅ |
| `Program.cs` (Runtime) — 添加 `runtime.start`/`runtime.exit` trace | ✅ |
| CLI viewer — span 树形展示（`_build_span_tree`），`traceId` 过滤 | ✅ |

### Phase 3: Runner 覆盖补齐 + Wiki ✅
| 项目 | 状态 |
|------|------|
| `native_compile_runner.py` — 增加 trace 调用 | ✅ |
| `native_benchmark_runner.py` — 增加 trace 调用 | ✅ |
| `subject_validations.py` — 增加 trace 调用 | ✅ |
| `wiki/04-工具与集成/统一追踪体系.md` — 新增 wiki 页面 | ✅ |
| `wiki/04-工具与集成/INDEX.md` — 更新引用 | ✅ |

## 关键文档
- roadmap: docs/dev/in-progress/unified-test-trace/roadmap-v1-01.md
- wiki: wiki/04-工具与集成/统一追踪体系.md

## 变更文件清单
- `build/toolchains/run/testing/trace.py`
- `build/toolchains/run/commands/trace.py`
- `build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py`
- `build/toolchains/run/testing/foundation_dll/batch_hotupdate_runner.py`
- `build/toolchains/run/testing/foundation_dll/batch_semantic_patch_runner.py`
- `build/toolchains/run/testing/foundation_dll/native_compile_runner.py`
- `build/toolchains/run/testing/foundation_dll/native_benchmark_runner.py`
- `build/toolchains/run/testing/subject_validations.py`
- `src/managed/Chaos.IL2CPP.Diagnostics/ChaosTrace.cs`
- `src/managed/Chaos.IL2CPP.Driver/ChaosTrace.cs` (deprecated)
- `src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj`
- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
- `src/native/common/CMakeLists.txt`
- `src/native/common/chaos/trace.h`
- `src/reference/Chaos.TestFramework.Runtime/RuntimeTrace.cs` (new)
- `src/reference/Chaos.TestFramework.Runtime/Program.cs`
- `src/reference/Chaos.TestFramework.Runtime/Hosts/ChaosProofRunner.cs`
- `src/reference/Chaos.TestFramework.Runtime/Hosts/ChaosBenchmarkRunner.cs`
- `wiki/04-工具与集成/统一追踪体系.md` (new)
- `wiki/04-工具与集成/INDEX.md`
