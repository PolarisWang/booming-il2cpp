# contract source cutover 设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

本文件把“contract source 从 `analysis/contracts` 切到 canonical `contracts/`”收敛成可实施设计。

本轮关注的是：

- `build/toolchains/run/testing/contracts.py` 怎么切
- 现有测试怎么跟着调
- `analysis/contracts` 何时退役

## 2. 结论

推荐采用：

- 路径立即切换
- API 名称首批暂不切换
- mirror 首批保留但冻结
- 新增实现一律禁止再读 `analysis/contracts`

也就是：

- `analysis_contract_json_paths()` 这个函数名先保留
- 但它的实现改为读取 canonical source：
  - `contracts/artifacts/v0/schemas`
  - `contracts/examples/v0/artifacts`
  - `tests/contracts/schema`

这样做的原因是：

- 可以立刻把读取路径拉回正确位置
- 但不会在同一轮引爆过多调用点和测试名变更

## 3. 为什么不建议首批连函数名一起改

这里有两个方案。

### 方案 A：路径和函数名一起改

例如：

- `analysis_contract_json_paths` -> `artifact_contract_json_paths`
- `validate_analysis_contracts` -> `validate_artifact_contracts`

优点：

- 语义更干净
- 名字和 canonical source 一致

缺点：

- 这一轮 blast radius 更大
- `commands/test.py`
- `verify-roadmap-0.py`
- `tests/tooling/run/*`
- 以及后续 subject cutover 分支

都会同时跟着改

### 方案 B：先只改实现路径，函数名暂留

优点：

- blast radius 最小
- 能先完成真正重要的“source 切换”
- 后续再统一 rename 更安全

缺点：

- 短期内名字和真实语义不完全一致

推荐结论：

- 采用方案 B

因为当前最重要的是把 canonical source 切换成功，而不是优先追求 helper 名称完美。

## 4. 首批应固定的新读取路径

## 4.1 analysis artifact contract

应固定为：

- schema root: `contracts/artifacts/v0/schemas`
- example root: `contracts/examples/v0/artifacts`
- snapshot root: `tests/contracts/schema`

依据：

- canonical schema 根已在 `schema-pack-v0` 固定
- `analysis/contracts` 已被明确标记为 compatibility mirror

见：
[schema-pack-v0.md](/E:/agent/booming-il2cpp/docs/archive/architecture/roadmap-0/schema-pack-v0.md#L11)
[schema-pack-v0.md](/E:/agent/booming-il2cpp/docs/archive/architecture/roadmap-0/schema-pack-v0.md#L14)

## 4.2 trace contract

保持不变：

- `tests/contracts/trace/schema`
- `tests/contracts/trace/snapshots`

这一层当前已经是 canonical baseline，不需要迁。

## 5. `contracts.py` 的首批调整

当前 `contracts.py` 还在读：

- `analysis/contracts/schemas`
- `analysis/contracts/examples`

见：
[contracts.py](/E:/agent/booming-il2cpp/build/toolchains/run/testing/contracts.py#L81)
[contracts.py](/E:/agent/booming-il2cpp/build/toolchains/run/testing/contracts.py#L93)

首批建议改成下面这样：

### `analysis_contract_json_paths(repo_root)`

改为返回：

- `contracts/artifacts/v0/schemas/*.json`
- `contracts/examples/v0/artifacts/*.json`
- `tests/contracts/schema/*.json`

### `validate_analysis_contracts(repo_root)`

改为：

- `schema_dir = repo_root / "contracts" / "artifacts" / "v0" / "schemas"`
- `example_dir = repo_root / "contracts" / "examples" / "v0" / "artifacts"`
- `snapshot_dir = repo_root / "tests" / "contracts" / "schema"`

### `trace_contract_json_paths` / `validate_trace_schema_contracts`

保持不变。

## 6. 首批受影响的调用点

当前最直接的调用点有两个：

- `build/scripts/verify-roadmap-0.py`
- `build/toolchains/run/commands/test.py`

其中：

- `verify-roadmap-0.py` 会先 parse，再 validate analysis contracts
- `commands/test.py` 的 `contract-check` 入口也会走同一套 helper

见：
[verify-roadmap-0.py](/E:/agent/booming-il2cpp/build/scripts/verify-roadmap-0.py#L206)
[test.py](/E:/agent/booming-il2cpp/build/toolchains/run/commands/test.py#L940)

由于 helper 函数名不改，所以这两个调用点首批无需改调用语义，只要 helper 内部切换路径即可。

## 7. 首批受影响的测试

## 7.1 `test_contract_paths.py`

当前只断言：

- analysis contract path 包含 `tests/contracts/schema`
- trace contract path 包含 `tests/contracts/trace`

见：
[test_contract_paths.py](/E:/agent/booming-il2cpp/tests/tooling/run/test_contract_paths.py#L25)

首批应补 stronger assertion：

- 包含 `contracts/artifacts/v0/schemas/aot-manifest.schema.json`
- 包含 `contracts/examples/v0/artifacts/aot-manifest.min.json`
- 不再包含 `analysis/contracts/schemas/aot-manifest.schema.json`
- 不再包含 `analysis/contracts/examples/aot-manifest.min.json`

## 7.2 `test_contract_check.py`

当前 mock 返回值里还硬编码了：

- `analysis/contracts/schemas/aot-manifest.schema.json`

见：
[test_contract_check.py](/E:/agent/booming-il2cpp/tests/tooling/run/test_contract_check.py#L28)

首批应改成：

- `contracts/artifacts/v0/schemas/aot-manifest.schema.json`

虽然这里是 mock，但它会持续误导后续维护者。

## 7.3 其他测试

如果还有测试通过字符串搜索 `analysis/contracts` 来判断 contract root，也应一并改掉。

原则是：

- 新测试只认 canonical source
- 不再为 `analysis/contracts` 建立新正向断言

## 8. `analysis/contracts` 的退役策略

## 8.1 首批状态：冻结但保留

首批不建议立刻删目录。

建议状态：

- 保留物理目录
- 不再作为新实现输入
- 不再新增对它的测试依赖
- 文档中明确它是 compatibility mirror

原因：

- 当前仓库还有历史文档和脚本在引用它
- 这一步先切 source，再删 mirror 更稳

## 8.2 第二阶段：新增守卫

在 helper 路径切换完成后，建议加一个简单守卫：

- 新代码禁止引入新的 `analysis/contracts` 读取点

可以通过：

- unit test
- grep-based repo check
- 或 lint 规则

来实现。

## 8.3 第三阶段：删除 mirror

只有在下面三个条件都满足后，才删：

1. `contracts.py` 已完全改读 canonical root
2. 所有 contract-check / verify / subject worker 都不再读 `analysis/contracts`
3. 相关测试与文档完成更新

这时候再删 `analysis/contracts`，成本最低。

## 9. 对新 subject 管线的直接意义

这一步虽然看起来只是路径修正，但它会直接影响后续几件事：

- `contract-validate.report.json` 中 `schemaSourceRoot` / `exampleSourceRoot` 的正式值
- `frontend-pipeline-worker` 的 validator 输入
- `subject` 执行时对 contract source 的统一引用方式
- `analysis/contracts` 退出正式执行路径

也就是说，这一步是后续 subject 化执行的基础清障，不是边角清理。

## 10. 推荐实施顺序

建议按下面顺序做：

1. 修改 `contracts.py` 内部路径
2. 修改 `tests/tooling/run/test_contract_paths.py`
3. 修改 `tests/tooling/run/test_contract_check.py`
4. 跑相关 unit / tooling tests
5. 更新讨论文档中的 canonical path 引用
6. 把 `analysis/contracts` 标记进删除路径

## 11. 本轮正式建议

本轮可以直接固定为：

- `contracts.py` 首批只切路径，不切函数名
- analysis artifact contract 的 canonical source 固定为：
  - `contracts/artifacts/v0/schemas`
  - `contracts/examples/v0/artifacts`
  - `tests/contracts/schema`
- `analysis/contracts` 进入冻结兼容态
- 新 subject/matrix/stage 实现禁止再读取 `analysis/contracts`
