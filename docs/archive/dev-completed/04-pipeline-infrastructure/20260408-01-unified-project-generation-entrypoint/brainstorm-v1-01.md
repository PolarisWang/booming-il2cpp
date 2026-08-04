# 统一入口工程生成与可调试工作区头脑风暴 v1

## 目标

在统一入口中新增 `generate project` 能力，满足以下用户诉求：

1. 为指定 subject 生成可本地调试的工程集合，至少包含：
   - subject 源工程
   - subject 测试工程
   - native codegen 产物对应的 native 工程
   - native 测试 / proof 工程
2. 生成 IL2CPP 核心代码工程。
3. 生成后的各目标平台工程都能通过统一入口触发 build。

## 当前仓库现状

- 统一入口当前只公开 `build / test / prepare / clean / inspect`，没有 project/workspace 生成命令。
- managed 核心代码已经以独立 `csproj` 形式存在于 `src/managed/*`。
- subject 源工程与测试工程已经 subject-local：
  - 源工程位于 `subjects/<subject-id>/source/`
  - validation/unit 项目位于 `subjects/<subject-id>/validation/unit/`
- native proof/codegen 流程已存在，但目前只产出 run-scoped 结果：
  - generated native 代码位于 `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/generated/`
  - native build/runtime 结果位于 `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/build|runtime/`
- Windows subject native 现在偏执行态导向，默认用 `subject-exec` 这个稳定 run id 作为本地固定入口，但不会额外生成一个面向 IDE 的统一工程工作区。
- CMake configure 已经可以在 Windows 上产出 `.sln`，但当前统一入口把它当构建中间产物使用，不把它提升为正式“可调试工程”能力。

## 关键设计张力

### 张力 1：生成“快照副本”还是生成“引用现有源码的工作区”

- 如果导出完整副本，用户打开方便，但会复制源码、generated cpp 和配置，后续容易改错位置。
- 如果生成引用现有源码的工作区，源码真源不变，但命令与产物组织必须更清晰。

### 张力 2：以运行结果为中心还是以调试体验为中心

- 当前 subject/native 体系以 run-scoped canonical artifact 为中心。
- 新需求要求能本地调试，因此需要在不破坏 canonical artifact 规则的前提下，增加稳定、可重复打开的 IDE/workspace 入口。

### 张力 3：一次性覆盖所有 host/IDE 还是先做 Windows/Visual Studio 一等公民

- 当前 subject proof/native 规则本来就明显偏 Windows。
- 如果一开始同时做 Windows/macOS/Linux 三套 IDE 体验，范围会快速膨胀。
- 用户当前请求更像“先把统一入口的工程生成能力做出来，并且目标平台可 build”，不要求第一次就提供所有 IDE 的最优体验。

## 方案对比

## 方案 A：导出独立快照工作区

做法：

- `run generate project ...` 在 `artifacts/projects/...` 下复制 subject/core 所需源码、generated cpp、测试工程和 native host 工程。
- 用户只打开这份导出工作区。

优点：

- 结果物理隔离，用户容易理解。
- 调试时不会直接碰仓库主路径。

缺点：

- 源码和 generated 结果会重复，极易出现“改了副本但主仓没变”。
- 需要复杂的同步/刷新策略。
- 不符合当前仓库“源码真源 + artifacts 可再生”的边界。

结论：

- 不推荐。

## 方案 B：生成引用仓库真源的调试工作区

做法：

- `run generate project ...` 只生成工作区元数据、solution、CMake configure root 与 workspace manifest。
- managed 工程直接引用 `subjects/...` 和 `src/managed/...` 下的真实 `csproj`。
- subject native generated 输入统一刷新到稳定 run `subject-exec`，native 工程指向该稳定 generated root。
- native 工程的 configure root 落到 `artifacts/projects/...`，用于 IDE 打开和本地 build。

优点：

- 不复制源码，单一真源清晰。
- 与当前 subject-exec 稳定路径、run-scoped artifact 模型兼容。
- 可以自然复用当前 CMake、subject worker、manifest 与 matrix 规则。
- 更适合作为统一入口的长期正式能力。

缺点：

- 需要同时处理 managed solution、subject generated 刷新和 native configure 三类对象。
- 首版设计要明确“哪些是工程文件，哪些是 canonical 证据”。

结论：

- 推荐。

## 方案 C：只生成 configure/build 目录，不生成调试友好的工程入口

做法：

- 统一入口只预热 generated cpp 和 CMake binary dir。
- 用户自行打开 `csproj` 或到 `artifacts/...` 找 `.sln`。

优点：

- 实现最小。

缺点：

- 不能真正满足“我需要能本地调试这些代码”。
- 仍然保留当前“用户自己拼路径”的问题。

结论：

- 不满足需求，不推荐。

## 推荐结论

推荐采用方案 B：

- 对外命令保持用户要的 `generate project` 语义。
- 对内实现为“生成可重复打开的工作区”，而不是导出源码副本。
- generated native 输入继续使用稳定 `subject-exec` 路径，避免引入新的伪 canonical 路径。
- Windows/Visual Studio 作为第一优先体验；目标平台 build 能力通过同一工作区 manifest 统一调度。

## 分流判断

本任务应进入 `roadmap`，理由如下：

1. 至少拆成 3 个相对独立阶段：
   - 统一入口命令面与 workspace manifest
   - subject 工程生成
   - core 工程生成与 build 集成
2. 需要同时改动 manifest、handler、subject worker、文档和验证。
3. 需要先锁定输出边界与用户体验，再进入实现，否则很容易在中途推翻命令面。

## 建议的下一步

- 先确认推荐方案与“Windows/Visual Studio 优先”的方向。
- 用户确认后进入 `roadmap`，按阶段实施。
