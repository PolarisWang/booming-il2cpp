# 进度记录 v1-07

## 时间

- 2026-04-06 10:47:46 +08:00

## 已完成

- 将 live run/tooling 相关环境变量前缀统一切换为 `CHAOS_RUN_*`，同步更新 `run.cmd`、`run.ps1`、`run.sh`、`build/toolchains/run/` 与对应 integration/tooling 测试。
- 将当前仍存活的 smoke/platform 导出符号统一切换为 `chaos_*`，覆盖 `PInvokeLite` smoke 导出、`verify-roadmap-0` 预期模式，以及 Android/iOS/Linux 的 gate/platform smoke 入口。
- 扩展 `tests/unit/run/test_native_prefix_naming.py` 的扫描范围，把 runner scripts、tooling/integration run 测试、platform/gate smoke 与相关 in-progress 文档纳入回归保护。
- 清理两处 in-progress 文档中的旧命名前缀直写，确保当前活跃文档面也不再回流旧 BOOM/Boom 系命名。

## 验证

- `python -m unittest tests.unit.run.test_native_prefix_naming`
- `powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/tooling/run/test_wrapper_bootstrap.ps1`
- `powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/tooling/run/test_run_cli.ps1`
- `powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/tooling/run/test_windows_terminal_handoff.ps1`
- `powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/integration/run/test_wrapper_bootstrap.ps1`
- `powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/integration/run/test_run_cli.ps1`
- `python -m unittest tests.unit.run.test_stage4_native_reference_proof`

## 未完成 / 阻塞

- `python -m unittest tests.tooling.run.test_wrapper_bootstrap_unix` 在当前 Windows 环境下因标准库缺少 `termios` 而无法运行，这一项仍需在 Unix 环境补验。
- `cmake` 与可用 C++ 编译器仍不可用，因此真实 native 编译验证依旧阻塞。

## 下一步

- 若工具链恢复，优先对 `src/native/runtime-core` 做真实编译验证。
- 若继续在当前环境推进，则进入 Chunk 4 的 `src/native/bootstrap/` 与 `src/native/support/` 骨架实现。
