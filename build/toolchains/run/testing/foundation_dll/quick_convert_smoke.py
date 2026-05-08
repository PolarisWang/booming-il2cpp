"""Quick convert-to-cpp smoke test across all families.
Verifies that the dead code cleanup didn't break codegen.
Skips Fact Static/Fact Runtime (pre-existing issues unrelated to recent changes)."""
from __future__ import annotations
import json, subprocess, sys, time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"

FAMILIES = [
    "array-indexing-copy", "buffer-memory", "collections-generic-core",
    "convert-char", "enum-parsing", "generic-nullable-value",
    "guid-random-hashcode", "math-numerics", "object-equality-identity",
    "primitive-numeric-conversions", "string-char-text-core",
    "time-date-time-timespan", "threading-monitor-interlocked",
    "attributes-custom-metadata", "boxing-unboxing-casts",
    "delegate-core-invocation", "exception-throw-diagnostics",
    "globalization-culture", "io-streams-basics",
    "reflection-member-basics", "runtime-compiler-services",
    "span-memory-buffers", "type-runtime-handles",
    "reflection-type", "reflection-assembly", "reflection-member-complete",
    "reflection-generics", "reflection-parameters", "reflection-module",
    "reflection-binding", "reflection-activation",
]

def main():
    passed = 0
    failed = 0
    results = []

    for family in FAMILIES:
        genuine_out = _VERIFICATION / family / "il2cpp_dist" / "genuine"
        genuine_out.mkdir(parents=True, exist_ok=True)

        # Find DLL
        dll_dir = _VERIFICATION / family / "il2cpp_dist" / "entrypoint" / "build-output"
        dlls = list(dll_dir.glob("*.dll")) if dll_dir.exists() else []
        if not dlls:
            # Maybe handwritten
            handwritten = _VERIFICATION / family / "handwritten"
            if handwritten.exists():
                dlls = list(handwritten.rglob("*.dll"))
        if not dlls:
            print(f"  [SKIP]  {family}: no DLL found")
            results.append((family, "SKIP", ""))
            continue

        dll_path = str(dlls[0])
        print(f"  [RUN]   {family}...", end=" ")

        # Clean previous output (skip directories like obj/)
        for f in genuine_out.glob("generated/*"):
            if f.is_file():
                f.unlink(missing_ok=True)

        cmd = [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "convert-to-cpp",
            "--assembly", dll_path,
            "--output", str(genuine_out),
        ]

        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
        except subprocess.TimeoutExpired:
            print("TIMEOUT")
            results.append((family, "TIMEOUT", ""))
            failed += 1
            continue

        if result.returncode != 0:
            # Extract last meaningful error
            lines = [l for l in result.stderr.splitlines() if l.strip()]
            err = lines[-1][:120] if lines else "unknown"
            print(f"FAILED: {err}")
            results.append((family, "FAILED", err))
            failed += 1
        else:
            cpp_path = genuine_out / "generated" / "native-aot.generated.cpp"
            size = cpp_path.stat().st_size if cpp_path.exists() else 0
            print(f"OK ({size} bytes)")
            results.append((family, "OK", f"{size} bytes"))
            passed += 1

    # Summary
    print(f"\n{'='*60}")
    print(f"Convert-to-cpp results: {passed} passed, {failed} failed, {len(FAMILIES)-passed-failed} skipped")
    print(f"{'='*60}")
    for family, status, detail in results:
        print(f"  {status:8s} {family}  {detail}")

    return 0 if failed == 0 else 1

if __name__ == "__main__":
    sys.exit(main())
