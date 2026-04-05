# 06-测试验证 INDEX

> 项目级测试与验证知识的唯一正式入口。

## 子类别

| 类别 | 说明 | 索引 |
|------|------|------|
| `模块` | 模块验证对象与主模块映射 | [`模块/INDEX.md`](./模块/INDEX.md) |
| `整体验证场景` | system 对象说明与入口 | [`整体验证场景/INDEX.md`](./整体验证场景/INDEX.md) |
| `测试管线` | pipeline 对象说明与入口 | [`测试管线/INDEX.md`](./测试管线/INDEX.md) |

## 重要文档

| 文档 | 主题 | 说明 |
|------|------|------|
| [`测试分层.md`](./测试分层.md) | code/module/system/pipeline | 说明验证层级和对象边界 |
| [`模块映射.md`](./模块映射.md) | 主模块归属 | 说明 `primaryModuleId`、`moduleIds`、`subsystemIds` |
| [`新增测试接入规范.md`](./新增测试接入规范.md) | 新增验证接入 | 说明重要验证如何升格为 case 或对象 |
| [`临时例外登记.md`](./临时例外登记.md) | 临时例外 | 说明事件驱动失效和登记规则 |

## 本层规则

- 项目级测试知识只能以本目录作为正式入口。
- 重要验证必须升格为正式 case、module、system 或 pipeline 对象，不能长期停留在临时手工步骤。
- registry 以 `tests/registry/**` 为运行时注册源，以本目录页面为说明和回退入口。
- skill 应优先读取 `artifacts/tests/registry/current/index.json` 或 `run test registry list --json`，再按 `docRefs` 回退到本目录页面。
- 完成前验证如果被认定为重要，必须补成正式 case 或对象，不能只停留在本次开发记录里。

## 最近更新

- `2026-04-06`：补入 `managed-closure` 的模块验证与 completion 管线对象，并把 Stage 3 closure bundle 验证接入统一 registry / `run test` 入口。
- `2026-04-04`：建立项目级测试与验证主入口，并接入 `module/system/pipeline` 注册对象。
