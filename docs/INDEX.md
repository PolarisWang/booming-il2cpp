# docs/ 文档索引

> 项目文档根目录。按文档生命周期组织：长期有效的架构设计、进行中的开发记录、已完成阶段的归档。

## 目录结构

| 目录 | 内容 | 状态 |
|------|------|------|
| [`architecture/`](architecture/INDEX.md) | 长期有效的架构决策与规范（evergreen） | 稳定 |
| [`design/`](design/) | 当前进行中的设计方案 | 活跃 |
| [`dev/`](dev/INDEX.md) | 开发阶段记录（in-progress / hanging；completed 已归档） | 活跃 |
| [`discuss/`](discuss/INDEX.md) | 技术讨论与评估报告 | 只读 |
| [`assessments/`](assessments/) | 能力评估 | 稳定 |
| [`reports/`](reports/) | 专项审计报告 | 只读 |
| [`verification/`](verification/INDEX.md) | 验证报告入口（数据在 `artifact/verification/`） | 生成 |
| [`weekly/`](weekly/) | 周报 | 归档 |
| [`archive/`](archive/) | 历史归档（废弃设计 / 旧版 roadmap） | 只读 |

## 文档生命周期

```text
design/ (当前方案) → dev/in-progress/ (实施中) → archive/dev-completed/ (已完成归档)
discuss/ (讨论) → architecture/ (稳定结论；架构权威见 wiki/03-功能模块/06-il2cpp核心架构)
```
