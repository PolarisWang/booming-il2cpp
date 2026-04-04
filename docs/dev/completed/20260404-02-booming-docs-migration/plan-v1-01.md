# docs/booming 历史文档迁移实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 将 `docs/booming` 的历史任务文档按新结构导入到 `docs/dev/completed/`，并确保现行入口能在新结构下看到这些任务。

**架构：** 迁移采用“逐目录复制导入 + 每目录补一份 `STATUS.md` + 更新索引”的机械式方案。对旧正文零改写，只修正现行 wiki 中仍引用旧路径的少量入口，确保内容不丢失且新结构可用。

**技术栈：** Markdown、文件系统目录迁移、索引维护

**设计文档：** `docs/dev/in-progress/20260404-02-booming-docs-migration/design-v1-01.md`

**预期知识沉淀：** `wiki/03-功能模块/03-文档体系/docs与wiki边界.md` 与相关现行引用

---

## 任务前提

- 原始 `docs/booming/` 文件全部保留
- 目标目录统一写入 `docs/dev/completed/`
- 旧任务目录名保持不变

## 需要创建或修改的文件

- 创建 `docs/dev/completed/<legacy-task>/STATUS.md`
- 创建导入后的 `docs/dev/completed/<legacy-task>/` 目录
- 更新 `docs/dev/INDEX.md`
- 更新 `docs/dev/in-progress/INDEX.md`
- 更新 `docs/dev/ACTIVE.md`
- 更新少量仍引用旧路径的现行文档
- 追加 `docs/dev/in-progress/20260404-02-booming-docs-migration/notes/progress-v1-01.md`

## 执行步骤

- [x] 步骤 1：为每个 `docs/booming/<legacy-task>/` 创建对应的 `docs/dev/completed/<legacy-task>/`
- [x] 步骤 2：将每个旧目录下的 Markdown 文件原样复制到对应完成目录
- [x] 步骤 3：为每个导入目录写入 `STATUS.md`，标记为历史归档任务
- [x] 步骤 4：更新 `docs/dev/INDEX.md`，将这些历史任务纳入总览
- [x] 步骤 5：更新仓库内现行文档对 `docs/booming/...` 的直接引用
- [x] 步骤 6：记录迁移结果与验证边界
- [x] 步骤 7：将本迁移任务归档为 `completed`

## 验证

- `find docs/dev/completed -maxdepth 2 -name 'STATUS.md' | sort`
- `git diff --check -- docs/dev wiki`
- `rg -n "docs/booming" wiki docs --glob '!docs/booming/**' --glob '!docs/dev/completed/**'`

## 执行期状态维护

- 执行中维护本任务的 `STATUS.md`
- 执行中维护 `docs/dev/ACTIVE.md`
- 在关键节点写入 `notes/progress-*.md`
- 结束时删除 `docs/dev/ACTIVE.md`，并将本任务从 `in-progress` 移到 `completed`
