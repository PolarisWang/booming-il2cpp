"""Unified path setup for the foundation-dll verification engine.

The engine's on-disk layout is intentionally decoupled from where the 28-family
tree and the shared `_pipeline` support package live. This module is the single
source of truth for resolving those roots:

  * foundation_root()  — dir holding `config/pipeline-config.yaml` + the
                         System.* family outputs. Resolved via the
                         CHAOS_FOUNDATION_DLL env override, else by walking up
                         from this package to find the pipeline-config marker.
  * testing_tree_root()— dir put on sys.path to make the `_pipeline` support
                         package importable. Resolved via CHAOS_TESTING_DIR,
                         else parent-of-foundation-root.

Module import also registers this package (verification/) on sys.path so the
engine's subpackages are importable regardless of where it was installed.
"""
import os
import sys
from pathlib import Path

_HERE = Path(__file__).resolve().parent  # verification/
if str(_HERE) not in sys.path:
    sys.path.insert(0, str(_HERE))

_PIPELINE_CONFIG_MARKER = Path("config") / "pipeline-config.yaml"


def _find_root_with_marker(start: Path) -> Path | None:
    """Walk up from `start` to the first dir containing config/pipeline-config.yaml."""
    cur = start
    while True:
        if (cur / _PIPELINE_CONFIG_MARKER).is_file():
            return cur
        nxt = cur.parent
        if nxt == cur:  # reached filesystem root
            return None
        cur = nxt


def foundation_root() -> Path:
    """The foundation-dll root (holds config/ + System.* family outputs)."""
    override = os.environ.get("CHAOS_FOUNDATION_DLL")
    if override:
        return Path(override)
    found = _find_root_with_marker(_HERE)
    if found is not None:
        return found
    # Fall back to parent-of-package (historical layout inference).
    return _HERE.parent


def testing_tree_root() -> Path:
    """The sys.path root that makes the `_pipeline` package importable."""
    override = os.environ.get("CHAOS_TESTING_DIR")
    if override:
        return Path(override)
    return foundation_root().parent
