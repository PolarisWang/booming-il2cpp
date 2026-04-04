objectId: pipeline/completion-managed-smoke
objectType: pipeline
primaryModuleId: managed-smoke

# completion-managed-smoke

> 该测试管线已废弃，公开入口已切换到 `pipeline/completion-runtime-core`。本页仅保留历史迁移痕迹，不再作为正式索引入口。

## 对象

- `pipeline/completion-managed-smoke`

## 状态

- deprecated
- replacement: `pipeline/completion-runtime-core`

## 历史覆盖范围

- `code`：`smoke/HelloWorld`、`smoke/GenericEcho`
- `module`：`module/managed-smoke/basic`
- `system`：`system/hosted-runtime-smoke`

## 替代入口

- `run test pipeline --id pipeline/completion-runtime-core`
- `run test pipeline --pipeline completion-runtime-core`
