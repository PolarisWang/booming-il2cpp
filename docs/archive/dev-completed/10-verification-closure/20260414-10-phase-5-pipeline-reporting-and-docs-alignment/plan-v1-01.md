# Phase 5 Pipeline Reporting And Docs Alignment 实现计划

> 面向执行 Agent：继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。  
> 目标：让 planner、registry、dashboard、reporting 与 architecture docs 真正消费 capability / archetype 模型，并统一 managed baseline 与 unsupported / partial support 的展示语义。

## 目标文件

- `build/toolchains/run/commands/benchmark.py`
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
- `build/toolchains/run/testing/perf_dashboard.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/testing/declarations.py`
- `build/toolchains/run/testing/declared_metadata_labels.py`
- `build/toolchains/run/testing/templates/benchmark-dashboard.html`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/il2cpp-hotupdate-capability-matrix.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/solution-archetype-matrix.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/reference-assembly-and-corelib-policy.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/full-project-hotupdate-workflow.md`
- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/unsupported-feature-policy.md`
- `tests/integration/registry/test_registry_scan.py`
- `tests/unit/performance/test_benchmark_dashboard_generator.py`
- `tests/unit/performance/test_benchmark_command.py`
- `tests/unit/reporting/test_subject_reporting.py`
- `tests/unit/selection/test_selectors.py`

## 执行步骤

- [x] 审计 planner / registry / dashboard / reporting / docs 的现有脏改动，明确残留的旧 subject / 字符串协议耦合点
- [x] 对齐 registry / declaration label / reporting 输出，使 capability family、capability item、archetype 成为稳定消费字段
- [x] 调整 benchmark dashboard 数据模型与页面布局，突出 managed baseline、capability family 汇总与 solution / case drill-down
- [x] 统一 benchmark 命令与报表生成逻辑，修正 unsupported / partial support / missing data 的输出语义
- [x] 更新 architecture 文档，使 subject authoring、reference/corelib policy、hot-update workflow 与 capability matrix 的表述与代码一致
- [x] 跑目标回归并收口状态文档，返回父 roadmap

## 验证

- `python -m pytest tests/integration/registry/test_registry_scan.py -q`
  - 结果：`10 passed`
- `python -m pytest tests/unit/performance/test_benchmark_dashboard_generator.py -q`
  - 结果：`6 passed`
- `python -m pytest tests/unit/performance/test_benchmark_command.py -q`
  - 结果：`11 passed`
- `python -m pytest tests/unit/reporting/test_subject_reporting.py -q`
  - 结果：`6 passed`
- `python -m pytest tests/unit/selection/test_selectors.py -q`
  - 结果：`2 passed`

## 执行备注

- 本阶段优先消灭消费层的旧 subject / 字符串协议耦合，不新增新的顶层 subject。
- benchmark dashboard 默认以 managed baseline 为基线；native / interpreter / hot-update 等结果都围绕这个基线展开。
- 页面先突出关键信息，再展开细粒度明细，避免只展示难以定位问题的粗粒度聚合结果。
