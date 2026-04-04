# 第 1 段：范围、交付物与验证总原则

## 目标定位

`Roadmap 0` 不是“轻脚手架阶段”，而是一次性冻结偏重的 contract-and-skeleton 基线。
它同时承担：

- 顶层仓库骨架落盘
- `schema pack` 冻结
- 公共 `native contract pack` 冻结
- 5 个真实 smoke 输入项目落盘
- host-specific verify 矩阵接入
- trace contract 与强验证入口建立

## 正式交付物

`Roadmap 0` 的正式交付物分为四层：

1. 源码树中的正式 contract 资产
2. 验证 contract 与 smoke 资产
3. 平台矩阵与 `verify-roadmap-0` gate
4. 顶层骨架、第三方依赖规则与产物目录规则

## 总原则

### 1. 本阶段可以冻结重 contract，但不偷跑完整实现

允许在 `Roadmap 0` 中冻结：

- 高阶 ABI/bridge contract
- 高阶反射/泛型语义承诺
- 偏重的验证资产
- 偏重的平台 gate

不要求在 `Roadmap 0` 中完成：

- 完整 runtime core
- 完整 codegen lowering
- 完整移动端语义实现

### 2. 验证要分平台矩阵，而不是单机同构

`verify-roadmap-0` 是全量硬 gate，但按 host OS 分子矩阵执行：

- Windows 开发机：`Windows + Android + Linux(cross-compile/packaging)`
- macOS 开发机：`macOS + iOS + Linux(cross-compile/packaging)`

### 3. 双 reference desktop 承担高阶 contract 强验证

`Windows x64` 与 `macOS` 同时作为 reference desktop。

- `Windows` 承担高阶 ABI/bridge contract 的主线基准集
- `macOS` 承担接近同范围的高阶子集
- 仅对真正平台能力相关的 `icall` 留白

### 4. 平台能力相关 `icall` 仍纳入 Roadmap 0，但只做到 basic-path

允许留白的能力集中在：

- 文件系统
- 时间 / 时区
- 动态库装载
- socket / network

这些能力在 `Roadmap 0` 中要求做到 query/basic-path 级验证，而不是完整平台语义打满。

## 非目标

以下内容虽然会被后续 roadmap 正式实现，但不要求在 `Roadmap 0` 中完成真实落地：

- 完整运行时行为
- 完整端到端 codegen backend
- 全平台等强度语义验证
- 所有动态特性与平台能力的完整实现
