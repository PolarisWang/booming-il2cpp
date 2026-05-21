# Roadmap 0 实现计划讨论记录

**日期：** 2026-04-01 03:29:16 +08:00

**状态：** 已根据分段 design 转写为分段 implementation plan

## 用户原始请求

用户要求把已经拆分好的 design 转成 plan，并强调：

- plan 也需要拆成入口文件和分段文件
- 每一段都要有可落地的 checklist 步骤

## 当前上下文

- `Roadmap 0` 的设计已经完成并拆分为入口文件与 6 段 design 文件
- 相关 design 已逐段确认通过
- 现有 brainstorm 与 finding 已完整记录大部分范围与约束决策

## 关键判断

1. 当前不需要再次做需求发散，直接进入 `writing-plans` 阶段。
2. 由于 design 已按段拆分，plan 最自然的组织方式是：
   - 一个入口 plan 文件
   - 多个 chunk plan 文件
3. 实现计划不应只复述设计摘要，而应补足：
   - 精确文件路径
   - 可执行 checklist 步骤
   - 验证命令
   - 每个 chunk 的落盘顺序
4. 计划 chunk 数量不必和 design 段数完全一致，更应该按实现推进顺序收敛。

## 本轮结论

本轮 plan 采用以下结构：

1. 入口 plan 文件：说明总目标、chunk 索引、全局约束、完成标准
2. `Chunk 1`：仓库骨架、第三方规则与产物目录
3. `Chunk 2`：schema pack 与 trace contract
4. `Chunk 3`：native contract pack（ABI / bridge）
5. `Chunk 4`：5 个 smoke 输入项目与子场景
6. `Chunk 5`：平台矩阵、harness 与 `verify-roadmap-0`

## 与 design 的关系

- design 仍是规格基线
- plan 负责把 design 转成可执行步骤
- 本轮没有新增产品范围决策，主要新增的是：
  - chunk 划分
  - 精确文件路径
  - checklist 步骤
  - 验证命令

## 讨论日志

- 2026-04-01 03:23:47 +08:00：用户要求把 design 拆成入口文件和每段单独文件。
- 2026-04-01 03:23:47 +08:00：已完成 design 拆分，形成入口 design 与 6 段 design 文件。
- 2026-04-01 03:29:16 +08:00：用户进一步要求把 design 转成 plan，并强调需要可落地的 checklist 步骤。
- 2026-04-01 03:29:16 +08:00：确认进入 `writing-plans` 阶段，不再重复 brainstorm。
- 2026-04-01 03:29:16 +08:00：决定把 plan 也拆分为入口文件与多个 chunk 文件，并按实现顺序而不是设计段数组织 chunk。
