# 进展记录 v1-12

**时间** 2026-04-04 23:38 +08:00

## 本轮产出

- 把 `workflow/roadmap-0-windows` 与 `workflow/roadmap-0-macos` 升格为正式 `system` 对象：
  - 新增 `system/roadmap-0-windows`
  - 新增 `system/roadmap-0-macos`
- 补齐 `wiki/06-测试验证/整体验证场景/roadmap-0-runtime.md` 与对应 `INDEX.md`，把 roadmap-0 的正式 system 入口纳入唯一项目测试知识入口。
- 修正 Windows `verify roadmap-0` 的脚本启动方式：
  - 增加 `powershell -ExecutionPolicy Bypass`
  - 失败时把底层脚本输出带回 `console.log` / `text`，便于准确定位错误位置

## 已验证

- `python -m unittest tests.integration.run.test_registry_scan tests.tooling.run.test_registry_command`
- `python -m unittest tests.tooling.run.test_verify_roadmap0`
- `python -B build/toolchains/run/run.py test registry check-consistency --json`
- 在无沙箱环境下实际跑通：
  - `python -B build/toolchains/run/run.py test system --id system/roadmap-0-windows --json`

## 当前已知失败

- `python -B build/toolchains/run/run.py test system --id system/roadmap-0-windows --json`
- 当前失败点已经可准确定位到：
  - `artifacts/logs/tests/20260404-233751-windows-4b7a/console.log`
  - `build/scripts/verify-roadmap-0.ps1:199`
  - 失败原因为清理旧产物目录 `artifacts/verify-roadmap-0/windows/common/native-abi-config/ALL_BUILD.dir/Debug` 时访问被拒绝
- 这说明 `system` 注册与调度链路已通，当前剩余问题转为 Windows 本机产物清理/权限问题

## 追加结论

- 后续对同一命令进行无沙箱重跑后已通过，说明前述 `MSBuild/VCTargetsPath.tlog/unsuccessfulbuild` 删除失败主要是当前沙箱对子进程文件操作的限制，不是 `roadmap-0` registry/system 接入或脚本流程本身的仓库逻辑缺陷。
- 因此 `roadmap-0` 这条正式 system 接入链路可以收口，下一步应回到继续接入下一条仍停留在 suite/手工验证层的真实验证链路。

## 下一步

- 评估是否把 `roadmap-0` 的 Windows 产物清理逻辑做稳健化处理，避免旧目录权限或占用导致整条正式验证链路失败。
- 如果暂不收这个环境问题，则继续接入下一条仍停留在 suite/手工验证层的真实验证链路。
