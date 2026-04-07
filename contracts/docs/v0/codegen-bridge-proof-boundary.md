# Codegen Bridge Proof Boundary v0

## 作用

本文补充 `contracts/native/docs/v0/codegen-bridge.md` 的 first-proof 冻结边界，明确哪些 helper 必须进首条证明链路，哪些 helper 只是声明保留。

## First-Proof Required Subset

Stage 1A 把 bridge 的 first-proof 必需面收窄为：

- `register_codegen`
- `bootstrap_runtime`
- `resolve_type_by_token`
- `resolve_method_by_token`
- `resolve_field_by_token`
- 面向 generated direct-call landing 的最小 helper 语义

换句话说，bridge 在 first proof 中首先要证明的是：

- registration / bootstrap 可以显式失败，而不是靠隐式全局状态硬拼
- generated `Program.Main` entry 可以落到已注册 method pointer
- generated `Greeter::.ctor(string)` 与 `Greeter.BuildMessage()` 可以走 direct symbol landing
- typed-il-ir / manifest / registration 中出现的 canonical `subjectId` 能追溯到 bridge 消费的 metadata surface

## Direct-Call Landing Contract

first proof 明确采用 generated direct-call landing，而不是以下替代路径：

- 不允许用 `method_invoke` 代替 `Program.Main`
- 不允许用解释执行代替 `Greeter::.ctor(string)` 或 `Greeter.BuildMessage()`
- 不允许用宿主直连手工补救代替 generated-code path

`callvirt` 出现在 first proof 的时候，只允许收窄到 “null-check + direct target landing” 这条最小语义，不要求在 Stage 1A 就交付完整通用虚派发。

## Deferred Helpers

下列 helper 继续声明在 `CodegenBridgeV0` 中，但本轮只标记为 deferred，不作为 first-proof blocker：

- `box_value` / `unbox_value`
- `create_delegate` / `delegate_invoke`
- `resolve_icall`

下列 helper 如果在 Stage 1 提前实现，也只允许先满足 first-proof 窄语义：

- `resolve_virtual_method`
- `invoke_virtual`

也就是说，`virtual` 相关 helper 可以先服务 `callvirt` 的 null-check pattern，但不能把“完整虚派发”伪装成当前已经冻结完成。

## Example

- generated direct-call landing 的最小示例见 `contracts/native/v0/samples/bridge/direct-call-landing.cpp`
