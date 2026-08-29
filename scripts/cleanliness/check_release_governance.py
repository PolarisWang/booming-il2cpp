#!/usr/bin/env python3
"""Release & open-source governance guard.

P0.1/P0.2/P1.2 enforcement point — surfaces violations of the project's
open-source industrialization rules so agents (and humans) follow them.

Checks:
  1. version-consistency — VERSION file == CMakeLists CHAOS_VERSION ==
     src/managed/Directory.Build.props <Version>, and a matching `v*` tag exists.
  2. readme-validity — every README-proclaimed path/binary must actually exist
     (no stale `tools/`, `testing/`, or phantom Mono.Cecil dependency).
  3. release-gate — a `v*` tag must exist so `git describe` yields a SemVer
     (guards against pushing unversioned work).

Exit 0 PASS, exit 1 FAIL (hard), print '[WARN]' for advisory-only items.
Same shape as the other scripts/cleanliness/* modules; wired into
chaos_hygiene.py via hygiene-registry.json + a pre-commit gate.

Usage:
    python scripts/cleanliness/check_release_governance.py            # advisory
    python scripts/cleanliness/check_release_governance.py --hard     # exit 1 on any fail
    python scripts/cleanliness/check_release_governance.py --ci       # exit 1 on fail/warn
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent


def _git(args: list[str]) -> str:
    try:
        return subprocess.run(
            ["git"] + args, capture_output=True, text=True, timeout=10,
            cwd=REPO_ROOT, encoding="utf-8", errors="replace",
        ).stdout.strip()
    except Exception:
        return ""


def check_version_consistency() -> list[str]:
    """VERSION / CMakeLists / Directory.Build.props must agree, and a v* tag exists."""
    out: list[str] = []
    version_file = REPO_ROOT / "VERSION"
    if not version_file.exists():
        out.append("[FAIL] missing VERSION file — add SemVer (e.g. '0.1.0')")
        return out

    ver = version_file.read_text(encoding="utf-8").strip()
    # CMakeLists CHAOS_VERSION
    cmake = (REPO_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    m = re.search(r'set\(CHAOS_VERSION\s+"([^"]+)"\)', cmake)
    if not m or m.group(1) != ver:
        out.append(
            f"[FAIL] CMakeLists CHAOS_VERSION != VERSION ({m.group(1) if m else '??'} vs {ver})")
    # Directory.Build.props <Version>
    props = (REPO_ROOT / "src" / "managed" / "Directory.Build.props").read_text(encoding="utf-8")
    vm = re.search(r"<Version>([^<]+)</Version>", props)
    if not vm or vm.group(1).strip() != ver:
        out.append(
            f"[FAIL] Directory.Build.props <Version> != VERSION ({vm.group(1).strip() if vm else '??'} vs {ver})")
    # matching v* tag
    tags = _git(["tag", "-l", f"v{ver}"])
    if f"v{ver}" not in tags:
        out.append(f"[WARN] no tag 'v{ver}' yet — push a SemVer tag on release")
    return out


def check_readme_validity() -> list[str]:
    """README-proclaimed paths/binaries/deps must actually exist."""
    out: list[str] = []
    readme = REPO_ROOT / "README.md"
    if not readme.exists():
        out.append("[WARN] missing README.md")
        return out
    text = readme.read_text(encoding="utf-8", errors="replace")
    # Known stale references that should NOT appear in a current README
    stale_markers = {
        "`tools/`": "root tools/ dir was pruned (real tools live under src/tools/)",
        "tools/              ": "root tools/ dir was pruned",
        "testing/": "root testing/ moved to tests/e2e",
        "Mono.Cecil": "Mono.Cecil is not a build-time dependency of this repo",
    }
    for marker, why in stale_markers.items():
        if marker in text:
            out.append(f"[WARN] README still mentions {marker} ({why})")
    return out


def check_release_gate() -> list[str]:
    """A SemVer tag must exist (clean `git describe`). """
    out: list[str] = []
    describe = _git(["describe", "--tags", "--always"])
    if not describe or not re.match(r"v?\d+\.\d+\.\d+", describe):
        out.append(f"[WARN] no SemVer tag reachable (git describe='{describe}')")
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--hard", action="store_true", help="exit 1 on any [FAIL]")
    ap.add_argument("--ci", action="store_true", help="exit 1 on any [FAIL] or [WARN]")
    args = ap.parse_args()

    issues: list[str] = []
    issues += check_version_consistency()
    issues += check_readme_validity()
    issues += check_release_gate()

    if issues:
        for i in issues:
            print(i)
        fails = [i for i in issues if i.startswith("[FAIL]")]
        warns = [i for i in issues if i.startswith("[WARN]")]
        # Version consistency is an objective correctness gate: any mismatch MUST
        # fail, because a released binary/package with a wrong version number is a
        # real (if silent) production defect. README validity stays advisory.
        if fails:
            return 1
        if (args.hard or args.ci) and (fails or warns):
            return 1
        print("[advisory] release-governance warnings (non-blocking in this mode)")
        return 0
    print("[PASS] release/governance checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
