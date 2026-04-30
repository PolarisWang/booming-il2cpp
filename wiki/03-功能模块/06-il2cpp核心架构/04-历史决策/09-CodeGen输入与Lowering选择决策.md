# CodeGen 输入合约与 Lowering 选择决策

**来源**: `docs/discuss/il2cpp-mainline/20260408-07-*` ~ `20260408-09-*`

## CodeGen 输入消费合约（20260408-07）

**决策**：CodeGen 的权威输入以 `LinkedWorld` 为中心，辅以 canonical semantic lookup 和 stable metadata lookup。

显式禁止用作语义发现源：
- raw typed-il
- instruction shape
- proof-shape

## LoweringFamily 选择面（20260408-08）

**决策**：使用 semantic subject structure + capability bundle 作为 `LoweringFamily` 选择的主决策面。

- `RetainedReasons`、`DependencyReasons`、`Dependencies` 仅作为辅助一致性检查
- 不作为主要分类器

## LoweringFamily 选择规则形状（20260408-09）

**决策**：基于优先级的规则形状。

- Family 从最具体到最通用排序
- 第一个满足 required predicates 且不触发 forbidden predicates 的 family 胜出
- Phase A baseline 不采用 scoring 或 unique-match 方案

## 关键约束

- CodeGen 不得从 IL/shape 反推语义
- Lowering family 选择基于语义结构而非 reason
- 规则形状为优先级优先，非评分优先
