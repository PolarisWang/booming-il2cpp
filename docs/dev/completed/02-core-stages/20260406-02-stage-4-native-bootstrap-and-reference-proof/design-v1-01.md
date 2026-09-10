# Stage 4 Native Bootstrap And Reference Proof Design

Date: 2026-04-06 09:43:58 +08:00
Status: ready-for-plan

## 设计目标

消费 Stage 3 产出的 managed closure bundle，在 `Windows x64` reference host 上打通 first generated C++ emission、native bootstrap 与 proof host，真实输出 `Hello, World!` 并返回 `0`。

## 设计边界

### 纳入

- `artifacts/proof/managed-closure/HelloWorldObject/` 作为 Stage 4 的唯一 managed 上游输入。
- `src/native/runtime-core/`
- `src/native/bootstrap/`
- `src/native/support/`
- `tests/proof/native-reference/HelloWorldObject/`
- `artifacts/proof/native-reference/HelloWorldObject/`
- `CMakeLists.txt` / `CMakePresets.json` / reference preset 接线中与 Stage 4 proof 直接相关的最小改动。

### 不纳入

- Stage 3 bundle schema 的再次扩张或语义改写。
- `macOS` / `Linux` 的并行 proof host。
- `GenericEcho`、`ReflectionLite`、`PInvokeLite` 等 follow-on sample。
- first proof 不需要的通用 GC、array、delegate、virtual-dispatch 全量运行时语义。

## 关键设计结论

1. Stage 4 只消费 Stage 3 bundle，不再重新发明中间产物。

- `typed-il-ir.json`
- `aot-manifest.json`
- `metadata-registration.json`
- `code-registration.json`
- `closure.manifest.json`

Stage 4 可以新增 proof 侧派生产物，但这些派生产物必须能够单向追溯回这五个输入文件，而不是另起一套手工 registration 或隐藏映射。

2. Stage 4 必须补上 first generated C++ emission，而不只是实现 runtime/bootstrap。

Stage 3 明确结束在 generated-code input bundle。Stage 4 的第一责任，是把这个 bundle materialize 成 first proof 所需的 native translation unit，然后再把它放进 runtime/bootstrap 闭环中编译和执行。

推荐把 proof-only generated output 放在：

- `artifacts/proof/native-reference/HelloWorldObject/generated/`

这样可以保持 `src/native/` 只承载可维护的 runtime/bootstrap/support 真源，而 generated C++ 保持为 proof 构建产物。

3. `Windows x64` 是 first proof host，`macOS` 保持延后。

原因：

- `reference-desktop-gates.md` 已明确 `Windows` 是主基线，`macOS` 是同语义子集。
- 当前仓库已有 `windows-x64-reference` 预设与 Windows gate 外壳。
- 对 first proof 而言，先在一个 host 打穿 vertical slice，比双平台并行更重要。

4. `src/native/runtime-core` 只承接 ABI 级最低职责。

`runtime-core` 负责：

- `runtime_init` / `runtime_shutdown`
- `thread_attach` / `thread_detach`
- `object_new`
- `string_new_utf8`
- `class_init`
- `field_get_value` / `field_set_value`
- proof 所需的最小 image / type / method / field 查询

它不负责：

- 把 generated-code helper 继续塞回 ABI
- 宿主输出协议
- 高阶 virtual / delegate / icall 完整语义

5. `src/native/bootstrap` 承接 registration、token resolve、generated landing 与 proof host 装配。

`bootstrap` 负责：

- 吃入 Stage 3 的 `code-registration` 与 `metadata-registration`
- 实现 `register_codegen` 与 `bootstrap_runtime`
- 提供 `resolve_type_by_token` / `resolve_method_by_token` / `resolve_field_by_token`
- 把 first generated C++ emission 与 proof host 装配成单一路径

Stage 4 不允许把 proof 成功建立在宿主 per-method 手工 patch-up 上。宿主最多只负责一次顶层启动，不负责替 generated code 逐个补线。

6. `src/native/support` 只实现 first proof 所需的窄 helper。

`support` 允许承接：

- narrow string concat path
- `Console.WriteLine(string)` glue
- first proof 所需的最小 UTF-8 / console surface

但不在本轮承接：

- 通用字符串库
- 宽字符 / 本地化 / 平台抽象全量语义
- 超出 `HelloWorldObject` 所需的额外 helper 家族

7. proof host 与 reference preset 的关系应是“增强”，不是“替换”。

当前 `windows-x64-reference` 预设仍承担 compile-only contract smoke 的 toolchain anchor。Stage 4 推荐做法是：

- 保留现有 contract smoke target
- 为 `windows-x64-reference` 引入新的 native proof target
- 让 reference preset 在 proof 跑通后同时具备“contract anchor + proof executable”两层语义

这样可以避免在 first proof 尚未稳定前丢掉现有 reference gate 的低层锚点。

## 推荐目录与产物形态

### 真源目录

- `src/native/runtime-core/`
- `src/native/bootstrap/`
- `src/native/support/`
- `tests/proof/native-reference/HelloWorldObject/`

### 生成产物目录

- `artifacts/proof/native-reference/HelloWorldObject/generated/`
- `artifacts/proof/native-reference/HelloWorldObject/run/`

其中：

- `generated/` 放 Stage 4 materialize 出来的 first generated C++ translation unit
- `run/` 放 proof executable、stdout capture、exit-code record 或同类 proof run artifacts

## 验证策略

1. Stage 4 先复用 Stage 3 的正式入口确认 managed closure bundle 仍可用。
2. `windows-x64-reference` 预设需要能把 Stage 4 proof target 编译出来。
3. proof executable 真实输出 `Hello, World!`，并返回 `0`。
4. direct-call landing 仍为唯一主路径，不回退到 `method_invoke` 或宿主手工 rescue。

## 设计结果

Stage 4 作为单个子任务继续推进，不再拆成新的 roadmap。下一步应进入 `plan-v1-01.md`，把 generated emission、runtime-core、bootstrap、support、preset 接线与 proof verification 拆成可执行 chunks。
