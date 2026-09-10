# 三模式全面工业化收官 — Roadmap v1

> task_id: full-industrialization-roadmap
> dispatch_model: sequential
> created: 2026-05-24

**目标：** 将 AOT/JIT/Hybrid 三模式的工业化就绪度从当前水平（AOT 4.0, JIT 2.5, Hybrid 1.5, 基础设施 2.0）全面提升至满分 5/5。覆盖全部已识别差距项和风险点。

**上游设计：** brainstorm 阶段已确认全部设计拍板（2026-05-24）。
**全局优先级：** P1（性能最优）> P2（方案完美性）> P3（HotUpdate 支持）

---

## 范围边界

- **范围内**：JIT 工业化收官（GC 精确槽/Liveness/OSR/Unwind 等）、Hybrid 生产级工业化、Foundation-DLL 全量验证攻坚（16 failures 硬修复）、AOT PHASE 4+5（HotUpdate 完整+性能优化）、CI/CD 完整体系、工程卓越度
- **范围外**：ARM64 JIT 支持（下一周期）、多平台支持（Linux/macOS/iOS/Android，结构预留不执行）、Interpreter T1-T3 执行引擎改造、runtime-core GC 算法改动
- **架构决策**（brainstorm 已确认）：
  - Hybrid = 正式生产模式，需独立工业化冲刺
  - Foundation-DLL Hybrid 验证 = 新增 stage 14
  - CoreLib 16 failures = 硬修复，blocking=0
  - AOT PHASE 5 = 完整计划推进
  - wf1-ci = 移入 WS5（CI/CD 统一建设）

---

## 非目标

- 不做 ARM64 JIT（推迟到下一周期）
- 不做多平台支持（结构预留）
- 不改变 Interpreter T1-T3 的执行路径
- 不改变 codegen 侧 C# 编译器输出格式
- 不做 HybridPrecode 核心机制重写（已有机制保持不变）

---

## 阶段列表

| Phase | 名称 | 描述 | 模式 |
|-------|------|------|------|
| phase-1 | 核心工业化 | 5 个工作流并行执行：JIT/Foundation-DLL/AOT/CICD/工程 | 内部串行工作流（单终端） |
| phase-2 | Hybrid 工业化 | Hybrid 生产级冲刺，依赖 WS1 完成 | 串行 |

---

## 工作流详细定义

### WS1: JIT 工业化收官

**目标：** 将 JIT 从 2.5/5 提升至 5.0/5。覆盖 JIT 审计全部已识别问题（除去 ARM64 和 CI 基线）。

**退出条件：**
- GC 精确槽映射就绪（GcSlotMapV0 含 GPR 条目；nm->gc_slot_map 非空）
- Liveness 默认开启，GC 精确扫描通过
- OSR 无 segfault（Test_OsrEntry 通过）
- Unwind 编译守卫（static_assert）就位
- 全回归通过（176/181 baseline 提升至 >= 99%）
- 无已知 P0/P1 缺陷

**与 WS2 关系：** WS1 完成后 WS2 才能启动（共享 jit_engine.cpp）

#### 子任务详情

##### wf1-arch: IEncoder/ISehHandler 接口抽象 + jit_engine.cpp 重构

| 字段 | 内容 |
|------|------|
| purpose | 将 x64 硬编码的编码器和 SEH 逻辑抽象为接口，为后续平台扩展做架构准备 |
| requirements | native proof |
| deliverables | `IEncoder.h`, `ISehHandler.h`, `X64Encoder` class, `WinSehHandler` class, `jit_engine.cpp` 重构 |
| exit_criteria | 架构可编译，全回归通过 |
| conflict_scope | `src/native/jit/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 提取 IEncoder 接口（EmitBytes/EmitAlignment/ReserveLabel/BindLabel/NewSlot 等纯虚方法） |
| | (2) 创建 X64Encoder 类，将 jit_engine.cpp 中所有 x64 硬编码 emit 逻辑移入 |
| | (3) 提取 ISehHandler 接口（RegisterT4Code/FindT4CodeByAddress 等纯虚方法） |
| | (4) 创建 WinSehHandler 类（基于 VEH） |
| | (5) 修改 Compile() 函数签名：接受 IEncoder* 和 ISehHandler* 参数 |
| | (6) 更新 CMakeLists.txt 添加新文件 |
| | (7) 构建验证 + 全回归通过 |

##### wf1-gc: GC 精确槽映射 — 全量发射 + GPR 条目

| 字段 | 内容 |
|------|------|
| purpose | 从保守栈扫描升级到精确 GC 槽映射，保障 GC 正确性和性能 |
| requirements | native proof + managed proof |
| deliverables | `jit_engine.cpp` (GC slot emission), `jit_method.h` (GcSlotMapV0 扩展) |
| exit_criteria | GcSlotMapV0 含 GPR 条目；nm->gc_slot_map 非空 |
| conflict_scope | `src/native/jit/` |
| estimated_effort | 大 |
| 具体步骤 | (1) 分析当前 GC slot map 结构（只含 XMM 寄存器槽） |
| | (2) 扩展 GcSlotMapV0：添加 GPR 条目（调用者保存寄存器 + callee 保存寄存器） |
| | (3) 在 jit_engine.cpp 5 阶段管线中增加 GC slot 发射阶段 |
| | (4) 发射所有 call 指令处的 GC slot map（每个 call 前扫描活跃 GPR） |
| | (5) gc_slot_map 写入 nm 对象 |
| | (6) 验证：在 GC stress 模式下运行测试，确认精确扫描生效 |
| | (7) 对比保守扫描 vs 精确扫描的内存效率 |

##### wf1-liveness: 开启 Liveness + 验证

| 字段 | 内容 |
|------|------|
| purpose | 默认开启 Liveness 分析，实现寄存器分配裁剪，减少栈溢出 |
| requirements | native proof |
| deliverables | `jit_engine.cpp` |
| exit_criteria | liveness 默认开启，GC 精确扫描通过 |
| conflict_scope | `src/native/jit/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 分步策略第一步：全量发射所有变量（不裁剪），验证精确扫描正确性 |
| | (2) 收集全量发射下的 GC 扫描正确性报告 |
| | (3) 分步策略第二步：开启 liveness 裁剪 |
| | (4) 验证 liveness 裁剪后 GC 扫描仍然正确 |
| | (5) 运行全回归确认无退化 |
| | (6) 对比开启前后性能差异（RDTSC 基准） |

##### wf1-osr: OSR segfault 修复（3 个向量）

| 字段 | 内容 |
|------|------|
| purpose | 修复已知 OSR（On-Stack Replacement）段错误，保障 T4 JIT 生成代码的稳定性 |
| requirements | native proof |
| deliverables | `jit_helpers.cpp`, `jit_engine.cpp` |
| exit_criteria | Test_OsrEntry 不 segfault, 防御性检查就位 |
| conflict_scope | `src/native/jit/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 向量 1：OsrResolveLoopHeader 中的空指针检查 |
| | (2) 向量 2：DeoptTlsState 中的栈帧对齐修复 |
| | (3) 向量 3：OSR entry 的基本块跳转偏移修正 |
| | (4) 增加防御性断言（nullptr 检查、边界检查） |
| | (5) 在 OSR 相关路径添加 PROFILE_SCOPE 预埋点 |
| | (6) Test_OsrEntry 通过验证 |

##### wf1-unwind: Unwind 编译守卫

| 字段 | 内容 |
|------|------|
| purpose | 为 Unwind Info 增加编译期 static_assert 守卫，防止 Unwind 数据结构不一致 |
| requirements | native proof |
| deliverables | `jit_unwind.cpp` |
| exit_criteria | static_assert 就位 |
| conflict_scope | `src/native/jit/` |
| estimated_effort | 极小 |
| 具体步骤 | (1) 分析 jit_unwind.cpp 中 UNWIND_INFO 结构体定义的约束条件 |
| | (2) 增加 static_assert 守卫：结构体大小、字段对齐、版本号一致性 |
| | (3) 增加 static_assert 守卫：函数表偏移量在合法范围 |
| | (4) 构建验证通过 |

##### wf1-tests: 模块级测试套件补齐

| 字段 | 内容 |
|------|------|
| purpose | 将 JIT 测试从 176/181（97.2%）提升至 >= 99%，补全缺失场景 |
| requirements | managed proof + native proof |
| deliverables | `testing/src/native/jit/` 扩展 |
| exit_criteria | 全回归通过，新测试通过 |
| conflict_scope | `testing/src/native/jit/` |
| estimated_effort | 大 |
| 具体步骤 | (1) 分析当前 5 个已知失败用例的 root cause |
| | (2) 修复或标记为 scope limitation（需用户确认） |
| | (3) 补齐 GC 精确槽映射场景测试 |
| | (4) 补齐 Liveness 开启后的场景测试 |
| | (5) 补齐 OSR 边界条件测试（循环嵌套、深栈、泛型方法） |
| | (6) 补齐 Unwind 边界测试（异常展开路径、跨帧展开） |
| | (7) 补齐 SEH 边界测试（嵌套 try/catch/finally、过滤表达式） |
| | (8) 补充 JIT 编译压力测试（大量方法快速 JIT 编译） |
| | (9) 补充多线程并发 JIT 编译测试 |
| | (10) 全回归基线确认 |

---

### WS2: Hybrid 工业化冲刺

**目标：** 将 Hybrid 从 1.5/5 提升至 5.0/5。从单一机制验证升级为经过完整生产验证的模式。

**启动条件：** WS1（JIT 工业化）完成（共享 jit_engine.cpp）。

**退出条件：**
- HybridPrecode 核心验证通过（计数器、CAS 原子性、AOT 回退、编译后切换）
- 完整三阶段端到端测试通过（AOT→升级→JIT）
- 多线程竞争测试通过（无 ABA、无数据竞争）
- Foundation-DLL stage 14 通过 >= 90% families
- AOT→JIT 升级延迟 P50/P99 基线建立
- HotUpdate 兼容验证通过
- Wiki 文档完成

#### 子任务详情

##### h1-core: HybridPrecode 核心验证

| 字段 | 内容 |
|------|------|
| purpose | 补全 HybridPrecode 状态机测试，确保核心机制在隔离环境中正确 |
| requirements | native proof |
| deliverables | `testing/src/native/jit/test_jit_hybrid_mode.cpp` 扩展 |
| exit_criteria | 所有核心场景覆盖（计数器递减路径、CAS 原子编译路径、AOT 回退路径、编译后切换路径） |
| conflict_scope | `testing/src/native/jit/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 测试 counter 递减至零触发编译 |
| | (2) 测试 CAS 原子性：多线程同时到达编译临界点 |
| | (3) 测试 AOT 回退：编译失败时保持 AOT 执行 |
| | (4) 测试编译后切换：验证 direct_ptr 被原子替换 |
| | (5) 测试 counter 初始值不同配置（kT4Threshold=30 以及自定义值） |
| | (6) 测试 counter 不触发场景（方法调用少于阈值） |

##### h2-e2e: 端到端集成测试

| 字段 | 内容 |
|------|------|
| purpose | 测试 managed 代码在 hybrid 模式下的完整生命周期：从 codegen → native 运行 → AOT→JIT 升级 |
| requirements | managed proof + native proof |
| deliverables | `testing/src/native/jit/test_hybrid_e2e.cpp`, codegen 测试 subject |
| exit_criteria | 三阶段全部通过，升级前后结果一致 |
| conflict_scope | `src/managed/`(codegen), `testing/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 编写 codegen 测试 subject（含多个被调用方法的 managed 代码） |
| | (2) codegen `--mode hybrid` 生成 C++ 输出 |
| | (3) 阶段 1（AOT）：验证前 N 次调用在 AOT 路径上执行，结果正确 |
| | (4) 阶段 2（升级）：超过阈值后触发 JIT 编译，观测 direct_ptr 切换 |
| | (5) 阶段 3（JIT）：验证后续调用在 JIT 编译代码上执行，结果与 AOT 一致 |
| | (6) 验证升级过程中无数据丢失（局部变量、参数、返回值正确） |
| | (7) 验证升级后 GC 正确性（混合使用 AOT 栈帧和 JIT 栈帧） |

##### h3-race: 多线程/竞争测试

| 字段 | 内容 |
|------|------|
| purpose | 验证 HybridPrecode 在多线程同时调用的竞争条件下无 ABA 问题、无数据竞争 |
| requirements | native proof |
| deliverables | `testing/src/native/jit/test_hybrid_race.cpp` |
| exit_criteria | 所有竞争场景无崩溃、无 ABA、无数据竞争 |
| conflict_scope | `testing/src/native/jit/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 多线程同时调用 hybrid 方法触发 CAS 编译 |
| | (2) ABA 问题验证：counter 值在不同线程间的可见性 |
| | (3) 编译中和编译完成瞬间有调用到达 |
| | (4) 编译失败+重试在竞争下的行为 |
| | (5) 升级后瞬间有旧线程仍在执行 AOT 代码 |
| | (6) ThreadSanitizer 运行确认无数据竞争 |

##### h4-fd: Foundation-DLL stage 14 集成

| 字段 | 内容 |
|------|------|
| purpose | 在 foundation-dll 13 阶段流水线中新增 stage 14（Hybrid 验证） |
| requirements | managed proof + native proof |
| deliverables | `testing/foundation-dll/_core/python/hybrid_runner.py`, `pipeline-config.yaml` |
| exit_criteria | stage 14 在所有 family 上通过 >= 90% |
| conflict_scope | `testing/foundation-dll/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 创建 hybrid_runner.py：codegen `--mode hybrid` → native compile → hybrid run → 结果校验 |
| | (2) 将 stage 14 注入 orchestrator.py 的 stage 列表 |
| | (3) 更新 pipeline-config.yaml 含 hybrid 配置节 |
| | (4) 在 ALL family 上运行 stage 14 基线 |
| | (5) 标记已知失败（如有） |
| | (6) 验证混合兼容性：同一个 family 在 AOT/JIT/Hybrid 三种模式下结果一致 |

##### h5-bench: 性能基准

| 字段 | 内容 |
|------|------|
| purpose | 建立 hybrid 模式下的性能基线，量化 AOT→JIT 升级开销 |
| requirements | native proof |
| deliverables | `testing/src/native/jit/test_hybrid_bench.cpp`, benchmark report |
| exit_criteria | 升级延迟 P50/P99 基线建立 |
| conflict_scope | `testing/src/native/jit/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 测量 AOT→JIT 升级延迟（从 counter 归零到 JIT 编译完成） |
| | (2) 测量升级后执行性能 vs 纯 AOT |
| | (3) 测量路径切换延迟（在 hybrid 调度中检测模式切换） |
| | (4) 收集 P50/P99/P999 数据 |
| | (5) 与纯 AOT 和纯 JIT 模式做 3-way 对比 |

##### h6-hotupdate: HotUpdate 兼容性

| 字段 | 内容 |
|------|------|
| purpose | 验证 hybrid 模式下 hotpatch 正常工作 |
| requirements | native proof + hotupdate proof |
| deliverables | `testing/src/native/hot-update/test_hybrid_hotupdate.cpp` |
| exit_criteria | hybrid 模式下 hotpatch 后所有方法正确执行 |
| conflict_scope | `testing/src/native/hot-update/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 测试 hybrid 方法被 hotpatch（已 JIT 升级的方法被 patching） |
| | (2) 测试 hotpatch 后方法回退到 InterpreterEntryDirect |
| | (3) 测试 hotpatch 撤销（Revert）后恢复 hybrid 调度 |
| | (4) 测试批量 hotpatch + hybrid 的交互 |

##### h7-wiki: Hybrid 文档

| 字段 | 内容 |
|------|------|
| purpose | 撰写完整的 Hybrid 模式技术文档 |
| requirements | — |
| deliverables | `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/28-Hybrid模式架构.md` |
| exit_criteria | 文档覆盖架构说明、使用指引、AOT/JIT/Hybrid 三模式选择决策树 |
| conflict_scope | `wiki/` |
| estimated_effort | 小 |
| 具体步骤 | (1) Hybrid 架构说明：HybridPrecode 状态机、三阶段生命周期 |
| | (2) 使用指引：`--mode hybrid` 编译选项、阈值调整 |
| | (3) 三模式选择决策树：什么场景选 AOT / JIT / Hybrid |
| | (4) 已知限制与 roadmap |
| | (5) 故障诊断指南 |

---

### WS3: Foundation-DLL 全量验证攻坚

**目标：** 将 foundation-dll 验证从 2.65/5 提升至 5.0/5。CoreLib 47 families 全通过，16 个 failures 全部硬修复。

**退出条件：**
- Phase 2 全量通过：140/140 families，blocking=0
- Phase 3 工业化评估 >= 4.5/5
- CI 集成就位（与 WS5 协作）
- 回归基线建立
- 文档完备

#### 子任务详情

##### f1-p0: P0 级 failures 修复

| 字段 | 内容 |
|------|------|
| purpose | 修复 6 个 P0 优先级的 CoreLib family 失败 |
| requirements | managed proof + native proof |
| deliverables | 每个 P0 family 的 fix 提交 + 验证报告 |
| exit_criteria | 6 个 P0 family 全部 13-stage passed |
| conflict_scope | `testing/foundation-dll/System.Private.CoreLib/`, `src/native/`(有限) |
| estimated_effort | 中 |
| 具体步骤 | (1) 按 FAMILY_ORDER.md 确认 P0 列表（collections-generic-core、convert-char、delegate-core-invocation、enum-parsing、error-info-basic、interface-dispatch 相关） |
| | (2) 对每个 family 执行 triage：定位失败 stage → root cause → 修复 |
| | (3) 提交修复 + 验证通过 |
| | (4) 更新 verification-tracker.json |

##### f2-p1: P1 级 failures 修复

| 字段 | 内容 |
|------|------|
| purpose | 修复 2 个 P1 优先级的 CoreLib family 失败 |
| requirements | managed proof + native proof |
| deliverables | 每个 P1 family 的 fix 提交 + 验证报告 |
| exit_criteria | 2 个 P1 family 全部 13-stage passed |
| conflict_scope | `testing/foundation-dll/System.Private.CoreLib/`, `src/native/`(有限) |
| estimated_effort | 中 |
| 具体步骤 | (1) 同 f1 流程 |
| | (2) P1 列表（pinvoke-dllimport、primitive-numeric-conversions-core 等） |

##### f3-p3: P3 级 failures 修复

| 字段 | 内容 |
|------|------|
| purpose | 修复 4 个 P3 优先级的 CoreLib family 失败 |
| requirements | managed proof + native proof |
| deliverables | 每个 P3 family 的 fix 提交 + 验证报告 |
| exit_criteria | 4 个 P3 family 全部 13-stage passed |
| conflict_scope | `testing/foundation-dll/System.Private.CoreLib/`, `src/native/`(有限) |
| estimated_effort | 小 |
| 具体步骤 | (1) 同 f1 流程 |
| | (2) P3 列表（snapshot-prover、string-construction、threading-monitor-interlocked/sync/tasks-primitives、rcw-basic、reflection-member-complete） |

##### f4-noncorelib: 非 CoreLib 覆盖确认

| 字段 | 内容 |
|------|------|
| purpose | 确认 CoreLib 外 93+ families 的 baseline 完整性 |
| requirements | — |
| deliverables | 非 CoreLib families 的 unified-verification-report 汇总 |
| exit_criteria | 所有非 CoreLib families 通过率 >= 95% |
| conflict_scope | `testing/foundation-dll/`(各非 CoreLib 程序集) |
| estimated_effort | 小 |
| 具体步骤 | (1) 遍历非 CoreLib 程序集的所有 families |
| | (2) 确认每个 family 有 unified-verification-report.json |
| | (3) 执行一次全量验证确保 baseline 正确 |
| | (4) 汇总到 batch dashboard |

##### f5-ci: Foundation-DLL CI 集成

| 字段 | 内容 |
|------|------|
| purpose | 将 foundation-dll pipeline 注入 CI workflow（与 WS5 协作） |
| requirements | — |
| deliverables | `.github/workflows/foundation-dll-regression.yml` |
| exit_criteria | PR 自动触发 foundation-dll 验证 |
| conflict_scope | `.github/workflows/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 创建 foundation-dll-regression.yml |
| | (2) trigger: pull_request + push to main |
| | (3) 配置 13-stage pipeline runner |
| | (4) 设置超时和资源限制 |
| | (5) 结果自动批注到 PR |

##### f6-baseline: 回归基线建立

| 字段 | 内容 |
|------|------|
| purpose | 在全量通过后建立首次回归 baseline |
| requirements | f1-f4 完成 |
| deliverables | `testing/results/verification-history/`(baseline 入库) |
| exit_criteria | baseline 数据入库，回归门槛明确 |
| conflict_scope | `testing/results/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 全量运行后 capture 所有 family 的 stage 结果 |
| | (2) 建立 per-family per-stage 的 pass/fail 基线 |
| | (3) 设定回归门槛（任何 stage 从 pass→fail 触发告警） |
| | (4) 写入 verification-history/ |

##### f7-phase3: Phase 3 工业化收口

| 字段 | 内容 |
|------|------|
| purpose | 将 foundation-dll 工业化评估从 2.65/5 提升至 5.0/5 |
| requirements | f1-f6 完成 |
| deliverables | wiki 文档、pipeline 自测、dashboard 增强 |
| exit_criteria | 工业化评估 >= 4.5/5 |
| conflict_scope | `wiki/`, `testing/foundation-dll/` |
| estimated_effort | 大 |
| 具体步骤 | (1) wiki 文档：管线架构页面 + 阶段说明 + 故障排除指南 |
| | (2) pipeline 自测：orchestrator runner 单元测试 |
| | (3) Dashboard 增强：历史趋势 + 回归告警可视化 |
| | (4) 架构审查：pipeline-config.yaml 与 orchestrator 同步 |
| | (5) 最终工业化评估报告 |

---

### WS4: AOT PHASE 4+5（HotUpdate 完整化 + 性能优化）

**目标：** 将 AOT 从 4.0/5 提升至 5.0/5。PHASE 4（HotUpdate 完整化）+ PHASE 5（性能优化）。

**退出条件：**
- PHASE 4：所有公开方法生成 hotpatch dispatch entry，PatchLoader 支持多模块，Interpreter 回退路径完整
- PHASE 5：方法内联、去虚拟化、LTO 集成、大程序集分页全部交付

#### 子任务详情

##### p4-hotpatch: Hotpatch Dispatch 全覆盖

| 字段 | 内容 |
|------|------|
| purpose | 确保每个公开方法都生成 hotpatch dispatch entry，消除当前可能的覆盖缺口 |
| requirements | managed proof + native proof |
| deliverables | `src/managed/Chaos.IL2CPP.Generator/`(hotpatch emit 扩展) |
| exit_criteria | 所有 AOT 编译的公开方法均含 HotpatchEntryV0 |
| conflict_scope | `src/managed/`, `src/native/runtime-core/` |
| estimated_effort | 大 |
| 具体步骤 | (1) 审计当前 hotpatch dispatch entry 的生成规则 |
| | (2) 识别未覆盖的方法类别（泛型实例化、InternalCall、P/Invoke、委托等） |
| | (3) 扩展 codegen 侧 hotpatch emit 逻辑覆盖所有方法 |
| | (4) 验证 coverage >= 99% |
| | (5) 验证 hotpatch dispatch 性能（额外分支开销） |

##### p4-multimodule: PatchLoader 多模块支持

| 字段 | 内容 |
|------|------|
| purpose | PatchLoader 支持跨多个 patch module 的批量加载/卸载 |
| requirements | native proof + hotupdate proof |
| deliverables | `src/native/runtime-core/patch_loader.cpp`(多模块扩展) |
| exit_criteria | 多模块并行加载/卸载通过 |
| conflict_scope | `src/native/runtime-core/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 设计多模块 PatchLoader 的模块间依赖解析 |
| | (2) 实现批量加载（按依赖顺序） |
| | (3) 实现批量卸载（逆序） |
| | (4) 实现增量加载（已加载模块跳过） |
| | (5) 测试：加载顺序无关、循环依赖检测、部分加载失败恢复 |

##### p4-interpreter: Interpreter 回退完整化

| 字段 | 内容 |
|------|------|
| purpose | 被 hotpatch 的方法从 AOT→Interpreter 的完整回退路径 |
| requirements | native proof + interpreter proof |
| deliverables | `src/native/runtime-core/interpreter_entry.h`(回退路径) |
| exit_criteria | 所有复杂场景（EH/泛型/PInvoke/委托）的 interpreter 回退通过 |
| conflict_scope | `src/native/runtime-core/`, `src/native/interpreter/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 审计当前 interpreter 回退路径的覆盖范围 |
| | (2) 补齐 EH 回退（嵌套 try/catch/finally、过滤表达式） |
| | (3) 补齐泛型实例化回退 |
| | (4) 补齐 P/Invoke 回退 |
| | (5) 补齐委托回退 |
| | (6) 端到端验证 |

##### p5-inline: 方法内联

| 字段 | 内容 |
|------|------|
| purpose | AOT codegen 侧方法内联，消除跨方法调用开销 |
| requirements | managed proof |
| deliverables | `src/managed/Chaos.IL2CPP.Generator/`(inliner 实现) |
| exit_criteria | 基准测试内联收益 >= 10% (视 baseline) |
| conflict_scope | `src/managed/Chaos.IL2CPP.Generator/` |
| estimated_effort | 极大 |
| 具体步骤 | (1) 内联候选识别（小方法、getter/setter、简单包装） |
| | (2) 内联安全性分析（异常边界、语义等价性） |
| | (3) 内联代码生成（将被调用方法体嵌入调用方） |
| | (4) 递归/循环内联深度控制 |
| | (5) 调试信息的影响处理 |
| | (6) 基准测试和收益量化 |

##### p5-devirt: 去虚拟化

| 字段 | 内容 |
|------|------|
| purpose | 基于可达性分析的虚拟调用去虚拟化 |
| requirements | managed proof |
| deliverables | `src/managed/Chaos.IL2CPP.Linker/`(devirt 分析), `src/managed/Chaos.IL2CPP.Generator/`(devirt 代码生成) |
| exit_criteria | 可去虚拟化的 callvirt 调用全部转换为直接调用 |
| conflict_scope | `src/managed/` |
| estimated_effort | 大 |
| 具体步骤 | (1) 在 Linker 阶段实现可达性驱动的类型层次分析 |
| | (2) 识别"只有一个实现"的虚拟方法 |
| | (3) 识别"密封类 + override"的可去虚拟化场景 |
| | (4) codegen 侧 emit direct call 而非 virtual dispatch |
| | (5) 验证在 hotupdate 场景下去虚拟化与 patching 的交互 |
| | (6) 基准测试收益量化 |

##### p5-lto: LTO 集成

| 字段 | 内容 |
|------|------|
| purpose | LTO（链接时优化）在 build 管线中的集成 |
| requirements | managed proof |
| deliverables | `CMakeLists.txt`(LTO 配置), build pipeline 扩展 |
| exit_criteria | LTO 构建通过，无链接错误，验证产出正确 |
| conflict_scope | `build/`, `CMakeLists.txt` |
| estimated_effort | 中 |
| 具体步骤 | (1) 在 CMake 中启用 LTO（`INTERPROCEDURAL_OPTIMIZATION`） |
| | (2) 验证 MSVC + LTCG 兼容性 |
| | (3) 解决 LTO 导致的符号可见性问题 |
| | (4) 构建时间影响评估 |
| | (5) 基准测试收益量化（体积 + 性能） |

##### p5-paging: 大程序集分页

| 字段 | 内容 |
|------|------|
| purpose | 超大程序集的 C++ 文件自动分页策略 |
| requirements | managed proof |
| deliverables | `src/managed/Chaos.IL2CPP.Generator/`(分页策略) |
| exit_criteria | 任何单 .cpp 文件不超过 50000 行或 2MB |
| conflict_scope | `src/managed/Chaos.IL2CPP.Generator/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 审计当前最大生成文件的大小 |
| | (2) 实现分页阈值控制（方法数/行数/大小） |
| | (3) 跨分页的符号引用处理 |
| | (4) CMake 生成中的多文件注册 |

---

### WS5: CI/CD 完整体系

**目标：** 将 CI/CD 从 2/5 提升至 5/5。覆盖 PR/push 触发、全模式回归、基线对比、失败通知、Dashboard。

**退出条件：**
- PR + push to main 自动触发 CI
- AOT/JIT/Hybrid/Foundation-DLL 全自动回归
- 性能基线自动对比
- 失败自动通知

#### 子任务详情

##### c1-workflow: GH Workflow 框架

| 字段 | 内容 |
|------|------|
| purpose | 建立 CI workflow 骨架和 trigger 机制 |
| requirements | — |
| deliverables | `.github/workflows/ci-framework.yml` |
| exit_criteria | push to main + PR 触发 workflow |
| conflict_scope | `.github/workflows/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 创建 ci-framework.yml |
| | (2) trigger: pull_request (opened/synchronize) + push (main) |
| | (3) 构建矩阵（debug/release, Windows/Linux） |
| | (4) 缓存策略（依赖缓存、构建缓存） |
| | (5) 超时和重试策略 |

##### c2-fd-ci: Foundation-DLL CI 集成

| 字段 | 内容 |
|------|------|
| purpose | foundation-dll pipeline 注入 CI |
| requirements | WS3 f5 协作 |
| deliverables | `.github/workflows/foundation-dll-regression.yml` |
| exit_criteria | PR 自动触发 foundation-dll 验证 |
| conflict_scope | `.github/workflows/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 创建 foundation-dll 专用 workflow |
| | (2) 配置 13-stage pipeline 执行参数 |
| | (3) 结果解析和 PR comment |
| | (4) 缓存和超时设置 |
| | (5) CI smoke 模式（仅关键 families）vs 全量模式（nightly） |

##### c3-jit-baseline: JIT 性能基线

| 字段 | 内容 |
|------|------|
| purpose | JIT 编译时间/执行时间基线入库，PR 自动对比 |
| requirements | WS1 完成 |
| deliverables | `.github/workflows/jit-baseline.yml`, baseline data |
| exit_criteria | 基线数据入库，PR 自动对比 |
| conflict_scope | `.github/workflows/`, `testing/results/benchmarks/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 从 WS1 wf1-tests 承接 JIT benchmark 执行 |
| | (2) 建立 per-commit 基线存储 |
| | (3) PR 自动运行 benchmark 对比基线 |
| | (4) 回归告警阈值设定 |

##### c4-aot-gate: AOT 回归门禁

| 字段 | 内容 |
|------|------|
| purpose | AOT codegen CI 门禁（编译 + 关键 family 快速验证） |
| requirements | — |
| deliverables | `.github/workflows/aot-regression.yml` |
| exit_criteria | AOT 编译 + smoke 验证自动通过 |
| conflict_scope | `.github/workflows/` |
| estimated_effort | 中 |
| 具体步骤 | (1) AOT codegen 编译验证 |
| | (2) 关键 CoreLib family 快速验证（subset of foundation-dll） |
| | (3) 构建产物缓存 |
| | (4) 增量编译支持 |

##### c5-hybrid-gate: Hybrid 回归门禁

| 字段 | 内容 |
|------|------|
| purpose | WS2 完成后集成 hybrid stage 14 到 CI |
| requirements | WS2 完成 |
| deliverables | `.github/workflows/hybrid-regression.yml` |
| exit_criteria | hybrid mode 验证自动通过 |
| conflict_scope | `.github/workflows/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 承接 WS2 h4 的 hybrid runner |
| | (2) 集成到 CI workflow |
| | (3) 混合模式下的 baseline 对比 |

##### c6-notify: 失败通知

| 字段 | 内容 |
|------|------|
| purpose | CI 失败自动通知机制 |
| requirements | c1-c5 就位 |
| deliverables | `.github/workflows/notify.yml` |
| exit_criteria | 失败自动通知 |
| conflict_scope | `.github/workflows/` |
| estimated_effort | 小 |
| 具体步骤 | (1) GitHub Checks API 集成 |
| | (2) 邮件/即时通讯通知配置 |
| | (3) 失败分类（编译失败 vs 测试失败 vs 回归） |
| | (4) 通知频率控制 |

##### c7-dashboard: Dashboard 增强

| 字段 | 内容 |
|------|------|
| purpose | 从单次结果 dashboard 升级为历史趋势 dashboard |
| requirements | — |
| deliverables | `testing/foundation-dll/_core/python/dashboard_enhanced.py` |
| exit_criteria | 历史趋势可视化可用 |
| conflict_scope | `testing/foundation-dll/_core/python/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 分析当前 deep-dashboard.html 的局限 |
| | (2) 设计历史数据模型（per-commit per-family per-stage） |
| | (3) 实现趋势图表（stage pass rate over time, 性能趋势） |
| | (4) 回归告警可视化 |
| | (5) 自托管 dashboard 配置 |

##### c8-threshold: 回归阈值设定

| 字段 | 内容 |
|------|------|
| purpose | 性能回归告警阈值设定与自动化 |
| requirements | c3, c7 完成 |
| deliverables | `testing/config/regression-thresholds.yaml` |
| exit_criteria | 阈值生效，回归自动阻断 |
| conflict_scope | `testing/config/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 分析 baseline 数据的波动范围 |
| | (2) 设定 per-metric 的告警阈值（绝对值 + 百分比） |
| | (3) 实现阈值校验自动化 |
| | (4) 假阳性抑制策略 |

---

### WS7: 工程卓越度

**目标：** 将文档 2/5、管线自测 1/5、基准回归 3/5、压力测试 waived 全部提升至 5/5。

**退出条件：**
- 架构文档体系完整
- Pipeline Python 自测覆盖 >= 80%
- 24h soak 通过
- Hybrid/跨模式压力测试通过
- 基准回归体系自动化

#### 子任务详情

##### e1-arch-docs: 架构文档体系

| 字段 | 内容 |
|------|------|
| purpose | 补全各模式的架构说明、模式选择决策树、故障诊断指南 |
| requirements | — |
| deliverables | 多篇 wiki 文档 |
| exit_criteria | 所有模式文档完备 |
| conflict_scope | `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/` |
| estimated_effort | 中 |
| 具体步骤 | (1) AOT 全量编译器架构更新（PHASE 4+5 完成同步） |
| | (2) JIT T4 架构文档更新（架构抽象后同步） |
| | (3) Hybrid 架构文档（h7 产出） |
| | (4) 三模式选择决策树（适用场景/性能/热更权衡） |
| | (5) 模式间切换指南（AOT→Hybrid, Hybrid→JIT 等） |
| | (6) 故障诊断指南（按模式分类） |

##### e2-pipeline-tests: Pipeline 自测

| 字段 | 内容 |
|------|------|
| purpose | foundation-dll pipeline Python runner 单元测试/集成测试 |
| requirements | — |
| deliverables | `testing/foundation-dll/_core/python/tests/` |
| exit_criteria | 管线自测覆盖 >= 80% |
| conflict_scope | `testing/foundation-dll/_core/python/` |
| estimated_effort | 中 |
| 具体步骤 | (1) orchestrator 单元测试 |
| | (2) 各 stage runner 单元测试（preflight/codegen/fact/audit/asm/microbench/bench/hotupdate） |
| | (3) pipeline config 解析测试 |
| | (4) 集成测试（mock stage runner） |
| | (5) CI 集成（与 WS5 协作） |

##### e3-soak: 24 小时压力测试

| 字段 | 内容 |
|------|------|
| purpose | 恢复 24h soak 测试，建立自动化长期压力框架 |
| requirements | WS1, WS3, WS4 核心完成 |
| deliverables | `testing/scripts/soak-runner.sh`, soak report |
| exit_criteria | 24h continuous run 无崩溃、无内存泄漏 |
| conflict_scope | `testing/scripts/` |
| estimated_effort | 大 |
| 具体步骤 | (1) 设计长期压力场景（GC stress + 多线程 + JIT 编译 + hotupdate 循环） |
| | (2) 自动化 soak runner（循环执行测试套件） |
| | (3) 内存跟踪（VirtualAlloc 统计 + GC 堆统计） |
| | (4) 24h 连续运行 |
| | (5) 结果分析报告 |

##### e4-hybrid-stress: Hybrid 压力测试

| 字段 | 内容 |
|------|------|
| purpose | hybrid 模式下多线程/长时间运行的稳定性验证 |
| requirements | WS2 完成 |
| deliverables | `testing/src/native/jit/test_hybrid_stress.cpp` |
| exit_criteria | 长时间高并发 hybrid 运行无失败 |
| conflict_scope | `testing/src/native/jit/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 多线程循环调用 hybrid 方法（触发频繁 AOT→JIT 升级） |
| | (2) 混合压力：同时运行 hybrid + 纯 AOT + 纯 JIT |
| | (3) 内存泄漏检测 |
| | (4) 长时间运行（4h+） |

##### e5-cross-stress: 跨模式互操作压力

| 字段 | 内容 |
|------|------|
| purpose | AOT+JIT+HotUpdate 混合场景下的长时间运行压力 |
| requirements | WS2, WS4 完成 |
| deliverables | `testing/src/native/stress/test_cross_mode_stress.cpp` |
| exit_criteria | 跨模式互操作长时间运行无崩溃 |
| conflict_scope | `testing/src/native/stress/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 设计跨模式场景：AOT 调用 JIT 编译的方法、Hybrid 方法被 hotpatch |
| | (2) GC 跨模式栈扫描（AOT 栈帧 + JIT 栈帧交替） |
| | (3) 异常跨模式传播（AOT throw → JIT catch，反向） |
| | (4) 长时间运行（4h+） |

##### e6-baseline: 基准回归体系

| 字段 | 内容 |
|------|------|
| purpose | 建立 perf baseline 自动对比流水线（与 WS5 协作） |
| requirements | WS5 c3/c7 协作 |
| deliverables | `testing/results/benchmarks/baseline.db`, 对比脚本 |
| exit_criteria | 基准数据自动采集 + 对比 |
| conflict_scope | `testing/results/benchmarks/` |
| estimated_effort | 中 |
| 具体步骤 | (1) 所有 benchmark 结果统一入库 |
| | (2) 自动基线对比脚本 |
| | (3) 可视化报告生成 |
| | (4) CI 集成（与 WS5 协作） |

##### e7-fd-wiki: Foundation-DLL Wiki 文档

| 字段 | 内容 |
|------|------|
| purpose | 独立的管线架构页面、阶段说明、故障排除指南 |
| requirements | WS3 完成 |
| deliverables | `wiki/07-验证体系/` 系列文档 |
| exit_criteria | 文档覆盖管线架构、阶段说明、故障排除 |
| conflict_scope | `wiki/` |
| estimated_effort | 小 |
| 具体步骤 | (1) 管线架构页面（13-stage 流程图 + runner 职责） |
| | (2) 阶段说明文档（每个 stage 的输入/输出/判断逻辑） |
| | (3) 故障排除指南（常见失败原因 + 排查步骤） |
| | (4) pipeline-config.yaml 配置说明 |

---

## 依赖关系

```
phase-1（并行组，互不依赖）
  ├─ ws1 (JIT 工业化) ─── 独立（native/jit/）
  ├─ ws3 (Foundation-DLL) ─ 独立（python/）
  ├─ ws4 (AOT PHASE 4+5) ─ 独立（managed/）
  ├─ ws5 (CI/CD) ────────── 独立（.github/）
  └─ ws7 (工程卓越度) ──── 独立（wiki/testing/）

phase-2（依赖 phase-1）
  └─ ws2 (Hybrid) ──────── 依赖 ws1 完成（共享 jit_engine.cpp）
                           独立于 ws3/ws4/ws5/ws7
```

**文件冲突矩阵：**

| 工作流 | src/native/jit/ | src/native/runtime-core/ | src/managed/ | testing/ | foundation-dll/ | .github/ | wiki/ |
|--------|----------------|-------------------------|-------------|----------|-----------------|----------|-------|
| WS1 | ✅ | ❌ | ❌ | ✅ | ❌ | ❌ | ❌ |
| WS2 | ✅ | ❌ | ❌ | ✅ | ✅ | ❌ | ✅ |
| WS3 | ❌ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ |
| WS4 | ❌ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ |
| WS5 | ❌ | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ |
| WS7 | ❌ | ❌ | ❌ | ✅ | ✅ | ❌ | ✅ |

> WS1 和 WS2 在 `src/native/jit/` 和 `testing/` 有重叠 → 必须串行（WS1→WS2）
> 其余所有 WS 互不冲突 → 可灵活切换

---

## 风险

| 风险 | 级别 | 影响阶段 | 缓解策略 |
|------|------|---------|---------|
| Liveness 首次启用暴露历史 GC 问题 | 中 | ws1-wf1-liveness | 分两步：先全量发射验证精确扫描正确，再开 liveness 裁剪 |
| OSR segfault 根因复杂 | 中 | ws1-wf1-osr | 3 个向量分别排查，增加防御断言 |
| 16 failures 逐个定位周期超出预期 | 中 | ws3 | 按 P0→P1→P3 优先级，每个设 deadline |
| 文件冲突 | 低 | 全部 | 冲突矩阵已明确，串行化 WS1→WS2 |
| 退化 | 低 | 全部 | 每 WS 提交前全回归 |
| PHASE 5（内联/去虚拟化）收益不确定 | 中 | ws4-p5 | 已有完整计划承诺，执行中尽早做收益评估 |
| CI workflow 配置时间超出预期 | 低 | ws5 | 基础框架优先再细化 |
| Hybrid 端到端测试发现设计缺陷 | 中 | ws2-h2 | 尽早启动，暴露问题后回 brainstorm 修正 |
| 24h soak 发现隐蔽崩溃 | 中 | ws7-e3 | 提前准备修复时间 buffer |

---

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|--------------|---------------|----------------|----------------|
| ws1-jit-industrialization | phase-1 | ready | main | JIT 工业化收官（接口抽象→GC→Liveness→OSR→Unwind→测试） | — | phase-1 | native proof + managed proof | 6 子任务产物 | GC 精确槽就绪、Liveness 默认开启、OSR 无 segfault、测试>=99% | `src/native/jit/`, `testing/src/native/jit/` | 大 |
| ws3-foundation-dll | phase-1 | ready | main | Foundation-DLL 全量验证（16 failures 硬修复） | — | phase-1 | managed proof + native proof | 7 子任务产物 | 140/140 通过、blocking=0 | `testing/foundation-dll/` | 大 |
| ws4-aot-phase-4-5 | phase-1 | ready | main | AOT PHASE 4 HotUpdate 完整化 + PHASE 5 性能优化 | — | phase-1 | managed proof + native proof | 7 子任务产物 | Hotpatch 覆盖率>=99%、内联/去虚拟化/LTO 交付 | `src/managed/`, `src/native/runtime-core/` | 极大 |
| ws5-cicd | phase-1 | ready | main | CI/CD 完整体系（WF 框架/fd CI/JIT 基线/AOT 门禁/Dashboard/通知/阈值） | — | phase-1 | — | 8 子任务产物 | PR 自动触发全回归、基线对比、失败通知 | `.github/workflows/`, `testing/foundation-dll/_core/python/` | 中 |
| ws7-engineering-excellence | phase-1 | ready | main | 工程卓越度（文档/pipeline 自测/24h 压力/跨模式压力/基准回归） | — | phase-1 | native proof | 7 子任务产物 | 文档完备、pipeline 自测>=80%、24h soak 通过 | `wiki/`, `testing/`, `testing/foundation-dll/` | 大 |
| ws2-hybrid-industrialization | phase-2 | planned | main | Hybrid 工业化冲刺（核心验证/端到端/竞争测试/Stage 14/基准/HotUpdate/文档） | ws1-jit-industrialization | phase-2 | native proof + managed proof + hotupdate proof | 7 子任务产物 | Hybrid 三阶段 e2e 通过、Stage 14 >= 90%、基线建立 | `src/native/jit/`, `testing/`, `testing/foundation-dll/` | 中 |

---

## 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child: ws1-jit-industrialization`

### 建议推进顺序（单终端串行视角）

```
Phase-1 启动顺序（按依赖和工作量）:
  1. ws1-jit-industrialization ← 当前 recommended_next_child（关键路径最长，优先启动）
  2. ws5-cicd（可随时启动，与 ws1 交替推进）
  3. ws3-foundation-dll（可随时启动，与 ws1 交替推进）
  4. ws7-engineering-excellence（可随时启动，与 ws1 交替推进）
  5. ws4-aot-phase-4-5（独立 C# 侧，与 ws1 交替推进）

Phase-2 启动（需 ws1 完成）:
  6. ws2-hybrid-industrialization
```

---

## 备选路径

- **WS2（Hybrid）提前启动**：如果 WS1 的 wf1-arch 完成后 HybridPrecode 不受后续 wf1-gc/liveness/osr 影响，可考虑 WS1(arch) → WS2 并行→ WS1(剩余)。当前保守策略（WS1 全完成→WS2）是安全的。
- **WS4 PHASE 5 收益不足时降级**：如果内联/去虚拟化收益低于预期，可降级为 heatmap 驱动模式。需用户确认。
- **WS5 分期交付**：基础门禁（c1-c4）先交付，增强功能（c5-c8）后续迭代。
