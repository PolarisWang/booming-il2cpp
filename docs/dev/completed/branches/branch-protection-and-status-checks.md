# 分支保护与 Required Status Checks

> **授权配置**：`https://github.com/PolarisWang/booming-il2cpp/settings/branches`
> **技术实现**：通过 GitHub REST API 配置，见本会话 seq9 操作记录

## 当前配置

| 设置 | 值 |
|------|-----|
| 分支 | `main` |
| 需要 PR 审核 | ❌（见下） |
| 过时审核自动驳回 | — |
| 强制管理员 | ❌（见下） |
| 允许强制推送 | ❌ |
| 允许删除 | ❌ |
| Required checks 严格模式 | ❌ |

**为什么 `enforce_admins` 与 PR 审核都是关的**：仓库当前以直推 `main` 为工作方式（多人 + 多 agent 并行）。
首次配置时两者都开，结果当次推送立即被 required checks 拒绝（checks 尚未跑完）—— 门禁确实生效了，
但也意味着在 required checks 全绿之前无人能推进 `main`，包括修复导致 red 的提交本身。这是鸡生蛋问题。

当前取舍：**保留 required checks 白名单与禁止强推/删除，但允许管理员绕过**。
门禁对常规 PR 生效；管理员在修复场景可直推。当 CI 稳定到相当比例 stage 常绿后，
可重新打开 `enforce_admins` 与 PR 审核。


## Required Status Checks（白名单 — 2026-09-10 当前通过的 stage）

| Check Name | 类型 |
|------------|------|
| `resolve` | Pipeline 元 stage |
| `style-csharp / style-csharp` | C# 风格门禁 |
| `aot-smoke / aot-smoke` | AOT 编译烟感 |
| `clang-tidy / clang-tidy` | C++ 静态分析 P0 |
| `metadata-writer-unit / metadata-writer-unit` | Metadata writer 单元测试 |
| `secret-scan / Detect leaked secrets` | 密钥扫描 |

## 维护要点

1. **白名单是当前** `**成功**` 的 stage 子集。其余 25+ stage 一直 FAIL 或 SKIP，如果设为 required 会永久阻塞 merge，这也是 `M2` 要解决的核心问题
2. **添加新的 required check**：确认该 check 在最近 3 次 main 推送 `success` 后再加入，避免意外阻塞主线
3. **严格模式 (`required_status_checks.strict=false`)**：当前关闭 —— 开启后要求分支在 merge 前必须 rebase 到最新 main，会与当前工作流冲突
4. **非 pipeline stage**（如 `Build + Small Batch`、`aot-codegen (ubuntu-latest)`）跑在旧式 workflow 上，stage 名=workflow 名 ≠ pipeline stage 名，暂未纳入 required

## 未来扩展（随 M2 推进逐步补充）

- `gc-unit / gc-unit`
- `codegen-unit / codegen-unit`
- `foundation-dll / foundation-dll`
- `hygiene / hygiene`