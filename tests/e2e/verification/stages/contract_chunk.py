"""Contract stage — run semantic contract tests against .NET 8 reference runtime.

Semantic contracts are standalone .NET 8 console applications (under
``tests/contracts/managed/``) that feed VALID inputs to API methods and assert
SEMANTIC results, unlike ATG-generated smoke tests which feed null/uninitialized
inputs and can only verify "does not crash".

This stage is per-chunk: only chunks that have a matching contract project under
``tests/contracts/managed/<chunk-slug>`` will run.  Contracts not found → skip.

Output is written to ``results/contract.json`` with passed/total counts, and
merged into ``results/fact.json`` under the ``contractPassed`` / ``contractTotal``
fields so down-stream tools (gate, honest_report) can see contract coverage.
"""

from __future__ import annotations

import json
import re
import subprocess
import time
from pathlib import Path

from verification.orchestration.context import ChunkContext, StageResult

_REPO_ROOT = Path(__file__).resolve().parents[4]
_CONTRACTS_ROOT = _REPO_ROOT / "tests" / "contracts" / "managed"

# Chunk slug -> contract project directory name.
# Explicit so contract projects keep readable names; add entries as contracts
# are written for more chunks.
_CONTRACT_DIRS: dict[str, str] = {
    "system": "corelib-system-contract",
}


def run_contract_chunk(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Run the chunk-specific contract if one exists, else skip.

    Matches slug ``slug`` -> ``tests/contracts/managed/corelib-<assembly>-<slug>-contract/``.
    For system.Private.CoreLib/system → ``corelib-system-contract/``.
    """
    start = time.perf_counter()

    # Derive contract project directory.  The directory name is an explicit
    # mapping rather than a mechanical slug transform, so contract projects can
    # be named for readability (e.g. corelib-system-contract) instead of the
    # full assembly name.
    contract_name = _CONTRACT_DIRS.get(ctx.slug)
    if contract_name is None:
        return StageResult(
            stage="contract", status="skipped",
            summary=f"no contract mapping for chunk '{ctx.slug}'",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    contract_dir = _CONTRACTS_ROOT / contract_name
    csproj = contract_dir / f"{contract_name}.csproj"

    if not csproj.exists():
        return StageResult(
            stage="contract", status="skipped",
            summary=f"no contract project: {csproj.name}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Run the contract as a standalone .NET 8 console app
    result_dir = ctx.chunk_dir / "results"
    result_dir.mkdir(parents=True, exist_ok=True)

    print(f"  [contract] running {contract_name}...")
    try:
        cp = subprocess.run(
            ["dotnet", "run", "--project", str(csproj)],
            capture_output=True, timeout=120,
            cwd=str(contract_dir),
        )
    except subprocess.TimeoutExpired:
        return StageResult(
            stage="contract", status="error",
            summary=f"contract {contract_name} timed out (120s)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    stdout_bytes = cp.stdout
    stderr_bytes = cp.stderr
    try:
        stdout = stdout_bytes.decode("utf-8", errors="replace")
    except UnicodeDecodeError:
        stdout = stdout_bytes.decode("gbk", errors="replace")
    try:
        stderr = stderr_bytes.decode("utf-8", errors="replace")
    except UnicodeDecodeError:
        stderr = stderr_bytes.decode("gbk", errors="replace")

    # Parse output to extract passed/failed
    passed, failed = 0, 0
    for line in stdout.splitlines():
        if " passed, " in line and " failed " in line:
            # Match "═══ N passed, M failed ═══"
            m = re.search(r"═══\s*(\d+)\s+passed,\s*(\d+)\s+failed\s+═══", line)
            if m:
                passed = int(m.group(1))
                failed = int(m.group(2))
                break

    # Write contract.json
    contract_data = {
        "contractPassed": passed,
        "contractTotal": passed + failed,
        "contractFailed": failed,
        "contractStdoutSnippet": stdout[:2000] if stderr or failed > 0 else "",
        "contractStderrSnippet": stderr[:2000] if stderr else "",
    }
    (result_dir / "contract.json").write_text(
        json.dumps(contract_data, indent=2), encoding="utf-8")

    # Merge contract counts into fact.json
    fact_path = result_dir / "fact.json"
    if fact_path.exists():
        try:
            fact = json.loads(fact_path.read_text(encoding="utf-8"))
            fact["contractPassed"] = passed
            fact["contractTotal"] = passed + failed
            fact["contractFailed"] = failed
            fact_path.write_text(json.dumps(fact, indent=2), encoding="utf-8")
        except (json.JSONDecodeError, OSError):
            print(f"  [contract] WARNING: could not read {fact_path} to merge contract data")

    if cp.returncode != 0 or failed > 0:
        status = "failed"
        summary = f"contract {contract_name}: {passed} passed, {failed} failed"
        errors = []
        if cp.returncode != 0:
            errors.append(f"exit code {cp.returncode}")
        if stderr:
            errors.append(stderr[:500])
        return StageResult(
            stage="contract", status=status,
            summary=summary,
            details=contract_data,
            errors=errors,
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    status = "passed"
    summary = f"contract {contract_name}: {passed}/{passed + failed} passed"
    print(f"  [contract] {summary}")
    return StageResult(
        stage="contract", status=status,
        summary=summary,
        details=contract_data,
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _slugify(assembly: str) -> str:
    """Normalize assembly name for directory naming."""
    return assembly.replace(".", "-").lower()