# Local Verification (Roadmap 0)

> 项目级测试对象、模块验证、整体验证和测试管线的正式知识入口统一位于 [`wiki/06-测试验证/INDEX.md`](../../../wiki/06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/INDEX.md)。本页只说明 `Roadmap 0` 本地执行入口。

## 推荐入口

- Windows host

```powershell
./run.ps1 doctor --json
./run.ps1 bootstrap --yes --json
./run.ps1 prepare workflow roadmap-0 --host windows --json
./run.ps1 test workflow roadmap-0-windows --json
./run.ps1 test pipeline --id pipeline/completion-runtime-core --json
./run.ps1 clean --scope workflow-roadmap0-windows --json
```

- macOS host

```bash
./run.sh doctor --json
./run.sh bootstrap --yes --json
./run.sh prepare workflow roadmap-0 --host macos --json
./run.sh test workflow roadmap-0-macos --json
./run.sh test pipeline --id pipeline/completion-runtime-core --json
./run.sh clean --scope workflow-roadmap0-macos --json
```

## `common` 检查

- `cmake --list-presets`
- `contracts/artifacts/v0/schemas/*.json`
- `contracts/artifacts/v0/samples/*.json`
- `contracts/artifacts/v0/snapshots/*.json`
- `CMakePresets.json`
- `tests/contracts/trace/**/*.json`
- analysis schema/example/snapshot 契约校验
- trace schema/snapshot 契约校验
- native ABI compile-only smoke
- native bridge compile-only smoke

## 说明

`Roadmap 0` 对 Android、iOS、Linux 的要求是“矩阵可调度、shell 可落盘、边界可追责”，不是在这个阶段完成完整平台 runtime 闭环。
