---
name: finishing-a-development-branch
description: 当实现完成、所有测试通过，需要决定如何集成工作时使用——通过呈现合并、PR 或清理的结构化选项来指导完成开发工作
---

# 完成开发分支

## 概述

通过呈现清晰选项并处理所选工作流来指导完成开发工作。

**核心原则：** 确认当前分支测试状态 → 呈现选项 → 执行选择 → 清理。

**开始时宣布：** "我正在使用 finishing-a-development-branch 技能来完成这项工作。"

## 流程

### 第一步：验证测试

**在呈现选项之前，确认当前分支已有最新的测试验证结果：**

```bash
# 如果调用方尚未运行，补跑项目测试套件
npm test / cargo test / pytest / go test ./...
```

如果调用方（例如 `executing-plans`）刚刚完成了完整套件并有最新证据，可以复用该结果；否则在这里补跑。

**如果测试失败：**
```
测试失败（N 个失败）。完成前必须修复：

[显示失败]

在测试通过之前无法继续合并/PR。
```

停下来。不要继续到第二步。

**如果测试通过：** 继续到第二步。

### 第二步：确定基础分支

```bash
# 尝试常见基础分支
git merge-base HEAD main 2>/dev/null || git merge-base HEAD master 2>/dev/null
```

或询问："这个分支从 main 分叉——这正确吗？"

### 第三步：呈现选项

恰好呈现这 4 个选项：

```
实现完成。你想怎么做？

1. 本地合并回 <base-branch>
2. 推送并创建 Pull Request
3. 保持分支不变（我稍后处理）
4. 丢弃这项工作

选哪个？
```

**不要添加解释** - 保持选项简洁。

### 第四步：执行选择

#### 选项 1：本地合并

```bash
# 切换到基础分支
git checkout <base-branch>

# 拉取最新
git pull

# 合并功能分支
git merge <feature-branch>

# 验证合并结果上的测试
<test command>

# 如果测试通过
git branch -d <feature-branch>
```

然后：清理 worktree（第五步）

#### 选项 2：推送并创建 PR

```bash
# 推送分支
git push -u origin <feature-branch>

# 创建 PR
gh pr create --title "<title>" --body "$(cat <<'EOF'
## 摘要
<2-3 条变更要点>

## 测试计划
- [ ] <验证步骤>
EOF
)"
```

然后：清理 worktree（第五步）

#### 选项 3：保持不变

报告："保持分支 <name>。Worktree 保留在 <path>。"

**不要清理 worktree。**

#### 选项 4：丢弃

**首先确认：**
```
这将永久删除：
- 分支 <name>
- 所有提交：<commit-list>
- <path> 处的 worktree

输入 'discard' 确认。
```

等待确切的确认。

如果确认：
```bash
git checkout <base-branch>
git branch -D <feature-branch>
```

然后：清理 worktree（第五步）

### 第五步：清理 Worktree

**对于选项 1、2、4：**

检查是否在 worktree 中：
```bash
git worktree list | grep $(git branch --show-current)
```

如果是：
```bash
git worktree remove <worktree-path>
```

**对于选项 3：** 保留 worktree。

## 快速参考

| 选项 | 合并 | 推送 | 保留 Worktree | 清理分支 |
|------|------|------|---------------|---------|
| 1. 本地合并 | ✓ | - | - | ✓ |
| 2. 创建 PR | - | ✓ | ✓ | - |
| 3. 保持不变 | - | - | ✓ | - |
| 4. 丢弃 | - | - | - | ✓（强制） |

## 常见错误

**跳过测试状态确认**
- **问题：** 合并损坏的代码，创建失败的 PR
- **修复：** 在提供选项之前始终确认已有最新测试结果；没有就补跑

**开放性问题**
- **问题：** "接下来我应该做什么？" → 模糊
- **修复：** 恰好呈现 4 个结构化选项

**自动 worktree 清理**
- **问题：** 在可能仍需要时删除 worktree（选项 2、3）
- **修复：** 只对选项 1 和 4 清理

**丢弃时无确认**
- **问题：** 意外删除工作
- **修复：** 需要输入 "discard" 确认

## 红旗

**绝不：**
- 在测试失败时继续
- 在没有验证结果上的测试的情况下合并
- 在没有确认的情况下删除工作
- 未经明确请求就强制推送

**始终：**
- 在提供选项之前确认已有最新测试结果
- 恰好呈现 4 个选项
- 对选项 4 获得输入确认
- 只对选项 1 和 4 清理 worktree

## 集成

**由以下调用：**
- **subagent-driven-development**（步骤 7）- 所有任务完成后
- **executing-plans**（步骤 5）- 所有批次完成后

**配合：**
- **using-git-worktrees** - 清理该技能创建的 worktree
