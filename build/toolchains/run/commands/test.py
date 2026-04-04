from __future__ import annotations

import sys
from pathlib import Path

try:
    from .. import manifest as manifest_module
    from ..testing import catalog as catalog_module
    from ..testing import session as session_module
    from ..testing import reporting as reporting_module
    from .. import tooling as tooling_module
    from ..common import combine_process_output, run_process
    from ..commands import build as build_commands
    from ..commands import verify as verify_commands
    from ..result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    import manifest as manifest_module
    from testing import catalog as catalog_module
    from testing import session as session_module
    from testing import reporting as reporting_module
    import tooling as tooling_module
    from common import combine_process_output, run_process
    from commands import build as build_commands
    from commands import verify as verify_commands
    from result import CommandResult


PUBLIC_TEST_SPECS = [
    {
        "id": "smoke/HelloWorld",
        "family": "smoke",
        "suite": "HelloWorld",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"build": "build-smoke-helloworld", "run": "test-smoke-helloworld", "all": "test-smoke-helloworld"},
    },
    {
        "id": "smoke/GenericEcho",
        "family": "smoke",
        "suite": "GenericEcho",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"build": "build-smoke-genericecho", "run": "test-smoke-genericecho", "all": "test-smoke-genericecho"},
    },
    {
        "id": "smoke/ReflectionLite",
        "family": "smoke",
        "suite": "ReflectionLite",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"build": "build-smoke-reflectionlite", "run": "test-smoke-reflectionlite", "all": "test-smoke-reflectionlite"},
    },
    {
        "id": "smoke/PInvokeLite",
        "family": "smoke",
        "suite": "PInvokeLite",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"build": "build-smoke-pinvokelite", "run": "test-smoke-pinvokelite", "all": "test-smoke-pinvokelite"},
    },
    {
        "id": "smoke/HostEmbeddingLite",
        "family": "smoke",
        "suite": "HostEmbeddingLite",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"build": "build-smoke-hostembeddinglite", "run": "test-smoke-hostembeddinglite", "all": "test-smoke-hostembeddinglite"},
    },
    {
        "id": "contract/trace-compare-windows",
        "family": "contract",
        "suite": "trace-compare-windows",
        "stages": ["all", "run"],
        "supported_hosts": ["windows"],
        "legacy_commands": {"run": "test-trace-compare-windows", "all": "test-trace-compare-windows"},
    },
    {
        "id": "contract/trace-compare-macos",
        "family": "contract",
        "suite": "trace-compare-macos",
        "stages": ["all", "run"],
        "supported_hosts": ["macos"],
        "legacy_commands": {"run": "test-trace-compare-macos", "all": "test-trace-compare-macos"},
    },
    {
        "id": "workflow/roadmap-0-windows",
        "family": "workflow",
        "suite": "roadmap-0-windows",
        "stages": ["all"],
        "supported_hosts": ["windows"],
        "legacy_commands": {"all": "verify-roadmap-0-windows"},
    },
    {
        "id": "workflow/roadmap-0-macos",
        "family": "workflow",
        "suite": "roadmap-0-macos",
        "stages": ["all"],
        "supported_hosts": ["macos"],
        "legacy_commands": {"all": "verify-roadmap-0-macos"},
    },
]


def find_public_test_suite_spec(family: str | None, suite: str | None) -> dict | None:
    for item in PUBLIC_TEST_SPECS:
        if item["family"] == family and item["suite"] == suite:
            return item
    return None


def list_public_test_suites(manifest: dict, host_platform: str) -> list[dict]:
    del manifest
    return [
        {
            "id": item["id"],
            "family": item["family"],
            "suite": item["suite"],
            "stages": list(item["stages"]),
        }
        for item in PUBLIC_TEST_SPECS
        if host_platform in item["supported_hosts"]
    ]


def resolve_legacy_test_command_id(family: str, suite: str, *, stage: str, host_platform: str) -> str | None:
    suite_spec = find_public_test_suite_spec(family, suite)
    if suite_spec is None:
        return None

    if host_platform not in suite_spec["supported_hosts"]:
        return None

    if stage not in suite_spec["stages"]:
        return None

    return suite_spec["legacy_commands"].get(stage)


def _render_public_test_list(items: list[dict]) -> str:
    lines = ["Available test suites", ""]
    for item in items:
        lines.append(f"{item['id']}: stages={','.join(item['stages'])}")
    return "\n".join(lines) + "\n"


def _handle_public_test_list(command_text: str, host_platform: str, options: dict, manifest: dict) -> CommandResult:
    family = options.get("family")
    items = list_public_test_suites(manifest, host_platform)
    if family:
        items = [item for item in items if item["family"] == family]

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=family,
        payload={"items": items},
        text=_render_public_test_list(items),
    )


def _execute_legacy_command(
    legacy_command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
) -> CommandResult:
    handler = legacy_command["handler"]
    if handler == "build.dispatch":
        return build_commands.handle(legacy_command, repo_root, host_platform, command_text)
    if handler == "verify.dispatch":
        return verify_commands.handle(legacy_command, repo_root, host_platform, command_text)
    return handle(legacy_command, repo_root, host_platform, command_text)


def _execute_public_test_session(
    family: str,
    suite: str,
    stage: str,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict,
) -> session_module.SessionResult:
    suite_spec = find_public_test_suite_spec(family, suite)
    legacy_command_id = resolve_legacy_test_command_id(family, suite, stage=stage, host_platform=host_platform)
    legacy_command = (
        manifest_module.find_command(manifest, legacy_command_id, host_platform, include_hidden=True)
        if legacy_command_id is not None
        else None
    )
    request = session_module.TestRequest(
        family=family,
        suite=suite,
        stage=stage,
        command_text=command_text,
    )
    return session_module.execute_suite_session(
        request,
        repo_root=repo_root,
        host_platform=host_platform,
        suite_spec=suite_spec,
        legacy_command=legacy_command,
        legacy_executor=_execute_legacy_command,
    )


def _handle_public_test_dispatch(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict,
    options: dict,
) -> CommandResult:
    command_id = command["id"]
    if command_id == "test-list":
        return _handle_public_test_list(command_text, host_platform, options, manifest)

    if command_id in {"test-all", "test-family-all"}:
        if options.get("strict"):
            try:
                catalog_module.scan_catalog(repo_root, host_platform=host_platform, strict=True)
            except ValueError:
                return CommandResult.failure(
                    command=command_text,
                    host_platform=host_platform,
                    target=options.get("family") or "all",
                    errors=["invalid suites found while scanning catalog"],
                    payload={"exitCode": 2},
                    text="invalid suites found while scanning catalog\n",
                )

        family = options.get("family")
        items = list_public_test_suites(manifest, host_platform)
        if family:
            items = [item for item in items if item["family"] == family]

        if not items:
            return CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=family,
                errors=["no matching test suites"],
                text="no matching test suites\n",
            )

        outputs: list[str] = []
        suite_results: list[dict] = []
        artifacts: list[str] = []
        for item in items:
            session_result = _execute_public_test_session(
                item["family"],
                item["suite"],
                options.get("stage", "all"),
                repo_root,
                host_platform,
                f"test {item['family']} {item['suite']}",
                manifest,
            )
            outputs.append(session_result.text or "")
            suite_results.extend(session_result.suite_results)
            artifacts.extend(session_result.artifacts)
            if session_result.status != "ok":
                result = CommandResult.failure(
                    command=command_text,
                    host_platform=host_platform,
                    target=family or "all",
                    errors=session_result.errors or [f"batch execution failed while running {item['id']}"],
                    payload={"items": items, "suiteResults": suite_results, "artifacts": artifacts, "exitCode": session_result.exit_code},
                    text="".join(outputs),
                )
                return _attach_session_report(result, repo_root, host_platform, command_text)

        result = CommandResult.success(
            command=command_text,
            host_platform=host_platform,
            target=family or "all",
            payload={"items": items, "suiteResults": suite_results, "artifacts": artifacts, "exitCode": 0},
            text="".join(outputs),
        )
        return _attach_session_report(result, repo_root, host_platform, command_text)

    family = options.get("family")
    suite = options.get("suite")
    stage = options.get("stage", "all")
    session_result = _execute_public_test_session(family, suite, stage, repo_root, host_platform, command_text, manifest)
    return _attach_session_report(session_result.to_command_result(), repo_root, host_platform, command_text)


def _attach_session_report(
    result: CommandResult,
    repo_root: Path,
    host_platform: str,
    command_text: str,
) -> CommandResult:
    try:
        report = reporting_module.write_session_report(
            repo_root=repo_root,
            host_platform=host_platform,
            command_text=command_text,
            status=result.status,
            suite_results=list(result.payload.get("suiteResults", [])),
            text=result.text or "",
            errors=list(result.errors),
            artifacts=list(result.payload.get("artifacts", [])),
        )
    except OSError:
        return result

    payload = dict(result.payload)
    payload.update(report)
    return CommandResult(
        command=result.command,
        status=result.status,
        host_platform=result.host_platform,
        target=result.target,
        duration_ms=result.duration_ms,
        checks=result.checks,
        errors=result.errors,
        payload=payload,
        text=result.text,
    )


def _success(command_text: str, host_platform: str, target: str | None, output: str, artifacts: list[str]) -> CommandResult:
    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=target,
        payload={"artifacts": artifacts},
        text=(output + "\n") if output else "",
    )


def _failure(command_text: str, host_platform: str, target: str | None, output: str, errors: list[str]) -> CommandResult:
    message = output if output else "\n".join(errors)
    if message and not message.endswith("\n"):
        message += "\n"
    return CommandResult.failure(
        command=command_text,
        host_platform=host_platform,
        target=target,
        errors=errors,
        text=message,
    )


def _ensure_smoke_build(command: dict, repo_root: Path, host_platform: str, command_text: str) -> tuple[bool, str, list[str]]:
    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        return False, bootstrap.output, bootstrap.errors

    project_path = repo_root / command["project_path"]
    completed = run_process(["dotnet", "build", str(project_path), "-c", "Release"], cwd=repo_root)
    output = "\n".join(part for part in [bootstrap.output.strip(), combine_process_output(completed)] if part)
    if completed.returncode != 0:
        return False, output, ["smoke build prerequisite failed"]
    return True, output, []


def _run_smoke_project(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    build_ok, build_output, build_errors = _ensure_smoke_build(command, repo_root, host_platform, command_text)
    if not build_ok:
        return _failure(command_text, host_platform, command.get("target"), build_output, build_errors)

    dll_path = repo_root / command["dll_path"]
    completed = run_process(["dotnet", str(dll_path)], cwd=repo_root)
    runtime_output = combine_process_output(completed)
    output = "\n".join(part for part in [build_output, runtime_output] if part)
    if completed.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["managed smoke execution failed"])

    for expected in command["expected_patterns"]:
        if expected not in runtime_output:
            return _failure(command_text, host_platform, command.get("target"), output, [f"missing expected smoke output: {expected}"])

    return _success(command_text, host_platform, command.get("target"), output, [str(dll_path)])


def _run_trace_compare(command: dict, repo_root: Path, host_platform: str, command_text: str) -> CommandResult:
    bootstrap = tooling_module.ensure_dotnet_available(command_text, host_platform)
    if not bootstrap.ready:
        return _failure(command_text, host_platform, command.get("target"), bootstrap.output, bootstrap.errors)

    host_embedding_project = repo_root / "tests" / "smoke" / "input" / "HostEmbeddingLite" / "HostEmbeddingLite.csproj"
    build = run_process(["dotnet", "build", str(host_embedding_project), "-c", "Release"], cwd=repo_root)
    build_output = "\n".join(part for part in [bootstrap.output.strip(), combine_process_output(build)] if part)
    if build.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), build_output, ["HostEmbeddingLite build failed"])

    host_embedding_dll = repo_root / "artifacts" / "smoke" / "bin" / "HostEmbeddingLite" / "Release" / "net8.0" / "HostEmbeddingLite.dll"
    trace_output = repo_root / command["actual_trace_path"]
    trace_output.parent.mkdir(parents=True, exist_ok=True)

    export = run_process(
        [
            "dotnet",
            str(host_embedding_dll),
            "--trace-platform",
            command["trace_platform"],
            "--trace-output",
            str(trace_output),
        ],
        cwd=repo_root,
    )
    export_output = combine_process_output(export)
    if export.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), "\n".join(part for part in [build_output, export_output] if part), ["trace export failed"])

    if host_platform == "windows":
        compare_args = [
            "powershell",
            "-NoLogo",
            "-NoProfile",
            "-NonInteractive",
            "-File",
            str(repo_root / "tests" / "contract" / "trace" / "compare-warmup-trace.ps1"),
            "-ExpectedPath",
            str(repo_root / command["expected_trace_path"]),
            "-ActualPath",
            str(trace_output),
        ]
    else:
        compare_args = [
            "sh",
            str(repo_root / "tests" / "contract" / "trace" / "compare-warmup-trace.sh"),
            str(repo_root / command["expected_trace_path"]),
            str(trace_output),
        ]

    compare = run_process(compare_args, cwd=repo_root)
    compare_output = combine_process_output(compare)
    output = "\n".join(part for part in [build_output, export_output, compare_output] if part)
    if compare.returncode != 0:
        return _failure(command_text, host_platform, command.get("target"), output, ["trace compare failed"])

    return _success(command_text, host_platform, command.get("target"), output, [str(trace_output)])


def handle(
    command: dict,
    repo_root: Path,
    host_platform: str,
    command_text: str,
    manifest: dict | None = None,
    options: dict | None = None,
) -> CommandResult:
    if command["id"] in {"test-family-suite", "test-family-all", "test-all", "test-list"}:
        return _handle_public_test_dispatch(command, repo_root, host_platform, command_text, manifest or {}, options or {})

    kind = command["kind"]
    if kind == "smoke-run":
        return _run_smoke_project(command, repo_root, host_platform, command_text)
    if kind == "trace-compare":
        return _run_trace_compare(command, repo_root, host_platform, command_text)

    return _failure(command_text, host_platform, command.get("target"), "", [f"unsupported test kind: {kind}"])
