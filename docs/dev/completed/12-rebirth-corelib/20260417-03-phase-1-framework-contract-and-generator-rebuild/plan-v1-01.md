# Phase 1 Framework Contract And Generator Rebuild 实施计划 v1.01

Date: 2026-04-17 13:30:09 +08:00
Status: in-progress

## 计划清单

- [x] Batch A: 对齐 collection schema version 与 Runtime loader contract
- [x] Batch B: 把 capability coverage 真源切到 `subject.features.json`
- [ ] Batch C: 重建 generated managed/native/hotupdate host contract
- [ ] Batch D: 把 generated host / project 的整文件输出切向模板边界
- [ ] Batch E: 收敛 collector responsibility，清理测试框架域与 IL2CPP 域的边界
- [ ] 更新父 roadmap / ACTIVE / INDEX / progress note，并为 Phase 2 准备输入

## 本阶段输出

- `docs/dev/in-progress/20260417-03-phase-1-framework-contract-and-generator-rebuild/design-v1-01.md`
- `docs/dev/in-progress/20260417-03-phase-1-framework-contract-and-generator-rebuild/STATUS.md`
- framework/runtime/collector/generator 相关代码改动
- 回归测试与阶段性验证记录

## 当前切入点

- 先收敛 generated host contract 的真实输入面
- 再把 file-level 生成从内联字符串拼接切向模板资产
- 最后整理 collector 责任边界，避免测试框架逻辑继续借壳在 IL2CPP 核心域中
