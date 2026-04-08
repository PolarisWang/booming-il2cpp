# 统一入口 test all 展示增强 brainstorm

## 背景

当前统一入口里的 `test all` 已经不只是“默认 public suite 批次”，而是：

- public suite 批次
- 当前 host 支持的全部 subject 对象

但展示面仍然停留在旧阶段：

- 菜单入口没有明确告诉用户批次组成
- 运行中页面只有 `Command / Status / Progress / Timeline`
- 完成后的 `summary` 只罗列 `phase / subject / suite`，没有真正把聚合信息转成可读摘要

这会导致用户在执行 `test all` 时看不到三个最关键的问题：

1. 这次到底会跑哪些东西
2. 当前已经过了多少、失败了哪些对象
3. 失败后应该看哪里、怎么重跑

## 目标

在不改变 `test all` 执行语义的前提下，增强展示信息，让统一入口能够把混合批次跑得“可解释”。

## 方案对比

### 方案 A：只补文案

- 改菜单标题
- 改 `summary` 首屏几行文本
- `watch` 页面补 1-2 行说明

优点：

- 成本最低

缺点：

- 解决不了运行中“看不清批次”和完成后“看不清失败对象”的核心问题

### 方案 B：批次可视化，分三层展示

- 启动前预览本次批次范围
- 运行中显示聚合统计和当前对象上下文
- 完成后输出按 family / type / failure digest 组织的摘要

优点：

- 保持统一入口简单
- 能直接回答用户最关心的执行问题
- 与现有 `summary.json / events.jsonl / subjectResults / suiteResults` 结构兼容

缺点：

- 需要补少量聚合字段，不能只改文案

### 方案 C：做复杂 dashboard

- 分页
- 折叠面板
- 动态筛选

优点：

- 信息最全

缺点：

- 明显过重
- 当前收益不匹配复杂度

## 结论

采用方案 B。

## 已确认边界

- 当前只做设计，不实现
- 不改 `test all` 的对象选择语义
- 不改 registry / execution plan 的根结构
- 展示增强覆盖三层：
  - 启动前预览
  - 运行中 progress
  - 完成后 summary
