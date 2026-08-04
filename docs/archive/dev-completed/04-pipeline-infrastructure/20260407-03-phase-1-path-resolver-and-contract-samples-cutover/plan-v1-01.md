# Phase 1 Path Resolver And Contract Samples Cutover 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 引入统一 path resolver，并把 contract canonical sample 命名从 `examples` 收口到 `samples`，同时保持必要兼容。

**架构：** 先用测试锁定 resolver API 与新的 canonical 路径，再以最小实现把 `contracts.py`、`subjects.py` 接到 resolver，最后迁移 `contracts` 样例目录和关键文档引用。

**技术栈：** Python 3、`unittest`、JSON contract assets、仓库内 Markdown 文档

**设计文档：** `docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover/design-v1-01.md`

**预期知识沉淀：** 按任务决定；本阶段先不写 wiki

---

## 目标文件与职责

- 新增：[build/toolchains/run/testing/path_resolver.py](D:/agent/booming-il2cpp/build/toolchains/run/testing/path_resolver.py)
  - 提供 subject、artifact、contract canonical/compatibility path helper
- 修改：[build/toolchains/run/testing/contracts.py](D:/agent/booming-il2cpp/build/toolchains/run/testing/contracts.py)
  - 通过 resolver 消费 canonical `samples` 根
- 修改：[build/toolchains/run/testing/subjects.py](D:/agent/booming-il2cpp/build/toolchains/run/testing/subjects.py)
  - 将 subject root / artifact root 统一经 resolver 表达
- 修改：[tests/tooling/run/test_contract_paths.py](D:/agent/booming-il2cpp/tests/tooling/run/test_contract_paths.py)
  - 锁定 canonical `samples` 路径和 compatibility 边界
- 新增：[tests/unit/run/test_path_resolver.py](D:/agent/booming-il2cpp/tests/unit/run/test_path_resolver.py)
  - 锁定 resolver API 和路径输出
- 修改：[.gitignore](D:/agent/booming-il2cpp/.gitignore)
  - 纳入新的 canonical `samples` 目录
- 迁移：
  - `contracts/examples/v0/artifacts/*` -> `contracts/artifacts/v0/samples/*`
  - `contracts/native/examples/v0/**/*` -> `contracts/native/v0/samples/**/*`
- 修改：[contracts/docs/v0/overview.md](D:/agent/booming-il2cpp/contracts/docs/v0/overview.md)
  - 更新 canonical 布局描述
- 修改：[wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/schema-pack-v0.md](D:/agent/booming-il2cpp/wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/roadmap-0/schema-pack-v0.md)
  - 更新 canonical sample 路径描述
- 修改：[docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover/STATUS.md](D:/agent/booming-il2cpp/docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover/STATUS.md)
  - 持续记录状态
- 修改：[docs/dev/in-progress/20260407-02-subject-layout-and-variant-traceability-roadmap/STATUS.md](D:/agent/booming-il2cpp/docs/dev/in-progress/20260407-02-subject-layout-and-variant-traceability-roadmap/STATUS.md)
  - 记录 child plan 已进入执行
- 修改：[docs/dev/in-progress/20260407-02-subject-layout-and-variant-traceability-roadmap/roadmap-v1-01.md](D:/agent/booming-il2cpp/docs/dev/in-progress/20260407-02-subject-layout-and-variant-traceability-roadmap/roadmap-v1-01.md)
  - 将 `Phase 1` 状态更新为进行中
- 修改：[docs/dev/ACTIVE.md](D:/agent/booming-il2cpp/docs/dev/ACTIVE.md)
  - 切换 active 指针到当前 child plan
- 修改：[docs/dev/INDEX.md](D:/agent/booming-il2cpp/docs/dev/INDEX.md)
  - 同步 child plan 与 parent roadmap 的活动关系
- 修改：[docs/dev/in-progress/INDEX.md](D:/agent/booming-il2cpp/docs/dev/in-progress/INDEX.md)
  - 同步 in-progress 索引

## 执行步骤

- [ ] 步骤 1：先写 resolver 与 canonical `samples` 路径测试，让测试正确失败
- [ ] 步骤 2：实现 `path_resolver.py`，并把 `contracts.py`、`subjects.py` 接入 resolver
- [ ] 步骤 3：迁移 contract sample 目录到新的 `samples` canonical 根，并保留兼容副本
- [ ] 步骤 4：更新关键文档、任务状态与索引
- [ ] 步骤 5：运行本阶段验证并记录结果

## 关键验证命令

- [ ] `python -m unittest tests.unit.run.test_path_resolver`
- [ ] `python -m unittest tests.tooling.run.test_contract_paths`

## 执行时的状态维护

- [ ] 每完成一个步骤，都更新当前 child `STATUS.md`
- [ ] 每完成一个步骤，都更新 `docs/dev/ACTIVE.md`
- [ ] 每完成一个步骤，都追加 `notes/progress-*.md`
- [ ] 关键状态变化要同步回 parent roadmap 的 `STATUS.md` 与索引

## 完成标准

- [ ] 存在统一 path resolver，能表达 subject、artifact、contract canonical/compatibility root
- [ ] `contracts/artifacts/v0/samples` 成为 artifact contract sample canonical 根
- [ ] `contracts/native/v0/samples` 成为 native sample canonical 根
- [ ] `contracts.py` 不再把 `contracts/examples` 当作 canonical 根
- [ ] 核心验证命令通过
