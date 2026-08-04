# runtime-baseline 命名空间迁移设计

## 1. 设计目标

把当前仍暴露为 `roadmap-0` 的 live surface 迁移为更贴近现状的正式命名：

- runtime 主线使用 `runtime-baseline`
- platform gate 文档使用 `platform-gates`
- `roadmap-0` 仅保留为兼容层和历史记录

## 2. canonical 命名映射

### 2.1 workflow / prepare / clean

- `prepare workflow roadmap-0 --host windows` -> `prepare workflow runtime-baseline --host windows`
- `prepare workflow roadmap-0 --host macos` -> `prepare workflow runtime-baseline --host macos`
- `clean --scope workflow-roadmap0-windows` -> `clean --scope workflow-runtime-baseline-windows`
- `clean --scope workflow-roadmap0-macos` -> `clean --scope workflow-runtime-baseline-macos`
- `test workflow roadmap-0-windows` -> `test workflow runtime-baseline-windows`
- `test workflow roadmap-0-macos` -> `test workflow runtime-baseline-macos`

### 2.2 system object

- `system/roadmap-0-windows` -> `system/runtime-baseline-windows`
- `system/roadmap-0-macos` -> `system/runtime-baseline-macos`
- `system/roadmap-0-android-startup-gate` -> `system/android-startup-gate`
- `system/roadmap-0-ios-packaging-gate` -> `system/ios-packaging-gate`
- `system/roadmap-0-linux-packaging-gate` -> `system/linux-packaging-gate`
- `system/roadmap-0-windows-reference-gate` -> `system/windows-reference-gate`
- `system/roadmap-0-macos-reference-gate` -> `system/macos-reference-gate`

### 2.3 low-level script / artifacts / docs

- `build/scripts/verify-roadmap-0.py` -> `build/scripts/verify-runtime-baseline.py`
- `build/scripts/verify-roadmap-0.ps1` -> `build/scripts/verify-runtime-baseline.ps1`
- `build/scripts/verify-roadmap-0.sh` -> `build/scripts/verify-runtime-baseline.sh`
- `artifacts/verify-roadmap-0/**` -> `artifacts/verify-runtime-baseline/**`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/` -> `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/runtime-baseline/`
- `wiki/06-测试验证/整体验证场景/roadmap-0-runtime.md` -> `runtime-baseline.md`
- `wiki/06-测试验证/整体验证场景/roadmap-0-platform-gates.md` -> `platform-gates.md`

## 3. compatibility 规则

### 3.1 保留但降级的入口

- `prepare workflow roadmap-0 --host <host>` 仍可解析，但内部转到 `runtime-baseline`
- `test workflow roadmap-0-<host>` 仍可解析，但内部转到新的 system object
- `run test system --id system/roadmap-0-*` 仍可解析，但内部映射到新 object id
- `build/scripts/verify-roadmap-0.*` 保留为 wrapper，内部调用新的 `verify-runtime-baseline.*`

### 3.2 不再作为 canonical 暴露

- registry list 不再把 `system/roadmap-0-*` 作为正式对象列出
- public suite list 不再把 `workflow/roadmap-0-*` 作为正式 suite 列出
- 新文档、menu、tui、manifest 示例统一只展示 `runtime-baseline`

## 4. 非目标

- 不重写 `docs/dev/completed/**`、`docs/dev/abandoned/**` 等历史归档正文
- 不清理 `docs/discuss/**` 下历史 `roadmap-0` 讨论
- 不改变现有 gate suite 名称，如 `gate/windows-reference-desktop`

## 5. 风险与控制

- 风险：旧脚本 / 旧命令直接失效。
  - 控制：保留 alias / wrapper。
- 风险：registry id 变化导致测试对象和 pipeline 断链。
  - 控制：先改失败测试，再改 manifest / registry / dispatch。
- 风险：artifact 路径变更导致 TUI 和 prepare/clean 路由丢失。
  - 控制：同步改 path recognizer 与 clean scope。
