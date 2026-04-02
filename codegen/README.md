# codegen

## 负责

- 承载代码生成入口、后端适配与输出组织。
- 将 `analysis/` 结果映射为 `runtime/` 可消费的目标代码或中间产物。

## 不负责

- 不负责平台安装包制作、签名与发布。
- 不负责第三方源码归档与许可证总表维护。

## 与其它目录的边界

- 上游依赖 `analysis/` 与 `contracts/`，下游对接 `runtime/` 与 `build/`。
- 验证正确性由 `tests/` 与 host-specific verify 流程覆盖。
