# Phase 2 Declaration Metadata Discovery Batch 2 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。

目标：把 `Chaos.TestFramework` 从“声明 contract 已冻结”推进到“可对编译程序集做 metadata 扫描并产出 compiled catalog”，为后续 thin manifest、subject cutover 与统一 registry/catalog 铺路。

## 计划任务

- [ ] Task 1: 激活 Batch 2 执行任务
  - 范围:
    - 新建 `docs/dev/in-progress/20260412-10-phase-2-declaration-metadata-discovery-batch-2/`
    - 更新 `docs/dev/ACTIVE.md`
    - 更新父 roadmap 状态与索引
  - 验收:
    - `ACTIVE.md` 指向当前 batch
    - `docs/dev/INDEX.md` 与 `docs/dev/in-progress/INDEX.md` 可见当前 batch

- [ ] Task 2: 通过失败测试冻结 discovery contract
  - 范围:
    - 新增真实 managed fixture，分别覆盖:
      - 无 `Chaos.TestFramework` 引用
      - 有框架引用但无声明
      - 有真实 `ChaosUnitTest` / `ChaosBenchmark` 声明
    - 新增 integration tests，覆盖:
      - metadata 扫描不执行程序集
      - 三态区分
      - 稳定 ID 生成
      - compiled catalog 输出形状
  - 验收:
    - 新测试在实现前明确失败
    - 失败原因来自能力缺失，而不是测试自身错误

- [ ] Task 3: 实现 declaration metadata discovery 与 compiled catalog
  - 范围:
    - 新增 managed metadata discovery 工具项目
    - 使用 `System.Reflection.Metadata` 读取 custom attributes
    - 在 Python 侧实现 discovery wrapper 与 compiled catalog builder
    - 产出最小 catalog 字段:
      - `subjectId`
      - `frameworkReferenced`
      - `subjectKind`
      - `warningCodes`
      - `declaredUnitTests`
      - `declaredBenchmarks`
  - 验收:
    - 扫描仅依赖 metadata，不执行程序集
    - unit / benchmark 条目都带稳定 ID
    - `require / auto / none` 约束继续通过 `summarize_declaration_scan(...)` 统一收敛

- [ ] Task 4: 完成验证并归档本批次
  - 范围:
    - 运行定向 pytest
    - 运行 discovery tool 的 `dotnet build`
    - 运行全量 `python -m pytest -q`
    - 更新状态、进度记录与索引
  - 验收:
    - 本 batch 全部验证通过
    - 子任务可归档为 `completed`
    - 父 roadmap 下一步明确指向 Phase 2 后续 catalog/registry 接线或 Phase 3 schema 设计

## 验证

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.DeclarationDiscovery/Chaos.IL2CPP.DeclarationDiscovery.csproj -c Release`
- `python -m pytest -q`

## 执行备注

- 本批次优先解决“真实程序集 metadata 扫描”和“compiled catalog 最小模型”，不在本批次做完整 registry/selector/CLI 切换。
- discovery 输出的枚举相关字段尽量保持紧凑数值表示，避免引入新的字符串枚举协议。
- fixture 只用于冻结 contract，不代表最终 subject cutover 方案；真实 subject 迁移仍在后续 Phase 4-7 推进。
