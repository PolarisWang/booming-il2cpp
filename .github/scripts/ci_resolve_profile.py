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
        # Failing hard (rather than returning an empty set) is essential: an empty
        # set flows into the orchestrator's `contains(needs.resolve.outputs.stages, ...)`
        # gates and silently disables every downstream stage job — the pipeline would
        # show green while running NO tests.  A missing PyYAML is a toolchain misconfig
        # that must surface as a red workflow, not a false-green skip.
        raise RuntimeError(
            "PyYAML is required to resolve the pipeline profile (import failed). "
            "Install it on the runner: python3 -m pip install pyyaml"
        )

    with open(path, encoding="utf-8") as fh:
        try:
            cfg = yaml.safe_load(fh)
        except yaml.YAMLError as exc:
            raise RuntimeError(f"Failed to parse pipeline profile {path}: {exc}") from exc

    if not isinstance(cfg, dict):
        raise RuntimeError(f"Pipeline profile {path} must be a YAML mapping (got {type(cfg).__name__})")

    stages_cfg = cfg.get("stages", {})
    if not isinstance(stages_cfg, dict):
        raise RuntimeError(
            f"Pipeline profile {path}: 'stages' must be a mapping of stage-name -> options "
            f"(got {type(stages_cfg).__name__}). Mis-configured profiles must fail loudly, "
            "not yield an empty enabled-set that silently skips every stage."
        )

    # Validate that every enabled stage name has a corresponding reusable workflow
    # file.  A typo'd stage name in the profile would otherwise pass resolve() and
    # only fail indirectly when the orchestrator references a non-existent job.
    stages_dir = os.path.join(repo_root, ".github", "workflows", "stages")
    enabled = []
    for name, opts in stages_cfg.items():
        if isinstance(opts, dict) and opts.get("enabled") is False:
            continue
        stage_file = os.path.join(stages_dir, f"{name}.yml")
        # Only the templates/plain stage files are expected; _template/partial are skipped.
        if not os.path.isfile(stage_file):
            raise RuntimeError(
                f"Enabled stage '{name}' (in {path}) has no matching reusable workflow "
                f"file: expected {stage_file}. Fix the stage name or create the file."
            )
        enabled.append(name)
    return enabled


def main() -> int:
    profile = sys.argv[1] if len(sys.argv) > 1 else "pr"
    repo_root = sys.argv[2] if len(sys.argv) > 2 else os.getcwd()
    try:
        result = resolve(profile, repo_root)
    except (FileNotFoundError, RuntimeError) as exc:
        # Both profile-not-found and hard-failures (missing PyYAML / malformed profile /
        # stage/name/file mismatch) must surface as a red workflow, never an empty
        # enabled-set that silently skips every downstream stage.
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
