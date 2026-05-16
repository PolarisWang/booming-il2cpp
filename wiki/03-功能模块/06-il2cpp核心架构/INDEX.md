# 06-il2cpp核心架构 INDEX

> **Chaos IL2CPP** 完整的架构知识体系。AI 在开发 il2cpp 功能前应查阅此目录。
>
> 阅读路径：先读 `01-翻译管线/` 建立全局理解 → 开发时查 `02-翻译路径参考/` 找到对应翻译策略 → 确认 `03-架构约束/` 中的硬性规则 → 需要时回溯 `04-历史决策/` 理解方案来源。

## 目录导航

| 分类 | 说明 | 入口 |
|------|------|------|
| `01-翻译管线` | 整体流程、输入合约、求值栈、ABI、NativeAotLoweringPlanner、异常处理、运行时生态 | [`01-翻译管线/INDEX.md`](./01-%E7%BF%BB%E8%AF%91%E7%AE%A1%E7%BA%BF/INDEX.md) |
| `02-翻译路径参考` | 按 IL 特性分类的翻译路径表，包括 opcode、异常 shape、委托、helper | [`02-翻译路径参考/INDEX.md`](./02-%E7%BF%BB%E8%AF%91%E8%B7%AF%E5%BE%84%E5%8F%82%E8%80%83/INDEX.md) |
| `03-架构约束` | 翻译层硬性约束、禁止模式、C++ 约定、合约边界 | [`03-架构约束/INDEX.md`](./03-%E6%9E%B6%E6%9E%84%E7%BA%A6%E6%9D%9F/INDEX.md) |
| `04-历史决策` | 设计讨论中提炼的最终决策结论 | [`04-历史决策/INDEX.md`](./04-%E5%8E%86%E5%8F%B2%E5%86%B3%E7%AD%96/INDEX.md) |
| `05-patchdata工具链` | .patchdata 二进制格式定义、CLI 命令、调试工作流 | [`05-patchdata工具链.md`](./05-patchdata%E5%B7%A5%E5%85%B7%E9%93%BE.md) |
| `20-threading-optimizations` | Threading 子系统深度优化（ThreadPool 工作窃取、Hill Climbing、WaitHandle O(1)、Monitor 自适应自旋、SyncBlock 池、iOS 热更适配） | [`20-threading-optimizations.md`](./20-threading-optimizations.md) |
| `21-interpreter-optimizations` | Interpreter 子系统深度优化（FastFrame 池深度、每指令检查开销降级、Handle_Call 拆分、CallVirt Monomorphic Inline Cache） | [`21-interpreter-optimizations.md`](./21-interpreter-optimizations.md) |

## 本层规则

- **全局优先级约束（覆盖全流程）**：性能最优(1) > 方案完美性(2) > HotUpdate支持(3)。详见 [`03-架构约束/05-全局优先级约束.md`](./03-%E6%9E%B6%E6%9E%84%E7%BA%A6%E6%9D%9F/05-%E5%85%A8%E5%B1%80%E4%BC%98%E5%85%88%E7%BA%A7%E7%BA%A6%E6%9D%9F.md)
- **AI 开发前置查询路径**：`01-翻译管线` → `02-翻译路径参考` → `03-架构约束`，三步完成后才能开始实现
- 新增翻译路径必须同步更新 `02-翻译路径参考/` 对应文件
- 新约束必须写入 `03-架构约束/` 对应文件
- 历史决策只记录最终方案，不保留讨论过程
- 知识以层级索引结构组织：INDEX.md 只做导航和摘要，具体内容在叶子文件中

## 文档层级约定

```
INDEX.md                    -- 入口导航，说明目录用途和阅读路径
  01-子主题/INDEX.md         -- 子主题导航，列出该主题下的所有文档
  01-子主题/01-具体文件.md     -- 具体知识内容
  01-子主题/02-具体文件.md
  02-子主题/INDEX.md
  ...
```

每个子主题目录的 INDEX.md 必须：
- 说明该子主题的用途和阅读顺序
- 列出所有叶子文档及其一句话说明
- 叶子文档只写该层级范围内的内容，不跨层引用不重要细节

## 最近更新

- `2026-04-29`：重构 il2cpp 核心架构为层级索引结构，按翻译管线/翻译路径/约束/决策分类
