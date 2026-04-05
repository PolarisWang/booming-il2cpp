# Stage 1B Structure And Test Migration Plan

Date: 2026-04-05 22:40:00 +08:00
Status: ready-after-stage-1a

## 目标

把 reboot 设计中确认的 managed 分层和 tests 角色分层落成可执行工程骨架。

## Chunk 1：建立 `Chaos.IL2CPP.*` managed skeleton

- [ ] 建立 8 个 `Chaos.IL2CPP.*` 项目骨架
- [ ] 定义最小 project reference 规则
- [ ] 在 `Chaos.IL2CPP.Contracts` 放置 shared contract model 入口

## Chunk 2：重组 tests family

- [ ] 固定 `tests/contracts` 为 canonical contract family
- [ ] 固定 `tests/contracts/trace` 为 canonical trace 路径
- [ ] 新建 `tests/proof` 相关目录
- [ ] 保留 `tests/smoke/input/HelloWorld` 的 smoke 语义

## Chunk 3：迁移 tooling / manifest / integration 路径

- [ ] 盘点并迁移 `tests/contract/...` 的硬编码引用
- [ ] 切换 suite manifest、registry、run tooling 的 family/path 常量
- [ ] 对 compatibility 路径保留明确过渡策略

## Chunk 4：文档与状态同步

- [ ] 同步 wiki / design / progress / status
- [ ] 确认 parent roadmap 中的 Stage 1B 状态可从 planned 切到 in-progress

## 完成标准

- repo 目录结构不再在 contract / tests 语义上自相矛盾
- `tests/proof` 与 `tests/smoke` 已形成独立角色
- tooling 中的 canonical path 不再继续指向旧路径

