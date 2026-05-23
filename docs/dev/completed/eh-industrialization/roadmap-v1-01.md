# EH 工业化 Roadmap v1

## 目标

将 EH（异常处理）子系统推向完美工业化标准，消除审查报告发现的所有差距项和风险隐患，实现 100% 功能完成度和生产级质量。

## 范围边界

- 全部 15 个代码差距项（GAP-01~15）
- 全部 8 个测试差距项（TEST-01~08）
- 代码质量 → 功能补全 → 测试加固 → LSDA + 文档 全链路

## 非目标

- 非 EH 相关的平台可移植性工作
- exception_stubs 完整异常类型覆盖度审计（仅对异常类型做最小化处理）
- CoreCLR 完整的 EH 调试器集成

## 阶段列表

### Phase 1：代码质量修复（P0 安全修复）

| 字段 | 值 |
|------|-----|
| **goal** | 消除所有 P0/P1 级别的代码质量缺陷，确保 EH 子系统基础健壮性 |
| **exit_criteria** | 所有 P0/P1 修复完成并经过验证；自旋锁 RAII 化、缓存跨线程安全、人格例程展开正确、POSIX 可安全失败 |
| **deliverables** | 修改后的 `t4_seh_handler.cpp/h`、`exception_jmp.h`、`exception_helpers.cpp/h` |
| **resolved_decisions** | 混合 TDD：行为变更修复先写测试，代码质量修复直接改 |
| **watch_items** | T4 handler 大量修改可能引入新回归 |

### Phase 2：功能补全

| 字段 | 值 |
|------|-----|
| **goal** | 补全 filter 完整语义、跨线程异常传播、异常对象 flood-fill 优化 |
| **exit_criteria** | Filter 全链路（macro → codegen → runtime → interpreter）通过验证；跨线程异常在 safepoint 正确重抛；flood-fill 只填充需要的寄存器槽 |
| **deliverables** | 修改 `eh.h`、`t4_seh_handler.cpp/h`、`exception_jmp.h`、`code_generator.cpp`、`fast_dispatch.cpp`、`interpreter_vm.cpp`、`ExceptionEmission.cs`、`ExceptionShapePlanning.cs`、`thread_state.cpp/h` |
| **resolved_decisions** | Filter A1（宏体系扩展）、跨线程 B1（safepoint rethrow） |
| **watch_items** | filter 跨 4 层修改，契约一致性 |

### Phase 3：测试工业化

| 字段 | 值 |
|------|-----|
| **goal** | 填补所有测试盲区，建立完整的回归保护网 |
| **exit_criteria** | T4 handler 单元测试覆盖核心函数；AOT codegen EH 运行时执行测试通过；SETJMP/WIN32_SEH 模式有等价测试；SEHStress 场景实现且通过；filter/负面/压力测试完备；benchmark CI 集成 |
| **deliverables** | `testing/src/native/codegen/t4_seh_handler_test.cpp` 新增；`tests/unit/compatibility/` 补充；`interpreter_stress_test.cpp` SEHStress 实现；CI 配置更新 |
| **resolved_decisions** | Phase 2 & 3 并行，测试先行 |
| **watch_items** | AOT 运行时执行测试可能发现 codegen EH 的隐藏问题 |

### Phase 4：SETJMP LSDA + 文档 + 最终验证

| 字段 | 值 |
|------|-----|
| **goal** | SETJMP 模式 OS 级栈回溯完整实现，wiki 更新，全量验证通过 |
| **exit_criteria** | SETJMP LSDA/ARM.extab 通过测试；wiki 评估更新完成；EH 工业化 STATUS.md 定稿；全量测试（含 foundation-dll verification pipeline + 96 family）通过 |
| **deliverables** | `unwind_info.cpp/h` 扩展；wiki 更新；`STATUS.md` 定稿 |
| **resolved_decisions** | LSDA C1（复用 DWARF 基础设施），编译期开关 |
| **watch_items** | 移动端 LSDA 体积 |

## 子任务映射

### Phase 1 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P1-A | Phase 1 | planned | RAII 自旋锁改造（RAII guard + pause + 超时 + 所有权跟踪） | — | `src/native/codegen/t4_seh_handler.cpp` | 0.5d |
| P1-B | Phase 1 | planned | TLS 缓存 generation counter（跨线程 stale entry 防护） | — | `src/native/codegen/t4_seh_handler.cpp/h` | 0.5d |
| P1-C | Phase 1 | planned | 人格例程展开传递修复（finally/fault 在 unwind pass 正确处理 + 返回值修正） | — | `src/native/codegen/t4_seh_handler.cpp` | 1d |
| P1-D | Phase 1 | planned | POSIX T4 EH 改进（nullptr crash → abort+诊断） | — | `src/native/codegen/t4_seh_handler.cpp` | 0.5d |
| P1-E | Phase 1 | planned | 资源泄漏修复（PendingFreeRegion LRU, RtlDeleteFunctionTable, AllocRuntimeFunction 生命周期） | — | `src/native/codegen/t4_seh_handler.cpp`, `unwind_info.cpp/h` | 1d |
| P1-F | Phase 1 | planned | 重复 clause 扫描提取共享辅助函数 BuildExceptionDispatch() | — | `src/native/codegen/t4_seh_handler.cpp` | 0.5d |
| P1-G | Phase 1 | planned | 编译期断言（static_assert 验证布局常量一致性） | — | `src/native/codegen/t4_seh_handler.h`, `code_generator.cpp` | 0.25d |

### Phase 2 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P2-A | Phase 2 | planned | Filter 完整语义全链路实现（macro → codegen → T4 handler → interpreter → FastExecute） | P1-A, P1-C, P1-F | `eh.h`, `t4_seh_handler.cpp`, `fast_dispatch.cpp`, `interpreter_vm.cpp`, `code_generator.cpp`, `ExceptionEmission.cs`, `ExceptionShapePlanning.cs` | 4-5d |
| P2-B | Phase 2 | planned | 跨线程异常传播（safepoint pending_exception rethrow） | — | `thread_state.cpp/h`, `exception_helpers.cpp/h` | 2d |
| P2-C | Phase 2 | planned | 异常对象 flood-fill 优化（只填充实际需要的寄存器槽） | — | `t4_seh_handler.cpp` | 0.5d |

### Phase 3 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P3-A | Phase 3 | planned | T4 SEH handler 单元测试（FindT4CodeByAddress, Register, EndFinallyHelper, 自旋锁竞争等） | — | `testing/src/native/codegen/` | 2d |
| P3-B | Phase 3 | planned | AOT codegen EH 运行时执行测试（生成代码后实际 throw→catch） | — | `tests/unit/compatibility/` | 2d |
| P3-C | Phase 3 | planned | SETJMP/WIN32_SEH 模式 sentinel 测试 | — | `testing/src/native/runtime-core/` | 1d |
| P3-D | Phase 3 | planned | 并发压力测试 SEHStress 实现 | — | `testing/src/native/interpreter/interpreter_stress_test.cpp` | 1d |
| P3-E | Phase 3 | planned | Filter 语义运行时测试 | P3-A (shared scope) | `testing/src/native/codegen/` | 1d |
| P3-F | Phase 3 | planned | 负面测试套件（未处理异常、深嵌套、finally 保证） | — | `testing/src/native/runtime-core/` | 1d |
| P3-G | Phase 3 | planned | Foundation-dll 测试补充（Assert.Throws throw+捕获） | — | `testing/foundation-dll/System.Private.CoreLib/exception-throw-diagnostics/` | 0.5d |
| P3-H | Phase 3 | planned | EH benchmark CI 集成 | — | `src/native/benchmark-host/`, CI 配置 | 0.5d |

### Phase 4 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P4-A | Phase 4 | planned | SETJMP LSDA/ARM.extab 完整实现（复用 DWARF infra，编译期开关） | Phase 1, Phase 2 | `unwind_info.cpp/h`, `config.h`, CMake | 3-4d |
| P4-B | Phase 4 | planned | Wiki 评估更新（异常处理架构、测试覆盖矩阵） | P4-A (shared) | `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/05-异常处理架构.md` | 1d |
| P4-C | Phase 4 | planned | EH 工业化 STATUS.md 定稿 | P4-A (shared) | `docs/dev/in-progress/20260523-eh-industrialization/STATUS.md` | 0.5d |
| P4-D | Phase 4 | planned | 全量验证（所有 EH 测试 + foundation-dll + 96 family + 压力测试） | 所有子任务 | 测试管线 | 1d |

## 依赖

- P1 子任务之间无严格依赖（修改文件不重叠），可按任意顺序执行
- P2 依赖 P1 完成（P2-A 依赖 P1-A/ P1-C/ P1-F 的代码质量修复）
- P3 与 P2 并行：P3-A/P3-B/P3-C/P3-D/P3-F 可在 Phase 1 完成后立即启动，不与 P2 冲突
- P3-E（Filter 测试）依赖 P2-A 完成
- P4 依赖 Phase 1/Phase 2/Phase 3 全部完成

## 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| Filter 全链路修改量大，容易契约不一致 | 中 | 高 | 先写跨层契约文档，逐层实现逐层验证 |
| T4 handler 大量代码修改引入回归 | 高 | 高 | Phase 3 并行进行，P3-A 优先启动 |
| LSDA 实现复杂度超预期 | 中 | 高 | 第一阶段只做 DWARF CIE/FDE 复用，第二阶段加 personality |
| 跨线程异常传播引入竞态 | 中 | 高 | safepoint STW 保护，严格遵循既有 pattern |
| 总工期长，执行中上下文切换成本高 | 高 | 中 | 优先使用子任务隔离，子任务完成后立即归档 |

## 备选路径

若某子任务阻塞：

1. **P2-A (Filter)** 阻塞 → 退回到 catch-all filter 行为 + 记录已知限制；不影响其他子任务
2. **P4-A (LSDA)** 阻塞 → LSDA 拆为子阶段：先做 x64 DWARF，ARM.extab 推迟到独立子任务
3. **P3-B (AOT runtime test)** 阻塞 → 至少确保解释器 EH 测试完全覆盖对应场景

## 当前建议推进顺序

1. Phase 1 子任务（P1-A → P1-B → P1-C → P1-D → P1-E → P1-F → P1-G）按任意顺序
2. Phase 2 & Phase 3 并行（先启动 P3-A/P3-B/P3-C/P3-D/P3-F，再启动 P2-A/P2-B/P2-C）
3. Phase 4（P4-A → P4-B → P4-C → P4-D）

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- 串行模式：每次一个 child，完成后自动推下一 child
- Phase 1 完成后创建 Phase 2 & Phase 3 子任务目录
- 子任务完成后物理移入 `docs/dev/completed/`

## 调度策略

`dispatch_model`: sequential

由于当前单终端执行，所有子任务按串行模式推进。Phase 2 & 3 的"并行"含义为：

1. 先完成 Phase 1
2. 按 P3-A/B/C/D/F → P2-A/B/C → P3-E/G/H 顺序推进（测试优先于实现）
3. 最后 Phase 4
