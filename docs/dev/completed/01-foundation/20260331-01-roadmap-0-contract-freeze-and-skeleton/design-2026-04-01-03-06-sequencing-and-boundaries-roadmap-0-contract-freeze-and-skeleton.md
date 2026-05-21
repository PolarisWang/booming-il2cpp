# 第 6 段：阶段切分、建议执行顺序与边界

## 推荐阶段切分

### 第 1 步：骨架与顶层规则

先落下：

- 顶层目录骨架
- ownership / README
- `third_party/` 规则
- `artifacts/` 规则
- host-specific verify 矩阵框架

### 第 2 步：正式 contract 内容

再冻结：

- schema pack
- native contract pack
- trace contract

### 第 3 步：真实 smoke 输入项目

再落：

- 5 个真实 `.csproj`
- 各自的职责映射
- 各自的内部子场景

### 第 4 步：verify-roadmap-0 串联

最后串起：

- Windows host 子矩阵
- macOS host 子矩阵
- Android 真启动
- iOS compile/link/packaging
- Linux cross-compile/packaging
- 双 reference desktop 高阶 contract 验证

## 建议 chunk 结构

如果后续把它转成 implementation plan，建议分成 5 个 chunk：

1. 骨架、第三方规则、artifacts 规则、host-specific verify 矩阵
2. schema pack 与 trace contract
3. native contract pack（ABI / bridge）
4. 5 个 smoke 输入项目与子场景分配
5. `verify-roadmap-0` 与平台 gate 串联

## 与后续 roadmap 的边界

尽管本设计已经很重，`Roadmap 0` 仍然不应越界到：

- 完整 runtime core 行为实现
- 完整 codegen lowering
- 全平台等强度语义实现
- 完整移动端 runtime 语义

更准确地说，`Roadmap 0` 是：

- 重规范
- 重验证入口
- 重 smoke / example / snapshot
- 重平台 gate

但仍然不是：

- `Roadmap 3` 的真实 runtime 落地
- `Roadmap 4` 的完整 codegen 与平台打通

## 最终判断

如果按本设计继续推进，`Roadmap 0` 将成为一个偏重的 contract freeze 阶段：

- 它的资产完整度很高
- 它的验证门槛很高
- 它对后续 roadmap 的边界约束也很强

但只要继续坚持“冻结 contract，不偷跑完整实现”的原则，它仍然可以保持在 `Roadmap 0` 的语义内，而不是提前变成后续 roadmap 的半实现版。
