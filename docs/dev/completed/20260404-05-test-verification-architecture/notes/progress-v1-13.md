# 进展记录 v1-13

**时间** 2026-04-05 00:11 +08:00

## 本轮产出

- 把 `linux-x64-packaging` 这条 platform gate 正式接入统一测试体系：
  - 新增 public suite `gate/linux-x64-packaging`
  - 新增 system 对象 `system/roadmap-0-linux-packaging-gate`
- 补齐项目 wiki：
  - `wiki/06-测试验证/整体验证场景/roadmap-0-platform-gates.md`
  - `wiki/06-测试验证/整体验证场景/INDEX.md`
- 修正 `build platform` 旧实现里依赖“先删除旧 binary dir”的问题：
  - `build/toolchains/run/commands/build.py` 中 platform gate 改为运行期唯一 binary dir
  - `build` 失败文本现在会透出底层输出，便于错误定位

## 已验证

- `python -m unittest tests.tooling.run.test_platform_gate_validate_only`
- `python -m unittest tests.integration.run.test_unified_test_command tests.integration.run.test_registry_scan tests.tooling.run.test_registry_command`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- 在无沙箱环境下实际跑通：
  - `python -B build/toolchains/run/run.py test system --id system/roadmap-0-linux-packaging-gate --json`

## 结论

- `linux-x64-packaging` 不再只是 `build platform ...` 的隐式平台校验，而是有正式 suite/system 对象、registry 入口和 wiki 回退页的项目级验证链路。
- 当前沙箱内运行仍可能因宿主构建环境限制导致失败，但无沙箱环境下该链路已通过，说明仓库侧对象接入与调度主线可以收口。

## 下一步

- 继续挑选下一条仍停留在隐式 gate / workflow 内部步骤、但已经具备稳定项目规则的真实验证链路，优先考虑：
  - Windows `android-startup-smoke`
  - macOS `ios-arm64-packaging`
  - reference desktop gate（windows / macos）
