# 04-工具与集成 INDEX

## 本目录职责

- 记录仓库级工具入口、wrapper、测试拉起方式和 harness / CI 集成约定。
- 记录长期有效的命令协议、产物边界和接入规则，不记录单次运行结果。

> 收录工具入口、测试入口和集成约定。

## 子类别

| 类别 | 说明 | 索引 |
|------|------|------|
| `-` | 当前无下级目录 | `-` |

## 重要文档

| 文档 | 主题 | 说明 |
|------|------|------|
| [`统一入口脚本.md`](./%E7%BB%9F%E4%B8%80%E5%85%A5%E5%8F%A3%E8%84%9A%E6%9C%AC.md) | 统一入口 `run.*` | 说明 bootstrap、全屏 TUI、prepare/clean、`--json` 和 harness 生命周期 |
| [`统一测试框架.md`](./%E7%BB%9F%E4%B8%80%E6%B5%8B%E8%AF%95%E6%A1%86%E6%9E%B6.md) | 统一测试域 `run test` | 说明 suite manifest、日志目录、事件流、红绿灯覆盖、性能基线和批量执行规则 |
| [`Claude技能测试入口.md`](./Claude%E6%8A%80%E8%83%BD%E6%B5%8B%E8%AF%95%E5%85%A5%E5%8F%A3.md) | Claude 测试入口 | 说明 `.codex/tests/claude-code/` 的运行入口 |

## 本层规则

- 工具与集成页面描述入口和长期协议，不记录单次运行结果。

## 最近更新

- `2026-04-04`：更新 `统一测试框架.md` 与 `统一入口脚本.md`，补充统一测试框架第一版发布摘要，以及统一入口菜单中的单一 `test` 菜单项规则。
- `2026-04-04`：更新 `统一测试框架.md` 与 `统一入口脚本.md`，补充 perf baseline、build fingerprint、strict 退出码，以及对外只保留 `run test ...` 的最终契约。
- `2026-04-04`：更新 `统一测试框架.md`，补充 traffic-light 覆盖模型、`coveragePolicy` selector 和 summary/report 三色计数规则。
- `2026-04-04`：更新 `统一测试框架.md` 与 `统一入口脚本.md`，补充 TUI / harness 消费统一测试事件流，以及 `run test ... --json` 优先输出 `events.jsonl` 的规则。
- `2026-04-04`：更新 `统一测试框架.md`，补充结果目录、`runId` / `summaryPath` 元数据和 `final-summary` 事件的最小契约。
- `2026-04-04`：更新 `统一测试框架.md`，补充 session 执行器、`implicit build` 标记和 host 不兼容时的退出码约定。
- `2026-04-04`：更新 `统一测试框架.md`，补充 adapter registry、synthetic `main` 和 `caseId` 冲突后缀规则。
- `2026-04-04`：更新 `统一测试框架.md`，记录 family 目录迁移的当前状态，以及旧目录兼容副本的边界。
- `2026-04-04`：继续更新 `统一入口脚本.md` 与 `统一测试框架.md`，补充 `prepare` 统一走 smoke build stage，以及 TUI 动态测试菜单输入规则。
- `2026-04-04`：更新 `统一入口脚本.md` 与 `统一测试框架.md`，补充 macOS 下 `dotnet` 按需引导、Homebrew 前提和非交互限制。
- `2026-04-03`：新增 `统一测试框架.md`，沉淀 `run test ...`、suite manifest、日志目录、事件流、红绿灯覆盖和 perf 基线规则。
- `2026-04-03`：更新 `统一入口脚本.md`，补充 `run` 全屏 TUI 菜单结构与按键说明。
- `2026-04-03`：新增 `统一入口脚本.md`，记录 `run.ps1` / `run.sh` / `run.cmd` 协议与 harness 生命周期。
- `2026-04-02`：建立工具与集成目录。
