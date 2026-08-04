objectId: module/managed-closure/basic
objectType: module
primaryModuleId: managed-closure

# managed-closure

## 对象

- `module/managed-closure/basic`

## 目的

- 验证 managed pipeline 能从真实 `HelloWorldObject.dll` 产出与 canonical truth 对齐的 managed closure bundle
- 保持 proof 约束停留在测试侧，不把 sample-specific 逻辑塞回 `src/managed`

## 覆盖

- `contract/managed-closure-bundle`

## 正式入口

- `run test module --id module/managed-closure/basic`
- `run test module --module managed-closure --profile basic`

## 补充说明

- 当前活动 proof 输入位于 `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/host-input/<subject-id>.dll`
- 活动 analysis 落点位于 `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/`
- `tests/proof/**` 下的 subject 资产已经全部移出活动树
- 规格边界见 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/managed-minimal-closure-v0.md`
