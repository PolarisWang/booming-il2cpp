# docs/archive/ 归档索引

> 已完成或废弃的历史文档。只读区域，不再更新。
>
> **约定**：已完成的正式任务输出统一归档到此（多为一次性评审/进度/证明产物）。
> `docs/archive/` 不是活性知识源——当前架构与决策以 `wiki/` 与 `docs/dev/` 为准。

## 归档内容

| 目录 | 内容 | 文件数 |
|------|------|--------|
| [`dev-completed/`](dev-completed/) | 已完成的开发任务正式产出（含校验 IR/证明 JSON） | ~1482 |
| [`discuss/`](discuss/) | 设计讨论与决策记录 | ~98 |
| [`abandoned-designs/`](abandoned-designs/INDEX.md) | 废弃的设计方案与 roadmap | ~53 |
| [`optimize/`](optimize/) | 性能优化过程记录 | ~34 |
| [`architecture/`](architecture/) | 从 `architecture/` 移入的旧版阶段性子文档 | ~24 |
| [`reports/`](reports/) | 阶段报告（评审/验证） | ~4 |
| [`assessments/`](assessments/) | 评估记录 | ~2 |
| [`2026-05-30-convert-char-review/`](2026-05-30-convert-char-review/) | 字符转换评审 | 1 |
| [`weekly/`](weekly/) | 周报 | 1 |

## 体积说明

- 全目录共 **~36 MB**，主体在 `dev-completed/12-rebirth-corelib/`（~25 MB 的语义 IR / 表面清单 `-v1-01.json` 校验数据）。
- 这些是已完成的 corelib 认证产物，作为归档证明保留；若后续磁盘告急可评估移入 Git LFS，但**不建议**为此重写 git 历史。

## 维护

- 新归档：完成任务后移入对应子目录，并在此表追加一行。
- 不更新：归档内容视为冻结，不再修订。
