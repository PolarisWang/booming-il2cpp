# Phase 2 Business Pipeline Rebuild Design v1.01

Date: 2026-04-17
Status: in-progress

## 1. 目标

Phase 2 负责把业务消费管线切到 Phase 0/1 已落地的新权威模型：

- planner 不再把旧 manifest 测试字段当主真源
- registry / workspace / run pipeline 统一消费 collection + `subject.features.json` + 精简后的 subject.manifest
- dashboard / reporting 以 capability/scenario 单轨模型出报表

## 2. 前置完成

- Phase 0 已冻结单轨 truth source 与 canonical subject authority files
- Phase 1 已完成：
  - collection schema versioning
  - file-backed capability authority
  - generated host / native workspace 模板边界
  - collector 工具归位到 `src/tools/Chaos.TestFramework.CollectionGen`

## 3. 当前主问题

### 3.1 subject.manifest 仍然过载

当前仍需识别并剥离：

- 测试入口选择字段
- 历史 workload / validation 协议
- 与 dashboard / registry 直接耦合的旧字段

### 3.2 planner / registry / workspace 仍带历史结构假设

重点风险：

- 直接依赖旧 subject 路径与命名
- 仍通过 manifest 而不是 collection/feature authority 推导行为
- 业务层继续复制旧 contract

### 3.3 reporting / dashboard 还未完全 capability-first

需要进一步确认：

- capability vs scenario 的对照是否完全来自 authority + collection
- required-but-missing benchmark / hotupdate proof 是否能自动审计

## 4. 本阶段切入顺序

1. 盘点 planner / registry / workspace 仍消费的旧 manifest 测试字段
2. 用 contract test 锁住目标输入面
3. 逐步切断旧字段依赖
4. 收敛 dashboard / reporting 的 capability/scenario 消费模型

## 5. 验收标准

- planner / registry / workspace 不再依赖旧测试入口字段作为主真源
- dashboard/reporting 可以基于 authority + collection 回答 coverage 完整性问题
- 业务管线切换后，不引入新的双轨 compatibility 层
