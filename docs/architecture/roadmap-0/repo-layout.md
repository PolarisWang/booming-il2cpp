# Repo Layout (Roadmap 0)

## Scope

本文档描述的是 Roadmap 0 产品骨架与正式落点，不是整个仓库的排他性目录清单。

Roadmap 0 产品骨架目录：

- `frontend/`: 前端集成接线与入口适配。
- `analysis/`: 分析与语义处理能力。
- `runtime/`: 运行时与桥接执行能力。
- `codegen/`: 代码生成与目标输出组织。
- `build/`: 编译/链接/打包/cross-compile 编排。
- `tests/`: 验证入口、测试资产与 smoke 执行。
- `contracts/`: schema/native/trace 等契约定义。
- `third_party/`: vendored 源码正式落点（当前含 `third_party/scriban`）。
- `artifacts/`: 运行输出、日志、打包结果与快照副本。

仓库仍保留的非产品骨架目录（示例）：

- `docs/`: 设计、执行记录与架构文档。
- `wiki/`: 项目知识沉淀。
- `3rd/`: 历史目录，`scriban` 已迁移后不再作为当前依赖落点。

## Boundary Rules

- 目录间协作必须通过 `contracts/` 的显式契约，避免隐式耦合。
- `third_party/` 只存放 vendored 源码，不承载工具链或构建产物。
- 新增或维护中的第三方依赖统一落在 `third_party/`。
- `artifacts/` 只承载临时或可再生输出，权威 snapshot 不放在此目录。
- 平台验证门禁以 `host-verify-matrix.md` 与 `platform-gates.md` 为准。
