# 进度记录 v1-13

## 时间

- 2026-04-06 12:07:05 +08:00

## 已完成

- 在 `build/scripts/verify-roadmap-0.py` 中新增 `validate_stage4_proof_run_artifacts`，把 Stage 4 proof run 的最小验证合同收口为脚本 helper。
- 在 `build/scripts/verify-roadmap-0.ps1` 中新增 `Assert-Stage4ProofRunArtifacts`，保持 Windows 低层脚本与 Python 入口对同一组 run artifacts 使用一致规则。
- 验证合同当前固定为：
  - `run/stdout.log` 必须包含 `Hello, World!`
  - `run/exit-code.txt` 必须记录 `0`
  - `run/stdout.log`、`run/stderr.log`、`run/exit-code.txt` 三个文件都必须存在
- 新增 `tests/tooling/run/test_verify_roadmap0.py` 回归覆盖通过路径、stdout 不匹配、exit code 不匹配，以及 PowerShell 脚本文本契约。

## 验证

- `python -m unittest tests.tooling.run.test_verify_roadmap0`
- `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- `python -m unittest tests.unit.run.test_native_prefix_naming`

## 未完成 / 阻塞

- 当前 helper 只收口了验证合同，还没有接入 `verify-roadmap-0` 的默认执行路径，因为本机仍缺少 `cmake` 与独立 C++ 编译器。
- 真实 `proof_run` target 的 configure/build/run 证据仍缺失，因此不能声称 Stage 4 proof gate 已经打开。

## 下一步

- 等工具链恢复后，将 `proof_run` target 与 `validate_stage4_proof_run_artifacts` / `Assert-Stage4ProofRunArtifacts` 接到真实 `verify-roadmap-0` Windows 分支，捕获第一批真实 build/link/run 失败信息。
- 若工具链短期仍不可用，则继续只在文件级收口 Stage 4 proof gate 的 build/run/verify graph，不提前打开默认 gate。
