# runtime-baseline 命名空间迁移实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划，并按 TDD 先补失败测试，再推进实现。
**目标：** 把 live canonical `roadmap-0` 命名空间迁移到 `runtime-baseline` / `platform-gates`，同时保留必要兼容入口。
**架构：** 先锁定新 canonical 名称与旧入口兼容行为，再迁移 run manifest、registry、low-level verify 脚本、artifact 路径与 live docs，最后跑聚焦回归并归档任务。
**技术栈：** Python、PowerShell、JSON manifest、registry manifests、wiki/docs、unittest。
**设计文档：** `docs/dev/in-progress/20260408-02-runtime-baseline-namespace-migration/design-v1-01.md`
**预期知识沉淀：** `wiki/04-工具与集成/统一入口脚本.md`、`wiki/06-测试验证/整体验证场景/`

## 目标文件与职责

- `build/toolchains/run/run_manifest.json`
  - 注册新的 canonical `runtime-baseline` 命令与旧 alias。
- `build/toolchains/run/run.py`
  - 更新 removed-command migration guidance。
- `build/toolchains/run/commands/prepare.py`
  - 迁移 prepare/clean scope 与 artifact root。
- `build/toolchains/run/commands/verify.py`
  - 改用新的 low-level script 与 artifact 路径，并保留旧 system-id 兼容映射。
- `build/toolchains/run/menu.py`
  - 更新菜单文案。
- `build/toolchains/run/tui.py`
  - 更新 TUI 文案、路径识别与进度显示。
- `build/toolchains/run/testing/public_specs.py`
  - 更新 workflow canonical suite 与 module/doc refs。
- `build/toolchains/run/testing/registry.py`
  - 如有需要，补 registry object compatibility 解析。
- `build/scripts/verify-runtime-baseline.py`
  - 新 canonical low-level verify 脚本。
- `build/scripts/verify-runtime-baseline.ps1`
  - 新 canonical PowerShell wrapper。
- `build/scripts/verify-runtime-baseline.sh`
  - 新 canonical shell wrapper。
- `build/scripts/verify-roadmap-0.py`
  - 兼容 wrapper。
- `build/scripts/verify-roadmap-0.ps1`
  - 兼容 wrapper。
- `build/scripts/verify-roadmap-0.sh`
  - 兼容 wrapper。
- `tests/registry/system/**`
  - 迁移 live system scenario 目录和 manifest。
- `docs/architecture/runtime-baseline/**`
  - 新 canonical 架构文档目录。
- `wiki/06-测试验证/整体验证场景/**`
  - 迁移 runtime 与 gate guide。

## Chunk 1：任务与 RED

- [ ] 创建并更新 `docs/dev/ACTIVE.md`、`STATUS.md`、`notes/progress-v1-01.md`
- [ ] 先修改失败测试，锁定新的 canonical surface：
  - `runtime-baseline-windows|macos`
  - `system/runtime-baseline-*`
  - `system/android-startup-gate` 等新 gate id
  - `artifacts/verify-runtime-baseline/**`
  - `docs/architecture/runtime-baseline/**`
- [ ] 为旧入口补兼容断言：
  - `prepare workflow roadmap-0 --host <host>` 仍可解析
  - `test workflow roadmap-0-<host>` 仍可解析
  - `test system --id system/roadmap-0-*` 仍可映射到新对象
  - `verify-roadmap-0.*` 仍可调用

## Chunk 2：命令与 registry canonical 迁移

- [ ] 更新 `run_manifest.json`
- [ ] 更新 `public_specs.py`
- [ ] 更新 live `tests/registry/system/**`
- [ ] 更新 `run.py` removed-command migration guidance
- [ ] 更新 `commands/test.py` 或 `testing/registry.py` 中的 legacy object-id 映射

## Chunk 3：low-level verify / artifact / prepare-clean 迁移

- [ ] 新增 `verify-runtime-baseline.*`
- [ ] 旧 `verify-roadmap-0.*` 改为 wrapper
- [ ] `commands/verify.py` 指向新 script
- [ ] `prepare.py`、`tui.py`、相关 tests 切到 `artifacts/verify-runtime-baseline/**`
- [ ] clean scope 迁到 `workflow-runtime-baseline-*`

## Chunk 4：文档与 live knowledge

- [ ] 迁移 `docs/architecture/roadmap-0/` 到 `docs/architecture/runtime-baseline/`
- [ ] 更新 `docs/architecture/INDEX.md`
- [ ] 新增 / 更新 `wiki/06-测试验证/整体验证场景/runtime-baseline.md`
- [ ] 新增 / 更新 `wiki/06-测试验证/整体验证场景/platform-gates.md`
- [ ] 更新 `wiki/06-测试验证/整体验证场景/INDEX.md`
- [ ] 更新 `wiki/04-工具与集成/统一入口脚本.md`

## Chunk 5：回归与收尾

- [ ] 运行受影响 unittest 批次
- [ ] 运行 `test registry list --json` / `test registry check --json`
- [ ] 如环境允许，运行新的 low-level verify 相关测试
- [ ] 更新 `STATUS.md`、`notes/progress-*.md`、`docs/dev` 索引
- [ ] 归档到 `docs/dev/completed/`

## 验证命令

- [ ] `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_prepare_scopes tests.tooling.run.test_unified_test_command tests.tooling.run.test_registry_command tests.tooling.run.test_tui tests.tooling.run.test_verify_roadmap0 tests.unit.run.test_repo_layout`
- [ ] `python build/toolchains/run/run.py test registry list --json`
- [ ] `python build/toolchains/run/run.py test registry check --json`

## 完成标准

- [ ] live public surface 不再把 `roadmap-0` 当作 canonical 名字
- [ ] 新 canonical 命名统一为 `runtime-baseline` / `platform-gates`
- [ ] 旧命令、旧 system-id、旧 verify wrapper 仍可兼容
- [ ] live docs / wiki / registry / artifact 路径全部一致
- [ ] 聚焦回归通过
