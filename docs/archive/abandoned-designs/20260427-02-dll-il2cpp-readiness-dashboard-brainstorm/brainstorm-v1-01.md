# DLL IL2CPP Readiness Dashboard Brainstorm v1

Date: 2026-04-27 02:05:00 +08:00
Status: in-progress

## 1. 目标

把 foundation-dll dashboard 从“proof 字段堆叠视图”改成“DLL 从开发到真正可用于 IL2CPP 的生命周期视图”。

它必须先回答四个问题：

1. 这个 DLL 现在处于哪个开发阶段
2. 它能不能真正被 IL2CPP 使用
3. 如果还不能，卡在哪个主闸门
4. 下一步最该做什么

## 2. 当前痛点

当前 dashboard 的结构混乱，核心原因有三点：

- 把不同性质的进度混成一个面板：
  - native proof
  - subject validation
  - hotupdate readiness
  - project execution evidence
- 顶层页面展示了大量只适合详情页的问题
- 缺少“能否真正用于 IL2CPP”的主判断对象

## 3. 先回答用户的 3 个问题

### 3.1 Native proof 如何提前生成好，native 产物正确性如何验证

`Native Proof` 应拆成两层：

#### A. 生成层

回答“native 产物是否已经生成、claim 是否已经建好”。

最低来源：

- family verification claims
- native reference plan
- coverage artifact
- codegen artifact index

这层只能说明：

- 该 family / DLL 有哪些待验证方法
- native translation 当前宣称覆盖到哪些方法
- 产物是否已经存在

#### B. 验证层

回答“这些 native 产物是否正确，是否真的可用”。

要拿到尽可能可信的结果，不能只靠产物存在或 coverage 百分比。

推荐按强度分层：

1. `artifact-exists`
   只说明生成了
2. `coverage-closed`
   说明 claim 方法集合已被 native proof 闭合
3. `runtime-executable`
   说明至少有 runtime / host 验证跑通
4. `il2cpp-usable`
   说明 DLL 已能真正进入 IL2CPP 使用路径

结论：

- `100% 靠谱` 不能只靠 native proof 进度条
- 顶层必须引入独立的 `IL2CPP Usable` 闸门
- `Native Completeness = 100%` 只代表“翻译闭合”，不代表“真的可用”

### 3.2 Subject validation 是什么，能否代表整个 DLL 翻译过程

`Subject Validation` 不是 native proof。

它回答的是：

- 是否存在行为级验证
- 是否有 subject / runtime 测试证明这些能力在宿主链路里可执行

它的价值是：

- 防止“翻译看起来完成，但实际行为不可用”
- 给 `IL2CPP Usable` 提供 runtime/behavior 侧证据

但它**不能单独代表整个 DLL 翻译过程**，因为：

- 它可能只覆盖代表方法
- 它未必覆盖完整 claim universe
- 它更接近“行为 readiness”，不是“翻译 completeness”

推荐：

- 保留独立的 `Validation Readiness`
- 不把它并入 `Native Completeness`
- 顶层可以给一个独立进度条，但不应该拿它替代整个 DLL 生命周期主进度

### 3.3 DLL 翻译验证的整个过程，还有哪些没考虑到

推荐把 DLL 从开发到可用于 IL2CPP 的过程定义成 7 个阶段：

1. `Planned`
2. `Modeled`
3. `Native In Progress`
4. `Native Complete`
5. `IL2CPP Usable`
6. `HotUpdate Ready`
7. `Release Ready`

其中：

- `Native Complete`
  - 回答：翻译闭合没有
- `IL2CPP Usable`
  - 回答：能不能真正被 IL2CPP 使用
- `HotUpdate Ready`
  - 回答：如果要求热更，热更能力是否闭合

当前还没有被正式考虑清楚的点：

#### A. IL2CPP Usable 的最低闸门

目前缺少明确规则：

- 是不是必须要求 subject validation
- 只要 native + runtime smoke 就算 usable，还是要更强验证

#### B. Subject Validation 的 authority 深度

当前还没有稳定回答：

- 只统计 test project 存在
- 统计代表方法
- 还是必须精确到 `methodSubjectId -> test case`

#### C. HotUpdate 的顶层展示策略

当前还没有稳定回答：

- 所有 DLL 固定显示 `HotUpdate`
- 还是只对命中 `hotupdateImpact` 的 DLL 显示

#### D. Project Evidence 与生命周期的关系

当前 `projects[]` 太强，容易把 dashboard 拉回“工程执行视角”而不是“可用性视角”。

结论：

- `projectSummary` 应保留，但降级为辅助信息
- lifecycle / readiness 才应成为主轴

## 4. 推荐 dashboard 方案

### 方案 A：双轴视图

顶层只看：

- `Native Completeness`
- `HotUpdate Readiness`

优点：

- 简单
- 技术上好实现

缺点：

- 看不出开发阶段
- 无法直接回答“能不能真正用于 IL2CPP”

### 方案 B：四泳道视图

顶层看：

- `Capability Closure`
- `Native Proof`
- `Subject Validation`
- `HotUpdate`

优点：

- 最真实
- 技术信息完整

缺点：

- 主表会很重
- 不适合快速决策

### 方案 C：生命周期主视图 + 详情泳道

顶层只看：

- `DLL Stage`
- `IL2CPP Usable`
- `Native Completeness`
- `HotUpdate Readiness`
- `Validation Readiness`
- `Main Blocker`
- `Next Action`

详情页再展开：

- `Route Gates`
- `Capability Closure`
- `Native Method Details`
- `Subject Validation`
- `Evidence & Blockers`

优点：

- 最符合“开发到能真正用于 IL2CPP”的问题
- 顶层简单，详情完整
- 生命周期、可用性、proof、验证各司其职

缺点：

- 需要重构现有 projection schema

### 推荐

推荐 `方案 C`。

## 5. 推荐的顶层对象

对每个 DLL，建议新增或重构为这 5 个主对象：

- `dllStage`
- `il2cppUsability`
- `nativeCompleteness`
- `hotupdateReadiness`
- `validationReadiness`

辅助对象：

- `projectSummary`
- `mainBlocker`
- `nextAction`

## 6. 建议的顶层表格

| DLL | Stage | IL2CPP Usable | Native | HotUpdate | Validation | Main Blocker | Next Action |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `System.Private.CoreLib` | `IL2CPP Usable` | `Ready` | `3/24 families` | `N/A` | `3/24 families` | `full DLL native closure incomplete` | `continue widening required families` |
| `System.Collections.Immutable` | `Modeled` | `Blocked` | `0/10 families` | `N/A` | `0/10 families` | `native proof not started` | `start immutable-array native lane` |
| `System.Text.Json` | `Native In Progress` | `Blocked` | `5/13 families` | `Proof Required` | `2/13 families` | `hotupdate proof missing` | `add json hotupdate proof` |

## 7. 当前 blocking questions

### Q1

`IL2CPP Usable` 的最低正式闸门是否必须要求 `Subject Validation` 至少达到 family 级闭合？

推荐默认：

- `managed-proof`
- `native-proof`
- 至少一层 `runtime / subject validation`
- 没有 codegen/runtime blocker

### Q2

`HotUpdate Readiness` 是否在顶层固定显示？

推荐默认：

- 顶层固定显示
- 值允许为 `N/A`

原因：

- 这样全表结构最稳定
- 也能一眼区分“没要求”和“没做好”

### Q3

`Subject Validation` 是否允许先以弱 authority 落地？

推荐默认：

- 第一阶段允许弱 authority：
  - test project
  - representative validated methods
- 第二阶段再补强到：
  - `methodSubjectId -> test case`

原因：

- 否则整个 dashboard 重构会被 authority 缺口卡住
- 但必须明确标注 `validationAuthority = weak/strong`

## 8. 下一步分流条件

只有以下 3 个问题全部拍板后，才进入正式开发计划：

1. `IL2CPP Usable` 最低闸门
2. `HotUpdate Readiness` 顶层展示策略
3. `Subject Validation` 第一阶段 authority 深度

在这之前，不应该直接写 implementation plan。
