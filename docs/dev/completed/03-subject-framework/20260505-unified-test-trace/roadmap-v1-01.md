# Roadmap: 统一测试链路追踪

## 目标
基于现有但未充分利用的 trace 基础设施，打通 Python/C#/C++ 三层测试链路的日志与 span 追踪，使开发者能通过统一方式追查整条链路上的问题。

## 范围边界
- 只涉及 trace 系统的激活、整合和扩展
- 不改变现有测试逻辑和断言行为
- 不引入新的外部依赖

## 非目标
- 不修改生成的 C++ 运行时代码
- 不引入 OpenTelemetry 等外部可观测性框架
- 不改变测试框架的断言 API

## 阶段列表

### Phase 1: 激活现有基础设施（方案 A）

- **goal**: 把已有的 trace 基础设施真正用起来
- **exit_criteria**: Python trace 自动启用；C# 重复 ChaosTrace 已合并、Driver 和 TestFramework.Runtime 有 trace 调用；C++ trace.h 已加入编译并启用
- **deliverables**:
  1. `build/toolchains/run/testing/trace.py` — 增加 traceId 字段，自动初始化
  2. `build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py` — 移除 try/except，`--trace` 默认启用
  3. `src/managed/Chaos.IL2CPP.Driver/ChaosTrace.cs` — 删除，统一用 Diagnostics 版
  4. `src/managed/Chaos.IL2CPP.Diagnostics/ChaosTrace.cs` — 增加 CHAOS_TRACE_ID 读取
  5. `src/managed/Chaos.IL2CPP.Driver/Program.cs` — 添加 InitFromEnv() 和 trace Point() 调用
  6. `src/native/common/CMakeLists.txt` — 加入 trace.h
  7. `src/native/common/chaos/trace.h` — 增加 traceId 字段支持
- **dependencies**: 无
- **resolved_decisions**: C# 统一使用 Diagnostics 版本的 ChaosTrace；Python trace 自动启用不降级

### Phase 2: Span 层级与 TestFramework 逐方法 instrument（方案 B 核心）

- **goal**: 建立完整的 span 层级，每个测试方法调用自动 trace
- **exit_criteria**: Python/C#/C++ 三层统一 traceId+spanId 传播；TestFramework.Runtime 每个 test/benchmark 调用都有 trace 入口/出口；CLI viewer 支持 span 树形展示
- **deliverables**:
  1. `build/toolchains/run/testing/trace.py` — 增加 spanId/parentSpanId
  2. `src/managed/Chaos.IL2CPP.Diagnostics/ChaosTrace.cs` — 增加 spanId/parentSpanId
  3. `src/reference/Chaos.TestFramework.Runtime/Hosts/ChaosProofRunner.cs` — TraceCall 包裹 test 调用
  4. `src/reference/Chaos.TestFramework.Runtime/Hosts/ChaosBenchmarkRunner.cs` — TraceCall 包裹 benchmark 调用
  5. `build/toolchains/run/commands/trace.py` — span 树形展示过滤
  6. `build/toolchains/run/core/operation_reporting.py` — runId 注入 trace
- **dependencies**: Phase 1

### Phase 3: Runner 覆盖补齐与 viewer 增强 + wiki 更新

- **goal**: 补齐所有 runner 的 trace 覆盖，viewer 树形展示，更新 wiki
- **exit_criteria**: 所有 batch/native/hotupdate runner 都有 trace；viewer 支持 span 树和按 traceId 过滤；wiki 已更新
- **deliverables**:
  1. `build/toolchains/run/testing/foundation_dll/native_compile_runner.py` — 添加 trace 调用
  2. `build/toolchains/run/testing/foundation_dll/native_benchmark_runner.py` — 添加 trace 调用
  3. `build/toolchains/run/testing/foundation_dll/hotupdate_verification_runner.py` — 添加 trace 调用
  4. `build/toolchains/run/testing/foundation_dll/batch_hotupdate_runner.py` — 移除 try/except trace
  5. `build/toolchains/run/testing/foundation_dll/batch_semantic_patch_runner.py` — 移除 try/except trace
  6. `build/toolchains/run/testing/subject_validations.py` — 添加 trace 调用
  7. `build/toolchains/run/commands/trace.py` — span 树形展示增强
  8. `wiki/04-工具与集成/统一追踪体系.md` — 新页面
  9. `wiki/04-工具与集成/INDEX.md` — 更新
- **dependencies**: Phase 2

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | estimated_effort |
|---------|-------|--------|-------|---------|------------|-----------------|
| p1-core-trace | 1 | ready | main | 激活 C#/C++ trace 代码，统一 traceId | 无 | 中等 |
| p2-span-schema | 2 | planned | main | 扩展 span 层级 schema，TestFramework 逐方法 instrument | p1-core-trace | 大 |
| p3-runner-coverage | 3 | planned | main | 补齐 runner trace 覆盖，viewer 增强，wiki 更新 | p2-span-schema | 中等 |

## 依赖
- 串行执行：p1 → p2 → p3

## 风险
| 风险 | 可能性 | 缓解 |
|------|--------|------|
| C# ChaosTrace 合并后行为不一致 | 低 | 保留原有接口签名，仅增加新字段 |
| C++ trace.h 编译问题 | 低 | 仅在 Debug 构建中启用 |
| trace 文件过大 | 低 | ring buffer 256 条目上限；File.AppendAllText 异步追加 |

## 当前建议推进顺序
Phase 1 → Phase 2 → Phase 3
