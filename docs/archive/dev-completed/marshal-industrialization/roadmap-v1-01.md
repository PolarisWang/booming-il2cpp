# P/Invoke & Marshal 工业化完善 路线图

> **面向执行 Agent：** 子任务使用 `dev-executing-plans` 执行（继承父级充分上下文，边界和接口契约已在 STATUS.md 中明确）。步骤使用复选框（`- [ ]`）语法跟踪。每个子任务达到终态时，必须先物理移动目录再同步父 roadmap。

**目标：** 将 P/Invoke & Marshal 子系统（5 marshal .cpp + 3 COM .cpp + variant，~2900 行）提升到工业级标准：补齐 native 测试覆盖（300+ tests）、修复 ICustomMarshaler 并发风险、Unity build → 独立 TU、struct_marshal 公开头文件、12 项 managed 测试、架构文档。

**范围边界：**
- 覆盖：marshal_alloc.cpp, marshal_api.cpp, marshal_string.cpp, struct_marshal.cpp, struct_marshal_descriptors.cpp, com_rcw.cpp, com_ccw.cpp, com_connection_point.cpp, marshal_api.cpp 中的 variant 实现
- 不覆盖：已有独立 COM wiki 测试计划的场景特定测试、runtime-core 的其他子系统

**架构审核模式：** critical

**结构告警重点：** 文件拆分与职责边界、重复代码（struct_marshal + marshal_api 之间的函数调用关系）、Unity build 向独立 TU 的变迁路径

**权责图审核主题：** marshal authority — 60 函数 API 入口的接口稳定性、codegen ↔ marshal descriptor ABI 合约、COM RCW/CCW 生命周期管理、hotupdate marshaler cache eviction

**问题清零来源：** brainstorm-design-confirmed (STATUS.md 已完整记录)

**计划来源：** direct-roadmap (brainstorm → roadmap 分流)

**预期知识沉淀：** `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-marshal-architecture.md`, `wiki/03-功能模块/07-COM互操作/`, `wiki/06-测试验证/模块/interop.md`

---

## 为什么需要 roadmap

8 个阶段、17 个子任务、23+ 个文件、跨多次会话持续推进。各阶段之间有强依赖关系（Phase A 测试基础设施 → Phase B/C/E/F 测试 → Phase D 代码重构 → Phase G/H 收尾），必须串行执行。

## 调度模型

`dispatch_model: sequential` — 子任务有强依赖链，串行推进。

## 阶段列表

### Phase A — 测试基础设施

| 字段 | 说明 |
|------|------|
| **goal** | 建立 marshal 测试的基础设施：测试 fixture、BSTR mock、CMake 集成 |
| **exit_criteria** | `marshal_test_fixture.h` 可用，包含真实 GC 堆初始化；`marshal_test_stubs.cpp` 编译通过；ctest 能发现 marshal 测试目标 |
| **deliverables** | marshal_test_fixture.h, marshal_test_stubs.cpp, 更新 CMakeLists.txt |
| **dependencies** | 无（起点） |
| **resolved_decisions** | 复用 BootstrapTestFixture 模式（每个测试进程独立 GC 初始化） |
| **watch_items** | GC 堆初始化复杂度可能超出预期 |

### Phase B — 核心 marshal 功能测试

| 字段 | 说明 |
|------|------|
| **goal** | 为 3 个核心 marshal 模块编写正确性 + 性能双验证测试 |
| **exit_criteria** | 3 个测试目标、~26 个测试用例全部通过 `ctest -L marshal` |
| **deliverables** | marshal_alloc_test.cpp, marshal_string_test.cpp, marshal_api_basic_test.cpp |
| **dependencies** | Phase A |
| **resolved_decisions** | 每个测试用例同时做正确性断言 + benchmark 性能锚点 |
| **watch_items** | StringNewUtf8 在测试 GC 堆中可能行为不完全 |

### Phase C — 结构体编组测试

| 字段 | 说明 |
|------|------|
| **goal** | 11 种 FieldKind 全覆盖测试 |
| **exit_criteria** | ~12 个测试用例通过，所有 FieldKind 至少一个正向 + 一个边界用例 |
| **deliverables** | struct_marshal_test.cpp |
| **dependencies** | Phase A |
| **resolved_decisions** | ManagedToNative → NativeToManaged roundtrip 作为核心测试模式 |
| **watch_items** | 嵌套结构体组合爆炸 — 关注深度 2 层即可 |

### Phase D — 模块提取 + 风险修复

| 字段 | 说明 |
|------|------|
| **goal** | marshal_internal.h 提取关键符号、从 Unity build 拆为独立 TU、struct_marshal.h 公开头文件、ICustomMarshaler 并发修复 |
| **exit_criteria** | 编译通过、ctest -L marshal 全部通过、与现有回归测试无差异 |
| **deliverables** | marshal_internal.h, struct_marshal.h, 更新 CMakeLists.txt, ICustomMarshaler 并发修复代码 |
| **dependencies** | Phase A (Phase B/C 可选并行, 见子任务表) |
| **resolved_decisions** | 先验证符号提取再改 CMake；自旋锁仅保护写路径 |
| **watch_items** | 符号暴露范围是否完整 |

### Phase E — ICustomMarshaler 测试

| 字段 | 说明 |
|------|------|
| **goal** | ICustomMarshaler 完整功能测试：resolve/cache hit-miss/cleanup/eviction |
| **exit_criteria** | ~6 个测试用例通过，与 Phase D 的并发修复联合通过 |
| **deliverables** | custom_marshaler_test.cpp |
| **dependencies** | Phase D (依赖并发修复) |
| **resolved_decisions** | 需要 mock GetRuntimeAbiV0 中的 method_invoke |

### Phase F — COM 测试

| 字段 | 说明 |
|------|------|
| **goal** | 3 个 COM 模块全覆盖测试：RCW/CCW/ConnectionPoint |
| **exit_criteria** | ~18 个测试用例通过 `ctest -L marshal` |
| **deliverables** | com_rcw_test.cpp, com_ccw_test.cpp, com_connection_point_test.cpp |
| **dependencies** | Phase A |
| **resolved_decisions** | 非 Win32 平台提供 mock；Win32 平台使用真实 COM |

### Phase G — Managed 测试补齐

| 字段 | 说明 |
|------|------|
| **goal** | 补齐 7 个测试族 ~12 项 "Not Started" 的 System.Runtime.InteropServices 管理测试 |
| **exit_criteria** | foundation-dll pipeline 全部通过 |
| **deliverables** | 7 个测试族的 managed 测试代码 |
| **dependencies** | Phase B/C/E/F (managed 测试验证 native 代码) |
| **resolved_decisions** | 使用现有 foundation-dll 管线，不创建新测试框架 |

### Phase H — 文档与收尾

| 字段 | 说明 |
|------|------|
| **goal** | 架构文档、wiki 更新、结构告警、回归测试、合并到 main |
| **exit_criteria** | 结构告警无 blocker、全回归测试通过、归档 completed + 合并提交 |
| **deliverables** | wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-marshal-architecture.md, wiki 更新, 合并 commit |
| **dependencies** | Phase A-G |
| **resolved_decisions** | 收尾使用 dev-finishing-a-development-branch |

## 子任务映射

| task_id | phase | status | purpose | depends_on | deliverables | exit_criteria | estimated_effort |
|---------|-------|--------|---------|------------|-------------|---------------|-----------------|
| a1-test-fixture | A | completed | 创建 marshal_test_fixture.h + CMake 集成 | — | marshal_test_fixture.h, CMakeLists.txt | 7/7 smoke 测试通过 | 中 |
| a2-test-stubs | A | completed | 创建 BSTR/COM mock 桩 | a1-test-fixture | marshal_test_stubs.cpp | stubs 编译链接通过 | 小 |
| b1-alloc-test | B | completed | marshal_alloc 函数测试 | a1-test-fixture | marshal_alloc_test.cpp | 8/8 tests 通过 | 中 |
| b2-string-test | B | completed | marshal_string 函数测试 | a1-test-fixture | marshal_string_test.cpp | 11 tests 通过，编码值全覆盖 | 中 |
| b3-api-basic-test | B | completed | marshal API 基础测试 | a1-test-fixture | marshal_api_basic_test.cpp | 12 tests 通过 | 中 |
| c1-struct-marshal-test | C | completed | 11 种 FieldKind 全覆盖测试 | a1-test-fixture | struct_marshal_test.cpp | 14 tests 通过 | 大 |
| d1-internal-header | D | planned | 创建 marshal_internal.h | a1-test-fixture | marshal_internal.h | 编译通过 | 小 |
| d2-struct-header | D | planned | 创建 struct_marshal.h | d1-internal-header | struct_marshal.h, struct_marshal.cpp 修改 | 编译通过，无行为变更 | 小 |
| d3-concurrent-fix | D | planned | ICustomMarshaler 自旋锁修复 | a1-test-fixture | marshal_api.cpp 修改 | 并发安全验证 | 小 |
| d4-separate-tu | D | completed | Unity build → 独立 TU | d1-internal-header | CMakeLists.txt | ctest -L marshal 全通过 | 中 |
| e1-custom-marshaler-test | E | completed | ICustomMarshaler 全功能测试 | d3-concurrent-fix | custom_marshaler_test.cpp | 6 tests 通过 | 中 |
| f1-rcw-test | F | completed | RCW 创建/缓存/释放/QI 测试 | a1-test-fixture | com_rcw_test.cpp | 8 tests 通过 | 中 |
| f2-ccw-test | F | completed | CCW 创建/aggregated 测试 | a1-test-fixture | com_ccw_test.cpp | 6 tests 通过 | 中 |
| f3-connection-point-test | F | completed | 连接点测试 | a1-test-fixture | com_connection_point_test.cpp | 4 tests 通过 | 中 |
| g1-managed-tests | G | completed | foundation-dll pipeline 验证 — 11 个 interop 测试族, 181/181 fact tests 通过 | b1-alloc-test, b2-string-test, b3-api-basic-test, c1-struct-marshal-test, e1-custom-marshaler-test, f1-rcw-test, f2-ccw-test, f3-connection-point-test | foundation-dll pipeline 结果 | 11 族 181/181 fact tests 通过 | 大 |
| h1-architecture-doc | H | planned | 创建架构文档 | g1-managed-tests | wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-marshal-architecture.md | 文档完成 | 中 |
| h2-wiki-update | H | planned | 更新 wiki 文档 | g1-managed-tests | wiki 修改 | wiki 与实现一致 | 小 |
| h3-structural-review-merge | H | planned | 结构告警 + 回归测试 + 合并 | h1-architecture-doc, h2-wiki-update | STATUS.md completed, merge commit | 全回归通过，合并到 main | 中 |

## 完成依赖图

```
a1 → a2
     ↓
b1 ─┤
b2 ─┤
b3 ─┤
c1 ─┤
f1 ─┤
f2 ─┤
f3 ─┤
     ├─ g1 → h1 → h3
     │       h2 ↗
d1 → d2 ─┤
d1 → d4 ─┤
d3 ──────┤
     ↓
e1 ──────┘
```

## 风险

| 风险 | 级别 | 缓解 |
|------|------|------|
| GC 堆初始化在测试中行为不一致 | 中 | 复用 BootstrapTestFixture 的已验证模式 |
| BSTR mock 与真实 oleaut32 行为偏差 | 低 | Win32 平台使用真实 DLL |
| 结构体编组 FieldKind 组合爆炸 | 低 | 只覆盖单字段 + 深度 2 层嵌套 |
| Managed 测试 foundation-dll 执行时间长 | 中 | 仅在收口阶段执行 |

## 备选路径

- 如果 Phase D 拆分 TU 导致编译崩溃：回退到相位 — 仅提取符号和头文件，独立 TU 推迟到后续批次
- 如果 Phase G managed 测试因外部环境问题无法执行：记录 blocker，确认受影响的 managed 接口已从 managed 侧验证，继续收尾

## 当前建议推进顺序

串行：a1-test-fixture → a2-test-stubs → (b1,b2,b3,c1,f1,f2,f3 并行备选但此处串行) → d1 → d2 → d3 → d4 → e1 → g1 → h1 → h2 → h3

## 子任务执行策略

- `child_execution_mode: auto` — 用户已授权自动推进
- `auto_continue: true` — 每个子任务完成后自动启动下一个
- `auto_stop_policy: blocking-only` — 只有 blocker 才停下来

## 权责图审核主题

| 维度 | 内容 |
|------|------|
| authority | marshal API 60 函数入口对 engine_binding.h 消费者的接口稳定性 |
| upstream | codegen 发射 StructMarshallingDescriptorV1 到 .rodata，runtime 消费 |
| downstream | engine_bridge, bootstrap, interpreter 通过 engine_binding.h 调用 marshal |
| allowed_deps | marshal 只能依赖 runtime_core 基础设施 (GC, memory_domain, string), 不得反向依赖 |
| shim_debt | struct_marshal 函数在 marshal_api.cpp 底部的跨 namespace extern 声明 (临时手法) |
| proof | 300+ native tests + 12 managed tests + 全回归 suite |
