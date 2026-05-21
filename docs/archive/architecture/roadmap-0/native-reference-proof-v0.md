# Native Reference Proof v0

## 目标

本文定义 Roadmap 0 Stage 4 的最小 native reference proof 边界。
Stage 4 负责消费 Stage 3 已冻结的 managed closure bundle，把它 materialize 成 first generated C++ emission，并逐步推进到 `Windows x64` reference host 上的真实 native proof。它不再改写 Stage 3 bundle 语义，也不在第一轮并行铺开 `macOS` / `Linux`。

## 输入

Stage 4 当前唯一正式的 managed 上游输入为：

- `artifacts/proof/managed-closure/HelloWorldObject/typed-il-ir.json`
- `artifacts/proof/managed-closure/HelloWorldObject/aot-manifest.json`
- `artifacts/proof/managed-closure/HelloWorldObject/metadata-registration.json`
- `artifacts/proof/managed-closure/HelloWorldObject/code-registration.json`
- `artifacts/proof/managed-closure/HelloWorldObject/closure.manifest.json`

其中 `closure.manifest.json` 继续作为 Stage 4 消费入口的聚合清单，Stage 4 不应绕过它重新定义另一套 managed 输入协议。

## 当前输出

Stage 4 第一轮已建立的 proof artifact 根目录为：

- `artifacts/proof/native-reference/HelloWorldObject/`

当前最小输出集合：

- `artifacts/proof/native-reference/HelloWorldObject/generated/native-reference.generated.cpp`
- `artifacts/proof/native-reference/HelloWorldObject/native-proof.manifest.json`

后续 native build / run 接线稳定后，再继续补：

- `artifacts/proof/native-reference/HelloWorldObject/run/`

## 当前 generated emission 边界

当前 `native-reference.generated.cpp` 只承担 Stage 4 的 first emission skeleton 角色：

- 从 Stage 3 bundle 推导 entry、ctor、instance method、field 与 metadata token。
- 明确使用 `register_codegen`。
- 明确使用 `bootstrap_runtime`。
- 明确保持 direct-call landing，不回退到 `method_invoke`。
- 在 generated path 内显式走 `field_set_value` / `field_get_value`、narrow string concat 与 `Console.WriteLine(string)` glue。

它当前还不等价于最终可运行的完整 generated method body 集合。后续 `runtime-core/bootstrap/support` 接线完成前，Stage 4 允许先把 generated emission 固定成可追溯、可测试、可持续扩展的 skeleton。

## Sample-Agnostic 边界

`src/**` 中禁止写入测试工程特化逻辑。

- 生产代码只能消费 Stage 3 bundle、contracts 和通用 helper surface。
- `HelloWorldObject` 这类样例信息只允许作为 canonical proof input / output truth 出现在 `artifacts/proof/**`、`tests/proof/**` 与对应文档中。
- 如果某条 Stage 4 逻辑无法脱离测试工程名而存在，则说明它还没有被正确收口到 bundle-driven contract 边界。

## Proof Host 约束

- first proof host 固定为 `Windows x64`。
- 继续保留 `windows-x64-reference` 作为 reference desktop 主基线。
- 不允许为了接入 Stage 4 proof 直接删除现有 compile-only contract smoke 锚点。

推荐策略：

- 保留现有 reference preset 的 contract anchor。
- 在同一个 preset 下增量接入 Stage 4 proof target。
- 在 generated C++ 尚未经过真实链接验证前，proof target 保持 `EXCLUDE_FROM_ALL`。

## 与 Stage 2 / Stage 3 的关系

Stage 4 仍受以下 canonical truth 约束：

- entry subject: `HelloWorldObject/Program::Main(System.String[])`
- stdout truth: `Hello, World!`
- exit code truth: `0`
- generated direct-call landing

Stage 4 可以新增 native 侧派生产物，但这些产物必须能够单向追溯回 Stage 3 bundle，而不是重新发明另一套 registration / metadata / subject 映射协议。
