# 统一工程生成与 CLI 重构 Roadmap v1.01

Date: 2026-04-10
Status: roadmap-frozen-v1-01

## 1. 目标

本 roadmap 的目标是把当前散落在多个模块中的工程生成能力，收口为：

1. 一个子命令风格的 `chaos-il2cpp` CLI（convert / build / publish）
2. 一套完整的 subject solution 生成流程（C# + C# Tests + native + native tests）
3. 一个可用的 deploy 输出目录结构
4. 优化后的统一入口体验（进度反馈、TUI 简化、友好错误提示）

## 2. 范围边界

本 roadmap 负责：

- 改造 `Chaos.IL2CPP.Driver` 为子命令 CLI
- 扩展 subject.manifest.json 的 `input` 字段（csproj / dlls / csproj+dlls）
- 改造 `generate_subject_workspace()` 使每个 subject solution 包含 4 类工程
- 新增 `convert` 子命令：读取 manifest → 准备程序集 → 运行 Pipeline → 输出 native 源码 + CMakeLists.txt
- 新增 `build` 子命令：读取 convert 产物 → cmake configure + build → 输出到 deploy/
- 新增 `publish` 子命令：convert + build 一步到位
- TUI 新增快捷入口、进度 spinner、友好错误提示
- 更新 run.ps1 / run.sh 中的命令注册

本 roadmap 不负责：

- 运行时库打包
- `src/native/` 目录结构调整
- subject source 内部结构变更
- 跨平台交叉编译支持

## 3. 非目标

- 不做 CI/CD pipeline 集成
- 不做 NuGet / package 发布
- 不做 IDE 插件

## 4. 阶段列表

### Phase 0: Manifest Input Model

扩展 subject.manifest.json，新增 `input` 字段，定义三种输入类型。更新现有 subject 的 manifest。

### Phase 1: Driver CLI 重构

把 `Chaos.IL2CPP.Driver` 从位置参数模式改为子命令 dispatch（convert / build / publish），保持向后兼容。

### Phase 2: Convert 子命令实现

实现 `chaos-il2cpp convert`：读取 manifest → 根据 input.kind 准备程序集 → 运行 Pipeline → 输出 generated + metadata + CMakeLists.txt + convert.manifest.json。

### Phase 3: Build 子命令与 Deploy 流程

实现 `chaos-il2cpp build`：读取 convert.manifest.json → cmake configure + build → 输出到 deploy/。实现 `publish` = convert + build。

### Phase 4: Subject Solution 生成重构

改造 `generate_subject_workspace()` 使每个 subject 的 .sln 包含 4 类工程（C# + C# Tests + native + native tests），支持三种 input.kind。

### Phase 5: 统一入口优化

TUI 菜单简化、进度 spinner、友好错误提示、命令注册更新。

## 5. 每阶段完成定义

### Phase 0

- goal: 所有现有 subject 的 manifest 都有 `input` 字段，CLI 和 tooling 能读取它
- exit_criteria:
  - subject.manifest.json schema 扩展完成
  - 现有 10 个 subject 的 manifest 已更新
  - manifest schema 验证测试通过
- deliverables:
  - 更新后的 subject.manifest.json × 10
  - manifest schema 文档
- dependencies: 无
- open_questions: 无

### Phase 1

- goal: Driver 支持子命令 dispatch，旧调用方式保持兼容
- exit_criteria:
  - `chaos-il2cpp convert --help` 可用
  - `chaos-il2cpp build --help` 可用
  - 旧的 `chaos-il2cpp <input.dll> <output>` 仍然可用
  - `dotnet build` + `dotnet publish` Driver 成功
- deliverables:
  - 改造后的 DriverEntry.cs
  - 子命令参数解析
- dependencies: Phase 0
- open_questions: 无

### Phase 2

- goal: `chaos-il2cpp convert <subject-dir> --output <dir>` 能跑通三种 input.kind
- exit_criteria:
  - kind=csproj: HelloWorldObject convert 成功，输出 generated + CMakeLists.txt
  - kind=dlls: 可用（如有测试 subject）
  - kind=csproj+dlls: 可用（如有测试 subject）
  - convert.manifest.json 输出正确
  - Pipeline 输出与现有 analysis worker 输出一致
- deliverables:
  - ConvertCommand 实现
  - CMakeLists.txt 生成器
  - convert.manifest.json 规格
- dependencies: Phase 0, Phase 1
- open_questions: CMakeLists.txt 模板是否复用现有 CMakePresets.json 的结构

### Phase 3

- goal: `chaos-il2cpp build` 和 `publish` 能把 convert 产物编译到 deploy/
- exit_criteria:
  - `chaos-il2cpp build <dir> --target windows-x64-reference` 成功
  - deploy/<SubjectId>/bin/<target-id>/ 下有可执行产物
  - `chaos-il2cpp publish` 一步到位跑通
- deliverables:
  - BuildCommand 实现
  - PublishCommand 实现
  - deploy/ 输出结构
- dependencies: Phase 2
- open_questions: 无

### Phase 4

- goal: 每个 subject 的 .sln 包含 C# + C# Tests + native + native tests 四类工程
- exit_criteria:
  - `run generate project subject --id HelloWorldObject` 生成的 .sln 包含 4 类项目
  - kind=dlls 的 subject 生成的 .sln 只有 native + native-tests
  - workspace.manifest.json 正确记录所有项目
  - VS 能打开 .sln 并正常加载所有项目
- deliverables:
  - 改造后的 generate_subject_workspace()
  - 更新后的 workspace.manifest.json schema
- dependencies: Phase 0, Phase 2
- open_questions: 无

### Phase 5

- goal: 统一入口体验优化完成
- exit_criteria:
  - TUI Quick Start 有 "Generate all" 和 "Build subject" 入口
  - convert / build / publish 过程中有步骤 spinner
  - 工具缺失时给出友好提示而非 traceback
  - run_manifest.json 注册新命令
- deliverables:
  - TUI 菜单改造
  - 进度反馈 hooks
  - 错误提示改进
  - manifest 命令注册
- dependencies: Phase 1, Phase 3
- open_questions: 无

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `manifest-input-model` | Phase 0 | `ready` | `codex` | 扩展 subject.manifest.json input 字段 | `-` |
| `driver-cli-subcommands` | Phase 1 | `ready` | `codex` | Driver 子命令 dispatch 重构 | `manifest-input-model` |
| `convert-command` | Phase 2 | `planned` | `codex` | 实现 convert 子命令（三种 input.kind + CMakeLists 生成） | `manifest-input-model`, `driver-cli-subcommands` |
| `build-deploy-command` | Phase 3 | `planned` | `codex` | 实现 build / publish 子命令与 deploy 输出 | `convert-command` |
| `subject-solution-reorg` | Phase 4 | `planned` | `codex` | subject solution 4 工程重构 | `manifest-input-model`, `convert-command` |
| `unified-entry-ux` | Phase 5 | `planned` | `codex` | TUI + 进度 + 错误提示优化 | `driver-cli-subcommands`, `build-deploy-command` |

## 7. 依赖

推荐推进顺序：

1. Phase 0: manifest input model（基础，无依赖）
2. Phase 1: Driver CLI 重构（依赖 Phase 0）
3. Phase 2: convert 子命令（依赖 Phase 0 + 1）
4. Phase 3: build / deploy（依赖 Phase 2）
5. Phase 4: subject solution 重构（可与 Phase 3 并行，依赖 Phase 0 + 2）
6. Phase 5: 入口优化（最后，依赖 Phase 1 + 3）

其中：
- Phase 4 可以在 Phase 2 完成后与 Phase 3 并行推进
- Phase 5 必须等 Phase 3 完成后才能开始（需要完整命令面才能注册和优化）

## 8. 风险

### 风险 1: Pipeline 接口变更

当前 Pipeline 接受单个 input assembly。如果 input.kind=dlls 需要传入多个 DLL，可能需要扩展 Pipeline 入口。

### 风险 2: CMakeLists.txt 生成复杂度

convert 输出的 CMakeLists.txt 需要适配多个 target 平台的工具链。如果模板化不够灵活，每个平台都要单独维护。

### 风险 3: VS 加载混合 .sln

一个 .sln 同时包含 C# (.csproj) 和 CMake (vcxproj) 项目，VS 需要同时加载两种工具链，可能有兼容性问题。

### 风险 4: 向后兼容

Driver 旧调用方式被测试和 CI 依赖。如果重构不小心改了行为，会导致现有测试回归。

## 9. 备选路径

### 备选 A: CLI 用 Python 而非 C#

把 CLI 入口放在 Python 侧（扩展 run.py），而非改造 C# Driver。优点是开发快，缺点是无法独立发布。

不推荐。用户明确要求类 dotnet CLI。

### 备选 B: 不做 build 子命令

CLI 只做 convert，build 交给用户自己 cmake。优点是简化实现，缺点是用户体验不完整。

不推荐。用户要求 convert + build 闭环。

### 备选 C: 保持当前路线

如上 6 个阶段顺序推进。

这是当前推荐路径。

## 10. 当前建议推进顺序

建议立刻启动 Phase 0 `manifest-input-model`，因为它是所有后续阶段的基础、改动最小、风险最低。

Phase 0 完成后紧接 Phase 1 `driver-cli-subcommands`，两者加起来建立起 CLI 骨架。

之后 Phase 2 → Phase 3 → Phase 4（可并行）→ Phase 5。
