# 统一测试框架实现计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 把仓库测试域统一收口到 `run test ...`，建立稳定的 suite catalog、adapter、结果契约、日志布局、红绿灯覆盖和 perf 基线机制，并完成现有测试资产迁移。

**架构：** 在现有 `build/toolchains/run/` Python 入口上新增 `testing/` 子系统，承担 catalog、manifest、adapter、session、events、report 和 policy 逻辑；`commands/test.py` 变为薄调度层，`run_manifest.json` 与 TUI 只暴露 `run test ...` 能力。目录迁移采用“先并存骨架、再迁移入口、最后清理旧接口”的顺序，优先稳定 JSON / 文件契约，再逐步替换旧命令。

**技术栈：** Python 3.12 runtime、`run.*` wrapper、`run_manifest.json`、现有 `CommandResult` JSON 契约、`dotnet`、`pytest`、`cmake`、shell / PowerShell 脚本、仓库内 JSON manifest / baseline 文件。

**设计文档：** `docs/booming/2026-04-03-unified-test-framework/design-2026-04-03-16-unified-test-framework.md`

**预期知识沉淀：** `wiki/04-工具与集成/统一测试框架.md`、`wiki/04-工具与集成/统一入口脚本.md`

---

## 文件结构

### 计划中的核心新增文件

- 创建：`build/toolchains/run/testing/__init__.py`
- 创建：`build/toolchains/run/testing/catalog.py`
- 创建：`build/toolchains/run/testing/suite_manifest.py`
- 创建：`build/toolchains/run/testing/session.py`
- 创建：`build/toolchains/run/testing/events.py`
- 创建：`build/toolchains/run/testing/reporting.py`
- 创建：`build/toolchains/run/testing/fingerprints.py`
- 创建：`build/toolchains/run/testing/traffic_light.py`
- 创建：`build/toolchains/run/testing/perf.py`
- 创建：`build/toolchains/run/testing/adapters/__init__.py`
- 创建：`build/toolchains/run/testing/adapters/base.py`
- 创建：`build/toolchains/run/testing/adapters/python_pytest.py`
- 创建：`build/toolchains/run/testing/adapters/dotnet_app.py`
- 创建：`build/toolchains/run/testing/adapters/native_cmake.py`
- 创建：`build/toolchains/run/testing/adapters/shell_script.py`
- 创建：`build/toolchains/run/testing/adapters/workflow.py`
- 创建：`tests/unit/`
- 创建：`tests/integration/`
- 创建：`tests/contract/`
- 创建：`tests/gate/`
- 创建：`tests/workflow/`
- 创建：`tests/perf/`
- 创建：`tests/integration/run/`
- 创建：`tests/perf/<suite>/baselines/<host>.json`（按实际 perf suite 落地）

### 计划中的核心修改文件

- 修改：`build/toolchains/run/commands/test.py`
- 修改：`build/toolchains/run/commands/prepare.py`
- 修改：`build/toolchains/run/commands/inspect.py`
- 修改：`build/toolchains/run/run.py`
- 修改：`build/toolchains/run/manifest.py`
- 修改：`build/toolchains/run/run_manifest.json`
- 修改：`build/toolchains/run/tui.py`
- 修改：`tests/tooling/run/test_command_manifest.py`
- 修改：`tests/tooling/run/test_json_output.py`
- 修改：`tests/tooling/run/test_run_cli.ps1`
- 修改：`tests/tooling/run/test_run_cli.sh`
- 修改：`tests/tooling/run/test_tui.py`
- 修改：`tests/tooling/run/test_tui_routing.py`
- 修改：`wiki/04-工具与集成/统一测试框架.md`
- 修改：`wiki/04-工具与集成/统一入口脚本.md`

### 职责边界

- `commands/test.py`：只负责公开命令到测试框架 session 的调度，不再自己内嵌 smoke / trace 逻辑。
- `testing/catalog.py`：扫描 `tests/<family>/<suite>/suite.manifest.json`，生成 valid / invalid suite 视图与缓存索引。
- `testing/suite_manifest.py`：负责 manifest schema 校验、host 约束、stage 能力、selector / perf 规则解析。
- `testing/adapters/*`：负责 case 发现、build/run/report 执行桥接与原始输出采集。
- `testing/session.py`：负责 `run test ...` 生命周期、并行策略、strict 模式、exit code、suite 调度。
- `testing/events.py` / `reporting.py`：负责 `events.jsonl`、`session.json`、`summary.json`、suite report 与 artifacts 事件。
- `testing/fingerprints.py`：负责 build / prepare 指纹、缓存路径与回收。
- `testing/traffic_light.py` / `perf.py`：负责覆盖映射与基线判定。

## Chunk 1: `run test` 公开命令与旧接口迁移

### 任务 1：为 `run test` 建立新的公开命令模型

**文件：**
- 修改：`build/toolchains/run/run_manifest.json`
- 修改：`build/toolchains/run/manifest.py`
- 修改：`build/toolchains/run/commands/test.py`
- 测试：`tests/tooling/run/test_command_manifest.py`
- 测试：`tests/tooling/run/test_run_cli.ps1`
- 测试：`tests/tooling/run/test_run_cli.sh`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：公开命令模型、`all` 规则、`--stage` 语义和 list/discovery 是长期稳定知识。

- [ ] **步骤 1：编写失败测试，锁定新的测试域 CLI**

```python
def test_manifest_exposes_unified_test_family_commands():
    manifest = load_manifest_module().load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
    syntaxes = {command["id"]: " ".join(command.get("tokens", [])) for command in manifest["commands"]}
    assert "test-family-suite" in syntaxes
    assert syntaxes["test-family-suite"] == "test"
    assert "verify-roadmap-0-macos" not in syntaxes
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_run_cli`

预期：FAIL，提示 manifest 仍暴露旧 `verify ...` / 测试型 `build ...` 命令，且 `run test all` / `run test list` 缺失。

- [ ] **步骤 3：实现最小命令模型迁移**

```json
{
  "id": "test-family-suite",
  "tokens": ["test"],
  "handler": "test.dispatch"
}
```

```python
def resolve_cli_command(...):
    # 支持：
    # run test <family> <suite>
    # run test <family> all
    # run test all
    # run test list [family]
```

- [ ] **步骤 4：把旧测试接口改为迁移提示或兼容映射**

运行：更新 `build/toolchains/run/commands/inspect.py` 与 `build/toolchains/run/commands/test.py`，使测试型旧入口：

- `run build smoke HelloWorld` 映射为 `run test smoke HelloWorld --stage build`
- `run verify roadmap-0 --host macos` 映射为 `run test workflow roadmap-0-macos`

并在普通文本和 JSON 结果里带明确 deprecation / migration 提示。

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_run_cli tests.tooling.run.test_tui_routing`

预期：PASS；`run help`、TUI 列表和 CLI 路由都只展示新的测试域公开命令。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/run_manifest.json build/toolchains/run/manifest.py build/toolchains/run/commands/test.py build/toolchains/run/commands/inspect.py tests/tooling/run/test_command_manifest.py tests/tooling/run/test_run_cli.ps1 tests/tooling/run/test_run_cli.sh tests/tooling/run/test_tui_routing.py
git commit -m "feat: unify public test command model"
```

### 任务 2：让 `prepare` / harness / TUI 消费新的测试入口

**文件：**
- 修改：`build/toolchains/run/commands/prepare.py`
- 修改：`build/toolchains/run/run.py`
- 修改：`build/toolchains/run/tui.py`
- 修改：`wiki/04-工具与集成/统一入口脚本.md`
- 测试：`tests/tooling/run/test_prepare_scopes.py`
- 测试：`tests/tooling/run/test_tui.py`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一入口脚本.md`
- 原因：prepare scope 和 TUI 分组会影响本地开发、harness、CI 的稳定使用方式。

- [ ] **步骤 1：编写失败测试，锁定 prepare/TUI 新语义**

```python
def test_prepare_scope_points_to_test_suites_instead_of_legacy_verify_commands():
    prepare_module = load_prepare_module()
    plan_ids = prepare_module._prepare_plan("global", "macos")
    assert "test-smoke-all" in plan_ids
    assert "verify-roadmap-0-macos" not in plan_ids
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.tooling.run.test_prepare_scopes tests.tooling.run.test_tui`

预期：FAIL，prepare 仍然复用旧 build / verify scope，TUI 菜单仍按旧命令展示。

- [ ] **步骤 3：实现最小 prepare/TUI 对齐**

```python
def _prepare_plan(scope: str, host_platform: str) -> list[str]:
    if scope == "smoke":
        return ["test", "smoke", "all", "--stage", "build"]
```

把环境预热切换为新的 suite build / report 生命周期，而不是旧 `verify` 入口。

- [ ] **步骤 4：更新文档与 examples**

运行：把 wiki 中的 harness 生命周期改为：

1. `run bootstrap --yes --json`
2. `run prepare ... --json`
3. `run test ... --json`
4. `run clean --scope ... --json`

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.tooling.run.test_prepare_scopes tests.tooling.run.test_tui tests.tooling.run.test_tui_routing`

预期：PASS；prepare 和 TUI 都消费新的 `run test ...` 模型。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/commands/prepare.py build/toolchains/run/run.py build/toolchains/run/tui.py tests/tooling/run/test_prepare_scopes.py tests/tooling/run/test_tui.py wiki/04-工具与集成/统一入口脚本.md
git commit -m "feat: route prepare and tui through unified test entrypoint"
```

## Chunk 2: family 目录迁移与 catalog / manifest 骨架

### 任务 3：建立 suite manifest schema 与 catalog 扫描

**文件：**
- 创建：`build/toolchains/run/testing/suite_manifest.py`
- 创建：`build/toolchains/run/testing/catalog.py`
- 创建：`build/toolchains/run/testing/__init__.py`
- 创建：`tests/unit/run/test_suite_manifest.py`
- 创建：`tests/integration/run/test_catalog_scan.py`
- 测试数据：`tests/integration/run/fixtures/catalog/...`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：manifest 位置、valid/invalid suite 语义和 host 兼容性规则是统一框架的核心长期契约。

- [ ] **步骤 1：编写失败测试，锁定 manifest 与 catalog 行为**

```python
def test_catalog_lists_valid_and_invalid_suites_without_failing_whole_scan():
    catalog = load_catalog(fixtures_root, host_platform="macos")
    assert catalog.valid_suite_ids == ["smoke/hello-world"]
    assert catalog.invalid_suite_ids == ["contract/bad-manifest"]
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.unit.run.test_suite_manifest tests.integration.run.test_catalog_scan`

预期：FAIL，相关模块尚不存在。

- [ ] **步骤 3：实现最小 schema / catalog**

```python
def load_suite_manifest(path: Path) -> SuiteManifest: ...
def scan_catalog(repo_root: Path, host_platform: str, strict: bool) -> CatalogIndex: ...
```

要求：

- 扫描固定根目录 `tests/unit tests/integration tests/contract tests/smoke tests/gate tests/workflow tests/perf`
- manifest 位于 `tests/<family>/<suite-id>/suite.manifest.json`
- invalid suite 不抛全局异常，进入 `invalidSuites`
- catalog 缓存写到 `artifacts/tests/catalog/<host>/index.json`

- [ ] **步骤 4：为现有测试资产创建第一批 suite.manifest.json**

创建：

- `tests/smoke/hello-world/suite.manifest.json`
- `tests/smoke/generic-echo/suite.manifest.json`
- `tests/smoke/reflection-lite/suite.manifest.json`
- `tests/smoke/pinvoke-lite/suite.manifest.json`
- `tests/smoke/host-embedding-lite/suite.manifest.json`
- `tests/contract/trace-compare/suite.manifest.json`

先以最小可用范围覆盖当前真实运行中的 smoke / contract suite。

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.unit.run.test_suite_manifest tests.integration.run.test_catalog_scan tests.tooling.run.test_json_output`

预期：PASS；catalog 可以生成 valid / invalid 视图和 host 过滤结果。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/testing/__init__.py build/toolchains/run/testing/suite_manifest.py build/toolchains/run/testing/catalog.py tests/unit/run/test_suite_manifest.py tests/integration/run/test_catalog_scan.py tests/smoke tests/contract
git commit -m "feat: add test suite manifest and catalog skeleton"
```

### 任务 4：迁移目录语义并保留过渡映射

**文件：**
- 创建：`tests/integration/run/`
- 创建：`tests/gate/`
- 创建：`tests/workflow/`
- 修改：`tests/tooling/run/*`
- 修改：`tests/contracts/*`
- 修改：`wiki/04-工具与集成/统一测试框架.md`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：family 目录语义和迁移路径是后续新增 suite 的基础约定。

- [ ] **步骤 1：编写失败测试，锁定 family 目录映射**

```python
def test_catalog_family_names_match_new_public_families():
    catalog = scan_catalog(REPO_ROOT, host_platform="macos", strict=False)
    assert "integration" in catalog.family_names
    assert "contracts" not in catalog.family_names
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.integration.run.test_catalog_scan`

预期：FAIL，catalog / fixtures 仍停留在旧 family 命名。

- [ ] **步骤 3：迁移测试目录并更新引用**

执行：

- `tests/tooling/run/* -> tests/integration/run/*`
- `tests/contracts -> tests/contract`
- `tests/platform -> tests/gate`
- `tests/reference-desktop -> tests/workflow`

同时更新 `run_manifest.json`、脚本路径、snapshot 路径和文档引用。

- [ ] **步骤 4：添加兼容 shim 或迁移提示**

在过渡窗口内保留最薄重定向：

- 旧路径只做兼容导入 / 说明文件，不继续承载新逻辑
- 计划最后一个块再统一删除过渡层

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.integration.run.test_catalog_scan tests.tooling.run.test_command_manifest tests.tooling.run.test_json_output`

预期：PASS；目录迁移后 catalog、run 命令和 JSON 输出仍一致。

- [ ] **步骤 6：提交**

```bash
git add tests/integration tests/contract tests/gate tests/workflow build/toolchains/run/run_manifest.json wiki/04-工具与集成/统一测试框架.md
git commit -m "refactor: align test directories with unified test families"
```

## Chunk 3: adapter 框架与 case 发现契约

### 任务 5：实现 adapter registry 与 case discovery 契约

**文件：**
- 创建：`build/toolchains/run/testing/adapters/__init__.py`
- 创建：`build/toolchains/run/testing/adapters/base.py`
- 创建：`build/toolchains/run/testing/adapters/python_pytest.py`
- 创建：`build/toolchains/run/testing/adapters/dotnet_app.py`
- 创建：`build/toolchains/run/testing/adapters/native_cmake.py`
- 创建：`build/toolchains/run/testing/adapters/shell_script.py`
- 创建：`build/toolchains/run/testing/adapters/workflow.py`
- 创建：`tests/unit/run/test_adapter_registry.py`
- 创建：`tests/integration/run/test_case_discovery.py`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：固定 adapter 类型、synthetic case、`caseId` 稳定性是跨 suite 的底层约束。

- [ ] **步骤 1：编写失败测试，锁定 adapter 与 case 发现**

```python
def test_dotnet_adapter_emits_stable_synthetic_main_case():
    cases = DotnetAppAdapter(...).discover_cases()
    assert [case.case_id for case in cases] == ["main"]
```

```python
def test_case_id_collision_adds_stable_suffix():
    ids = [case.case_id for case in adapter.discover_cases()]
    assert ids == ["duplicate", "duplicate-2f8a"]
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.unit.run.test_adapter_registry tests.integration.run.test_case_discovery`

预期：FAIL，adapter registry / case discovery 尚不存在。

- [ ] **步骤 3：实现 registry 与公共 adapter 接口**

```python
class BaseAdapter:
    runner_type: str
    def discover_cases(self, suite: SuiteManifest) -> list[DiscoveredCase]: ...
    def build(...): ...
    def run(...): ...
    def report(...): ...
```

要求：

- `runnerType` 固定为 `python-pytest | dotnet-app | native-cmake | shell-script | workflow`
- `caseId` 统一 ASCII kebab-case
- 保留 `sourceId` / `sourceName`
- 无天然 case 时生成 synthetic case

- [ ] **步骤 4：把现有 smoke / trace / verify 迁到 adapter**

迁移：

- 现有 smoke 运行逻辑从 `build/toolchains/run/commands/test.py` 下沉到 `adapters/dotnet_app.py`
- 现有 trace compare 下沉到 `adapters/shell_script.py` 或 `adapters/workflow.py`
- 现有 roadmap-0 verify 下沉到 `adapters/workflow.py`

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.unit.run.test_adapter_registry tests.integration.run.test_case_discovery tests.tooling.run.test_json_output`

预期：PASS；case 发现稳定、adapter 可按 suite manifest 驱动。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/testing/adapters build/toolchains/run/commands/test.py tests/unit/run/test_adapter_registry.py tests/integration/run/test_case_discovery.py
git commit -m "feat: add adapter registry and case discovery contract"
```

### 任务 6：建立测试 session 执行器与 stage 语义

**文件：**
- 创建：`build/toolchains/run/testing/session.py`
- 修改：`build/toolchains/run/commands/test.py`
- 创建：`tests/unit/run/test_session.py`
- 创建：`tests/integration/run/test_stage_execution.py`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：`--stage`、implicit build / prepare、strict、exit code 和批量过滤都会进入长期自动化契约。

- [ ] **步骤 1：编写失败测试，锁定 session 行为**

```python
def test_stage_run_marks_implicit_build_when_build_cache_missing():
    result = execute_test_command(["test", "smoke", "hello-world", "--stage", "run"])
    assert result.payload["suiteResults"][0]["stageResults"]["build"]["implicit"] is True
```

```python
def test_explicit_incompatible_suite_fails_with_contract_exit_code():
    exit_code = run_main(["test", "workflow", "roadmap-0-windows", "--host", "macos"])
    assert exit_code == 2
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.unit.run.test_session tests.integration.run.test_stage_execution`

预期：FAIL，当前 `commands/test.py` 仍然是单命令直跑实现。

- [ ] **步骤 3：实现 session 执行器**

```python
def execute_session(request: TestRequest) -> SessionResult:
    # catalog 解析
    # host-compatible 过滤
    # strict 升级
    # stage build/setup/run/report
    # suite 级串行/并行
```

- [ ] **步骤 4：在 `commands/test.py` 中退化为薄包装**

```python
def handle(command, repo_root, host_platform, command_text):
    request = parse_test_request(command, ...)
    session = execute_session(request)
    return session.to_command_result(command_text, host_platform)
```

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.unit.run.test_session tests.integration.run.test_stage_execution tests.tooling.run.test_run_cli`

预期：PASS；`run test ...` 已经以 session 为中心执行。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/testing/session.py build/toolchains/run/commands/test.py tests/unit/run/test_session.py tests/integration/run/test_stage_execution.py
git commit -m "feat: add unified test session execution engine"
```

## Chunk 4: 日志、事件流、summary / report 契约

### 任务 7：落地运行目录、结果文件和事件流

**文件：**
- 创建：`build/toolchains/run/testing/events.py`
- 创建：`build/toolchains/run/testing/reporting.py`
- 创建：`tests/unit/run/test_events_schema.py`
- 创建：`tests/integration/run/test_reporting_layout.py`
- 修改：`tests/tooling/run/test_json_output.py`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：`artifacts/logs/tests/` 布局、事件封套和 `summary.json` 是 TUI、CI、harness 的共享消费契约。

- [ ] **步骤 1：编写失败测试，锁定结果文件与事件结构**

```python
def test_reporting_layout_writes_fixed_session_files():
    run_id = execute_fixture_suite(...)
    root = REPO_ROOT / "artifacts" / "logs" / "tests" / run_id
    assert (root / "session.json").is_file()
    assert (root / "summary.json").is_file()
    assert (root / "events.jsonl").is_file()
```

```python
def test_final_summary_event_is_last_and_medium_grained():
    events = load_events_jsonl(...)
    assert events[-1]["eventType"] == "final-summary"
    assert "suiteResults" not in events[-1]["payload"]
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.unit.run.test_events_schema tests.integration.run.test_reporting_layout tests.tooling.run.test_json_output`

预期：FAIL，当前结果仍是旧 `CommandResult` 扁平 artifacts 输出。

- [ ] **步骤 3：实现 events/reporting**

要求：

- 运行目录：`artifacts/logs/tests/<run-id>/`
- 固定文件：`session.json summary.json console.log events.jsonl telemetry.json suites/<suite-id>/{report.json,stdout.log,stderr.log}`
- `--json` 采用 line-delimited event stream
- 最后一条事件必须是 `final-summary`

- [ ] **步骤 4：把 `CommandResult` 输出桥接到新 summary**

```python
def to_command_result(summary: Summary) -> CommandResult:
    return CommandResult(
        payload={"runId": summary.run_id, "summaryPath": summary.path, ...}
    )
```

使旧 `run` 外壳仍能消费统一状态，同时让 JSON 正式由事件流与 summary 驱动。

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.unit.run.test_events_schema tests.integration.run.test_reporting_layout tests.tooling.run.test_json_output`

预期：PASS；文件布局、事件顺序和 summary/report 粒度符合设计。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/testing/events.py build/toolchains/run/testing/reporting.py tests/unit/run/test_events_schema.py tests/integration/run/test_reporting_layout.py tests/tooling/run/test_json_output.py
git commit -m "feat: add unified test events and reporting layout"
```

### 任务 8：接通 TUI / harness 的进度、warning、artifact、summary 事件

**文件：**
- 修改：`build/toolchains/run/tui.py`
- 修改：`build/toolchains/run/run.py`
- 修改：`wiki/04-工具与集成/统一测试框架.md`
- 修改：`wiki/04-工具与集成/统一入口脚本.md`
- 测试：`tests/tooling/run/test_tui.py`
- 测试：`tests/tooling/run/test_tui_routing.py`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：TUI / harness 如何消费事件流属于面向开发者的稳定使用说明。

- [ ] **步骤 1：编写失败测试，锁定 TUI 消费事件流而非旧扁平输出**

```python
def test_tui_renders_progress_from_event_stream():
    screen = render_tui_from_events([session_start, progress_10, progress_50, final_summary])
    assert "50%" in screen
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.tooling.run.test_tui tests.tooling.run.test_tui_routing`

预期：FAIL，当前 TUI 只消费静态 manifest，不消费测试事件流。

- [ ] **步骤 3：实现最小事件消费桥接**

要求：

- TUI 可以消费 `run test list` 元数据
- 运行态消费 `progress` / `warning` / `artifact` / `final-summary`
- harness 示例命令全面切换到 `run test ... --json`

- [ ] **步骤 4：更新文档**

把 wiki 中所有测试域示例命令、TUI 说明、harness 生命周期更新为新事件和文件布局。

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.tooling.run.test_tui tests.tooling.run.test_tui_routing tests.tooling.run.test_json_output`

预期：PASS；TUI / harness 文档与实现都指向统一事件模型。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/tui.py build/toolchains/run/run.py tests/tooling/run/test_tui.py tests/tooling/run/test_tui_routing.py wiki/04-工具与集成/统一测试框架.md wiki/04-工具与集成/统一入口脚本.md
git commit -m "feat: integrate test event stream with tui and harness"
```

## Chunk 5: 红绿灯覆盖、perf 基线与最终清理

### 任务 9：落地 traffic-light 覆盖模型

**文件：**
- 创建：`build/toolchains/run/testing/traffic_light.py`
- 创建：`tests/unit/run/test_traffic_light.py`
- 修改：`build/toolchains/run/testing/suite_manifest.py`
- 修改：`build/toolchains/run/testing/reporting.py`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：红绿灯选择器与计数模型是测试质量契约的一部分。

- [ ] **步骤 1：编写失败测试，锁定覆盖映射规则**

```python
def test_each_discovered_case_must_map_to_exactly_one_traffic_light_bucket():
    with self.assertRaises(TrafficLightConfigError):
        assign_traffic_lights(cases, overlapping_selector_manifest)
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.unit.run.test_traffic_light`

预期：FAIL，traffic-light 模块尚不存在。

- [ ] **步骤 3：实现最小覆盖分配与校验**

要求：

- 只支持 `caseIds sourceIds tags include exclude`
- 每个 case 恰好落入 `green / red / yellow`
- 结果进入 `trafficLightCounts`
- `workflow / perf` 默认非强制，允许 suite override

- [ ] **步骤 4：把覆盖结果写入 suite report / summary**

运行：更新 `report.json` 和 `summary.json` 序列化，补充三色计数与 case 级映射结果。

- [ ] **步骤 5：运行测试验证通过**

运行：`python3 -m unittest tests.unit.run.test_traffic_light tests.integration.run.test_reporting_layout`

预期：PASS；traffic-light 选择器错误会变成明确配置失败。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/testing/traffic_light.py build/toolchains/run/testing/suite_manifest.py build/toolchains/run/testing/reporting.py tests/unit/run/test_traffic_light.py
git commit -m "feat: add traffic light coverage policy"
```

### 任务 10：落地 perf baseline、退出码、旧入口清理与最终文档收口

**文件：**
- 创建：`build/toolchains/run/testing/perf.py`
- 创建：`build/toolchains/run/testing/fingerprints.py`
- 创建：`tests/unit/run/test_perf_policy.py`
- 创建：`tests/unit/run/test_fingerprints.py`
- 修改：`build/toolchains/run/testing/session.py`
- 修改：`build/toolchains/run/run_manifest.json`
- 修改：`wiki/04-工具与集成/统一测试框架.md`
- 修改：`wiki/04-工具与集成/统一入口脚本.md`

**知识沉淀：**
- 目标：`wiki/04-工具与集成/统一测试框架.md`
- 原因：perf 基线、退出码和旧入口清理决定第一版何时可对外作为正式契约。

- [ ] **步骤 1：编写失败测试，锁定 perf / fingerprint / exit code**

```python
def test_perf_suite_uses_repo_baseline_file_without_auto_rewriting():
    result = execute_perf_suite(...)
    assert result.payload["baselinePath"].endswith("/tests/perf/startup/baselines/macos.json")
    assert result.payload["baselineUpdated"] is False
```

```python
def test_strict_invalid_suite_returns_exit_code_2():
    exit_code = run_main(["test", "all", "--strict"])
    assert exit_code == 2
```

- [ ] **步骤 2：运行测试验证失败**

运行：`python3 -m unittest tests.unit.run.test_perf_policy tests.unit.run.test_fingerprints tests.unit.run.test_session`

预期：FAIL，perf policy、缓存指纹和 strict 退出码尚未完整落地。

- [ ] **步骤 3：实现 perf / fingerprint / cache cleanup**

要求：

- build cache：`artifacts/tests/build/<host>/<family>/<suite>/fingerprints/<fingerprint>/`
- `current` 指针指向最近成功构建
- perf 基线来自 `tests/perf/<suite>/baselines/<host>.json`
- 只允许显式 `--update-baseline`
- 退出码稳定为 `0/1/2/3`

- [ ] **步骤 4：删除旧测试型 build/verify 公开入口**

执行：

- 从 `run_manifest.json` 删除旧测试型 `build ...` / `verify ...` 公开命令
- 清理过渡 shim
- 更新文档与脚本示例，只保留 `run test ...`

- [ ] **步骤 5：运行最终验证**

运行：

```bash
python3 -m unittest tests.unit.run.test_suite_manifest tests.unit.run.test_adapter_registry tests.unit.run.test_session tests.unit.run.test_events_schema tests.unit.run.test_traffic_light tests.unit.run.test_perf_policy tests.unit.run.test_fingerprints
python3 -m unittest tests.integration.run.test_catalog_scan tests.integration.run.test_case_discovery tests.integration.run.test_stage_execution tests.integration.run.test_reporting_layout
python3 -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_json_output tests.tooling.run.test_run_cli tests.tooling.run.test_prepare_scopes tests.tooling.run.test_tui tests.tooling.run.test_tui_routing
```

预期：PASS；公开接口、目录结构、结果契约、覆盖模型和 perf 语义全部稳定。

- [ ] **步骤 6：提交**

```bash
git add build/toolchains/run/testing build/toolchains/run/run_manifest.json wiki/04-工具与集成/统一测试框架.md wiki/04-工具与集成/统一入口脚本.md tests/unit tests/integration
git commit -m "feat: finalize unified test framework contract"
```

## 执行提醒

- 执行时优先使用 `@dev:test-driven-development`，每个任务都先写失败测试，再补最小实现。
- 当迁移目录或公开命令时，优先保证 `run test ...` 正确，再删除旧入口，避免一次性大爆炸迁移。
- 每完成一个 Chunk，都要更新 `docs/executions/CURRENT.md`、补一次 wiki 对应页面，并跑该 Chunk 末尾的验证命令。
- 如果某个 Chunk 涉及大量路径迁移，先提交纯重命名，再提交行为修改，降低 review 噪音。

计划已完成并保存到 `docs/booming/2026-04-03-unified-test-framework/plan-2026-04-03-23-unified-test-framework.md`。准备好执行了吗？
