# analysis / contract 存储策略 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 结论

要调整，但不是把根目录下的 `analysis/` 和 `contracts/` 整体都改成阶段产物目录。

正确的拆分方式是区分三类东西：

- 规范源
- 验证基线
- 运行产物

在新 `subject + matrix + stage` 管线里：

- 根目录 `contracts/` 应继续承担规范源角色
- 根目录 `analysis/` 不应继续承担实际 analysis 产物落点
- 所有针对具体 `subject` 执行后生成的 analysis / generated / build / runtime 产物，都应进入 `artifacts/subjects/...`

所以答案不是“全部拆到阶段目录”，而是：

- 规范源留在源码树
- 运行产物拆到阶段桶
- 当前重复和混放的路径要清理

## 2. 当前仓库的实际状态

当前仓库里已经存在三套语义：

### 2.1 `contracts/` 更接近 canonical source

现状：

- `contracts/artifacts/v0/schemas/*.schema.json`
- `contracts/examples/v0/artifacts/*.min.json`
- `contracts/native/v0/*.h`
- `contracts/native/docs/v0/*.md`

其中 `schema-pack-v0` 已明确写出：

- `contracts/artifacts/v0/schemas/*.schema.json` 是 canonical schema 根
- `contracts/examples/v0/artifacts/*.min.json` 是 canonical example 根

见：
[schema-pack-v0.md](/E:/agent/booming-il2cpp/docs/archive/architecture/roadmap-0/schema-pack-v0.md#L7)

### 2.2 `analysis/contracts` 只是 compatibility mirror

现状：

- `analysis/contracts/schemas/*.json`
- `analysis/contracts/examples/*.json`

但同一份文档已经明确：

- `analysis/contracts/{schemas,examples}` 当前保留为 compatibility mirror

见：
[schema-pack-v0.md](/E:/agent/booming-il2cpp/docs/archive/architecture/roadmap-0/schema-pack-v0.md#L14)

这说明它不是长期权威位置。

### 2.3 `tests/contracts/*` 承担验证基线与 smoke

现状：

- `tests/contracts/schema/*.snapshot.json`
- `tests/contracts/trace/schema/*.json`
- `tests/contracts/trace/snapshots/*.json`
- `tests/contracts/native/abi`
- `tests/contracts/native/bridge`

其中：

- `tests/contracts/schema` 是 snapshot baseline
- `tests/contracts/trace` 是 trace contract / snapshot baseline
- `tests/contracts/native/*` 是 compile-only smoke / contract harness

它们不是 runtime 产物目录，而是验证资产目录。

## 3. 新管线下应如何定性

## 3.1 `contracts/` 是 stage 的输入规范，不是 stage 的输出产物

`contracts/` 里的东西本质上是：

- schema
- example
- header
- 文档
- 规范化 example

这些应该被看作：

- stage worker 的输入
- contract validator 的输入
- 人工维护的稳定规范源

不应看作：

- 某次 subject 执行后的阶段产物

所以它不应该被搬进：

- `artifacts/subjects/<subject-id>/...`

也不应该为每个 matrix 都复制一份。

## 3.2 `analysis` 这个名字应回到“能力域”而不是“产物域”

在新框架里，`analysis` 更合理的含义应是：

- 分析能力
- 分析模块
- 分析规范

而不是：

- 某次运行生成出来的 typed-il-ir / metadata-registration / code-registration 文件存放地

这些真正的 analysis 产物，应进入：

- `artifacts/subjects/<subject-id>/shared/analysis/`

或者在 matrix 不共享时进入：

- `artifacts/subjects/<subject-id>/matrices/<matrix-id>/analysis/`

因此：

- 根目录 `analysis/` 可以保留能力域含义
- 但不应再承担运行产物存储职责

## 4. 推荐的三层模型

## 4.1 第一层：规范源

推荐保留在源码树：

```text
contracts/
  artifacts/v0/schemas/
  examples/v0/artifacts/
  native/v0/
  native/docs/v0/
  native/examples/v0/
```

角色：

- canonical contract source
- 版本化规范资产

## 4.2 第二层：验证基线

推荐继续保留在源码树：

```text
tests/contracts/
  schema/
  trace/schema/
  trace/snapshots/
  native/abi/
  native/bridge/
```

角色：

- snapshot baseline
- trace baseline
- compile-only smoke harness

## 4.3 第三层：阶段运行产物

必须放到 `artifacts/`：

```text
artifacts/subjects/<subject-id>/
  shared/
    host-input/
    analysis/
    generated/
  matrices/
    <matrix-id>/
      build/
      runtime/
      report.json
```

角色：

- 本次 subject 执行的真实阶段产物
- 问题定位证据
- 报告与 diff 的输入

## 5. 因此哪些东西要迁，哪些不要迁

## 5.1 应迁出的

应从“根目录长期落盘”迁出的，是运行后生成的实际产物，例如：

- `typed-il-ir.json`
- `aot-manifest.json`
- `metadata-registration.json`
- `code-registration.json`
- `closure.manifest.json`
- `HelloWorldObject.generated.cpp`
- native proof manifest
- build 输出
- runtime trace

这些都应进入 `artifacts/subjects/...` 的阶段桶。

## 5.2 不应迁出的

不应迁出源码树的是：

- schema
- example
- native ABI / bridge header
- trace snapshot baseline
- compile-only smoke harness

这些本质上不是“某次执行结果”，而是“定义与验证这次执行的规则”。

## 6. 当前最该调整的两个点

## 6.1 `analysis/contracts` 不应继续作为长期正式路径

现在最明显的问题不是“它没分阶段”，而是“它重复了 canonical 内容”。

推荐定性：

- `contracts/` 是正式 contract source
- `analysis/contracts` 是短期 compatibility mirror
- 新 subject/matrix/stage 实现不再读取 `analysis/contracts`
- 旧逻辑迁完后删除这层 mirror

这会要求相关 validator 逐步改读：

- `contracts/artifacts/v0/schemas`
- `contracts/examples/v0/artifacts`

而不是继续读：

- `analysis/contracts/schemas`
- `analysis/contracts/examples`

## 6.2 contract 校验结果可以入阶段桶，但 contract 本身不要入阶段桶

可以入阶段桶的是：

- contract validate report
- schema/example/snapshot compare report
- trace compare report
- resolved input manifest

例如：

```text
artifacts/subjects/HelloWorldObject/shared/analysis/
  contract-validate.report.json
  analysis.manifest.json

artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/
  trace-compare.report.json
```

但不要把 canonical schema/example/header 复制成：

- `artifacts/.../analysis/contracts/...`

否则源码树与运行树会再次双份漂移。

## 7. 对当前 worker 设计的直接影响

这会直接影响前面已经讨论的 worker：

- `frontend-pipeline-worker`
  - 输出 `proof-input-bundle` 到 `artifacts/.../analysis/`
- `native-proof-emitter`
  - 输出 generated 产物到 `artifacts/.../generated/`
- `trace-compare-worker`
  - 读取 `tests/contracts/trace` baseline
  - 输出 compare report 到当前 matrix 的 `runtime/`
- contract validator
  - 读取 `contracts/` 与 `tests/contracts/`
  - 输出 validate report 到 `analysis/` 或 `report/`

## 8. 最终建议

如果按新管线落地，我建议正式固定下面这条原则：

- 根目录 `contracts/`：保留，作为 canonical 规范源
- 根目录 `analysis/`：保留能力域含义，但不再承载运行产物
- `analysis/contracts`：标记为 compatibility mirror，进入删除路径
- `tests/contracts/*`：保留，作为 baseline 与 harness
- 所有 subject 执行出来的 analysis / generated / build / runtime 产物：进入 `artifacts/subjects/...`

所以对你这个问题的直接回答是：

- `analysis` 产物要拆到自己的阶段去存储
- `contract` 本体不要拆成阶段产物
- `contract` 的验证结果可以按阶段存储
