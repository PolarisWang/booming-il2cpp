# Verification V1 测试流程规范

## 目的

本文档定义新的正式测试与验证流程。它�?`Verification V1` �?`wiki/06-测试验证/` 下的长期 canonical 入口，用来替代旧�?run-oriented 测试流程描述�?
架构 authority �?[`../../docs/architecture/verification-v1/spec.md`](../../docs/architecture/verification-v1/spec.md) �?[`../../docs/architecture/verification-v1/schema.md`](../../docs/architecture/verification-v1/schema.md) 为准；本文档负责把这些规则收敛成长期流程规范�?
## 默认入口

- 刷新并校�?`verification-v1` 正式产物：`run verify verification-v1 --json`
- `test inventory` 只保留为内部实现细节，不作为公开主入�?
- 如果本轮改动触及 formal report / projection contract，例如 `Program / DLL / Verification Project / Artifact` 报告对象、`latest/master/reports` 字段、projection 页面字段或证据链接规则，则不能只改 schema、模板或页面读取逻辑；formal object 通过后仍必须执行 `run verify verification-v1 --json` 刷新正式数据
## 正式主线

新的正式测试流程固定为：

```text
Closure
  -> ObligationClaim
  -> EvidenceClaim
  -> Execution Facts
  -> latest/master/reports
  -> testing-inventory / benchmark / dashboard
```

执行主线仍依托：

```text
managed solution
  -> dotnet 8 collection analysis
  -> collection files
  -> managed test host
  -> native test host
  -> hotupdate patch + host
```

但正�?authority 不再是“跑完某�?pipeline 就算完成”，而是“closure 下的 claim �?evidence 是否闭合”�?
## Formal Source

新的 formal source 固定只有以下几类�?
- `verification/archive/latest/`
- `verification/archive/master/`
- `verification/archive/reports/`
- `verification/evidence/owners/<OwnerPack>/codegen-stubs/`

`artifacts/` 只放临时过程产物，不是 formal source。
benchmark 补充规则：
- `benchmark --record` 只写 raw benchmark records，不直接刷新 `verification/archive/*` 或 formal projections。
- 需要新的 benchmark merged data、dashboard 或 archive summary 时，必须后续执行 `run verify verification-v1 --json`。
- 命中 report / projection contract 时，还必须确认对应 `verification/projections/**` 派生产物已从新的 formal source 重生成。

## 覆盖模型

新的覆盖模型固定为：

```text
capability x route x platform x deviceProfile x evidenceKind
```

其中�?
- `capability`
  - 表示验证对象
- `route`
  - `managed / aot / hotupdate`
- `platform`
  - 宿主或目标平�?- `deviceProfile`
  - 设备 profile，而不是物理设�?id
- `evidenceKind`
  - `semantic / contract / lifecycle / perf / packaging`

所有测试都必须能映射到这个模型中，不能只以目录或脚本名存在�?
## 状态语�?
叶子层验证状态固定为�?
- `passed`
- `failed`
- `blocked`
- `missing`
- `planned`
- `not_required`

支持状态独立表达为�?
- `supported`
- `unsupported`
- `not_applicable`

`partial` 只允许出现在聚合层�?
## 新流程如何覆盖所有测�?
### correctness

- declared unit test / proof
  - 进入 `semantic`
- collection / manifest / ABI / binding
  - 进入 `contract`
- load / bind / activate / rollback
  - 进入 `lifecycle`
- package / version / integrity
  - 进入 `packaging`

### performance

- declared benchmark
  - 进入 `perf`
- 行粒度固定为�?  - `一行一�?declared-benchmark x device-profile`

### owner

所�?capability 都必须回�?owner subject�?
- `SolutionCorePack`
- `MixedExecutionFeaturePack`
- `HotUpdateHostPack`

## 首页与投影视�?
默认固定只展示：

1. `Closure Strip`
2. `Responsibility View`
3. `Route Topology View`
4. `Platform-Device Matrix`
5. `Detail`

首页不默认展�?scenario 和物�?device�?
下游投影视图固定为：

- `verification/projections/testing-inventory/unit-test-inventory.*`
  - 一行一个方�?x route/stage
- `verification/projections/testing-inventory/benchmark-inventory.*`
  - 一行一�?declared-benchmark x device-profile
- `verification/projections/testing-inventory/capability-inventory.*`
  - 一行一�?capability x closure
- `verification/projections/benchmark/*`
  - perf 视角的投影视�?
## 与旧测试流程的关�?
旧测试流程的核心问题是：

- formal authority 与页面聚合混在一�?- run-oriented 路径会持续膨胀
- success/failure/blocked/missing/unsupported 混用
- inventory / benchmark / dashboard 各自维护一套口�?
因此新的长期规则是：

- �?`pipeline/*` 页面不再作为长期 authority
- �?run-oriented formal 路径不再作为正式归档入口
- inventory / benchmark / dashboard 只能从新�?formal source 派生

## 旧流程清理规�?
�?`Verification V1` 完成 cutover 后，必须清理�?
- �?run-oriented formal report 写入逻辑
- �?`pipeline/*` 主入�?- �?inventory / benchmark 自行聚合 authority 的逻辑
- 仍然把旧测试流程当正式入口的 wiki 导航

清理前提是：

- �?formal source 已稳�?- �?projection 已接管页面入�?- �?wiki 规范已成为主入口

## 阅读路径

如果你要看：

- 总体架构
  - �?[`../../docs/architecture/verification-v1/spec.md`](../../docs/architecture/verification-v1/spec.md)
- formal schema
  - �?[`../../docs/architecture/verification-v1/schema.md`](../../docs/architecture/verification-v1/schema.md)
- 权责图审�?  - �?[`../../docs/architecture/verification-v1/ownership-map.md`](../../docs/architecture/verification-v1/ownership-map.md)
- 切换顺序
  - �?[`../../docs/architecture/verification-v1/roadmap-v1-01.md`](../../docs/architecture/verification-v1/roadmap-v1-01.md)

## 长期约束

- 不使�?`run-id` 作为 formal 归档目录
- `artifacts/` 只放临时过程产物
- `supportState` �?`verificationState` 必须分离
- `failed / blocked / missing / unsupported` 不得混写
- `AOT / IL2CPP / codegen / contracts / loader / test governance` 六条主线必须进入权责图审�?
- 只要 formal report / projection contract 发生变化，就必须刷新 `verification/archive/{latest,master,reports}` 与对应 `verification/projections/**`；不允许只改页面或 schema 而不刷新正式数据

