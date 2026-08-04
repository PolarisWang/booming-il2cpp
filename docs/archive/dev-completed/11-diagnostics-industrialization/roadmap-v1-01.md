# Diagnostics 工业化 Roadmap v1

## 目标

将 Diagnostics（诊断）子系统推向完美工业化标准，消除评估报告发现的所有 29 项差距项和风险隐患，实现 100% 功能完成度和生产级质量。

## 范围边界

- 全部构建系统差距（B-01~03）
- 全部架构差距（A-01~03）
- 全部代码质量差距（C-01~12）
- 全部测试差距（T-01~07）
- 全部文档差距（W-01~04）
- CommonLibrary 诊断（log/trace/profile）回顾性审查

## 非目标

- 不重新设计诊断架构
- 不替换现有日志/追踪/性能分析系统
- 不改 ABI 合约
- 不做 EventPipe Linux/macOS 跨平台（Windows-only，后续再议）
- 不实现 DAP 完整规范的全部功能（只补全评估报告标记的功能断点）

## 阶段列表

### Phase 1：构建修复 + 代码质量（P0/P1 安全修复）

| 字段 | 值 |
|------|-----|
| **goal** | 消除所有 P0/P1 级别的构建和代码质量缺陷，确保诊断子系统可编译、无 data race、基础代码质量达标 |
| **exit_criteria** | 所有 P0/P1 修复完成并经过验证；DAP/EventPipe/SOS 加入构建且编译通过；废弃 v1 debug_agent；线程安全修复完成；CommonLibrary 回顾无严重问题 |
| **deliverables** | 修改后的 `diagnostics/CMakeLists.txt`、`dbg_breakpoint.cpp/h`、`dbg_server.cpp`、`dbg_runtime.cpp`、`ep_event.cpp`、`ep_gc_bridge.cpp`、`chaos_diag.cpp`；删除 `debug_agent` 相关文件 |
| **resolved_decisions** | v1 debug_agent → 废弃、v2 DAP → 启用；串行执行 |
| **watch_items** | DAP 代码长期未编译，首次编译可能暴露大量编译错误 |

### Phase 2：功能补全

| 字段 | 值 |
|------|-----|
| **goal** | 补全 DAP 断点功能、变量展开、evaluate 支持、EventPipe GC 真实数据 |
| **exit_criteria** | setBreakpoints 从存根→完整实现（source→IL offset）；变量展开支持嵌套对象；EventPipe GC 事件字段非零；pause 命令实现 |
| **deliverables** | `dbg_server.cpp`、`dbg_breakpoint.cpp/h`、`dbg_variables.cpp/h`、`ep_gc_bridge.cpp`、`dbg_runtime.cpp` |
| **resolved_decisions** | DAP setBreakpoints 采用 source→IL offset lookup（复用既有 metadata API） |
| **watch_items** | source→IL offset 映射可能涉及 codegen 层 metadata 调整 |

### Phase 3：测试工业化

| 字段 | 值 |
|------|-----|
| **goal** | 填补所有测试盲区，建立完整的回归保护网 |
| **exit_criteria** | DAP 调试器 30+ 单元测试 + 10+ 集成测试；EventPipe 20+ 单元测试；SOS 5+ 测试；CommonLibrary 补充 15+ 测试；负面/压力/性能 20+ 测试；CI pipeline 集成 |
| **deliverables** | `testing/src/native/diagnostics/` 新增测试目录和相关测试文件；CI 配置更新 |
| **resolved_decisions** | 测试框架沿用 GoogleTest（C++）/pytest（Python），与项目现有保持一致 |
| **watch_items** | DAP 集成测试需要模拟解释器线程交互 |

### Phase 4：文档 + 验证

| 字段 | 值 |
|------|-----|
| **goal** | Wiki 文档完备，全量验证通过，归档 |
| **exit_criteria** | Wiki 新增诊断子系统架构/DAP 使用/EventPipe 协议/SOS 命令文档；全量编译+测试+CI 通过；任务目录归档 |
| **deliverables** | Wiki 文档更新；STATUS.md 定稿；任务目录移入 completed/ |
| **resolved_decisions** | Wiki 放在 `06-il2cpp核心架构/` 下 |
| **watch_items** | 无 |

## 子任务映射

### Phase 1 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P1-A | Phase 1 | completed | 废弃 debug_agent + 清理 CMake (加入 debugger/DAP + EventPipe + SOS) | — | `src/native/diagnostics/CMakeLists.txt`, `src/native/diagnostics/debug_agent.*`, `debug_breakpoints.*`, `debug_stack_walk.*`, `debug_variables.*`, `debug_commands.h` | 1d |
| P1-B | Phase 1 | completed | DAP 线程安全修复（无锁map、全局buffer、帧快照、自旋锁→condvar） | P1-A | `src/native/diagnostics/debugger/dbg_breakpoint.cpp/h`, `dbg_server.cpp`, `dbg_runtime.cpp` | 1.5d |
| P1-C | Phase 1 | completed | DAP 代码质量修复（atoi→strtol, const_cast, dead code, 魔法常量） | P1-A | `src/native/diagnostics/debugger/dbg_server.cpp`, `dbg_protocol.h` | 0.5d |
| P1-D | Phase 1 | completed | EventPipe 代码质量修复（strncpy终止+helper、GC零字段、I/O检查） | P1-A | `src/native/diagnostics/eventpipe/ep_event.cpp`, `ep_gc_bridge.cpp`, `ep_transport.cpp`, `receiver/chaos_diag.cpp` | 1d |
| P1-E | Phase 1 | completed | SOS 加入构建 + 编译通过 | P1-A | `src/native/diagnostics/sos/gc_sos.cpp`, `diagnostics/CMakeLists.txt` | 0.5d |
| P1-F | Phase 1 | completed | CommonLibrary 诊断回顾审查（验证工业化完成状态，确认无回归） | — | `src/native/common/chaos/log.h`, `trace.h`, `profile.h` | 0.5d |

### Phase 2 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P2-A | Phase 2 | completed | DAP setBreakpoints 完整实现（source行号→IL offset映射） | Phase 1 | `src/native/diagnostics/debugger/dbg_server.cpp`, `dbg_breakpoint.cpp/h` | 2d |
| P2-B | Phase 2 | completed | DAP 变量展开 + evaluate 支持 | P2-A | `src/native/diagnostics/debugger/dbg_variables.cpp/h`, `dbg_server.cpp` | 2d |
| P2-C | Phase 2 | completed | EventPipe GC 真实数据接入 | Phase 1 | `src/native/diagnostics/eventpipe/ep_gc_bridge.cpp` | 1d |
| P2-D | Phase 2 | completed | DAP pause 命令 + condition variable 替换自旋 | P1-B | `src/native/diagnostics/debugger/dbg_runtime.cpp`, `dbg_server.cpp` | 1d |

### Phase 3 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P3-A | Phase 3 | planned | DAP 调试器单元测试（protocol/breakpoints/stepping/stack/variables） | Phase 2 | `testing/src/native/diagnostics/` | 2d |
| P3-B | Phase 3 | planned | DAP 集成测试（server message loop/transport/全链路） | P3-A | `testing/src/native/diagnostics/` | 1.5d |
| P3-C | Phase 3 | completed | EventPipe 单元测试（transport/protocol/platform/bridges/instance） | Phase 1 | `testing/src/native/diagnostics/` | 1.5d |
| P3-D | Phase 3 | planned | EventPipe receiver 测试 | P3-C | `testing/src/native/diagnostics/` | 0.5d |
| P3-E | Phase 3 | planned | SOS 扩展测试 | Phase 1 | `testing/src/native/diagnostics/` | 0.5d |
| P3-F | Phase 3 | planned | CommonLibrary 诊断补充测试 | P1-F | `testing/src/native/common/` | 1d |
| P3-G | Phase 3 | completed | 负面/压力/性能测试 | Phase 2 | `testing/src/native/diagnostics/` | 1.5d |
| P3-H | Phase 3 | completed | CI 集成（诊断回归 pipeline） | P3-A~G | `.github/workflows/` | 0.5d |

### Phase 4 子任务

| task_id | phase | status | purpose | depends_on | conflict_scope | estimated_effort |
|---------|-------|--------|---------|-----------|---------------|-----------------|
| P4-A | Phase 4 | completed | Wiki: 诊断子系统架构文档 | Phase 1~3 | `wiki/03-功能模块/` | 1d |
| P4-B | Phase 4 | completed | Wiki: DAP 调试器使用说明 | P4-A | `wiki/` | 0.5d |
| P4-C | Phase 4 | completed | Wiki: EventPipe 协议 + 使用 | P4-A | `wiki/` | 0.5d |
| P4-D | Phase 4 | completed | Wiki: SOS 命令参考 | P4-A | `wiki/` | 0.5d |
| P4-E | Phase 4 | completed | 全量验证 + STATUS.md 定稿 | P4-A~D | `docs/dev/in-progress/diagnostics-industrialization/` | 1d |
| P4-F | Phase 4 | completed | 任务目录归档 | P4-E | `docs/dev/` | 0.25d |

## 依赖

- P1-B/C/D/E 依赖 P1-A（必须先完成 CMakeLists.txt 清理 + 构建系统修复后才能改具体代码）
- P1-F 无依赖（纯阅读审查）
- Phase 2 子任务全部依赖 Phase 1 完成
- Phase 3 依赖 Phase 1 + Phase 2（P3-C 只需 Phase 1，P3-A/B 需要 Phase 2）
- Phase 4 依赖全部前置

## 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| DAP 代码长期未编译，首次编译大量错误 | 高 | 高 | P1-A 优先，加入构建后立即修复编译错误 |
| debug_agent 废弃后其他模块有引用 | 中 | 中 | P1-A 做全局引用搜索 |
| EventPipe receiver 对 GC 改动敏感 | 中 | 中 | P3-D 做 receiver 测试 |
| 自旋锁→condvar 改造可能引入新 bug | 中 | 中 | P1-B 需要有 condvar 超时保护 |
| setBreakpoints source→IL 映射需要 metadata API | 中 | 中 | P2-A 需要先确认 reflection API 可用性 |
| 测试工程量大（全模块零测试基线） | 高 | 中 | 优先核心路径，负面/性能次之 |

## 备选路径

1. **P1-A 阻塞**（CMake 修改复杂）→ 先不做 CMake 重构，改为直接在现有 CMakeLists.txt 追加源文件
2. **P2-A 阻塞**（source→IL 映射不可用）→ 退回到 token-based setBreakpoints（基于 method token + IL offset，不依赖 source line）
3. **P3-A 阻塞**（调试器难以 mock）→ 退回到仅 server-level 集成测试
4. **P4 阻塞** → Wiki 可以先写 skeleton（骨架），内容后续补充

## 当前建议推进顺序

1. Phase 1 全部子任务（P1-A → P1-B/C/D/E 并行 → P1-F）
2. Phase 2 全部子任务（P2-A → P2-B → P2-C → P2-D）
3. Phase 3 全部子任务（P3-C → P3-A → P3-B → P3-D → P3-E → P3-F → P3-G → P3-H）
4. Phase 4 全部子任务（P4-A → P4-B/C/D → P4-E → P4-F）

## 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

串行模式，每次一个 child，完成后自动推下一个。Phase 1 内部 P1-B/C/D/E/F 在 P1-A 完成后可并行（共享 P1-A 的输出作为起点），由于单终端限制，按 P1-B → P1-C → P1-D → P1-E → P1-F 顺序执行。

## 调度策略

`dispatch_model`: sequential

由于当前单终端执行，所有子任务按串行模式推进。
