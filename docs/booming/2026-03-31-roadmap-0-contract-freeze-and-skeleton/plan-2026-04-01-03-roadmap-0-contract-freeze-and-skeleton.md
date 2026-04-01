# Roadmap 0：Contract Freeze And Skeleton 实现计划

> **For agentic workers:** REQUIRED: Use booming-code:subagent-driven-development (if subagents available) or booming-code:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**目标：** 把 `Roadmap 0` 的分段设计落成可执行、可验证、可按 host-specific matrix 运行的工程骨架、contract pack、smoke 输入与 verify gate。

**架构：** 本计划按 5 个实现 chunk 推进，先固定仓库骨架与规则，再冻结 schema/native/trace contract，随后落 5 个真实 smoke 输入项目，最后把平台矩阵和 `verify-roadmap-0` 串成硬 gate。计划默认这是一个“重 contract + 重验证入口”的 `Roadmap 0`，但仍禁止偷跑完整 runtime/codegen 实现。

**技术栈：** Markdown、JSON Schema、C/C++ 头文件、CMake、PowerShell、Shell、.NET SDK、Android NDK、Xcode/iOS toolchain 描述、snapshot/diff、host-specific verification scripts。

---

## 使用说明

- 先阅读设计入口文件：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/design-2026-04-01-03-roadmap-0-contract-freeze-and-skeleton.md`
- 按 Chunk 1 -> Chunk 5 顺序执行，不要跨 chunk 提前展开后续实现
- 每个 chunk 完成后先跑本 chunk 验证，再用 `@booming-code:requesting-code-review` 做检查
- 任何声称 `Roadmap 0` 完成的结论前，都必须补跑 `@booming-code:verification-before-completion`

## Chunk 索引

1. [Chunk 1：仓库骨架、第三方规则与产物目录](./plan-2026-04-01-03-01-repo-layout-and-rules-roadmap-0-contract-freeze-and-skeleton.md)
2. [Chunk 2：Schema Pack 与 Trace Contract](./plan-2026-04-01-03-02-schema-and-trace-contracts-roadmap-0-contract-freeze-and-skeleton.md)
3. [Chunk 3：Native Contract Pack（ABI / Bridge）](./plan-2026-04-01-03-03-native-contract-pack-roadmap-0-contract-freeze-and-skeleton.md)
4. [Chunk 4：Smoke 输入项目与子场景分配](./plan-2026-04-01-03-04-smoke-projects-roadmap-0-contract-freeze-and-skeleton.md)
5. [Chunk 5：平台矩阵、Harness 与 Verify Gate](./plan-2026-04-01-03-05-platform-gates-and-verify-roadmap-0-contract-freeze-and-skeleton.md)

## 全局约束

- 不在本计划中实现完整 runtime core
- 不在本计划中实现完整 codegen lowering
- 不把 Android/iOS 提前推到完整平台语义实现阶段
- 允许冻结重 contract、重 smoke、重 verification，但实现仍停在 `Roadmap 0`

## 完成标准

满足以下条件时，才算本计划全部完成：

- 顶层骨架、`third_party/` 规则、`artifacts/` 规则与 host-specific matrix 已落盘
- schema/example/snapshot 与 trace contract 成套存在
- native contract pack 的 header/docs/examples/compile-only smoke 成套存在
- 5 个 smoke 输入项目全部为真实可构建项目
- `verify-roadmap-0` 能按 Windows/macOS host-specific matrix 调度验证
- Windows 与 macOS 双 reference desktop 的高阶 contract gate 已被脚本化并有可对比输出

## 关联文档

- 设计入口：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/design-2026-04-01-03-roadmap-0-contract-freeze-and-skeleton.md`
- Brainstorm：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/brainstorm-2026-04-01-00-roadmap-0-contract-freeze-and-skeleton.md`
- Plan Discuss：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/plan-discuss-2026-04-01-03-roadmap-0-contract-freeze-and-skeleton.md`
- Finding：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/finding-2026-03-31-23-roadmap-0-contract-freeze-and-skeleton.md`

## 执行交接

计划已拆分为入口文件与 5 个 chunk 文件，可直接进入执行阶段。
