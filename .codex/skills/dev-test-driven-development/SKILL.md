---
name: test-driven-development
description: 在实现任何功能、修复任何 bug、调整任何长期规则之前使用，先补失败测试，再改实现
---

# 测试驱动开发

## 核心纪律

先写会失败的自动化测试，再写实现。

如果你没有亲眼看到测试先失败，再因为你的改动变绿，就不要声称这次改动是可靠的。

## 使用时机

- 新功能
- bug 修复
- 回归修复
- 重构导致的行为变化
- skill / planner / runner / dashboard / schema 的长期规则调整

## 仓库补充规则

### 1. 以下改动必须先补自动化测试

- IL2CPP compiler / loader / linker / semantic world / codegen
- `Chaos.TestFramework.Sdk` / `Chaos.TestFramework.Runtime`
- UnitTest / Benchmark / HotUpdate collection file schema 或 loader
- managed / native / hotupdate runner 生成逻辑
- benchmark dashboard 数据聚合或展示逻辑
- subject planner、subject discovery、generated solution pipeline

### 2. 测试优先级

优先用下面的顺序沉淀失败用例：

1. `tests/unit/**` 下的 Python 模板化单测
2. `tests/contracts/**` 下的 contract / snapshot / schema 测试
3. `tests/integration/**` 下的端到端流程测试
4. 必要时才新增长期保留的 C# canonical template 或 canonical subject

### 3. Python 测试优先模板化

- 优先复用 `tests/templates/`
- 优先通过临时工作目录实例化最小 managed/native/hotupdate 样例
- 不要为单个 bug 长期保留手写的小型 C# fixture，除非它已经被提升为公共模板或正式 subject

### 4. 手工结果不能替代测试

以下都不能作为唯一验证证据：

- 手工跑 benchmark
- 手工看 dashboard
- 手工看控制台输出
- 手工跑 subject 再肉眼判断

### 5. 对测试框架本身也要用正式断言

- managed 层验证应优先使用 `Chaos.TestFramework.Sdk` 中的 `Assert`
- 不要依赖 `Console.WriteLine` 再在外部脚本里解析成功失败

## 最小循环

1. 写失败测试
2. 运行，确认是因为目标能力缺失而失败
3. 写最小实现
4. 运行，确认目标测试变绿
5. 必要时重构，并保持测试继续变绿

## 红旗

- 先改实现，之后再补测试
- 只补手工脚本，不补自动化用例
- 为了图快直接把逻辑塞进现有大 fixture
- 只修 dashboard 显示，不补 collection 或数据生产端测试
- 只看 benchmark 页面，不验证数据生产链路

## 结束前检查

- 是否先看到了失败测试
- 是否把 bug 沉淀为模板化或 contract 化测试
- 是否避免继续扩大字符串协议面
- 是否没有把测试框架逻辑混进 IL2CPP Core IR / planner / emitter
- 是否保留了清晰的模块边界
