# 03-功能模块 INDEX

> 收录功能本体知识。二级目录固定，具体功能作为三级页面进入。

## 子类别

| 类别 | 说明 | 索引 |
|------|------|------|
| `01-执行系统` | 当前执行上下文、history 归档等执行能力 | [`01-执行系统/INDEX.md`](./01-%E6%89%A7%E8%A1%8C%E7%B3%BB%E7%BB%9F/INDEX.md) |
| `02-Skill工作流` | 项目主线 skill 工作流的功能化知识 | [`02-Skill工作流/INDEX.md`](./02-Skill%E5%B7%A5%E4%BD%9C%E6%B5%81/INDEX.md) |
| `03-文档体系` | `docs` / `wiki` 边界和文档结构知识 | [`03-文档体系/INDEX.md`](./03-%E6%96%87%E6%A1%A3%E4%BD%93%E7%B3%BB/INDEX.md) |
| `04-测试与验证` | skill 行为测试和验证策略 | [`04-测试与验证/INDEX.md`](./04-%E6%B5%8B%E8%AF%95%E4%B8%8E%E9%AA%8C%E8%AF%81/INDEX.md) |
| `05-工具能力` | 项目内工具能力、生成链约束与长期技术方案 | [`05-工具能力/INDEX.md`](./05-%E5%B7%A5%E5%85%B7%E8%83%BD%E5%8A%9B/INDEX.md) |
| `08-GC子系统` | CRAG GC 分配器、收集器、诊断知识 | [`08-GC子系统/INDEX.md`](./08-GC%E5%AD%90%E7%B3%BB%E7%BB%9F/INDEX.md) |
| `06-il2cpp核心架构` | IL2CPP 运行时核心模块 | [`06-il2cpp核心架构/runtime-support-stubs.md`](./06-il2cpp%E6%A0%B8%E5%BF%83%E6%9E%B6%E6%9E%84/runtime-support-stubs.md) |

## 重要文档

| 文档 | 主题 | 说明 |
|------|------|------|
| [`01-执行系统/CURRENT执行上下文机制.md`](./01-%E6%89%A7%E8%A1%8C%E7%B3%BB%E7%BB%9F/CURRENT%E6%89%A7%E8%A1%8C%E4%B8%8A%E4%B8%8B%E6%96%87%E6%9C%BA%E5%88%B6.md) | CURRENT 机制 | 说明恢复执行的结构化上下文。 |
| [`03-文档体系/docs与wiki边界.md`](./03-%E6%96%87%E6%A1%A3%E4%BD%93%E7%B3%BB/docs%E4%B8%8Ewiki%E8%BE%B9%E7%95%8C.md) | 文档边界 | 说明过程文档与长期知识的分流。 |
| [`05-工具能力/SIMD语义降级与多后端IR方案C3.md`](./05-%E5%B7%A5%E5%85%B7%E8%83%BD%E5%8A%9B/SIMD%E8%AF%AD%E4%B9%89%E9%99%8D%E7%BA%A7%E4%B8%8E%E5%A4%9A%E5%90%8E%E7%AB%AFIR%E6%96%B9%E6%A1%88C3.md) | SIMD 语义抽象 | 沉淀 fixed-vector / SIMD 通用抽象、Shape IR、Semantic IR 与多 backend lowering 方案。 |
| [`08-GC子系统/CRAG-GC-Phase3-并发能力.md`](./08-GC%E5%AD%90%E7%B3%BB%E7%BB%9F/CRAG-GC-Phase3-%E5%B9%B6%E5%8F%91%E8%83%BD%E5%8A%9B.md) | Phase 3 并发能力 | Server GC 模式、并发根扫描、NUMA 感知空闲页回收。 |

## 本层规则

- 二级目录固定，不直接在本层堆放具体功能名称。
- 新知识应进入对应二级分类下的页面，并同步更新索引。

## 最近更新

- `2026-04-29`：补充 `05-工具能力/SIMD语义降级与多后端IR方案C3.md`，沉淀更通用的 SIMD 抽象方案。
- `2026-04-02`：建立功能模块固定大类。
