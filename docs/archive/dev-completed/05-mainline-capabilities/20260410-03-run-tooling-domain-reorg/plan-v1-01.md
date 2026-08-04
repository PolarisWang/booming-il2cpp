# Run Tooling Domain Reorg 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 把 `build/toolchains/run/` 从松散根目录结构重组为 domain-based layout，拆分超长文件。

**架构：** 分 4 个 batch 逐层推进：先创建子目录骨架并移动简单文件，再重组 testing/ 子目录，然后拆分超长文件，最后统一 rewire imports。每个 batch 完成后运行测试验证。

**技术栈：** Python

**设计文档：** `docs/dev/completed/20260410-01-layout-boundary-freeze/layout-target-v1.md`

**预期知识沉淀：** 按任务决定

---

## 关键约束

1. **import 模式：** 所有模块使用 try/except 双路径 import（相对 + sys.path fallback）
2. **测试加载：** tests 使用 `importlib.util.spec_from_file_location` 动态加载，依赖文件路径
3. **CLI 兼容：** `run.py` 是入口，`run_manifest.json` 驱动命令发现，不能破坏 CLI 行为
4. **testing/__init__.py：** 当前 re-export 8 个 symbol，移动后需要维护

## 步骤

### Batch 1: 创建 core/ 并移动根目录公共模块

- [ ] **1.1** 创建 `build/toolchains/run/core/__init__.py`
- [ ] **1.2** `git mv` 以下文件到 `core/`：
  - `common.py` → `core/common.py`
  - `manifest.py` → `core/manifest.py`
  - `result.py` → `core/result.py`
  - `operation_reporting.py` → `core/operation_reporting.py`
  - `tooling.py` → `core/tooling.py`
- [ ] **1.3** 在每个被移动文件中更新 try/except import（如有内部引用）
- [ ] **1.4** 更新所有引用这些模块的文件的 import 语句：
  - `run.py`: result, manifest, operation_reporting, runtime, tui
  - `tui.py`: manifest, project_workspace
  - 所有 `commands/*.py`
  - 所有 `testing/*.py` 中引用 common/tooling 的文件
- [ ] **1.5** 运行 `python -m pytest tests/unit/run/ tests/tooling/run/ -x --timeout=60` 验证

### Batch 2: 创建 subject/ 并移动 workspace/deploy 模块

- [ ] **2.1** 创建 `build/toolchains/run/subject/__init__.py`
- [ ] **2.2** `git mv` 以下文件到 `subject/`：
  - `project_workspace.py` → `subject/project_workspace.py`
  - `deploy_layout.py` → `subject/deploy_layout.py`
- [ ] **2.3** 更新所有引用这两个模块的 import 语句
- [ ] **2.4** 运行测试验证

### Batch 3: 重组 testing/ 子目录

- [ ] **3.1** 创建子目录：`testing/contracts/`、`testing/registry/`、`testing/subject/`、`testing/session/`、`testing/subject/workers/`（各含 `__init__.py`）
- [ ] **3.2** `git mv` testing/ 模块到对应子目录：
  - contracts.py, path_resolver.py, selectors.py → `testing/contracts/`
  - catalog.py, registry.py, public_specs.py, suite_manifest.py → `testing/registry/`
  - subjects.py, subject_planner.py, subject_executor.py, subject_reporting.py, subject_validations.py → `testing/subject/`
  - events.py, session.py, reporting.py, traffic_light.py → `testing/session/`
- [ ] **3.3** 更新 `testing/__init__.py` 的 re-export 路径
- [ ] **3.4** 更新所有 testing 内部的相互 import
- [ ] **3.5** 更新 commands/test.py 和其他引用 testing 子模块的文件
- [ ] **3.6** 运行测试验证

### Batch 4: 验证与完成

- [ ] **4.1** 运行完整测试套件
- [ ] **4.2** 确认 CLI 命令仍可正常 dispatch
- [ ] **4.3** 更新 STATUS.md
