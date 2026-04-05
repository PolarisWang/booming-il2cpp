# Runtime ABI Proof Boundary v0

## 作用

本文不是重复 `contracts/native/docs/v0/runtime-abi.md` 的全量接口说明，而是补充 first proof 真正要求冻结到什么边界。

## First-Proof Required Subset

first proof 只把 ABI 冻结到 “活着、分配、读写、查询、抛错” 这一层，不要求把所有 helper 都做成 proof blocker。

- lifecycle: `runtime_init`、`runtime_shutdown`、`thread_attach`、`thread_detach`
- allocation: `object_new`、`string_new_utf8`、`class_init`
- field access: `field_get_value`、`field_set_value`
- minimal query: `assembly_get_image`、`image_find_type`、`type_find_method`、`type_find_field`
- managed failure: `raise_managed_exception`

这组入口要足以支撑 `Program.Main`、`Greeter::.ctor(string)`、`Greeter.BuildMessage()` 这条 first-proof spine 的 runtime/bootstrap 装配。

## Declared But Not First-Proof Blocking

下面这些入口继续留在 `RuntimeAbiV0` 里，但 Stage 1A 不把它们当作 first proof 的必需闭环：

- `array_new`
- `gc_handle_new` / `gc_handle_free`
- `method_invoke`
- `type_find_property`
- `type_find_event`
- `method_get_parameter`
- `method_get_generic_context`

如果后续实现提前补了这些入口，语义也不能反向侵蚀 first-proof 的 direct-call 主路径。

## Boundary Reminder

- generated `Program.Main`、`Greeter::.ctor(string)`、`Greeter.BuildMessage()` 不允许退回到 `method_invoke` 作为主落点
- ABI 负责提供底层可组合元件，不负责替 generated code 承担高层 direct-call / virtual / delegate 语义
- 需要为生成代码提供“更像 C++ 调用面”的 helper 时，应该优先放在 bridge，而不是继续把 ABI 做胖
