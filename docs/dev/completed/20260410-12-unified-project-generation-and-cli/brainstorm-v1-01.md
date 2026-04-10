# 统一工程生成与 CLI 重构 Brainstorm

Date: 2026-04-10
Status: confirmed

## 用户需求

1. 统一入口脚本优化表现与流程（进度反馈、简化 TUI、改善错误提示）
2. 生成 IL2CPP 核心工程
3. 每个 subject 生成完整 solution（C# 工程 + C# 测试 + native 工程 + native 测试）
4. IL2CPP 核心工程输出为类 dotnet CLI（convert + build），输出到 deploy/

## 现状分析

### 已有能力

- `Chaos.IL2CPP.Driver` 已能做 managed closure generation 和 native reference emission
- `project_workspace.py` 已能生成 core 和 subject workspace
- Subject workspace 已包含 managed + native 项目
- TUI 菜单和 --json 输出已有

### 差距

- Driver 是位置参数模式（`<input> <output>`），不是子命令 CLI
- Driver 只做 codegen，不做 cmake build
- Subject solution 中没有统一包含 C# 测试工程和 native 测试工程
- deploy/ 目录为空，无部署流程
- TUI 中生成工程的命令入口不够直观
- 进度反馈和错误提示不够友好

## 决策记录

| 决策 | 结论 | 备选 |
| --- | --- | --- |
| CLI 风格 | 子命令风格（chaos-il2cpp convert/build/publish） | 保持 run 入口 / 两者都要 |
| Solution 位置 | 复用现有 solutions/subjects/ 结构 | deploy/ 目录 / solutions + deploy 分离 |
| CLI 职责 | 转换 + 构建（不含运行时打包） | 转换 + 构建 + 运行时打包 |
| 入口优化 | 进度反馈 + 简化 TUI + 改善错误提示 | 加结构化事件输出 |

## 分流判断

本任务需要进入 `roadmap`，原因：

- 涉及 4 个相对独立的子系统变更（CLI 重构、solution 生成、入口优化、deploy 流程）
- 需要分阶段推进（先 CLI，再 solution 生成，再入口优化）
- 会跨多次会话持续推进
