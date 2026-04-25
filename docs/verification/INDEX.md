# Verification Reports

> 面向审核者的稳定报告入口。当前优先查看 foundation DLL 翻译审计报告。

## 当前主入口

- [`foundation-dll-audit/dashboard.html`](./foundation-dll-audit/dashboard.html)
  - `System.Private.CoreLib + 13 DLL` 的总览页，包含整体进度、DLL 矩阵、DLL 详情入口与 Artifact Index
- [`foundation-dll-audit/summary.md`](./foundation-dll-audit/summary.md)
  - 适合快速查看当前完成数、阻塞数与活跃 DLL

## DLL 明细入口

- [`foundation-dll-audit/dlls/System.Private.CoreLib.html`](./foundation-dll-audit/dlls/System.Private.CoreLib.html)
  - CoreLib 的验证项目、验证方法、证据与支持引用
- 其他 DLL 明细页位于 [`foundation-dll-audit/dlls/`](./foundation-dll-audit/dlls/)

## 数据刷新

- 正式刷新命令：`python build/toolchains/run/run.py verify verification-v1 --json`
- 本目录下的报告是发布视图；正式投影仍同步输出到 `verification/projections/foundation-dll-audit/`

## 证据口径

- `artifacts/**` 下的真实产物才算 primary evidence
- `docs/**`、`subjects/**`、`verification/**` 只作为 support refs 展示，不直接驱动项目通过
