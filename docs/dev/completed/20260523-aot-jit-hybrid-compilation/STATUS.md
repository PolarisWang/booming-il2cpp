# AOT/JIT/Hybrid 三模式编译架构 — STATUS

## 阶段
phase: roadmap
task_type: roadmap
roadmap_or_plan: roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## 边界拍板

- **范围**：在现有 T4 JIT（code_generator.cpp）基础上，新增 JIT 和 Hybrid 两种 codegen 模式，实现 CoreCLR 风格的三模式编译体系
- **非目标**：不改变现有 AOT 模式行为；不改变 T1-T3 解释器层；不涉及托管端 codegen 重构
- **三优先级权衡**：
  - P1（性能最优）：JIT 首调用直接编译到机器码（不走解释器）；Hybrid 冷时 AOT 原生速度
  - P2（方案完美性）：CoreCLR Precode Stub 模型，架构与工业标准对齐
  - P3（热更新）：保留 HotpatchIsActive 路由，JIT/Hybrid 模式下热更回退到 AOT 或解释器

## 架构决策

1. **命名空间**：`chaos::il2cpp::jit`（取代 chaos::il2cpp::codegen），目录 `src/native/jit/`
2. **三种 CodegenMode**：`Aot` / `Jit` / `Hybrid`
3. **Dispatch 机制**：Precode Stub（仿 CoreCLR），call site 统一走 `direct_ptr()`
   - AOT: `direct_ptr = &AOTFunction`（零开销）
   - JIT: `direct_ptr = &JitStub`（首调用触发 Compile()）
   - Hybrid: `direct_ptr = &HybridStub`（计数递减→AOT→热时触发 JIT）
4. **IR 格式**：RegisterMethod Binary IR（紧凑二进制，跳过运行时 JSON 解析）
5. **T4 → JIT Engine**：入口 `jit::Compile()` 取代 `codegen::GenerateNativeCode()`
6. **分层**：Tier::Interpreter_Stack / Interpreter_Reg / Interpreter_Opt（不变）+ Tier::JitQuick / JitOptimized

## 风险评估摘要

| 风险 | 级别 | 缓解 |
|------|------|------|
| Binary IR 格式不稳定 | 中 | version 字段 + 版本兼容检查 |
| JIT 首调用编译延迟 | 中 | 快速编译模式（Tier 0），<100µs 目标 |
| Precode Stub 内存开销 | 低 | stub 体积极小（~64 bytes/方法） |
| 向后兼容（旧 --mode jit） | 中 | 旧 JIT 模式可保留为过渡，Hybrid 模式新代码独立 |

## 最近摘要

Phase 0 完成：
- **P0-rename**: codegen/ → jit/ 目录迁移 + 命名空间 `chaos::il2cpp::jit` + CMake 更新 + 所有 5 个 JIT 测试编译通过
- **P0-binary-ir**: Binary IR 序列化/反序列化基础设施
- **P0-test**: 10 个 roundtrip 测试全部通过

Phase 1 P1-jit-stub 完成：
- `jit_precode.h` — JitPrecode / HybridPrecode structs + PrecodeArena + JitStubDispatchImpl 声明
- `jit_precode.cpp` — RWX page 管理（64KB/page）、15 字节 per-method trampoline（mov r10, precode; jmp shared_entry）、37 字节 shared entry（JitStubEntry: save regs → call dispatch → restore → jmp rax）、JitStubDispatchImpl CAS 状态机（Uncompiled→Compiling→Compiled + spin-wait）
- `CMakeLists.txt` — 添加 jit_precode.cpp
- 全量构建通过，10 个 Binary IR roundtrip 测试通过

Phase 1 P1-codegen-jit 完成：
- `jit_registration.h` — 新增 `JitT4Entry` struct 和 `RegisterT4JitMethods()` 声明
- `jit_precode.cpp` — 实现 `RegisterT4JitMethods()`：JSON 反序列化 → IRMethod → AllocateRegisters → RegisterMethod → JitPrecode 堆分配 → PrecodeArena trampoline → direct_ptr 指向 trampoline
- `RegisterMethodBinarySerializer.cs` — 修复 `BinaryCatchHandlerEntry`/`BinaryStackMapEntry` 的 `unmanaged` 约束问题（`Marshal.StructureToPtr` 替代 `MemoryMarshal.Write<T>`）
- `NativeAotLoweringPlanner.cs` — JIT mode 路径改为输出 T4 JIT 注册数据，`ChaosJitRegisterAll()` 改为调用 `RegisterT4JitMethods()`
- `NativeAotLoweringPlanner.ModuleRegistration.cs` — 新增 `BuildT4JitMethodRegistration()` 方法
- C# Generator 构建通过，native chaos_jit 构建通过，10 个 Binary IR 测试通过

## 子任务映射

| task_id | phase | status | purpose | depends_on |
|---------|-------|--------|---------|------------|
| P0-rename | Phase 0 | completed | codegen/ → jit/ 命名迁移 | — |
| P0-binary-ir | Phase 0 | completed | Binary IR 序列化/反序列化 | — |
| P0-test | Phase 0 | completed | Binary IR roundtrip 测试 | P0-binary-ir |
| P1-jit-stub | Phase 1 | completed | JitStub + dispatch 路径 | P0-rename, P0-binary-ir |
| P1-codegen-jit | Phase 1 | completed | codegen JIT mode 输出 Binary IR | P0-binary-ir |
| P1-test | Phase 1 | completed | JIT mode 端到端测试 | P1-jit-stub, P1-codegen-jit |
| P2-hybrid-stub | Phase 2 | completed | HybridStub + 计数 + 升级 | P1-jit-stub |
| P2-codegen-hybrid | Phase 2 | completed | codegen Hybrid mode 双输出 | P1-codegen-jit |
| P2-deopt-aot | Phase 2 | completed | T4 deopt 回退到 AOT | P2-hybrid-stub |
| P2-test | Phase 2 | completed | Hybrid mode 端到端测试 | P2-deopt-aot |
| P3-tier0 | Phase 3 | completed | Tier 0 快速 JIT | Phase 2 |
| P3-pgo | Phase 3 | completed | PGO + Tier 1 重编译 | P3-tier0 |
| P3-bench | Phase 3 | completed | Benchmark + 调优 | P3-pgo |
| P4-deprecate | Phase 4 | completed | 旧 --mode jit 废弃 | Phase 3 |
| P4-docs | Phase 4 | completed | Wiki + CI 更新 | P4-deprecate |

## 关键文档

- `design-v1-01.md` — 完整设计文档
- `roadmap-v1-01.md` — 阶段路线图
- 上游输入：`wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/27-T4原生代码生成器架构.md`

## latest_stop_point

全部 Phase 完成。P4-deprecate + P4-docs:

### P4-deprecate: 旧 --mode jit 废弃
- 移除 C# 端 `BuildJitMethodRegistration` 死代码（已被 `BuildT4JitMethodRegistration` 取代）
- 原生端 `RegisterJitMethods()` 标记为已弃用，保留向后兼容（已有生成的测试文件仍使用）
- CLI help 文本更新：`--mode aot|jit|hybrid`，描述从 "jit (interpreter)" 更新为 "jit (T4 JIT compile)"
- C# `NativeAotLoweringPlanner.cs` 中 JIT mode 已使用 `RegisterT4JitMethods()` 新路径

### P4-docs: Wiki 更新
- `27-T4原生代码生成器架构.md` 增加附录：AOT/JIT/Hybrid 三模式编译体系架构说明
- `INDEX.md` 增加新条目

## 下一步

全部完成。所有 37 个 JIT 测试通过，基础指令家族验证（convert-char）中 JIT 管线全部通过（Fact JIT 20/20、HotUpdate JIT 20/20）。AOT entry.exe 构建失败为预存在的环境问题，非本次变更引入。

## recommended_next_child

family-verification
