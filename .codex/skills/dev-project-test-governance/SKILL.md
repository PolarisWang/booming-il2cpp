---
name: project-test-governance
description: Use when changing subject/test workflow, Chaos.TestFramework, generated managed/native/hotupdate runners, benchmark or unit verification flow, or project codegen structure in this repository
---

# 项目测试治理

## 概览

这是本仓库针对 subject 测试主线、`Chaos.TestFramework.Sdk/Runtime` 分层、collection file、manifest、Python 模板化测试和 Scriban codegen 的项目级治理技能。

核心原则：

- managed solution 是唯一测试源
- managed project 只直接引用 `Chaos.TestFramework.Sdk`
- collection file 是唯一跨执行形态契约
- collection file 统一由 `.NET 8` collector 分析 managed project 中的 `Sdk` 声明后产出
- managed/native/hotupdate test host 都只消费 collection file
- native / hotupdate 的执行绑定通过 manifest 分层
- bugfix 必须先补自动测试
- file-level codegen 默认 Scriban
- 迁移完成后删除旧命名和旧双轨逻辑

## 文档语言要求

除非用户明确要求其他语言，所有由本技能产出的设计文档、计划、roadmap、wiki、规范说明默认使用中文。代码、命令、路径、标识符保持原文。

## 使用时机

以下任务必须使用本技能：

- 调整 `subjects/` 的测试入口、runner、registry 或统一入口
- 修改 `Chaos.TestFramework`
- 修改 managed/native/hotupdate 测试主线
- 修改 UnitTest / Benchmark / HotUpdate collection 的生成或消费逻辑
- 修改 native dispatch manifest 或 hotupdate binding manifest
- 修改 benchmark 页面、测试报告与 collection 驱动关系
- 修改生成整文件的 test/codegen emitter
- 处理“修了 bug 但还没有正式自动测试资产”的情况

## 强制规则

### 1. 先有自动测试，再动实现

本仓库中，以下 bugfix 不允许只靠手工复跑或看 dashboard：

- IL2CPP compiler / loader / linker / semantic world / codegen bugfix
- `Chaos.TestFramework.Sdk` / `Runtime` 行为修复
- `.NET 8` collector 修复
- collection file schema 或加载逻辑修复
- manifest 生成逻辑修复
- runner / host 生成逻辑修复
- benchmark 数据生成与展示修复

优先证据顺序：

1. `tests/unit/**` 的 Python 模板化单测
2. `tests/contracts/**` 的 contract / snapshot 测试
3. `tests/integration/**` 的端到端测试
4. subject 级实跑

### 2. `Sdk` 与 `Runtime` 分层

`Sdk` 只负责：

- attributes
- enums
- stable contracts
- `Assert`
- `ChaosAssertionException`
- 最小 assertion state / subject authoring helper

`Runtime` 只负责：

- collection loader
- benchmark harness
- execution sink
- reporting / result state
- managed / hotupdate test host

`.NET 8` collector 属于测试收集/生成链路，不属于 subject 引用程序集。

### 3. managed solution -> dotnet 8 collection analysis -> test hosts 是唯一主线

统一流程固定为：

`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`

禁止：

- subject 直接引用 `Runtime`
- 手工注册测试列表
- managed/native/hotupdate 各自维护不同入口清单
- 三端各自重新扫描 `Chaos.TestFramework.Sdk`
- 让 dashboard 或临时脚本替代正式 collection contract

### 4. collection file 必须 versioned 且 enum-first

collection schema 要求：

- versioned
- 分类字段优先枚举值
- 仅在不可避免的标识符、alias、路径上使用字符串
- managed/native/hotupdate 消费同一 schema

### 5. execution binding 必须分层到 manifest

- native 使用 dispatch manifest
- hotupdate 使用 binding manifest
- 不允许把 native/hotupdate 专有绑定写进 collection file

### 6. Python 测试必须优先模板化

优先使用：

- `tests/templates/` 下的最小模板
- 临时工作目录参数化生成

避免：

- 为单个 bug 长期保留完整手写 C# 小工程
- 在 `tests/fixtures/` 持续堆积一次性样例

### 7. file-level codegen 默认 Scriban

以下场景默认必须使用 Scriban：

- managed test project 生成
- native test project 生成
- hotupdate test host project 生成
- manifest 生成的整文件
- IL2CPP 生成完整 C# / C++ 文件
- `NativeAotEmitter` 的文件级输出

### 8. 不允许继续生长单文件巨石与兼容垃圾

禁止把以下职责混在同一个文件：

- `Sdk` attributes / assertions
- `Runtime` execution
- collection schema / loader
- collector
- manifest binding
- IL2CPP planner
- IL2CPP emitter
- runtime support helper
- hotupdate verification

禁止长期保留：

- `Chaos.TestFramework.Annotation`
- compatibility alias
- 旧 runner / registry / fixture / 文档双轨

## 推荐路径

```text
src/reference/
  Chaos.TestFramework.Sdk/
  Chaos.TestFramework.Runtime/

src/tools/
  Chaos.TestFramework.CollectionGen/

tests/
  templates/

src/managed/Chaos.IL2CPP.CodeGen/
  Planner/
  Emitter/
  RuntimeSupport/
  Templates/
```

## 输出要求

如果本次任务改变了长期规则，必须同时更新：

- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/04-工具与集成/统一测试框架.md`
- `wiki/06-测试验证/新增测试接入规范.md`
- `wiki/06-测试验证/INDEX.md`
- `wiki/02-Skill体系/04-质量保障/project-test-governance.md`

## 关联技能

- 上游：`brainstorming`、`systematic-debugging`
- 协作：`test-driven-development`、`project-wiki-maintenance`
- 下游：`roadmap`、`writing-plans`、`executing-plans`
