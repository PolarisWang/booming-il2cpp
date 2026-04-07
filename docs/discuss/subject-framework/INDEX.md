# subject-framework

## 主题说明

- 这一组文档是新测试框架的实现级讨论主区。
- 覆盖 contract source、artifact placement、stage graph、manifest schema、planner / executor 边界与 cutover 顺序。

## 文档

- `20260406-07-analysis-contract-storage-strategy-v1-01.md`
  - `analysis/`、`contracts/` 与 `tests/contracts/` 的职责重划。
- `20260406-08-first-implementation-cutover-order-v1-01.md`
  - 第一批从设计进入实施时的切换顺序。
- `20260406-09-artifact-manifest-and-report-naming-v1-01.md`
  - 最小文件命名草案。
- `20260406-10-contract-source-cutover-design-v1-01.md`
  - `contracts.py` 切向 canonical `contracts/` 的实施设计。
- `20260406-11-stage-output-placement-design-v1-01.md`
  - 阶段产物与输出的正式落点。
- `20260406-12-stage-reuse-and-invalidation-design-v1-01.md`
  - 阶段复用、失效传播与 fingerprint。
- `20260406-13-stage-dependency-graph-design-v1-01.md`
  - stage dependency graph 的正式设计。
- `20260406-14-stage-dependency-graph-scenarios-v1-01.md`
  - `stage / dependency / graph` 的具体落地场景。
- `20260406-15-subject-manifest-schema-draft-v1-01.md`
  - `subject.manifest.json` 增量 schema 草案。
- `20260406-16-manifest-consumer-boundaries-v1-01.md`
  - manifest 字段的消费边界。
- `20260406-17-planner-output-design-v1-01.md`
  - planner 输出结构。
- `20260406-18-executor-stage-io-boundary-design-v1-01.md`
  - executor、worker、reporting 之间的 IO 边界。
