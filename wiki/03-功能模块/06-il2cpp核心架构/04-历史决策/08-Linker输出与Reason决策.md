# Linker 输出、Reason 分类法与 EmissionOrder 决策

**来源**: `docs/discuss/il2cpp-mainline/20260408-02-*` ~ `20260408-06-*`

## Linker 输出合约（20260408-02）

**决策**：`LinkedWorld` 输出包含：

- entries
- retained types/members + declarative reasons
- external managed dependencies
- stable emission order
- linker-scoped diagnostics

不得输出：lowering family、runtime roles、host bindings、template kind、literal bindings。

## Reason 分类法粒度（20260408-03）

**决策**：Phase A baseline 使用单层语义原因。

- `Linker` 只输出语义层 `RetainedReasons` 和 `DependencyReasons`
- 双层原因模型（Path 3）作为未来升级方向保留

## Reason 词汇表（20260408-04）

**决策**：两个 reason family 共享命名空间：

| Reason | 分类 | 说明 |
|--------|------|------|
| `structural.entry-root` | structural | 入口根 |
| `structural.entry-reachable` | structural | 从入口可达 |
| `semantic.*` (8-item min) | semantic | 8 项最小语义原因集 |

`DependencyReasons` 只使用 `semantic.*`。任何泄漏 lowering/template/runtime/host/engine 决定的原因名称被明确禁止。

## EmissionOrder 合约（20260408-05）

**决策**：`EmissionOrder` 覆盖：

- `EntryOrder` — 入口顺序
- `TypeOrder` — 类型顺序
- `MemberOrder` — 成员顺序
- `DependencyOrder` — 依赖顺序

作为 `LinkedWorld` 正式合约的一部分。它是稳定的语义遍历合约，不是代码生成调度或模板渲染脚本。

## Linker 诊断合约（20260408-06）

**决策**：Linker 拥有 4 个诊断类别：

- `entry` — 入口诊断
- `closure` — 闭包诊断
- `dependency` — 依赖诊断
- `order/invariant` — 顺序/不变式诊断

任何 error 级别的诊断会阻止 `CodeGen` 消费该 `LinkedWorld`。

## 关键约束

- Linker 输出不含 lowering 级信息
- Reason 词汇表严格禁止泄漏低层级决定
- EmissionOrder 是语义合约，不是渲染脚本
