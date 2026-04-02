# build

## 负责

- 承载编译、链接、打包、cross-compile 与门禁脚本编排。
- 对接不同 host 的平台构建流程与输出规范。

## 不负责

- 不负责业务运行时行为实现。
- 不负责第三方 vendored 源码内容维护。

## 与其它目录的边界

- 消费 `codegen/`、`runtime/`、`contracts/` 的输入，输出进入 `artifacts/`。
- 验证标准由 `tests/` 和 `docs/architecture/roadmap-0/` 中矩阵与 gates 定义。
