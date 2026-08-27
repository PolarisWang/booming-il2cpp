# 05-项目经验 INDEX

> 收录跨功能复用的长期经验和项目级规则。

## 子类别

| 类别 | 说明 | 索引 |
|------|------|------|
| `-` | 当前无下级目录 | `-` |

## 重要文档

| 文档 | 主题 | 说明 |
|------|------|------|
| [`活动计划优先.md`](./%E6%B4%BB%E5%8A%A8%E8%AE%A1%E5%88%92%E4%BC%98%E5%85%88.md) | 活动任务优先 | 说明任何问答前都必须先处理 active 任务 |
| [`codegen-quality-fixes-2026-05.md`](./codegen-quality-fixes-2026-05.md) | Codegen 代码质量修复 | 记录 2026-05 codegen 风险评估报告的修复实施（COM refcount 原子化、TypeInfo type-id、enum 碰撞检测、ret_tag 验证、DCE 跨 body 保护、>8 参数回退） |
| [`并行git-index-干扰处置.md`](./%E5%B9%B6%E8%A1%8Cgit-index-%E5%B9%B2%E6%89%B0%E5%A4%84%E7%BD%AE.md) | 并行 git index 干扰处置 | 记录多并行 session 共用 git 仓库时 index 竞态的 5 种模式（staged 被扫走/误吸收/卷入并行链/fresh·stale lock）、fresh-vs-stale 判别与处置纪律 |

## 本层规则

- 本层只收录跨功能复用经验，不收录模块专属知识。

## 最近更新

- `2026-08-27`：新增并行 git index 干扰处置记录。
- `2026-05-20`：新增 codegen 代码质量修复记录。
- `2026-04-02`：建立项目经验目录。
