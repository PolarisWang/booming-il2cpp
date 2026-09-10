# docs/booming 历史文档迁移设计

## 目标

把旧流程遗留在 `docs/booming/` 的过程文档整理到 `docs/dev` 新结构中，同时满足以下约束：

- 不修改旧文档正文
- 不丢失任何历史文件
- 新结构下每个历史任务都有可恢复的元信息入口

## 设计原则

### 1. 正文零改写

所有来自 `docs/booming/` 的 Markdown 文件按原文件名、原内容导入，不做正文替换、裁剪或重写。

### 2. 结构补齐而不是内容重造

新结构中只额外补充这些新流程需要的元数据：

- 每个历史任务目录新增 `STATUS.md`
- `docs/dev/INDEX.md` 汇总这些历史任务
- 必要时更新现行 wiki 对旧路径的直接引用

### 3. 历史原件保留

迁移采用复制导入，而不是删除式移动。`docs/booming/` 保留，作为：

- 历史路径兼容层
- 内嵌旧引用的安全回退层
- 后续若要二次清理时的对照源

## 目标结构

每个旧目录在 `docs/dev/completed/` 下建立一个同名目录，例如：

- `docs/dev/completed/2026-03-29-il2cpp-like-cs-to-cpp/`
- `docs/dev/completed/2026-03-30-roadmap/`

每个导入目录至少包含：

- 原始 Markdown 文件副本
- `STATUS.md`

## 历史任务状态语义

这些导入目录统一标记为：

- `lifecycle_status: completed`
- `phase: archived`
- `task_type: legacy-import`

这里的 `completed` 表示“历史文档导入归档已完成”，不对对应功能是否已经完全实现做额外判断。

## 现行引用处理

对仓库中仍直接引用 `docs/booming/...` 的现行文档做有限修正：

- 只改现行 wiki / 现行说明文档
- 不改历史导入文档正文

## 不做的事

- 不批量改写历史 Markdown 内部链接
- 不删除 `docs/booming/`
- 不根据现有代码实现反推每个历史任务的真实工程完成度
