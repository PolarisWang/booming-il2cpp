objectId: module/managed-closure/basic
objectType: module
primaryModuleId: managed-closure

# managed-closure

## 对象

- `module/managed-closure/basic`

## 目的

- 验证 Stage 3 managed pipeline 能从真实 `HelloWorldObject.dll` 产出与 Stage 2 canonical truth 对齐的 managed closure bundle。
- 固定 proof 约束仍留在测试侧，不把 sample-specific 逻辑重新塞回 `src/managed`。

## 覆盖

- `contract/managed-closure-bundle`

## 正式入口

- `run test module --id module/managed-closure/basic`
- `run test module --module managed-closure --profile basic`

## 补充说明

- 当前 proof 输入固定为 `tests/proof/input/HelloWorldObject/bin/Release/net8.0/HelloWorldObject.dll`。
- 关键输出固定为 `artifacts/proof/managed-closure/HelloWorldObject/typed-il-ir.json`、`aot-manifest.json`、`metadata-registration.json`、`code-registration.json` 和 `closure.manifest.json`。
- 规格边界见 `docs/architecture/roadmap-0/managed-minimal-closure-v0.md`。
