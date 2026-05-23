# Foundation DLL 工业化级验证 — Roadmap v1-01

## 目标

对 14 个 assembly、~135 个 family 完成 13-stage 工业化级全覆盖验证。每个 family 跑通：preflight → codegen AOT → jit_codegen → fact AOT → fact JIT → audit → asm_compare → microbench → benchmark → hotupdate ×4。

## 范围边界

- 14 个 assembly（CoreLib、Linq、Collections.Immutable、S.R.IS、Text.Json、Formats.Asn1、Brotli、ZipFile、Pipelines、ServerSentEvents、ObjectModel、Serialization.Formatters、Security.Principal.Windows、Tasks.Parallel）
- 所有具备 contract.json 的 family
- 推进粒度：family 级
- CoreLib 优先攻坚

## 非目标

- 不新增 family 或 method subject
- 不修改 contract.json 内容
- 不重构测试基础设施

## 全局优先级约束

P1 性能最优、P2 方案完美性、P3 HotUpdate 方向一致，无冲突。所有阶段默认继承此裁决。

## 阶段列表

### Phase 0: 基础设施搭建

- **Goal**: 确保 13-stage 管线可端到端运行，JIT/hotupdate flag 可靠
- **Exit Criteria**: 至少 1 个 family（如 Brotli/decoder）完整跑通 13-stage 并输出 UnifiedReport
- **Deliverables**: 修改后的 batch_run_all.py、pipeline_adapter.py（如需）
- **Dependencies**: 无
- **Resolved Decisions**: JIT codegen 使用 verification/ → testing/ 迁移方案；HotUpdate flag 入口确认

### Phase 1: CoreLib 优先攻坚

- **Goal**: System.Private.CoreLib 全部 47 个 family 通过 13-stage
- **Exit Criteria**: 47/47 family 的 13-stage 全部 passed（合法 skip 可接受）
- **Deliverables**: CoreLib 各 family 的 UnifiedReport
- **Dependencies**: Phase 0
- **Watch Items**: C# 编译错误（GC.LatencyMode 等 API 兼容性）；Codegen 覆盖率不足导致的 audit 失败

### Phase 2: 小 Assembly 热身

- **Goal**: 5 个小 assembly（19 families）通过 13-stage
- **Exit Criteria**: 19/19 family 全部通过
- **Deliverables**: 各 assembly 的批量报告
- **Dependencies**: Phase 0（不依赖 Phase 1，可并行推进）
- **Resolved Decisions**: 小 assembly 用于验证基础设施可扩展性

### Phase 3: 中等 Assembly

- **Goal**: 3 个中等 assembly（18 families）通过 13-stage
- **Exit Criteria**: 18/18 family 全部通过
- **Deliverables**: 各 assembly 的批量报告
- **Dependencies**: Phase 0

### Phase 4: 大 Assembly 收尾

- **Goal**: 5 个大 assembly（53 families）通过 13-stage
- **Exit Criteria**: 53/53 family 全部通过
- **Deliverables**: 各 assembly 的批量报告 + 全量汇总 dashboard
- **Dependencies**: Phase 0（Text.Json 和 Formats.Asn1 需先修复缺失 subject ID）

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|--------------|---------------|----------------|-----------------|
| fd-p0-infra | 0 | planned | main | 基础设施搭建（batch_run_all 扩展、JIT/hotupdate 通路验证） | — | — | 端到端验证 1 family 通过 13-stage | batch_run_all.py patch, pipeline_adapter.py patch | 1 family 完整跑通 13-stage | testing/foundation-dll/_core/ | 3h |
| fd-p1-corelib-b1 | 1 | planned | main | CoreLib batch 1: array-indexing-copy → boxing-unboxing-casts → buffer-memory → ccw-basic → codegen-edge-cases → collections-generic-core → convert-char (7 families) | fd-p0-infra | — | 7 families 13-stage | UnifiedReport x7 | 7/7 passed | testing/foundation-dll/System.Private.CoreLib/<family>/ | 4h |
| fd-p1-corelib-b2 | 1 | planned | main | CoreLib batch 2: delegate-core-invocation → dispatch-basic → enum-parsing → error-info-basic → exception-throw-diagnostics → garbage-collection → generic-nullable-value (7 families) | fd-p1-corelib-b1 | — | 7 families 13-stage | UnifiedReport x7 | 7/7 passed | testing/foundation-dll/System.Private.CoreLib/<family>/ | 5h |
| fd-p1-corelib-b3 | 1 | planned | main | CoreLib batch 3: globalization-culture → guid-random-hashcode → interface-dispatch → io-streams-basics → math-numerics → object-equality-identity → pinvoke-dllimport (7 families) | fd-p1-corelib-b2 | — | 7 families 13-stage | UnifiedReport x7 | 7/7 passed | testing/foundation-dll/System.Private.CoreLib/<family>/ | 4h |
| fd-p1-corelib-b4 | 1 | planned | main | CoreLib batch 4: primitive-numeric-conversions → primitive-numeric-conversions-core → rcw-basic → reflection-activation → reflection-assembly → reflection-binding → reflection-field-property (7 families) | fd-p1-corelib-b3 | — | 7 families 13-stage | UnifiedReport x7 | 7/7 passed | testing/foundation-dll/System.Private.CoreLib/<family>/ | 4h |
| fd-p1-corelib-b5 | 1 | planned | main | CoreLib batch 5: reflection-generics → reflection-member-basics → reflection-member-complete → reflection-module → reflection-parameters → reflection-type → runtime-compiler-services (7 families) | fd-p1-corelib-b4 | — | 7 families 13-stage | UnifiedReport x7 | 7/7 passed | testing/foundation-dll/System.Private.CoreLib/<family>/ | 4h |
| fd-p1-corelib-b6 | 1 | planned | main | CoreLib batch 6: snapshot-prover → span-memory-buffers → string-char-text-core → string-construction → threading-monitor-interlocked → threading-sync → threading-tasks-primitives (7 families) | fd-p1-corelib-b5 | — | 7 families 13-stage | UnifiedReport x7 | 7/7 passed | testing/foundation-dll/System.Private.CoreLib/<family>/ | 5h |
| fd-p1-corelib-b7 | 1 | planned | main | CoreLib batch 7: threading-thread-basics → time-date-time-timespan → type-runtime-handles → attributes-custom-metadata → interface-dispatch,... → (5 families + 1 combined) | fd-p1-corelib-b6 | — | 6 families 13-stage | UnifiedReport x6 | 6/6 passed | testing/foundation-dll/System.Private.CoreLib/<family>/ | 4h |
| fd-p2-brotli | 2 | planned | main | System.IO.Compression.Brotli (4 families: decoder, encoder, options, stream) | fd-p0-infra | — | 4 families 13-stage | UnifiedReport x4 | 4/4 passed | testing/foundation-dll/System.IO.Compression.Brotli/<family>/ | 1.5h |
| fd-p2-zipfile | 2 | planned | main | System.IO.Compression.ZipFile (2 families) | fd-p2-brotli | — | 2 families 13-stage | UnifiedReport x2 | 2/2 passed | testing/foundation-dll/System.IO.Compression.ZipFile/<family>/ | 1h |
| fd-p2-pipelines | 2 | planned | main | System.IO.Pipelines (5 families) | fd-p2-zipfile | — | 5 families 13-stage | UnifiedReport x5 | 5/5 passed | testing/foundation-dll/System.IO.Pipelines/<family>/ | 1.5h |
| fd-p2-sse | 2 | planned | main | System.Net.ServerSentEvents (4 families) | fd-p2-pipelines | — | 4 families 13-stage | UnifiedReport x4 | 4/4 passed | testing/foundation-dll/System.Net.ServerSentEvents/<family>/ | 1.5h |
| fd-p2-tasks-parallel | 2 | planned | main | System.Threading.Tasks.Parallel (4 families) | fd-p2-sse | — | 4 families 13-stage | UnifiedReport x4 | 4/4 passed | testing/foundation-dll/System.Threading.Tasks.Parallel/<family>/ | 1.5h |
| fd-p3-objectmodel | 3 | planned | main | System.ObjectModel (6 families) | fd-p2-tasks-parallel | — | 6 families 13-stage | UnifiedReport x6 | 6/6 passed | testing/foundation-dll/System.ObjectModel/<family>/ | 2h |
| fd-p3-security-windows | 3 | planned | main | System.Security.Principal.Windows (6 families) | fd-p3-objectmodel | — | 6 families 13-stage | UnifiedReport x6 | 6/6 passed | testing/foundation-dll/System.Security.Principal.Windows/<family>/ | 2h |
| fd-p3-formatters | 3 | planned | main | System.Runtime.Serialization.Formatters (6 families) | fd-p3-security-windows | — | 6 families 13-stage | UnifiedReport x6 | 6/6 passed | testing/foundation-dll/System.Runtime.Serialization.Formatters/<family>/ | 2h |
| fd-p4-immutable | 4 | planned | main | System.Collections.Immutable (10 families) | fd-p3-formatters | — | 10 families 13-stage | UnifiedReport x10 | 10/10 passed | testing/foundation-dll/System.Collections.Immutable/<family>/ | 3h |
| fd-p4-linq | 4 | planned | main | System.Linq (12 families) | fd-p4-immutable | — | 12 families 13-stage | UnifiedReport x12 | 12/12 passed | testing/foundation-dll/System.Linq/<family>/ | 4h |
| fd-p4-sris | 4 | planned | main | System.Runtime.InteropServices (14 families) | fd-p4-linq | — | 14 families 13-stage | UnifiedReport x14 | 14/14 passed | testing/foundation-dll/System.Runtime.InteropServices/<family>/ | 5h |
| fd-p4-text-json | 4 | planned | main | System.Text.Json (13 families) | fd-p4-sris | — | 13 families 13-stage + 修复 attributes family | UnifiedReport x13 | 13/13 passed | testing/foundation-dll/System.Text.Json/<family>/ | 5h |
| fd-p4-asn1 | 4 | planned | main | System.Formats.Asn1 (6 families) | fd-p4-text-json | — | 6 families 13-stage + 修复 encoding-rules family | UnifiedReport x6 | 6/6 passed | testing/foundation-dll/System.Formats.Asn1/<family>/ | 2h |

## 依赖

- Phase 0 → Phase 1-4：基础设施必须先就绪
- Phase 1 CoreLib batches：串行推进，逐 batch 验证
- Phase 2-4：串行推进（终端容量 1），但 Phase 2-4 与 Phase 1 无直接依赖（都只依赖 Phase 0）
- Text.Json 和 Formats.Asn1 各有一个 family 缺少 subject IDs，需在对应子任务中修复

## 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| JIT codegen 对某些 family 不通 | 中 | 高 | Phase 0 用 Brotli/decoder 验证；skip + 记录 |
| HotUpdate 未接入 entry.exe | 中 | 高 | Phase 0 确认；skip + 开基础设施 issue |
| CoreLib 47 family 编译/运行时错误多 | 高 | 高 | 从简单 family 开始，增量修复 |
| Benchmark 基线不可比 | 中 | 中 | 只做回归检查，不做绝对值 |
| asm_compare 对复杂 IL 序列误报 | 低 | 中 | audit stage 中有 false_passing 机制 |
| 总执行时间 12-60h | 高 | 中 | 支持 `--family` 过滤，增量；可中断恢复 |

## 备选路径

- 如果 JIT codegen 大面积不可用 → 降级为 "AOT 优先 + JIT 按需"：Phase 0 验证后决策
- 如果 CoreLib 编译错误过多 → 不阻塞 Phase 2-4，先跑小 assembly 积累经验
- 如果 hotupdate flag 未就绪 → skip hotupdate ×4 stage，标记为基础设施不足

## 当前建议推进顺序

```
fd-p0-infra → fd-p1-corelib-b1 → ... → fd-p1-corelib-b7 →
fd-p2-brotli → fd-p2-zipfile → fd-p2-pipelines → fd-p2-sse → fd-p2-tasks-parallel →
fd-p3-objectmodel → fd-p3-security-windows → fd-p3-formatters →
fd-p4-immutable → fd-p4-linq → fd-p4-sris → fd-p4-text-json → fd-p4-asn1
```

## 子任务执行策略

- child_execution_mode: auto
- auto_continue: true
- auto_stop_policy: blocking-only
- dispatch_model: sequential
- recommended_next_child: fd-p0-infra
