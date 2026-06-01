"""Auto-Bisect Tool — automatically locate the commit that introduced a performance regression.

Uses git bisect in an isolated worktree to binary-search commit history for the
first commit that degrades a specific benchmark subject beyond a threshold.

CLI:
    # List current benchmark status for a family
    python -m verification.analysis.auto_bisect status \\
        --assembly System.Private.CoreLib \\
        --family enum-parsing

    # Bisect a specific subject's regression
    python -m verification.analysis.auto_bisect bisect \\
        --assembly System.Private.CoreLib \\
        --family enum-parsing \\
        --subject "Enum::Format:System.String(System.Type,System.Object,System.String)" \\
        --good-commit abc1234 \\
        --bad-commit HEAD \\
        --max-degradation 1.20

Note: bisect runs the full pipeline for each evaluated commit (via git worktree).
      For N commits in the bisect range, up to log2(N) pipeline runs are needed.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path
from typing import Any


# ── Paths ─────────────────────────────────────────────────────────────────

_HERE = Path(__file__).resolve().parent  # verification/analysis/
_VERIFICATION_ROOT = _HERE.parent  # verification/
_TESTING_ROOT = _VERIFICATION_ROOT.parent  # testing/foundation-dll/
_PROJECT_ROOT = _TESTING_ROOT.parent  # booming-il2cpp/


# ── Git helpers ────────────────────────────────────────────────────────────

def _run_git(
    args: list[str],
    cwd: Path = _PROJECT_ROOT,
    capture: bool = True,
    timeout: int = 30,
) -> subprocess.CompletedProcess:
    """Run a git command."""
    return subprocess.run(
        ["git"] + args,
        capture_output=capture,
        text=True,
        timeout=timeout,
        cwd=str(cwd),
    )


def _ensure_clean_working_tree() -> None:
    """Raise RuntimeError if the working tree has uncommitted changes."""
    r = _run_git(["status", "--porcelain"])
    if r.returncode != 0:
        raise RuntimeError(f"git status failed: {r.stderr}")
    if r.stdout.strip():
        raise RuntimeError(
            "Working tree is not clean. Please commit or stash changes first.\n"
            f"Uncommitted files:\n{r.stdout}"
        )


def _resolve_commit(ref: str) -> str:
    """Resolve a git ref to a full SHA."""
    r = _run_git(["rev-parse", ref])
    if r.returncode != 0:
        raise ValueError(f"Cannot resolve ref: {ref}")
    return r.stdout.strip()


def _get_commit_info(commit: str) -> dict[str, str]:
    """Return author, date, and subject for a given commit."""
    r = _run_git(["log", "--format=%H|%an|%ai|%s", "-1", commit])
    if r.returncode != 0 or not r.stdout.strip():
        raise ValueError(f"Cannot resolve commit: {commit}")
    parts = r.stdout.strip().split("|", 3)
    return {
        "hash": parts[0],
        "author": parts[1] if len(parts) > 1 else "",
        "date": parts[2] if len(parts) > 2 else "",
        "subject": parts[3] if len(parts) > 3 else "",
    }


def _get_current_commit(cwd: Path = _PROJECT_ROOT) -> str:
    """Get the full SHA of HEAD in the specified working tree."""
    return _resolve_commit("HEAD")


# ── Worktree management ────────────────────────────────────────────────────

def _create_worktree(commit: str) -> tuple[Path, str]:
    """Create a git worktree at a specific commit.

    Returns (worktree_path, worktree_name).
    The caller MUST remove the worktree when done.
    """
    worktree_name = f"auto-bisect-{commit[:8]}-{int(time.time())}"
    worktree_path = _PROJECT_ROOT / ".claude" / "worktrees" / worktree_name
    worktree_path.parent.mkdir(parents=True, exist_ok=True)

    r = _run_git([
        "worktree", "add", "--detach",
        str(worktree_path), commit,
    ])
    if r.returncode != 0:
        raise RuntimeError(f"Failed to create worktree at {commit}: {r.stderr}")

    return worktree_path, worktree_name


def _remove_worktree(worktree_path: Path, worktree_name: str) -> None:
    """Remove a git worktree safely."""
    try:
        _run_git(["worktree", "remove", "--force", str(worktree_path)])
    except Exception as e:
        print(f"  [auto-bisect] Warning: failed to remove worktree: {e}", file=sys.stderr)
    try:
        import shutil
        if worktree_path.exists():
            shutil.rmtree(worktree_path)
    except Exception as e:
        print(f"  [auto-bisect] Warning: failed to clean worktree dir: {e}", file=sys.stderr)


# ── Benchmark data helpers ─────────────────────────────────────────────────

def _load_multirun_report(family_dir: Path) -> dict[str, Any] | None:
    """Load the multi-run-report.json for a family. Returns None on failure."""
    path = family_dir / "multi-run" / "multi-run-report.json"
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _resolve_family_dir(assembly: str, family: str) -> Path:
    """Resolve the family directory path."""
    return _TESTING_ROOT / assembly / family


def _find_method_entry(
    methods: list[dict[str, Any]],
    subject: str,
) -> dict[str, Any] | None:
    """Find a method entry by subject_id in the multi-run methods list.

    Accepts both full subject_id (e.g. "System.Private.CoreLib/System.Enum::...")
    and short name (e.g. "Enum::ToString:String").
    """
    for m in methods:
        sid = m.get("subject_id", "")
        if sid == subject:
            return m
    # Fallback: match on final "::" segment
    for m in methods:
        sid = m.get("subject_id", "")
        if sid.endswith("::" + subject):
            return m
    # Last-resort: substring match
    for m in methods:
        sid = m.get("subject_id", "")
        if subject in sid:
            return m
    return None


def _extract_ns_per_op(
    entry: dict[str, Any],
    runtime: str = "chaos-aot",
) -> float:
    """Extract mean_ns for a given runtime from a method entry."""
    samples = entry.get("samples", {})
    runtime_data = samples.get(runtime, {})
    if isinstance(runtime_data, dict):
        val = runtime_data.get("mean_ns", 0.0)
        if isinstance(val, (int, float)) and val > 0:
            return float(val)
    # Fallback: any available runtime
    for rt_name, rt_data in samples.items():
        if isinstance(rt_data, dict):
            val = rt_data.get("mean_ns", 0.0)
            if isinstance(val, (int, float)) and val > 0:
                return float(val)
    return 0.0


# ── Pipeline invocation ────────────────────────────────────────────────────

def _run_pipeline_in_worktree(
    worktree_root: Path,
    assembly: str,
    family: str,
) -> dict[str, Any]:
    """Run the verification pipeline inside a worktree and return the multi-run report.

    Skips all stages except benchmark using --skip and --incremental.
    Returns the multi-run report dict, or raises RuntimeError on failure.
    """
    cli_py = worktree_root / "testing" / "foundation-dll" / "verification" / "entry_points" / "cli.py"

    # Skip everything except benchmark
    skip_arg = "preflight,codegen,jit_codegen,managed_fact,managed_record,cross_verify,"
    skip_arg += "managed_patch_fact,fact,fact_jit,audit,asm_compare,microbench,"
    skip_arg += "hotupdate,hotupdate_aot_benchmark,hotupdate_jit_fact,"
    skip_arg += "hotupdate_jit_benchmark,multi_patch_hotupdate,patch_cross_verify,cleanup"

    cmd = [
        sys.executable, str(cli_py),
        family,
        "--assembly", assembly,
        "--skip", skip_arg,
        "--incremental",
    ]

    print(f"  [auto-bisect] Running pipeline: {' '.join(cmd)}")
    start = time.perf_counter()

    r = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        timeout=1800,  # 30 minutes per pipeline run
        cwd=str(worktree_root / "testing" / "foundation-dll"),
    )

    elapsed = time.perf_counter() - start
    print(f"  [auto-bisect] Pipeline finished in {elapsed:.0f}s (exit={r.returncode})")

    # Load the multi-run report (it may still exist even if pipeline exit != 0)
    family_dir = _resolve_family_dir(assembly, family)
    report = _load_multirun_report(family_dir)
    if report is None:
        stderr_snippet = (r.stderr or "")[:500]
        stdout_snippet = (r.stdout or "")[:500]
        print(f"  [auto-bisect] Pipeline stderr: {stderr_snippet}")
        print(f"  [auto-bisect] Pipeline stdout: {stdout_snippet}")
        raise RuntimeError(
            f"Pipeline completed but multi-run report not found at "
            f"{family_dir / 'multi-run' / 'multi-run-report.json'}"
        )
    return report


# ── status subcommand ──────────────────────────────────────────────────────

def cmd_status(args: argparse.Namespace) -> int:
    """Print current benchmark status for all methods in a family."""
    family_dir = _resolve_family_dir(args.assembly, args.family)
    if not family_dir.is_dir():
        print(f"ERROR: Family directory not found: {family_dir}", file=sys.stderr)
        return 1

    report = _load_multirun_report(family_dir)
    if report is None:
        print(f"No multi-run report found for {args.assembly}/{args.family}")
        print(f"  Expected at: {family_dir / 'multi-run' / 'multi-run-report.json'}")
        return 1

    methods = report.get("methods", [])
    if not methods:
        print(f"No methods found in report for {args.assembly}/{args.family}")
        return 0

    header = f"{'Idx':<6} {'ChaosAOT(ns)':<14} {'NET8(ns)':<14} {'SubjectID'}"
    print(f"\nBenchmark Status: {args.assembly}/{args.family}")
    print(f"Report timestamp: {report.get('timestamp', 'N/A')}")
    print("=" * len(header))
    print(header)
    print("-" * len(header))

    with_data = 0
    for m in methods:
        sid = m.get("subject_id", "")
        idx = m.get("method_index", -1)
        chaos_ns = _extract_ns_per_op(m, "chaos-aot")
        net8_ns = _extract_ns_per_op(m, "net8-jit")
        if chaos_ns > 0:
            with_data += 1
        chaos_str = f"{chaos_ns:.2f}" if chaos_ns > 0 else "N/A"
        net8_str = f"{net8_ns:.2f}" if net8_ns > 0 else "N/A"
        print(f"{idx:<6} {chaos_str:<14} {net8_str:<14} {sid}")

    if with_data:
        print(f"\n{with_data} methods with data")
    return 0


# ── _evaluate subcommand (called by git bisect run) ────────────────────────

def cmd_evaluate(args: argparse.Namespace) -> int:
    """Evaluate the current commit's benchmark performance.

    Called by `git bisect run`. This runs INSIDE the bisect worktree,
    so it uses the current working tree paths directly (no worktree creation).

    Returns:
      0 (good) — performance is within threshold
      1 (bad)  — regression detected
      125      — skip (unable to evaluate)
    """
    assembly = args.assembly
    family = args.family
    subject = args.subject
    baseline_ns = args.baseline_ns
    max_degradation = args.max_degradation

    # Determine the project root from the current working tree
    cwd = Path.cwd().resolve()
    # Walk up to find the project root (where .git exists)
    project_root = cwd
    while project_root.parent != project_root:
        if (project_root / ".git").exists() or (project_root / ".git").is_dir():
            break
        project_root = project_root.parent
    else:
        print("[bisect-eval] Could not locate project root", file=sys.stderr)
        return 125

    testing_root = project_root / "testing" / "foundation-dll"
    cli_py = testing_root / "verification" / "entry_points" / "cli.py"

    if not cli_py.exists():
        print(f"[bisect-eval] cli.py not found: {cli_py}", file=sys.stderr)
        return 125

    # ── Run pipeline (benchmark only) ──────────────────────────────────
    skip_arg = "preflight,codegen,jit_codegen,managed_fact,managed_record,cross_verify,"
    skip_arg += "managed_patch_fact,fact,fact_jit,audit,asm_compare,microbench,"
    skip_arg += "hotupdate,hotupdate_aot_benchmark,hotupdate_jit_fact,"
    skip_arg += "hotupdate_jit_benchmark,multi_patch_hotupdate,patch_cross_verify,cleanup"

    cmd = [
        sys.executable, str(cli_py),
        family,
        "--assembly", assembly,
        "--skip", skip_arg,
        "--incremental",
    ]

    print(f"[bisect-eval] Running: {' '.join(cmd)}")

    try:
        r = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=1800,
            cwd=str(testing_root),
        )
    except subprocess.TimeoutExpired:
        print("[bisect-eval] Pipeline timed out", file=sys.stderr)
        return 125
    except Exception as e:
        print(f"[bisect-eval] Pipeline error: {e}", file=sys.stderr)
        return 125

    # ── Load multi-run report ──────────────────────────────────────────
    report_path = testing_root / assembly / family / "multi-run" / "multi-run-report.json"
    if not report_path.exists():
        print(f"[bisect-eval] Report not found: {report_path}", file=sys.stderr)
        return 125

    try:
        report = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as e:
        print(f"[bisect-eval] Failed to read report: {e}", file=sys.stderr)
        return 125

    # ── Find subject and extract timing ────────────────────────────────
    methods = report.get("methods", [])
    entry = _find_method_entry(methods, subject)
    if entry is None:
        print(f"[bisect-eval] Subject '{subject}' not found")
        available = [m.get("subject_id", "?") for m in methods[:10]]
        print(f"[bisect-eval] Available (first 10): {available}")
        return 125

    current_ns = _extract_ns_per_op(entry, "chaos-aot")
    if current_ns <= 0:
        print(f"[bisect-eval] No valid timing for subject")
        return 125

    # ── Compare vs baseline ────────────────────────────────────────────
    ratio = current_ns / baseline_ns
    print(f"[bisect-eval] ns={current_ns:.2f} ratio={ratio:.4f}x baseline={baseline_ns:.2f}")

    if ratio > max_degradation:
        print(f"[bisect-eval] => BAD ({ratio:.2f}x > {max_degradation}x)")
        return 1
    else:
        print(f"[bisect-eval] => GOOD ({ratio:.2f}x <= {max_degradation}x)")
        return 0


# ── bisect subcommand ──────────────────────────────────────────────────────

def cmd_bisect(args: argparse.Namespace) -> int:
    """Run git bisect to find the commit that introduced a performance regression."""
    assembly = args.assembly
    family = args.family
    subject = args.subject
    good_commit = args.good_commit
    bad_commit = args.bad_commit
    max_degradation = args.max_degradation

    # Handle aliases
    if not good_commit and hasattr(args, 'good') and args.good:
        good_commit = args.good
    if not good_commit:
        print("ERROR: --good-commit or --good/-g is required", file=sys.stderr)
        return 1

    print(f"\n{'=' * 60}")
    print("Auto-Bisect: Performance Regression Locator")
    print(f"{'=' * 60}")
    print(f"  Assembly:     {assembly}")
    print(f"  Family:       {family}")
    print(f"  Subject:      {subject}")
    print(f"  Good commit:  {good_commit}")
    print(f"  Bad commit:   {bad_commit}")
    print(f"  Max degrad:   {max_degradation}x")
    print()

    # ── Step 0: Safety checks ──────────────────────────────────────────
    print("[1/6] Resolving commits and verifying clean tree...")
    try:
        _ensure_clean_working_tree()
    except RuntimeError as e:
        print(f"ERROR: {e}", file=sys.stderr)
        return 1

    resolved_good = _resolve_commit(good_commit)
    resolved_bad = _resolve_commit(bad_commit)
    good_info = _get_commit_info(resolved_good)
    bad_info = _get_commit_info(resolved_bad)

    print(f"  Good: {good_info['hash'][:12]} ({good_info['date'][:10]}) {good_info['subject']}")
    print(f"  Bad:  {bad_info['hash'][:12]} ({bad_info['date'][:10]}) {bad_info['subject']}")

    # ── Step 1: Record baseline at bad commit ──────────────────────────
    print("\n[2/6] Recording baseline at bad commit...")
    try:
        worktree_bad, name_bad = _create_worktree(resolved_bad)
        bad_report = _run_pipeline_in_worktree(worktree_bad, assembly, family)
        bad_method = _find_method_entry(bad_report.get("methods", []), subject)
        if bad_method is None:
            print(f"ERROR: Subject '{subject}' not found in bad commit's report", file=sys.stderr)
            return 1
        bad_ns = _extract_ns_per_op(bad_method, "chaos-aot")
        if bad_ns <= 0:
            print(f"ERROR: No valid timing for subject at bad commit", file=sys.stderr)
            return 1
        baseline_ns = bad_ns
        print(f"  Bad commit timing: {bad_ns:.2f}ns")
    finally:
        _remove_worktree(worktree_bad, name_bad)

    # ── Step 2: Verify good commit is faster ───────────────────────────
    print("\n[3/6] Verifying good commit is faster...")
    try:
        worktree_good, name_good = _create_worktree(resolved_good)
        good_report = _run_pipeline_in_worktree(worktree_good, assembly, family)
        good_method = _find_method_entry(good_report.get("methods", []), subject)
        if good_method is None:
            print(f"WARNING: Subject '{subject}' not found in good commit's report")
            print("  Continuing anyway — subject may have been introduced later.")
        else:
            good_ns = _extract_ns_per_op(good_method, "chaos-aot")
            if good_ns > 0:
                print(f"  Good commit timing: {good_ns:.2f}ns")
                good_ratio = bad_ns / good_ns
                print(f"  Bad vs Good ratio: {good_ratio:.4f}x")
                if good_ratio <= 1.0:
                    print("  WARNING: Good commit is NOT faster than bad commit!")
                    print(f"  The regression may not be in the range [{resolved_good[:12]}, {resolved_bad[:12]}]")
            else:
                print(f"  No valid timing at good commit (ns={good_ns})")
    finally:
        _remove_worktree(worktree_good, name_good)

    # ── Step 3: Count commits in range ─────────────────────────────────
    print("\n[4/6] Counting commits in bisect range...")
    r = _run_git(["rev-list", "--count", f"{resolved_good}..{resolved_bad}"])
    commit_count = int(r.stdout.strip())
    estimate = max(1, commit_count.bit_length())  # ceil(log2(N))
    print(f"  {commit_count} commits in range (est. {estimate} pipeline evaluations)")

    # ── Step 4: Run git bisect in isolated worktree ────────────────────
    print(f"\n[5/6] Running git bisect (up to {estimate} evaluations)...")
    print(f"  {'=' * 50}")

    try:
        bisect_wt, bisect_name = _create_worktree("HEAD")

        # Generate an evaluate wrapper script inside the worktree
        wrapper = _make_evaluate_wrapper(
            resolved_bad, resolved_good,
            assembly, family, subject, baseline_ns, max_degradation,
            bisect_wt,
        )
        wrapper_path = bisect_wt / ".claude" / "worktrees" / f"bisect-run-{int(time.time())}.py"
        wrapper_path.parent.mkdir(parents=True, exist_ok=True)
        wrapper_path.write_text(wrapper, encoding="utf-8")

        # Run git bisect inside the worktree
        bisect_stdout = ""
        for cmd in [
            ["git", "bisect", "start", resolved_bad, resolved_good],
            ["git", "bisect", "run", sys.executable, str(wrapper_path)],
        ]:
            r = subprocess.run(
                cmd,
                capture_output=True, text=True,
                timeout=7200,  # 2 hours max
                cwd=str(bisect_wt),
            )
            print(f"  $ {' '.join(cmd)}")
            if r.stdout:
                for line in r.stdout.splitlines():
                    print(f"    {line}")
            bisect_stdout = r.stdout or ""

        # Clean up bisect state
        _run_git(["bisect", "reset"], cwd=bisect_wt)

    except subprocess.TimeoutExpired:
        print("\nERROR: Bisect timed out after 2 hours", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"\nERROR: Bisect failed: {e}", file=sys.stderr)
        return 1
    finally:
        _remove_worktree(bisect_wt, bisect_name)

    # ── Step 5: Report results ─────────────────────────────────────────
    print(f"\n[6/6] Results")
    print(f"{'=' * 60}")

    first_bad = _extract_first_bad_commit(bisect_stdout)
    if first_bad:
        info = _get_commit_info(first_bad)
        print(f"\nFirst bad commit found:")
        print(f"  Hash:    {info['hash']}")
        print(f"  Author:  {info['author']}")
        print(f"  Date:    {info['date']}")
        print(f"  Subject: {info['subject']}")
        print(f"\nBaseline: {baseline_ns:.2f}ns (bad) vs good commit")
        return 0
    else:
        print(f"\nNo regression found in range [{resolved_good[:12]}, {resolved_bad[:12]}]")
        print("All commits performed within threshold.")
        return 1


def _extract_first_bad_commit(git_output: str) -> str | None:
    """Extract the first bad commit SHA from git bisect run output."""
    for line in git_output.splitlines():
        line = line.strip()
        if "is the first bad commit" in line:
            parts = line.split()
            if parts:
                return parts[0]
    return None


def _make_evaluate_wrapper(
    resolved_bad: str,
    resolved_good: str,
    assembly: str,
    family: str,
    subject: str,
    baseline_ns: float,
    max_degradation: float,
    project_root: Path,
) -> str:
    """Generate a self-contained Python wrapper for `git bisect run`.

    The wrapper sets up the Python path and delegates to auto_bisect's
    `_evaluate` subcommand. It is placed inside the bisect worktree so that
    `git bisect run` can invoke it as it checks out each commit.
    """
    # Escape backslashes for Windows paths
    project_root_str = str(project_root).replace("\\", "\\\\")

    return f'''"""Auto-bisect evaluate wrapper — generated by auto_bisect.py.

Called by `git bisect run` inside the bisect worktree. Delegates to
auto_bisect._evaluate after setting up the Python path.
"""
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(r"{project_root_str}")
TESTING_ROOT = PROJECT_ROOT / "testing" / "foundation-dll"

# Ensure the verification package is importable
sys.path.insert(0, str(TESTING_ROOT))

# Verify the module is accessible
try:
    from verification.analysis.auto_bisect import cmd_evaluate
except ImportError as e:
    print(f"[bisect-wrapper] Import error: {{e}}", file=sys.stderr)
    # Fallback: run via subprocess
    cmd = [
        sys.executable, "-m", "verification.analysis.auto_bisect",
        "_evaluate",
        "--assembly", "{assembly}",
        "--family", "{family}",
        "--subject", "{subject}",
        "--baseline-ns", str({baseline_ns}),
        "--max-degradation", str({max_degradation}),
    ]
    r = subprocess.run(
        cmd,
        cwd=str(TESTING_ROOT),
    )
    sys.exit(r.returncode)

# Dummy namespace for cmd_evaluate
class Args:
    assembly = "{assembly}"
    family = "{family}"
    subject = "{subject}"
    baseline_ns = {baseline_ns}
    max_degradation = {max_degradation}

sys.exit(cmd_evaluate(Args()))
'''


# ── CLI ────────────────────────────────────────────────────────────────────

def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Auto-Bisect Tool — locate the commit that introduced a benchmark regression",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )

    subparsers = parser.add_subparsers(dest="command", required=True)

    # ── status ─────────────────────────────────────────────────────────
    sp = subparsers.add_parser(
        "status",
        help="Show current benchmark status for all methods in a family",
        description="Display per-method benchmark timing from the latest multi-run report.",
    )
    sp.add_argument("--assembly", "-a", default="System.Private.CoreLib")
    sp.add_argument("--family", "-f", required=True)

    # ── bisect ─────────────────────────────────────────────────────────
    sp = subparsers.add_parser(
        "bisect",
        help="Bisect to find the commit that introduced a performance regression",
        description="Binary-search commit history to find the first commit degrading a subject.",
    )
    sp.add_argument("--assembly", "-a", default="System.Private.CoreLib")
    sp.add_argument("--family", "-f", required=True)
    sp.add_argument("--subject", "-s", required=True,
                    help="Benchmark subject name")
    sp.add_argument("--good-commit", "--good", "-g", default=None,
                    help="Commit known to be fast (the good baseline)")
    sp.add_argument("--bad-commit", "--bad", "-b", default="HEAD",
                    help="Commit that has the regression (default: HEAD)")
    sp.add_argument("--max-degradation", "-m", type=float, default=1.20,
                    help="Max allowed degradation ratio (default: 1.20 = 20%%)")

    # ── _evaluate (internal) ───────────────────────────────────────────
    sp = subparsers.add_parser(
        "_evaluate",
        help="[internal] Evaluate current commit's benchmark performance",
        description="Internal subcommand used by git bisect run. "
                    "Returns exit code 0 (good), 1 (bad), or 125 (skip).",
    )
    sp.add_argument("--assembly", "-a", default="System.Private.CoreLib")
    sp.add_argument("--family", "-f", required=True)
    sp.add_argument("--subject", "-s", required=True)
    sp.add_argument("--baseline-ns", required=True, type=float,
                    help="Baseline ns/op to compare against")
    sp.add_argument("--max-degradation", type=float, default=1.20)

    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    if args.command == "status":
        return cmd_status(args)
    elif args.command == "bisect":
        return cmd_bisect(args)
    elif args.command == "_evaluate":
        return cmd_evaluate(args)
    else:
        parser.print_help()
        return 0


if __name__ == "__main__":
    sys.exit(main())
