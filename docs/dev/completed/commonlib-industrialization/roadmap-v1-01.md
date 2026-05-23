# CommonLibrary 工业化完善路线图

> **面向执行 Agent：** 必须使用 dev-executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。当前任务带有 parent_task_id = commonlib-industrialization 和 source_relation: roadmap-child。执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 完成 CommonLibrary 全部 25 模块的测试覆盖、CI 集成、风险修复和文档，达到工业化标准。

**调度模式：** `sequential` — 所有阶段串行推进，每阶段依赖前一阶段的产出。

**子任务执行策略：**
- child_execution_mode: auto
- auto_continue: true
- auto_stop_policy: blocking-only

---

## 范围边界

| 包含 | 不包含 |
|------|--------|
| 16 个无测试模块的新增单元测试 | Linux/ARM64 跨平台（Windows only） |
| 已有测试模块的并发/压力测试补全 | 重构模块 API |
| json_reader 模糊测试 | 新增功能模块 |
| profile leak / trace race 等风险修复 | 性能优化（只加测试，不改实现，除风险修复外） |
| Windows CI 集成 | |
| 模块使用文档 | |

## 非目标

- 不改变现有模块的 API 签名
- 不引入新的第三方依赖
- 不做跨平台适配（保持 Windows 优先）
- 不做性能优化

## 依赖关系

```
Phase A (Core Type Tests) ──→ Phase B (Infrastructure Tests)
                                    ↓
                              Phase C (Utility Tests)
                                    ↓
                              Phase D (Stress & Fuzz Tests)
                                    ↓
                              Phase E (CI Integration)
                                    ↓
                              Phase F (Special Module Tests)
                                    ↓
                              Phase G (Risk Remediation + Docs)
```

每个阶段内部子任务可以独立执行（同一阶段内无文件冲突），但阶段之间有顺序依赖。

---

## 阶段定义

### Phase A: Core Type Tests (P0)

| 字段 | 值 |
|------|-----|
| **goal** | 覆盖 4 个核心类型模块的单元测试 |
| **exit_criteria** | 所有测试通过，行覆盖率 ≥80%（基于 Visual Studio 覆盖率工具） |
| **deliverables** | 4-5 个测试文件 + CMakeLists.txt 更新 |
| **dependencies** | 无（直接依赖 chaos_common 库） |
| **resolved_decisions** | 测试文件命名 `testing/src/native/common/common_<module>_test.cpp` |
| **watch_items** | type_info 的 static_assert 测试是否在 MSVC 和 Clang 下行为一致 |

### Phase B: Infrastructure Tests (P1)

| 字段 | 值 |
|------|-----|
| **goal** | 覆盖 5 个基础设施模块的单元测试 |
| **exit_criteria** | 所有测试通过，log/trace 多线程场景无崩溃 |
| **deliverables** | 5 个测试文件 + CMakeLists.txt 更新 |
| **dependencies** | Phase A（复用测试模式和 CMake 配置） |
| **resolved_decisions** | log 测试使用临时文件重定向 stdout |
| **watch_items** | trace ring buffer 环绕行为在多线程下的正确性 |

### Phase C: Utility Template Tests (P1)

| 字段 | 值 |
|------|-----|
| **goal** | 覆盖 6 个工具模板模块的单元测试 |
| **exit_criteria** | 所有测试通过，边界值测试覆盖 |
| **deliverables** | 5-6 个测试文件 + CMakeLists.txt 更新 |
| **dependencies** | Phase A |
| **resolved_decisions** | 模板类需要实例化多个类型组合测试 |
| **watch_items** | tls_buffer 在 CHAOS_IL2CPP_NO_TLS 和标准 TLS 下的不同行为 |

### Phase D: Stress & Concurrency Tests (P1)

| 字段 | 值 |
|------|-----|
| **goal** | 覆盖并发/压力场景 + json_reader 模糊测试 |
| **exit_criteria** | 多线程测试 100 轮无崩溃无 data race；fuzz 测试 10000+ 随机输入无崩溃 |
| **deliverables** | 5 个测试文件 + CMakeLists.txt 更新 |
| **dependencies** | Phase B（log/trace 单元测试已通过） |
| **resolved_decisions** | 压力测试使用 std::thread，超时保护；fuzz 使用自生成随机输入 |
| **watch_items** | CI 环境下多线程测试的超时设置 |

### Phase E: CI Integration (P1)

| 字段 | 值 |
|------|-----|
| **goal** | 在 Windows CI 流程中自动运行所有 CommonLibrary 测试 |
| **exit_criteria** | CI 构建包含 test_common_* 目标，提交后自动触发并报告结果 |
| **deliverables** | CI 配置文件改动 |
| **dependencies** | Phase D（所有测试存在） |
| **resolved_decisions** | CI 阶段不运行压力测试的长超时版本 |
| **watch_items** | CI 配置与现有 pipeline 的兼容性 |

### Phase F: Special Module Tests (P2)

| 字段 | 值 |
|------|-----|
| **goal** | 覆盖 asan_interface、unordered_dense、profile 启用模式的测试 |
| **exit_criteria** | 所有测试通过，profile 启用模式下 scope 计时验证正确 |
| **deliverables** | 3 个测试文件 + CMakeLists.txt 更新 |
| **dependencies** | Phase E |
| **resolved_decisions** | profile 测试需要 PROFILE 构建配置 |
| **watch_items** | ASAN 测试仅做编译验证（运行时需要 ASAN 环境） |

### Phase G: Risk Remediation + Docs (P2)

| 字段 | 值 |
|------|-----|
| **goal** | 修复已识别的 4 个风险点，补全模块使用文档 |
| **exit_criteria** | 修复提交后测试通过；文档覆盖全部 25 模块 |
| **deliverables** | 风险修复代码 + wiki 文档更新 |
| **dependencies** | Phase F（所有测试环境就绪） |
| **resolved_decisions** | trace race 修复使用原子操作或 spinlock；profile leak 增加 thread exit hook |
| **watch_items** | 修复后对性能的影响（profile 路径） |

---

## 子任务映射

| task_id | phase | status | purpose | depends_on | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|---------|------------|--------------|-------------|---------------|----------------|-----------------|
| clib-iw-a1 | A | ready | type_info + native_types 静态断言和布局测试 | — | 正确性证明 | `common_type_info_test.cpp`, `common_native_types_test.cpp` | 20+ 测试通过 | `testing/src/native/common/` | 较小 |
| clib-iw-a2 | A | ready | arithmetic + checked_conv 边界测试 | — | 正确性证明 | `common_arithmetic_test.cpp`, `common_checked_conv_test_ext.cpp` | 25+ 测试覆盖所有边界 | `testing/src/native/common/` | 较小 |
| clib-iw-b1 | B | planned | log + config 编译和行为测试 | clib-iw-a2 | 基础设施完整性 | `common_log_test.cpp`, `common_config_test.cpp` | 15+ 测试通过 | `testing/src/native/common/` | 较小 |
| clib-iw-b2 | B | planned | trace ring buffer 功能和边界测试 | clib-iw-b1 | 基础设施完整性 | `common_trace_test.cpp` | 10+ 测试通过 | `testing/src/native/common/` | 较小 |
| clib-iw-b3 | B | planned | eh + compiler_hints 编译和宏测试 | clib-iw-b2 | 基础设施完整性 | `common_eh_test.cpp`, `common_compiler_hints_test.cpp` | 10+ 测试通过 | `testing/src/native/common/` | 较小 |
| clib-iw-c1 | C | planned | load_store + ptr_tag round-trip 测试 | clib-iw-b3 | 工具层完整性 | `common_load_store_test.cpp`, `common_ptr_tag_test.cpp` | 15+ 测试覆盖边界值 | `testing/src/native/common/` | 较小 |
| clib-iw-c2 | C | planned | finally + static_init + status 测试 | clib-iw-c1 | 工具层完整性 | `common_finally_test.cpp`, `common_static_init_test.cpp`, `common_status_test.cpp` | 15+ 测试通过 | `testing/src/native/common/` | 较小 |
| clib-iw-c3 | C | planned | tls_buffer 线程隔离测试 | clib-iw-c2 | 工具层完整性 | `common_tls_buffer_test.cpp` | 5+ 测试覆盖 TLS/NO_TLS | `testing/src/native/common/` | 较小 |
| clib-iw-d1 | D | planned | log + trace 多线程压力测试 | clib-iw-c3 | 并发正确性 | `common_log_stress_test.cpp`, `common_trace_stress_test.cpp` | 100 轮无崩溃 | `testing/src/native/common/` | 中等 |
| clib-iw-d2 | D | planned | async + monitor 并发压力测试 | clib-iw-d1 | 并发正确性 | `common_async_stress_test.cpp`, `common_monitor_stress_test.cpp` | 100 轮无 data race | `testing/src/native/common/` | 中等 |
| clib-iw-d3 | D | planned | json_reader 模糊测试 | clib-iw-d2 | 鲁棒性 | `common_json_reader_fuzz_test.cpp` | 10000+ 随机输入无崩溃 | `testing/src/native/common/` | 较小 |
| clib-iw-e1 | E | planned | Windows CI 流水线集成 | clib-iw-d3 | CI 自动化 | CI 配置变更 | 提交后自动触发 | `.github/` 或等价 | 较小 |
| clib-iw-f1 | F | planned | asan_interface + unordered_dense 测试 | clib-iw-e1 | 特殊模块覆盖 | `common_asan_test.cpp`, `common_unordered_dense_test.cpp` | 10+ 测试通过 | `testing/src/native/common/` | 较小 |
| clib-iw-f2 | F | planned | profile enabled-mode 功能测试 | clib-iw-f1 | profile 正确性 | `common_profile_enabled_test.cpp` | 5+ 测试验证计时 | `testing/src/native/common/` | 较小 |
| clib-iw-g1 | G | planned | 风险修复: profile leak + trace race + async error prop + collection type safety | clib-iw-f2 | 风险清零 | `profile.h`, `trace.h`, `async.h`, `collection.h` 修改 | 修复后测试通过 | `src/native/common/chaos/` | 中等 |
| clib-iw-g2 | G | planned | 模块使用文档补全 | clib-iw-g1 | 文档完整性 | wiki 文档 | 全部 25 模块有简要使用说明 | `wiki/` | 较小 |

---

## 风险

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|----------|
| type_info static_assert MSVC 特有行为 | 低 | 中 | 在 CMakeLists 中添加编译器检查 |
| trace ring buffer 环绕导致数据竞争 | 中 | 高 | Phase D 压力测试先行复现，G1 修复 |
| CI 配置与外部系统不兼容 | 中 | 中 | 先在本地验证全部测试通过再提交 |
| fuzz 测试发现 json_reader 崩溃 | 中 | 高 | Phase D 预留额外 buffer 保护 |
| 跨会话进度丢失 | 高 | 中 | 每个子任务独立 STATUS.md + 父 roadmap 定期同步 |

## 备选路径

- **如果测试量超出预期**：每个子任务内部可进一步拆分（例如 Phase B 的 5 个模块可拆为独立子任务）
- **如果 CI 集成阻塞**：先跳过 Phase E，继续执行 F/G，回头再处理 CI
- **如果风险修复影响现有行为**：使用 feature flag 隔离修复

## 当前建议推进顺序

1. **clib-iw-a1** (type_info + native_types) → 首先启动
2. **clib-iw-a2** (arithmetic + checked_conv) → A1 之后
3. 按依赖链顺序推进至 Phase G

## 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child: clib-iw-a1`
- `preflight_check_before_start`: 每个子任务启动前做最小 preflight self-review
