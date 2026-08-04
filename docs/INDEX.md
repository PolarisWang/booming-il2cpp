# docs/ 文档索引

> 项目文档根目录。按文档生命周期组织：长期有效的架构设计、进行中的开发记录、已完成阶段的归档。

## 目录结构

| 目录 | 内容 | 状态 |
|------|------|------|
| [`architecture/`](architecture/INDEX.md) | 架构参考 redirect（权威在 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/`） | 稳定 |
| [`design/`](design/) | 当前进行中的设计方案 | 活跃 |
| [`dev/`](dev/INDEX.md) | 开发阶段记录（in-progress / hanging；completed 已归档） | 活跃 |
| [`verification/`](verification/INDEX.md) | 验证报告入口（生成物已 ignore，保留手写 md） | 生成 |
| [`archive/`](archive/INDEX.md) | 历史归档（dev-completed / discuss / optimize / reports / assessments / weekly） | 只读 |

> 原 `docs/discuss`、`docs/optimize`、`docs/reports`、`docs/assessments`、`docs/weekly`
> 已于 2026-08-04 归档至 `docs/archive/`（git mv，保历史）。

## 文档生命周期

```text
design/ (当前方案) → dev/in-progress/ (实施中) → archive/dev-completed/ + archive/<历史类> (已完成归档)
discuss/→archive/discuss (讨论) ；架构权威见 wiki/03-功能模块/06-il2cpp核心架构
```
