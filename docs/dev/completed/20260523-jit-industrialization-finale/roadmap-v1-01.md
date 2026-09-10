# JIT 工业化收官 — Roadmap v1

> task_id: 20260523-jit-industrialization-finale
> dispatch_model: hybrid
> created: 2026-05-23

**目标：** 将 JIT 子系统（T4 Native Code Generator）从功能原型级提升到工业化生产级。覆盖全部 18 个已识别问题，包含核心管线补全、Linux 平台支持、TLAB 内联分配、调试工具集成、ARM64 支持。

**上游设计**：brainstorm 阶段已确认全部设计拍板。
**全局优先级**：P1（性能最优）> P2（方案完美性）> P3（HotUpdate 支持）

---

## 范围边界

- **范围内**：src/native/jit/、src/native/interpreter/（有限）、src/native/runtime-core/（有限）、testing/src/native/jit/、CI pipeline
- **范围外**：Interpreter T1-T3 执行引擎改造；runtime-core GC 算法改动；managed/codegen 侧 C# 编译器改动
- **架构决策**（brainstorm 已确认）：
  - IEncoder 接口抽象（x64/ARM64）
  - ISehHandler 接口抽象（VEH/Signal）
  - TLAB 内联分配（纯代码生成优化，不改分配策略）
  - 调试信息：CodeView + DWARF

---

## 非目标

- 不改变 Interpreter (T1-T3) 的执行路径
- 不做 T4 JIT 全模块重写——只做目标问题修复和架构抽象
- 不改变 managed side C# 编译器输出格式
- ARM64 不要求首次提交即达到 x64 同等性能，只要求功能正确

---

## 工作流架构概览

```
工作流 1: 核心管线补全 (串行内部6子任务)
  ├─ 1a: GC 精确槽映射 + GPR 条目
  ├─ 1b: 开启 Liveness + 验证
  ├─ 1c: OSR segfault 修复 (3个向量)
  ├─ 1d: Unwind 编译守卫 (static_assert)
  ├─ 1e: 模块级测试套件补齐
  └─ 1f: CI 性能基线建立

工作流 2: Linux 平台支持 (独立)
  ├─ 2a: POSIX signal handler (替代 VEH)
  ├─ 2b: DWARF CFI 完整验证
  └─ 2c: Linux CI 集成

工作流 3: TLAB 内联分配 (等待流1完成，共享 jit_engine.cpp)
  ├─ 3a: GC 分配路径分析
  ├─ 3b: EmitTLABAlloc 内联发射
  ├─ 3c: NewObj/Box/NewArr 内联集成
  └─ 3d: GC slot map + safepoint 协作

工作流 4: 调试与工具 (独立)
  ├─ 4a: T4 调试信息发射 (CodeView + DWARF)
  └─ 4b: SOS 调试器扩展适配

工作流 5: ARM64 支持 (独立，等待编码器接口)
  ├─ 5a: ARM64 指令编码器
  ├─ 5b: ARM64 帧布局 + Prologue/Epilogue
  ├─ 5c: ARM64 SEH
  └─ 5d: ARM64 测试套件
```

---

## 依赖关系

```
流1 (串行6个子任务) ──(jit_engine.cpp 冲突)── 流3 (流1完成后启动)
       │                                              │
       │  (无文件冲突)                                  │  (无文件冲突)
       ▼                                              ▼
流2 (jit_seh.cpp 等)                             流4 (独立文件)
       │
       │  (编码器接口抽象完成)
       ▼
流5 (全新编码器，完全独立)
```

---

## 风险

| 风险 | 级别 | 阶段 | 缓解 |
|------|------|------|------|
| Liveness 首次启用可能暴露旧有 GC 问题 | 中 | 1b | 分两步：先全量发射验证精确扫描正确，再开 liveness 裁剪 |
| Linux signal handler 语义差异（VEH 可修改 RIP 继续执行，signal 不行） | 中 | 2a | 使用 siglongjmp 回退到解释器 + 重建帧 |
| ARM64 编码器规模被低估 | 中高 | 5a | x64_encoder.h 928 行 → ARM64 估算 1500-2000 行 |
| 编码器接口虚函数影响 JIT 编译速度 | 低 | 1a/5a | 虚函数调用 ~ 几百次/方法编译，总开销 < 1µs，可忽略 |
| TLAB 内联后 GC slot 注册遗漏 | 中 | 3d | 强制检查路径：内联分配的 safepoint 必须记录 GC point |
| 多工作流并行时回归定位困难 | 低 | 全部 | 每个工作流合入前跑全回归 |

---

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|--------------|---------------|----------------|----------------|
| wf1-arch | 1a | planned | main | IEncoder/ISehHandler 接口抽象 + jit_engine.cpp 重构 | — | batch-1 | native proof | x64_encoder.h→IEncoder.h, jit_seh.h→ISehHandler.h, jit_engine.cpp 重构 | 架构可编译，全回归通过 | src/native/jit/ | 中 |
| wf1-gc | 1b | planned | main | GC 精确槽映射：全量发射 + GPR 条目 | wf1-arch | batch-1 | native proof + managed proof | jit_engine.cpp, jit_method.h | GcSlotMapV0 含 GPR 条目；nm->gc_slot_map 非空 | src/native/jit/ | 大 |
| wf1-liveness | 1c | planned | main | 开启 Liveness + 验证 | wf1-gc | batch-1 | native proof | jit_engine.cpp | liveness 默认开启，GC 精确扫描通过 | src/native/jit/ | 中 |
| wf1-osr | 1d | planned | main | OSR segfault 修复 | wf1-liveness | batch-1 | native proof | jit_helpers.cpp, jit_engine.cpp | Test_OsrEntry 不 segfault, 防御检查就位 | src/native/jit/ | 小 |
| wf1-unwind | 1e | planned | main | Unwind 编译守卫 | wf1-osr | batch-1 | native proof | jit_unwind.cpp | static_assert 就位 | src/native/jit/ | 极小 |
| wf1-tests | 1f | planned | main | 模块级测试套件补齐 | wf1-unwind | batch-1 | managed proof + native proof | testing/jit 扩展 | 全回归通过，新测试通过 | testing/src/native/jit | 大 |
| wf1-ci | 1g | planned | main | CI 性能基线建立 | wf1-tests | batch-1 | native proof | CI pipeline 配置 | 基线数据入库，构建时自动对比 | .github/workflows/ | 中 |
| wf2-linux | 2 | planned | main | Linux 平台 SEH + DWARF + CI | — | batch-2 | native proof | jit_seh.cpp Linux signal handler, jit_unwind.cpp, CI Linux job | Linux x64 T4 JIT 可用，回归通过 | src/native/jit/ | 大 |
| wf3-tlab | 3 | planned | main | TLAB 内联分配 | wf1-arch, wf1-tests | batch-3 | native proof | jit_engine.cpp, jit_helpers.cpp | NewObj/Box TLAB bump path，无辅助函数调用 | src/native/jit/ | 大 |
| wf4-debug | 4 | planned | main | 调试信息 + SOS 集成 | — | batch-2 | native proof | jit_engine.cpp, jit_unwind.cpp | T4 代码可调试，SOS 可解析栈帧 | src/native/jit/ | 大 |
| wf5-arm64 | 5 | planned | main | ARM64 完整支持 | wf1-arch | batch-4 | native proof | arm64_encoder.h, arm64_seh.cpp, 测试 | ARM64 回归通过 | src/native/jit/ | 极大 |

---

## 调度策略

```yaml
dispatch_model: hybrid
parallel_merge_policy: independent-completed

batches:
  - batch_id: batch-1
    mode: sequential        # 流1内部6子任务串行
    description: "核心管线补全（编码器抽象 → GC → Liveness → OSR → Unwind → 测试 → CI）"
    tasks:
      - id: wf1-arch
      - id: wf1-gc
      - id: wf1-liveness
      - id: wf1-osr
      - id: wf1-unwind
      - id: wf1-tests
      - id: wf1-ci
    wait_for: []
    merge_after: all-completed

  - batch_id: batch-2
    mode: parallel           # Linux + 调试并行
    description: "Linux 平台支持和调试工具（与 batch-1 串行，内部并行）"
    tasks:
      - id: wf2-linux
        conflict_check: no-shared-output
      - id: wf4-debug
        conflict_check: no-shared-output
    # wf2 改 jit_seh.cpp, jit_unwind.cpp; wf4 改 jit_engine.cpp + 新建文件
    # 两者以 jit_seh.h 为共享合约，修改范围无重叠
    wait_for: [batch-1]
    merge_after: all-completed

  - batch_id: batch-3
    mode: sequential
    description: "TLAB 内联分配（等待 batch-1 完成，共享 jit_engine.cpp）"
    tasks:
      - id: wf3-tlab
    wait_for: [batch-1]
    merge_after: all-completed

  - batch_id: batch-4
    mode: sequential
    description: "ARM64 完整支持"
    tasks:
      - id: wf5-arm64
    wait_for: [batch-2, batch-3]
    merge_after: all-completed
```

---

## 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child: wf1-arch`
