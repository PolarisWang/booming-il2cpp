from __future__ import annotations

import sys
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from base import BaseAdapter, DiscoveredCase, finalize_discovered_cases, normalize_case_id
    from dotnet_app import DotnetAppAdapter
    from native_cmake import NativeCmakeAdapter
    from python_pytest import PythonPytestAdapter
    from shell_script import ShellScriptAdapter
    from workflow import WorkflowAdapter
else:
    from .base import BaseAdapter, DiscoveredCase, finalize_discovered_cases, normalize_case_id
    from .dotnet_app import DotnetAppAdapter
    from .native_cmake import NativeCmakeAdapter
    from .python_pytest import PythonPytestAdapter
    from .shell_script import ShellScriptAdapter
    from .workflow import WorkflowAdapter


_ADAPTERS = {
    "python-pytest": PythonPytestAdapter,
    "dotnet-app": DotnetAppAdapter,
    "native-cmake": NativeCmakeAdapter,
    "shell-script": ShellScriptAdapter,
    "workflow": WorkflowAdapter,
}


def supported_runner_types() -> list[str]:
    return sorted(_ADAPTERS)


def create_adapter(suite) -> BaseAdapter:
    adapter_class = _ADAPTERS.get(suite.runner_type)
    if adapter_class is None:
        raise KeyError(f"unsupported runner type: {suite.runner_type}")
    return adapter_class(suite)


__all__ = [
    "BaseAdapter",
    "DiscoveredCase",
    "DotnetAppAdapter",
    "NativeCmakeAdapter",
    "PythonPytestAdapter",
    "ShellScriptAdapter",
    "WorkflowAdapter",
    "create_adapter",
    "finalize_discovered_cases",
    "normalize_case_id",
    "supported_runner_types",
]
