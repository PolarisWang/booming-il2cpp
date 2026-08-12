# DISPATCH.md — JIT A-2 调度总纲

> 调度模式由 roadmap-v1-01.md 的 D1-D4 决策派生：`hybrid`
> JIT 主线（J0-J3）串行，能力线（C1-C4）并行批次。

```yaml
dispatch_model: hybrid
parallel_merge_policy: independent-completed
# parallel_capacity 由运行时指定，不写入文档

batches:
  # ── 第一批次：先固化防丢，串行 ─────────────────────────────
  - batch_id: batch-j0
    mode: sequential
    description: "JIT 主线起点 — 固化 §0 已验证成果"
    tasks:
      - id: T0.1
        conflict_check: no-shared-output
        conflict_scope: "src/native/jit/, tests/unit/runtime-native/jit/, tests/contracts/native/runtime-core/"
        note: "就地提交到 fix/sdk-build-path-authority-converge (D1), commit含 root_cause/fix_strategy/regression_check 三段元数据"

  # ── 第二批次：kQuick 正确性修复，串行 ──────────────────────
  - batch_id: batch-j1
    mode: sequential
    description: "JIT 主线 — 修 T4 kQuick 发射返回 0"
    tasks:
      - id: T1.1
        conflict_check: no-shared-output
        depends_on: []
        exit: "Tier0CodeWorks=42, test_jit_pgo 全过"

  # ── 第三批次：A-2 本体（先拆再改 D3），串行 ─────────────────
  - batch_id: batch-j2
    mode: sequential
    description: "A-2 本体 — monolith拆分(T2.4)→图着色(T2.1)→精确GC(T2.2)→spill契约(T2.3)"
    tasks:
      - id: T2.4
        depends_on: [T0.1]
      - id: T2.1
        depends_on: [T2.4, T1.1]
        note: "图着色接入T4发射主路径, 消除write-through栈往返"
      - id: T2.2
        depends_on: [T2.1]
        note: "liveness→精确物理寄存器掩码+溢出栈槽"
      - id: T2.3
        depends_on: [T2.2]
        note: "GcSlotMapV0序列化+deopt同offset语义, 热更重编译GC map一致"

  # ── 第四批次：技术债，串行 ─────────────────────────────────
  - batch_id: batch-j3
    mode: sequential
    description: "系统性技术债 — 测试群恢复+L11清理+孤儿检查器"
    tasks:
      - id: T3.1
        depends_on: [T0.1]
        note: "全ctest -L jit建基线, 逐一确认test_jit_* ABI"
      - id: T3.2
        depends_on: [T0.1]
        note: "删artifacts/presets/testing陈旧vcxproj"
      - id: T3.3
        depends_on: [T3.1]
        note: "check_workarounds.py+API引用检查孤立门禁"

  # ── 能力线批次：跨域并行（D4: 文档确认后启动）───────────────
  - batch_id: batch-c1
    mode: parallel
    description: "能力线批次1 — benchmark可信度(供J2验收证据)"
    tasks:
      - id: T4.1
        note: "修benchmark伪影ns/alloc=0/fast-path=1.0, mean/median统一"

  - batch_id: batch-c2
    mode: parallel
    description: "能力线批次2 — 消除中间层dispatch (依赖C1)"
    tasks:
      - id: T4.2
        depends_on: [T4.1]
        note: "优化kChaosExternalRuntimeFnTable绕行+intrinsic落interpreter"

  - batch_id: batch-c3
    mode: parallel
    description: "能力线批次3 — 翻译语义缺口(other-family)"
    tasks:
      - id: T4.3
        owner: translation-expert
        note: "GVM/Span<T>/vararg/tail, 走翻译域, 不阻塞JIT主线"

  - batch_id: batch-c4
    mode: parallel
    description: "能力线批次4 — 覆盖盲区+数据治理"
    tasks:
      - id: T4.4
        owner: data-governance
        note: "CoreLib语义方法覆盖+均/中位口径+audit台账更新"

# 合并策略
parallel_merge_policy: independent-completed
# 说明: C1-C4 相互独立, 各自完成后即可 merge, 不等待其它批次
```
