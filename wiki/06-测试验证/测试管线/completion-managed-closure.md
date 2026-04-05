objectId: pipeline/completion-managed-closure
objectType: pipeline
primaryModuleId: managed-closure

# completion-managed-closure

## 对象

- `pipeline/completion-managed-closure`

## 用途

- completion

## 覆盖模块

- `managed-closure`

## 阶段

- `code`：`contract/managed-closure-bundle`
- `module`：`module/managed-closure/basic`

## 正式入口

- `run test pipeline --id pipeline/completion-managed-closure`
- `run test pipeline --pipeline completion-managed-closure`

## 补充说明

- 该 completion 管线只负责 Stage 3 managed closure bundle 的收口验证，不替代 Stage 4 native bootstrap / reference runtime proof。
