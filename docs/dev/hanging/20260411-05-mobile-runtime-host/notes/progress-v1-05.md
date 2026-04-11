# 进展记录 v1-05

## 时间

- 2026-04-11 22:45:00 +08:00

## 本轮完成

- 扩展 public command `run doctor`：
  - 新增 Android SDK / NDK / `adb` / `emulator` 探测
  - 在非 macOS 宿主明确提示 iOS runtime 仍需要 macOS/Xcode
- 当前 Windows 宿主现在可以直接得到 Phase 4 恢复前的 mobile runtime host 环境阻塞清单。

## 验证结果

- `python -m pytest tests/tooling/run/test_doctor.py -v`
- `python -m pytest tests/tooling/run/test_prepare_scopes.py -v`
- `python build/toolchains/run/run.py doctor`

## 备注

- 本轮新增的是 blocker-reduction/preflight 能力，不是新的 Android/iOS runtime 证据。
- `run doctor` 只负责暴露环境缺口，不能替代真机 / emulator / simulator / device 的启动、crash 与 perf 验证。

## 下一步

- 在目标宿主补齐 `run doctor` 报出的 Android SDK/NDK/adb/emulator 缺口。
- 准备 macOS/Xcode 宿主后恢复 Phase 4，继续 Batch 2.4/2.5、3.4-3.7、4.2-4.4。
