from __future__ import annotations

import json
from pathlib import Path
from typing import Any
import sys

try:
    from ..core.common import write_json
    from ..core import tooling as tooling_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from core.common import write_json
    from core import tooling as tooling_module


def _request_path(runtime_root: Path, collector_name: str) -> Path:
    return runtime_root / f"{collector_name}.request.json"


def _write_manual_request(path: Path, payload: dict[str, Any]) -> None:
    write_json(path, payload)


def collect_android_perf(
    *,
    repo_root: Path,
    runtime_root: Path,
    native_executable_path: Path,
    runtime_arguments: list[str],
    sample_count: int,
    host_platform: str,
) -> dict[str, Any]:
    request_path = _request_path(runtime_root, "android-mobile-perf")
    _write_manual_request(
        request_path,
        {
            "collector": "adb",
            "hostPlatform": host_platform,
            "nativeExecutablePath": native_executable_path.as_posix(),
            "runtimeArguments": list(runtime_arguments),
            "sampleCount": sample_count,
        },
    )

    bootstrap = tooling_module.ensure_android_host_tooling_available(
        "mobile native perf collection",
        host_platform,
        repo_root,
    )
    if not bootstrap.ready:
        raise RuntimeError((bootstrap.output or "android host tooling bootstrap failed").strip())

    raise RuntimeError(
        "android mobile native perf collection scaffold is ready, but executing adb-driven benchmark "
        "samples still requires a connected device/emulator orchestration flow."
    )


def collect_ios_perf(
    *,
    repo_root: Path,
    runtime_root: Path,
    cmake_binary_dir: Path,
    sample_count: int,
    host_platform: str,
) -> dict[str, Any]:
    del repo_root

    request_path = _request_path(runtime_root, "ios-mobile-perf")
    _write_manual_request(
        request_path,
        {
            "collector": "xcodebuild",
            "hostPlatform": host_platform,
            "cmakeBinaryDir": cmake_binary_dir.as_posix(),
            "sampleCount": sample_count,
        },
    )

    raise RuntimeError(
        "ios mobile native perf collection scaffold is ready, but real sampling still requires "
        "macOS + Xcode test execution and result parsing."
    )
