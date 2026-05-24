# AOT/JIT/Hybrid 三模式编译架构 — Roadmap v1

## 1. 目标

在现有 T4 JIT 基础上，实现 CoreCLR 风格的三模式编译体系：
- **AOT**（不变）：纯 AOT C++ 编译
- **JIT**（新）：首调用直接 T4 JIT 编译到机器码，不走解释器
- **Hybrid**（新）：冷时 AOT 原生执行，热时升级到 JIT 优化代码

## 2. 范围边界

- **范围内**：codegen 端三模式输出、runtime 端 Precode Stub dispatch、Binary IR 格式、命名迁移（codegen→jit）、JIT/Hybrid 模式测试
- **范围外**：不改变现有 AOT 行为、不改变 T1-T3 解释器、不重构托管端 codegen 架构、不涉及 HotUpdate 系统架构变更（只保留兼容性）

## 3. 非目标

- 不修改现有 `--mode aot` 的 codegen 输出或运行时行为
- 不重构 T1-T3 解释器层
- 不改变 Hotpatch 表 / codegen_bridge.h 的现有 ABI 合约（只做兼容扩展）
- 不处理旧 `--mode jit` 的废弃（Phase 4 才涉及）

## 4. 为什么需要 roadmap

1. **多阶段强依赖链**：命名迁移→Binary IR→JIT 模式→Hybrid 模式→T4 增强，各阶段有严格前后依赖
2. **跨子系统改动**：涉及 C# codegen 端、C++ runtime 端、测试框架三个子系统
3. **跨会话推进**：每个阶段都是独立的可交付单元，适合分多次会话完成
4. **渐进式验证**：每阶段完成后可独立验证，降低风险

## 5. 阶段列表

### Phase 0: 命名迁移与基础设施

- `goal`：完成 `codegen` → `jit` 的命名迁移，建立 Binary IR 序列化/反序列化基础设施
- `exit_criteria`：
  - 所有原 `codegen/` 目录文件已迁移到 `jit/`，编译通过
  - `chaos::il2cpp::jit` 命名空间可用，`jit::Compile()` 正常工作
  - Binary IR 序列化器（C#）和反序列化器（C++）完成 roundtrip 测试
- `deliverables`：
  - `src/native/jit/` 目录及其所有文件
  - `src/managed/.../JitOutput/RegisterMethodBinarySerializer.cs`
  - `src/native/jit/jit_binary_reader.h/cpp`
  - `testing/jit/test_binary_ir.cpp` + 通过
- `dependencies`：无（独立阶段）
- `resolved_decisions`：
  - 命名空间 `chaos::il2cpp::jit`
  - 目录 `src/native/jit/`
  - Binary IR 格式 v1（如 design-v1-01.md 定义）
- `watch_items`：
  - RegisterMethod IR 类型定义是否稳定（变更时需同步 Binary IR 格式）

### Phase 1: JIT 模式 — Precode Stub 与 dispatch

- `goal`：实现 CoreCLR 风格的 JIT 模式——首调用通过 JitStub 直接触发 `jit::Compile()`，不走解释器
- `exit_criteria`：
  - JitStub 实现完成：CAS 线程安全、spin-wait、首次编译后 direct_ptr 原子替换
  - codegen 端 JIT mode 输出：RegisterMethod Binary IR 而不是 AOT C++ 函数体
  - JIT 模式端到端测试通过：方法首调用触发 Compile()，后续调用直通
  - 热更兼容性：`kHotpatchActive` 仍能拦截 JIT 执行
- `deliverables`：
  - `src/native/jit/jit_precode.h/cpp`（JitStub）
  - `src/native/runtime-core/jit_registration.h/cpp` 修改（RegisterJitMethodsV2）
  - `src/managed/.../Generator/` 修改（JIT mode 路径）
  - `testing/jit/test_jit_mode.cpp` + 通过
- `dependencies`：Phase 0（需要 Binary IR + jit 命名空间就绪）
- `resolved_decisions`：
  - JitStub 使用 CAS 状态机（kUncompiled/kCompiling/kCompiled）
  - 使用 spin-wait 而非阻塞等待（T4 编译 < 100µs）
- `watch_items`：
  - JIT 首调用延迟：需要 benchmark 验证 < 100µs 目标
  - 多线程竞争：CAS + spin-wait 的正确性

### Phase 2: Hybrid 模式 — 冷 AOT + 热 JIT 升级

- `goal`：实现 Hybrid 模式——方法冷时走 AOT 原生代码，计数归零后触发 JIT 升级
- `exit_criteria`：
  - HybridStub 实现完成：原子计数器、AOT entry 调用、JIT 升级触发
  - codegen 端 Hybrid mode 输出：AOT C++ 函数体 + Binary IR 共存
  - Hybrid 模式端到端测试通过：冷走 AOT、计数归零后触发 JIT、热走 JIT
  - 去优化回退：T4 deopt 发生后回退到 AOT（不回退到解释器）
  - Hotupdate 兼容性：DemoteT4ByToken 后回退到 AOT
- `deliverables`：
  - `src/native/jit/jit_precode.h/cpp`（HybridStub 部分）
  - `src/native/runtime-core/jit_registration.h/cpp` 修改（RegisterHybridMethods）
  - `src/managed/.../Generator/` 修改（Hybrid mode 路径）
  - `testing/jit/test_hybrid_mode.cpp` + 通过
- `dependencies`：Phase 1（需要 JitStub + JIT mode 支撑）
- `resolved_decisions`：
  - HybridStub 计数器初始值 = kT4Threshold（同现有 T3→T4 阈值）
  - 去优化回退直接走 AOT entry，不经过解释器
- `watch_items`：
  - 计数器原子开销：每调用 1 次 relaxed atomic sub（~1 cycle）
  - T4 编译完成前的并发调用：AOT 执行期间 T4 在后台编译

### Phase 3: JIT 代码质量与 PGO

- `goal`：Tier 0 (Quick JIT) + Tier 1 (Optimized JIT) 分层，引入 PGO
- `exit_criteria`：
  - Tier 0 快速 JIT：减少优化 pass，编译时间 < 50µs
  - Tier 1 优化 JIT：利用运行时 profile 做内联决策、分支预测
  - PGO 数据收集：调用频次、分支跳转率、类型分布
  - 后台异步重编译：不阻塞调用线程
  - benchmark 验证：Hybrid 热路径性能 ≥ AOT 稳态性能
- `deliverables`：
  - `jit::Compile()` 增加 `CompileTier` 参数
  - PGO profile 数据收集器
  - 后台编译调度器
  - `testing/jit/bench_jit_vs_aot.cpp` benchmark
- `dependencies`：Phase 2（需要 Hybrid mode 稳定运行）
- `resolved_decisions`：（Phase 3 启动前在 brainstorm 中确认）
- `watch_items`：
  - PGO 数据内存开销
  - 后台编译与去优化的交互

### Phase 4: 迁移与清理

- `goal`：废弃旧 `--mode jit`，更新文档和测试管线
- `exit_criteria`：
  - 旧 `--mode jit` 标记为 deprecated，输出警告
  - wiki 文档更新（翻译路径参考、T4 架构文档）
  - CI 管线更新：JIT/Hybrid 模式加入 regression 测试
  - benchmark 基线建立：冷启动、稳态、内存占用
- `deliverables`：
  - wiki 文档更新
  - CI 配置更新
  - benchmark 报告
- `dependencies`：Phase 3
- `resolved_decisions`：（Phase 4 启动前确认）
- `watch_items`：
  - 旧用户迁移成本

## 6. 子任务映射

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|------------|-----------------|
| P0-rename | Phase 0 | planned | codegen/ → jit/ 目录 + 命名空间迁移 | — | 1 session |
| P0-binary-ir | Phase 0 | planned | Binary IR 序列化/反序列化 | — | 1 session |
| P0-test | Phase 0 | completed | Binary IR roundtrip 测试 | P0-binary-ir | 0.5 session |
| P1-jit-stub | Phase 1 | completed | JitStub + dispatch 路径实现 | P0-rename, P0-binary-ir | 1 session |
| P1-codegen-jit | Phase 1 | completed | codegen JIT mode 输出 Binary IR | P0-binary-ir | 1 session |
| P1-test | Phase 1 | planned | JIT mode 端到端测试 | P1-jit-stub, P1-codegen-jit | 1 session |
| P2-hybrid-stub | Phase 2 | planned | HybridStub + counter + 升级触发 | P1-jit-stub | 1 session |
| P2-codegen-hybrid | Phase 2 | planned | codegen Hybrid mode AOT+IR 双输出 | P1-codegen-jit | 1 session |
| P2-deopt-aot | Phase 2 | planned | T4 deopt 回退到 AOT 路径 | P2-hybrid-stub | 0.5 session |
| P2-test | Phase 2 | planned | Hybrid mode 端到端测试 | P2-hybrid-stub, P2-codegen-hybrid, P2-deopt-aot | 1 session |
| P3-tier0 | Phase 3 | planned | Tier 0 快速 JIT | Phase 2 | 1 session |
| P3-pgo | Phase 3 | planned | PGO 数据收集 + Tier 1 重编译 | P3-tier0 | 2 sessions |
| P3-bench | Phase 3 | planned | Benchmark + 性能调优 | P3-pgo | 1 session |
| P4-deprecate | Phase 4 | planned | 旧 --mode jit 废弃 | Phase 3 | 0.5 session |
| P4-docs | Phase 4 | planned | Wiki + CI 更新 | P4-deprecate | 0.5 session |

## 7. 依赖关系

```
Phase 0 ──→ Phase 1 ──→ Phase 2 ──→ Phase 3 ──→ Phase 4
  │            │            │            │            │
  ├ P0-rename  ├ P1-jit-stub ├ P2-hybrid-stub  ├ P3-tier0  ├ P4-deprecate
  ├ P0-binary  ├ P1-codegen  ├ P2-codegen      ├ P3-pgo    └ P4-docs
  └ P0-test    └ P1-test     ├ P2-deopt        └ P3-bench
                              └ P2-test
```

## 8. 风险

| 风险 | 阶段 | 级别 | 缓解 |
|------|------|------|------|
| Binary IR 格式需随 RegisterMethod IR 演进 | P0 | 中 | v1 格式 + version 检查 + 升级脚本 |
| JIT 首调用延迟超标 | P1 | 中 | Tier 0 快速模式 + 后台 Tier 1 |
| 多线程 CAS 竞争 | P1 | 低 | 确定性测试 + 压力测试 |
| Hybrid 计数器原子开销 | P2 | 低 | 放宽 relaxed ordering + benchmark |
| T4 去优化后回退 AOT 的正确性 | P2 | 中 | 完整 deopt→AOT 测试 |
| PGO 数据一致性 | P3 | 中 | 边界内不做跨方法 PGO |

## 9. 备选路径

- **如果 Binary IR 格式稳定性不足**：回退到使用 JSON AotCoreIr（性能降级但功能正常），Binary IR 作为 Phase 3 优化
- **如果 JitStub spin-wait 导致性能问题**：改为 futex 或 signal-based 等待
- **如果 Hybrid 模式不需要去优化回退**：deopt→AOT 路径推迟到 Phase 3

## 10. 当前建议推进顺序

**串行推进，从 Phase 0 开始**，每阶段完成后由用户决定是否继续下一阶段。

推荐启动顺序：
1. Phase 0: 命名迁移 + Binary IR（最小可行基础设施）
2. Phase 1: JIT 模式（CoreCLR 风格的核心能力）
3. Phase 2: Hybrid 模式（混合编译）
4. Phase 3: T4 质量 + PGO（性能增强）
5. Phase 4: 清理收尾

## 11. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `dispatch_model: sequential`
- `recommended_next_child: P0-rename`

## 12. 调度策略

`dispatch_model: sequential` — 子任务有强依赖链，串行推进。
