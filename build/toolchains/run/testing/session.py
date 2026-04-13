from __future__ import annotations

import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable

try:
    from ..core.result import CommandResult
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.result import CommandResult


@dataclass(frozen=True)
class TestRequest:
    family: str
    suite: str
    stage: str = "all"
    command_text: str = ""

    @property
    def suite_key(self) -> str:
        return f"{self.family}/{self.suite}"


@dataclass
class SessionResult:
    request: TestRequest
    host_platform: str
    status: str
    suite_results: list[dict]
    text: str = ""
    errors: list[str] = field(default_factory=list)
    artifacts: list[str] = field(default_factory=list)
    exit_code: int = 0

    def to_command_result(self) -> CommandResult:
        factory = CommandResult.success if self.status == "ok" else CommandResult.failure
        kwargs = {
            "command": self.request.command_text,
            "host_platform": self.host_platform,
            "target": self.request.suite_key,
            "payload": {
                "suiteResults": self.suite_results,
                "artifacts": self.artifacts,
                "exitCode": self.exit_code,
            },
            "text": self.text,
        }
        if self.status != "ok":
            kwargs["errors"] = self.errors
        return factory(**kwargs)


def _stage_result(status: str, *, implicit: bool = False) -> dict:
    payload = {"status": status}
    if implicit:
        payload["implicit"] = True
    return payload


def _default_stage_results() -> dict[str, dict]:
    return {
        "build": _stage_result("not-requested"),
        "setup": _stage_result("not-requested"),
        "run": _stage_result("not-requested"),
        "report": _stage_result("not-requested"),
    }


def _suite_result_from_execution_result(request: TestRequest, suite_spec: dict, execution_result: CommandResult) -> dict:
    stage_results = _default_stage_results()
    status = "ok" if execution_result.status == "ok" else "fail"

    if request.stage == "build":
        stage_results["build"] = _stage_result(status)
    elif request.stage == "run":
        if "build" in suite_spec["stages"]:
            stage_results["build"] = _stage_result(status, implicit=True)
        stage_results["run"] = _stage_result(status)
    else:
        if "build" in suite_spec["stages"]:
            stage_results["build"] = _stage_result(status)
        stage_results["run"] = _stage_result(status)

    return {
        "suiteId": request.suite_key,
        "status": status,
        "stageResults": stage_results,
        "artifacts": list(execution_result.payload.get("artifacts", [])),
    }


def _incompatible_suite_result(request: TestRequest, host_platform: str) -> SessionResult:
    text = f"test suite `{request.suite_key}` is not supported on host `{host_platform}`\n"
    return SessionResult(
        request=request,
        host_platform=host_platform,
        status="error",
        suite_results=[],
        text=text,
        errors=[f"test suite is not supported on host: {host_platform}"],
        exit_code=2,
    )


def _unsupported_target_result(request: TestRequest, host_platform: str) -> SessionResult:
    text = f"unsupported unified test target: {request.suite_key}\n"
    return SessionResult(
        request=request,
        host_platform=host_platform,
        status="error",
        suite_results=[],
        text=text,
        errors=[f"unsupported unified test target: {request.suite_key}"],
        exit_code=2,
    )


def _unsupported_stage_result(request: TestRequest, host_platform: str) -> SessionResult:
    text = f"test suite `{request.suite_key}` does not support stage `{request.stage}`\n"
    return SessionResult(
        request=request,
        host_platform=host_platform,
        status="error",
        suite_results=[],
        text=text,
        errors=[f"unsupported stage for suite: {request.stage}"],
        exit_code=2,
    )


def execute_suite_session(
    request: TestRequest,
    *,
    repo_root: Path,
    host_platform: str,
    suite_spec: dict | None,
    suite_executor: Callable[[dict, Path, str, str, str], CommandResult],
) -> SessionResult:
    if suite_spec is None:
        return _unsupported_target_result(request, host_platform)

    supported_hosts = suite_spec.get("supported_hosts") or []
    if supported_hosts and host_platform not in supported_hosts:
        return _incompatible_suite_result(request, host_platform)

    if request.stage not in suite_spec["stages"]:
        return _unsupported_stage_result(request, host_platform)

    execution_result = suite_executor(
        suite_spec,
        repo_root,
        host_platform,
        request.command_text,
        request.stage,
    )
    suite_result = _suite_result_from_execution_result(request, suite_spec, execution_result)
    return SessionResult(
        request=request,
        host_platform=host_platform,
        status=execution_result.status,
        suite_results=[suite_result],
        text=execution_result.text or "",
        errors=list(execution_result.errors),
        artifacts=list(execution_result.payload.get("artifacts", [])),
        exit_code=0 if execution_result.status == "ok" else 1,
    )
