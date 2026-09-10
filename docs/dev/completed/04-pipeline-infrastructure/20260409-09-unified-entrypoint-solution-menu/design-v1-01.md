# 统一入口工程 / IDE 菜单设计

## 1. 设计目标

本设计解决统一入口中的“工程生成与工程构建”菜单体验，并同步调整 solution/workspace 的输出布局。

本次回答五个问题：

1. 这些入口应该放在菜单的什么位置？
2. 顶层应该暴露哪些能力，哪些不应该平铺？
3. 生成出来的 solution/workspace 应该落在仓库根下什么目录结构？
4. 菜单里如何表达“全部支持目标”和“生成所有 Solution”？
5. 用户点进菜单后，如何补齐必填参数并在执行前看清将要生成什么？

## 2. 现状约束

### 2.1 文本菜单与全屏 TUI 不是一套渲染路径

- `run menu` 的文本菜单走 `run_manifest.json` + `menu.py`
- 全屏/内联 TUI 走 `tui.py`

因此本次不能只改 manifest group；否则文本菜单和全屏 TUI 会继续分裂。

### 2.2 `generate/build project ...` 都带必填参数

例如：

- `generate project subject --id subject/<subject-id>`
- `build project subject --id subject/<subject-id>`

所以这不是简单“把命令显示出来”的问题。菜单必须在进入命令前收集参数，否则只是把错误提示提前暴露给用户。

### 2.3 现有输出布局不符合新要求

当前工作区默认落在 `artifacts/projects/**`。

新增要求是：

- 生成的 solution 全部要位于仓库根下；
- 目录结构和目录名需要统一设计；
- 生成 `subject` solution 时，要顺带生成 subject native project，并让 solution 引用它。

因此本次不再沿用 `artifacts/projects/**` 作为主工作区根。

## 3. 根目录输出布局

统一采用仓库根下的 `solutions/` 目录：

```text
solutions/
  manifest.json
  all/
    generation.report.json
  subjects/
    <subject-id>/
      workspace.manifest.json
      <subject-id>.sln
      native/
        <matrix-id>/
          <generated native project and IDE files>
  core/
    <host>/
      workspace.manifest.json
      chaos-il2cpp-core.sln
      native/
        <target-id>/
          <generated native project and IDE files>
```

设计理由：

- `solutions/` 是仓库根下直接可见的 IDE 入口目录，符合“生成到根目录下”的预期。
- 继续保留 `subject` / `core` 分层，避免根目录堆满单个 `.sln` 文件。
- 每个工作区目录仍可保留 `workspace.manifest.json`，便于后续 `build project ...` 读取。
- 根级 `solutions/manifest.json` 和 `solutions/all/generation.report.json` 可作为“生成所有 Solution”命令的聚合索引。

### 3.1 Subject 输出

对于 `subject/<subject-id>`：

- solution 路径：
  `solutions/subjects/<subject-id>/<subject-id>.sln`
- native project 根路径：
  `solutions/subjects/<subject-id>/native/<matrix-id>/`

要求：

- 每个被选中的 `matrix` 都生成一个 native project/configure root。
- solution 文件必须把这些 native project 一起引用进去。
- 默认目标时通常只引用一个 native project；`--all-targets` 时引用全部已生成 native project。

### 3.2 Core 输出

对于 `core/<host>`：

- solution 路径：
  `solutions/core/<host>/chaos-il2cpp-core.sln`
- native project 根路径：
  `solutions/core/<host>/native/<target-id>/`

本次硬性要求只冻结 subject solution 必须引用 native project。core 仍保持“solution 与 native workspace 并列输出”的实现优先级；如果实现成本低，可一并把 core native project 引入 solution，但不作为本轮菜单改造的唯一完成标准。

## 4. 菜单信息架构

### 4.1 文本菜单

在现有组顺序中新增 `Project And IDE` 分组，并放在 `Quick Start` 后、`Build` 前：

```text
Quick Start
Project And IDE
Build
Test And Verify
Environment
Inspect
```

中文别名显示为：

```text
快速开始
工程 / IDE
构建产物
测试验证
环境准备
信息查看
```

### 4.2 全屏 TUI 一级菜单

当前一级菜单为：

- prepare
- build
- test
- clean
- inspect

改为：

- prepare
- project
- build
- test
- clean
- inspect

其中 `project` 的中文标题为：

`生成或构建可供 VS / IDE 打开的 subject 与 core 工程工作区`

放置位置：

- 在 `prepare-menu` 之后
- 在 `build-menu` 之前

理由：

- 它依赖环境准备，但又不等于产物构建。
- 对用户来说先找“工程入口”，再决定是否构建，顺序更自然。

## 5. `工程 / IDE` 子菜单结构

### 5.1 顶层暴露 5 个入口

- `generate-project-all`
  - 标题：`生成所有 Solution（当前宿主）`
- `generate-project-subject`
  - 标题：`生成 Subject 调试工程`
- `generate-project-core`
  - 标题：`生成 IL2CPP Core 调试工程`
- `build-project-subject`
  - 标题：`构建 Subject 已生成工程`
- `build-project-core`
  - 标题：`构建 IL2CPP Core 已生成工程`

不在顶层平铺以下变体：

- `all-targets`
- 不同 `matrix`
- 不同 `target`
- `variant`
- `refresh-generated`

这些都通过二级/三级选择流完成。

### 5.2 生成所有 Solution 的选择流

新增正式命令：

```text
run generate project all [--host windows|macos|linux] [--refresh-generated]
```

语义：

- 对当前宿主支持的所有 `subject` 生成 solution。
- 每个 subject 默认按“全部支持目标”生成所有 native project。
- 对当前宿主支持的所有 `core` target 生成 solution / native workspace。
- 输出根级聚合清单到 `solutions/manifest.json` 和 `solutions/all/generation.report.json`。

菜单流：

1. 选择是否刷新全部 subject generated
2. 展示将生成的 subject 数量、core target 数量与输出根目录
3. 确认后执行

### 5.3 Subject 选择流

#### 生成 Subject 调试工程

1. 选择 subject
2. 选择范围
   - 默认目标
   - 指定目标
   - 全部支持目标
3. 若选“指定目标”，再选 matrix
4. 选择 variant
   - CHECK
   - PROFILE
   - SHIP
5. 选择 generated 刷新策略
   - 使用现有 generated
   - 刷新 generated 后再生成
6. 展示确认页并返回最终 argv

执行时附加要求：

- 为所选 matrix 生成 `solutions/subjects/<subject-id>/native/<matrix-id>/` 下的 native project。
- 生成 `subject` solution 时，把 source/validation `.csproj` 与 native `.vcxproj` 一并写入 `.sln`。

#### 构建 Subject 已生成工程

1. 选择 subject
2. 选择范围
   - 默认目标
   - 指定目标
   - 全部支持目标
3. 若选“指定目标”，再选 matrix
4. 展示确认页并返回最终 argv

### 5.4 Core 选择流

#### 生成 IL2CPP Core 调试工程

1. 读取当前 host 可用 target
2. 选择范围
   - 默认目标
   - 指定目标
   - 全部支持目标
3. 若选“指定目标”，再选 target
4. 展示确认页并返回最终 argv

#### 构建 IL2CPP Core 已生成工程

1. 读取当前 host 可用 target
2. 选择范围
   - 默认目标
   - 指定目标
   - 全部支持目标
3. 若选“指定目标”，再选 target
4. 展示确认页并返回最终 argv

## 6. 说明区设计

### 6.1 说明区位置

全屏 TUI 不新增复杂双栏布局，优先在现有列表下方增加“选中项说明区”。

原因：

- 当前 `render_menu_screen(...)` 是单列实现。
- 下方说明区比右侧面板改动更小，也更适配窄终端。

### 6.2 说明区固定字段

对于 `工程 / IDE` 菜单项，说明区固定展示以下字段：

- `简介`
- `输出`
- `目标范围`
- `当前宿主支持`
- `执行命令`
- `备注`

### 6.3 Subject 入口的说明区内容

`generate-project-subject` 选中时显示：

```text
简介:
  生成 Subject 的 solution 与 native 调试工程。

输出:
  - solutions/subjects/<subject-id>/<subject-id>.sln
  - solutions/subjects/<subject-id>/native/<matrix-id>/

目标范围:
  默认目标 / 指定目标 / 全部支持目标

当前宿主支持:
  从 subject.manifest.json 动态读取本机可用 matrices

执行命令:
  run generate project subject --id subject/<subject-id> [--matrix <matrix-id>] [--all-targets] [--variant CHECK|PROFILE|SHIP] [--refresh-generated]

备注:
  生成 solution 时会一并生成 native project，并把 native project 引入 solution。
```

`build-project-subject` 说明区把 `variant` 和 `refresh-generated` 去掉，强调“基于已有 workspace.manifest.json 构建”。

### 6.4 生成所有 Solution 入口的说明区内容

`generate-project-all` 选中时显示：

```text
简介:
  为当前宿主生成全部 subject/core solution 与对应 native project/workspace。

输出:
  - solutions/manifest.json
  - solutions/all/generation.report.json
  - solutions/subjects/<subject-id>/
  - solutions/core/<host>/

目标范围:
  当前宿主支持的全部 subject 与全部 core target

当前宿主支持:
  动态汇总当前宿主可生成的 subject 数量与 target 列表

执行命令:
  run generate project all --host <host> [--refresh-generated]

备注:
  这是重操作入口；subject 默认按全部支持目标生成 native project 并写入 solution。
```

### 6.5 Core 入口的说明区内容

`generate-project-core` 选中时显示：

```text
简介:
  生成 IL2CPP Core 的 managed solution 与 native 调试工作区。

输出:
  - solutions/core/<host>/chaos-il2cpp-core.sln
  - solutions/core/<host>/native/<target-id>/

目标范围:
  默认目标 / 指定目标 / 全部支持目标

当前宿主支持:
  Windows: windows-x64-reference / android-arm64 / linux-x64
  macOS: macos-reference / ios-arm64 / linux-x64
  Linux: linux-x64

执行命令:
  run generate project core --host <host> [--target <target-id>] [--all-targets]

备注:
  不是所有 target 都会导出 Visual Studio solution；native 侧统一视为 workspace。
```

## 7. 文本菜单展示规则

文本菜单不做多步交互，但要把标题改成用户读得懂的中文：

- `generate-project-all: 生成所有 Solution（当前宿主）`
- `generate-project-subject: 生成 Subject 调试工程`
- `generate-project-core: 生成 IL2CPP Core 调试工程`
- `build-project-subject: 构建 Subject 已生成工程`
- `build-project-core: 构建 IL2CPP Core 已生成工程`

标题后建议补一个短括号提示：

- `（默认目标 / 指定目标 / 全部支持目标）`

这样即使只看 `run menu` 的纯文本输出，用户也能知道“支持全部目标生成”已经内建，同时还能看到存在一个独立的“生成所有 Solution”入口。

## 8. 术语与命名冻结

### 8.1 分组命名

- 英文 group：`Project And IDE`
- 中文显示：`工程 / IDE`

### 8.2 根目录命名

- 仓库根下固定目录：`solutions/`
- subject solution 根：`solutions/subjects/<subject-id>/`
- core solution 根：`solutions/core/<host>/`

### 8.3 目标范围命名

- `default target` -> `默认目标`
- `specific target` -> `指定目标`
- `all-targets` -> `全部支持目标`

### 8.4 输出命名

- managed `.sln` 可以叫 `solution`
- native `.vcxproj` / IDE 工程叫 `native project`
- configure root 统一叫 `workspace` 或 `调试工作区`

## 9. 范围外事项

以下内容明确不在本次菜单改造范围内：

- 新增脱离 `project.dispatch` 的第二套生成链
- 在菜单中直接调用 VS 打开 solution
- 新增 `deploy core` 的 IDE 化入口
- 把 `deploy core` 混入 `工程 / IDE` 一级菜单

## 10. 推荐实现方向

### 10.1 Manifest 层

保留现有 `generate/build project subject|core` 命令兼容性，并新增：

- `generate project all`

### 10.2 生成层

- `project_workspace.py` 负责把工作区根从 `artifacts/projects/**` 迁移到 `solutions/**`
- subject solution writer 需要支持把 native `.vcxproj` 写入 `.sln`
- 聚合生成入口需要输出根级 manifest/report

### 10.3 菜单层

- `menu.py` 负责文本菜单中的组名和标题中文化
- `tui.py` 负责：
  - 新增 `project-menu`
  - 新增 `generate-project-all`
  - 新增项目菜单子项
  - 增加参数收集与确认页
  - 在列表底部渲染选中项说明区

### 10.4 运行时数据来源

- `subject` 可选项来自 `subjects/*/subject.manifest.json`
- `core` 可选 target 来自 `project_workspace.py` 中的 host/target 规格

## 11. 设计结论

这次不是“再加几个命令到 Build 菜单”。

它本质上是把现有 workspace 能力重新组织为面向 IDE 的入口层：

- 位置上，从 `Build` 里独立出来；
- 布局上，从 `artifacts/projects/**` 迁移到仓库根下的 `solutions/**`；
- 展示上，显式区分 solution、native project 与 workspace；
- 能力上，新增“生成所有 Solution（当前宿主）”聚合入口；
- 交互上，补上必填参数的菜单内选择流；
- 组成上，要求 subject solution 引用同次生成出的 native project。
