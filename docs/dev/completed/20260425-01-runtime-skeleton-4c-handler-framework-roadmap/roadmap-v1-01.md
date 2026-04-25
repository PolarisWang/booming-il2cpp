# 20260425-01 Runtime Skeleton 4C Handler Framework Roadmap

## 1. 目标

建立一个独立于 foundation audit 主线的 runtime-skeleton `4C family/handler` 框架升级路线图，把当前 `NativeReferenceProofEmitter` 的 runtime-skeleton builder 体系从线性分发表升级为统一的：

- `BuildContext`
- `MatchResult / BuildResult`
- `Family`
- `Handler`
- ordered dispatcher
- legacy cutover + final cleanup

完成后，后续 `System.Convert` 与其他 CoreLib family 的 coverage 扩张，应建立在新框架上推进，而不是继续向旧 dispatcher 堆叠条件分支。

## 2. 范围边界

本 roadmap 覆盖：

- `NativeReferenceProofEmitter` 中 runtime-skeleton 分支调度模型升级
- runtime-skeleton family/handler contract
- runtime-skeleton diagnostics / unsupported reason / ordering 规则收敛
- legacy runtime-skeleton builder 迁移与删旧
- 对应 tests/contracts/canonical gate 调整

本 roadmap 不覆盖：

- 全量关闭 `System.Convert` coverage gap
- Complex BCL 13 DLL lane
- 非 runtime-skeleton 的其他 codegen 大改
- foundation audit 父 roadmap 的继续自动推进

## 3. 非目标

- 不把整个 `Chaos.IL2CPP.CodeGen` 全部插件化
- 不把所有 emitter 一次拆成大量新项目或外部框架
- 不在本 roadmap 中承诺 CoreLib translation truth boundary 全关闭
- 不把 `Convert` 所有未覆盖 lane 在本 roadmap 内全部补完

## 4. 阶段列表

### Phase 1: 4C Framework Foundation

- 建立 runtime-skeleton 的 `context/result/family/handler` 基础 contract
- 定义 ordered dispatcher 和 legacy coexistence 规则

### Phase 2: Dispatcher Cutover

- 把 runtime-skeleton 总入口切到 family router
- 保持已有支持 family 行为等价

### Phase 3: Family Migration Batch A

- 优先迁移 `System.Convert`
- 同步迁移最相关的 shared helper / matcher / reason code

### Phase 4: Family Migration Batch B

- 迁移剩余主要 runtime-skeleton family
- 收敛 unsupported reason、priority、diagnostics

### Phase 5: Legacy Cleanup And Verification

- 删除旧 runtime-skeleton legacy dispatcher 路径
- 补齐 tests/contracts/canonical verification
- 明确恢复旧 foundation audit 主线的入口

## 5. 每阶段完成定义

### Phase 1

- goal:
  - 产出 4C framework 正式 design 与首批实现计划
- exit_criteria:
  - `BuildContext / MatchResult / BuildResult / Family / Handler` contract 已定稿
  - 新旧并存 cutover 规则明确
- deliverables:
  - design 文档
  - foundation cutover plan
- dependencies:
  - 当前 runtime-skeleton codegen 现状与现有 tests 基线
- resolved_decisions:
  - 采用完整 4C 方向
  - 旧 foundation audit 主线在本 roadmap 完成前挂起
- watch_items:
  - contract 是否过早固化仍在演化的 family 边界

### Phase 2

- goal:
  - runtime-skeleton 总 dispatcher 切换到 family router
- exit_criteria:
  - 新 router 可驱动至少一批 family
  - 现有关键 tests 无回退
- deliverables:
  - dispatcher cutover 代码
  - 对应 tests
- dependencies:
  - Phase 1 contract 完成
- resolved_decisions:
  - first-match ordered dispatch
  - family/handler priority 必须显式稳定
- watch_items:
  - 新旧 dispatcher 并存期的行为漂移

### Phase 3

- goal:
  - 完成 `System.Convert` 作为首个正式 family 的迁移
- exit_criteria:
  - Convert family 在 4C 框架下可运行
  - legacy Convert 入口不再是主路径
- deliverables:
  - Convert family handlers
  - Convert family tests/contracts
- dependencies:
  - Phase 2 cutover 完成
- resolved_decisions:
  - Convert 是首个迁移 family
- watch_items:
  - Convert 家族边界是否仍继续外扩

### Phase 4

- goal:
  - 迁移剩余主要 runtime-skeleton family
- exit_criteria:
  - 旧模式不再承接主要 family
- deliverables:
  - family batch migration
  - diagnostics/reason alignment
- dependencies:
  - Phase 3
- resolved_decisions:
  - family 迁移按风险与共享 helper 依赖排序
- watch_items:
  - shared helper 拆分对稳定 family 的回归

### Phase 5

- goal:
  - 收口 legacy，形成恢复旧 foundation audit 的稳定基座
- exit_criteria:
  - legacy dispatcher 删除
  - canonical 与 contract gate 通过
  - 恢复入口写回挂起任务
- deliverables:
  - cleanup code
  - verification evidence
  - recovery note
- dependencies:
  - Phase 4
- resolved_decisions:
  - 本 roadmap 完成后再恢复旧主线
- watch_items:
  - canonical 证据与 unsupported reason 是否一致

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260425-02-runtime-skeleton-4c-framework-foundation-cutover` | Phase 1 | in-progress | codex | 落 4C 基础 contract、design、plan，并启动首轮 cutover | - |
| `20260425-03-runtime-skeleton-4c-dispatcher-cutover` | Phase 2 | planned | codex | 切 runtime-skeleton dispatcher 到 family router | `20260425-02` |
| `20260425-04-runtime-skeleton-4c-convert-family-migration` | Phase 3 | planned | codex | 将 `System.Convert` 迁为首个正式 4C family | `20260425-03` |
| `20260425-05-runtime-skeleton-4c-remaining-family-migration` | Phase 4 | planned | codex | 迁移剩余主要 runtime-skeleton families | `20260425-04` |
| `20260425-06-runtime-skeleton-4c-legacy-cleanup-and-restore-foundation-roadmap` | Phase 5 | planned | codex | 删除 legacy dispatcher，完成验证并恢复旧主线入口 | `20260425-05` |

## 7. 依赖

- 现有 `NativeReferenceProofEmitter` runtime-skeleton codegen 主线
- 当前 tests/contracts/canonical 事实基线
- 被挂起的 foundation audit 主线恢复入口

## 8. 风险

- `4C` 可能过早固化尚在演化的 family 边界
- 大规模 dispatcher cutover 可能导致已有 runtime-skeleton family 回归
- 新旧并存阶段可能出现 unsupported reason 不一致
- 如果 design 不够克制，可能把 emitter 任务误升级成过度框架化

## 9. 备选路径

- 备选路径 A：回退到“仅最小整理 + 继续 Convert 扩张”
  - 当前已被用户显式放弃
- 备选路径 B：只做 `Convert` 局部 4B-lite
  - 当前已被用户升级为完整 `4C` 方向

## 10. 当前建议推进顺序

1. 先完成 `20260425-02`
2. 再切 `20260425-03`
3. 然后迁 `20260425-04`
4. 最后推进 `20260425-05` 和 `20260425-06`

## 11. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- 当前唯一推荐下一 child：
  - `20260425-02-runtime-skeleton-4c-framework-foundation-cutover`
- 旧主线恢复规则：
  - 仅当本 roadmap 完成后，才恢复：
    - `docs/dev/hanging/20260419-01-foundation-dll-translation-audit-roadmap`
    - `docs/dev/hanging/20260419-37-corelib-supplemental-runtime-skeleton-coverage-widening`
