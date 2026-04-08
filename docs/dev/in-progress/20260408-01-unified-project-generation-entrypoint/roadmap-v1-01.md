# 统一入口工程生成与可调试工作区 Roadmap v1.01

Date: 2026-04-08
Status: active-roadmap

## 1. 目标

把“统一入口缺少正式 project/workspace 生成能力”的问题拆成一条可阶段推进的路线，优先满足用户已经确认的第一目标：

- 统一入口支持 `generate project subject`
- `subject-exec` 作为本地调试用稳定 generated 槽位，但只允许手动刷新
- 不在当前阶段实现 core project，也不在当前阶段实现 `build project`

本 roadmap 的职责是冻结阶段边界、依赖关系和后续 child task 方向，而不是直接承载实现。

## 2. 范围边界

本 roadmap 负责：

- 统一入口 `generate project` 的产品语义与阶段边界
- `subject-exec` 的手动刷新规则
- subject 调试工作区的输出目录与 manifest 边界
- 后续 `build project subject` 与 `core project` 的进入顺序

本 roadmap 不负责：

- 直接改动 `run_manifest.json`
- 直接新增 handler / CLI 解析逻辑
- 直接生成 `.sln` / CMake configure root
- 直接改动 tests 或 wiki

## 3. 非目标

当前父 roadmap 不一次性解决以下问题：

- `generate project core` 的具体实现
- `build project subject` 的具体实现
- 所有 host/IDE 的一等公民体验
- 导出型“源码副本工作区”
- 让 `subject-exec` 自动跟随每次 `run test subject`

## 4. 阶段列表

### Phase 1: Subject Project Generation

- 对外建立 `generate project subject` 语义。
- 冻结 `subject-exec` 的手动刷新行为。
- 冻结 subject 工作区输出布局、workspace manifest 和 stable generated root 的绑定方式。

### Phase 2: Subject Project Build Integration

- 为已生成的 subject 工作区增加统一入口 build 能力。
- 明确 `build project subject` 如何读取 workspace manifest、如何定位 managed/native 构建入口。

### Phase 3: Core Project And Target Build Expansion

- 把生成能力扩展到 `src/managed` 核心工程。
- 按 host 能力逐步补齐目标平台工程生成与 build。

## 5. 每阶段完成定义

### Phase 1

- `goal`: 把 `generate project subject` 的命令面、输出布局和 `subject-exec` 刷新规则冻结成稳定设计，并作为后续实现入口。
- `exit_criteria`:
  - `generate project subject` 的命令语法固定
  - `subject-exec` 被定义为“手动刷新、面向本地调试”的稳定 generated 槽位
  - subject 工作区输出目录与 `workspace.manifest.json` 结构固定
  - 明确区分 `artifacts/projects/**` 与 canonical run artifacts
- `deliverables`:
  - subject project generation 设计文档
  - phase 1 child plan 入口
  - 对 `subject-exec` 更新规则的明确说明
- `dependencies`:
  - 当前 brainstorm/design 结论
- `open_questions`:
  - 首版 subject managed solution 是否只纳入 `source + validation/unit`，还是同时纳入 perf harness
  - Windows 首版 native IDE 入口是否直接固定为 Visual Studio solution

### Phase 2

- `goal`: 为 subject 工作区补齐统一入口 build 语义。
- `exit_criteria`:
  - `build project subject` 命令面固定
  - build 过程从 workspace manifest 读取，而不是重新推断路径
  - managed/native build 产物落点和报告边界固定
- `deliverables`:
  - subject build integration 设计与 child plan
  - build report / artifact 输出规则
- `dependencies`:
  - Phase 1
- `open_questions`:
  - 首版是否要求 `--all-targets`
  - build report 是否写回 workspace manifest 还是独立 report 文件

### Phase 3

- `goal`: 把工作区生成能力扩到 core 工程和目标平台工程。
- `exit_criteria`:
  - `generate project core` 语义固定
  - core managed solution 范围固定
  - 各 host 下允许暴露的 target build 列表固定
- `deliverables`:
  - core project 设计与 child plan
  - 目标平台工程生成/构建范围说明
- `dependencies`:
  - Phase 2
- `open_questions`:
  - Windows/macOS/Linux 的优先级
  - 平台 build 是仅“可 configure/build”，还是也要求 IDE 可直接打开

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `subject-project-generation` | `Phase 1` | `ready` | `codex` | 冻结 `generate project subject` 的命令面、输出布局和 `subject-exec` 手动刷新语义，并为实现准备 child plan | `-` |
| `subject-project-build-integration` | `Phase 2` | `planned` | `codex` | 在 Phase 1 稳定后补齐 `build project subject` 的正式入口 | `subject-project-generation` |
| `core-project-and-target-build-expansion` | `Phase 3` | `planned` | `codex` | 将工作区生成能力扩展到 core 工程与目标平台工程 | `subject-project-build-integration` |

## 7. 依赖

全局依赖顺序固定为：

1. 先冻结 subject project generation 语义
2. 再补 subject build integration
3. 最后扩到 core 和目标平台

不能倒置的原因是：

- 如果 subject 工作区本身还未冻结，`build project subject` 就没有稳定输入
- 如果 subject build 语义还未稳定，core/target 扩展会把命令面和输出边界再次打散

## 8. 风险

### 风险 1：把 `subject-exec` 误当 latest-run 别名

如果实现阶段把 `subject-exec` 做成“每次 `test subject` 自动覆盖”，会破坏 run-scoped canonical artifact 语义，也会让本地调试目录在用户不知情时被刷新。

### 风险 2：把工作区目录误当正式证据

如果没有明确区分 `artifacts/projects/**` 和 `artifacts/subjects/<id>/runs/<run-id>/**`，后续很容易把 IDE 辅助文件、configure root 误当正式证据路径。

### 风险 3：首版范围膨胀

如果在 Phase 1 就同时引入 `core + build + multi-host IDE`，任务会重新从“subject project generation”膨胀成大而杂的入口改造。

## 9. 备选路径

### 方案 A：先只做 subject project generation

- 优点：范围最稳，直接匹配用户当前确认内容。
- 缺点：`build project` 与 core 能力需要后续阶段补齐。

### 方案 B：subject generation 与 build 一起做

- 优点：用户更快得到完整闭环。
- 缺点：命令面、workspace manifest 和 build report 会同时变化，返工风险更高。

### 方案 C：直接上 subject + core + multi-target

- 优点：看起来“一步到位”。
- 缺点：设计和实现范围都会明显失控。

当前采用：

- **方案 A**

## 10. 当前建议推进顺序

1. 先基于本 roadmap 派生 `Phase 1` child plan，只覆盖 `generate project subject`
2. Phase 1 设计与实现闭环后，再决定是否开启 Phase 2
3. `core` 与目标平台扩展保持在 Phase 3，不提前并入

## 11. 当前已确认决策

以下决策已由用户确认：

1. 当前阶段是落设计，不进入实现。
2. `subject-exec` 采用手动刷新，不做自动跟随。
3. 统一入口下一步优先加入 `generate project subject`。

## 12. 结论

当前已经具备进入 `Phase 1` child plan 的条件。下一步不应直接改代码，而应先把 `generate project subject` 的 child plan 写出来，严格限定在 subject 工作区生成与 `subject-exec` 手动刷新这两个核心面。
