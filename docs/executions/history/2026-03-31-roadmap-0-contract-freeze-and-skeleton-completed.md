# 已完成：roadmap-0-contract-freeze-and-skeleton

- 计划名：`Roadmap 0: Contract Freeze And Skeleton`
- 状态：`completed`
- 完成日期：`2026-04-02`
- 设计文档：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/design-2026-04-01-03-roadmap-0-contract-freeze-and-skeleton.md`
- 计划文档：`docs/booming/2026-03-31-roadmap-0-contract-freeze-and-skeleton/plan-2026-04-01-03-roadmap-0-contract-freeze-and-skeleton.md`
- 隔离 worktree：`D:\agent\booming-il2cpp-roadmap-0-contract-freeze-and-skeleton`
- 分支：`roadmap-0-contract-freeze-and-skeleton`
- 最终 HEAD：`8f244aa`（`fix: harden roadmap0 verification gates`）

## 进度

- 总 Chunk 数：5
- 已完成：5
- 进行中：0
- 未开始：0

## 已确认上下文

- 必须按 5 个 Chunk 文档顺序执行，不能把后续 Chunk 的工作前拉。
- `Roadmap 0` 是重 contract、重验证的基线阶段，但不实现完整 runtime core，也不实现完整 codegen lowering。
- 顶层骨架必须包含 `frontend/`、`analysis/`、`runtime/`、`codegen/`、`build/`、`tests/`、`contracts/`、`third_party/` 和 `artifacts/`。
- `third_party/` 只存 vendored source；工具链、缓存和构建产物都不放这里。
- `artifacts/` 存构建输出、日志、运行时快照副本与 diff，不存 canonical snapshot。
- host-specific verification 按操作系统分流：Windows host 负责 `Windows + Android + Linux (cross-compile/packaging)`；macOS host 负责 `macOS + iOS + Linux (cross-compile/packaging)`。
- `Windows x64` 与 `macOS` 都是参考桌面目标，其中 Windows 是主要高层 contract baseline。

## 用户已确认决策

- 执行计划 `2026-03-31-roadmap-0-contract-freeze-and-skeleton`。
- 使用隔离 git worktree 执行，不污染带未提交改动的 `main` 工作区。
- `3rd/` 下的 vendored library 直接迁移到 `third_party/`，不保留长期有效的 legacy 位置。
- Chunk 3 的 native contract pack 类型/结构体命名统一去掉 `Boom` 前缀。
- Chunk 3 的 header 文件名统一去掉 `boom_` 前缀，改为 `runtime_abi.h` 与 `codegen_bridge.h`。

## 完成结果

- Chunk 1 已完成并接受：完成仓库骨架、`third_party/` / `artifacts/` 规则，以及 `3rd/scriban -> third_party/scriban` 的直接迁移。
- Chunk 2 已完成并接受：完成 6 份 schema、6 份最小 example、6 份 canonical snapshot，以及 warmup trace schema 与 Windows/macOS snapshot。
- Chunk 3 已完成并接受：完成 native ABI / bridge contract header、docs、examples、compile-only smoke，并同步命名与文件名规范修正。
- Chunk 4 已完成并接受：完成 5 个真实 smoke 输入项目、稳定输出、生命周期 guard 自检，以及统一的 `artifacts/smoke/bin` 构建落点。
- Chunk 5 已完成并接受：完成平台矩阵、toolchain preset、reference desktop gate、Android/iOS/Linux shell gate、trace compare、`verify-roadmap-0` 主线脚本与本地验证文档。
- Chunk 5 follow-up 修补已完成：`verify-roadmap-0.ps1` 现已校验 schema/example/snapshot 与 trace contract、运行全部 5 个 managed smoke，并修复 PowerShell 5.1 下的 cwd / 数组 / strict-mode 细节；`HostEmbeddingLite` 导出的 runtime trace 现已携带来自刚执行 session 的 provenance 与 per-event warmup marker；`compare-warmup-trace.ps1` 现已校验 `scenario` 与 event-to-sessionTrace 对应关系。

## 验证与复审

- `powershell -NonInteractive -File 'D:\agent\booming-il2cpp-roadmap-0-contract-freeze-and-skeleton\build\scripts\verify-roadmap-0.ps1' -HostProfile windows`：在最终状态下完整通过。
- 最终规格复审：无 actionable findings。
- 最终代码质量复审：先发现 2 个重要问题；修复后复审通过，no remaining actionable findings。

## 产出提交

- Chunk 4 接受提交：`8c38340`
- Chunk 5 主提交：`5a664cd`
- Chunk 5 follow-up 提交：`8f244aa`
- 备份分支 `roadmap-0-contract-freeze-and-skeleton_backup` 已推进到 `8f244aa`

## 后续

- 当前执行已归档完成。
- 后续分支集成方式交由 `finishing-a-development-branch` 流程决定：本地合并、创建 PR、保留分支，或放弃工作。
