# WS2 Hybrid 工业化冲刺 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。当前任务带有 `source_relation: roadmap-child`，执行完成时必须先归档当前子任务目录，再同步父 roadmap。

**目标：** 将 Hybrid 从 1.5/5 提升至 5.0/5 — 从单一机制验证升级为经过完整生产验证的模式。

**架构：** 基于现有 HybridPrecode（计数器+AOT→JIT CAS 状态机）+ PrecodeArena（RWX 分页 trampoline 分配器），通过扩展测试、端到端集成、竞争测试、Foundation-DLL 集成、性能基准和文档完成工业化验证。

**技术栈：** C++20, Google Test, CMake, Windows x64, AotCoreIr JSON, PrecodeArena, HybridStubDispatchImpl

**架构审核模式：** normal（无新架构引入，全为已有模式的测试扩编和集成）

**结构告警重点：** 测试代码中的重复 helper 模式、codegen subject 的 Scriban 分层、benchmark 代码复用

**权责图审核主题：** n/a

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: hybrid-mode
- capabilityItem: industrialized-verification
- ownerSubjectId: n/a
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Smoke（HotUpdate 兼容性验证）
- formalVerificationObjects: []（本批为测试扩编而非 formal proof）
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（直接基于父 roadmap 边界拍板与现有 HybridPrecode 实现）

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

**预期知识沉淀：** wiki/03-功能模块/07-JIT引擎/ -> hybrid-mode.md（h6-doc）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 文件清单

### 修改文件
| 文件 | 职责 |
|------|------|
| `testing/src/native/jit/jit_hybrid_mode_test.cpp` | h1-core: 扩展 HybridPrecode 测试（补齐边界/失败恢复/PGO 交互） |
| `testing/src/native/jit/jit_hybrid_e2e_test.cpp` | h2-e2e: 端到端三阶段测试（AOT→升级→JIT） |
| `testing/src/native/jit/jit_hybrid_race_test.cpp` | h3-race: ABA/数据竞争/内存序测试 |
| `testing/src/native/jit/CMakeLists.txt` | 添加新 test targets（h2-e2e, h3-race, h5-bench） |
| `testing/foundation-dll/CMakeLists.txt` | h4-fd: 添加 stage 14 hybrid 验证子目录 |
| `testing/foundation-dll/.../stage14/`（新目录） | h4-fd: Foundation-DLL stage 14 hybrid 验证 |
| `testing/src/native/jit/bench_hybrid.cpp` | h5-bench: Hybrid 性能基准 |

### wiki 文件
| 文件 | 职责 |
|------|------|
| `wiki/03-功能模块/07-JIT引擎/hybrid-mode.md` | h6-doc: Hybrid 模式完整文档 |

---

## 子任务

### h1-core: HybridPrecode 核心验证（扩展测试）

**目标：** 将现有 12 个测试扩展至覆盖所有边界场景。

**新增/扩展测试点：**
1. `CounterExactThresholdBehavior` — counter = kT4Threshold + 1 时仍返回 AOT，= kT4Threshold 时触发 JIT
2. `CounterInitialValue` — 验证 call_counter 初始化为 kT4Threshold
3. `AotEntryIsNull` — aot_entry=null 时的 dispatch 行为
4. `HybridPgoInteraction` — PGO 启用时 hybrid dispatch 是否正确计数 call_count
5. `RepeatedCompileFailRecovery` — 连续编译失败时 state 和 counter 的正确重置
6. `DirectPtrUpdateOnCompile` — JIT 编译后 direct_ptr 被正确更新为 compiled->code
7. `DispatchWithNullEntry` — entry=null 时 HybridStubDispatchImpl 是否仍能正确工作

### h2-e2e: 端到端集成测试

**目标：** 创建完整的三阶段验证：AOT 执行 → 注册 Hybrid 方法 → JIT 升级。

**实现方案：** 新文件 `testing/src/native/jit/jit_hybrid_e2e_test.cpp`。
1. 模拟 AOT 函数（如 `AotReturn42`）
2. 创建 `RegisterHybridMethods` 使用的完整模式：HybridPrecode + PrecodeArena + HotpatchEntryV0
3. 阶段验证：
   - Phase 1 (AOT): 调用 < kT4Threshold 次，验证走 AOT 路径
   - Phase 2 (Upgrade): 最后一次调用触发 JIT
   - Phase 3 (JIT): 后续调用走编译路径
4. 验证 JIT 编译结果正确性（返回值、GC slot map）

### h3-race: 多线程/竞争测试

**目标：** 验证 ABA 问题、数据竞争、内存序正确性。

**实现方案：** 新文件 `testing/src/native/jit/jit_hybrid_race_test.cpp`。
1. `AbAOnCallCounter` — ABA 模式：counter 递减后重置，验证状态一致性
2. `ConcurrentCompileAndRead` — 一线程编译，多线程读取 compiled->code
3. `MemoryOrderStress` — 不同内存序下的 counter/state 一致性（relaxed vs acquire/release vs seq_cst）
4. `TrampolineConcurrentAccess` — 多个 HybridPrecode 共享 PrecodeArena 的页分配竞争

### h4-fd: Foundation-DLL stage 14 集成

**目标：** 创建 Foundation-DLL stage 14（Hybrid 验证 stage）。

**实现方案：**
1. 创建 `testing/foundation-dll/hybrid/` 目录
2. 创建 `hybrid_runner.py` — 与现有 foundation-dll runner 模式一致
3. 集成到 foundation-dll 验证管线
4. 验证 >= 90% families 通过

### h5-bench: 性能基准

**目标：** 建立 AOT→JIT 升级延迟 P50/P99 基线。

**实现方案：** 新文件 `testing/src/native/jit/bench_hybrid.cpp`。
1. `UpgradeLatency` — 测量从首次调用到 JIT 编译完成的完整升级延迟（P50/P99）
2. `DispatchOverhead` — 测量 AOT 路径 dispatch 开销（counter decrement）
3. `TrampolineAllocLatency` — PrecodeArena 分配 trampoline 延迟
4. `MultiMethodUpgrade` — 批量方法升级延迟

### h6-doc: Wiki 文档

**目标：** 补充 Hybrid 模式 wiki 文档。

**实现方案：** 创建 `wiki/03-功能模块/07-JIT引擎/hybrid-mode.md`。
1. 概述与架构
2. HybridPrecode 状态机说明
3. 计数器阈值与行为
4. 升级流程（AOT→JIT）
5. 失败恢复机制
6. PGO 交互
7. 配置选项

---

## 验证

```bash
# 构建所有 JIT 测试
cmake --build build/native --target test_jit_hybrid_mode --config Debug
cmake --build build/native --target test_jit_hybrid_e2e --config Debug
cmake --build build/native --target test_jit_hybrid_race --config Debug
cmake --build build/native --target bench_hybrid --config Debug

# 运行测试
build/native/testing/src/native/jit/Debug/test_jit_hybrid_mode.exe
build/native/testing/src/native/jit/Debug/test_jit_hybrid_e2e.exe
build/native/testing/src/native/jit/Debug/test_jit_hybrid_race.exe
build/native/testing/src/native/jit/Debug/bench_hybrid.exe

# 全回归
cmake --build build/native --config Debug
cd build/native && ctest --output-on-failure -C Debug
```

---

## 执行顺序（串行）

h1-core → h2-e2e → h3-race → h4-fd → h5-bench → h6-doc

每个任务完成后更新 STATUS.md 并推进下一任务。
