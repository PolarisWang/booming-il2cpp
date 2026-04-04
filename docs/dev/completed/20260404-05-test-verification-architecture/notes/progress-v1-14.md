# 进展记录 v1-14

**时间** 2026-04-05 00:16 +08:00

## 本轮产出

- 把 `android-startup-smoke` 这条隐式 platform gate 接入统一测试体系：
  - 新增 public suite `gate/android-arm64-smoke`
  - 新增 system 对象 `system/roadmap-0-android-startup-gate`
- 扩展 `wiki/06-测试验证/整体验证场景/roadmap-0-platform-gates.md`，把 Android 与 Linux 两条 platform gate 放进同一正式说明页。
- 更新 `wiki/06-测试验证/整体验证场景/INDEX.md`，新增 Android startup gate 正式入口。

## 已验证

- `python -m unittest tests.integration.run.test_unified_test_command tests.integration.run.test_registry_scan tests.tooling.run.test_registry_command`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- 在无沙箱环境下实际跑通：
  - `python -B build/toolchains/run/run.py test system --id system/roadmap-0-android-startup-gate --json`

## 结论

- `android-startup-smoke` 不再只是 `build platform android-arm64-smoke` 的隐式步骤，而是有正式 suite/system 对象、registry 入口和 wiki 回退页的项目级验证链路。
- 当前沙箱内运行仍可能因宿主构建环境限制导致 `cmake routing configure failed`，但无沙箱环境下该链路已通过，说明仓库侧对象接入与调度主线可以收口。

## 下一步

- 继续接入下一条仍停留在隐式 gate / workflow 内部步骤、但已经具备稳定项目规则的真实验证链路，优先处理 `ios-arm64-packaging`。
