# 进度记录 v1-02

## 时间

- 2026-04-05 23:00:12 +08:00

## 已完成

- 建立 canonical contract 根：
  - `contracts/artifacts/v0/schemas/`
  - `contracts/examples/v0/artifacts/`
  - `contracts/docs/v0/`
- 补 `.gitignore` 例外规则，确保新的 canonical contract 目录不会被通用 `artifacts/` 忽略规则吞掉。
- 明确 `analysis/contracts/*` 继续作为 compatibility mirror，`tests/contracts/schema` 保持 canonical snapshot baseline，`tests/contract/schema` 同步为 compatibility snapshot。
- 更新 first-proof artifact contract：
  - `typed-il-ir` 显式新增 `methods[].subjectId`
  - `aot-manifest` 补齐 `field` subjectKind
  - `typed-il-ir`、`aot-manifest`、`code-registration`、`metadata-registration` 全部切到 `HelloWorldObject` 主线
- 新增 proof-boundary 文档：
  - `contracts/docs/v0/runtime-abi-proof-boundary.md`
  - `contracts/docs/v0/codegen-bridge-proof-boundary.md`
- 新增 generated direct-call landing 示例：
  - `contracts/native/examples/v0/bridge/direct-call-landing.cpp`
- 完成 analysis compatibility 路径 contract 校验、canonical JSON 解析检查、mirror 一致性检查。

## 当前状态

- 任务状态：`in_progress`
- 当前阶段：`verification`

## 验证结果

- 已通过：analysis schema/example/snapshot 契约校验
- 已通过：canonical `contracts/` JSON 解析检查
- 已通过：canonical / compatibility mirror 文件一致性检查
- 未完成：`tests/contracts/native/abi` 与 `tests/contracts/native/bridge` compile-only smoke

## 阻塞

- 当前 shell 中没有可用 `cmake`，所以 compile-only smoke 无法执行。

## 下一步

- 在具备 `cmake` 的 shell 中补跑 native compile-only smoke。
- 若环境仍无法提供 `cmake`，则把这项验证缺口显式记录为已知残留，再决定是否关闭 Stage 1A。
