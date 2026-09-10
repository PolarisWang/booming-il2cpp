# Phase 3 Progress 2026-04-14 18:37:13 +08:00

- 完成 `run test` / registry 对 workspace manifest v2 与 generated host 的主链切换。
- benchmark command 与 dashboard 已统一消费 workspace declared catalog，并支持 `entryIndex` 透传。
- 删除 registry-object compatibility alias、legacy run shim 与 dead legacy dispatch code。
- 修复 loader 缺失 `conv.i` / `conv.u` 导致的 native-aot workload bundle 失败。
- 验证：`python -m pytest` 全量通过。
