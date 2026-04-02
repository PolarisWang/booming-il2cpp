from __future__ import annotations

import itertools
import sys
from pathlib import Path

try:
    from ..common import write_json
    from ..result import CommandResult
    from .. import manifest as manifest_module
    from . import build as build_commands
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import write_json
    from result import CommandResult
    import manifest as manifest_module
    from commands import build as build_commands


SMOKE_BUILD_IDS = [
    "build-smoke-helloworld",
    "build-smoke-genericecho",
    "build-smoke-reflectionlite",
    "build-smoke-pinvokelite",
    "build-smoke-hostembeddinglite",
]

VERIFY_WINDOWS_BUILD_IDS = [
    "build-native-contract-abi",
    "build-native-contract-bridge",
    *SMOKE_BUILD_IDS,
    "build-preset-windows-x64-reference",
    "build-platform-android-arm64-smoke",
    "build-platform-linux-x64-packaging",
]

VERIFY_MACOS_BUILD_IDS = [
    "build-native-contract-abi",
    "build-native-contract-bridge",
    *SMOKE_BUILD_IDS,
    "build-preset-macos-reference",
    "build-platform-ios-arm64-packaging",
    "build-platform-linux-x64-packaging",
]


def resolve_prepare_scope(command_id: str) -> str:
    mapping = {
        "prepare": "global",
        "prepare-smoke": "smoke",
        "prepare-verify-roadmap-0-windows": "verify-roadmap0-windows",
        "prepare-verify-roadmap-0-macos": "verify-roadmap0-macos",
    }
    return mapping[command_id]


def _prepare_plan(scope: str, host_platform: str) -> list[str]:
    if scope == "smoke":
        return list(SMOKE_BUILD_IDS)
    if scope == "verify-roadmap0-windows":
        return list(VERIFY_WINDOWS_BUILD_IDS)
    if scope == "verify-roadmap0-macos":
        return list(VERIFY_MACOS_BUILD_IDS)
    if scope == "global":
        host_specific = VERIFY_WINDOWS_BUILD_IDS if host_platform == "windows" else VERIFY_MACOS_BUILD_IDS if host_platform == "macos" else SMOKE_BUILD_IDS
        return list(dict.fromkeys(itertools.chain(SMOKE_BUILD_IDS, host_specific)))
    raise KeyError(f"unknown prepare scope: {scope}")


def prepare_state_path(repo_root: Path, scope: str) -> Path:
    return repo_root / "artifacts" / "run" / "prepare" / f"{scope}.json"


def resolve_clean_paths(repo_root: Path, scope: str) -> list[Path]:
    smoke_root = repo_root / "artifacts" / "smoke"
    run_root = repo_root / "artifacts" / "run"
    verify_root = repo_root / "artifacts" / "verify-roadmap-0"
    preset_root = repo_root / "artifacts" / "presets"

    if scope == "smoke":
        return [smoke_root, prepare_state_path(repo_root, "smoke")]
    if scope == "verify-roadmap0-windows":
        return [
            verify_root / "windows",
            preset_root / "windows-x64-reference",
            run_root / "native-contract-abi",
            run_root / "native-contract-bridge",
            run_root / "platform" / "android-arm64-smoke",
            run_root / "platform" / "linux-x64-packaging",
            prepare_state_path(repo_root, scope),
        ]
    if scope == "verify-roadmap0-macos":
        return [
            verify_root / "macos",
            preset_root / "macos-reference",
            run_root / "native-contract-abi",
            run_root / "native-contract-bridge",
            run_root / "platform" / "ios-arm64-packaging",
            run_root / "platform" / "linux-x64-packaging",
            prepare_state_path(repo_root, scope),
        ]
    if scope == "global":
        return [
            smoke_root,
            run_root,
            verify_root,
            preset_root / "windows-x64-reference",
            preset_root / "macos-reference",
            prepare_state_path(repo_root, "global"),
        ]
    raise KeyError(f"unknown clean scope: {scope}")


def handle(command: dict, repo_root: Path, host_platform: str, command_text: str, manifest: dict) -> CommandResult:
    scope = resolve_prepare_scope(command["id"])
    state_path = prepare_state_path(repo_root, scope)
    if state_path.is_file():
        return CommandResult.success(
            command=command_text,
            host_platform=host_platform,
            target=scope,
            payload={"prepareScope": scope, "preparedCommands": [], "cached": True},
            text=f"prepare scope '{scope}' already available\n",
        )

    plan_ids = _prepare_plan(scope, host_platform)
    executed: list[str] = []
    outputs: list[str] = []

    for command_id in plan_ids:
        entry = manifest_module.find_command(manifest, command_id, host_platform)
        if entry is None:
            continue

        syntax = " ".join(entry["tokens"])
        result = build_commands.handle(entry, repo_root, host_platform, syntax)
        outputs.append(result.text or "")
        if result.status != "ok":
            return CommandResult.failure(
                command=command_text,
                host_platform=host_platform,
                target=scope,
                errors=[f"prepare failed while executing {command_id}"],
                payload={"prepareScope": scope, "preparedCommands": executed},
                text="".join(outputs),
            )
        executed.append(command_id)

    write_json(
        state_path,
        {
            "scope": scope,
            "hostPlatform": host_platform,
            "preparedCommands": executed,
        },
    )

    return CommandResult.success(
        command=command_text,
        host_platform=host_platform,
        target=scope,
        payload={"prepareScope": scope, "preparedCommands": executed, "cached": False},
        text="".join(outputs) if outputs else f"prepare scope '{scope}' completed\n",
    )
