# Productization Gates Brainstorm v1.01

## 背景

- 父 roadmap 的唯一硬阻塞已经回到 Phase 4 mobile runtime host 外部环境。
- `20260411-09-productization-gates` 作为 Phase 8 任务，原计划依赖全部前置阶段完成，但其中部分子项并不依赖真实移动端运行证据。

## 可选切入点

### 方案 A：等待 Phase 4 环境恢复后再整体执行 Phase 8

- 优点：严格按 roadmap 依赖顺序推进。
- 缺点：本地可做的产品化准备全部停滞，无法继续消化剩余工作。

### 方案 B：先做 Batch 1 compatibility matrix

- 优点：更接近 Phase 8 主体。
- 缺点：需要同时补 subject、runner、报告格式，范围偏大，且容易和 rollback / package version 规则耦合。

### 方案 C：先做 Batch 3 版本兼容与 iOS 发布边界

- 优点：不依赖真实移动端环境；可以直接沉淀文档与 `PackageValidator` 规则；实现面小，适合在当前主线阻塞期间继续推进。
- 缺点：只能完成 Phase 8 的局部，不会解除主任务总阻塞。

## 结论

- 选择方案 C。
- 本轮范围限定为：
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/ios-distribution-policy.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/version-compatibility-matrix.md`
  - `PackageValidator` 的版本兼容检查
  - 对应的定向测试
- rollback、compatibility matrix runner、soak test 继续留在后续批次。
