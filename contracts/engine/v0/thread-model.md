# Engine Thread Model v0

## 目标

本文件冻结 engine-facing contract 的最小线程规则，用于约束后续 `engine-bridge`、`Chaos.IL2CPP.EngineBinding` 与 host embedding 行为。

## 主线程 lane

- lifecycle 回调默认在主线程执行。
- callback dispatch 默认在主线程 lane 进入 engine host。
- host 应提供 `engine_is_main_thread` 作为线程判定入口。
- 非主线程工作需要通过 `engine_post_to_main_thread` 回投到主线程 lane。

## 最小约束

- `engine_init` / `engine_shutdown` / `engine_reload` / `engine_scene_attach` 必须由 engine host 串行化。
- `engine_register_callback` 可在初始化阶段调用，但 callback 的实际执行仍应遵守主线程约束。
- ownership 协议中的 transfer/release 不能绕过主线程约束直接操作需要主线程保护的 engine state。

## 当前边界

- 本轮只冻结 `engine_post_to_main_thread` 和 `engine_is_main_thread` 的语义，不在本文件中规定具体队列实现。
- 本轮不定义 job system、线程池或 fiber 语义。
