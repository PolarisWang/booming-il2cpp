from __future__ import annotations

import argparse
import hashlib
import json
import os
import platform
import shutil
import sys
import tarfile
import urllib.parse
import urllib.request
import zipfile
from pathlib import Path
from typing import Any

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from common import join_relative, read_json, write_json
    from result import CommandResult
else:
    from .common import join_relative, read_json, write_json
    from .result import CommandResult


DEFAULT_MANIFEST = Path("build/toolchains/run/runtime_manifest.json")


def resolve_repo_root(repo_root: str | None = None) -> Path:
    candidate = repo_root or os.environ.get("CHAOS_RUN_REPO_ROOT")
    if candidate:
        return Path(candidate).resolve()
    return Path(__file__).resolve().parents[3]


def resolve_manifest_path(repo_root: Path, manifest_path: str | Path | None = None) -> Path:
    candidate = manifest_path or os.environ.get("CHAOS_RUN_RUNTIME_MANIFEST")
    if candidate is None:
        return (repo_root / DEFAULT_MANIFEST).resolve()

    path = Path(candidate)
    if not path.is_absolute():
        path = repo_root / path
    return path.resolve()


def load_runtime_manifest(repo_root: Path, manifest_path: str | Path | None = None) -> dict[str, Any]:
    resolved_path = resolve_manifest_path(repo_root, manifest_path)
    return read_json(resolved_path)


def get_platform_entry(manifest: dict[str, Any], platform_id: str) -> dict[str, Any]:
    platforms = manifest["platforms"]
    if platform_id not in platforms:
        raise KeyError(f"unsupported platform id: {platform_id}")
    return platforms[platform_id]


def get_cache_root(repo_root: Path, manifest: dict[str, Any]) -> Path:
    return (repo_root / Path(manifest["cache_root"])).resolve()


def get_current_state_path(repo_root: Path, manifest: dict[str, Any]) -> Path:
    return get_cache_root(repo_root, manifest) / "current.json"


def get_runtime_root(repo_root: Path, manifest: dict[str, Any], platform_id: str) -> Path:
    return get_cache_root(repo_root, manifest) / platform_id / manifest["python_version"]


def get_runtime_python_path(repo_root: Path, manifest: dict[str, Any], platform_id: str) -> Path:
    runtime_root = get_runtime_root(repo_root, manifest, platform_id)
    entry = get_platform_entry(manifest, platform_id)
    return join_relative(runtime_root, entry["python_relpath"])


def detect_host_platform() -> str:
    system = platform.system().lower()
    machine = platform.machine().lower()

    if system == "windows":
        if machine in {"amd64", "x86_64"}:
            return "windows-x64"
        if machine in {"arm64", "aarch64"}:
            return "windows-arm64"
    elif system == "darwin":
        if machine in {"arm64", "aarch64"}:
            return "macos-arm64"
        if machine in {"x86_64", "amd64"}:
            return "macos-x64"
    elif system == "linux":
        if machine in {"x86_64", "amd64"}:
            return "linux-x64"
        if machine in {"arm64", "aarch64"}:
            return "linux-arm64"

    raise RuntimeError(f"unsupported host platform: system={system} machine={machine}")


def detect_host_platform_family(platform_id: str) -> str:
    if platform_id.startswith("windows"):
        return "windows"
    if platform_id.startswith("macos"):
        return "macos"
    if platform_id.startswith("linux"):
        return "linux"
    return platform_id


def calculate_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def file_url_to_path(url: str) -> Path:
    parsed = urllib.parse.urlparse(url)
    if parsed.scheme != "file":
        raise ValueError(f"not a file url: {url}")

    path = urllib.request.url2pathname(parsed.path)
    if os.name == "nt" and path.startswith("/") and len(path) > 2 and path[2] == ":":
        path = path[1:]
    if parsed.netloc:
        path = f"//{parsed.netloc}{path}"
    return Path(path)


def download_archive(url: str, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    parsed = urllib.parse.urlparse(url)

    if parsed.scheme in {"", "file"}:
        source = file_url_to_path(url) if parsed.scheme == "file" else Path(url)
        shutil.copyfile(source, destination)
        return

    if parsed.scheme not in {"http", "https"}:
        raise RuntimeError(f"unsupported runtime download scheme: {parsed.scheme}")

    with urllib.request.urlopen(url) as response, destination.open("wb") as stream:
        shutil.copyfileobj(response, stream)


def extract_archive(archive_path: Path, archive_type: str, destination: Path) -> None:
    if destination.exists():
        shutil.rmtree(destination)
    destination.mkdir(parents=True, exist_ok=True)

    if archive_type == "zip":
        with zipfile.ZipFile(archive_path, "r") as archive:
            archive.extractall(destination)
        return

    if archive_type in {"tar.gz", "tgz"}:
        with tarfile.open(archive_path, "r:gz") as archive:
            try:
                archive.extractall(destination, filter="fully_trusted")
            except TypeError:
                archive.extractall(destination)
        return

    raise RuntimeError(f"unsupported archive type: {archive_type}")


def write_current_state(repo_root: Path, manifest: dict[str, Any], platform_id: str, manifest_path: Path) -> None:
    state = {
        "provider": manifest["provider"],
        "provider_release": manifest["provider_release"],
        "python_version": manifest["python_version"],
        "platform_id": platform_id,
        "manifest_path": str(manifest_path),
        "python_path": str(get_runtime_python_path(repo_root, manifest, platform_id)),
    }
    write_json(get_current_state_path(repo_root, manifest), state)


def probe_runtime(
    repo_root: Path,
    manifest_path: str | Path | None = None,
    platform_id: str | None = None,
) -> dict[str, Any]:
    manifest = load_runtime_manifest(repo_root, manifest_path)
    resolved_manifest = resolve_manifest_path(repo_root, manifest_path)
    resolved_platform = platform_id or detect_host_platform()
    python_path = get_runtime_python_path(repo_root, manifest, resolved_platform)
    runtime_root = get_runtime_root(repo_root, manifest, resolved_platform)

    return {
        "provider": manifest["provider"],
        "providerRelease": manifest["provider_release"],
        "pythonVersion": manifest["python_version"],
        "platformId": resolved_platform,
        "manifestPath": str(resolved_manifest),
        "cacheRoot": str(get_cache_root(repo_root, manifest)),
        "runtimeRoot": str(runtime_root),
        "pythonPath": str(python_path),
        "isInstalled": python_path.is_file(),
        "currentStatePath": str(get_current_state_path(repo_root, manifest)),
    }


def bootstrap_runtime(
    repo_root: Path,
    manifest_path: str | Path | None = None,
    platform_id: str | None = None,
    force: bool = False,
) -> dict[str, Any]:
    manifest = load_runtime_manifest(repo_root, manifest_path)
    resolved_manifest = resolve_manifest_path(repo_root, manifest_path)
    resolved_platform = platform_id or detect_host_platform()
    entry = get_platform_entry(manifest, resolved_platform)
    cache_root = get_cache_root(repo_root, manifest)
    runtime_root = get_runtime_root(repo_root, manifest, resolved_platform)
    python_path = get_runtime_python_path(repo_root, manifest, resolved_platform)

    if python_path.is_file() and not force:
        write_current_state(repo_root, manifest, resolved_platform, resolved_manifest)
        return {
            "status": "already-installed",
            "platformId": resolved_platform,
            "runtimeRoot": str(runtime_root),
            "pythonPath": str(python_path),
        }

    downloads_root = cache_root / ".downloads"
    archive_name = Path(urllib.parse.urlparse(entry["download_url"]).path).name or "runtime.archive"
    archive_path = downloads_root / archive_name

    download_archive(entry["download_url"], archive_path)
    actual_sha256 = calculate_sha256(archive_path)
    expected_sha256 = entry["sha256"].lower()
    if actual_sha256.lower() != expected_sha256:
        raise RuntimeError(
            f"runtime archive sha256 mismatch: expected {expected_sha256} actual {actual_sha256.lower()}"
        )

    extract_archive(archive_path, entry["archive_type"], runtime_root)

    if entry["extract_dir"] not in {"", "."}:
        extract_root = runtime_root / entry["extract_dir"]
        if not extract_root.exists():
            raise RuntimeError(f"runtime extract dir missing after extraction: {extract_root}")

    if not python_path.is_file():
        raise RuntimeError(f"runtime python executable missing after extraction: {python_path}")

    write_current_state(repo_root, manifest, resolved_platform, resolved_manifest)
    return {
        "status": "installed",
        "platformId": resolved_platform,
        "runtimeRoot": str(runtime_root),
        "pythonPath": str(python_path),
        "archivePath": str(archive_path),
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Manage the unified entrypoint Python runtime.")
    subparsers = parser.add_subparsers(dest="command", required=True)

    def add_common_arguments(command_parser: argparse.ArgumentParser) -> None:
        command_parser.add_argument("--repo-root", default=None)
        command_parser.add_argument("--manifest", default=None)
        command_parser.add_argument("--platform", default=None)
        command_parser.add_argument("--json", action="store_true")

    probe_parser = subparsers.add_parser("probe", help="Describe the current runtime state.")
    add_common_arguments(probe_parser)

    bootstrap_parser = subparsers.add_parser("bootstrap", help="Download and install the configured runtime.")
    add_common_arguments(bootstrap_parser)
    bootstrap_parser.add_argument("--force", action="store_true")
    bootstrap_parser.add_argument("--yes", action="store_true")

    return parser


def _bootstrap_payload(probe: dict[str, Any], runtime_status: str) -> dict[str, Any]:
    return {
        "runtimeStatus": runtime_status,
        "provider": probe["provider"],
        "providerRelease": probe["providerRelease"],
        "pythonVersion": probe["pythonVersion"],
        "platformId": probe["platformId"],
        "manifestPath": probe["manifestPath"],
        "cacheRoot": probe["cacheRoot"],
        "runtimeRoot": probe["runtimeRoot"],
        "pythonPath": probe["pythonPath"],
        "currentStatePath": probe["currentStatePath"],
        "isInstalled": probe["isInstalled"],
    }


def _safe_platform_id(explicit_platform: str | None) -> str:
    if explicit_platform:
        return explicit_platform
    try:
        return detect_host_platform()
    except Exception:
        return "unknown"


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    repo_root = resolve_repo_root(args.repo_root)

    try:
        if args.command == "probe":
            result = probe_runtime(repo_root, args.manifest, args.platform)
            sys.stdout.write(json.dumps(result, indent=2))
            sys.stdout.write("\n")
            return 0

        if args.command == "bootstrap":
            result = bootstrap_runtime(repo_root, args.manifest, args.platform, args.force)
            if args.json:
                probe = probe_runtime(repo_root, args.manifest, args.platform)
                summary = CommandResult.success(
                    command="bootstrap",
                    host_platform=detect_host_platform_family(probe["platformId"]),
                    target=None,
                    payload=_bootstrap_payload(probe, result["status"]),
                )
                sys.stdout.write(summary.to_json())
                sys.stdout.write("\n")
                return 0
            message = (
                "Python runtime already installed."
                if result["status"] == "already-installed"
                else "Python runtime bootstrap completed."
            )
            sys.stdout.write(message + "\n")
            return 0
    except Exception as exc:  # pragma: no cover - exercised from wrapper tests
        if getattr(args, "json", False):
            platform_id = _safe_platform_id(getattr(args, "platform", None))
            summary = CommandResult.failure(
                command=getattr(args, "command", "runtime"),
                host_platform=detect_host_platform_family(platform_id),
                target=None,
                errors=[str(exc)],
                payload={"runtimeStatus": "error", "platformId": platform_id},
            )
            sys.stdout.write(summary.to_json())
            sys.stdout.write("\n")
            return 1
        sys.stderr.write(str(exc) + "\n")
        return 1

    parser.error(f"unsupported command: {args.command}")
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
