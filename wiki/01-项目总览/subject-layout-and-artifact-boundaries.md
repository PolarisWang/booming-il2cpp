# Subject 布局与产物边界

> 本页补充 `src/`、`tests/`、`subjects/`、`contracts/` 与 `artifacts/subjects/` 的正式分工。

## 正式边界

- `src/` 只保留 il2cpp 核心实现，不放 subject 源码、subject unit test、perf harness 或 native proof host
- `tests/` 只保留通用测试基础设施与通用验证资产，例如 tooling、registry、contract trace 对比脚本、平台 gate
- `subjects/<subject-id>/` 是测试工程正式根目录，承载 `source/`、`validation/`、`expected/`、`baselines/`
- `contracts/` 是正式契约根目录，artifact/native schema、sample、snapshot 与文档都应落在这里
- `artifacts/subjects/<subject-id>/runs/<run-id>/...` 是 subject 流程的正式中间产物根
- Windows native `cmake` scratch binary dir 允许临时落到系统 `%TEMP%/chaos-cmake-*`；它不是正式产物、不是 contract 可见路径，也不应被测试源码或 wiki 当作 canonical 根

## `tests/` 与 `subjects/` 的分界线

- 属于通用运行框架、公共校验逻辑、公共 contract 对比脚本的内容，进 `tests/`
- 属于某个 subject 自己的源码、unit/perf/proof 验证代码、expected、baseline 的内容，进 `subjects/<subject-id>/`
- 只要某段内容会让 `tests/` 对某个具体 subject 产生直接构建依赖，这段内容就放错了

## 正式产物路径

给定 `subject-id`、`run-id`、`matrix-id`：

- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/source/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/host-input/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/generated/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/build/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/runtime/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/validations/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/pipeline-report/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/subject-report/`
- `artifacts/subjects/<subject-id>/runs/<run-id>/run-report/`

## 当前活动迁移点

- Windows native reference proof 的活动宿主已经切到 `subjects/HelloWorldObject/validation/proof/native-reference/`
- `tests/proof/**` 下的 subject 资产已经移除
- `tests/smoke/input/**` 只能承载通用 smoke 输入，不能再承载 subject 专属内容
- Windows subject native build 的 scratch `cmake` 目录已经从仓库内构建目录抽离到系统临时目录，避免把临时锁文件和中间缓存误当成 subject 正式证据
