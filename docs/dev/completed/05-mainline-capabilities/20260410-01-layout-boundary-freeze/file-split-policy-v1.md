# File Split Policy v1 — 文件拆分策略与软性行数预算

Date: 2026-04-10
Status: frozen-v1

---

## 1. 软性行数预算

"软性"意味着超过阈值不会自动阻塞合并，但需要有意识地决定是否拆分。

| 文件类型 | 软性上限 | 触发审查条件 | 硬性上限（建议） |
| --- | --- | --- | --- |
| Python 脚本 (`.py`) | 500 行 | 超过即进入拆分审查 | 800 行 |
| C# 源码 (`.cs`) | 600 行 | 超过即进入拆分审查 | 1000 行 |
| JSON manifest (`.json`) | 300 行 | 超过即考虑目录化分片 | 500 行 |
| PowerShell (`.ps1`) | 400 行 | 超过即进入拆分审查 | 600 行 |
| Shell 脚本 (`.sh`) | 300 行 | 超过即进入拆分审查 | 500 行 |

### 当前超标热点文件

| 文件 | 当前行数（约） | 超标程度 | 计划处理阶段 |
| --- | --- | --- | --- |
| `build/toolchains/run/tui.py` | ~1650 行 | 3.3× 软性上限 | Phase 2 |
| `build/toolchains/run/commands/test.py` | ~1389 行 | 2.8× 软性上限 | Phase 2 |
| `build/toolchains/run/testing/subject_workers.py` | ~978 行 | 2.0× 软性上限 | Phase 2 |
| `build/toolchains/run/project_workspace.py` | ~647 行 | 1.3× 软性上限 | Phase 2 |
| `build/toolchains/run/run_manifest.json` | ~925 行 | 3.1× 软性上限 | Phase 3 |
| `src/managed/.../NativeReferenceLoweringPlanner.cs` | ~2036 行 | 3.4× 软性上限 | Phase 4 |
| `src/managed/.../LoaderStage.cs` | ~1899 行 | 3.2× 软性上限 | Phase 4 |
| `src/managed/.../NativeReferenceProofEmitter.cs` | ~1165 行 | 1.9× 软性上限 | Phase 4（可选） |

---

## 2. 拆分原则

### 2.1 按职责边界拆分，不按行数机械切割

- 拆分单位是"职责"而不是"行数"
- 一个文件应该有一个清晰的核心职责
- 如果一个文件中可以识别出 3+ 个独立职责，即使总行数未超标，也建议考虑拆分

### 2.2 Facade 保留原文件

- 原文件保留为 facade / orchestration 入口
- facade 负责：公开 API、参数校验、流程编排、结果组装
- 具体实现逻辑下沉到子模块或子目录中的专门文件

### 2.3 Python 拆分规则

**从大文件拆出子模块的标准做法：**

```
# 拆分前
some_module.py  (1500 行，包含 A、B、C 三个职责)

# 拆分后
some_module/
  __init__.py      # re-export public API，保持外部 import 不变
  core.py          # 职责 A (facade)
  b_impl.py        # 职责 B
  c_impl.py        # 职责 C
```

**关键约束：**
- 如果原文件被外部 `from module import X` 引用，拆分后的 `__init__.py` 必须 re-export `X`
- 优先保持外部 import 路径不变，内部重组
- 如果拆分导致循环 import，用 lazy import 或调整依赖方向解决

### 2.4 C# 拆分规则

**从大文件拆出子类/子文件的标准做法：**

```
// 拆分前
NativeReferenceLoweringPlanner.cs  (2036 行)

// 拆分后
Lowering/
  NativeReferenceLoweringPlanner.cs   (facade, ~200-300 行)
  ShapeMatchers.cs                     (shape match 规则)
  LoweringFamilies.cs                  (family registry)
  PlanFactories/
    ConstructorThenInstanceCallFactory.cs
    ...
  Templates/
    TemplateLoader.cs
    TemplateModelBuilder.cs
  Validation/
    LoweringValidation.cs
```

**关键约束：**
- facade 文件保留原类名和 namespace
- 拆出的类使用 `internal` 可见性（除非确实需要 public）
- 拆分时不改变行为逻辑，仅重新组织代码位置
- `.csproj` 不需要显式 include（默认 glob 包含所有 `.cs`）
- 如果原文件使用 `partial class`，可以继续使用 partial 分文件

### 2.5 测试文件不拆分

- 测试文件一般不需要按行数拆分
- 如果测试文件过长，优先考虑按测试场景拆分为多个测试文件
- 每个测试文件应该测试一个明确的子系统或场景

---

## 3. Manifest 分片策略

### 3.1 当前问题

`run_manifest.json` (~925 行) 是一个包含 groups 定义和所有 command 定义的单一 JSON 文件。随着命令数量增加，此文件会持续膨胀。

### 3.2 分片方案

```
build/toolchains/run/manifests/
  run_manifest.json        # 组合加载入口（可选保留为兼容）
  run/
    groups.json            # group 列表定义
    commands.quick-start.json    # quick-start group 下的 commands
    commands.build.json          # build group 下的 commands
    commands.test.json           # test-and-verify group 下的 commands
    commands.environment.json    # environment group 下的 commands
    commands.inspect.json        # inspect group 下的 commands
    commands.project.json        # project-and-ide group 下的 commands
```

### 3.3 分片文件格式

**`groups.json`:**
```json
{
  "groups": [
    { "id": "quick-start", "title": "Quick Start" },
    { "id": "build", "title": "Build" },
    ...
  ]
}
```

**`commands.<group-id>.json`:**
```json
{
  "group": "<group-id>",
  "commands": [
    { "id": "...", "title": "...", ... },
    ...
  ]
}
```

### 3.4 组合加载器

- manifest loader 在运行时扫描 `manifests/run/` 下所有 `commands.*.json`
- 按 `groups.json` 中的顺序合并为完整 command registry
- 提供 `load_manifest()` 函数，返回与当前 `run_manifest.json` 等价的结构
- 支持 fallback：如果 `manifests/run/` 不存在，回退到旧单文件

### 3.5 分片原则

- 按 command group 分片，不按字母顺序或文件大小分片
- 每个分片文件应该自包含（可单独 validate）
- 分片文件的命名必须与 group id 对应
- `run_manifest.json` 的 `groups` 定义集中在 `groups.json`，不分散

---

## 4. 防回退约束

### 4.1 Enforcement Tests（Phase 6 交付）

在 `tests/unit/run/` 下新增以下 enforcement tests：

- **repo layout test**：验证终态目录结构（`contracts/` 不含 samples/snapshots, `tests/contract/` 不存在, etc.）
- **file budget test**：扫描 `build/toolchains/run/**/*.py` 和 `src/managed/**/*.cs`，报告超过软性上限的文件
- **path ownership test**：验证 formal definition、shared fixture、subject-owned content 的 ownership 边界

### 4.2 Repo Layout Test 扩展

现有 `test_repo_layout.py` 已包含部分 layout 约束。Phase 6 将在此基础上扩展：

- 添加 `test_contracts_dir_contains_only_formal_definitions()`
- 添加 `test_no_legacy_contract_alias_paths()`
- 添加 `test_build_tooling_uses_domain_based_layout()`
- 添加 `test_managed_hot_files_are_within_budget()`
