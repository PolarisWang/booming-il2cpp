# docs/ 文档索引

> 项目文档根目录。按文档生命周期组织：活跃开发记录（`dev/`）、历史归档（`archive/`）、验证报告（`verification/`）。
> 长期权威知识见 [`wiki/INDEX.md`](../wiki/INDEX.md)。

## 目录结构

| 目录 | 内容 | 状态 |
|------|------|------|
| [`dev/`](dev/INDEX.md) | 活跃开发与设计（designs / tests / assessments / in-progress / hanging / scripts） | 活跃 |
| [`verification/`](verification/INDEX.md) | 验证报告入口（生成物已 ignore，保留手写 md） | 生成 |
| [`archive/`](archive/INDEX.md) | 历史归档（dev-completed / discuss / optimize / reports / assessments / weekly） | 只读 |
| [`architecture/`](architecture/INDEX.md) | 架构参考 redirect（权威在 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/`） | 稳定 |

## dev/ 内部结构

| 子目录 | 内容 |
|--------|------|
| `designs/` | 当前活跃设计方案（原 `docs/design/`）|
| `tests/` | 测试架构文档 |
| `assessments/` | 能力评估 / 限制说明（memory / roadmap / t4 / gap-analysis）|
| `in-progress/` | 进行中开发阶段 |
| `hanging/` | 挂起开发阶段 |
| `scripts/` | 活跃验证/工具脚本 |

> 2026-08-04 整理：`design`、`tests`、`gap-analysis.md` 已并入 `dev/`；历史类已归档至 `docs/archive/`。

## 文档生命周期

```text
dev/designs (设计方案) → dev/in-progress/ (实施中) → dev/hanging/ 或 archive/dev-completed/ (归档)
架构权威见 wiki/03-功能模块/06-il2cpp核心架构
```
