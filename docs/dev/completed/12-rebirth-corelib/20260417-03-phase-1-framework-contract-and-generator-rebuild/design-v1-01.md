# Phase 1 Framework Contract And Generator Rebuild Design v1.01

Date: 2026-04-17
Status: in-progress

## 1. 目标

Phase 1 负责把全量重生路线中的 framework contract 与 generator boundary 真正切到单轨：

- collection 成为 Python / Runtime / generated hosts 共用的 versioned contract
- `subject.features.json` 成为 capability authority 的正式消费源
- generated managed/native/hotupdate hosts 只消费 collection + manifest 分层信息
- file-level codegen 开始从内联字符串拼接切向模板资产
- collector 责任边界从 IL2CPP 核心域剥离，回到测试框架域

## 2. 已完成输入

### 2.1 Batch A: collection contract 对齐

已完成：

- `compiled_catalog.py` 输出 `schemaVersion`
- `ChaosTestCollection` 补齐：
  - `SchemaVersion`
  - `SubjectId`
  - `FrameworkReferenced`
  - `SubjectKind`
  - `WarningCodes`
- `ChaosTestCollectionLoader` 增加 schema version 校验

### 2.2 Batch B: capability authority 切换

已完成：

- 3 个 canonical subject 的 `subject.features.json` 补齐 owner capability entries
- `capability_coverage.py` 改为消费 `subjects/*/subject.features.json`
- 新增文件 authority 红测，锁定不再退回硬编码表

## 3. 剩余设计入口

### 3.1 Generated Host Contract Rebuild

需要完成：

- managed/native/hotupdate host 的输入 contract 统一
- host 不再从旧 manifest 字段和历史 subject 布局推导测试入口
- host generation 的 file-level 输出开始收敛到模板层

### 3.2 Template Asset Cutover

需要完成：

- 梳理当前 generated host / project 生成点
- 优先迁移整文件输出的入口模板
- 为后续 Phase 2/3 的 subject 重生留下稳定模板边界

### 3.3 Collector Boundary Cleanup

需要完成：

- 明确 `.NET 8 collector` 属于测试框架域，而不是 IL2CPP Core IR / planner / emitter
- 逐步把测试声明收集职责从 `Chaos.IL2CPP.DeclarationDiscovery` 的历史命名和边界中剥离出来
- 保持 subject 只引用 `Chaos.TestFramework.Sdk`

## 4. 结构约束

- 不把测试框架逻辑混进 IL2CPP Core IR / planner / emitter
- 不新增 compatibility alias
- 不恢复手工 entry 注册
- 不恢复 `Program::Main + Console.WriteLine` correctness 路线
- 文件级生成优先模板化，为后续 Scriban 统一迁移铺路

## 5. 验收标准

Phase 1 结束时必须满足：

- collection contract 已被 Python / Runtime / generated host 共用
- capability coverage 不再依赖硬编码 owner/support matrix
- generated hosts 的关键整文件输出具备明确模板边界
- collector 的职责归属与测试框架域边界清晰
- 回归能证明 registry / workspace / dashboard 持续消费新 contract
