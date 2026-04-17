# systematic-debugging

> 项目中的系统化调试 skill，负责在出现失败或异常行为时追到根因。

## 基本信息

- 分层：质量保障
- 项目职责：帮助定位 skill、测试或流程中的根因问题
- 实现路径：`.codex/skills/dev-systematic-debugging/SKILL.md`
- 状态：`project`

## 项目规则

- 活动任务守卫、任务目录状态文档和 wiki 边界出错时，应优先追根因而不是打补丁
- 测试阶段如果 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃，必须保留崩溃证据，定位 project / target / task，并修复根因后再继续

## 最近变更

- `2026-04-17`：补充测试阶段 `dotnet` 编译崩溃的根因调查要求。
- `2026-04-02`：纳入项目质量保障层。
