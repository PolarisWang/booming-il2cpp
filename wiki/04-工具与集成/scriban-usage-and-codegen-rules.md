# Scriban 使用说明与 Codegen 规范

## 1. 目的

本页固化仓库内 `Scriban` 的正式使用方式，约束：

- IL2CPP 核心代码生成如何优先通过 `Scriban` 落地产物
- Python 侧代码生成如何遵循同一套模板优先规则
- 当现有模板能力不足时，应该先扩展 `Scriban`，而不是回退到长期手写字符串拼接

本页记录长期有效规则，不记录单次任务过程。

## 2. 适用范围

本规范适用于以下场景：

- `src/managed/Chaos.IL2CPP.CodeGen/` 中的 managed / native codegen
- managed test host、native test host、hotupdate host 等结构化文本产物生成
- manifest、配置文件、桥接代码、生成型 `.g.cs` / `.cpp` / `.h` / `.json`
- Python 侧新增或重构的代码生成、样例生成、测试模板生成

## 3. 正式入口

当前仓库内与 `Scriban` 相关的正式入口包括：

- `src/managed/Chaos.IL2CPP.CodeGen/Templating/ScribanTemplateRenderer.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/`
- `src/managed/Chaos.IL2CPP.CodeGen/ReferenceProof/NativeReferenceProofCatalog.cs`
- 各类 planner / emitter 对 template path 与 render model 的映射

结论：

- `Scriban` 不是临时外部工具，而是 `IL2CPP` 核心 codegen 的正式组成部分。
- 模板文件、模板模型、模板函数、模板渲染基础设施都属于核心实现。

## 4. IL2CPP 核心 Codegen 使用说明

### 4.1 基本流程

新增或重构 IL2CPP 结构化文本产物时，默认流程固定为：

1. `Planner` 负责语义分析、ABI / dispatch / EH / runtime-support 规划，生成 typed plan。
2. `Emitter` 只消费 plan，不重新做业务决策。
3. `Emitter` 选择 `Scriban` 模板路径与 render model。
4. 通过统一渲染入口加载模板、渲染字符串并写出最终产物。

### 4.2 必须走 Scriban 的产物

以下产物新增或重构时，默认必须优先走 `Scriban`：

- `.cpp`
- `.h`
- `.g.cs`
- `.json`
- manifest
- 其他结构化文本产物

### 4.3 禁止事项

以下做法不符合正式规范：

- 因为模板写起来不方便，就新增长期 `StringBuilder.AppendLine(...)` 发射路径
- 在 emitter 中重新做 planner 已经确定的 ABI / dispatch / EH 决策
- 在模板中硬编码语义级 lowering 规则
- 为单个 feature 临时复制一套平行模板渲染入口

## 5. Python Codegen 使用说明

### 5.1 正式规则

Python 侧新增或重构代码生成时，同样必须优先使用 `Scriban`：

- 优先消费已有的 `Scriban` 模板与正式生成入口
- 如果需要新增模板，应优先新增 `.scriban` 模板，而不是继续扩散自定义 `.tmpl`、`f-string`、`format()` 或大段字符串拼接
- 存量非 `Scriban` 模板路径视为 legacy，可继续维护，但不得作为新方案继续扩散

### 5.2 推荐姿势

Python 侧生成结构化文本时，优先顺序固定为：

1. 复用现有 `Chaos.IL2CPP.CodeGen` 的 `Scriban` 模板与 contract
2. 如确需 Python 直接生成，则消费同一份 `Scriban` 模板真相源
3. 只有产物不是结构化文本时，才允许不走模板

### 5.3 禁止事项

以下做法不符合正式规范：

- 新增 Python 手写字符串拼接生成 `.cs` / `.cpp` / `.h` / `.json`
- Python 和 C# 侧各自维护两套模板真相源
- 因为现有模板能力不足，就把生成逻辑回退成脚本内硬编码

## 6. 能力不足时的扩展顺序

当现有 `Scriban` 模板无法表达目标产物时，固定按以下顺序扩展：

1. 补模板模型字段
2. 补 template function / filter / adapter
3. 补模板加载、catalog、renderer、渲染辅助设施
4. 仍然不足时，优先扩展 vendored `Scriban` 的语法或仓库内 `Scriban` 支撑层
5. 只有当产物明确不是结构化文本时，才允许走非模板路径

约束：

- 不允许因为“先做出来更快”而绕开上述顺序。
- 任何例外都必须是显式、短期、可收回的，而不是形成新的长期主线。

## 7. 评审检查表

涉及 codegen 的变更在评审时至少回答以下问题：

1. 产物是否属于结构化文本；如果是，是否已经优先走 `Scriban`？
2. planner、emitter、template 的职责是否仍然分离？
3. Python 侧是否复用了同一份 `Scriban` 模板真相源，而不是复制生成逻辑？
4. 若功能不满足，是否先扩展了 `Scriban`，而不是新增手写拼串？
5. 是否留下了新的 alias、双轨生成路径或不可回收的 legacy 模板？

## 8. 相关文档

- [`il2cpp-core-structure-and-scriban-governance.md`](./il2cpp-core-structure-and-scriban-governance.md)
- [`../../docs/architecture/subject-test-framework-v1/INDEX.md`](../../docs/architecture/subject-test-framework-v1/INDEX.md)
- [`../../docs/architecture/verification-v1/spec.md`](../../docs/architecture/verification-v1/spec.md)
- [`../06-测试验证/AOT新Feature接入自测规范.md`](../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/AOT%E6%96%B0Feature%E6%8E%A5%E5%85%A5%E8%87%AA%E6%B5%8B%E8%A7%84%E8%8C%83.md)
