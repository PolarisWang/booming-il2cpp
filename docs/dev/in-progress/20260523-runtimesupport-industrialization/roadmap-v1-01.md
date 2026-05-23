# RuntimeSupport 全面工业化路线图

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本 roadmap。子任务使用独立目录跟踪，串行推进。每子任务完成后必须归档子任务目录并同步父 roadmap。

**目标：** Runtime Stubs（5,434行）+ COM Interop + support 模块 + runtime-core 基础设施全面达到工业级标准。

**架构：** 6 个 Phase 串行推进，测试安全网先行，功能完整实现居中，硬化验证收尾。

**技术栈：** chaos_runtime_core, chaos_support, ska::unordered_dense, GcAllocateAtomic, stub_common.h

**架构审核模式：** critical

**结构告警重点：** 重复代码（多个 stub 有独立的 resolve_string_arg 实现）、抽象边界（stub_common.h 职责）、codegen ↔ stub ABI 一致性

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: runtime-core
- capabilityItem: runtime-support-stubs
- ownerSubjectId: n/a
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Smoke
- formalVerificationObjects: [测试套件全量通过]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（brainstorm 对话已确认所有边界）

**问题清零来源：** brainstorm-conversation（user 确认所有 6 项边界拍板）

**计划来源：** brainstorm → roadmap

**预期知识沉淀：** wiki/03-功能模块/06-il2cpp核心架构/runtime-support-stubs.md

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 1. 目标

将 RuntimeSupport 全域（Runtime Stubs、COM Interop、support 模块、runtime-core 基础设施）提升至工业化品质标准：
- 每个 stub 达到 ⭐⭐⭐⭐ 级质量（功能完整、性能最优、架构一致）
- 测试全覆盖：每文件独立测试目标
- 性能基准：高频热点建立可重复基线
- 文档化：行为边界清晰记录

## 2. 范围

- `src/native/runtime-core/runtime_stubs/*.cpp`（20 文件）
- `src/native/runtime-core/runtime_stubs/*.h`（所有头文件）
- `src/native/support/support.cpp` + `support.h`
- `src/native/runtime-core/com_ccw.cpp`、`com_rcw.cpp`、`com_connection_point.cpp`
- `src/native/runtime-core/layout_engine.cpp/h`
- `src/native/runtime-core/generic_context.cpp/h`
- `src/native/runtime-core/aot_core_ir_reader.cpp`
- `src/native/runtime-core/assembly_manager.cpp/h`
- `src/native/runtime-core/domain_unloader.cpp/h`
- `src/native/runtime-core/native_library.cpp/h`
- `src/native/runtime-core/static_var_store.cpp/h`
- `src/native/runtime-core/convert.cpp/h` + `parse_convert.cpp/h`
- `src/native/runtime-core/memory_domain.cpp/h`（快速验证）
- `src/native/runtime-core/thread_state.cpp/h`（快速验证）
- `testing/src/native/runtime-core/CMakeLists.txt`（新增测试目标）
- `testing/src/native/runtime-core/`（新增测试文件）

## 3. 非目标

- 已工业化模块（GC、Threading、VTable、Reflection、Bootstrap、Interpreter、HotUpdate、Codegen、Diagnostics）不重复覆盖
- thread_state.cpp / memory_domain.cpp 仅快速验证
- Managed RuntimeSupport（`Chaos.IL2CPP.RuntimeSupport`）不纳入（单一职责、风险低）
- Managed EngineBinding 不纳入（已有独立 baseline）

## 4. 阶段列表

### Phase 0：测试安全网（2.5周）

**Goal：** 对所有 Runtime Stubs 和 support 模块建立测试覆盖，记录当前行为 baseline

**Exit Criteria：**
- 每个 stub 文件至少有一个对应的测试文件或测试 section
- Phase 0 测试在 CHECK 配置下全部通过
- 测试覆盖确认所有 public API 的输入输出

**Deliverables：**
- `testing/src/native/runtime-core/runtime_stubs/` 目录 + 7-10 个测试文件
- `testing/src/native/runtime-core/CMakeLists.txt` 新增测试目标

**Dependencies：** 无

**Resolved Decisions：**
- 测试先行，记录当前 stub 的 baseline 行为（包括空操作、空返回值）
- Phase 1a 完成后需更新 expectation

**Watch Items：**
- 部分 stub 当前是空操作，测试只能验证"不崩溃"

### Phase 1a：空Stub完整实现 + 质量升级（4.5周）

**Goal：** 所有 ≤⭐⭐ 评级的 stub 完整实现至 ⭐⭐⭐⭐ 级，3-star 文件审计确认

**Exit Criteria：**
- convert_stubs：30+ 函数完整实现（Int32/Int64/... → string 互转 + 溢出检查 + 格式化）
- stream_stubs：20+ 函数完整实现（MemoryStream Read/Write/Seek/Flush/Close/Length 等）
- varargs_stubs：完整 __arglist 编组实现
- exception_stubs：完整异常 helper 链实现
- object_stubs：MemberwiseClone/GetHashCode/Equals/GetType 完整实现
- misc_stubs Culture 完整实现、Console/Delegate/Stacktrace 真实实现
- collection_stubs unordered_dense 改造
- 3-star 文件审计通过
- Phase 0 测试在完整实现后全量更新通过

**Deliverables：**
- 修改 10+ stub 源文件
- 新增 ~1,500-2,000 行实现代码

**Resolved Decisions：**
- String 保持 ASCII-only（已确认）
- Collection 用 unordered_dense（已确认）
- Threading/Mutex/Interlocked/Array/Char/Hashcode/Random stubs 只需审计

### Phase 1b：Stub 功能修复（2.5周）

**Goal：** 已实现 stubs 的功能完善和边界加固

**Exit Criteria：** 全部调用路径正确，日志标识到位

### Phase 2：COM Interop + support 工业化（2周）

**Goal：** COM CCW/RCW/ConnectionPoint 代码审计+测试+文档化

**Exit Criteria：**
- ANALYSIS.md 完成
- 测试覆盖全部 COM 功能
- P0/P1 问题修复
- 文档化完成

### Phase 3：基础设施工业化（3周）

**Goal：** layout_engine/generic_context/aot_core_ir_reader 等完成审计→修复→测试→归档

**Exit Criteria：** 所有子任务 ANALYSIS.md 完成、P0/P1 修复、测试通过

### Phase 4：硬化 + 文档 + 验证（2周）

**Goal：** 性能基准、LOG_WARN 标识、wiki 文档、全量回归

**Exit Criteria：**
- 性能基准可重复执行
- wiki 文档记录每个 stub 的真实状态
- 全量测试通过
- 结构告警无 blocker

## 5. 子任务映射

### Phase 0：测试安全网

| task_id | phase | status | purpose | depends_on | deliverables | estimated_effort |
|---------|-------|--------|---------|------------|--------------|-----------------|
| t0-1-string-stubs-test | Phase 0 | completed | string_stubs 15 函数单元测试（实际 19 API，77 测试） | — | string_stubs_test.cpp | 3d |
| t0-2-collection-stubs-test | Phase 0 | completed | collection_stubs List/Dict/HashSet 测试（14 API，34 测试） | — | collection_stubs_test.cpp | 2d |
| t0-3-enum-stubs-test | Phase 0 | completed | enum_stubs 全 API 测试（实际 12 API，16 测试） | — | enum_stubs_test.cpp | 2d |
| t0-4-datetime-guid-math-test | Phase 0 | planned | datetime/guid/math stubs 测试 | — | datetime_guid_math_test.cpp | 2d |
| t0-5-misc-stubs-test | Phase 0 | planned | misc_stubs Array/Culture/Console/Delegate/Gc 测试 | — | misc_stubs_test.cpp | 2d |
| t0-6-interlocked-mutex-hash-random-test | Phase 0 | planned | interlocked/mutex/hashcode/random stubs 测试 | — | interlocked_mutex_hash_random_test.cpp | 2d |
| t0-7-support-test | Phase 0 | planned | support 模块 5 函数测试 | — | support_test.cpp | 1d |

### Phase 1a：空Stub完整实现 + 质量升级

| task_id | phase | status | purpose | depends_on | deliverables | estimated_effort |
|---------|-------|--------|---------|------------|--------------|-----------------|
| t1a-1-convert-stubs | Phase 1a | planned | convert_stubs 30+ 函数完整实现 | t0-1~t0-7 | convert_stubs.cpp | 5d |
| t1a-2-stream-stubs | Phase 1a | planned | stream_stubs 20+ 函数完整实现 | t0-1~t0-7 | stream_stubs.cpp | 5d |
| t1a-3-varargs-stubs | Phase 1a | planned | varargs 完整编组实现 | t0-1~t0-7 | varargs_stubs.cpp | 4d |
| t1a-4-exception-stubs | Phase 1a | planned | exception_stubs 完整 helper 链 | t0-1~t0-7 | exception_stubs.cpp | 3d |
| t1a-5-object-stubs | Phase 1a | planned | object_stubs MemberwiseClone/GetHashCode/Equals/GetType | t0-1~t0-7 | object_stubs.cpp | 3d |
| t1a-6-misc-culture | Phase 1a | planned | misc_stubs Culture 完整实现 | t0-1~t0-7 | misc_stubs.cpp | 4d |
| t1a-7-misc-console-delegate | Phase 1a | planned | Console/Delegate/Stacktrace 真实实现 | t0-1~t0-7 | misc_stubs.cpp | 3d |
| t1a-8-collection-unordered-dense | Phase 1a | planned | collection_stubs unordered_dense 改造 | t0-1~t0-7 | collection_stubs.cpp | 2d |
| t1a-9-threadpool-events | Phase 1a | planned | threadpool_events 审计 + registration_globals 审查 | t0-1~t0-7 | threadpool_events.cpp | 1d |
| t1a-10-three-star-audit | Phase 1a | planned | 3-star 文件审计（threading/mutex/interlocked/array/char/hashcode/random） | t0-1~t0-7 | 审计报告 | 3d |
| t1a-11-test-update | Phase 1a | planned | Phase 0 测试在完整实现后全量更新 + 回归 | t1a-1~t1a-10 | 测试文件更新 | 2d |

### Phase 1b：Stub 功能修复

| task_id | phase | status | purpose | depends_on | deliverables | estimated_effort |
|---------|-------|--------|---------|------------|--------------|-----------------|
| t1b-1-collection-hash | Phase 1b | planned | Collection unordered_dense 集成验证 + edge case | t1a-8 | collection_stubs.cpp | 2d |
| t1b-2-console-stacktrace | Phase 1b | planned | ConsoleWriteLine 生产级 + StackTrace 真实实现 | t1a-7 | misc_stubs.cpp | 2d |
| t1b-3-delegate | Phase 1b | planned | Delegate 生产级实现（multicast? 只做 single） | t1a-7 | misc_stubs.cpp | 2d |
| t1b-4-culture-warn | Phase 1b | planned | Culture 入口 LOG_WARN + ordinal fallback 文档化 | t1a-6 | misc_stubs.cpp | 1d |
| t1b-5-convert-stream-varargs-fix | Phase 1b | planned | convert/stream/varargs edge case + 溢出测试 | t1a-1~t1a-3 | 3 文件 | 3d |
| t1b-6-object-exception-fix | Phase 1b | planned | object/exception edge case + finalize 协同 | t1a-4~t1a-5 | 2 文件 | 2d |
| t1b-7-regression | Phase 1b | planned | Phase 0+1a 全量回归 + 修复 | t1b-1~t1b-6 | — | 2d |

### Phase 2：COM Interop + support 工业化

| task_id | phase | status | purpose | depends_on | deliverables | estimated_effort |
|---------|-------|--------|---------|------------|--------------|-----------------|
| t2-1-com-analysis | Phase 2 | planned | COM interop ANALYSIS.md | Phase 1b | ANALYSIS.md | 3d |
| t2-2-com-tests | Phase 2 | planned | COM 测试（CCW/RCW/ConnectionPoint） | t2-1 | 测试文件 | 4d |
| t2-3-com-fixes | Phase 2 | planned | COM P0/P1 修复 | t2-2 | 3 com_*.cpp | 3d |
| t2-4-com-docs | Phase 2 | planned | COM 文档化 | t2-3 | wiki 文档 | 1d |

### Phase 3：基础设施工业化

| task_id | phase | status | purpose | depends_on | deliverables | estimated_effort |
|---------|-------|--------|---------|------------|--------------|-----------------|
| t3-1-layout-engine | Phase 3 | planned | layout_engine audit+修复+测试 | Phase 2 | ANALYSIS.md + tests | 5d |
| t3-2-generic-context | Phase 3 | planned | generic_context audit+修复+测试 | Phase 2 | ANALYSIS.md + tests | 3d |
| t3-3-aot-ir-reader | Phase 3 | planned | aot_core_ir_reader audit+测试 | Phase 2 | 测试 | 2d |
| t3-4-assembly-manager | Phase 3 | planned | assembly_manager+domain_unloader audit+测试 | Phase 2 | 测试 | 2d |
| t3-5-convert-parse | Phase 3 | planned | convert+parse_convert audit+测试 | Phase 2 | 测试 | 2d |
| t3-6-native-static | Phase 3 | planned | native_library+static_var_store audit+测试 | Phase 2 | 测试 | 1d |
| t3-7-thread-state | Phase 3 | planned | thread_state 快速验证 | Phase 2 | 验证报告 | 1d |
| t3-8-memory-domain | Phase 3 | planned | memory_domain 快速验证 | Phase 2 | 验证报告 | 1d |

### Phase 4：硬化 + 文档 + 验证

| task_id | phase | status | purpose | depends_on | deliverables | estimated_effort |
|---------|-------|--------|---------|------------|--------------|-----------------|
| t4-1-benchmarks | Phase 4 | planned | Runtime Stubs 性能基准 | Phase 3 | benchmark_test.cpp 等 | 4d |
| t4-2-log-warn | Phase 4 | planned | 所有 stub 入口 LOG_WARN 标识 | Phase 3 | 全部 stub 文件 | 2d |
| t4-3-wiki-docs | Phase 4 | planned | wiki 行为文档表 | Phase 3 | runtime-support-stubs.md | 2d |
| t4-4-regression | Phase 4 | planned | 全量 runtime-core 测试通过 | Phase 3 | — | 2d |
| t4-5-arch-review | Phase 4 | planned | 结构告警与架构审视 | Phase 3 | 报告 | 2d |

## 6. 依赖图

```
Phase 0 (t0-1~t0-7) ──→ Phase 1a (t1a-1~t1a-11) ──→ Phase 1b (t1b-1~t1b-7)
                                                          ↓
Phase 4 (t4-1~t4-5) ←── Phase 3 (t3-1~t3-8) ←── Phase 2 (t2-1~t2-4)
```

## 7. 风险

| # | 风险 | 等级 | 缓解 |
|---|------|------|------|
| R1 | unordered_dense rehash 与 GC pointer 稳定性 | 🟡 中 | key/value 为 INTPTR 整数级，rehash 移动安全 |
| R2 | Phase 0 baseline 测试在 Phase 1a 后需全部更新 | 🟢 低 | Phase 0 测试设计为"当前行为"baseline，Phase 1a 结束后统一 update expectation |
| R3 | layout_engine 874 行可能发现架构级问题 | 🟡 中 | T3-1 预留 5 天，有充足缓冲 |
| R4 | COM interop 与 managed side 耦合，隔离测试难 | 🟡 中 | T2-2 优先 native 侧 API，managed 侧用集成测试 |
| R5 | 范围过大执行周期长 | 🟡 中 | 6 个 Phase 独立 exit criteria，每 Phase 完成独立归档 |

## 8. 三优先级权衡

| 维度 | 结论 |
|------|------|
| 性能最优 | collection unordered_dense 直接提升性能；stub 实现用位运算/查表代替格式化函数；Stream 用固定栈 buffer |
| 架构完美 | 全部沿用 stub_common.h 现有约定、extern "C" ABI、GcAllocateAtomic 分配策略；不新增抽象层 |
| 热更适配 | varargs 与 hotupdate ABIs 兼容；object MemberwiseClone 在 hotupdate 替换后仍然正确；所有新 ABIs 与 GeneratedAssembly 兼容 |

三条维度**无冲突**，无需优先级裁决。

## 9. 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `recommended_next_child`: t0-2-collection-stubs-test
- 每个子任务启动前自动执行最小 `preflight self-review`
