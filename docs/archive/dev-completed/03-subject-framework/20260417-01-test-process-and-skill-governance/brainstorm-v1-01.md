# 测试流程与 TestFramework Sdk 化 Brainstorm v1.02

Date: 2026-04-17
Status: concluded

## 1. 用户目标

- 规范整个项目的测试流程
- managed project 只直接引用一个测试 DLL
- collection file 统一由 `.NET 8` collector 产出
- managed/native/hotupdate 三端统一消费 collection file
- 清理旧写法，保持项目干净

## 2. 关键纠偏

### 2.1 `Assert` 不应留在 `Runtime`

如果 subject / managed project 只直接引用一个测试 DLL，那么断言能力也必须在这个 DLL 中，否则 subject 代码无法独立编写测试。

因此：

- `Assert` 属于 subject-side authoring surface
- `Assert` 应放入 `Chaos.TestFramework.Sdk`

### 2.2 `Annotation` 命名过窄

当同一个程序集同时承载：

- attributes
- enums
- contracts
- assert

它已经不是纯 `Annotation` 概念，而是 subject-side test SDK。

因此：

- `Chaos.TestFramework.Annotation` 改名为 `Chaos.TestFramework.Sdk`

### 2.3 native / hotupdate 不应直接污染 collection contract

collection file 应只描述稳定测试声明。

执行形态特有细节应单独分层：

- native：dispatch manifest
- hotupdate：binding manifest

## 3. 方案比较

### 方案 A：`Sdk` 承担 subject-side authoring API，`Runtime` 承担执行宿主

- 优点：
  - 符合“managed project 只直接引用一个 DLL”
  - subject 边界清晰
  - collector、runtime、native、hotupdate 都容易围绕稳定 authoring contract 构建
- 缺点：
  - 需要一次性改名和迁移断言位置

### 方案 B：保留 `Annotation + Runtime`，再新增 assertions 程序集

- 优点：
  - 概念更细
- 缺点：
  - managed project 不再是单 DLL 依赖
  - 破坏用户明确目标

结论：采用方案 A。

## 4. native / hotupdate 建议

### native

- subject 代码只引用 `Sdk`
- native test project 采用薄宿主
- 输入：
  - collection file
  - native dispatch manifest

### hotupdate

- patch project 只引用 `Sdk`
- hotupdate test host project 引用 `Sdk + Runtime`
- 输入：
  - collection file
  - hotupdate binding manifest

## 5. 清理原则

- 不保留长期 `Annotation` compatibility alias
- 不保留旧 runner / 旧 registry / 旧 discovery 双轨
- 不保留已被模板替代的旧 fixture
- 不保留旧文档中的错误边界描述

## 6. 分流结论

该任务需要：

- 多阶段推进
- 拆分多个相对独立子任务
- 涉及长期命名切换与清理工作

因此从 `plan` 升级为 `roadmap`。
