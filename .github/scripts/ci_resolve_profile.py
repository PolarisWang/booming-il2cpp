#!/usr/bin/env python3
"""Resolve which stages a pipeline profile enables, and emit as GITHUB_OUTPUT.

Usage:
    python3 ci_resolve_profile.py <profile> <repo_root>

Reads `.github/pipelines/<profile>.yml`, collects stage names whose `enabled`
is not False, and writes them to GITHUB_OUTPUT as `stages=<json-array-of-names>`.

Design: the orchestrator workflow later gates each stage job with
`contains(fromJson(needs.resolve.outputs.stages), '<stage>')`.  Keeping this
logic in a committed script (rather than a bash-`-c` inline) is robust, lintable
and unit-testable — and avoids YAML-in-bash escaping fragility.
"""
from __future__ import annotations

import json
import os
import sys

try:
    import yaml  # PyYAML
except ImportError:
    yaml = None  # type: ignore[assignment]


def resolve(profile: str, repo_root: str) -> list[str]:
    """Return the list of enabled stage names for a profile."""
    path = os.path.join(repo_root, ".github", "pipelines", f"{profile}.yml")
    if not os.path.isfile(path):
        # No PyYAML available in runner and no file to statically fall back to.
        raise FileNotFoundError(f"Pipeline profile not found: {path}")

    if yaml is None:
        # Degenerate: without yaml we cannot parse; empty set (nothing enabled).
        return []

    with open(path, encoding="utf-8") as fh:
        cfg = yaml.safe_load(fh)
    stages_cfg = cfg.get("stages", {}) if isinstance(cfg, dict) else {}
    enabled = [
        name for name, opts in stages_cfg.items()
        if not (isinstance(opts, dict) and opts.get("enabled") is False)
    ]
    return enabled


def main() -> int:
    profile = sys.argv[1] if len(sys.argv) > 1 else "pr"
    repo_root = sys.argv[2] if len(sys.argv) > 2 else os.getcwd()
    try:
        result = resolve(profile, repo_root)
    except FileNotFoundError as exc:
        print(f"::error::{exc}", file=sys.stderr)
        return 1

    payload = json.dumps(result)
    # Emit for GitHub Actions
    try:
        out = os.environ["GITHUB_OUTPUT"]
        with open(out, "a", encoding="utf-8") as fh:
            fh.write(f"stages={payload}\n")
    except KeyError:
        # Local run: print it for inspection.
        print(f"[local] profile={profile} enabled_stages={payload}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
