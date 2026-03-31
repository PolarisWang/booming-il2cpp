# Roadmap 0 详细规划发现记录

## 发现 1：当前仓库没有任何前端、运行时或后端实现代码，Roadmap 0 必须优先冻结边界而不是提前写模块

### 触发场景

为 `Roadmap 0` 制定详细计划时，需要确认当前仓库究竟是“已有半成品代码待整理”，还是“几乎纯文档仓库”。

### 证据

- 仓库顶层只有 `.booming`、`.codex`、`docs`、`.gitignore`、`README.md`
- `README.md` 当前只有仓库标题
- 现有核心内容都位于 `docs/booming/...` 的规划与分析文档中

### 影响范围

如果一开始就按“补模块实现”拆任务，会把不同契约面混在一起，导致后续 Roadmap 1-4 反复返工。

### 当前结论

`Roadmap 0` 应先交付：

- 仓库骨架
- 契约文档
- schema
- ABI/header
- smoke 输入
- 本地验证入口

而不应试图提前实现 Typed IL IR、AOT 分析或 Runtime Core 真实语义。

### 复用价值

后续任何“先做哪个阶段”的讨论，都可以把这条发现作为顺序基线。

## 发现 2：Roadmap 0 的最高优先级是为 `AOT manifest`、`Runtime-Contract IR`、内部 ABI 建立稳定边界

### 触发场景

制定详细任务时，需要决定哪些契约最该先冻结。

### 证据

- 既有 follow-up 讨论明确指出最适合优先细化的切入点是：
  - `AOT manifest`
  - `Runtime-Contract IR`
  - 内部 ABI

### 影响范围

这决定了 `Roadmap 0` 里 schema 与 header 的优先级：必须优先覆盖这些产物，而不是先做外围脚手架。

### 当前结论

`Roadmap 0` 计划中应把以下内容作为核心交付：

- `aot-manifest.schema.json`
- runtime ABI v0 header/doc
- codegen bridge v0 header/doc
- 示例 registration 产物与契约验证

### 复用价值

后续拆 Roadmap 1、Roadmap 2、Roadmap 3 的详细计划时，可以直接沿用这条优先级判断。

## 发现 3：Unity IL2CPP 的真正启发点是“注册 + 元数据缓存 + codegen helper”三件套，而不是某个单独的转换算法

### 触发场景

为 `Roadmap 0` 设计文件和任务结构时，需要从 Unity IL2CPP 分析中提炼最该复用的模式。

### 证据

- `Runtime::Init()` 先接入 `g_CodegenRegistration()`，再做 `MetadataCache::Initialize()`
- generated C++ 通过 `il2cpp_codegen_*` helper 调运行时
- 运行时元数据初始化明确区分可先初始化和依赖 GC 的后初始化

### 影响范围

说明 `Roadmap 0` 不能只写抽象 ABI 文档，还必须把：

- code registration
- metadata registration
- metadata cache 生命周期
- codegen helper surface

作为正式契约一并规划进去。

### 当前结论

Roadmap 0 的详细计划必须同时覆盖 schema、runtime ABI 和 codegen bridge，三者缺一不可。

### 复用价值

这条发现会直接影响后续 Runtime Core 和 C++ 发射器的接口设计。

## 发现 4：Android/iOS 在 Roadmap 0 中最合理的目标是“配置/编译/启动 smoke 骨架”，而不是完整语义对齐

### 触发场景

用户要求把 Android/iOS 纳入路线图验证，详细规划时需要明确它们在 `Roadmap 0` 的层级。

### 证据

- 现有路线图已经把 Android/iOS 纳入平台矩阵，但说明前期允许先做编译/打包/smoke 级验证
- 当前仓库尚无运行时和生成器实现，不具备在移动端做完整行为验证的前提

### 影响范围

`Roadmap 0` 的移动端任务应该落在：

- toolchain 描述
- host harness 形态定义
- compile/link smoke 入口

而不是提前要求 emulator/device 上的深度语义用例。

### 当前结论

详细计划中应为 Android/iOS 单独安排：

- toolchain/preset 文件
- smoke harness 目录
- 最小启动样例
- 平台验证矩阵文档

### 复用价值

可以避免后续把移动端集成成本错误地挤进 Roadmap 4 或 Roadmap 5 的末尾。
