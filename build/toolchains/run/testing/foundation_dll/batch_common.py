"""Shared utilities for batch family runners.

Provides:
  - _resolve_driver_cmd_parts() — direct exe or fallback to dotnet run
  - FamilyLogger — thread-safe per-family log collector
  - trace_init_from_args() — optional trace session initialisation
"""

from __future__ import annotations

import argparse
import os
import threading
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_DRIVER_RELEASE_DIR = (
    _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0"
)


def _resolve_driver_cmd_parts() -> list[str]:
    """Return the command prefix to invoke the chaos-il2cpp driver.

    Prefers a pre-built Release exe (avoids dotnet CLI overhead).
    Falls back to ``dotnet run --no-build`` if the exe is missing.
    """
    env_exe = os.environ.get("CHAOS_DRIVER_EXE")
    if env_exe:
        exe = Path(env_exe)
        if exe.is_file():
            return [str(exe)]
        print(f"[config] CHAOS_DRIVER_EXE={env_exe} not found, falling back")

    exe = _DRIVER_RELEASE_DIR / "Chaos.IL2CPP.Driver.exe"
    if exe.is_file():
        return [str(exe)]

    print("[config] Release driver exe not found, using dotnet run --no-build")
    return [
        "dotnet", "run", "--no-build",
        "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
        "--",
    ]


class FamilyLogger:
    """Collects log lines for one family, flushed atomically by the main thread."""

    _lock = threading.Lock()

    def __init__(self, family_slug: str) -> None:
        self.family_slug = family_slug
        self._lines: list[str] = []

    def log(self, message: str) -> None:
        self._lines.append(message)

    def flush(self) -> None:
        with self._lock:
            for line in self._lines:
                print(line)

    def header(self, title: str | None = None) -> None:
        self.log(f"\n{'='*60}")
        self.log(f"Family: {self.family_slug}" + (f" ({title})" if title else ""))
        self.log(f"{'='*60}")


def add_common_cli_args(parser: argparse.ArgumentParser) -> None:
    """Attach ``--max-workers`` and ``--families`` to a subparser."""
    parser.add_argument(
        "-j", "--max-workers",
        type=int, default=0,
        help="Max parallel workers (default: CPU count)",
    )
    parser.add_argument(
        "--families",
        nargs="*",
        help="Space-separated subset of family slugs to process (for testing)",
    )


def filter_families(all_families: list[str], requested: list[str] | None) -> list[str]:
    """Return the subset of families to process."""
    if not requested:
        return list(all_families)
    result = [f for f in all_families if f in requested]
    missing = set(requested) - set(all_families)
    if missing:
        print(f"WARNING: unknown families: {missing}")
    return result
