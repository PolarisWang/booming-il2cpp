#!/usr/bin/env python3
"""Generate a provenance manifest for a CI run's artifacts.

Why
---
Every stage uploads artifacts, but nothing records *what produced them*.  When a
baseline, benchmark record, or SDK build is later questioned, the run is only
reconstructable from the GitHub UI.  This writes a machine-readable sidecar that
travels with the artifact so consumers can assert compatibility (same commit,
same toolchain, same runner class) instead of guessing.

Usage
-----
    python3 ci_provenance.py write --out provenance.json \
        --artifact <name>=<path> [--artifact ...]

    python3 ci_provenance.py write --out provenance.json \
        --artifact-dir dist

Environment (all optional; absent values are recorded as null rather than
guessed, so a partial manifest is never mistaken for a complete one):
    GITHUB_SHA, GITHUB_REF, GITHUB_RUN_ID, GITHUB_RUN_ATTEMPT, GITHUB_JOB,
    GITHUB_WORKFLOW, GITHUB_REPOSITORY, GITHUB_EVENT_NAME, GITHUB_ACTOR,
    RUNNER_OS, RUNNER_ARCH, IMAGE_OS (runner image)
    CHAOS_PROVENANCE_STAGE, CHAOS_PROVENANCE_PROFILE (set by the orchestrator)
    CHAOS_VCPKG_BASELINE, CHAOS_LLVM_VERSION, CHAOS_DOTNET_VERSION (toolchain)

Exit codes: 0 on success, 2 on usage error.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import platform
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

SCHEMA_VERSION = 1
_HASH_CHUNK = 1 << 20  # 1 MiB


def _env(*names: str) -> str | None:
    """First non-empty env var among names, else None (never empty string)."""
    for n in names:
        v = os.environ.get(n)
        if v:
            return v
    return None


def _env_int(*names: str) -> int | None:
    """As _env, but coerced to int — run ids must not be strings, so that
    downstream consumers can compare/order them without parsing."""
    v = _env(*names)
    if v is None:
        return None
    try:
        return int(v)
    except ValueError:
        return None


def _git(*args: str, cwd: Path | None = None) -> str | None:
    """Run a git command, returning stripped stdout or None on any failure.

    Provenance must never fail the build: a missing git binary or a detached
    shallow checkout degrades the manifest to null fields, it does not abort.
    """
    try:
        out = subprocess.run(
            ["git", *args],
            cwd=str(cwd) if cwd else None,
            capture_output=True,
            text=True,
            timeout=15,
            check=False,
        )
        return out.stdout.strip() or None if out.returncode == 0 else None
    except (OSError, subprocess.SubprocessError):
        return None


def sha256_file(path: Path) -> str | None:
    """Streamed SHA-256 of a file; None if unreadable."""
    try:
        h = hashlib.sha256()
        with path.open("rb") as f:
            while chunk := f.read(_HASH_CHUNK):
                h.update(chunk)
        return h.hexdigest()
    except OSError:
        return None


def _dir_fingerprint(path: Path, max_files: int = 5000) -> dict:
    """Order-independent fingerprint over a directory tree.

    For artifact directories we record the file count, total size, and a digest
    over the sorted (relpath, size) list.  This is deliberately not a hash of
    every byte — artifact trees can be many GB and the point is to detect
    *different* trees, not to make the manifest the expensive part of the job.
    """
    files: list[tuple[str, int]] = []
    total = 0
    try:
        for p in sorted(path.rglob("*")):
            if not p.is_file():
                continue
            try:
                sz = p.stat().st_size
            except OSError:
                continue
            files.append((str(p.relative_to(path)).replace("\\", "/"), sz))
            total += sz
            if len(files) >= max_files:
                break
    except OSError:
        return {"path": str(path), "error": "unreadable"}

    if not files:
        # An empty dir and a missing dir are different facts; keep them distinct.
        return {"path": str(path), "file_count": 0, "total_bytes": 0,
                "fingerprint": None, "truncated": False}

    h = hashlib.sha256()
    for rel, sz in files:
        h.update(f"{rel}\0{sz}\n".encode())
    return {
        "path": str(path),
        "file_count": len(files),
        "total_bytes": total,
        "fingerprint": h.hexdigest(),
        "truncated": len(files) >= max_files,
    }


def _describe(name: str, target: Path) -> dict:
    """Provenance record for one named artifact (a file or a directory)."""
    rec: dict = {"name": name}
    if target.is_dir():
        rec["kind"] = "directory"
        rec.update(_dir_fingerprint(target))
    elif target.is_file():
        rec["kind"] = "file"
        rec["path"] = str(target)
        rec["size_bytes"] = target.stat().st_size
        rec["sha256"] = sha256_file(target)
    else:
        # Absent artifacts are recorded, not omitted — a consumer asking "was X
        # produced?" needs a definite no, not a missing key.
        rec["kind"] = "missing"
        rec["path"] = str(target)
    return rec


def build_manifest(repo_root: Path, artifacts: list[tuple[str, Path]]) -> dict:
    now = datetime.now(timezone.utc).isoformat()
    return {
        "schemaVersion": SCHEMA_VERSION,
        "generatedAt": now,
        "run": {
            "repository": _env("GITHUB_REPOSITORY"),
            "workflow": _env("GITHUB_WORKFLOW"),
            "job": _env("GITHUB_JOB"),
            "runId": _env_int("GITHUB_RUN_ID"),
            "runAttempt": _env_int("GITHUB_RUN_ATTEMPT"),
            "eventName": _env("GITHUB_EVENT_NAME"),
            "actor": _env("GITHUB_ACTOR"),
            "ref": _env("GITHUB_REF"),
            "profile": _env("CHAOS_PROVENANCE_PROFILE"),
            "stage": _env("CHAOS_PROVENANCE_STAGE"),
        },
        "source": {
            # GITHUB_SHA is the authoritative commit for the run; the git
            # fallback covers local/manual invocations of this script.
            "commit": _env("GITHUB_SHA") or _git("rev-parse", "HEAD", cwd=repo_root),
            "shortCommit": _git("rev-parse", "--short", "HEAD", cwd=repo_root),
            "branch": _git("rev-parse", "--abbrev-ref", "HEAD", cwd=repo_root),
            "dirty": bool(_git("status", "--porcelain", cwd=repo_root)),
            "commitDate": _git("log", "-1", "--format=%cI", cwd=repo_root),
            "subject": _git("log", "-1", "--format=%s", cwd=repo_root),
        },
        "runner": {
            "os": _env("RUNNER_OS"),
            "arch": _env("RUNNER_ARCH"),
            "imageOs": _env("IMAGE_OS"),
            "hostname": platform.node() or None,
            "pythonVersion": platform.python_version(),
        },
        "toolchain": {
            # Explicit overrides win; unset means "not captured for this stage",
            # which is honest — do not substitute a locally-detected version,
            # since this script often runs on a different host than the build.
            "vcpkgBaseline": _env("CHAOS_VCPKG_BASELINE"),
            "llvmVersion": _env("CHAOS_LLVM_VERSION"),
            "dotnetVersion": _env("CHAOS_DOTNET_VERSION"),
            "cmakeVersion": _env("CHAOS_CMAKE_VERSION"),
        },
        "artifacts": [_describe(n, p) for n, p in artifacts],
    }


def _collect_artifacts(args) -> list[tuple[str, Path]]:
    out: list[tuple[str, Path]] = []
    for spec in args.artifact or []:
        name, sep, path = spec.partition("=")
        if not sep or not name:
            raise ValueError(f"--artifact expects NAME=PATH, got {spec!r}")
        out.append((name, Path(path)))
    if args.artifact_dir:
        d = Path(args.artifact_dir)
        for child in sorted(d.iterdir()) if d.is_dir() else []:
            out.append((child.name, child))
    return out


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    w = sub.add_parser("write", help="Write a provenance manifest")
    w.add_argument("--out", required=True, help="Manifest output path")
    w.add_argument("--repo-root", default=".", help="Repository root (default: .)")
    w.add_argument("--artifact", action="append", metavar="NAME=PATH",
                   help="Named artifact to fingerprint (repeatable)")
    w.add_argument("--artifact-dir", metavar="DIR",
                   help="Fingerprint every child of DIR as an artifact")

    args = ap.parse_args(argv)
    if args.cmd != "write":
        ap.error(f"unknown command {args.cmd!r}")

    try:
        artifacts = _collect_artifacts(args)
    except ValueError as e:
        print(f"ERROR: {e}", file=sys.stderr)
        return 2

    manifest = build_manifest(Path(args.repo_root).resolve(), artifacts)

    out = Path(args.out)
    if out.parent and str(out.parent):
        out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n",
                   encoding="utf-8")

    print(f"provenance -> {out}")
    print(f"  commit   {manifest['source']['commit']}")
    print(f"  run      {manifest['run']['runId']} attempt {manifest['run']['runAttempt']}")
    print(f"  stage    {manifest['run']['stage']} profile {manifest['run']['profile']}")
    print(f"  artifacts {len(manifest['artifacts'])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
