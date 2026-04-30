# SemanticWorld 与 Linker 边界决策

**来源**: `docs/discuss/il2cpp-mainline/20260407-04-*`, `20260408-01-*`, `20260408-02-*`, `20260408-21-*`, `20260408-24-*`, `20260408-25-*`

## 决策

SemanticWorld 与 Linker 的职责边界按以下划分。

### SemanticWorld 6 块输出

| 输出块 | 说明 |
|--------|------|
| Canonical semantic objects | 语义归一化后的类型、方法、字段对象 |
| Operation stream | 方法体的标准化操作流 |
| Capability hints | 类型/方法的能力描述（值大小、向量宽度等） |
| CanonicalSubjects | 跨程序集引用的规范主题标识 |
| StableMetadataLookup | 稳定的元数据查找边界（不随裁减变化） |
| Diagnostics | 语义分析诊断信息 |

### CanonicalSubjects

跨程序集引用和类型转发的统一标识模型：
- 不依赖具体程序集版本的规范名称解析
- 类型转发（TypeForwardedTo）在 SemanticWorld 层解析
- 为 Linker 提供稳定的引用标识

### Linker Output Contract

Linker 输出的 retained world 包含：
- 保留的类型集合及其依赖理由（reason taxonomy）
- 保留的方法集合
- 保留的字段集合
- 外部 managed 依赖列表（引用的外部程序集成员）
- 每条依赖附 reason（为什么被保留）

### StableMetadataLookup

SemanticWorld 层建立的元数据查找接口，Linker、CodeGen 都通过此接口查询元数据：
- 屏蔽 Loader 层的具体实现
- 不随 Linker 裁减变化
- 保证 CodeGen 在任意阶段都能查询原始元数据

## 关键约束

- SemanticWorld 不做 retained/dependency 决策
- Linker 不承担 lowering 级决策
- CanonicalSubjects 在 SemanticWorld 层完全解析
