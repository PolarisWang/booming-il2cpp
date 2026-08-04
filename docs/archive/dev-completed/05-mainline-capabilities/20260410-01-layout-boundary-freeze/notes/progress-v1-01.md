# Progress v1.01 — Layout Boundary Freeze 完成

Date: 2026-04-10

## 完成项

- [x] 编写 `layout-target-v1.md`
  - 冻结 `contracts/`、`tests/contracts/`、`build/toolchains/run/`、`src/managed/CodeGen`、`src/managed/Loader`、`tests/tooling/run/`、`tests/unit/run/` 的终态目录树
  - 定义 formal definition / shared validation fixture / subject-owned content 三类 ownership
  - 冻结目录和文件命名规则
  - 列出不动项
- [x] 编写 `path-mapping-v1.md`
  - 覆盖 Phase 1-6 所有路径迁移映射
  - 标注每条映射影响的 consumer（path_resolver、run_manifest、test discovery、docs 等）
  - 包含 `path_resolver.contract_roots()` 的具体代码变更前后对照
- [x] 编写 `file-split-policy-v1.md`
  - 冻结 Python ≤500 行、C# ≤600 行、JSON ≤300 行的软性预算
  - 定义按职责边界拆分原则
  - 定义 manifest 分片方案与组合加载器规格
  - 列出 8 个当前超标热点文件及其计划处理阶段
- [x] 三份文档内部一致性验证通过

## 无 Wiki 更新

本任务是 roadmap Phase 0 的规范冻结，产出为任务目录内文档，不涉及 wiki 更新。
