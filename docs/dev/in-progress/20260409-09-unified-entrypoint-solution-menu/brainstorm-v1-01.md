# 统一入口 Solution 菜单入口 Brainstorm

## 背景

统一入口已经有以下正式能力：

- `generate project subject`
- `generate project core`
- `build project subject`
- `build project core`

但当前方案还不满足新的使用目标，核心问题变成了四类：

1. 这些能力仍被混在 `Build` 语义里，用户难以直接联想到“生成可被 VS 打开的工程”。
2. 生成结果默认位于 `artifacts/projects/**`，不符合“solution 要在仓库根下可见”的要求。
3. 菜单里缺少“生成所有 solution”的聚合入口，用户无法一键生成当前宿主支持的全部工程。
4. `subject` solution 目前没有把 native project 一并纳入 solution，仍需要手动分别打开 managed solution 与 native workspace。

## 目标

把“生成/构建工程工作区”从底层命令升级为统一入口中的一等菜单体验，使用户能够：

- 从菜单里直接理解 `Subject` 与 `IL2CPP Core` 的区别。
- 在仓库根下直接看到统一的 solution 输出根，而不是去 `artifacts/` 里翻路径。
- 从菜单里选择“默认目标 / 指定目标 / 全部支持目标”，而不是回到 CLI 手敲参数。
- 一键生成当前宿主支持的全部 solution。
- 在生成 `subject` solution 时，顺带生成 subject native project，并让 solution 直接引用它。

## 备选方案

### 方案 A：继续留在 `Build` 组，只加更多平铺命令

示例：

- `generate-project-subject`
- `generate-project-core`
- `generate-project-all`

优点：

- 改动小。
- 基本不动现有 TUI 结构。

缺点：

- 菜单仍然以“构建产物”为主语，不符合用户找“工程入口”的心智。
- 不能自然承载 `subject` / `target` / `variant` / `refresh-generated` 的选择流。
- 即使加了 `generate-project-all`，说明层也依旧薄弱。

### 方案 B：新增 `工程 / IDE` 一级菜单，并补根目录布局与参数选择流

一级菜单暴露：

- 生成所有 Solution（当前宿主）
- 生成 Subject 调试工程
- 生成 IL2CPP Core 调试工程
- 构建 Subject 已生成工程
- 构建 IL2CPP Core 已生成工程

同时把所有生成结果统一落到仓库根下的 `solutions/`。

优点：

- 与用户心智一致，用户找的是“工程入口”，不是“构建产物”。
- 可以明确表达“当前宿主支持的全部 solution”。
- 可以把 `subject` solution 与 native project 的关系直接冻结到同一套设计里。
- 后续可扩展 `open solution`、`open workspace` 等 IDE 相关能力，而不会污染 `Build` 菜单。

缺点：

- 需要同时改 `run_manifest.json`、`menu.py`、`tui.py`、`project_workspace.py` 与相关测试。
- 需要新增 `generate project all` 聚合能力。

### 方案 C：把 `subject` 与 `core` 再统一成单个“生成 Solution”入口

优点：

- 顶层菜单最少。

缺点：

- `subject` 与 `core` 的参数、产物和支持目标差异很大。
- 对用户来说反而更抽象，需要先分叉一次“你要哪类工程”。
- 容易把 managed `.sln`、native project、workspace 混成一个概念。

## 结论

选择方案 B。

原因：

- 它最符合当前目标：菜单中直接暴露 solution 入口、支持一键生成全部工程、并把输出挪到仓库根。
- 它不仅解决“菜单放哪里”，也同时解决“生成结果放哪里”和“solution 里引用什么”的问题。
- 它允许把 `generate project all` 作为正式命令与正式菜单项，而不是让 TUI 私下拼接多个命令。

## 冻结的术语

- 顶层菜单名称使用 `工程 / IDE`。
- 仓库根下统一生成目录使用 `solutions/`。
- 新增聚合入口标题使用 `生成所有 Solution（当前宿主）`。
- 对 `--all-targets` 的中文表述统一为“全部支持目标”。
- `subject` solution 必须引用同次生成的 native project。
- 说明区里必须明确声明：
  managed `.sln`、native project、workspace 是三个不同层次的产物。
